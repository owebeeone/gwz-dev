# R2-D Phase 4 Step 4.3 — coexistence closure, the dirent-barrier residual, and the Phase 4 disposition batch: single-axis State review

Date: 2026-08-23. **Single-axis State-lead** (`GwzM5-8R2DInterfaceFreeze.md` §9
:1556-1558: "Everything else is **single-axis with automatic escalation on any
P0/P1/P2**: … and Phase 4 steps 4.1-4.3 **including the Phase 4 settle**. …
P3 findings file and continue; they do not become packages and do not enlarge
R2-D"). Round 1 of a two-round cap. Peer-blind: the implementer's report was
read as *claims to verify*, never as evidence; every count below was recounted
against the live tree and every affordable gate re-run by this review.

**Escalation IS triggered** — one P2 finding against the object ([P2-1]).
Recorded here per the brief; **routing is the lane owner's**. [P2-1] is not a
correctness defect: it is an accuracy-and-routing finding on a measurement this
step publishes and hands to Step 5.1. Seven P3 findings file and continue.

Object: the uncommitted Step 4.3 edits over gwz-core base `51a9cba` ("Land R2-D
Step 4.2: retire the legacy Windows durability anchor machinery (review GO round
2)"). Dirty set confirmed by `git -C gwz-core status --short`: **exactly 10
modified files, no adds, no deletes**, `+250 / −34` = **net +216** (`git diff
--numstat`), against the plan's `<300 LOC` for this step. The step's principal
deliverable, `dev-docs/GwzM5-8R2DPhase4Closure.md` (445 lines), is untracked in
the **outer** `gwz-dev` repo, not in `gwz-core` — correct for this program's
doc convention, and noted because the two repos' dirty sets are separate.
(`dev-docs/GwxMergeFindingsDraft.md` is also untracked in `gwz-dev`; unrelated
to this step, not reviewed.)

| File | + / − | Item |
| --- | --- | --- |
| `protocol/authority_record.rs` | 2 / 0 | settle 8 |
| `protocol/authority_record/owner.rs` | 19 / 1 | settle 8 |
| `protocol/authority_record/owner/host.rs` | 25 / 1 | settle 8 |
| `capability/pre_catalog/provider/authority_record_binding.rs` | 5 / 0 | settle 8 |
| `mod.rs` | 15 / 23 | settle 7 |
| `catalog.rs` | 20 / 0 | settle 7 |
| `coordinator/mod.rs` | 10 / 0 | settle 7 |
| `interface_tests/durable_records.rs` | 85 / 5 | settle 8 evidence |
| `coordinator/tests_execution.rs` | 26 / 4 | 3.3 P3s + settle 8 isolation |
| `platform/anchor/tests.rs` | 43 / 0 | 4.2 [P3-6] |

---

## 1. Verdict

**GO**, with escalation recorded and eight findings, none blocking the landing.

Every load-bearing claim in the two halves plan §4 Step 4.3 names verifies
against the tree. The (a) decision packet's three new facts are true; §2.7 is
substantively fit to lift and needs one identifier corrected first. The (b)
dirent-barrier closure is verified complete — six of six Ready owner edges, the
create edge correctly exempt on both arms, keyed, and driven by exactly the
mechanism its own review prescribed. Settle item 8 lands the gate at the seam
with the paired consumer obligation and two genuinely-isolated smuggles. Settle
item 7's narrowing is correct and proved by a warning-clean clippy from a clean
target. All executable gates are green.

The P2 is on the *measurement* item 7 publishes, not on the narrowing itself.

---

## 2. (a) The coexistence acceptance criterion — verification of the packet

Per the lane-owner ruling carried into this review, (a) is verified, not
re-decided. Three verification duties, all discharged.

### 2.1 The packet's load-bearing facts

**Windows permanence — VERIFIED, and it is the strongest fact in the packet.**
The E10/E14 activation annotation reads, verbatim: only the checked-artifact
private area may retain the round-trip anchor, because "the anchor is permanent
by design and `finish()` never removes it" (`GwzM5-8R2DInterfaceFreeze.md`
:1020-1023). The code carries the same fact in both directions: off Windows
`platform::prepare_private` is `Ok(())` and plants nothing (`platform.rs:345-352`),
while the Windows arm delegates to `anchor::prepare` (`platform.rs:510-517`,
via `residue.rs:102`, the family's only caller); and the family's own fixture
comment records the consequence independently — the staging scan is
suffix-scoped rather than prefix-scoped precisely because "`prepare_private`
plants and `finish()` never removes" the anchor, "the Windows probe caught
exactly that" (`tests/leaf_publication.rs:80-86`). §2.3's conclusion — a Windows
workspace that has ever written a checked artifact can never host an observable
catalog at that path — follows and is not overstated.

**`MAX_ROOT_ENTRIES = 74` supersession — VERIFIED, framing honest in direction,
imprecise in one clause (see [P3-5]).** `protocol/bounds.rs:21-23` defines
`MAX_ROOT_ENTRIES = MAX_ACTIVE_ACTION_DIRS + MAX_INFRASTRUCTURE_ENTRIES`, with
`const _: () = assert!(MAX_ROOT_ENTRIES == 74)` at `:71` and the interface pin
at `interface_tests/bounds_slots.rs:8`. Phase 1 did do it: the observer's
`MAX_INTERIOR_ENTRIES` is now literally `= MAX_ROOT_ENTRIES`
(`provider/interior.rs:40`), landed at `5a7ff0f` ("Park R2-D Phase 1 Steps
1.1+1.2 pre-settle"), and its doc comment quotes freeze §4.4 Class 2 fact 3 and
says both the cap and the grammar move. So the packet's supersession claim is
true and correctly attributed.

**`recover_or_create` has no production caller at `51a9cba` — VERIFIED.**
`grep -rn recover_or_create src/`: one definition (`catalog/bootstrap.rs:213`),
one sealed re-export (`catalog.rs:36`), one comment (`interior.rs:194`), and
**every** remaining hit is `#[cfg(test)]` — `catalog/bootstrap/tests.rs` (×11)
and `bootstrap/managed/tests_provider.rs` (×2). Nothing in `gwz-cli` or
`gwz-py`. Plan §5 item 2 forbids giving it one in R2-D; the gate is fail-closed
in code, as the record claims.

**MAX_PATH numbers — VERIFIED by construction from the name builders**
(`authority.rs:226-272`), not by trusting the table:
`ca1-{64}-{64}-{32}.source` = 4+64+1+64+1+32+7 = **173**; `.goal` = **171**;
`.authority` = **143**; Step 4.2's staging `.ca1-{64}-{64}-authority.scratch` =
5+64+1+64+10+8 = **152**; `-goal.scratch` = **147**. The pre-4.2 nonce shape
was `.ca1-scratch-{kind}-{hex32}` (`51a9cba~1:authority.rs:242-247`) = **55**
for `kind="authority"`, matching the table's row. Anchor prefixes confirmed at
`platform/anchor.rs:76/:82/:100`. So Step 4.2 did **not** widen the exposure:
173 remains the longest and 152 is below it. Checkpoint :1022-1024 is quoted
accurately.

Grammar A itself verifies too: the classifier admits exactly the ten canonical
`InfrastructureSlotV1::ALL` names and `ActiveAction` rows and refuses everything
else with `ambiguous("catalog interior", "catalog directory contains an unowned
child")`, and `retain_completed_catalog` rests on that observation
(`provider/completed.rs:52-72`). **But the function is `exact_row`, not
`exact_slot`** — see [P3-1].

### 2.2 Is §2.7 fit to lift verbatim?

**Substantively yes; textually not until [P3-1] is fixed.** Measured against
the three criteria:

- *Self-contained* — yes. A reader needs no other document: the record states
  the criterion's own alternative, declines it, gives the three facts, names the
  executing package, states the gate, and states the MAX_PATH consequence.
- *House form* — yes. Bold headline with disposition, date, actor and occasion
  ("CLOSED by decision (2026-08-23, lane owner, at the Step 4.3 landing)"),
  matching the form of §3's and §6.3's records and of the lifted blocks at
  Steps 4.1/4.2.
- *States the decision + the R2-F execution pin + what Option 1 would have
  required* — yes on all three. The pin quotes freeze §5 decision 2 accurately
  ("the move itself executes on the R2-F-candidate relocation package, on the
  same P1 family, not inside R2-D", freeze :1383-1386) and cites plan §5 item 7
  correctly. Option 1's cost is stated at the level of consequence (widening a
  frozen closed grammar; an open-ended family against a 74-entry budget;
  reconciling admission's `extra_children: 0`), which is the level a decision
  record needs; §2.8 carries the concrete obligation list if the fallback is
  ever ruled.

One defect blocks a verbatim lift: the record names `exact_slot`, which does not
exist ([P3-1]). Fix that one token in §2.7 (and §2.2/§2.4/§2.8) and the block is
liftable as written.

### 2.3 The A1-gating linkage

**Stated correctly.** The criterion requires the decision be recorded "before
any production catalog activation" (plan §4 Step 4.3(a)); plan §5 item 2 makes
that activation an R2-E/A1-adjacent decision *behind* this criterion and forbids
a production caller in R2-D. This record is filed at Step 4.3, ahead of any
activation, and therefore satisfies the acceptance criterion as written. The
record does not merely assert the gate, it points at the code fact that enforces
it — which I re-verified above. Audit P3-3 (`GwzM5-8R2C2PublicationAudit.md:80-89`)
is discharged by it: the audit's disposition was "record as an explicit R2-D
acceptance criterion", and its "ten-slot grammar" phrasing is superseded by 74
exactly as the packet says.

---

## 3. (b) The dirent-barrier closure — verified from the tree

I recounted the whole evidence table rather than reading it. **Every row holds,
and no Ready owner edge is uncovered.**

| Claim | What I found |
| --- | --- |
| Prologue exists | `capability/pre_catalog.rs:276-279` — `revalidate_observation()?` then `provider::finish_ready_edge_root_barrier(&self._retained_root)` |
| Idempotent, root-scoped | `provider/mutation.rs:274-283` → `finish_private_parent_edge(retained.root().handle())` |
| **All six** Ready owner edges, exactly once each | call sites at `pre_catalog.rs:286, 329, 359, 373, 387, 401`, one inside each of `execute_owner_scratch` (:281), `publish_active` (:324), `prepare_or_rewrite_staging` (:354), `publish_final` (:368), `retire_active` (:382), `complete` (:396). Six functions, six calls, no function with two |
| No seventh edge | the owner dispatch (`catalog/bootstrap.rs:231-291`) has exactly seven arms: the six above plus `Ambiguous`, which is a typed error that mutates nothing. **Nothing is uncovered** |
| `create_and_retry` correctly zero | `pre_catalog.rs:542-556` calls `create_git_private_parent`, which issues `finish_private_parent_edge(parent)` on **both** arms — the `Ok(())` create arm (`mutation.rs:87`) and the `AlreadyExists` re-entry arm (`mutation.rs:93`, with the State-2 [P3-1] rationale in-line). Same handle the prologue uses |
| Preflight read-only | `finish_ready_edge_root_barrier` has exactly one call site outside `mutation.rs` — the prologue. `preflight_catalog_target` (`pre_catalog.rs:87-107`) only inspects and issues permits |
| Keyed | `CatalogBootstrapReadyEdgeRootFlush => "catalog_bootstrap.ready_edge_root_flush"` at `fault_v1.rs:55`, announced *after* the barrier at `mutation.rs:278-281`, and registered at `interface_tests/fault_expected_keys.rs:28` |
| Regression test mechanism | `catalog/bootstrap/tests.rs:454-490`: panic armed at `Fault::CatalogBootstrapScratchWrite`, the interrupt asserted reached; then a **non-panicking** callback armed at `Fault::CatalogBootstrapReadyEdgeRootFlush`, the resume drive run to `Ok`, and the flag asserted true. That is exactly the mechanism `DirentBarrier-ReviewState.md:183-188` prescribed, including its "non-panicking callback … assert the slot is consumed" wording |
| Landed at `9939b02` | confirmed: `git log -S finish_ready_edge_root_barrier` returns that commit and no other |

**§3's closure record is fit to lift** — accurate, self-contained, house form,
and it resolves plan §7.5's conditional to the first branch on evidence rather
than assertion.

**The R2-F power-loss companion item survives.** It is tracked independently at
`CurrentProgramCheckpoint.md:1106-1112` ("routes to the next durability package
or a §6-style erratum alongside the R2-F power-loss item"); §3 discharges only
the residual half and says so. *Landing note (not a finding):* that checkpoint
entry still reads as open for the residual; the lane owner may want to mark it
CLOSED against §3's record in the same edit that files the pins.

---

## 4. Settle item 8 — seam-level action binding

Verified end to end; **the gate is real and unbypassable.**

- `AuthorityObservationFactsV1` carries `action_digest` (`owner.rs:23-24`), set
  in both the production and synthetic constructors.
- `AuthorityFactsIssuerV1::issue` takes it as its first argument
  (`owner/host.rs`).
- `CheckedAuthorityObservationOwnerV1::observe` refuses on disagreement with the
  **resident** reservation (`owner.rs:89-93`), before the
  `request_owner_binding` check, with its own typed detail.
- The production transaction supplies `self.proof.action()`
  (`authority_record_binding.rs:381-390`).
- **Unbypassable:** `CheckedAuthorityObservationV1::owner_issue` is module-private
  (`authority_record.rs:93`) and has exactly one caller in the tree —
  `owner.rs:99`, inside `observe`, downstream of the new check. `owner_issue`
  still copies `action_digest` from the reservation, which is now sound rather
  than merely convenient: after the gate the two are provably equal.

**The provenance claim behind the consumer obligation is load-bearing and true.**
`observe_streamed_payloads` builds *both* slot names from the digest it is given
— `slot(action, BaseActionSlotV1::SourcePayload)` and
`slot(action, …::GoalPayload)` (`authority_record_binding.rs:209-226`) — and
stores that same `action` into the proof it mints (`:227-230`), and
`StreamedPayloadProofV1` is mintable by nothing else (`:130-146`). So the bytes a
proof carries really are the bytes of that digest's rows, and "pass the digest
you derived your reads from" is the exact obligation. Stating it on `issue`'s
signature (`owner/host.rs:53-66`) rather than only in a record is the right
placement and matches the checkpoint's settle disposition (8) verbatim
("it rests on slot-name derivation, so it lands PAIRED with an explicit consumer
obligation stated at the seam", `CurrentProgramCheckpoint.md:442-446`).

**The seam-level negative row does what it claims**
(`durable_records.rs:230-282`): two reservations, one `request_owner_binding`
(asserted equal in-test, with the reason spelled out), the observation issued
against `target` while carrying `other`'s digest, refused **at the seam** with
`matches!` pinning the seam's own detail string, and the honest pairing then
issuing — so it is a pairing check, not a blanket refusal. It needs no help from
the 3.3 gate.

**The two-smuggle isolation claim is TRUE, and I checked both directions.**

1. *Seam row* — smuggles the **digest**; provenance is honest. Refused at
   `owner.rs:89`.
2. *3.3 consumer row* (`tests_execution.rs:380-420`) — now passes the **target's**
   digest (`let action_digest = reservation.action_digest();` at `:165`), which
   the seam accepts, over the **other** action's `directory_identity()`, which
   the seam never inspects. Refused only by the consumer gate, and the row now
   pins that gate's own detail — verified byte-identical to the production
   string at `coordinator/execution.rs:263`.

I also checked the *third* row on this path for silent rot, which the new
earlier gate could have caused:
`authority_owner_rejects_two_request_substitution_in_both_directions` still
isolates the binding gate, because it constructs `second` with
`first.action_digest()` and asserts them equal (`durable_records.rs:284-296`) —
so the digest gate provably cannot fire there. Structural rather than
detail-pinned, but airtight. No rot.

---

## 5. Settle item 7 — the narrowing

**The narrowing itself is correct, and I have decisive evidence for it.** The
table's structural claims are exactly what a warning-clean build proves, and
`CLIPPY_CONF_DIR="$PWD" cargo clippy -p gwz-core --all-targets --all-features --
-D warnings` **from a cleaned target** is silent (§8). With the `admission` and
`catalog_names` allows deleted outright, and the `catalog` / `coordinator`
allows moved inward to `bootstrap`/`classifier`/`enumeration` and `identity`
respectively, a single surviving dead item in `admission`, `catalog_names`,
`catalog/scratch.rs` or `coordinator/schedule.rs` would have failed that run. It
did not. `admission` = 0 and `catalog_names` = 0 are therefore established, and
"`scratch` needs none" / "`schedule` needs none" with them.

Counts confirmed from the diff: **eleven** module-level allows in `mod.rs`
before, **four** deleted-or-moved, **seven** kept, and exactly **six** of the
kept ones previously carried the identical stale string "R1 freezes interfaces
before R2 consumer conversion" — all six now name R2-E and plan §5 item 1.
`coordinator/mod.rs`'s inward allow is placed correctly: `mod execution` already
carried its own from Step 3.3 (`coordinator/mod.rs:10-14`) and `mod schedule`
carries none.

**Three corrected reasons spot-checked for truth:**

1. `catalog/bootstrap` — "`recover_or_create` gains its first production caller
   in R2-E, behind the Phase 4.3 coexistence criterion (plan §5 item 2)".
   **True**: no production caller (§2.1) and plan §5 item 2 says precisely that.
2. `entry` — rewritten from "production consumer activation" to name R2-E.
   **True**: the old text named a gate that does not exist in the plan's
   vocabulary; R2-E is the gate plan §5 item 1 actually sets.
3. `coordinator/identity` — "consumed by `execution` and by the interface-test
   contract pins; R2-E converts the merge consumer that reads the rest".
   **True in substance**: `identity` reaches the rest of the subsystem only
   through `pub use identity::*` at `coordinator/mod.rs:28`, and the live
   consumers are `execution` plus `interface_tests/coordinator_contract.rs` /
   `coordinator_remediation.rs`.

   *Nit, not filed separately:* the parallel `catalog/classifier` reason says
   "consumed only by the owner above", but the direct consumer of
   `classify_catalog_attempt` is `capability/pre_catalog.rs`'s
   `classify_observed`, which the owner drives. Defensible paraphrase; worth a
   word if the reasons are ever revisited.

**The honest no-orphans finding is honest.** "The narrowing found no orphan to
delete, it found two stale covers and two over-broad ones" is exactly what the
tree shows, and reporting it as a *different* outcome from the Step 3.2 review's
two findings rather than dressing it up is the right call.

**The 481 figure is where this step earns its P2** — see [P2-1].

---

## 6. The remaining routed items

**6.1 — 3.3's two round-2 P3s: both fixed, both verified.** The cross-action
row's detail assertion is byte-identical to the production string
(`execution.rs:263`), and the stranded first line of `leaf_digests`' doc comment
("A leaf request of the given operation, over a fixed workspace-relative path.")
is deleted — it did belong to `leaf_request`. The closure doc's observation that
this pin "mattered immediately" is correct and is the reason the isolation check
in §4 above was worth doing.

**6.2 — [P3-6] racing-drive row: it drives real contention, and it does not
execute here.**

*Real contention, not a serialized imitation* — **confirmed at the mechanism.**
`survey` computes `retirement_ordinal = smallest_free_ordinal(&mut retired)` at
`platform/anchor.rs:311` and carries it into `NeedsRetireAlias`; the edge renders
`retired_name(retirement_ordinal)` at `:147`, fires
`BeforeAnchorAliasRetirement` at `:148-152`, and only then executes the
no-replace `publish` at `:153`. The test's hook plants `retired_name(0)` exactly
in that gap, so the drive really is holding a stale ordinal against an occupied
destination — not a pre-planted state the survey would have seen. Deterministic
injection at the true interleaving point is also the *better* construction than
a thread race for a regression row. The assertions are right too: the loser
leaves `{0}` (publishes nothing), the grammar stays closed, and the next drive
re-surveys to `{0,1}`.

*But it did not execute.* The row opens with
`let Some((anchor, _)) = strand_alias(&root.0) else { … return; }`, and
`strand_alias` returns `None` whenever hard links do not share a durable
identity (`platform/anchor/tests.rs:258-276`) — which is macOS, by the platform
fact this program pinned by measurement and guards with a dedicated test whose
message is "the retirement rows must stop skipping here"
(`:293-318`). On this host the new row is a no-op. See [P3-2].

**6.3 — [P3-7] `Invalid` derivation record: substantively right, enumeration
short by one.** The thesis verifies: every `Invalid` arm's trigger is foreign
contamination of the reserved grammar, the protocol renames and never links
(`publish` only), writes anchor-prefixed names only through `anchor_name` and
retired names only through `retired_name`, the refusal is typed and non-silent
(`anchor.rs:173-176`, "private durability anchor is missing or ambiguous while
family state exists"), and the [P2-2] contrast is drawn correctly. The
no-mutation citation is real (`tests.rs:515-537`) though its listing-unchanged
assertion lives in the re-homed branch, i.e. the macOS branch. The arm count is
wrong — see [P3-3].

---

## 7. Publication seam and pins

**Both canonical counts unchanged, verified by the checker's own pass rather
than by grep.** `CATALOG_PUBLICATION_CALL_COUNTS` is 6 files summing to
1+5+2+1+2+2 = **13** (`check_checked_artifact_boundaries.py:683-703`), and the
checker's publication rule (`:886-900`) recounts every entry against the masked
source and would have appended "catalog publication seam changed" on any drift.
It did not fire. The permit literal is intact:
`assert_eq!(callers.matches("publish_verified_no_replace(").count(), 13)`
(`interface_tests/capability_permit.rs:139`), and its test passes inside the
green suite.

*Note on the implementer's unreconciled "21":* the lane owner's brief settles it
by naming the canonical figures (6 files / 13 calls, permit 13), both of which I
verified. The `21` in the Step 3.3 commit message reproduces against nothing in
today's tree; recording it here so it is not chased again.

**Exactly four pin moves, each attributable, none unexpected.** The checker
reports:

```
checked-artifact boundary: failed
- protected source allowlist changed: checked_artifact/mod.rs
- protected source tree changed: checked_artifact/capability/pre_catalog.rs
- protected source tree changed: checked_artifact/catalog.rs
- protected source tree changed: checked_artifact/platform.rs
```

I enumerated the pin inventory (17 flat, 7 tree) and cross-checked it against
the dirty set: `mod.rs` is the only dirty file under a flat pin;
`authority_record_binding.rs` sits under the `pre_catalog` tree pin, `catalog.rs`
under the `catalog` tree pin, `platform/anchor/tests.rs` under the `platform`
tree pin; and the remaining six dirty files (`protocol/authority_record*.rs` ×3,
`coordinator/*` ×2, `interface_tests/durable_records.rs`) are under **no** pin.
So four is complete and correct, and there is no fifth. The refresh is the lane
owner's landing edit, as at Steps 3.3/4.1/4.2. The step's own record does not
carry the obligation — see [P3-4].

---

## 8. Gates re-run by this review

All from `gwz-core/`, foreground, `CARGO_TARGET_DIR=…/scratchpad/p41-target`, on
this macOS host, against the live dirty tree.

```text
git -C gwz-core status --short        → 10 modified, 0 added, 0 deleted
git diff --numstat                    → +250 / −34 (net +216; budget <300) ✓
cargo test --lib -p gwz-core checked_artifact::
                                      → ok. 398 passed; 0 failed; 0 ignored;
                                        1170 filtered out; 36.19s          ✓ (claim: 398/0)
cargo test --lib -p gwz-core          → ok. 1567 passed; 0 failed; 1 ignored;
                                        0 filtered out; 694.98s            ✓ (claim: 1567/0/1)
cargo fmt --all -- --check            → clean, exit 0                      ✓
cargo clean -p gwz-core               → Removed 583 files, 983.6MiB
CLIPPY_CONF_DIR=$PWD cargo clippy -p gwz-core --all-targets --all-features -- -D warnings
                                      → Finished in 11.50s, zero diagnostics ✓ (from clean)
python3.13 scripts/checks/check_checked_artifact_boundaries.py
                                      → 4 findings, all pin moves, all attributed (§7)
CATALOG_PUBLICATION_CALL_COUNTS       → 6 files / 13 calls, checker's own rule silent ✓
capability_permit.rs literal          → 13 ✓
recover_or_create production callers  → none (definition + sealed re-export + comment) ✓
ready_edge_prologue per execute_owner_* → 6 × 1; create_and_retry × 0 ✓
RUSTFLAGS="--force-warn dead_code" cargo check --all-targets --all-features
                                      → 1657 unique dead-item spans / 85 files
                                        in checked_artifact (see [P2-1])
```

**L2-04 retained-reader harness: accepted without re-running**, per the brief's
"re-run only if you doubt". I do not doubt it: the implementer reports 7 rules /
7 bindings, 14 predicate tests, 86 harness tests, `tuple_count: 24` — the same
shape recorded at Step 4.1 — and nothing in this step touches the merge-side
call graph the harness reads. The Windows/platform matrices are dispatched
automatically at the landing push, which is also where [P3-2]'s row gets its
first execution.

---

## 9. Findings

### [P2-1] The 481-item figure this step publishes and routes to Step 5.1 is mislabelled, and is not recorded anywhere Step 5.1 will look

**Where:** `GwzM5-8R2DPhase4Closure.md:335-337` and `:361-369`.

**What it says:** "All eleven module-level `allow(dead_code)` attributes in
`checked_artifact/mod.rs` were deleted and the tree recompiled, giving **the true
global inventory** rather than a guess: **481 unique dead items across 50
files**" … "The 481-item surface itself is **the honest measure** of how much of
this subsystem still awaits R2-E and it belongs in Step 5.1's LOC/cohesion audit
as a number rather than as a feeling."

**Two problems, and the second is the one that bites.**

*(i) It is a method-scoped number wearing a global label.* Deleting the eleven
module-level allows leaves every **inner** and **item-level** `allow(dead_code)`
in force, and rustc treats an allowed item as a live root — so everything those
inner allows cover, **and everything reachable only from it**, is excluded from
the count. I measured the other reading: `RUSTFLAGS="--force-warn dead_code"
cargo check --all-targets --all-features -p gwz-core`, which overrides every
allow, yields **1,657 unique dead-item spans across 85 files** inside
`checked_artifact` (deduped by file+span across targets). That is 3.4× the
reported figure. Neither number is "wrong" — they answer different questions —
but 481 is *the surface those eleven allows were hiding*, not "the true global
inventory", and the excluded set is not incidental: it includes
`coordinator/execution.rs`'s 26 items, whose own allow reason reads "Step 3.3
wires the machinery; consumer conversion is **R2-E**" — i.e. items that are
"awaiting R2-E" by the very definition the sentence uses.

*(ii) The routing is asserted but not executed.* `481` appears in **no other
document**: not in `CurrentProgramCheckpoint.md`, not in
`GwzM5-8ChangeBudget.md`, not in the plan. The checkpoint is demonstrably the
place where a Phase-5.1 obligation gets recorded — settle disposition (5) reads
"multi-component writer interruption rows — assigned to **Phase 5.1's evidence
train**" (`CurrentProgramCheckpoint.md:437-438`). A number that lives only in a
Phase-4 closure doc's prose is a number Step 5.1 will not find.

**Why P2 rather than P3.** This is not a citation slip; it is the accuracy of a
measurement the step publishes as a deliverable of settle item 7 and hands
forward as an input to a later gate. The narrowing it accompanies is correct
(§5) and nothing in the code is wrong — which is why it is not P1.

**Correction.** In §5, label the method on the number ("the surface the eleven
module-level allows were hiding, with inner and item-level allows still in
force"), and either record the fully-exposed figure beside it or drop the "true
global" / "honest measure" framing. Then route it for real: a line in
`CurrentProgramCheckpoint.md`'s Step 4.3 landing entry naming it as a Step 5.1
LOC/cohesion input, in the same form disposition (5) uses.

### [P3-1] The lift-verbatim decision record names a symbol that does not exist: `exact_slot`

The catalog interior's row classifier is **`exact_row`**
(`provider/interior.rs:394-418`). `exact_slot` appears nowhere in
`checked_artifact`; the only tree hit is an unrelated test name,
`interface_tests/namespace_roles.rs:553`. The closure doc uses the stale name
six times: §2.2 twice (under the heading "re-verified at `51a9cba`"), §2.4
twice, **§2.7 once — inside the block to be lifted** — and §2.8 once. The name
came from freeze §4.4 Class 2 (`:1309`), which predates the Phase 1 C-3
extension the doc itself credits with widening the bound; the bound was
re-verified against the tree, the identifier was not. The *behaviour* described
is correct in every instance. Fix the token in all four sections before the
lift, since the activation package that reads this record will grep for it.

### [P3-2] §6.2 claims the [P3-6] racing window "now is" executed; on the review host it is skipped, and this step ran no matrix

The row is correctly built at the true window (§6.2 above), but
`strand_alias` returns `None` on macOS and the test early-returns without
driving anything (`platform/anchor/tests.rs:258-276`, guarded by
`hard_link_identity_sharing_is_what_the_retirement_rows_assume` at `:308-318`).
The implementer's characterisation — "`platform/anchor/tests.rs` gained a
portable row only" — is inaccurate: the row takes the same platform branch the
two sibling retirement rows take. Step 4.2's retirement evidence came from a
native Windows probe for exactly this reason. **Not blocking**, because the
Windows and platform matrices dispatch automatically at the landing push and
`hard_links_share_durable_identity()` is true there, so the row does execute at
the landing. Correct §6.2 to say the row is *written*, with execution pinned to
the landing's matrix run — the same phrasing Step 4.2 used for its own
retirement rows. Confirm it in the run-21 result before the Phase 4 settle.

### [P3-3] The [P3-7] derivation record enumerates four `Invalid` arms; the tree has five

`survey` has six `AnchorState::Invalid` returns (`anchor.rs:304, 319, 328, 335,
338, 359`). Folding the two duplicate-entry sites the way the record does, that
is **five** distinct conditions, and the record lists four. The omitted one is
the catch-all at `:359`: `(Some(final), Some(alias))` where the alias's own
derived final name is not the resident final name, or their identities differ —
both entries individually well-formed, mutually inconsistent. It is reachable by
foreign contamination (a planted, self-consistent `.roundtrip` alias of a
*different* anchor identity beside the real anchor — two anchor-prefixed
entries, so the `> 2` arm does not catch it). The record's thesis survives
intact: that trigger is foreign contamination of the reserved grammar too, and
the protocol cannot produce it, since it writes anchor names only through
`anchor_name` from each object's own identity. Corroboration that five is the
right count: the Step 4.2 review itself wrote "Round 2 adds one arm to **a set
of four**" (`GwzM5-8R2DStep42-Review.md:804-805`). Add the fifth arm before
lifting.

### [P3-4] §7's verification appendix omits the one gate that is currently red, and the pin-refresh obligation with it

The appendix (`:432-445`) lists eight lines and does **not** include
`python3.13 scripts/checks/check_checked_artifact_boundaries.py`, `cargo clippy`,
`cargo fmt`, the full-lib run, or the L2-04 harness — all of which the
implementer actually ran and reported, and the first of which currently reports
four findings. The four moved pins and their recomputed digests exist only in
the implementer's transcript, which does not survive the landing. Freeze §4.4
requires the digest refresh in the converting commit, and Steps 3.3/4.1/4.2 each
recorded theirs. Extend §7 with the full gate list and add the four pins by name
so the landing commit cannot forget them.

### [P3-5] §2.2's supersession sentence conflates the cap's value with the zero-headroom property; only the value moved

The doc says the 74-entry budget "supersedes freeze §4.4 Class 2's
`MAX_INTERIOR_ENTRIES = 10` … zero headroom". The *cap* moved (10 → 74); **zero
headroom did not** — `MAX_ROOT_ENTRIES` is still exactly
`MAX_INFRASTRUCTURE_ENTRIES + MAX_ACTIVE_ACTION_DIRS` by construction, which
`admission/tests.rs:484-490` states in those words and tests. This cuts the
doc's own way: a still-exact budget makes admitting an open-ended legacy family
*worse*, not better, so the Option 2 case is stronger than §2.2 states. The
lifted record variant A already phrases it correctly ("open-ended against a
74-entry root budget"); only §2.2's prose needs the clause split.

### [P3-6] The new `too_many_arguments` rationale over-argues its second clause

The allow on `AuthorityFactsIssuerV1::issue` (`owner/host.rs:67-73`) is the
right call — it is scoped to one function, carries a written reason, matches the
established precedent (`BarrierIntentV1::issue`, `protocol/barrier.rs:36-38`),
and the arity moved 7 → 8 only because settle item 8 added the digest;
refactoring the seam's shape is not a <300-LOC disposition step's business. But
the added clause — "collapsing them into a struct would hand callers a value
they could build outside the transaction" — does not hold on its own terms:
eight positional arguments are exactly as caller-supplied as one struct, and the
unforgeability lives in `RetainedAuthorityFactsV1`'s private constructor, which
a collapse would not touch. The precedent's own reason ("deliberately binds each
independent retained fact") is the sound half and is sufficient. Trim the second
clause.

### [P3-7] §2.5 presents plan §9 item 2 as quoted verbatim while silently truncating its final clause

The quote at `:140-144` stops at "before any production catalog activation" and
drops "— and Step 0.1's freeze memo carries the Track-P analysis for both
options so the 4.3 decision is evidence-based, not re-scoped" (plan §9
`:600-602`). No ellipsis marks the cut, under a sentence that says "records the
adopted §7.2 decision **verbatim**". The dropped clause is the one that points at
freeze §5 decision 2's discharge — which §2.4 cites separately, so nothing is
lost in substance. Restore the clause or mark the elision.

---

## 10. Escalation

**Freeze §9 escalation trigger: HIT** — [P2-1] is a P2 against the object, and
§9 :1556 makes escalation automatic on any P0/P1/P2 for Phase 4 steps 4.1-4.3.
**Recorded here; routing is the lane owner's**, per the brief and per §9's own
"That edit is the lane owner's".

Context for the routing decision, offered without prejudging it: [P2-1] is a
record/measurement finding, not a mechanism finding. No code claim in this step
failed verification, no gate is red on the code, and the narrowing the number
accompanies is proved correct by a clean-target clippy. The Step 4.2 precedent
(round-1 escalation recorded, discharged at round 2 once both P2s were closed at
the mechanism) fits: this one closes at the record.

## 11. Landing checklist (non-findings, for the commit)

1. Lift **§2.7 variant A** — after [P3-1]'s token fix.
2. Lift **§3**'s dirent-barrier closure record; consider marking
   `CurrentProgramCheckpoint.md:1106-1112` CLOSED against it.
3. Lift **§6.3**'s `Invalid` derivation record — after [P3-3]'s fifth arm.
4. Refresh the **four** digest pins in
   `scripts/checks/check_checked_artifact_boundaries.py` and re-run the checker
   to green ([P3-4]).
5. Record the Step 4.3 entry in `CurrentProgramCheckpoint.md`, carrying the
   [P2-1] routing decision, the settle-item-7/8 dispositions, and the Step-5.1
   input.
6. Confirm [P3-2]'s racing row executed in the landing's Windows matrix run
   before the Phase 4 settle.

---

**Verdict: GO.** Round 1 of 2. One P2 (escalation recorded), seven P3. Nothing
blocks the landing; items 1-4 above are the corrections to make in it.
