# GwzM5-8 R2-F Relocation Package — Plan (re-chartered)

Status: **ADOPTED 2026-09-01** — the original plan died at the two-round
cap (round 1 NO-GO 1P0/3P1/5P2/8P3; round 2 NO-GO terminal); this
re-charter (the -B arm) adopted the round-2 remedy verbatim and its own
review returned **GO-WITH-CONDITIONS** (0 P0 / 1 P1 / 1 P2 / 4 P3, all
folded same day — §5): **one new name, two directories** — the catalog
relocates; the legacy writer stays exactly where it is, INSIDE the
collision domain. Builders launch on the operator's word (all
non-Fable). Full three-round record:
`GwzM5-8R2F-RelocationPlan-Review.md`.
Charter: the operator's 2026-09-01 "start the remaining m5c train",
Stage 1. The tree governs; the trace
(`GwzM5-8R2F-RelocationTrace.md`) and both review rounds are the
evidence base.

## 1. The design (the round-2 remedy, adopted)

`CatalogPrivateNameV1` is ONE name serving TWO consumers today — the
catalog's private area AND the legacy writer's — through
`private_parent()`. The two terminal failures were the two half-designs:
relocating the residue (round 1: blocked bootstrap everywhere, since
presence alone refuses and cleanup never removes the directory) and
renaming the shared name (round 2: marches the legacy writer into the
catalog's new home, reproducing the collision one drive after
bootstrap). The whole design is the split:

- **`Final`'s on-disk bytes become `catalog-final`** (verified against
  the grammar in round 2: ASCII on the production path; 13 < 17 chars —
  the honest, small, negative MAX_PATH delta; case-fold distinct from
  every leaf; no `ca1-` stem; no collision with the ten infrastructure
  slot names).
- **The legacy writer is pinned to a NEW variant whose leaf stays
  `checked-artifacts`** — zero behavioral change for legacy: it keeps
  writing where its residue already lives, so nothing is orphaned, the
  git-status dirt exemption and preservation-image blindness at
  `preservation_image.rs:8` remain CORRECT AND JUSTIFIED untouched, and
  the g12/g15/stash/max_path fixtures stay green as-is.
- Consequences, verified in round 2: the catalog bootstraps everywhere
  (`catalog-final` absent by construction); resident `checked-artifacts`
  directories are the legacy writer's LIVE area, not residue;
  `exact_row` never sees a `ca1-*` child in the catalog's interior;
  §2.7's criterion — the catalog interior grammar closed, legacy names
  never catalog rows — holds by construction, and R1.1 carries its
  executable statement (the drive-after-bootstrap row, §3).
