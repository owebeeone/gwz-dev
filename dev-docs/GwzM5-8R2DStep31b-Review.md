# R2-D Phase 3 Step 3.1b — the managed intent record's durable lifecycle (freeze §4.3 row E17): single-axis State review

Date: 2026-08-22. **Single-axis State-lead** (`GwzM5-8R2DInterfaceFreeze.md` §9:
"Everything else is **single-axis with automatic escalation on any P0/P1/P2**:
… Phase 3 steps 3.1-3.3 **including the Phase 3 settle**"). Round 1 of a
two-round cap. Peer-blind: the implementer's report was read as *claims to
verify*, never as evidence; every number below was recounted and every gate
re-run on an independently-constructed tree. **Escalation is TRIGGERED** — see
§12; routing is the lane owner's.

Object: the uncommitted Step 3.1b edits over gwz-core base `e72e376`
("Checkpoint: land … Step 3.1"), exactly ten files:

| File | Disposition |
| --- | --- |
| `checked_artifact/bootstrap/managed/tests_intent_matrix.rs` | **NEW** |
| `checked_artifact/bootstrap/managed.rs` | modified |
| `checked_artifact/bootstrap/managed/provider.rs` | modified |
| `checked_artifact/bootstrap/managed/tests_provider.rs` | modified |
| `checked_artifact/capability/pre_catalog.rs` | modified |
| `checked_artifact/capability/pre_catalog/provider.rs` | modified |
| `checked_artifact/capability/pre_catalog/provider/managed_mutation.rs` | modified |
| `checked_artifact/interface_tests/fault_expected_keys.rs` | modified |
| `checked_artifact/namespace/host.rs` | modified |
| `checked_artifact/namespace/roles.rs` | modified |

Baseline: gwz-core `e72e376`; gwz-dev repo HEAD `9939605`.

**Concurrent-lane attribution, constructed rather than assumed.** The live tree
also carries a concurrent ARM-fixture lane's seven uncommitted files
(`git/tests/g15/root_preservation/{observation,support}.rs` and
`workspace_ops/merge/v1_lifecycle/tests/{finalization,finalization_root,fixtures,forward,reverse_preservation/mod}.rs`).
They are **attributed, not judged**. My gate tree is a copy outside the repo in
which those seven files were restored from `git show HEAD:` and nothing else was
touched; a per-file digest table (§1) proves the ten object files are at their
live bytes and the seven foreign files at HEAD. The scratch's checker output is
**exactly** the one expected land-time pin, which is the precondition the brief
set for trusting the rest of the train.

---

## 1. Object inventory and the attributable tree

Independent digest attribution, live vs scratch vs `HEAD` (first 16 hex of
SHA-256, all seventeen changed files):

| File | live | scratch | HEAD |
| --- | --- | --- | --- |
| the ten object files | *live* | **= live** | ≠ live |
| the seven ARM-lane files | *live* | **= HEAD** | = scratch |

`tests_intent_matrix.rs` is absent at HEAD (empty-string digest), confirming it
is new rather than a rename.

Independent line count, non-blank and non-comment (`//`, `///`, `//!`, `*`
excluded), computed from `git diff -U0` per file:

| File | Raw | Code |
| --- | --- | --- |
| `bootstrap/managed/provider.rs` | +311/−144 | **+198** |
| `capability/pre_catalog/provider/managed_mutation.rs` | +253/−10 | **+167** |
| `namespace/host.rs` | +72/−3 | **+52** |
| `namespace/roles.rs` | +21/−0 | **+15** |
| `capability/pre_catalog.rs` | +12/−7 | **+3** |
| `capability/pre_catalog/provider.rs` | +13/−6 | **+3** |
| `bootstrap/managed.rs` | +2/−0 | **+2** |
| **Production subtotal** | | **440** |
| `bootstrap/managed/tests_intent_matrix.rs` | NEW 375 | 253 |
| `bootstrap/managed/tests_provider.rs` | +131/−28 | +93 |
| `interface_tests/fault_expected_keys.rs` | +74/−30 | +26 |
| **Test subtotal** | | **372** |
| **Total** | | **812** |

The lane's self-reported **440 / 372 / 812** is **confirmed exactly**, as is
its `+889 / −228 across 9` modified-file raw count.

---

## 2. Gate train — executed by this review on the attributable tree

Every command below ran in the scratch with
`CARGO_TARGET_DIR=…/scratchpad/p31-target`.

| Gate | Result |
| --- | --- |
| `cargo check --all-targets` | clean, no warnings |
| `cargo test --lib checked_artifact::` | **346 passed; 0 failed**; 1169 filtered |
| `cargo test --lib checked_artifact::bootstrap::managed::` | **16 passed; 0 failed** |
| `cargo test --lib interface_tests::fault_expected_keys` | **6 passed; 0 failed** |
| `cargo test --lib interface_tests::capability_permit` | **8 passed; 0 failed** |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings` | **exit 0**, clean |
| `rustfmt --edition 2024 --check` on the ten files | **exit 0** |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | failed with **exactly one** finding: `checked_artifact/capability/pre_catalog.rs` |

**The checker pin is exactly what the brief required and nothing else.** On the
*live* tree the implementer reported two findings; the second
(`workspace_ops/merge/v1_lifecycle/mod.rs`) vanishes when the ARM lane's files
are at HEAD, which independently confirms the attribution split. Both are
tree-digest pins refreshed by their owner at land time.

**341 → 346 (+5) is arithmetically right**: the predecessor review recorded 341
at `e72e376`; this object adds the four `tests_intent_matrix` tests and takes
`tests_provider` from 11 to 12 (one test replaced by its positive twin, two
added). One incidental corroboration of attribution: my filtered count is 1169
against the implementer's 1170 — the missing test is the ARM lane's, restored to
HEAD in my tree.

---

## 3. Priority 1 — the wedge is closed, and it is the review's own case

`a_row_interrupted_inside_marker_retirement_resumes_from_its_resident_intent`
(`tests_provider.rs`). **Verified to reproduce [P1-1] exactly**, on four counts:

1. **≥2 components.** The fixture plans `PreservationBundles` alone on a fresh
   workspace, so the missing suffix is `stash` + `bundles` — the ordinary
   first-merge `.gwz/stash/bundles` shape [P1-1] names as reachable.
2. **Interrupted inside marker retirement.** It stops the process at
   `ManagedBootstrapMarkerRetire`, which is Step 2.3's own post-rename boundary,
   not a synthetic file deletion.
3. **Component 0 retired, component 1 not.** The test asserts both halves
   (`retirement_row(0).is_file()`, `!retirement_row(1).exists()`) *before*
   resuming, so the durable precondition is pinned rather than assumed.
4. **It converges.** The resumed drive returns the retained parent at full
   declared depth 3, and `.gwz/stash/bundles` is durably present.

**The old code would have refused this state — verified against the base, not
inferred.** At `e72e376`, `execute_row` calls `classify_resume` before any
statement that can mutate, and `classify_resume` counts resident
`RetiredBootstrapMarker(ordinal)` rows over the row's `component_range`:
`0` → re-derive, `len` → settled, anything else → typed refusal
("…partially retired and needs its resident intent to resume"). One of two
retired is exactly the middle arm. Because the derivation is deterministic, every
later drive re-enters the same refusal — the permanent wedge. The base's own
test `a_partially_retired_row_is_refused_rather_than_replanned` pinned that
refusal on the identical durable predicate, and this step deliberately replaces
that test with its positive twin. That replacement is correct and is the right
way to record the semantic change.

**[P3-2] is likewise driven for real.**
`a_settled_row_whose_component_vanished_refuses_at_the_final_reproof` bootstraps
fully, removes `.gwz/merge`, and asserts a refusal. That is the
`depth != components.len()` guard at `provider.rs:447-451`, reached through the
`IntentResumeV1::Settled` arm (which runs zero loop iterations), so the guard is
now driven rather than only structurally argued.

---

## 4. Priority 2 — chain linkage, not adoption

**The mechanism is as described.** `resume_intent` (`provider.rs:189-232`) walks
`row.generation_range()` from its **first** generation, breaks at the first
generation with neither an active nor a retired row resident, and threads
`predecessor = Some(intent.intent_id())` forward into the next
`read_and_bind_managed_bootstrap_intent`. The classification is a total function
of `(newest active, any retired)` and its three arms are disjoint by the
publish-before-retire order.

I put four adversarial questions to it.

**(a) Can a fabricated or leftover record at a higher generation be adopted
without its predecessors? No — twice over.** The walk *breaks* at the first
absent generation, so a higher record is never read and never becomes the
resume. When the drive later reaches that generation, `settle_generation`
(`provider.rs:244-281`) skips write-and-publish only because the active row is
resident, and then compares the resident row **byte-for-byte** against the bytes
it derived (`if namespace.observe_bootstrap_intent_row(…) != bytes { … }`). A
foreign record is a typed refusal, not an adoption.

**(b) Can two interleaved actions' records cross-link? No.** The frozen
`read_and_bind_managed_bootstrap_intent` (`protocol/…/codec.rs:189-206`,
untouched by this step) requires `matches_bound_plan`, which delegates to
`matches_reservation` (`managed_bootstrap_record.rs:349-371`) and compares
`action_digest`, `request_owner_binding`, `reservation_digest`,
`schedule_digest`, `managed_plan_digest`, `spec_digest`, `bootstrap_ordinal`,
`generation_start`, `component_start`, component count and the generation bound
— **and then** purpose, plan digest, spec digest, the initial retained parent and
the missing-suffix names. So the record binds the **action and the reservation**,
which is precisely what the Step-3.1 review's token-boundary note demanded, plus
the generation and predecessor the caller supplies. A record from another action
fails at `action_digest`. A record moved into the wrong generation slot fails at
`generation_ordinal != expected_generation`, so the slot index and the record's
own generation field must agree.

**(c) Is the self-consistency-not-exclusion boundary stated in the doc contract?
Yes, and it is well stated.** `provider.rs:51-61` says the read-back is
"*self-consistency* — 'this chain is the one this admitted action's plan
describes'", that it is "**not** an adoption or exclusion proof", and that "a
later step that uses a resident record to decide adoption of state this action
did not create makes determinism load-bearing for exclusion, and must
re-litigate the token then". That is the right boundary in the right place, and
it is inherited from the E16 record's same-user namespace boundary.

**(d) Does the read-back path honour it — no adoption-proof upgrade?** Yes.
`try_initial` runs **only** on the `Fresh` arm; a resumed drive takes the token
from the resident record, and because the token is a deterministic function of
the bound plan the two values coincide anyway, so nothing is upgraded. The one
place where a truncated chain could "look complete" — a resident retired
generation with no active one — falls through to the final reproof, which
`observe_spec` fails unless every declared component is durably present. That is
the [P3-2] guard doing exactly the work the design assigns it.

**Verified-and-benign, recorded not filed.** The `BootstrapIntentScratch` row is
a single `BaseActionSlotV1` shared by every generation of every row of one
action (`protocol/slots.rs:108`), while `BootstrapIntentActive/Retired(g)` are
per generation. Sharing is safe here because write and publish are inside one
guarded block, the publish re-observes both source and destination bytes, and
the post-publish comparison re-proves the row. The matrix's slot-set stability
rows are the executed evidence. No slot, name, record or key is minted: the
whole vocabulary pre-exists in the frozen `protocol/` tree, which this object
does not touch.

---

## 5. Priority 3 — the fifteen-key activation

**All fifteen sites are in `managed_mutation.rs`, and the owner holds none.**
Counted, not taken on trust: `grep -c 'fault_v1::hit'` gives **14** in
`capability/pre_catalog/provider/managed_mutation.rs` (8 from Step 2.3 + 6 added
here) and **0** in `namespace/host.rs`. The new host entry points
(`write_bootstrap_intent_scratch`, `observe_bootstrap_intent_row`,
`read_bootstrap_intent_row`, `bootstrap_intent_row_is_resident`) validate the
action binding and forward; the announcements happen in the provider owner,
which is the shape Step 2.3 established and which the fixture's declared-source
list depends on. Six physical `hit` sites carry fifteen keys through the
`ManagedIntentEdgeV1` selector — the same edge-selector shape
`ActionNamespaceEdgeV1` already uses for `namespace.*`, and the fixture's
per-key scan resolves each of the fifteen key names in that file.

**The rows are real on both target variants.** I ran the matrix with
`--nocapture` and counted the evidence lines: **30** interrupt/restart/converge/
re-settle rows (15 keys × 2 variants) and **26** repeated-boundary rows (13 × 2)
at `rounds=12 | slots-stable=yes | converged=yes`. Each matrix row asserts three
things, not one: the fault point was reached; the restart converges to the
byte-identical three-directory census of the settled baseline; and a further
fresh session mutates nothing.

**Why 13 of 15 repeat — the two exclusions have a stated, correct reason.** The
inclusion criterion is carried over verbatim from
`namespace/tests_managed_matrix.rs`: a boundary is single-crossing when the
durable state its crash leaves routes the next drive *past* it. The two
excluded are `final_intent_retire` and `final_intent_retired_reobserve`, because
once the final retirement is durable no active generation remains, the resume
classifies the row `Settled`, and the drive short-circuits to the final reproof
— I traced that and it holds. The taxonomy is machine-enforced:
`assert_boundary_partition()` fails if the two classes overlap or if their union
is not exactly the fifteen-key matrix, and `reconcile_executed_keys()` pins the
matrix against the fifteen stable key strings. This is the discipline the
program asks for, applied without prompting.

**The fixture edit is sound.** `MANAGED_BOOTSTRAP_STEP_2_3_KEYS` →
`MANAGED_BOOTSTRAP_EXECUTED_KEYS`, 23 entries, delimited by the step that
converted each (`// Step 2.3 —`, `// Step 3.1b —`); the `PartiallyExecuted`
owner string names both steps and both rows; the `only_the_families_…` literal
tracks; and a new `assert_eq!(…len(), 23, …)` size assertion fails closed if the
subset is widened without its matrix. The key-by-key siteless proof
(`reserved_fault_families_have_no_injection_sites_before_their_package`) is
genuine in both directions: every activated key must have a site, and every
non-activated key of a partial family must have none. I independently grepped
all seven reserved key variants and found **zero** references outside the
vocabulary file.

**Counts, recounted.** 165 total held (`fault_v1.rs` untouched — not in the
object). `managed_bootstrap.*` = 30 keys; executed **8 → 23**; reserved
**22 → 7**; no key minted.

**Recorded, not filed: the Git-directory arm's managed prefix is fixture-placed.**
`prepare_managed_root` creates `<gitdir>/.gwz` so the walk has a root, because a
Git-directory catalog has no `.gwz` ancestor. That is *the second of the two
routes the Step-3.1 review's [P3-3] explicitly sanctioned*, the production
refusal stays pinned by
`a_git_directory_target_refuses_the_workspace_rooted_managed_paths` (green), and
the axis the variant genuinely exercises — the catalog target whose lease,
catalog and action directory hold every intent record — is the axis E17 lives on.
Disclosed in the fixture doc, in the matrix header and in the report. Follow-up 3
remains the owner decision that would change it.

---

## 6. Priority 4 — the seven remaining reserved keys, and a ruling

**The five writer keys are genuinely untouched by 3.1b's edges — verified.**
`staging_directory_create`, the three `ownership_marker_*` and
`staging_directory_flush` are crossed by `stage_component` /
`write_or_rewrite_marker`, which E17's lifecycle never calls: `settle_generation`
and `retire_generation` reach only `write_managed_intent_scratch`, the Step-2.2
backend's `publish_bootstrap_generation` / `retire_bootstrap_generation`, and
`observe_managed_intent_row`. All five remain siteless (zero references outside
`fault_v1.rs`), so the deferral to Step 3.2 stands untouched by this step. Note
that 3.1b's matrix *drives through* those edges every round without activating
them, which is exactly the intended distinction and is what the fixture proves.

**`preflight` and `plan_complete` — the ruling asked for.**

*Leaving them "reserved" indefinitely is not coherent, and the reason is
structural.* "Reserved" in §3.5 means *its converting package has not landed
yet*. For these two the converting package **has** landed: Step 3.1 implemented
`observe_preflight` and `execute_bound`, and 3.1b has now finished the record
lifecycle those two keys sit at the ends of. They are therefore in a third state
the frozen map has no cell for — *converted, and determined to have no
boundary*. The plan's own forward clause ("any key whose edge genuinely converts
later is explicitly re-reserved for R2-E/R2-F in the same update") presupposes
that every key's edge converts *somewhere*; a key with no edge at all has no home
in that vocabulary, so silence will not resolve itself.

*And the stated justification is weaker than it sounds.* "They name plan-level
states rather than durable edges" is true but not sufficient, because this very
family already activates non-edge markers: `parent_revalidate`,
`component_reobserve`, `final_directory_reobserve`, `final_identity_reobserve`
and eight of this step's own fifteen are observation or restart-entry markers,
not durable edges. `managed_bootstrap.preflight` ("stopped after the preflight
walk") and `managed_bootstrap.plan_complete` ("stopped after the last row was
bootstrapped, before the caller consumed the proof") are both perfectly
well-formed crash points, and `plan_complete` in particular is restart-relevant
for Step 3.3's coordinator glue. So the honest statement is the implementer's
second clause — "no step has yet given them a boundary" — which is a
*description*, not a disposition.

*Ruling.* They need a disposition, and its natural owner is the **Phase 3
settle** (with Step 3.3 the only remaining candidate to give `plan_complete` a
boundary). Two acceptable forms: a named step that owns them, or a recorded
determination that they are permanently siteless plan-level vocabulary retained
in the 165 census. This is **not** 3.1b's debt — 3.1b converts no edge of theirs
— and a durable in-tree record already exists in two places
(`fault_expected_keys.rs` and the matrix header), which is why this is P3 and not
P2. But if the Phase 3 settle closes without the determination it becomes
settle-blocking: Step 5.1 owes "per-key executed fault evidence … every activated
family, both target variants", and a partially-executed family carrying two
permanently-siteless, unowned keys cannot be checked off against that.

Filed as **[P3-1]**.

---

## 7. Priority 5 — E17's two §4.4 Class 1 arms resolve to none

**Verified by reading the write, publish and retire paths end to end, not from
the annotation.** The chain is:

- `write_managed_intent_scratch` (`managed_mutation.rs`) — primitive family P2
  only: `durable_write_options`, `write_all`, `sync_all`, `sync_directory_edge`,
  then a bounded regular-file reobservation. It writes a **regular file** into
  the retained action directory. No rename, no directory.
- `publish_bootstrap_generation` / `retire_bootstrap_generation`
  (`namespace/operations.rs:93-129`, Step 2.2, unchanged) — both call
  `validate_action_source(…, NamespaceObjectKind::RegularFile)` before the edge.
  That is the same role validator the E16 annotation leans on.
- `HostActionNamespaceV1::{publish_no_replace, retire_exact}` (`host.rs:602-629`,
  unchanged, documented as edges E12/E13) → `execute`, which re-refuses unless
  `source.kind() == NamespaceObjectKind::RegularFile` →
  `RetainedActionNamespaceV1::execute_edge`
  (`namespace_mutation.rs:250-305`, unchanged), which calls
  `publish_verified_no_replace(&self.handle, …, &self.handle, …,
  PublicationSourceV1::regular_file(&source.encoded_identity, &source.bytes),
  DestinationRecheckV1::None, label)`.

So every move of this lifecycle is a regular-file protocol record travelling
between two deterministic slots of **one** retained action directory — source and
destination handle are literally the same `self.handle` — carrying
`PublicationSourceV1::regular_file` and `DestinationRecheckV1::None`. **No
directory is published anywhere in the lifecycle and no retirement destination
is re-checked anywhere**, which is the falsification test the brief set. Neither
`PublicationSourceV1` nor `DestinationRecheckV1` is touched by this object
(`publication.rs` is not among the ten files). The E16-annotation
conditional-resolution mechanism is the right precedent and is applied in its
form.

Consequently §4.4's arm table rows are, on this tree, **"managed source-interior
| E15, E17" driven by E15 alone** (Step 2.3's `install_managed_component` carries
that arm, `host.rs:631-635`) and **"managed destination | E16, E17" driven by
neither**. The block's arithmetic on this point is correct.

---

## 8. Priority 6 — the publication seam

Counted myself, not taken from the report:

| Check | Result |
| --- | --- |
| `grep -rn 'publish_verified_no_replace(' src/` on the object | **21** |
| the same at `HEAD` (`git grep -c … HEAD`) | **21** |
| `managed_mutation.rs` | **2**, unchanged |
| `tests_intent_matrix.rs` (new file) | **0** |
| `interface_tests/capability_permit.rs` | byte-untouched; `:139` still asserts **13**; suite 8/8 green |
| `CATALOG_PUBLICATION_CALL_COUNTS` | lives in `scripts/checks/check_checked_artifact_boundaries.py`, not in the object; the checker's caller-inventory arm passes |

No companion is owed. The prediction held exactly.

---

## 9. Priority 7 — is the liftable §3.5 block fit to lift verbatim?

**House form: yes.** It is a dated blockquote in the `admission.*`/Step-2.3
form, names what it discharges (`GwzM5-8R2DStep31-Review.md` [P2-1] and 3.1b's
own conversion), carries counts, and states obligations as future duties rather
than as indicative claims about undone work.

**Claims checked against the tree, one at a time:** 23 of 30 executed ✓; the
fifteen key names ✓ (exact match against the vocabulary and the fixture); "all
fifteen have injection sites in `managed_mutation.rs` … the `namespace` owner
holds none" ✓ (14 sites there, 0 in `host.rs`); "13×2 repeated-boundary rows at
twelve rounds each" ✓ (26 lines observed); "Step 3.1 (landed `e72e376`)" ✓;
`stage_component` and `write_or_rewrite_marker` cross the five ✓; `mod provider`
carries `allow(dead_code)` ✓ (`bootstrap/managed.rs:5-10`); the §4.4 arm-rows
sentence ✓ (§7 above); "the `capability_permit.rs` caller inventory holds at 13
and `CATALOG_PUBLICATION_CALL_COUNTS` is unchanged … adds zero
`publish_verified_no_replace` call sites" ✓; 8 → 23 / 22 → 7 / 165 ✓.

**But it is NOT fit to lift verbatim, for one reason — see [P2-1].** §3.5
already carries a landed *Deferral record* (`InterfaceFreeze.md:465-478`, the
lane owner's discharge of the Step-3.1 review's [P2-1]) whose forward-looking
figures this step falsifies:

> "Step 3.1's `execute_bound` converts the *edges* of five of **the 22 reserved
> keys** … Step 3.2 therefore owes: … and the `PartiallyExecuted` list edit
> (**8 → 13 executed**). The five stay counted among **the 22 reserved** until
> 3.2's flip".

After 3.1b the five are among the **7** reserved and Step 3.2's flip is
**23 → 28**. The new block restates Step 3.1's deferral in fuller form but says
nothing about the record it duplicates — and the house mechanism is explicitly
*append, do not edit* ("the rows are left as written, since this annotation is
the sanctioned mechanism"), so the default action produces a §3.5 that states
the same deferral twice with contradictory arithmetic, in the two sentences that
are *instructions to Step 3.2*.

**Wording caution, recorded not filed.** "Nothing unactivated is reachable in the
interim: `mod provider` carries `allow(dead_code)` and plan §5 item 2 forbids
production catalog activation." The conclusion is true, but `allow(dead_code)`
is what *suppresses the warning*, not what establishes unreachability — it is in
fact the same blanket allow that hid the Step-3.1 review's [P3-1] item. The real
warrant is the second clause plus the absence of a production caller for the
provider (Step 3.3's work). Worth one word of rephrasing while the block is
being edited anyway.

---

## 10. Priority 8 — `retain_managed_parent`, [P3-1]'s second half

**Grep confirmed, exactly as claimed:**

```
src/checked_artifact/capability/pre_catalog.rs:38                    (comment)
src/checked_artifact/capability/pre_catalog/provider.rs:74,:78       (comment)
src/checked_artifact/capability/pre_catalog/provider/managed_mutation.rs:129   definition
src/checked_artifact/capability/pre_catalog/provider/managed_mutation.rs:305   (comment)
src/checked_artifact/capability/pre_catalog/provider/managed_mutation.rs:390   call site
```

`:390` is inside `retain_managed_parent_at_for_test`, which is `#[cfg(test)]`.
**Zero production callers**, confirmed. No lint catches it because
`checked_artifact/mod.rs:19-23` puts a blanket `#[allow(dead_code)]` on
`mod capability` — verified at the source.

The definition's comment says what the report claims, in the second of the two
forms the Step-3.1 review offered: "This wrapper has **no production caller** …
its only caller is the `#[cfg(test)]` enclosure door … It is kept rather than
folded into that door so the door stays a two-line ambient-open, and its
production visibility is a standing item for the lane owner at the Phase 3
settle." All three misleading comments are corrected to the real call graph
(`retain_managed_prefix` → `retain_managed_child`), each citing the review.

**The settle disposition is the lane owner's and can wait.** I looked for a
reason it could not: the item is `pub(in crate::checked_artifact)`, is reachable
from no production path, changes no durable behaviour, and its retention is now
stated at the definition rather than implied. There is none. Recorded as a
standing settle item, not filed as a finding.

---

## 11. Budget

**812 code (440 production + 372 test) against an aspirational <500** — ~1.6×,
down from Step 3.1's ~2×.

**Judgement: load-bearing, and better than the headline ratio suggests.** The
*production* half is **440 — inside the target**. The whole overrun is the
mandated activation evidence: the 253-line matrix is fifteen keys × two target
variants × interrupt/restart/converge/re-settle plus thirteen repeated-boundary
rows at twelve rounds, which is the shape RemPlan §10 and the L1-16/L2-14 form
require, and 93 of the remaining test lines are the wedge and [P3-2] pins the
predecessor review asked for by name. Reducing it would mean deleting required
evidence. The plan's own §4 File-ownership note anticipates this band
("1,000-line cohesion trigger, sub-500 targets"), and no file crosses the
cohesion trigger. Recorded for the settle; no finding.

---

## 12. Findings

### [P2-1] The §3.5 annotation, appended by the house mechanism, leaves the landed Step-3.1 deferral record's forward-looking figures false

`GwzM5-8R2DInterfaceFreeze.md:465-478` — the Deferral record the lane owner
landed to discharge the Step-3.1 review's [P2-1] — instructs Step 3.2 in terms
that 3.1b falsifies: the five writer keys are "five of **the 22 reserved keys**"
(now 7) and Step 3.2 owes "the `PartiallyExecuted` list edit (**8 → 13
executed**)" (now 23 → 28). The offered block restates the same deferral more
fully but gives no instruction about the paragraph it duplicates, and the
sanctioned mechanism in this section is to append rather than edit. Appended
as-is, §3.5 carries the Step-3.1 deferral twice, with contradictory arithmetic,
in exactly the two sentences a future Step-3.2 implementer reads as their duty.

This is the same defect class the predecessor filed at P2, one turn later: the
durable record is the thing the machinery exists to get right, and a discharge
that leaves its own section self-contradictory has not fully discharged. The
severity is capped below P1 because the in-tree fixture is correct and
machine-checked — `MANAGED_BOOTSTRAP_EXECUTED_KEYS.len() == 23` and the
partial-list equality assertion would fail a Step-3.2 implementer who followed
"8 → 13" — so the error is caught at a gate rather than shipping silently.

**Required remedy — the lane owner's edit at land, one of two forms:** replace
the Deferral-record paragraph with the new block's Step-3.1 paragraph (which
supersedes it in substance), or add one superseding clause to the new block —
"superseding the figures in the deferral record above: the five writer keys are
now among the **7** reserved, and Step 3.2's `PartiallyExecuted` edit is
**23 → 28 executed**". Counts stay at 165 throughout either way.

### [P3-1] `preflight` and `plan_complete` have no disposition, and "reserved" no longer describes them

Ruled in §6. Their converting package has landed, so "reserved for Phase 3" is
now a category error, and the stated justification ("plan-level states rather
than durable edges") is in tension with the family's own precedent — five of the
twenty-three executed keys are non-edge observation or restart-entry markers.
The remedy is a **Phase 3 settle determination**: a named step that owns them
(Step 3.3 is the only remaining candidate, for `plan_complete`), or a recorded
determination that they are permanently siteless plan-level vocabulary retained
in the 165 census. Not 3.1b's debt; 3.1b converts no edge of theirs, and a
durable in-tree record of the current state exists in `fault_expected_keys.rs`
and `tests_intent_matrix.rs`. Becomes settle-blocking if the settle closes
without it, because Step 5.1's per-key evidence duty cannot be checked off
against two unowned permanently-siteless keys.

### [P3-2] `BootstrapIntentRowV1::Scratch` is never constructed, and the enum's own doc overstates its use

`namespace/roles.rs` adds the three-armed selector so "the intent lifecycle names
rows by *role* and never by leaf", but the scratch write bypasses it —
`write_bootstrap_intent_scratch` takes `slots.scratch_leaf()` directly — so
`Scratch` has no constructor anywhere (`grep` finds only `Active` and `Retired`,
at `provider.rs:86-87`). Nothing flags it because `mod namespace` carries the
same blanket `#[allow(dead_code)]`. Remedy, either way: route the scratch write
through the selector, or drop the variant and narrow the doc sentence. Dead
vocabulary introduced by the same step that states the rule it violates is worth
a line now rather than an archaeology question later.

### [P3-3] Two comments state the opposite of the code

Same genus as the predecessor's [P3-1], which this step otherwise discharges
well.

1. `provider.rs:558-560`, in `ownership_token`: "When the follow-up lands the
   intent record's durable lifecycle the token is read back from the resident
   record instead of re-derived; the derivation stays as the first-generation
   seed." The follow-up **is this step**, and it has landed — the behaviour is
   now current, not future. Present tense, please.
2. `tests_provider.rs`, in `IntentFixture::new`: "The catalog must exist before
   the managed root is placed for the Git-directory arm; `plan_for` recovers
   it." The code does the opposite order — `prepare_managed_root` runs first and
   `plan_for` (which recovers the catalog) second. The intended statement is
   presumably that the managed root must exist before the plan is made; as
   written the two halves are swapped.

---

## 13. Verdict

**GO**, conditional, with **escalation TRIGGERED** under freeze §9 ("automatic
escalation on any P0/P1/P2"). Routing is the lane owner's; no second axis is
assumed.

The object is sound and is the right closure of the review's [P1-1]. The wedge
is genuinely shut: the fixture reproduces the exact case the predecessor
identified, the base would have refused it permanently, and the resumed drive
converges. The chain is verified link by link from the row's first generation
against a frozen binder that pins action, reservation, plan, purpose, generation
and predecessor, so neither a fabricated higher generation nor another action's
record can be adopted; the self-consistency-not-exclusion boundary is stated in
the contract and honoured by the code. E17's two §4.4 Class 1 arms resolve to
none by direct inspection of the write, publish and retire paths — one retained
directory, regular-file source, `DestinationRecheckV1::None`, neither type
touched. The publication seam is unmoved at 21/2/0 with the permit pin at 13 and
`capability_permit.rs` byte-untouched. The fifteen keys are activated
same-commit with 30 matrix rows and 26 repeated-boundary rows across both target
variants, and the two single-crossing exclusions carry a correct,
machine-partitioned reason. Every gate is green on an independently constructed
tree whose checker output is exactly the one expected pin.

The one escalating finding is a **lane-owner record edit, not an implementer
remediation round**.

| Finding | Grade | Disposition |
| --- | --- | --- |
| [P2-1] the §3.5 block leaves the landed Step-3.1 deferral record's "22 reserved" and "8 → 13" false | P2 | lane-owner supersession clause (or paragraph replacement) in the same edit that lands the block |
| [P3-1] `preflight`/`plan_complete` have no disposition; "reserved" no longer describes them | P3 | Phase 3 settle determination; file and continue |
| [P3-2] `BootstrapIntentRowV1::Scratch` never constructed; doc overstates the selector | P3 | file and continue |
| [P3-3] two comments state the opposite of the code | P3 | file and continue |

Standing item, recorded not filed: `retain_managed_parent`'s production
visibility, retained deliberately as the test door's shared retainer with its
fate assigned to the Phase 3 settle — the lane owner's disposition, and I found
no reason it cannot wait.

---

## 14. Verification appendix — commands executed for this review

Scratch tree: `…/scratchpad/p31b-scratch` = a copy of the gwz-dev workspace
(`Cargo.toml`, `Cargo.lock`, `gwz-core`, `gwz-cli`, `taut-shape-rs`, excluding
`target/` and `.git/`) in which the ARM lane's **seven** files were overwritten
from `git show HEAD:<path>`. `CARGO_TARGET_DIR` = `…/scratchpad/p31-target` on
every cargo call.

```
# attribution, per file, before any gate
for f in $(git status --porcelain | awk '{print $2}'); do
  shasum -a256 live | scratch | git show HEAD:$f
done
  → the 10 object files:   scratch == live,  live != HEAD
  → the  7 ARM-lane files: scratch == HEAD,  live != HEAD
diff -rq --exclude=target --exclude=.git gwz-core p31b-scratch/gwz-core
  → differs in exactly the 7 ARM-lane files, and nothing else

# checker — the precondition for trusting the train
(p31b-scratch/gwz-core) python3.13 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: failed
  - protected source tree changed: checked_artifact/capability/pre_catalog.rs
  EXIT=1                                    [exactly the expected pin, alone]

(p31b-scratch/gwz-core) cargo check --all-targets
  Finished `dev` profile … in 9.64s          [no warnings]

(p31b-scratch/gwz-core) cargo test --lib 'checked_artifact::'
  test result: ok. 346 passed; 0 failed; 0 ignored; 0 measured; 1169 filtered out

(p31b-scratch/gwz-core) cargo test --lib 'checked_artifact::bootstrap::managed::'
  test result: ok. 16 passed; 0 failed; 0 ignored; 0 measured; 1499 filtered out

(p31b-scratch/gwz-core) cargo test --lib 'checked_artifact::bootstrap::managed::tests_intent_matrix' -- --nocapture --test-threads=1
  test result: ok. 4 passed; 0 failed
  30 interrupt/restart/converge rows (15 keys × 2 variants)
  26 repeated-boundary rows (13 × 2) at rounds=12 | slots-stable=yes | converged=yes

(p31b-scratch/gwz-core) cargo test --lib 'interface_tests::fault_expected_keys'
  test result: ok. 6 passed; 0 failed
(p31b-scratch/gwz-core) cargo test --lib 'interface_tests::capability_permit'
  test result: ok. 8 passed; 0 failed

(p31b-scratch/gwz-core) CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings
  CLIPPY_EXIT=0                              [clean]

(p31b-scratch/gwz-core) rustfmt --edition 2024 --check <the 10 files>
  RUSTFMT_EXIT=0

# companion pins and key census, counted not assumed
grep -rn 'publish_verified_no_replace(' src/            object 21 | HEAD 21
  managed_mutation.rs 2 | tests_intent_matrix.rs 0 | capability_permit.rs 2 (:139 asserts 13)
grep -c 'fault_v1::hit' managed_mutation.rs             14
grep -c 'fault_v1::hit' namespace/host.rs                0
grep -c 'managed_bootstrap\.' fault_v1.rs               30
for the 7 reserved key variants: grep -rn '<Variant>' src/ | grep -v fault_v1.rs
  → 0 hits for each of StagingDirectoryCreate, OwnershipMarker{Create,Write,Flush},
    StagingDirectoryFlush, Preflight, PlanComplete
git status/diff -- fault_v1.rs capability_permit.rs publication.rs
    protocol/ scripts/checks/                           [all empty — untouched]

# base-behaviour check for the wedge
git show HEAD:src/checked_artifact/bootstrap/managed/provider.rs
  → classify_resume: retired ∉ {0, len} ⇒ typed refusal, called before any mutation
```

No git write operation was performed at any point; every git call was
`show`/`diff`/`status`/`log`/`grep`/`rev-parse`. The only repo-adjacent file
written by this review is this document.
