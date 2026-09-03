# R2-D Phase 4 Step 4.2 — legacy Windows durability-anchor retirement and the `platform.rs` split: single-axis Code review

Date: 2026-08-23. **Single-axis Code-lead** (`GwzM5-8R2DInterfaceFreeze.md` §9
:1400-1406: "Everything else is **single-axis with automatic escalation on any
P0/P1/P2**: … and Phase 4 steps 4.1-4.3 **including the Phase 4 settle**").
Round 1 of a two-round cap. Peer-blind: the implementer's report was read as
*claims to verify*, never as evidence; every count below was recounted and every
affordable gate re-run by this review in the live tree.

**Escalation IS triggered** — two P2 findings against the object ([P2-1],
[P2-2]). Recorded here per the brief; **routing is the lane owner's**. Neither
P2 is a correctness defect: both are evidence/record findings on the step's own
headline claim, and the underlying arguments verify sound by inspection (§3).
Five P3 findings file and continue (§9: "P3 findings file and continue; they do
not become packages and do not enlarge R2-D").

Object: the uncommitted Step 4.2 edits over gwz-core base `9c454ce` ("Land R2-D
Step 4.1: the four legacy rename edges convert in situ through P1 (review GO)").
Confirmed the whole dirty set by `git -C gwz-core status --porcelain=v1 -uall`:
**10 ` M` + 2 `??` = 12 files** (the report says "11 tracked files" — see
[P3-4]).

| File | Disposition |
| --- | --- |
| `checked_artifact/platform/anchor.rs` | **NEW** (382 lines) |
| `checked_artifact/platform/anchor/tests.rs` | **NEW** (433 lines) |
| `checked_artifact/platform.rs` | modified (+18/−225; 1,093 → 884) |
| `checked_artifact/fault.rs` | modified (48 changed) |
| `checked_artifact/interface_tests/capability_permit.rs` | modified (+51) |
| `checked_artifact/residue.rs` | modified (2 lines) |
| `checked_artifact/transition.rs` | modified (2 lines) |
| `checked_artifact/tests/leaf_publication.rs` | modified (27 changed) |
| `checked_artifact/tests/recovery_protocol.rs` | modified (+4/−1 rows) |
| `checked_artifact/tests/removal_recovery.rs` | modified (+2/−1 rows) |
| `checked_artifact/tests/staging_recovery.rs` | modified (+3/−1 rows) |
| `scripts/checks/check_checked_artifact_boundaries.py` | modified (+10/−1) |

Baseline: gwz-core `9c454ce` (= `origin/main`, unmoved); gwz-dev repo HEAD
`7b84ee1`. Working tree byte-identical to probe branch `ba1f185` for all six
files checked (§7).

---

## 1. Verdict

**GO**, with escalation recorded.

The step converts §4.3 row **E22** in situ on that row's own frozen primitives
("P1 + P5", "New primitive? no"), and the load-bearing scope divergence — not
routing the legacy anchor through the 2.2 barrier ops — is the correct reading of
the authorities, not a shortcut (§2). Backward and forward compatibility hold in
both directions and by construction, not by luck (§3.1). The crash-window
argument for the durable retirement is sound on inspection (§3.2). Both
zero-flips are right and both non-activation records are in house form and fit to
lift, with one clause I would add (§4). The macOS hard-link discovery is real,
measured rather than asserted, and correctly pinned (§5). The split is clean, the
carriers stay true without edits, and the seven `AnchoredPrivateArea` call sites
are byte-identical (§6). The [P3-4] discharge is exact — the row-to-edge mapping
recounts to 4/2/3 (§8). The allowlist shrink is exact by the checker's own
counter and every digest I recomputed matches the reported value (§9). Every gate
I re-ran is green at the claimed numbers (§10).

What the two P2s cost is *evidence and record*, not soundness: the retirement's
own two fault boundaries are announced and never driven, and the second-retirement
refusal is a permanent state recorded as a transient one.

Findings, most severe first:

| # | Finding |
| --- | --- |
| [P2-1] | `BeforeAnchorAliasRetirement` / `AfterAnchorAliasRetirement` are announced (`fault.rs:49`, `:51`) and injected (`anchor.rs:141`, `:154`) but **driven by no test on any platform**. `BOUNDARIES` (`anchor/tests.rs:31-40`) carries **eight**, not ten. So the step's headline claim — the correctness-critical removal becomes a *durable* retirement — has zero executed interruption/restart/convergence evidence at the retirement itself, including on the green Windows probe. The report compounds this by calling it "a 10-boundary interruption/restart/convergence matrix". |
| [P2-2] | The `retirement_occupied` refusal (`anchor.rs:130-139`) is **permanent**, not "a second *outstanding* retirement": nothing ever removes `.ca1-anchor-retired-v1` (the report says so itself), so once the state recurs, every `prepare`/`private_barrier` on that private area fails for ever with no in-code exit and no recorded operator escape. Legacy `remove_file` self-healed here. The E16 no-permanent-wedge standard admits a permanent typed refusal *when the derivation is recorded* ("refused permanently and by design, typed rather than silent"); that derivation is absent, and the words "outstanding" and "bounded at one" state the opposite. |
| [P3-1] | Legacy nonce orphans are **tolerated but never reclaimed**. `survey` matches `.ca1-anchor-scratch-v1` by exact equality (`anchor.rs:298`), so `.ca1-anchor-scratch-<32hex>` left by a pre-4.2 Windows crash falls through all four arms and is ignored for ever. Inert (nothing refuses on it), so the brief's bar — "reclaims **or** tolerates" — is met. But the module's own closed-grammar invariant ("Four names, and no protocol state can produce a fifth") is true only on a fresh tree, and `assert_closed_grammar` would fail on an upgraded one. |
| [P3-2] | The same nonce harm class **survives one file away and is unrecorded**: `authority::scratch_name` (`authority.rs:242-247`) still mints `.ca1-scratch-<kind>-<16 random bytes>` per attempt at `residue.rs:346` and `:444` — the E20/E21 edges Step 4.1 just converted, i.e. exactly the plan's own trigger ("a standing violation of the R2 stop clause **the moment it is on a successful converted path**", plan :408-409). Dotted, so `inspect_family`'s `ca1-<family>-` filter skips it (`residue.rs:127`): a crash between create and publish leaves an orphan nothing can see, name or reclaim. |
| [P3-3] | §4.4's arm table row "any further retirement-destination arm \| E7's Phase-4 half and the terminal retirement edges \| **Phase 4**" is left **unresolved**. The `terminal.*` record resolves the plan's conditional but not the freeze row, where E16's and E17's annotations set the precedent of stating the resolution and leaving the row as written. Nothing is breached yet (Step 4.3 is still Phase 4); the disposition is owed by Phase 4's close. |
| [P3-4] | Stale citations and one miscount. §4.1's P5 row cites the anchor internals at `platform.rs:427/:430/:433/:551-584/:607/:665/:672`; they now live in `platform/anchor.rs`. The E10/E14 annotation's `platform.rs:514` (arm), `:281` (class), `:294` (non-Windows fsync) now resolve to `:526`, `:364`, `:377`. Most were already drifted at `9c454ce`; the split widens it. Disclosed by the implementer as "amendment (1)" and assigned to the lane owner, matching precedent. Separately, the report's "**11 tracked files** +148/−268" is **10** tracked + 2 new; eleven is the count of touched *Rust* files (which is what the rustfmt line means). |
| [P3-5] | Two behaviour deltas outside the report's "behaviour-unchanged" frame. (a) The resume arm opens the staging name with `truncate(true)` (`anchor.rs:221-225`); "scratch resident was unreachable under the random name" is true for gwz-produced state, but for a *foreign* object planted at the now-**predictable** deterministic name the behaviour is new — truncate-and-reuse where legacy would never have touched it. (b) `NeedsReturn` gains a pre-publication `verify` the legacy arm did not have (`anchor.rs:120`); it can only reject, and it converts rename-then-fail into refuse-without-mutation. Both are improvements or immaterial; both are deltas, not nulls. |

Opinions requested by the brief are at §9.3 (pin shape: **convert `platform.rs`
to a TREE pin**) and §10.2 (budget: **record, do not trim**).

---

## 2. The scope divergence — is the `barrier.*` route the right one to decline? **YES**

Plan §4 Step 4.2 says "with the closed barrier/anchor protocol **via the 2.2
barrier ops**". The implementer did not take that route. Verified against the
frozen protocol rather than the report:

`BarrierIntentV1` (`src/checked_artifact/protocol/barrier.rs:19-32`) binds
`action_digest`, `request_owner_binding`, `reservation_digest`,
`schedule_digest`, `ordinal`, `catalog_anchor_identity`,
`private_home_parent_identity`, `private_home_name`, `target_parent_identity`,
`target_path_profile`, `reserved_target_leaf`, `intent_id`. `issue` (`:39-49`)
takes `&NamespaceBarrierAuthority` and `&ActionCapacityReservationV1` and refuses
with `"barrier ordinal is not reserved by the action schedule"` unless
`ordinal.index() < reservation.schedule().barrier_count()`. `barrier_slots`
(`namespace/mod.rs:169-198`) wires `BaseActionSlotV1::BarrierIntentScratch`,
`ActionSlotV1::BarrierIntentActive(index)`, `BarrierIntentRetired(index)` and
`RetiredRoamingAnchorAlias(index)`. The sixteen `barrier.*` keys
(`fault_v1.rs:132-147`) name that protocol edge for edge: five intent-record
keys, then `anchor_outbound`, `target_barrier`, `anchor_return`,
`target_alias_retire`, and their reobservations.

**The legacy anchor is structurally that protocol's ancestor**, confirmed in the
code: `anchor::publish` passes the *same* `dir` as both source and destination
parent (`anchor.rs:266-278`, with the comment "Every anchor edge is also
same-directory, which is why one parameter names the parent"); there is no intent
record, no ordinal, no target leaf anywhere in the module.

Of the three sufficiency reasons, **(i) alone is airtight**: every entry point of
the frozen protocol demands an `ActionCapacityReservationV1`, i.e. an
`AdmittedActionV1`, i.e. consumer conversion — placed outside R2-D by plan §5
items 1 and 2, and independently confirmed by `recover_or_create` still having
**no production caller** (defined `catalog/bootstrap.rs:213`, re-exported
`catalog.rs:16`; every other hit is a comment). Reason (iii) is a true scope
argument: the arm is a §4.4 Class 1 `DestinationRecheckV1` extension, owned by a
phase, not a step. Reason (ii) — admission's `extra_children: 0` — is the weakest
of the three and I would not lean on it alone, since `barrier_slots` already
reserves a `RetiredRoamingAnchorAlias` slot and so was evidently designed against
that grammar; but it is not *wrong*, and it does not need to carry weight.

Freeze §4.3 row **E22** says what the step used, verbatim: "legacy Windows
durability anchor retirement | 4.2 | **P1 + P5** | n/a (`cfg(windows)`) |
`platform.rs:380-604`; random scratch at `:420` is the R2 stop-clause violation
this step removes | n/a | **no**". The implementation is exactly that. **The
divergence is correct.**

---

## 3. Compatibility and the crash window

### 3.1 Backward/forward compatibility — **the claim holds, and by construction**

The verdict-critical claim is "no legacy state can reach the new refusal" and "a
legacy binary meeting a new tree ignores `.ca1-anchor-retired-v1` entirely
(neither `ANCHOR_PREFIX` nor `ca1-`-prefixed)". Verified against
`git show 9c454ce:src/checked_artifact/platform.rs`, not against the report.

**The legacy classifier's exact prefix logic** (`9c454ce` `platform.rs:693-697`):

```rust
if text.starts_with(ANCHOR_PREFIX) {          // ".ca1-durability-anchor-"
    anchors.push(name);
} else if text.starts_with("ca1-") {          // NO leading dot
    family_state = true;
}
```

The family-state check matches `ca1-` **without** the leading dot. That is not an
accident: every family name is undotted (`authority.rs:227-239`,
`ca1-{family}-{action}.authority` / `.goal` / `.source`) and every staging name is
dotted and deliberately outside that grammar (`tests/leaf_publication.rs:62`,
"The staging name is dotted and outside the `ca1-` family grammar"). So
`.ca1-anchor-retired-v1` and `.ca1-anchor-scratch-v1` match neither branch, and a
legacy binary ignores both. **Claim verified.**

**Every other legacy-era check, walked:**

| Check | Scoping | Verdict on the new names |
| --- | --- | --- |
| `inspect_family` (`residue.rs:120-137`) — the private-area survey that gates `foreign` | filters on `family_prefix()` = `ca1-<family>-`, **undotted**, at `:127` before any classification | skipped; cannot set `foreign` |
| `finish()` cleanup walk (`cleanup.rs:77-174`) | removes only `residue.goal.name` / `residue.source.name` / `authority_name`, all from `inspect_family` | never sees them |
| Action-directory exactness (`extra_children: 0`, `provider/interior.rs:544/:620`) | a different directory class; the private area is not an action directory | not applicable |
| Preservation-image capture/decode (`git/gitbackend/preservation_image.rs:225`) | `raw_excluded_paths` prepends `CHECKED_ARTIFACT_PRIVATE_PATH = ".gwz/checked-artifacts"` and filters with `path_is_at_or_below` — a **subtree** exclusion, no name grammar | excluded by path, inherited |
| Dirty classification (`git/gitbackend/repository_support.rs:107-127`) | `checked_artifact_private_status_path` is `strip_prefix` + `/` — again path-scoped; the untracked comparison is `untracked > private_untracked`, so an extra private untracked file raises both sides equally | no behaviour change |

**A new binary on a legacy tree** (orphaned random scratch names resident): the
new `survey` (`anchor.rs:296-304`) tests `starts_with(ANCHOR_PREFIX)`, then
`== SCRATCH_NAME`, then `== RETIRED_NAME`, then `starts_with("ca1-")`.
`.ca1-anchor-scratch-<32hex>` matches **none** — it is dotted, so the last arm
misses it too. It therefore sets neither `scratch_present` nor `family_state`:
`establish` proceeds on `create_new` under its own name and succeeds. The orphan
is **tolerated, never reclaimed** — see [P3-1]. The brief's bar ("reclaims *or*
tolerates") is met; the residual is a record matter, not a break.

**A legacy binary on a new tree**: `.ca1-anchor-scratch-v1` and
`.ca1-anchor-retired-v1` are both invisible to `anchor_state`, so a legacy drive
allocates its own random scratch and proceeds. No refusal, no removal, no
capture. **Forward- and backward-inert, confirmed.**

### 3.2 The crash-window argument — **sound by inspection, unexecuted in evidence**

All three `NeedsRetireAlias` post-crash cases verify against the code:

- **(a) rename lost.** The directory still holds `{anchor, alias}` and no
  `.ca1-anchor-retired-v1`, so `survey` returns
  `NeedsRetireAlias { retirement_occupied: false }` and the drive retires again.
  The destination is the compile-time constant `RETIRED_NAME` (`anchor.rs:85`),
  so the re-drive is genuinely idempotent rather than merely convergent. ✓
- **(b) rename durable, flush lost.** The alias is gone, so `anchors` holds one
  entry and `survey` returns `Ready`; `retirement_occupied` is computed but
  unread on that path. Indistinguishable from (a)'s successor, exactly as
  claimed. ✓
- **(c) no dual-name state.** The publication is
  `publish_verified_leaf_no_replace` onto one deterministic destination; a rename
  is one metadata transaction and cannot leave two names, and the guard refuses
  before an occupied destination could be reached. ✓

**Bounded-at-one is typed** — `Err(error(code, label, "durability anchor
retirement slot is already occupied"))`, asserted at
`anchor/tests.rs:332-345` including "the refusal allocates no second retirement
name". **The exit path does not exist**, which is [P2-2]. Note the mitigations,
all real and none recorded: the state needs a *foreign hard link* (the protocol
renames and never links, so `NeedsRetireAlias` is not producible by the protocol
on any platform — the retirement arm is a foreign-interference reconciliation
everywhere, not only on macOS); E16's standard does admit a permanent typed
refusal for states off the supported table; and a native stash sweep of the
private area would incidentally clear it (`preservation_image.rs:353-355` says
the stash "may sweep its residue"). Any one of those, written down, discharges
[P2-2]; none is written down, and "outstanding" / "bounded at one" say the
opposite.

**The retired object as permanent evidence** conforms to the area's rule: the
E10/E14 annotation states "the anchor is permanent by design and `finish()` never
removes it", and nothing in `cleanup.rs`'s three removal paths can reach a name
outside `inspect_family`'s prefix. ✓

---

## 4. The zero-flips — both correct, both records fit to lift

**`barrier.*`: 0 of 16.** Sixteen keys confirmed at `fault_v1.rs:132-147`. They
genuinely name the frozen roaming-anchor protocol, not the legacy anchor (§2).
The three sufficiency reasons are independent and reason (i) is sufficient alone.
✓

**`terminal.*`: 0 of 11.** Eleven keys confirmed at `fault_v1.rs:189-199`:
`authority_reobserve`, `payload_reobserve`, `cleanup_reobserve`,
`reservation_reobserve`, `directory_flush`, `retired_slot_reserve`,
`action_directory_retire`, `retired_directory_reobserve`, `catalog_barrier`,
`terminal_revalidate`, `authority_release`. Every one is admitted-action- or
catalog-scoped; §4.4's arm table row assigns them to Phase 4 as "E7's Phase-4 half
and the terminal retirement edges", exactly as cited; and `recover_or_create` has
no production caller, so plan §5 item 2 holds. **The conditional resolves to no.**
✓

**Both records fit the house form** established by the `cleanup.*` non-activation
record at Step 4.1 (freeze `:614-668`): dated, "0 of N executed; all N stay
reserved", the vocabulary argument, the §5 items 1-2 citation, the carried-clause
re-reservation for R2-E, "RemPlan §10's duty … never attached", a "what the step
did convert and where its evidence is" section, and a "Counts" close. Arithmetic
recounted: census **165** (`fault_expected_keys.rs:174`, untouched — the file is
not in the dirty set); `barrier.*` 0/16 ✓; `terminal.*` 0/11 ✓;
`capability_permit.rs` caller inventory **13** (`:139`, untouched) ✓;
`CATALOG_PUBLICATION_CALL_COUNTS` untouched ✓; zero
`publish_verified_no_replace` sites added ✓ (the anchor uses
`publish_verified_leaf_no_replace`, the Step-4.1 twin). The blocks correctly avoid
the "row below" wording Step 4.1's [P3-3] flagged.

**One clause I would add before lifting** ([P3-2]): the record establishes the
nonce harm class in strong terms — "one per crash and unreclaimable" — and a
reader will take the private area to be nonce-free afterwards. It is not.
`authority::scratch_name` mints the same shape on the paths Step 4.1 just
converted. One sentence naming it and its closure lane (R2-E's slot vocabulary,
or R2-F legacy removal) keeps the record honest. Note this is *not* a trim
request on the anchor's own fix: the anchor's deterministic name is safe
precisely because its content is a compile-time constant, so concurrent drives
converge or fail closed; `authority::scratch_name`'s content is per-drive, and
Step 4.1's own review leaned on its randomness ("two concurrent drives cannot
collide", `GwzM5-8R2DStep41-Review.md:334-336`). Replacing it needs the reserved
slot vocabulary — the same R2-E wall.

---

## 5. The macOS hard-link discovery — **real, measured, correctly pinned**

`hard_links_share_durable_identity` (`anchor/tests.rs:253-262`) writes a file,
`std::fs::hard_link`s it, and compares `verify(a) == verify(b)`. It **measures**;
it reads no `cfg`. ✓ The branch pin
`hard_link_identity_sharing_is_what_the_retirement_rows_assume` (`:268-284`)
calls it and asserts `!shared` on macOS and `shared` elsewhere, so a green run is
evidence of *which* branch the retirement rows took. ✓

The consequence follows from the identity semantics: `NeedsRetireAlias` requires
`final_identity == alias_identity` (`anchor.rs:344-346`), and on macOS
`ATTR_CMN_OBJPERMANENTID` is per hard link, so the two names never carry one
identity and the state — hence the legacy `remove_file` this step replaces — was
always unreachable there. ✓ The rows behave as claimed: `strand_alias`
(`:218-236`) returns `None` on macOS and both retirement rows then assert
`!hard_links_share_durable_identity()` before returning, so they are *guarded*,
not silently vacuous. `a_foreign_link_that_re_homes_the_anchor_is_refused_without_mutation`
covers the refusal side and asserts `names(&root.0) == before` — a refusal
mutates nothing. ✓

Confirmed locally: on this macOS host both retirement rows take the skip branch,
so the retirement arm executed **only** on the Windows probe. That is disclosed
and the pin makes it legible. The broader fact the report does not state — that
the state needs a foreign hard link on *every* platform, not only macOS — is
folded into [P2-2].

---

## 6. The split — **clean; carriers true without edits**

Moved/stayed table recounted against `git diff` and the new files. Moved out:
`ANCHOR_BYTES`, `ANCHOR_PREFIX`, `AnchorState`, `anchor_state`→`survey`,
`anchor_roundtrip_name`→`roundtrip_name`, `verify_anchor`→`verify`,
`anchor_name`, the Windows `prepare_private` body→`prepare`, the
`AnchoredPrivateArea` half of `private_barrier`→`round_trip`, and the anchor-name
test→`the_outbound_alias_name_is_derived_natively`. ✓ Stayed: the P1 pair,
`LeafPublicationSourceV1` (`:169`) + `publish_verified_leaf_no_replace` (`:199`),
`open_dir_share_delete`, `windows_destination_path`, `DirentBarrierClass`
(`:364`), `sync_parent`, `rename_relative`, the `ExactInterior` no-op arm
(`:526`) **with its full E10/E14 comment intact**, and the P1 `windows_tests`
module. ✓ (`hex` also stayed, with its cfg widened to `any(windows, test)`; the
report's table omits it — immaterial.)

The arm's comment is intact and *extended* by five lines recording that the
machinery it declines relocated and that nothing about its argument moves. That
is an improvement, not a breach of "intact".

`mod anchor;` is declared `#[cfg(any(windows, test))]` at `platform.rs:9-10` with
no `#[path]`, so **`APPROVED_RUST_PATH_EDGES` is unchanged** — confirmed: the
checker diff touches only the allowlist and a comment block. ✓ The cfg is
load-bearing, not decorative: the six new `CheckedArtifactFault` variants carry
the same `cfg(any(windows, test))`, so a non-test non-Windows build would not
compile the module's `fault()` calls if the module were unconditional.

**Carriers verified true without edits.** `authority_record_binding.rs` cites
`platform::private_barrier` and `DirentBarrierClass::ExactInterior` **by name**
(`:39`), never by line, and both stayed in `platform.rs`. The E10/E14
annotation's *subject* — which arm a caller takes on Windows — is unchanged:
`ExactInterior` still takes the no-op, `host.rs` and `namespace_mutation.rs:326`
still pin the class. ✓ (The annotation's *line* citations drift — [P3-4].)

**The seven `AnchoredPrivateArea` call sites are byte-identical.** `cleanup.rs`
`:89/:134/:169`, `residue.rs` `:415/:530/:564`, `transition.rs` `:453` — 3+3+1=7.
`cleanup.rs` is not in the dirty set at all; the `residue.rs` and `transition.rs`
diffs are confined to hunks at `:391`/`:506` and `:283`/`:384`, nowhere near the
barrier calls. ✓ (`residue.rs:102`'s `prepare_private` call site is likewise
untouched.)

Cohesion: 1,093 → 884, under the 1,000-line trigger. The Step-4.1 [P3-1] docket
(twin-seam unification at R2-F) is untouched. ✓

---

## 7. The nonce's harm and its removal

**Zero random names in the anchor module** — confirmed by reading, not by the
report: `anchor.rs` contains no `getrandom` and no `remove_file`. The new seam
pin `the_durability_anchor_protocol_is_closed_and_allocates_no_retry_name`
(`capability_permit.rs:206-245`) asserts both as *call forms* (`getrandom::`,
`.remove_file(`) so the module's own prose naming them does not falsify the pin —
a good detail. It also pins both constants, pins `mod anchor;` and
`!platform.contains("enum AnchorState")` in `platform.rs`, and asserts exactly one
`publish_verified_leaf_no_replace(` call site in the anchor. ✓

**The deterministic staging name, resumed onto**, with the house `create_new`-vs-
`truncate` shape (`anchor.rs:216-251`): a fresh drive keeps the legacy
`create_new(true)` fail-closed open, and only a resume opens for truncation. The
claim "scratch resident was unreachable under the random name" is true — the
legacy scratch name was freshly allocated per attempt and the legacy classifier
could not see a prior one — so the truncate branch replaces no legacy *gwz*
behaviour. See [P3-5](a) for the foreign-object case it does change.

**The 12-round repeated-boundary row** (`anchor/tests.rs:155-175`) drives
`AfterAnchorScratchFlush` twelve times and asserts
`names(&root.0) == BTreeSet::from([SCRATCH_NAME.to_owned()])` after **each**
round — exactly `{.ca1-anchor-scratch-v1}`, nothing else — then converges. ✓ This
is the sharpest single piece of evidence in the package.

---

## 8. Probe evidence

Both runs confirmed via `gh run view --json`, not from the report:

| Run | conclusion | branch |
| --- | --- | --- |
| 32608697879 | `success` | `probe/anchor-retirement-win` |
| 32609244340 | `success` | `probe/anchor-retirement-win` |

Final run's numbers read from `gh run view 32609244340 --log`:
**16 + 27 + 10 + 4 + 4 + 14 = 75 passed, 0 failed**, `probe slice status=0`. ✓

The six trimmed filters, read from the workflow on the probe branch:
`checked_artifact::platform` (the retired paths and the P1 Windows tests) ·
`checked_artifact::tests` (the legacy suites that drive the seven
`AnchoredPrivateArea` call sites) ·
`checked_artifact::interface_tests::capability_permit` (the seam pins) ·
`checked_artifact::namespace::tests_fault_matrix` and `::tests_managed_matrix`
(both namespace matrices, i.e. the E10/E14 `ExactInterior` arm) ·
`checked_artifact::capability::pre_catalog::provider::tests_authority_record`
(the authority suite). **Coverage as claimed.** ✓

**Scope of the two pushes.** `git ls-remote` confirms `refs/heads/main` is still
`9c454ce56527ae15b171012dc9066586ee1db360` — the review base, unmoved — and
`refs/heads/probe/anchor-retirement-win` is `ba1f185`. No tag names the probe or
the anchor. The branch carries exactly two commits over main; the second
(`ba1f185`, "PROBE ONLY: pin the hard-link durable-identity fact the retirement
rows branch on") touches **one file, `platform/anchor/tests.rs`, +25/−4** —
precisely the disclosed identity pin. The branch's diff vs main is the 12 step
files plus the workflow trim, nothing else, and the trim is annotated "Never
merge this trim to main." **Disclosure accurate; scope stayed probe-branch-only.**
✓

I additionally verified the probe tested *this* object: the working-tree copies
of `anchor.rs`, `anchor/tests.rs`, `platform.rs`, `fault.rs`,
`capability_permit.rs` and the checker are **byte-identical** to `ba1f185`'s. The
green run is therefore evidence about the object under review, not about a
divergent branch.

---

## 9. [P3-4] discharge, checker, and pins

### 9.1 [P3-4] — exact

Four per-edge variants replace one (`fault.rs`), with the edge named in each
doc comment. Mapping recounted against freeze §4.3: `BeforeAuthorityPublication`
→ E21 `residue.rs:506` ✓ · `BeforeGoalPublication` → E20 `residue.rs:391` ✓ ·
`BeforeDetachPublication` → E18 `transition.rs:283` ✓ · `BeforeManagedPublication`
→ E19 `transition.rs:384` ✓.

Matrices 3 → 9 rows, each flow carrying exactly the edges it crosses:
`recovery_protocol` (replace-over-existing) **4** ✓ · `removal_recovery`
(removal — stages and publishes no goal) **2** ✓ · `staging_recovery`
(missing-source — no source to detach) **3** ✓. All three green in the focused
run. The rationale is stated in-comment at each site.

### 9.2 Checker

`python3.13 scripts/checks/check_checked_artifact_boundaries.py` →
**exactly three findings**, all pin moves, all attributed:

```
checked-artifact boundary: failed
- protected source allowlist changed: checked_artifact/platform.rs
- protected source allowlist changed: checked_artifact/residue.rs
- protected source allowlist changed: checked_artifact/transition.rs
```

Every reported digest recomputed and **matches**:
`platform.rs` `3523ac03…a3b8d7` ✓ · `transition.rs` `13b483bc…9b37ca` ✓ ·
`residue.rs` `d48aacb8…c33b20251` ✓ · `platform/anchor.rs` (flat)
`5aa99fe0…f836bdb15a` ✓.

Allowlist `platform.rs: rename_relative 5 → 1` verified **by the checker's own
masked bare-identifier rule** (definitions excluded, comments masked):
`platform.rs` names `rename_relative` at `:94` (a call), `:178`/`:193`
(comments, masked) and `:387/:413/:435/:456` (four `pub(super) fn` definitions,
excluded) → **one** counted reference, and `:94` sits inside
`#[cfg(not(windows))] pub(super) fn rename_open_source` — the ONE survivor is
exactly "`rename_open_source`'s own non-Windows delegation". ✓ No other file in
the subsystem names it in production. `CATALOG_PUBLICATION_CALL_COUNTS`, the
permit's **13**, and `APPROVED_RUST_PATH_EDGES` are all untouched. ✓

### 9.3 Pin-shape opinion for the lane owner — **convert `platform.rs` to a TREE pin**

Value, computed with the checker's own `source_tree_digest`:

```python
PROTECTED_SOURCE_TREE_DIGESTS = {
    ...
    "checked_artifact/platform.rs": "2572702e5633bc701fa5068973ef92f67e8862e4fcfc2552c24b55751bb2ea1d",
}
```
with the flat `"checked_artifact/platform.rs"` entry removed. Reasons, in order:

1. **A flat `platform/anchor.rs` entry leaves `platform/anchor/tests.rs`
   unpinned** — 433 lines, and the entirety of this step's behaviour evidence.
   The flat pin worked at `platform.rs` only because its tests lived *inside* the
   pinned file; the split broke that property, and a flat entry restores half of
   it.
2. **The mechanism already exists for exactly this shape.**
   `source_tree_digest` derives the descendant root as `root_file.with_suffix("")`
   (`:788-789`), i.e. `platform/`, and `rglob`s it — the same root-file-plus-child-
   directory shape `catalog.rs`, `capability/pre_catalog.rs`, `capability/path.rs`
   and `bootstrap/runtime/catalog_lease.rs` already use.
3. **No strength is lost.** The tree digest hashes path *and* content
   (`digest.update(content)`, `:801`), so it is not weaker than the flat sha256.
4. **It is a manifest move, not a checker-logic change** — both dicts are already
   implemented and verified in the same loop (`:845-851`). The cost is one deleted
   line and one added line; the finding text changes from "protected source
   allowlist changed" to "protected source tree changed", settle-visible either
   way as the brief notes.
5. **It future-proofs the file-ownership clause.** Plan `:456-457` anticipates
   "new owners … `checked_artifact/namespace/backend/` (with OS children if
   platform code splits)"; a tree pin covers the next platform child without
   another pin edit, where flat pins accrue one entry per split.

---

## 10. Gates

### 10.1 Re-run by this review, in the live tree

All with `CARGO_TARGET_DIR=…/scratchpad/p41-target`:

- `cargo check --all-targets -p gwz-core` → `Finished` **zero diagnostics** ✓
  (re-verified after `touch`ing `anchor.rs`, so the warm target dir is not
  masking a stale build)
- `cargo test --lib -p gwz-core checked_artifact::` →
  `test result: ok. **389 passed; 0 failed**; 0 ignored; 0 measured; 1170 filtered
  out; finished in 35.86s` ✓ — the claimed 389/0 exactly, and the three expanded
  matrices are named green inside it
- `cargo test --lib -p gwz-core platform::anchor` →
  `test result: ok. **13 passed; 0 failed**; 1546 filtered out` ✓
- `CLIPPY_CONF_DIR="$PWD" cargo clippy -p gwz-core --all-targets --all-features
  -- -D warnings` → `Finished`, **zero diagnostics** ✓
- `rustfmt --edition 2024 --check` over all **eleven** touched Rust files → clean ✓
- `python3.13 scripts/checks/check_checked_artifact_boundaries.py` → three
  attributed findings only ✓
- **L2-04** (`retained-readers.yml` `harness` job) re-run locally end to end:
  `validated 7 migration rules and 7 runtime bindings` · `Ran 14 tests … OK` ·
  `Ran 86 tests in 150.050s … OK` · `{"status": "valid", "tuple_count": 24}` ·
  `{"status": "manifest-ready", "tuple_count": 24}` ✓. The network `matrix` job
  is correctly left to CI: it `needs: harness` and its `push`/`pull_request` path
  filters include `scripts/checks/**`, which this step touches, so it fires
  without a dispatch. ✓
- Probe run 32609244340 → `success`, 75/0, `probe slice status=0` ✓

### 10.2 Full lib — **not re-run, and no cause to**

The asserted 1558/0/1 is **independently corroborated without the 703s run**: the
anchor run reports `13 passed; 1546 filtered out` → 1,559 total, and the focused
run reports `389 passed; 1170 filtered out` → 1,559 total. 1558 passed + 1 ignored
= 1,559. Both agree with the claimed baseline arithmetic 1544 → +14 (13 anchor
rows + 1 seam pin). ✓

### 10.3 Budget — **record, do not trim**

Recounted: tracked diff **+148 / −268** over **10** files; new files 382 + 433 =
815; gross **+963**, net **+695**. Split: relocated ≈ **211**, new production ≈
**243**, new tests ≈ **505** (anchor suite 433 − 7 relocated = 426, seam pin 51,
matrices + `leaf_publication` net 28 → 505 ✓; 211 + 243 + 505 = 959 ≈ 963 within
rounding ✓). Net new ≈ **748** against the aspirational <500.

**My load-bearing judgment: record it, exactly as [P3-6] was recorded at Step
4.1.** New production logic is 243, comfortably inside the target; the 211
relocated lines are a *bound obligation* the lane owner attached to this step, not
new logic; and the 505 test lines are the whole of the behaviour evidence for a
`cfg(windows)` object that no local gate would otherwise touch — trimming them
would trade the only thing making this step reviewable for a number. The one thing
I would put in front of the lane owner: this is the **second consecutive Phase-4
step over the aspirational budget** (4.1 at 507 gross, 4.2 at 748 net-new), with
one step left in the phase. That is a ChangeBudget-row observation, not a finding.

---

## 11. §9 escalation

Freeze §9 (:1400-1406) makes Phase 4 steps single-axis "**with automatic
escalation on any P0/P1/P2**". This review returns **two P2s**, so the trigger
**is hit** and is recorded here. Routing — a second axis, a remediation round on
the two P2s alone, or acceptance with the record amended — is the lane owner's
call, not this review's.

What I would want discharged before the §3.5 blocks are lifted, in order:

1. **[P2-1]** — one fault row at the retirement boundaries (it will guard-skip on
   macOS with the same `strand_alias` guard the two retirement rows already use,
   and execute on the probe), **or** a named exclusion in the form Step 3.2 used
   for its "four named single-crossing exclusions". And correct "10-boundary
   matrix" to eight in the report the settle reads.
2. **[P2-2]** — the E16-form derivation for the permanent refusal (producing
   condition is foreign hard-linking, outside the accepted same-user namespace
   boundary; state not producible by the protocol on any platform), plus either a
   named operator escape or an explicit "none, by design". Drop "outstanding".
3. **[P3-2]** — one clause naming the surviving `authority::scratch_name` nonce
   and its closure lane.

Round 1 of 2. No third architectural root cause is in play: neither P2 asks for a
redesign, and both are dischargeable inside the existing object.

---
---

# Round 2 — focused re-verdict on the remediation

Date: 2026-08-23. Same axis (**single-axis Code-lead**), same peer-blind
protocol: the round-2 report was read as claims to verify, every count recounted
and every affordable gate re-run in the live tree over `9c454ce`. Round **2 of
2**; the two-round cap is now spent.

Object recounted: **11 tracked-modified + 2 new = 13 files** (`authority.rs`
joins the tracked set). `origin/main` is still `9c454ce`. Working tree
byte-identical to probe head `ad104d1` for every file I sampled
(`anchor.rs`, `anchor/tests.rs`, `authority.rs`, `residue.rs`,
`leaf_publication.rs`).

## R2.1 Verdict

**GO. Escalation discharged.**

Both P2s are closed on the code, not on the record: the retirement's two
boundaries are now driven, on the platform the anchor serves, and the wedge is
removed mechanically rather than derived away. Three of the five P3s are closed
outright, one is closed and one half-closed with its residue correctly assigned
to the lane owner. Round 2 introduces **no P0/P1/P2**, so the §9 trigger fired in
round 1 is **discharged rather than re-fired**, and no third architectural root
cause appeared against the two-round cap. Two new P3s file and continue.

| Round-1 finding | Round-2 grade |
| --- | --- |
| **[P2-1]** retirement boundaries driven by nothing | **CLOSED** |
| **[P2-2]** permanent wedge on a reachable state | **CLOSED** |
| [P3-1] legacy nonce orphans tolerated, never reclaimed | **CLOSED** (recorded in the block, pinned by a row) |
| [P3-2] surviving `authority::scratch_name` nonce | **CLOSED** (fixed, not routed) |
| [P3-3] §4.4's Phase-4 arm row unresolved | **CLOSED** (resolved in the block, E16/E17 mechanism) |
| [P3-4] stale freeze citations + file miscount | **HALF CLOSED** — miscount fixed; citation drift **OPEN**, lane owner's |
| [P3-5] two undeclared behaviour deltas | **CLOSED** (declared, argued, and mechanically backstopped) |
| **[P3-6]** *(new)* report prose overstates the executed crash windows | open, record-only |
| **[P3-7]** *(new)* `survey`'s `Invalid` is still a permanent no-exit refusal; "no branch can wedge" is too broad | open, record-only |

## R2.2 [P2-1] — **CLOSED**

`BOUNDARIES` now carries all ten (`anchor/tests.rs:33-44`), and
`RETIREMENT_BOUNDARIES` (`:48-51`) names the two that only exist inside the
stranded-alias state. The matrix builds that state with `strand_alias` before
arming (`:154-164`), and on a platform that cannot hold it, asserts
`!hard_links_share_durable_identity()` before skipping — so the skip cannot go
silent.

**The rows crash AT the boundaries, not near them.** Verified in the harness
rather than inferred: `fail_next_checked_artifact_at` stores `(boundary, None)`
and `fault()` fires only on `at == boundary` (`fault.rs:96-97`, `:129-130`) — it
is boundary-*targeted*, not "next crossing". So for
`BeforeAnchorAliasRetirement` the drive reaches the arm, passes both `verify`
calls (which cross no boundary of their own) and dies at `anchor.rs:148`; for
`AfterAnchorAliasRetirement` the publication lands first and the drive dies at
`:154`. Both then run `assert_closed_grammar`, a converging `prepare` +
`round_trip`, a byte check on the resident anchor, and `assert_closed_grammar`
again.

**Executed natively.** Probe `32612243125`, `checked_artifact::platform` →
**19 passed / 0 failed**, with
`hard_link_identity_sharing_is_what_the_retirement_rows_assume` green in its
non-macOS branch — which is the machine-checked proof that the rows took the
retirement path and not the skip. Worth noting the *red* run `32611909974` also
shows `platform` at 19/0: the retirement evidence was already green before the
fixture fix, so it does not depend on it.

## R2.3 [P2-2] — **CLOSED**, and the design blessed

**The refusal is gone.** Read, not taken on report: the `NeedsRetireAlias` arm
(`anchor.rs:140-161`) is `verify(final)` → `verify(alias)` → compute
`retired_name(ordinal)` → fault → publish → fault → `verify(retired)` →
`round_trip`. No guard, no `retirement_occupied`, no early `Err`. Every `Err`
reachable inside it is a fault injection or a foreign-substitution refusal that
predates the step.

**Smallest-free vs count vs max+1 — their argument is correct, and I verified
the fixed point rather than accepting it.** `smallest_free_ordinal`
(`:371-382`) sorts, dedups, and walks from 0: `{}`→0, `{1,2}`→0, `{0,2}`→1,
`{0,1,2}`→3. Against a resident `{0,2}`:

- **count** = 2, and 2 ∈ `{0,2}`, so the no-replace publication can never
  succeed and every retry recomputes 2. That is a genuine fixed point — a fresh
  permanent wedge *inside the fix*. The argument is sound.
- **max+1** = 3 never collides, so it is safe; its only cost is that the gap is
  never reused. That cost is small (a gap can only arise from a foreign
  deletion), so max+1 was not *wrong* — but it is strictly less parsimonious.
- **smallest-free** is by construction not a resident name, so every drive has a
  free destination, and if a racing drive takes it the no-replace publication
  refuses and the next survey observes the new state and picks again. No fixed
  point exists.

**Blessed.** `the_retirement_ordinal_is_the_smallest_free_one_observed`
(`:411-429`) plants exactly the `{0,2}` gap and requires the fill at 1, so the
distinguishing case is pinned rather than argued.

**The no-cap ruling: blessed.** A cap restores a wedge at the cap, which is the
defect being fixed; each retired object costs one foreign stranding *interrupted
by a crash*, so the count is bounded by real occurrences and not by anything the
protocol can inflate; and the scan is linear in the directory by pigeonhole
(`free ≤ observed.len()`), so it needs no cap for cost either. I looked for a
u32 overflow on `free += 1` — it needs ~4×10⁹ resident entries and is
unreachable.

**Malformed retired names: refused, not adopted.** `survey` (`:300-306`)
`strip_prefix(RETIRED_PREFIX)` then `parse::<u32>()`, and a parse failure returns
`Invalid`. `a_malformed_retired_name_is_refused_not_adopted` (`:433-443`) plants
`.ca1-anchor-retired-v1` — round 1's own name — and requires the refusal. Good
choice of fixture.

**Crash windows.** Executed: *before the edge* (`:378-389`, ordinals stay `{0}`,
next drive converges to `{0,1}`), *after the edge before the reobservation*
(`:394-405`, ordinals advance to `{0,1,2}` immediately and the next drive
settles), and *recurrence* (three strandings converge onto `{0,1,2}` with the
barrier working after each). The *racing-drive* window is argued only — see
[P3-6].

**Compatibility of the new destination family re-checked.**
`.ca1-anchor-retired-<n>` is dotted, so the legacy classifier's undotted
`starts_with("ca1-")` still misses it and a legacy binary ignores it exactly as
it ignored `-v1`; `inspect_family`'s `family_prefix` is still
`format!("ca1-{family}-")` (`authority.rs:230-232`, unchanged), so the family
survey still skips it; and the preservation-image and dirty-status exclusions are
path-scoped, so nothing there moves. Round 1's §3.1 walk holds unchanged.

## R2.4 [P3-2] — **CLOSED**

`scratch_name(family, action, kind) -> String` =
`.ca1-{family}-{action}-{kind}.scratch` (`authority.rs:266-268`), infallible, and
**`getrandom` is gone from `authority.rs`** — the only remaining occurrences in
the crate are `platform.rs:594/:641` (Windows test fixtures, not names),
`catalog/bootstrap.rs:304` (the catalog bootstrap scratch, in `.gwz/` rather than
the private area, a different owner with no production caller) and
`workspace_ops/handle_commit.rs:300`. The block's scope — *the private area* — is
therefore accurate.

- **Derived from observed durable state**: family key and action key, the same
  inputs `authority_name`/`goal_name`/`source_name` already take. ✓
- **Action-scoped** ✓. The collision claim checks out in substance: under the
  nonce, two same-family+action drives published two goal aliases and
  `inspect_family` returned `foreign` (`residue.rs:181-188`) — a typed refusal.
  Under determinism they share one staging file, both write **identical** bytes
  (same family + same action means same expected fact and same goal), one wins
  the no-replace publication and the other gets a typed refusal; any interleaving
  is caught because the sealed publication carries `bytes: goal` and re-verifies
  through the renamed handle (`residue.rs:392-402`, verified at both edges). Safe
  — and arguably better than `foreign`, which poisons the family. See [P3-7] for
  the one phrase that overstates this.
- **Dotted** ✓ — `family_prefix` is untouched, so **no older-binary
  classification change**, confirmed by reading rather than by the pin.
- **Resume-aware opener** ✓ — `staging_options` (`residue.rs:530-567`) probes
  with `symlink_metadata` (nofollow, so a planted symlink counts as resident and
  then fails the `follow(FollowSymlinks::No)` open — fail-closed), `truncate` on
  resident, `create_new` fresh. Both edges take it; `staging_options` has exactly
  2 call sites, as the seam pin asserts.

**The three rows and the seam pin verified.**
`repeated_crashes_reuse_one_deterministic_authority_staging_name` and
`…_goal_staging_name` each drive twelve rounds and assert one staging entry, the
**same** name every round, and that the resumed publication consumes it;
`a_short_staging_file_from_a_crashed_write_is_rewritten_in_place` covers the
create/write window. The seam pin
`the_family_staging_name_is_derived_and_allocates_no_retry_name`
(`capability_permit.rs:259-281`) pins the absence of `getrandom::`, the exact
signature, the exact `format!`, `family_prefix`'s survival, and both
`staging_options` call sites. The red/green claim (nonce revert → 3/5 failed) is
consistent with the suite's shape and I did not need to reproduce it: all 8
`leaf_publication` rows are green here and the three new ones are name-sensitive
by construction.

## R2.5 The remaining P3s, and the probe

**[P3-1] and [P3-3] are fit to lift.** [P3-1] is stated in the block *and* in
`assert_closed_grammar`'s own doc comment — "the grammar is closed *forward*, not
retroactively" — which is the right place for it, since that assertion is what
the residual falsifies on an upgraded tree; the row
`legacy_nonce_orphans_are_tolerated_and_block_nothing` plants a 32-hex orphan and
requires the protocol to work around it and leave it alone. [P3-3] uses the E16/E17
conditional-resolution mechanism correctly: the retirement destination is a
regular file through P1 with no destination interior re-check, so
`DestinationRecheckV1` is unchanged and there is no arm to add; the row is left as
written and driven by the terminal edges alone. Both blocks are in house form.

**[P3-5] is adequate**, and better than declared: I verified the mechanical
backstop the argument leans on. The truncate-over-foreign case cannot promote
foreign content because the resume writes our own bytes, takes identity from its
own write handle, and the sealed publication re-verifies identity **and bytes**
through the handle it renames — present at the anchor edge (`anchor.rs:266-277`)
and at both family edges. The `NeedsReturn` pre-verify is reject-only, as stated.

**[P3-4] half closes.** The file count is corrected and now recounts correctly
(11 tracked + 2 new = 13). The freeze citation drift is unchanged and remains the
lane owner's: §4.1's P5 row (`platform.rs:427/:430/:433/:551-584/:607/:665/:672`
→ `platform/anchor.rs`) and the E10/E14 annotation's `:514/:281/:294` →
`:526/:364/:377`. `platform.rs` did not change in round 2, so neither did the
drift.

**Probe-run-1 attribution: confirmed from the log, not the fixture diff.**
`gh run view 32611909974 --log-failed` shows exactly three failures — the three
new `leaf_publication` staging rows — with `platform` 19/0, `capability_permit`
11/0, both namespace matrices 4/0 and the authority suite 14/0 all green
alongside. Fixture-side, in the new rows only, exactly as claimed; the in-code
comment on `staging_entries` records the cause (`.ca1-` prefix scan counting the
resident durability anchor).

**Disclosure and scope: confirmed.** Four commits on
`probe/anchor-retirement-win`, two new this round — `27a39f2` (remediation) and
`ad104d1` (fixture fix) — with the branch head equal to the remote head.
`origin/main` is untouched at `9c454ce`; 29 remote tags, **none** matching
anchor/probe/retire/4.2. Never main, never a tag. Final probe `32612243125`:
19 + 30 + 11 + 4 + 4 + 14 = **82 passed / 0 failed**, `probe slice status=0`, six
filters, with all six named retirement/ordinal/orphan rows green in the log.

## R2.6 New findings

**[P3-6] The report overstates executed evidence, for the second time.** It lists
"Crash windows, all four now executed rather than asserted" and includes
*"Racing drive takes the ordinal first"*. There is no such row —
`grep -n 'racing\|race\|concurrent'` over `anchor/tests.rs` returns nothing.
Three windows are executed, plus the `{0,2}` gap row; the racing case is argued.
The argument is sound and the row is cheap (~15 lines:
`run_next_checked_artifact_at(BeforeAnchorAliasRetirement, hook)` planting
`retired_name(0)`, then re-driving and requiring ordinal 1). **The liftable block
does not carry the error** — it names only the two rows that exist — so this is
the round-1 [P3-2] situation from Step 4.1: the report's prose carries it, and
the settle reads the report. Correct the prose.

**[P3-7] `survey`'s `Invalid` is still a permanent, no-exit refusal, and round 2
widens it by one name shape.** A foreign `.ca1-anchor-retired-<non-numeric>`
sends `survey` to `Invalid`, so every `prepare` and `private_barrier` on that
private area fails for ever with no in-code exit — structurally the same shape as
the wedge [P2-2] rejected. Two things make it a P3 and not a re-fire:

1. It is the **pre-existing** posture, not a new class. `Invalid` already meant a
   permanent refusal at `9c454ce` for a foreign `.ca1-durability-anchor-<junk>`,
   for more than two anchors, and for duplicate final/roundtrip entries. Round 2
   adds one arm to a set of four.
2. Its trigger is **foreign contamination of the reserved grammar only**, inside
   the amendment's cooperating-same-user boundary. [P2-2]'s trigger was the
   protocol's *own* durable artifact — the protocol wedged itself on a retirement
   it had created. That distinction is the whole of the fix, and the fix makes it.

What needs correcting is one sentence of prose, not the design: the report says
"**The refusal is gone** — there is no branch that can wedge", which is false.
The liftable block says "Every reachable post-crash state has a typed exit",
which is **true and precise** — I checked every crash window and none produces
`Invalid`. Keep the block's wording; fix the report's. Folded in here: the block's
"now it refuses earlier, before writing" for the same-family+action collision is
also imprecise — the two drives share one staging file and the refusal arrives at
the no-replace publication or its byte re-verify, not before the write. The
outcome is safe either way (identical inputs, identical bytes, byte re-verify as
backstop), so this is wording, not behaviour.

## R2.7 Gates, re-run in the live tree

`CARGO_TARGET_DIR=…/scratchpad/p41-target` throughout.

| Gate | Result |
| --- | --- |
| `cargo check --all-targets -p gwz-core` | `Finished … in 8.07s`, **zero diagnostics** ✓ |
| `cargo test --lib checked_artifact::` | `ok. **396 passed; 0 failed**; 1170 filtered out; 36.20s` ✓ (389 + 7) |
| `platform::anchor` | `ok. **16 passed; 0 failed**; 1550 filtered out` ✓ |
| `tests::leaf_publication` | `ok. **8 passed; 0 failed**; 1558 filtered out` ✓ |
| `interface_tests::capability_permit` | `ok. **11 passed; 0 failed**; 1555 filtered out` ✓ |
| clippy `--all-targets --all-features -- -D warnings` | `Finished`, **zero diagnostics** ✓ (warm dir, but gwz-core recompiled, so its own lints did run) |
| `rustfmt --edition 2024 --check`, all 13 touched Rust files | clean ✓ |
| `python3.13 check_checked_artifact_boundaries.py` | **three findings**, the same three pin moves ✓ |
| **L2-04** harness, re-run end to end | `validated 7 migration rules and 7 runtime bindings` · `Ran 14 tests … OK` · `Ran 86 tests in 150.332s … OK` · `{"status": "valid", "tuple_count": 24}` · `{"status": "manifest-ready", "tuple_count": 24}` ✓ |
| Probe `32612243125` | `success`, 82/0, `probe slice status=0` ✓ |

**Full lib not re-run, and no cause to.** The claimed 1565/0/1 is corroborated
four independent ways from the filtered counts above: 396+1170 = 16+1550 =
8+1558 = 11+1555 = **1566**, and 1565 passed + 1 ignored = 1566. It also matches
round 1's 1559 + 7.

**Digests all recomputed and matching**: `platform.rs`
`3523ac03…a3b8d7` (unmoved from round 1 — `platform.rs` did not change) ·
`residue.rs` `717c2417…80150` · `transition.rs` `13b483bc…9b37ca` ·
`authority.rs` `fd300c5b…bdf1ed` · `platform/anchor.rs` (flat)
`86139c89…9af660`.

## R2.8 Pin opinions — both blessed

**(a) Convert `checked_artifact/platform.rs` to a `PROTECTED_SOURCE_TREE_DIGESTS`
entry — BLESSED, and the case is stronger than in round 1.** Updated value,
computed with the checker's own `source_tree_digest`:

```python
"checked_artifact/platform.rs": "e46a69cecce9b0500a88a9421af185cc811393fea0192c993773395f7036c0cd",
```

with the flat entry removed. Round 1's five reasons all stand; round 2 sharpens
the first: the descendant tree grew from 815 lines to **961** (`anchor.rs`
382→413, `anchor/tests.rs` 433→548), so a flat `platform/anchor.rs` pin would now
leave **548 unpinned lines** — the entirety of both P2 closures' evidence,
including the ordinal recurrence row and the malformed-name row that pin the
[P2-2] fix. `source_tree_digest` derives the descendant root as
`platform/` and hashes path **and** content, so nothing is lost.

**(b) Flat-pin `checked_artifact/authority.rs` — BLESSED, with one refinement.**
Verified independently: it appears in neither `PROTECTED_SOURCE_DIGESTS` nor any
`PROTECTED_SOURCE_TREE_DIGESTS` root today. The case is good and it is [P3-2]
that makes it: `authority.rs` is now the single naming authority for every
private-family name — `authority_name`, `goal_name`, `source_name`,
`family_prefix` **and** `scratch_name` — and the whole correctness of the
staging-name fix (dotted, action-scoped, derived from durable state) is one
`format!` in that file. An undetected edit there silently changes what
`inspect_family` can see, which is precisely the drift class the pin table
exists for. Digest confirmed: `fd300c5b8fb9dfacd41a4f0c6c39923fc8decbb07a6933af2eaa471c4ebdf1ed`.

*Refinement:* make it a **flat** entry, not a tree entry. There is no
`src/checked_artifact/authority/` directory, so a tree pin would degrade to the
same single file while implying a descendant surface that does not exist; the
flat dict is the correct home. Both (a) and (b) are manifest-only moves — no
checker logic changes for either.

## R2.9 Budget

Recounted: **13 files — 11 tracked (+405 / −305) + 2 new (413 + 548 = 961)**;
gross **+1366**, net **+1061**. Composition across both rounds: production ≈ 300
(~28%), bound relocation ≈ 211 (~20%), evidence ≈ 550 (~52%).

**Judgment: record, do not trim** — and more firmly than in round 1. Nearly all
of round 2's +366 net is this review's own asks: the ten-boundary matrix and the
retirement rows ([P2-1]), the ordinal recurrence and gap and malformed rows
([P2-2]), the [P3-2] fix with its three rows, and two seam pins. A review that
demands executed evidence cannot then bill that evidence against the
implementer's budget.

For the lane owner's ChangeBudget row, two observations rather than findings.
First, this is the **second consecutive over-budget Phase-4 step** (4.1 at 507
gross, 4.2 at 1,061 net-new), with Step 4.3 still to come. Second — and more
useful than the total — the **mass composition** is what the row should carry:
barely a quarter of this step is production logic, and the evidence majority is
the direct product of the dual-gate posture at Phase 4 plus a `cfg(windows)`
object that no local gate would otherwise reach. If the aspirational <500 is
meant to bound *production* change, this step is inside it at ≈300; if it is
meant to bound total diff, then it is the wrong instrument for Windows-only
durability work and the row should say so.

## R2.10 Escalation status

Round 1 fired §9's automatic escalation on two P2s. Round 2 returns **no P0, P1
or P2**, both round-1 P2s are closed on the code, and the two new findings are
P3s that "file and continue". The trigger is therefore **discharged, not
re-fired**; whether any escalated routing already initiated should now be stood
down is the lane owner's call, as is the disposition of the three record items
below.

Lane-owner items carried out of this review, none blocking:

1. **[P3-6]** — correct "all four executed" to three-plus-the-gap-row in the
   report the settle reads, or add the ~15-line racing row.
2. **[P3-7]** — correct "there is no branch that can wedge" to the block's own
   precise "every reachable post-crash state has a typed exit", and soften "it
   refuses earlier, before writing".
3. **[P3-4] residue** — the §4.1 P5 row and E10/E14 annotation line citations,
   plus the two pin decisions at R2.8.

Round 2 of 2. The cap is spent and was not needed: no third architectural root
cause arose, and the remediation addressed both P2s at the mechanism rather than
at the record.
