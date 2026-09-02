# GwzM5-8 R2-E Plan — checked-artifact consumer conversion and the re-reserved semantics

Date: 2026-08-26. Author: the implementation lane. Status: **ADOPTED — E0
closed 2026-08-27** (dual #1 both axes GO-class with every round-2 condition
folded and lane-verified; the semantics authority is the E0.2 amendment pair
— `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` +
`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md`, addendum controlling; the
operator's sequencing ruling is recorded at §1.1).
First post-v0.11.0 lane per the release checkpoint's resume order.

## 0. Object, charter sources, and one census correction

**The object:** R2-E converts the production consumers onto the
checked-artifact boundary and installs the fault semantics the R2-D
freeze re-reserved to this lane. At its close: the §10 consumer
conversion table is executed, `recover_or_create` has its first
production caller (the first production catalog activation), legacy
writers no longer mutate inside `.gwz/checked-artifacts`, and the
cleanup/barrier/terminal fault families are injected and matrix-green.

**Charter sources, consumed directly:** `GwzM5-8R2D-Plan.md` §5
items 1-2 (the defer-outs this lane discharges);
`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 (the conversion table) and
§11 (packages/ownership); `GwzM5-8R2DInterfaceFreeze.md` §3.5's dated
re-reservation records (cleanup :727-728, barrier :787, terminal
:794/:880) and §4.3; `GwzM5-8A1Activation-ReviewCompleteness.md`
§1.3 bucket (c) (the routed R2-E stream);
`GwzM5-8A1ReleaseR2-ReviewState.md` [P3-R2-1]/[P3-R2-2];
`GwzM5-8A1ReleaseR1-ReviewState.md` §(b) (the recommended guard
tie); the checkpoint's L6/L14 carrier rulings (the archive/GC
sub-package); `GwzMergeCheckpoint-v0.11.0.md` (the shipped
residuals this lane owns).

**Census correction, on the record:** the program checkpoint and the
lane memory have said "67 re-reserved keys." The freeze's own
records and the Completeness review's bucket (c) say **38**: cleanup
11 + barrier 16 + terminal 11. 38 is correct; the 67 was a
lane-introduced error, corrected in the checkpoint with this plan's
commit. The full census stays 165 (`EXPECTED_KEY_COUNT`) — the 38
exist unbound; R2-E binds them without moving the census.

## 1. The consolidated obligation ledger

| # | Obligation | Source | Phase |
| --- | --- | --- | --- |
| O1 | The §10 consumer conversion table, executed: WorkspaceMutatorLock, first merge record, merge record rewrite, terminal archive source-only, stash bundles, markers, workspace checked mutation, `.git/info/exclude` | ConsumerCheckpoint §10; R2-D plan §5 item 1 | E4 *[2026-09-02: obligation text RE-SCOPED (the SECOND re-scope, after ruling "a"): "executed as far as E0.2 §5.2 permits — every row dispositioned by name: converted (E4.1, E4.2, the `finalization/execute.rs` forward arms), carved by dated exception (`GwzM5-8R2E-CapabilityFreeAmendment.md`; the record-root amendment), or already-converted-and-dated (its §6)"; closes DISCHARGED at E4.7 against this text, citing O14 as RE-OWNED to DR-1 — `GwzM5-8R2E-CapabilityFreeAmendment.md` §4.]* |
| O2 | First production catalog activation — `recover_or_create` gains its production caller (gated by settled tuple §11.3) | R2-D plan §5 item 2; bucket (c) | E4 |
| O3 | Legacy writers stop mutating inside `.gwz/checked-artifacts` | thin-A1 §2's accepted residual, expiring here | E4 |
| O4 | cleanup.* semantics + injection, 11 keys (all need `AdmittedActionV1` — the duty attaches now that admission is production) | freeze :727-728; checkpoint Phase-5 record | E1 |
| O5 | barrier.* semantics + injection, 16 keys | freeze :787 | E2 |
| O6 | **BINDING**: `BarrierIntentV1::issue` observe-or-refuse — the last shape of the caller-supplied-restatement class, production-unreachable at the settle | settled dual Code round 1; bucket (c) | E0 decide, E2 implement |
| O7 | terminal.* semantics + injection, 11 keys (terminal retirement edges) | freeze :794/:880 | E3 |
| O8 | The archive/GC consumer sub-package: the 18-UNBOUND per-scenario registry rows + cheap closures (i)+(ii) landing TOGETHER (registry rows + one parametric `adapt_open` refusal test over every non-whitelisted scenario); the archive-equivalence mechanism decision; the two archive shapes riding A1 [P2-1] option (i); the `gc_archived` production route | Evidence §12.7; L6 ruling; A1 record §6 | E0 decide, E5 *[2026-09-02: the `gc_archived` production route's carrier (E4.4) dissolved — the route RE-OWNS to DR-1 conditional on (C); the dead family behind `archive.rs:108-111` is dispositioned by E4.7; the un-GC-able v1-archive defect is fixed standalone through the LIVE `store/gc.rs`/`retention.rs` path, which is NOT this route — `GwzM5-8R2E-CapabilityFreeAmendment.md` §5/§7.]* |
| O9 | [P3-R2-2] the eligible-row upgrade-failure fallback: one executed composed-path test (whitelisted Finalizing row, atomic upgrade `Err(_)` → v0 completes; needs a filesystem-level fault — production hardcodes `AtomicUpgradeFault::None`) — also the moved-pin coverage restoration (both axes converged on this arm) | R2.3 report; A1 record §14 | E6 |
| O10 | [P3-R2-1] `AtomicUpgradeFault` hygiene: cfg-split signature or sealed constructor (injected variants production-compiled but inert) | R2.3 report | E6 |
| O11 | The `DurableObjectIdentity` production-reach question: does the v0.11.0 `--no-ff` v1 store hit the identity probe on Linux? (decides whether capability-refusal UX and docs work exist) + the capability-refusal UX if reach is real | release verify diagnosis, memory note | E0 answer, E6 if real |
| O12 | Recommended riders: the abort rewrite-set bound guard tied (g12:737-739, R1.2 (b)); the `authority_name` self-checking-name weigh; the anchor nits; the 481-item frozen-surface reconciliation at the settle | R1.2; bucket (c) | E6/E7 |

Explicitly NOT R2-E (owners on record): C-2's four fixtures, T-5
regeneration, the multi-repo CI checkout, legacy-path removal,
MAX_PATH, native power-loss evidence (all R2-F); the ordinary-start
v1 owner + floor raise (M5c); the escape implementation packages
(second lane, operator handoff); renormalize (R6); the per-predicate
rejection names (F5 §9 item 6 — unowned candidate, not adopted
here).

### 1.1 E0 amendments to this ledger (2026-08-27 — the E0.3 round-2 verdicts, the E0.2b addendum, and the operator's sequencing ruling)

Authority: the E0.2 amendment pair (addendum controlling), dual #1
closed GO-conditional (Code) / conditional-GO (State) with every
round-2 condition folded and lane-verified; and the operator's ruling
below. The §1 table above is left as written; these dated rows amend
it.

- **O13 — MINTED: the v1 store's raw-writer clause** (source:
  `ConsumerCheckpoint:280`, surfaced at E0.3 State round-2 [P2-R1];
  addendum §7.6.1). *Convert the v1 checked store/root/bundle paths
  per §10 row `:280`'s frozen ordering — "same purposes and artifact
  actions" — and discharge its "no legacy raw writer" clause, whose
  test-gate expired at A1 (2026-08-25).* Split ownership, per the
  addendum's analysis: the **substantive half rides E4.2/E4.3 as an
  explicit scope clause** (E4.2 owns the store's creation path, E4.3
  its rewrite path — the same store; no tenth E4 row); the **"no
  legacy raw writer" half is a pin, landed now** — a boundary-checker
  inventory of `v1_lifecycle/`'s non-test `durable_fs` writer files
  (executed surface at the pin: `store/rewrite.rs`,
  `store/archive.rs`, `archive.rs`), failing closed on growth,
  retired to empty in E4.2/E4.3's own commits. *[Corrected 2026-09-01
  (E4.2 review C2, file ownership measured against the tree): the
  inventory empties across E4.2–E4.4, not E4.2/E4.3 — `archive.rs`
  (`remove_archive`'s sync_dir) and `store/archive.rs`
  (`MergeArchive`'s raw bootstrap) belong to the terminal-archive §10
  row, E4.4's. E4.2 retires NO file: it converted `create_open`'s
  writers and `store/rewrite.rs` stays with reduced counts until
  E4.3's `commit` pair — which is exactly why the pin widened at
  E4.2 from a file SET to a per-file COUNT map
  (V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES now maps each file to its
  remaining raw-call counts, red-probed in three directions), so a
  conversion is measured even when it empties nothing. The dated
  accepted-residual record below is UNTOUCHED by this correction: it
  is scoped to the v1 STORE on the no-ff path, and E4.2/E4.3 do
  complete that store's conversion.]* *[Amended 2026-09-02,
  `GwzM5-8R2E-RecordRootAmendment.md` (dual GO-WC both axes, folds executed): the
  record's REWRITE path — `store/rewrite.rs::commit` — is CARVED OUT
  and keeps `rename_durable` permanently as the RECORD-ROOT EXCEPTION
  (the record is the root of reconciliation; the boundary's
  detach-then-publish shape opens a discovery-dead window no shipped
  reconciler closes — driven at probe/e4-3-detach-window-evidence
  `c9a7303`). Read the two bracketed sentences above accordingly: the
  inventory empties across E4.2/E4.4 for the CONVERTIBLE files while
  `store/rewrite.rs` stays as a PERMANENT-DOCUMENTED row, fail-closed
  both directions (P-1); and "E4.2/E4.3 do complete that store's
  conversion" reads: creation converted (E4.2), rewrite carved out
  (E4.3-B), the archive files E4.4's. The accepted residual's expiry
  is re-scoped at the amendment's §6.]* *[Amended again 2026-09-02,
  `GwzM5-8R2E-CapabilityFreeAmendment.md` (ADOPTED, dual GO×2): the two ARCHIVE files are
  CARVED OUT too — every `store/archive.rs` writer runs from a listed
  operation on the PLAIN lease and `archive.rs::remove_archive` is a
  dead arm — so the O13 inventory is now PERMANENT-DOCUMENTED in ALL
  THREE rows (creation converted at E4.2; rewrite carved by the record-
  root amendment; archive carved by the capability-free amendment) and
  NEVER EMPTIES. Read every "empties across …" and "E4.4's" phrase in
  the two brackets above accordingly; the retire-on-conversion marker
  survives on no row.]* **Dated
  accepted-residual record (2026-08-27): between A1 — the gate's
  expiry, 2026-08-25 — and the E4.2/E4.3 conversions, v0.11.0-lineage
  trees carry the v1 store as a production raw durable writer on the
  no-ff path, in breach of row `:280`'s second clause; the residual
  is accepted, bounded to that interval, pinned against growth by the
  checker inventory, and expires with E4.2/E4.3's landings.** O1
  cannot close DISCHARGED without O13.
- **O14 — the §8 `:239-240` / §9 `:264-266` write-authority gate
  (MINTED 2026-09-01, E4.2 review C1, the §7.6.1/O13 precedent:
  unowned frozen clause → minted obligation).**
  `authorize_write`/`RetainedWriteAuthorityV1` have ZERO production
  consumers while every converted §10 leaf write rides a path-based
  `CheckedArtifact` action — which §9 `:264-266` names as NOT parent
  authority — and the type's own doc promises R2-E consumers take it.
  E4.2 is where this became demonstrable rather than latent: it mints
  the first production `AdmittedCheckedActionV1` and
  `ManagedParentFacadeV1`s, revalidates them (§9 `:266`'s facade
  operation), then discards them and writes the leaf from a path.
  Unowned before this record; no E4 step's charter names the
  conversion. THE FORK, explicit: either converted leaf writes route
  through the write authority (a real conversion package), or §8/§9's
  claims are amended to the landed architecture (a frozen-text move
  needing its own review tier). OWNER: the fork is DECIDED at E4.6's
  chartering, when every converted shape exists; the decision
  escalates to the operator if it amends frozen text. *[Moved
  2026-09-02 to DR-1, the phase-end design round minted by the operator's
  ruling (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`) and chartered by `GwzM5-8R2E-CapabilityFreeAmendment.md` §5 — E4.6 as a
  conversion step dissolves under that amendment; O14's fork, the
  record-root re-examination that rides it, and the legacy in-place-
  writer retirement (now O14's outcome) all decide at DR-1.]* Until decided,
  every E4 conversion states at its execution seam which gate it
  opens (durability) and which stays closed (authority) — the E4.2
  C1(iii) pattern, binding on E4.3–E4.6's charters. Row `:280` is NOT
  breached by the interim state (the E4.2 review's flag-6 judgment);
  O1's close must cite O14's disposition. *[Agendum added
  2026-09-02, `GwzM5-8R2E-RecordRootAmendment.md` §2/§5: the RECORD-ROOT EXCEPTION is
  re-examined AT THIS FORK — option (a), reader-side residue
  reconciliation, is the named candidate; if converted leaf writes
  move onto the write authority with it, the record root converts
  there or the exception is re-affirmed there, deliberately. O1's
  close cites the amendment alongside O14.]* *[2026-09-02: the fork's home
  is DR-1 (above); O1 closes DISCHARGED at E4.7 against its re-scoped
  text citing O14 as RE-OWNED to DR-1 — `GwzM5-8R2E-CapabilityFreeAmendment.md` §4.]*
- **O1 enumeration corrected, in §0's correction form:** the §10
  table is NINE consumer rows (`ConsumerCheckpoint:272-280`); O1's
  row above enumerates eight and drops row `:280` — the ninth is
  O13's, and O1's close carries row `:280` via O13.
- **CROSS-LANE DEPENDENCY ROW — O1 and O2 are blocked on R2-F's
  quarantine/relocation package** (settled tuple §11.3 item 1, the A1
  coexistence gate; §11.2 `:791` pins relocation to R2-F; mechanism
  verified at `interior.rs:414-417`). **Operator ruling, 2026-08-27,
  one-line reply verbatim: "a" — option (a): relocation STAYS
  R2-F's; the dependency is taken explicitly, declining the
  addendum's option-(b) pull-forward proposal.** Consequences, per
  addendum §7.8's fallback four: (i) this row; (ii) O3 below; (iii)
  **Phase E4 is re-scheduled after R2-F's relocation package lands**
  — E1-E3 and E5.1/E5.2 are NOT gated (addendum §7.6.2's reliefs);
  (iv) if relocation has not landed by E7, the acceptance closes
  O1/O2 as *re-owned with a named carrier* (R2-F relocation → this
  lane's E4 resumption), not DISCHARGED, with O1's close additionally
  carrying row `:280`.
  *[LANDED 2026-09-01 (R2-F R1.3 record): the relocation is live on
  gwz-core main — R1.1 `027da5b` (the split) with the R1.2 activation
  tripwire at `bb52dc0`. The named-carrier condition is met: O1/O2
  UNBLOCK at this phase's E4 resumption, O1's close still carrying
  row `:280` via O13. Phase E4 is OPEN — see its gate note.]*
- **O3 — re-owned, not blocked:** O3's text is a location property,
  and the legacy private parent has exactly one non-test owner
  (`policy.rs:33-42` defines it; `observation.rs:93` is its only
  consumer). R2-F's relocation package **discharges O3 directly**;
  E4.7 remains this plan's stronger full-retirement mechanism *[2026-09-02:
  that mechanism — the legacy in-place-writer retirement — is RE-OWNED
  to DR-1 as O14's outcome, `GwzM5-8R2E-CapabilityFreeAmendment.md` §4; E4.7 keeps its
  allowance-expiry and close-out duties]*. Under
  ruling (a), E7 records O3 as *discharged by R2-F's relocation,
  re-owned there* unless E4.7 has also landed by then.
  *[DISCHARGED 2026-09-01 (R2-F R1.3 record): the relocation landed at
  gwz-core `027da5b`. The location property now holds by construction —
  the catalog composes `catalog-final`, so the legacy writers'
  `.gwz/checked-artifacts` is no longer inside the catalog's directory;
  the two grammars no longer share one (disjointness asserted per
  persisted field; the decisive test drives a real legacy write after
  bootstrap and re-observes the catalog recoverable). E7-Acceptance §4's
  row, quoted verbatim per the R1.3 charter: "O3 | **RE-OWNED to R2-F's
  quarantine/relocation package, which discharges O3 directly on
  landing** (§7.6.2's single-owner fact: `policy.rs:33-42` defines the
  legacy private parent, `observation.rs:93` its only consumer); E4.7
  remains this lane's stronger full-retirement mechanism *[2026-09-02:
  RE-OWNED to DR-1, `GwzM5-8R2E-CapabilityFreeAmendment.md` §4]* at the E4
  resumption; OPEN-R1 rides with the package (blocking for its owner).
  *[Tense per State [P3 F8]: nothing has discharged O3 yet — the package
  discharges it on landing.]*" — the landing has now happened. Scope
  note (the [P3-7] single-owner correction): that fact is a
  location-ownership property of the parent COMPOSER and stays true;
  the checked-artifacts NAME FAMILY separately has THREE production
  spellings — the legacy variant's leaf bytes, the bootstrap marker
  stem, and the scratch stem (`catalog/scratch.rs:6`) — counted here so
  "single owner" is never misread as "single spelling". E4.7 remains
  the stronger full-retirement mechanism *[2026-09-02: RE-OWNED to DR-1,
  `GwzM5-8R2E-CapabilityFreeAmendment.md` §4]*.]*
- **OPEN-R1 routed to R2-F** with the relocation package, per the
  ruling: must the relocation *move* resident legacy residue, or only
  relocate future writes? Blocking for that package's owner; the
  Windows permanent-anchor evidence says leave-in-place keeps the
  catalog unobservable there (`GwzM5-8R2DPhase4Closure.md:180-190`).
  Recorded so R2-F inherits the question rather than re-deriving it.
  *[RESOLVED BY DESIGN, recorded 2026-09-01 (R2-F R1.3; adopted plan
  `GwzM5-8R2F-RelocationPlan.md` §2, verbatim): "resolved by design:
  neither — the relocation relocates the CATALOG'S name; residue is
  not a category this package creates." Nothing moved, nothing
  orphaned; the legacy area remains the legacy writer's live directory
  until E4.7 retires the legacy writers — where the retire-the-area
  question already lives on its own row *[2026-09-02: that retirement and
  the retire-the-area question TRAVEL TO DR-1 with O14,
  `GwzM5-8R2E-CapabilityFreeAmendment.md` §4]*. The Windows permanent-anchor
  hazard this row cites is exactly why the round-1 leave-in-place-
  same-leaf design was killed: the review proved presence-refusal
  (`directory_mutation.rs:179-185`) plus never-removed directories
  brick bootstrap in every used workspace — the split is the design
  that makes "neither" safe. The operator's veto stands open in the
  adoption record (gwz-dev `d2e5636`).]*
- **O8 denominators corrected** (E0.3 State [P2-2]; addendum §8):
  E5.1 owes **10** registry rows (the progress shapes) — this plan's
  "18 registry rows" is right as a debt count and wrong as a
  registry-row count; E5.2 owes **8** archive-corpus rows + **2**
  PENDING-FIXTURE rows (carrier R2-F); 10 + 8 = 18; the other two
  NO-FIXTURE scenarios (`B-NOT-STARTED`, `B-PREPARING-EMPTY`) are
  not R2-E's at all. *[E5 landing, 2026-08-28: the 10 progress-shape
  debts execute as **9 registry rows + 1 clause-cited disposition** —
  `G-VERIFYING` is a `Finalizing` shape, so §12.9(c)'s own ground
  applies and a tenth row would weaken the registry; ratified by the
  E5 interior review (adjudication A). Evidence §12.9(e).]*
- **O11 — CLOSED NEGATIVE at E0.1** (no v0.11.0 production path
  reaches the identity probe; the exposure arrives at E4.1), and its
  capability-refusal UX **folded into E4.1's precondition set**,
  which is now SEVEN items (E0.2 §5.3's five + item 0, the
  coexistence gate + item 6, refusal-ordering/restart legality).
  *[Record corrected 2026-09-01 (E4.1 builder flag 2, reviewer
  CONFIRMED): the negative holds only for the CATALOG-LEASE probe.
  The LEGACY identity module is production-reachable at v0.11.0 —
  `observation.rs`'s `observe()` calls `parent_is_current` →
  `identity::object_identity` on every legacy checked observation and
  `observe_leaf_exact` calls `identity::file_identity` on every
  `Bytes` observation, with ordinary-merge production callers — so on
  Linux `name_to_handle_at` was already executed by shipped code.
  E4.1's typed-capability cure covers both modules; the closed-
  negative CONCLUSION (no UX work owed pre-E4.1) was right for the
  wrong denominator.]*
- **O6 — COMPLETED at E0.2b §5**: the read-side identity refusal
  (the owner re-mints the witness on every resume and refuses on
  disagreement) joins the mint-side mechanism; key #5's semantic is
  extended accordingly.
- **O12 re-routed:** three riders consumed at E0.2 (A-1 re-framed —
  the checked path *replaces* `authority_name`, so the settle's
  trigger does not fire in R2-E; the `unsupported.rs` nit moved into
  E4.1's preconditions; the 481-item reconciliation stays E7.2's,
  against both denominators); the fourth rider — the abort
  rewrite-set bound guard tie (g12:737-739) — is **E6.2's**, named
  at E0.2b [P3-5].
- **E6.3 — VOID** (both branches empty: reach negative, UX moved to
  E4.1); closes with a dated no-work record at E7.2.
- **OPEN-C1 — STRUCK** (premise refuted:
  `authority_record_binding.rs:486` takes `AuthorityScratch`);
  E1.1/E1.2 owe no interleaving proof.

## 2. Review tiering (the adopted process, applied)

Two duals maximum: **dual #1 at the E0 semantics amendment** (it
extends the frozen §3.5/§4.3 surface — the same class as R2-D's
Phase 0), **dual #2 at the E7 settled tree**. *[Budget superseded
2026-09-02: the refined tier policy's amendment-tier line (checkpoint
2026-08-22) carries three further duals on its own authority — the
record-root amendment, the capability-free amendment, and DR-1's
design object — each recorded at its own header; this line's "two"
is R2-E's plan-time budget, not a cap on amendment-tier duals.]* Every interior step:
single-axis peer-blind with automatic escalation on P0/P1/P2;
two-round remediation cap; reports filed verbatim to dev-docs as
`GwzM5-8R2E<Step>-Review*.md`. Schedule in gates and review rounds,
not LOC (the R2-D plan's own lesson) — nominal: 1 amendment round +
8-10 interior reviews + 1 settled round. All landings by the
standing rituals (pristine overlay, executed pins, untruncated
sweeps with zero-match residual greps, per-OS counts where measured).

## 3. Phases

### Phase E0 — the object frozen (milestone: R2-E is defined, decided, and dual-reviewed)

- **E0.1 — the reach traces** (read-only, ~0 LOC): (a) O11 — a
  bounded trace from `handle_start_durable_v1`/the checked v1 store
  into `checked_artifact::identity` on Linux, answering whether any
  v0.11.0 production path executes the `name_to_handle_at` probe;
  (b) the O2 gate check — restate what tuple §11.3 requires before
  production catalog activation. Both answers land as a dated note
  the E0.2 amendment cites.
- **E0.2 — the semantics amendment** (docs, one train): dated §3.5
  activation records installing injection semantics for all 38 keys
  (cleanup's AdmittedActionV1-bound duties; barrier incl. the O6
  observe-or-refuse RESOLUTION — decide observe vs refuse per shape,
  recorded; terminal retirement edges); the O8 archive-equivalence
  mechanism DECISION; the §4.3 E-table annotations these imply.
  Quote-preserving, per the sanctioned annotation mechanism.
- **E0.3 — dual #1**, peer-blind Code+State on the amendment.
  GO/GO unblocks E1-E3 in parallel.

### Phase E1 — cleanup.* installed (11 keys; milestone: family matrix-green)

- **E1.1** — injection sites in the owner-private mutation files per
  the Phase-1 pattern (driver holds zero), semantics per E0.2
  (<500 LOC).
- **E1.2** — the cleanup fault matrix: every key
  interrupted+restarted+converged; census statements; checker
  count/pin companions in the SAME commit (ritual 5 — including the
  R4b-G driver's per-OS fault markers, which move with any lib-test
  addition).
- **E1.3** — single-axis review; land.

### Phase E2 — barrier.* installed (16 keys + O6; milestone: family matrix-green, the restatement class extinct)

- **E2.1** — barrier injection semantics (<500 LOC).
- **E2.2** — `BarrierIntentV1::issue` per E0's resolution: the seam
  stops accepting a caller-supplied restatement (observe) or refuses
  typed (refuse); the last named-class shape closes (<300 LOC).
- **E2.3** — the barrier fault matrix + companions; review; land.

### Phase E3 — terminal.* installed (11 keys; milestone: family matrix-green)

- **E3.1** — terminal retirement-edge semantics (<500 LOC).
- **E3.2** — matrix + companions; review; land.

E1/E2/E3 are parallel-friendly after E0 (distinct families, distinct
owner files) with the shared-driver duty split observed; if run in
parallel, isolated worktrees per lane and the lane owner lands
sequentially.

### Phase E4 — the consumer conversion (milestone: production writes go through the boundary — *re-scoped 2026-09-02 by `GwzM5-8R2E-CapabilityFreeAmendment.md` §4: CHECKED-FEATURE writes go through the boundary; capability-free arms are a dated exception, not unfinished work*)

One step per §10 table row, each a reviewed <500-LOC package
converting that consumer onto the checked boundary with its frozen
ordering; the rows are parallel-friendly except where the table's
ordering couples them *[one recorded exception to the universal,
2026-09-02: the merge record's rewrite path keeps its atomic raw
publication — `GwzM5-8R2E-RecordRootAmendment.md` §2, the record-root exception; and that
amendment's §4 PLAIN-LEASE PROBE CLAUSE binds every remaining
conversion, pointer at the gate note below; and 2026-09-02,
`GwzM5-8R2E-CapabilityFreeAmendment.md` §3: rows `:275`–`:279`'s listed-operation writers are
CARVED OUT permanently, only the three `finalization/execute.rs`
forward arms convert (E4.5/6-B), E4.4–E4.6 as chartered do not start]*:

*(E0 gate, 2026-08-27, under the operator's ruling (a): this phase
opens only after R2-F's quarantine/relocation package lands — §1.1's
cross-lane dependency row. **[GATE LIFTED 2026-09-01 — THIS PHASE IS
OPEN.** The relocation landed: R1.1 at gwz-core `027da5b`, the R1.2
activation tripwire at `bb52dc0` (production caller count pinned at
ZERO; E4.1 moves it to one in the same reviewed commit as the caller).
E4.1's attention set additionally carries, recorded at the R1.3
records train: (1) **anchor nit 1's Q1 shape** — the R2-F alternative
carrier fired without implementing it (see E7-Acceptance's carrier-row
deviation note); E4's first production train implements, not
re-decides; (2) the stale `#[allow]` reason at `catalog.rs:10-16`
("gains its first production caller in R2-E" — E4.1 is the mover and
the tripwire is the enforcement; both R2-F builders flagged it;
rewrite it in the caller-adding commit); (3) the R1.2 review's
matcher-edge notes for the tripwire file ([P3-6] message wording free
improvement; [P3-8]'s residual habit — a `// was:` comment — is closed
by the landed comment-strip but stays the named shape to not
reintroduce); (4) **the path-constant second-authority pin** (operator
skim, 2026-09-01, mechanics verified): the catalog leaf is owned by
`CatalogPrivateNameV1::Final` but spelled AGAIN as
`preservation_image.rs:20` `CATALOG_PRIVATE_PATH = ".gwz/catalog-final"`
(the legacy twin at `:8` has the same relationship) — the dirt
exemption and preservation blindness compile against the strings, not
the enum, so a future leaf rename moves the catalog while merge
silently keeps exempting the OLD path. Fixtures are fail-loud; the two
production constants are the silent pair. Neither side can see the
other (`pub(in crate::checked_artifact)` vs `pub(super)`), so the pin
is the source-scan idiom IN `contracts.rs` beside the `leaf_bytes`
pins it already holds (`:212`): read `preservation_image.rs`, assert
both `= ".gwz/…"` literals byte-adjacent to their constant names —
~6 lines, carried when E4.1 is already in `contracts.rs` moving the
member pins.]** **[E4.4–E4.6 BINDING — POINTER (2026-09-02; the
AUTHORITY is `GwzM5-8R2E-RecordRootAmendment.md` §4): the PLAIN-LEASE PROBE CLAUSE — no E4
conversion may place the durable-identity probe on a path reachable
from the PLAIN `V1MutationLease` (the abort/reverse arms) without an
explicit dispositive record in its own charter (split by lease, or
reverse-arm reach proven absent, or the capability-free list amended
at the amendment tier); charter authors enumerate lease-reachability
of every converted function BEFORE the build. E4.4 is the first
bound step: `archive_terminal` is reached from abort's `respond()` on
the plain lease. *[2026-09-02: E4.4 as a conversion step is GONE — the
archive is carved out entirely by `GwzM5-8R2E-CapabilityFreeAmendment.md` §3 — and the clause's
option "the capability-free list amended at the amendment tier" is
CLOSED by the operator's ruling (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`); the clause otherwise
survives and binds E4.5/6-B's three forward arms.]* Also binding from that amendment: every charter
carries the RECORD-ROOT ANALYSIS duty — any leaf that is its own
discovery root gets the §1a analysis before conversion.]** E4.1's precondition set is SEVEN (§1.1's
O11 row). The `WorkspaceMutatorLock` §10 row's declared purpose —
"runtime bootstrap only" — is load-bearing at E4.1: the runtime lease
is bootstrap-only and identity-probe-free; the catalog lease is where
the durable-identity capability is required and where its absence is
refused (E0.2 §5.2, with E0.2b §6.4's fifth ground —
`workspace_mutator_lock.rs` is byte-pinned). E2.2 strictly precedes
any E4 row that admits actions touching the roaming anchor.)*

*(E7.2 annotation, 2026-08-29 — the [R2-P3-1] scope clause written
where E4's builder will read it, discharging the E7.1 State axis's
[P2 F1]: **a settled barrier ordinal does not imply its target
parent's dirents were ever ordered; E4 consumers must not rely on
the implication** ([R2-P3-1], E2 review round 2). The dual's
disposition of the underlying contract fork: the non-implication is
stated explicitly rather than widening the restart path to
re-barrier a converged alias — re-barriering would be an
E0.2b-class semantics move on the frozen 16-key record, for a
property no current consumer needs; E4 consumers that need ordering
must barrier for themselves. Its terminal sibling clause, same
authority (dual #2 Code axis F4/Q4, disposing E3 F7): **the terminal
converged-restart return — converged-by-observation — does not imply
key #8's retired-root flush or key #9's catalog-root barrier ran on
that drive; converged does not imply flushed.** E4's first consumers
must not assume either implication; any corpus-wide re-flush
tightening is a hardening-lane decision (R2-F class), not a consumer
assumption.)*

- **E4.1** — `WorkspaceMutatorLock` runtime bootstrap (the O2
  activation rides here: `recover_or_create`'s first production
  caller, gated by the E0.1(b) §11.3 restatement). *(Corrected
  2026-08-26, E0.3 State [P1-2]: the operative gate is the REAL
  §11.3 item 1 — the A1 coexistence gate: no production catalog
  activation until the R2-F-pinned relocation lands. E0.2b
  analyzes which E4 rows truly require catalog activation and
  proposes the sequencing; a cross-lane dependency or pull-forward
  decision may fall to the operator.)*
  **LANDED 2026-09-01 at gwz-core `e56124b`** (ff-merge of the
  four-commit train, per-commit lane-gate green: (a) `7f2862d`
  hygiene riders 148 — Q1 bounded read verbatim + [R2-P3-3]/F3 +
  the path-constant pin; (b) `1f3ba71` activation 472 — the
  `activate_workspace_catalog` door in `entry.rs`,
  `PlatformCapability::PersistentFilesystemIdentity`, all seven
  preconditions, tripwire 0→1; (c) `6688f34` the [P1-1]/[P2-1] cure
  331, cap RE-RULED 300→331-MEASURED — round 1 was NO-GO on the
  reviewer's driven wedge: the refusal sat BEHIND dispatch's durable
  v0→v1 upgrade, permanently wedging an interrupted ordinary merge
  on a non-admitted filesystem; the ruled contract R1-R7 cures it at
  the adapter (activate-before-upgrade viability window, Resume-only;
  declined viability → the v0 lifecycle completes per the adapter's
  own [P1-1] doctrine) and splits the lease (`acquire` plain for
  abort — capability-free end to end — vs `acquire_activated` for the
  forward paths); (d) `e56124b` the round-2 fold — [P2-C1] Windows
  compile gate (one-line cfg deletion, real-platform proof = the
  landing dispatch), [P3-C2]/[P3-C3] staleness cures. Round 2: GO;
  R1-R7 discharged and driven, rider audit clean, 331 exact. R4
  residual DISCLOSED: post-upgrade non-capability obstruction refuses
  with `--abort` as the exit — narrowed, driven, not a wedge. Pins
  landed: CA 456 / v1 257 / remainder 1099+1 darwin MEASURED; linux
  466/1100 DERIVED FIRST-DISPATCH-EXPECTED at Windows 33498089904 +
  Platform 33498092726 (dispatched at the landing; the Windows leg is
  the split's first compile of the activation AND [P2-C1]'s proof;
  [P3-7]'s note stands — the v1 partition now needs a
  catalog-admitted dispatch filesystem, ext4/APFS/NTFS). Operator
  rulings recorded: ext4-only Linux posture "fine for now"
  (2026-09-01); the E4-train standing order (land on GO; lane rules
  escalations; terminal NO-GO returns to the operator). Carriers out
  of this step: [P3-1] release-notes line → the release train (E7
  ledger) *[2026-09-02: E4.1's line as written — "`gwz merge --abort`,
  which needs no such filesystem" — is an OVER-CLAIM (three checked-
  door abort paths probe today); its on-disk home is HERE, per
  `GwzM5-8R2E-CapabilityFreeAmendment.md` §6 and the pins review
  [P2-2]. The corrected sentence, verbatim: "an abort that touches no
  checked artifact needs no such filesystem; aborts that must
  re-verify checked artifacts — preservation bundles, a selected
  root's manifest and lock, or the merge's published evidence,
  re-verified through the checked boundary — need persistent file
  handles and a mount identity." Carrier: the release train (the E7
  ledger's release-notes line); the pins package's fold commit
  restates it in its message.]*; [P3-2] renderer row (~10 lines, pub(super) — NOT
  pub(crate), clippy private_interfaces) + [P3-3] identity.rs Windows
  bare-errno arm + [P3-4] entry.rs scan-hole record → E4.2; [P3-C1]
  R2 unpinned against E4.4's terminal-archive arm regaining
  capability-dependence for abort → E4.4's charter *[2026-09-02: E4.4
  dissolved; the carrier is the pins package's pin that an abort touching
  no checked door stays capability-free, `GwzM5-8R2E-CapabilityFreeAmendment.md` §7]*; [P3-5]
  workspace_mutator_lock stale allow + catalog.rs allowances →
  E4.7; [P3-8] catalog-mutation invisibility in reverse_rollback
  rows → E4.2-E4.6 as consumers arrive *[2026-09-02: CLOSED — nothing
  further converts on those rows' paths, no snapshot exclusion grows,
  `GwzM5-8R2E-CapabilityFreeAmendment.md` §7]*. Review verbatim, both
  rounds: `GwzM5-8R2E-E4.1-Review.md` (1051 lines).**
- **E4.2** — first merge record (`MergeStore` + `PreservationBundles`
  bootstrap ordering). *(E0: + O13's substantive half, creation path;
  and the §11.3-item-2 duties — the `retain_managed_parent_at_for_test`
  door's disposition and the Git-directory workspace-root binding —
  answered in this commit, addendum §7.7.)*
  **LANDED 2026-09-01 at gwz-core `7f28907`** (no-ff merge of the
  reviewed train: candidate `7214010` — round 1 GO-WITH-CONDITIONS,
  0 P0 / 0 P1 / 1 P2 / 5 P3, cap EXACTLY 500 with no rider — plus
  condition fold `1f47d6e` and round-2 fix `3717249`, which reverts
  the reviewer's own round-1 conflation to the true pre-fold text,
  owned by the review on the record; round 2: GO). The when-missing
  bootstrap rides the managed-parent door under row `:273`'s frozen
  ordering, driven both clauses; `create_open` refuses an
  unbootstrapped parent (the adapted route structurally never reaches
  it — the E4.1 wedge class is not reborn); O13's creation-path
  writers converted with the pin widened to per-file counts; both
  §11.3-item-2 duties answered with tree evidence (the test door
  SURVIVES dated; the Git-directory prefix binds to the target's own
  root, no production parent on that variant); the E4.1 [P3-2]
  renderer row landed `pub(super)`; tripwire UNMOVED at 1. [P2-1]
  became O14 (above) with the C1(iii) opened-vs-closed gate statement
  at the execution seam, binding E4.3–E4.6. Pins at the landing
  (which also reconciles the --target handoff snapshot `c201a01`,
  baseline-measured ALL GREEN on bare darwin): CA 457 / v1 260 /
  remainder 1110+1 darwin MEASURED; linux 467/1111 DERIVED
  FIRST-DISPATCH-EXPECTED at Windows 33511535235 + Platform
  33511538719. Review verbatim, both rounds:
  `GwzM5-8R2E-E4.2-Review.md` (997 lines).**
- **E4.3** — merge record rewrite (exact store; unknown fields and
  exact reread preserved). *(E0: + O13's substantive half, rewrite
  path — `v1_lifecycle/store/rewrite.rs`'s raw writer converts here;
  the O13 checker inventory retires to empty across E4.2/E4.3.
  [Corrected 2026-09-01, E4.2 review C2: across E4.2–E4.4 — the two
  archive files are E4.4's; E4.3 empties `store/rewrite.rs`.] [2026-09-02:
  the archive files are CARVED OUT by `GwzM5-8R2E-CapabilityFreeAmendment.md` §3; the inventory never
  empties.])*
  **E4.3 OUTCOME 2026-09-02 — CARVED OUT, not converted.** The
  conversion as chartered was built (candidate `c9a7303`, full battery
  green, preserved at gwz-core `probe/e4-3-detach-window-evidence`)
  and REJECTED at delivery by its own builder's driven P0: the
  boundary's detach-then-publish replacement opens a crash window in
  which the open record vanishes from discovery with no reconciler
  (the record is the root of reconciliation), and the shared `commit`
  put the identity probe on every abort. The lane owner verified both,
  the charter's contradictory clause pair was owned, and the operator
  ruled option (c): the RECORD-ROOT EXCEPTION,
  `GwzM5-8R2E-RecordRootAmendment.md` (dual Code+State Fable×2, both
  GO-WITH-CONDITIONS, folds executed 2026-09-02). E4.3-B lands the
  pins — P-1 the permanent O13 row fail-closed both directions, P-2
  the negative tripwire — with NO production conversion;
  `store/rewrite.rs` does NOT empty. The rejected alternative (reader-
  side residue reconciliation) is the named candidate at O14's E4.6
  fork.** *[2026-09-02: at DR-1, where O14's fork now lives —
  `GwzM5-8R2E-CapabilityFreeAmendment.md` §5.]*
- **E4.4** — terminal archive source-only (+ the O8 `gc_archived`
  production route lands with the archive consumer). *[2026-09-02: DOES
  NOT START as chartered — every `store/archive.rs` writer runs from a
  listed operation on the plain lease and `remove_archive` is a dead arm
  (charter prep `GwzM5-8R2E-E4.4-CharterPrep.md` §1, corrected by the
  amendment's dual); the archive is CARVED OUT by `GwzM5-8R2E-CapabilityFreeAmendment.md` §3; O8's
  `gc_archived` route re-owns to DR-1 conditional on (C), the dead family
  dispositioned by E4.7 (§5); tier 2 EXPLICITLY DEFERRED to DR-1 (§5);
  the shipped un-GC-able v1-archive defect is fixed standalone, read-side
  (§7).]* *[2026-09-02, LANDED: the standalone GC fix — gwz-core
  `0dccd3e` (the shared `record_wire::decode_archived_common`, three
  decode sites cured: `merge/gc.rs`, `store/gc.rs`, `store/retention.rs`),
  `c39f6d4` (a completed `--no-ff` archive collected end to end),
  `b5ff7fc` (the retention site pinned; its `cfg(test)` mask DELETED —
  test-only, never shipped — so the row drives the shipped arm), and the
  landing fold `3c632ec`; Opus interior review
  `GwzM5-8R2E-GcArchiveDecode-Review.md`, round 1 GO-WC ([P2-1] the
  unpinned retention site), round 2 GO-WC (three message/comment P3s,
  folded at landing). GC stays capability-free: no probe on any of the
  three sites, verified by chain. The disclosed limit: a well-formed
  archive whose `operation_drift` identity is duplicated is refused by
  the shared decoder and retained forever, uniformly with `--gc <id>` —
  fail-closed and contract-§7-consistent; the retained-forever set
  strictly shrinks against the shipped base, which retained EVERY v1
  archive. Reach: ordinary retention now sweeps v1 archives too (release-
  notes line in `b5ff7fc`'s message; carrier the release train).]*
  *[2026-09-02, LANDED: **the pins package E4.4-6-B** (charter = the
  capability-free amendment's §7) — gwz-core `9fbe4ac` (the package:
  the twenty-row `CAPABILITY_FREE_RAW_WRITER_INVENTORY` with its key-
  set digest and per-ARM negative scans; the carved-site annotations;
  the abort sentence re-scoped BY PATH at its ten homes; the eight JSON
  carriers; the allowance annotations; `store/gc.rs` and
  `store/retention.rs` counts; NO production conversion), `a7e5e21`
  (the round-1 fold: the shared masker made a FAITHFUL port of the
  checker's `mask_non_code` — a scanner, no quote toggle — with a fail-
  loud belt and a 15-row self-test; the twentieth row
  `store/archived.rs::archive`; §6's sentence verbatim in the message
  with the release train as carrier; a flat digest on `artifact/mod.rs`),
  `f563446` (the landing fold: three round-2 doc conditions). Opus
  interior review `GwzM5-8R2E-E4.4-6B-Review.md`: round 1 NO-GO (P1 the
  first masker's `quoted` flag leaked across lines through `'"'` char
  literals — 178 live blinded lines, the E4.3-B door invisible; P2 the
  twentieth row; P2 the undischarged §6 restatement), ruled by the lane
  owner under the E4 standing order (§12); round 2 GO-WC (§13): the
  masker CURED structurally and empirically — a 411-file differential
  against the checker's own masker shows ZERO blinded lines; twenty rows
  recount 20/20; all 25 digests reproduce. Cap re-ruled to the measured
  figure (770 by sum, +713/−68 net against ≤ 589) on the ruling's own
  clause — a quiet masker under an absence pin is the trade refused, not
  line count. O13 stays the v1_lifecycle SCAN; the inventory is the
  DATA layer; `store/rewrite.rs` stays the record-root amendment's
  alone.]* *(E5 landing,
  2026-08-28 — two lane-owner determinations, review [P2-3] and
  adjudication G: (1) E4.4 is RATIFIED as carrier of the owed tier-2
  archive-equivalence executions for the eight
  byte-preserved-v0-origin corpus rows — transitively R2-F-dependent,
  since E4 opens only after R2-F's relocation under the operator's
  ruling (a); the corpus carrier strings name the encumbrance.
  (2) Tier 2 as written in amendment §6.3 is NOT SATISFIABLE against
  the whole frozen projection surface — a v1-produced and a
  v0-produced archive of the same scenario differ by construction in
  `source_version` and the acceptance discriminant — so before E4.4
  executes tier 2, a comparable sub-surface must be minted BY
  AMENDMENT WITH DUAL REVIEW, not by the implementing step;
  deliberately unminted at E5 per §6.3's rejected-alternative
  warning. Queued for the E7 dual.)*
- **E4.5** — stash bundles + markers (two rows, one package if they
  share the parent-durability seam).
- **E4.6** — workspace checked mutation + `.git/info/exclude`.
  *[Both re-scoped 2026-09-02, `GwzM5-8R2E-CapabilityFreeAmendment.md` §3/§7: the listed-operation
  writers of rows `:276`–`:279` are CARVED OUT; `:276` has no forward
  writer at all; what converts is exactly the three `acquire_activated`-
  only sites in `v1_lifecycle/finalization/execute.rs` (`:45` marker,
  `:48` lock, `:51` boundary — the last needs a git-directory replace
  door), chartered as ONE small step E4.5/6-B after the pins package
  E4.4-6-B, each arm with its own reachability proof.]* *[Re-dispositioned
  2026-09-02 on the charter prep (`GwzM5-8R2E-E45-6B-CharterPrep.md`): `:48`/`:51` hit the
  observation-dead window in the forward and abort observers and STAY
  RAW as the [R2-P3-1] dated residual (E4.6-B does not open); `:45`'s
  parent is bootstrapped by nothing on the v1 path, so E4.5-B is ONE
  step in E4.2's shape — a `DurableMerge` parent door at merge start,
  then the marker conversion — cap 500; row `:279`'s cell-2 wording
  ("Git-directory artifact action") already contradicts the shipped
  reverse writer's workspace door → DR-1 frozen-text item.
  `GwzM5-8R2E-CapabilityFreeAmendment.md` §7's post-adoption note carries the full record.]*
  *[2026-09-02 (later): **E4.5-B STOPPED on its trigger (5)** — no
  commit (`GwzM5-8R2E-E45B-Report.md`, verified by the lane owner):
  converting `:45` strands `--abort` after an interrupted checked
  publication (directional residue — the prep's §2.4 hazard its §2.5
  table missed); the parent premise above is FALSE (`gwz.conf/markers`
  is created at workspace creation) and the `DurableMerge` door at
  merge start is unbuildable (record-derived owner). A different
  scoping → the operator (checkpoint packet; recommended: `:45` joins
  the [R2-P3-1] residual, E4.7 carries the three residual sentences,
  DR-1 gains the directional-residue class). E4.7 waits on the ruling.]*
- **E4.7** — O3 close-out: the legacy in-place writers inside
  `.gwz/checked-artifacts` retire behind the converted paths; the
  A1-era allowances (`catalog.rs:10-16` class) expire with dated
  annotations.
  *[Re-scoped 2026-09-02, `GwzM5-8R2E-CapabilityFreeAmendment.md` §4 — by exactly the operator's
  sentence "E4.7 does not retire those writers" (the carved
  capability-free arms) and no further: the legacy in-place-writer
  RETIREMENT is RE-OWNED to DR-1 as O14's outcome (the legacy writer is
  the pre-catalog `CheckedArtifact` every converted path still rides),
  with OPEN-R1's retire-the-area question travelling; E4.7 KEEPS the
  A1-era allowance expiry — the class NAMED at the amendment §4
  (`catalog.rs`, `coordinator/mod.rs:16`, `entry.rs:299-300`,
  `bootstrap/managed.rs`, `checked_artifact/mod.rs`, checker `:358`,
  `workspace_mutator_lock.rs:44-47`), each expiring or re-reasoned as
  permanent — plus E0.2 §7.1's per-converted-consumer `finish()`-
  reachability record, the A-1 rider's reopen check, the dead
  `gc_archived` family's disposition, O1's DISCHARGED close against its
  re-scoped text (O14 cited as RE-OWNED to DR-1), and the phase
  close-out records.]* *[Charter prep 2026-09-02 (`GwzM5-8R2E-E47-CharterPrep.md`): 23
  actionable allow sites (2 expire proven; 1 candidate to measure; 18
  re-reasoned permanent; 2 delete-or-permanent) + 12 prose/checker
  rewrites, with byte-exact texts in the report; the prescribed needle
  set corrected (it missed `archive.rs:110` and `merge/gc.rs:31-34`); six
  unnamed allows added incl. `namespace/mod.rs:10-19`'s undischarged
  E7 re-owning; A-1 does NOT reopen (reopen condition at E7-Acceptance
  `:181`); `gc_archived` KEPT permanent-pending-DR-1 (deletion fires O13's
  shrinkage arm); the pins package owns `archive.rs:108-111` /
  `catalog_names.rs:44` / the O13 block — E4.7 builds on them. Shape:
  one docs+allowances step (Opus builder for the gwz-core edits +
  digests; the lane owner for the close record in the E7-Acceptance
  form), after E4.5-B lands; its record IS the phase close, then DR-1.]*
- **DR-1** — the phase-end design round (minted by the operator's ruling
  `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md` point 5; chartered at `GwzM5-8R2E-CapabilityFreeAmendment.md` §5; its own dual on the
  amendment-tier line). Opens AFTER the pins package and E4.7 land, never
  inside E4. Agenda: (C) the non-identity / degraded boundary mode;
  reader-side record reconciliation; O14's fork (`authorize_write`
  consumers vs amending §8/§9); the tier-2 comparable sub-surface (minted
  BY AMENDMENT WITH ITS OWN DUAL, naming the execution carrier); the
  record-root exception's re-examination; §6's already-shipped abort
  narrowing; the legacy in-place-writer retirement; O8's checked-archive
  route (conditional on (C)). NAMED for the operator, not decided: is
  DR-1 R2-E's phase E8 or a new lane?

Each step: single-axis review, matrix at the landing, T-6-class
tripwires re-verified.

### Phase E5 — the archive/GC evidence sub-package (milestone: the unbound scenario space machine-recorded)

- **E5.1** — the 18 registry rows + the parametric `adapt_open`
  refusal test, ONE package (the L6 ruling: they land together);
  the two archive shapes' dispositions; scenario-map and
  compatibility-checker companions in the same commit.
- **E5.2** — archive-equivalence per E0's decision; review; land.

Parallel-friendly with E4 (registry/test surface vs production
conversion surface).

### Phase E6 — donated hardening riders (milestone: the review-debt ledger empty)

*[E6 landed 2026-08-28 at `afbc25d` (review GO; its F-7): the milestone is met
except two rows that spill by lane-owner ruling — anchor nit 1, re-routed to
the E7 dual because its deferral terms no longer describe the tree (the
unbounded read lives in the shared `observe_leaf_exact`, observation.rs:249;
E7 inherits the in-tree bounded-read template at platform.rs:219-234 per
review F-6), and E6.3's VOID, closing with its dated no-work record at E7.2.
The review-debt ledger therefore empties at E7, not E6.]*

- **E6.1** — O9: the composed-path upgrade-failure test (fs-level
  fault injection; reuses E1-E3's injection machinery — schedule
  after any one of them lands).
- **E6.2** — O10 hygiene + the abort-bound guard tie + the anchor
  nits (one small package).
- **E6.3** — O11's consequence IF reach is real: the
  capability-refusal UX (typed message naming the filesystem
  capability) + runbook/notes lines; if reach is not real, a dated
  no-work record closes O11. *(E0, 2026-08-27: VOID — reach answered
  negative at E0.1 and the UX moved into E4.1's precondition set;
  closes with the dated no-work record at E7.2 — §1.1.)*

### Phase E7 — the settled gate (milestone: R2-E accepted)

- **E7.1** — full-tree settled review, dual #2 (peer-blind
  Code+State), carrying the E1-E6 escalation second-axis scrutiny
  per the deferred-escalation design.
- **E7.2** — three-platform matrix acceptance at the settled tree
  (win/plat dispatches; per-OS battery pins re-measured where
  counts moved); the 481-item frozen-surface reconciliation; the
  acceptance record with census statements and the ledger's
  row-by-row close (O1-O12 each DISCHARGED or re-owned with a
  named carrier).

## 4. Non-goals

Everything in the "Explicitly NOT R2-E" list of §1; any change to
`ACTIVE_WRITER_FLOOR`; any new wire (protocol/generated.rs stays
zero-diff); any release cut (v0.11.1+ is its own train on this
plan's completion or the operator's call).

## 5. Open operator decisions (one line each, none blocking E0.1)

1. **Parallelism width** — run E1/E2/E3 as three parallel lanes
   (faster, three worktrees + the shared-driver ritual load) or
   sequentially (calmer). Lane recommendation: parallel, the
   R2-D interior proved the discipline. *(Exercised 2026-08-27 at
   the E1-E3 launch per this recommendation, standing since the
   plan's presentation: parallel, three isolated worktrees, the lane
   owner lands sequentially. The operator may override.)*
2. **The executable-template policy** (recorded post-A1 operator
   question): if decided, its typed refusal lands as an E4 rider;
   if not, it stays an open operator item.
3. **Scheduling vs quota** — E0 is cheap (one read-only trace + one
   docs train + one dual); the E1-E7 implementation spend is
   R2-D-interior scale. If this week's pool is tight, E0 now and
   the implementation lanes after the Friday reset. *(2026-08-27:
   E0 closed and E1-E3 launched pre-reset, on the operator's "a".)*

## 6. First action on approval

E0.1's reach traces (read-only) start immediately on "go"; E0.2
drafts against their answers; dual #1 follows. No tree mutation
before the E0.3 GO/GO.