- **Name-set coupling, decided with the true mechanism and the full
  trade (re-charter [RC-P1-1] folded):** BOTH names live inside
  `CatalogPrivateNameV1::ALL`. De-recognition of resident directories
  never depended on `ALL` at all — the recognition table is
  `fixed_roles`' fixed three-entry array keyed by VARIANT, so
  de-recognition comes solely from `Final.leaf_bytes()` changing.
  `ALL`'s two real consumers are the collision domain
  (`reject_private_collisions`) and a test pin; keeping the legacy
  variant INSIDE the domain preserves today's protection — git index
  paths overlapping the still-live legacy area keep refusing — which is
  what "zero behavioral change for legacy" actually requires
  (outside-`ALL` would silently drop that guard from a live directory).
  Companions of the decision, named where it is made ([RC-P3-4]): the
  two `contracts.rs` pins asserting `members().len() == 4` move to 5.
  The digest movement (`version_digest()` →
  `historical_collision_digest`) is free strictly before first
  activation; the TRADE STATEMENT names everything it declares free —
  both persisted CBOR fields and the scratch-directory name
  ([RC-P2-1]'s restatement) — and R1.1 asserts the movement once per
  persisted field, deliberately, WITHOUT enumerating the digest's ~57
  read references.
- **The scratch stem stays unchanged** (`catalog/scratch.rs:6`,
  `checked-artifacts-catalog-bootstrap-v1.scratch.` — the THIRD
  production spelling, [P3-7]): it brands the family, not the
  directory; the cosmetic name-family split ([R2-P3-3]) is accepted
  with a dated note rather than widening the diff.
- **GitDirectory-arm honesty** ([P3-7]): production never writes under
  the git-directory root today; the variant change there is symmetry,
  not behavior, and is recorded as such.

## 2. OPEN-R1 — answered by design (the routed package-owner ruling)

Under §1 nothing is moved and nothing is orphaned: the legacy area
remains the legacy writer's live directory until E4.7 retires the legacy
writers — which is where the eventual retire-the-area question already
lives, on its own recorded row. OPEN-R1 ("must the relocation move
resident residue, or only relocate future writes?") is therefore
**resolved by design: neither — the relocation relocates the CATALOG'S
name; residue is not a category this package creates.** Recorded
verbatim at OPEN-R1's routed homes by R1.3. (This ruling is the package
owner's, per the E0 routing "blocking for that package's owner"; the
operator's veto stands open in the adoption record.)

## 3. Steps

Process: the review loop at feature tier (Opus builder, single-axis
peer-blind Opus review per step, verbatim reports
`GwzM5-8R2F-<Step>-Review.md` committed with landings, two-round cap
then -B, per-commit lane gate green — the relocation with ALL its
companions lands as one commit or a ritual-7 squash citing reviewed
shas), pristine-overlay landings, direct exit codes, real-workspace
batteries at every `checked_artifact/`-touching landing, the Windows
matrix leg as the named first-dispatch obligation.

- **R1.1 — the split** *(gwz-core; HARD cap 250 incl. tests; one
  semantic decision: the split and its coupling)*. The new legacy
  variant (leaf `checked-artifacts`, INSIDE `ALL` per §1);
  `policy.rs::private_parent` pinned to it; `Final`'s bytes →
  `catalog-final`; the per-persisted-field digest assertions (§1) and
  the two `contracts.rs` member-count pins 4 → 5; in-code dated
  comments carrying the ordering ground (pre-activation freedom), the
  scratch-stem note, and the ASCII constraint NAMED AT ITS THREE
  enforcement sites ([RC-P3-2]: `catalog_names.rs:59`,
  `directory_mutation.rs:721`, `completed.rs:561`).
  Companions same-commit per the duty-split ritual, dictionaries
  enumerated from the boundary checker at build time; aggregate-gate
  lib pins move as a near-certainty — darwin MEASURED / linux DERIVED
  FIRST-DISPATCH-EXPECTED with the driver's dated provenance block.
  The catalog's new directory joins the dirt-exemption and
  preservation-blindness surfaces ALONGSIDE the legacy path — each
  under ITS OWN stated ground ([RC-P3-3]: the legacy comment's ground
  is anchor-specific and does not transfer; the catalog's exemption
  gets its own written ground as exact-managed catalog state, anchors
  structurally impossible) — with the round-1-named fixtures (`g12`,
  `g15`, `stash`) extended to cover the new path and prove the old one
  still covered. TESTS, decisive rows first: **bootstrap the catalog,
  run one legacy checked-artifact drive, re-observe the catalog and
  assert it is still recoverable** ([R2-P2-1] — the executable §2.7
  criterion; verified by the re-charter review to fail under either
  dead design and pass under this one) — with the two [RC-P3-1]
  assertions that make it decisive on both platforms, including the
  structural no-anchor guarantee (`prepare_private` has exactly one
  production caller, the legacy writer — the catalog's directory
  CANNOT acquire a durability anchor; asserted, and carried into the
  Windows first-dispatch obligation); bootstrap-with-legacy-directory-
  present on both roots; old-name directories recognized-by-nothing;
  the per-persisted-field digest assertions; the MAX_PATH constants
  re-measured
  (`max_path.rs` updated to the new leaf arithmetic, naming WHICH git
  directory each arm measures — commondir for the catalog,
  per-worktree `repo.path()` for the legacy git-dir arm — [P3-2]
  completed).
  **LANDED 2026-09-01 at gwz-core `027da5b`** (squashed per ritual 7,
  citing candidate `4ba9071` — round 1 GO-WITH-CONDITIONS, 0 P0 / 1 P1
  / 1 P2 / 6 P3 — plus condition fold `3d417f7` and digest companion
  `1e2a106` — round 2 verification GO, probes A–F). The no-anchor
  guarantee went spelling-blind ([R1.1-P1-1], the R1.2 precedent
  mirrored: production namers of `prepare_private` ==
  `[platform.rs, residue.rs]`); the decisive row's mechanism record is
  platform-split ([R1.1-P2-1]: the `ca1-*` interior refusal is the
  Windows arm; the disjointness assertion discriminates darwin/linux);
  the FLAG-1 semantic-vector regeneration ruled LEGITIMATE — forced by
  `catalog_bootstrap_record.rs:242`, round-trip validated
  independently, the reviewed derivation recorded in the vectors' own
  header. CAP RE-RULED 420 → 427-MEASURED: the +33 over the delivered
  394 is entirely round-1-condition cure, reviewer rider-audit "none"
  — a cap must never punish review conditions. Reconciled pins landed:
  checked_artifact:: 448 → 452 darwin MEASURED / 458 → 462 linux
  DERIVED FIRST-DISPATCH-EXPECTED; remainder 1097/1098 UNMOVED; six
  checker digests moved (four delivery + two following the fold);
  merge-docs battery from the real workspace ok (12 sources, 155
  assertions); lane gate ok. [P3-2] (the trace's stale §7.6) and
  [P3-5]'s capture-full-output note ride R1.3. Review verbatim:
  `GwzM5-8R2F-R1.1-Review.md`, both rounds.
- **R1.2 — the A1 activation tripwire** *(gwz-core; HARD cap 130 —
  ruled 2026-09-01 at the R1.2 round-1 review's counting-basis P2: the
  R2-F cap basis is WHOLE handwritten lines including comments,
  generated excluded, per the sibling convention; the original "60"
  was a code-lines figure, and the mandated dated comments plus the
  driver provenance block are the difference — reset recorded, not
  waived. R1.1's cap, same ruling applied at its delivery: the mandated
  dated-prose volume (ordering ground, ALL-placement ground, ASCII at
  three sites, scratch-stem note, two exemption grounds, three-arm
  MAX_PATH statement, driver provenance) makes whole-line 250 jointly
  unsatisfiable with the charter — R1.1's HARD cap RESET to 420 whole
  lines, code-substance expectation ~200; the delivery's
  flag-not-trim was the stop-and-report behaving correctly, and its
  reviewer verifies nothing beyond mandate rides in the prose. Its
  own step per round-2's P1-3 disposition — a distinct feature, not a
  rider)*. The test asserting `recover_or_create`'s production caller
  count is exactly zero, dated comment naming E4.1 as the step that
  deliberately moves it to one.
  **LANDED 2026-09-01 at gwz-core `bb52dc0`**: candidate `3c6fdee`
  (round 1 NO-GO — [P1-1] call-site prefix evasion, remediated to the
  ALT-4 file-set contains scan; round 2 GO final, 0 P0 / 0 P1 / 0 P2)
  ff-merged, plus the review fold (P3-8: the premise assertion reads
  through the scan's own comment strip; P3-7: the aliased-caller
  residual holds for either owner file). CA pin 447 → 448 darwin
  MEASURED / 457 → 458 linux DERIVED. Review verbatim:
  `GwzM5-8R2F-R1.2-Review.md`, both rounds.
- **R1.3 — the records train** *(docs-only; after R1.1/R1.2 land)*.
  The MAX_PATH rider's dated falsification at ALL FIVE homes
  (Phase4Closure §2.4 + §2.7, evidence-map F-5, settled tuple `:792`,
  checkpoint `:3192-3194`) and the drifted `:1022-1024` anchor
  re-pointed; the single-owner correction at its CLOSED-record homes —
  `GwzM5-8R2E-E7-Acceptance.md:170` and the addendum §7.6.2 ([P3-4]
  as re-graded in round 2) — now counting THREE production spellings
  ([P3-7]); `GwzM5-8R2F-EvidenceMap.md` §2.3 re-owned with F-4/F-5's
  landed actions noted; OPEN-R1's §2 resolution recorded verbatim at
  its routed homes; THE GATE LIFTS — settled tuple §11.3 item 1
  satisfied at R1.1's landing, recorded there and at the checkpoint,
  unblocking O1/O2's named carrier (O1's close still carrying §10 row
  `:280` via O13) and closing O3 with its E7-Acceptance §4 row quoted
  VERBATIM at the record site; E4's gate note marked open.

## 4. Exit criteria

R1.1 + R1.2 landed green with companions and batteries; R1.3 committed;
four CI legs and the Windows matrix green. Then **E4 is open**; the M5c
sequence proceeds to Stage 2 on the operator's word.

## 5. Charter trail

Round 1 (NO-GO): the OPEN-R1 recommendation inverted the trace's
conclusion; the review's own tree finds (presence-refusal;
never-removed directory) killed leave-in-place-same-leaf. Round 2
(NO-GO, terminal): the direction fix dropped the two-consumer split —
renaming shared `Final` marched the legacy writer into the catalog's
new home. Both remedies were the reviews' own; this re-charter adopts
the round-2 remedy verbatim and folds every open finding: [R2-P2-1]
the drive-after-bootstrap decisive test; [R2-P2-2]/[P1-1] dissolved by
the split (exemptions stay justified; both-path coverage now means
both-justified-paths); [R2-P3-1] the digest-consumer enumeration;
[R2-P3-2] the recognized-by-nothing phrasing; [R2-P3-3] the scratch
stem stays, noted; [P1-3] the tripwire split out (R1.2); [P3-2] both
git-directory measurements named; [P3-4] the closed-record homes;
[P3-7] third spelling counted, GitDirectory-arm symmetry-only noted.
OPEN-R1 resolved by design (§2), operator veto open.

**Re-charter round 1 (2026-09-01): GO-WITH-CONDITIONS — ADOPTED.**
0 P0 / 1 P1 / 1 P2 / 4 P3, all folded same day. [RC-P1-1] the
outside-`ALL` rationale rested on a mechanism the tree doesn't
implement (fixed_roles is a fixed variant-keyed array; `ALL` feeds
only the collision domain and a test pin) — decided INSIDE-`ALL` for
the legacy variant, preserving the collision-domain guard over the
live legacy area (the actual zero-behavioral-change), with the two
`contracts.rs` member-count pins named as companions; de-recognition
correctly attributed solely to `Final`'s byte change. [RC-P2-1] the
57-reference digest enumeration replaced by the complete trade
STATEMENT (both persisted CBOR fields + the scratch name) with
per-persisted-field assertions — R1.1 back inside its HARD 250.
[RC-P3-1] the decisive test gains its two both-platform assertions
incl. the structural no-anchor guarantee (single `prepare_private`
caller), carried into the Windows obligation. [RC-P3-2] the ASCII
constraint named at all three enforcement sites. [RC-P3-3] the
catalog's exemption carries its own written ground (the legacy
comment's anchor-specific ground does not transfer). [RC-P3-4] the
placement decision's companions stated where the decision is made.
The reviewer verified the decisive test fails under both dead designs
and passes under this one. **The plan is ADOPTED; builders launch on
the operator's word.**
