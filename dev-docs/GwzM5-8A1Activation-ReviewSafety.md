# A1 activation review — Safety axis (round 1 of 2)

Date: 2026-08-24
Axis: **Safety** (peer-blind to the Completeness axis)
Reviewer mandate: adversarial — this is the review that flips production;
find the reason it must not, or establish there is none.

Object under review: **the activation DECISION** — whether the v1 merge
writer and `--no-ff` may be enabled for production per thin-A1 §2 — at the
tuple:

| repo | pin | verified |
| --- | --- | --- |
| gwz-core | `26f48f5` | ✓ HEAD ("Land the C-1 closure…") |
| gwz-cli | `3cca145` | ✓ HEAD |
| gwz-py | `929efb0` | ✓ HEAD |
| taut | `f008419` | ✓ HEAD |

Governing terms read in order: `GwzM5-8ThinA1Amendment.md` (ACCEPTED —
§1/§2 the activation's definition, §2.11 the ninth-stop-clause re-scope,
§4 the unmoved bindings incl. the R2-F/R5 release gates);
`GwzM5-8R4bR2ConsumerCheckpoint.md` §14 (the nine stop conditions, home
text at :373-386); `CurrentProgramCheckpoint.md` RESUMED records (R2-D
SETTLED at `b91bdeb`, R4b-G ACCEPTED at `1bd885f`, PRE-A1 QUEUE COMPLETE
at `26f48f5`); `GwzM5-8R2DSettledTuple.md` §11 (the A1 input register);
`GwzM5-8R2DPhase4Closure.md` §2.7 (coexistence: quarantine/relocation,
execution pinned to R2-F, fail-closed linkage).

Also consumed and independently spot-verified: the frozen
`GwzM5-8I2CompatibilityContract.md` (§2 writer floor, §4/§5 migration,
§8 retained-reader window, §9 "A1 alone activates the v1 writer floor,
eligible v0 migration, and public no-ff surface in one reviewed change");
`GwzM5-8R4bG-Evidence.md` §8/§12.7-12.9; the M5b design's tripwire table
(T-1..T-6); the C-1 closure commit `26f48f5` itself; and the v0.10.2
released reader's source at its tag.

Verification hygiene: read-only throughout; all test executions under the
mandated scratch `CARGO_TARGET_DIR`; no git write operations; this file is
the review's only write.

---

## 1. VERDICT

**GO — conditional.** The activation decision is sound: no fact was found
that says the v1 writer and `--no-ff` must not be enabled at this tuple.
Every fail-closed gate claimed by the record was re-verified in code and
holds today; the migration surface is exactly the frozen seven-shape
whitelist, and its full O8 property is executed green; the exclusion of
everything else is enforced by code-level predicates I traced, not by
prose; T-5's narrowing survives adversarial re-derivation down to the
v0.10.2 binary's own tagged source; the M5b bound proofs (T-6) and the
no-ff determinism/reverse suites run green at the tuple under my own
execution; the platform record (runs 18-22, nine-of-nine green, run 22 =
1530/0/1 at `b91bdeb` with a zero-production-line delta to `26f48f5`)
carries to this pin by construction.

The GO is conditional in the R4b-G round-1 sense: **§2's enumeration is
the activation implementation's binding spec**, and the findings in §7
are its acceptance criteria. Round 2 of this review verifies the landed
activation change against that spec; any enumerated condition missing in
the landed change is an open finding at its stated severity there, and
L1-19 then applies to it literally.

Finding counts: **0 P0 · 1 P1 · 3 P2 · 5 P3** — all five P1/P2 items are
conditions on the not-yet-written activation change or on the activation
record, not defects in the reviewed tree. The reviewed tree itself is
clean against this axis: every v1 gate fail-closed, zero production v1
reachability, `recover_or_create` uncalled, wire untouched.

---

## 2. THE ACTIVATION PACKAGE, ENUMERATED (the implementer's spec on GO)

The frozen contract defines the activation's whole content:
`GwzM5-8I2CompatibilityContract.md:281-286` — "**A1 alone activates the
v1 writer floor, eligible v0 migration, and public no-ff surface in one
reviewed change**", and the pre-A1 state is "a compile/call-graph
boundary, not a runtime flag." The package is therefore exactly the
following, in one train, at these coordinates (all verified at
`26f48f5`):

### 2.1 Compile gates that fall (`cfg(test)` → production), gwz-core

| # | gate | what it releases |
| --- | --- | --- |
| G1 | `src/workspace_ops/merge/mod.rs:23-24` — `#[cfg(test)] mod v1_lifecycle;` (with the `:25-26` sentinel const, and the module's own `#![allow(dead_code, reason = "v1 lifecycle remains test-reachable until A1 activates production dispatch")]` at `v1_lifecycle/mod.rs:1-4`, which expires with it) | the R4b reverse lifecycle: 59 non-test files, ~13.8k production lines (69 test files ride) |
| G2 | `src/workspace_ops/merge/model/mod.rs:7-8` — `#[cfg(test)] pub(crate) mod v1;` | the v1 canonical model (acceptance/canonical/journal/record/validate) |
| G3 | `src/workspace_ops/merge/acceptance/mod.rs:2-5` — `#[cfg(test)] mod v1;` + `mod v1_candidate;` (plus that file's cfg(test) re-export block) | v1 acceptance construction and candidate build |
| G4 | `src/workspace_ops/merge/record_wire/mod.rs:13-16` — `#[cfg(test)] mod open_v0;` + `mod unknown_fields;` | the migration adapter (adapter/baseline/descriptor/structural/upgrade) and the unknown-field survival manifest |
| G5 | `src/workspace_ops/merge/store/mod.rs:14-15` (+ re-export `:25-28`) — `#[cfg(test)] mod atomic_upgrade;` | the durable v0→v1 record upgrade |
| G6 | the `#[cfg(test)]` re-export blocks in `merge/mod.rs` (notably `:44-50` — `adapt_open_v0_for_r3_tests`, `decode_v0_for_r3_tests`, `decode_v1_for_r3_tests`, `prepare_upgrade`, `verified_v0_descriptor`; `:52-56` `RecordVersion`/`test_v1_record`) | these must be **replaced by production entry points**, not un-gated under their `_for_r3_tests` names; test-only doors that remain test-only (e.g. fault injectors, `archived_fixture_for_test`) keep their cfg |

Test-suite modules stay gated: `merge/mod.rs:29-34` (acceptance_v0,
transition_matrix_v0), `model/mod.rs:17-18`, every `mod tests`.

### 2.2 Runtime/production gates that change — four, and two are coupled

| # | gate | change |
| --- | --- | --- |
| R1 | `src/workspace_ops/merge/validate.rs:12-13` — the typed refusal `"no_ff requires the v1 record lifecycle and is not yet activated"` | falls; `MergeMode::NoFf` start routes to the v1 lifecycle |
| R2 | `src/workspace_ops/merge/runtime/dispatch.rs:116` — the `request.mode != Some(NoFf)` exclusion on `validate_custom_commit_message` | **must fall in the same edit as R1** — these are two halves of one gate. R1 alone would let a NoFf start carry an unvalidated custom message into record creation (the v1 forward path consumes `row.commit_message` from the record, `v1_lifecycle/forward/execute.rs:38/:59`, and performs no request-message validation of its own). The M5b test `custom_messages_validate_while_no_ff_remains_reserved` (`validate.rs:201-224`) is the designed inversion marker. |
| R3 | `src/workspace_ops/merge/store/mod.rs:197-203` — `validate_record`'s v0-only envelope gate (`schema != MERGE_RECORD_SCHEMA \|\| record_schema_version != MERGE_RECORD_SCHEMA_VERSION → unreadable`) | replaced by the contract-§1 envelope registry dispatch: v0 → v0 model (+ the mutating-command adaptation preflight), v1 → v1 decode, anything else → the frozen typed projection |
| R4 | `src/workspace_ops/merge/start/record.rs:75` — creation writes `record_schema_version: 0` | replaced by the contract-§2 writer floor: `max(v1, requested)` — **every new record becomes v1, ordinary and no-ff alike**. This is the part of thin-A1 easiest to under-read: A1 is not "no-ff only"; the A1 row of the frozen creation matrix makes ordinary/custom starts v1 too. |

### 2.3 CLI surfaces (both distributed surfaces, one train)

- gwz-cli `src/globalargs/parser.rs:412-414`: `#[arg(long, hide = true)]`
  unhidden (the comment above it names core as the rejection owner —
  update it); the g01 help pin `src/tests/g01/commands.rs:338`
  (`assert!(!help.contains("--no-ff"))`) inverts; `:313` (mutual
  exclusion) stays.
- gwz-py `src/gwz/cli_merge.py:85`: `help=argparse.SUPPRESS` off; parity
  tests updated. gwz-py routes through the shared core
  (`_gwz_core.abi3.so`) — one core, one activation; no independent
  Python lifecycle exists to drift.

### 2.4 Production callers that land

- `runtime/dispatch.rs` routes Start(NoFf) and every v1-record
  continue/resume/abort/status/gc into the v1 lifecycle service
  (`v1_lifecycle/service.rs:69 run`, today `pub(super)` — gains its
  merge-visible production entry).
- Mutating commands on an **open v0** record gain the adaptation
  preflight: `decode_production_v0` (already production) →
  `adapt_open` → on `Eligible`, the atomic upgrade
  (`store/atomic_upgrade.rs`) then v1 continuation; on `ValidUnlisted`,
  the v0 lifecycle exactly as today; on typed refusal, **see [P1-1]** —
  the v0 lifecycle must remain in command of rows it can already
  recover.
- Read-only status/GC/archive gain the v1 and archive projections
  (contract §6/§7) — code already landed, gains reachability.

### 2.5 What must NOT flip (each verified in code at the tuple)

1. **`recover_or_create` stays without a production caller.** Verified:
   only `cfg(test)` callers exist (`bootstrap/managed.rs:28-29` gates
   `tests_provider`; `catalog/bootstrap/tests.rs`); the allowance at
   `checked_artifact/catalog.rs:10-16` names R2-E behind the Phase-4.3
   criterion. The coexistence decision being MADE (Phase4Closure §2.7,
   variant A quarantine/relocation) does not open this door — the
   decision's own content pins execution to the R2-F relocation package
   and installs the gate "*no production catalog activation at
   `.gwz/checked-artifacts` until the relocation lands*". Status at
   activation: **decided, fail-closed, execution R2-F.**
2. **D2 foreign-filter policy** — release-gated, untouched.
3. **v2/v3/v4 semantics** (branch lifecycle, snapshot source, partial
   composition) — "unsupported requested semantics reject before record
   creation" (contract §2); the A1 writer floor is v1, nothing higher.
4. **T-6 and the v0 forged-action resume gate** — survive A1 for the v0
   lifecycle's lifetime (M5b design §6). Executed by me at the tuple:
   `v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair`
   and `v0_resume_rejects_forged_no_ff_mode_row` — 2/2 green.
5. **The legacy call graphs** — merge store, archive, stash bundles,
   the legacy artifact writers (`gwz.conf/markers`, `LOCK_PATH`) — keep
   their current shapes (amendment §1 named residual). The residual's
   known Windows/durability class rides with them: `durable_fs.rs:33-35`
   `sync_dir` is a no-op on Windows; the store's temp naming is
   pid+sequence; `write_atomic_verified` does `create_dir_all`; the
   record read is unbounded `fs::read`. **v1 records inherit v0-grade
   record durability until R2-E/R2-F** — accepted residual, restated
   here so nobody reads activation as a durability upgrade.
6. **`checked_artifact/observation.rs:216`** (executable leaves classify
   Invalid) and `git/gitbackend/preservation_root.rs:23` (the exact
   durable handoff refusal) — untouched by the package; the operator
   policy question stays post-A1 by record (§6.1).
7. **The R2-F/R5 native-evidence RELEASE gates — unmoved**
   (amendment §4). Activation is not release: no release tag, no
   published artifact, and no retained-reader-manifest "A1 generation"
   row until R2-F/R5 pass. The A1-enabled writer reaches no shipped
   user through this package.
8. **`protocol/generated.rs` untouched** — no new wire (the v1 rows and
   `required_wave` are already frozen in it; verified present).
9. **The re-reserved R2-E families** (`cleanup.*` 11, `barrier.*` 16,
   `terminal.*` 11 keys; the §10 consumer table; the 481-item frozen
   surface) — stay reserved; census 165 stands (verified:
   `EXPECTED_KEY_COUNT: usize = 165` at
   `checked_artifact/interface_tests/fault_expected_keys.rs:174`).

### 2.6 Records that must land with the activation (the GO's paper half)

1. The dual activation-review records, both axes (this file is one half
   of round 1).
2. **The subsumption statement** (amendment §1 branch (a)): the
   activation record states that the v1_lifecycle P2/P3/P4 acceptance
   debt (~3,545 lines: P2 1,941 + P3 1,096 + P4 508) is discharged by
   subsuming R4b-G's full-tree dual acceptance (`1bd885f`, zero open
   P0-P2), the M5b-IMPL settled review GO, and the D3 dual GO/GO.
3. **The operator-signed D3/D4 named-residual disposition** — the
   dirty-boolean fall-through predicates, live and verified by me at
   this tuple: `preservation/cursor.rs:305-306` and `:332-333` (both
   now behind D3 durable-marker fast paths), companions
   `preservation/phase.rs:187,198`, `observe/finalization.rs:217`, and
   the post-D3 third site `cursor.rs:498`. Closure remains
   post-A1-chain work; the signature is the operator's, per the
   amendment.
4. **The PARTIAL byte-equivalence restatement** — carried forward
   verbatim from R4b-G §12.8; never cited green (see §4.4).
5. **The archive-equivalence mechanism decision** for the ten archive
   shapes (R4b-G §12.7 closure (iii)) — owed at activation; the
   registry has no vocabulary for archive shapes, so the decision is a
   mechanism choice, not a registry edit.
6. **The C-2 disposition** — fixtures or the named-residual +
   state-pre-gate condition of [P2-1].
7. **The tripwire inversion record**: T-1 (R1 above), T-2 (the
   `required_wave: A1` decoder refusal inverts to acceptance), T-3 (the
   16-file no_ff surface pin,
   `no_ff_wire.rs:185 no_ff_mode_mentions_stay_inside_the_pinned_surface`,
   re-pinned to the post-activation surface), T-4 (the
   `ForceMergeCommit` construction-site allowlist,
   `no_ff_wire.rs:227`, re-verified — the set should stay
   v1_lifecycle-only). T-6 unmodified.
8. ChangeBudget row for the activation package; checkpoint activation
   record; the §14 ninth-clause post-A1 note (§3 below).
9. **A full native matrix dispatch at the activation commit** — the
   first tree with production v1 code; runs 18-22 cover only the
   `cfg(test)`-era trees.

---

## 3. THE NINE §14 STOP CONDITIONS, WALKED AGAINST THE PACKAGE

Home text `GwzM5-8R4bR2ConsumerCheckpoint.md` §14; all nine bind every
pre-A1 R2 package unchanged (amendment §4); the walk below is against
the activation package itself.

1. **Consumer construction of raw digest/reservation/identity/
   observation/evidence/namespace values** — PASS. The package adds
   callers, not constructors; the caller-restatement class is hardened
   at the seam (4.3 settle item 8), and the one remaining seam shape
   (`BarrierIntentV1::issue`) is a named R2-E obligation with zero
   production callers, unaffected here.
2. **New protocol record/slot/purpose/action/durability phase** — PASS.
   Wire untouched; v1 rows and `required_wave` already frozen in
   `protocol/generated.rs` (verified).
3. **Unbounded read / infallible large allocation** — PASS with named
   residual. Converted families are bounded (verified
   `record.bounded_read` at `checked_artifact/protocol/
   authority_record.rs:357-359` behind a per-kind byte budget; checked
   owner records capped at 16 MiB, `record_wire/checked_owner.rs:9`).
   The merge store's unbounded `fs::read` is the defer-out's existing
   v0 class — rides to R2-E/R2-F, unchanged by this package.
4. **Random scratch allocation on retry** — PASS with named residual.
   The nonce class is extinct in the converted families (4.2/4.3); the
   merge store's pid+monotonic-sequence temp loop
   (`store/atomic_upgrade.rs:145-163`, same convention as the v0
   store since v0.10.2) is deterministic-per-process, not random, and
   is the defer-out's existing graph.
5. **Managed mutation before capability/collision/catalog/admission
   acceptance** — PASS. The v1 lifecycle writes `.gwz/merge`,
   `gwz.conf/markers`, `LOCK_PATH` — legacy areas; no catalog
   activation (§2.5 item 1); no managed-area writer is added.
6. **Recursive parent creation or legacy Windows parent-sync success** —
   PASS with named residual. No converted path touches either; the
   merge store's `create_dir_all` + no-op `sync_dir` are the
   defer-out's existing graphs (§2.5 item 5).
7. **A compatibility shim that lets one consumer bypass the
   coordinator** — PASS. Migration is representation-only (contract §5,
   verified in `atomic_upgrade.rs`: no Git/ref/index/worktree action;
   staged-byte verification, source-unchanged re-check before the
   replacing rename, post-rename hash verification); v0 and v1 both
   route through the single `handle_merge` dispatch.
8. **A filesystem fallback outside the closed support table** — PASS.
   No new fallback; the v1 lifecycle deliberately keeps the legacy
   any-filesystem artifact writers per the defer-out (the
   closed-support-table routes remain checked_artifact's).
9. **Production merge-v1 decode, write, migration, or dispatch
   reachability** — **this is the activation.** Per amendment §2.11 the
   clause is re-scoped, not deleted. **What it becomes post-A1:**
   production v1 reachability is lawful exactly as bounded by this
   review's accepted package enumeration (§2), with the §1
   acceptance-debt obligation discharged per §2.6 items 2-3. Anything
   v1-reachability-shaped beyond the enumeration — a production
   `recover_or_create` caller, catalog activation at
   `.gwz/checked-artifacts`, widening the migration whitelist, v2+
   semantics, a second migration entry point — is outside the
   acceptance and remains stop-condition territory: the post-A1
   R2-E/R2-F packages run under the remaining eight conditions
   unchanged, and any reachability extension takes its own review.

---

## 4. THE SAFETY INPUTS, JUDGED

### 4.1 C-1's four dispositions — derivations verified against the frozen contract, and executed

I re-derived all four from `GwzM5-8I2CompatibilityContract.md` §4/§5
myself, read the closure's tests
(`src/workspace_ops/tests/g23/compatibility_residue_v0.rs`, landed at
`26f48f5`), and executed them: **3/3 green** (with the full `g23::`
partition 114/0, matching the commit's own pin; total lib census 1573,
matching the reported partition off-by-one figure).

- **F-BASELINE → DISPOSITIONED-UNLISTED.** Correct. The durable phase is
  record-only (contract §4: "Live Git … cannot select an earlier durable
  phase"), so the row cannot match rule 5 (evidence-recorded, pinned at
  step `committing_evidence`), and its observation
  (`observation.root = recorded_evidence`) fails rule 6's
  `prefix_boundary` — zero matches → `:159` "Zero whitelist matches is
  not an error", bytes exact. The decisive measurement is real: the test
  asserts the descriptor's `publication` block **byte-identical** to the
  whitelisted rule's with `observation.root` the single separating
  field, in both directions, with the fully-staged F-BOUNDARY positive
  control adapting in the same test. The coincidence C-1 feared is
  measured, not assumed.
- **F-MARKER / F-LOCK → DISPOSITIONED-REFUSED**, typed
  `PublicationPrefixMismatch` before staging, message carrying the
  registry's own frozen rejection sentence. Correct per `:123-125`
  (excluded by name) read with `:121-123` (exactness) and `:167-169`
  (rejects before staging): a torn mid-write observation is not one
  exact recorded prefix, so no descriptor exists to match, and the
  fail-closed answer is the typed refusal — with **zero mutation**,
  which the tests pin. The refusal channel itself is frozen-contract
  vocabulary (the registry's `rejection_reasons`), not a C-1 invention.
- **J-NO-PUBLICATION-UNBORN → ADAPTED (the unborn twin), per case** —
  and the label correction is real and load-bearing: the whitelist has
  no born-root rule at all (every rule's baseline is
  `unborn_attached`/`root_commit_hash: absent`; `:124` excludes "born
  root"), so the ADAPTED case is the UNBORN twin and the born twin is
  valid-unlisted; the terminal twins stay v0 for both roots
  (`:162-163`). All executed in two tests. **The A1 review must not
  read the old §12.3 J-BORN label as migratable — it is not**, and the
  activation record inherits this correction.
- **Reader integrity:** the test expectations are pulled from the same
  frozen registry the production adapter `include_str!`s
  (`GwzM5-8I2CompatibilityPredicates.json`, bound at
  `record_wire/open_v0/adapter.rs:21-22` and
  `tests/g23/compatibility_v0.rs:10`) — "never hand-copied" holds
  structurally.

One consequence of the F-MARKER/F-LOCK refusal disposition is a live
activation-wiring hazard — **[P1-1]** below. The dispositions are
correct as migration answers; the danger is what production dispatch
does with a refusal on resume.

### 4.2 C-2's four unfixtured scenarios — does any become reachable at activation?

**Yes, two of the four gain a new production code path.**
`B-NOT-STARTED` and `B-PREPARING-EMPTY` are open v0 progress shapes a
pre-A1 binary's crash can leave on disk; post-A1, a mutating command on
them traverses the NEW adaptation preflight. The adapter's order
(verified, `open_v0/adapter.rs:80-101`) is: envelope → NoFf check →
**`validate_v0_structure`** → `classify_open_v0`. The structural
validator carries a substantial typed-refusal surface
(`structural.rs`: MergeRecordUnreadable, RecoveryEvidenceMismatch,
TerminalEvidenceMismatch, UnexpectedPublicationEvidence,
CandidateIntegrityMismatch, TerminalRollbackMismatch,
RecordedEvidenceDrift…) and runs BEFORE the cheap state
pre-classification that would route non-Finalizing rows straight to
ValidUnlisted. With **zero fixtures**, whether a legal NotStarted /
Preparing-empty crash row passes that validator is unmeasured — the
exact C-2 worry, now with a reachability date. The two archive shapes
(`AC-NOPUB-UNBORN`, `AP-PRESERVED`) gain no new reachability: archived
v0 never migrates and uses only the archive decoder/projection
(contract `:163-164`). Disposition: **[P2-1]**.

### 4.3 The T-5 lane-owner narrowing — JUDGED: ACCEPTED

The question: is header-keyed envelope classification sufficient
evidence that a v0.10.2 reader refuses a no_ff v1 record, or does
activation demand the literal fixture pair? I re-derived the narrowing
adversarially at three levels, each verified directly:

1. **The matrix tooling** (`retained_reader_matrix.py`): fixture
   identity is the `(schema, record_schema_version)` pair
   (`ENVELOPE_FIXTURES`), case identity is
   (pair, lifecycle, projection, classification), duplicates are
   refused ("envelope matrix is not exact"), and every unsupported
   envelope must classify `record_unreadable`. A body-mode-
   differentiated case is **inexpressible in the tooling's
   vocabulary** — the structural ground the narrowing claimed, held.
2. **The manifest contract** (`retained_reader_harness.py:88-100` +
   `manifest.json`, dumped): both v0.10.2 surfaces declare
   `header_guarded` dispatchers whose unsupported set is exactly the
   six pairs, v1/1 → `record_unreadable` among them.
3. **The released reader itself** — the level neither cited surface
   reaches, checked from the `v0.10.2` tag's source: every decode
   route (`discover_open` :30, `load` :45, `load_archived` :54,
   `archive` :75/:81) funnels through one `read_record` →
   `validate_record`, which refuses on
   `schema != v0 || record_schema_version != 0` unconditionally on the
   success path. And v0.10.2's model **already knows both mode
   variants** (`model.rs:15-19`, `MergeExecutionMode::{Normal,FfOnly,
   NoFf}` with serde default), so `mode: no_ff` cannot even alter the
   deserialization branch relative to an ordinary body: for a fixed v1
   construction differing only in mode, v0.10.2's behavior is
   **identical by construction** — refusal decided by the header pair,
   no mutation, on every route.

The only body-dependence anywhere is *which* unreadable message fires
when a body also fails v0 deserialization — a message-text nuance the
saved candidate pair will pin when it rides. Judgment: the narrowing is
**sufficient for activation**. The literal fixture's cost claim
(five-surface frozen-contract change + network regeneration of
executed-binary evidence) was measured, not assumed; the candidate pair
is BUILT and digest-saved
(`scratchpad/insurance-t5/CANDIDATE-generator-t5-pair.NOT-APPLIED.patch`)
and **must ride R2-F's native-evidence regeneration** — which is the
release gate, so the pair executes against real binaries before any
shipped user meets a v1 record. That carrier obligation is binding
(§11.1 already records it); restated as part of [P3-1]'s record.

### 4.4 The PARTIAL byte-equivalence statement — weighed

The O8 battery ("byte-equivalent accepted lock, candidate, root output,
and archive results across all M4") stands PARTIAL: 7 proven +
6 refusal-pinned of 39 at R4b-G; the C-1 closure adds the 4 live
residue rows (1 adapted-per-case, 2 refused-typed, 1 unlisted + the
terminal twins) — **17 of 39 now carry per-row execution; 18 remain
class-cited; 4 have no fixture.**

What activating with that denominator means for live flows, weighed:

- **The live migration surface is the whitelist, and the whitelist is
  fully proven.** Only the seven one-member Finalizing normal-mode
  shapes can migrate, and those seven carry the complete O8 property
  (descriptor + canonical-hash equality, restart-action identity,
  accepted-lock byte identity, the four-point upgrade fault matrix,
  unknown-field survival, no-repo-mutation) — executed, and re-executed
  by me inside g23 114/0.
- **The exclusion of the other 32 is code, not prose.** I traced
  `classify_open_v0` (`adapter.rs:42-78`): mode ≠ Normal, state ≠
  Finalizing, drift, multi-member, `@root`, born root, non-Member,
  pending action, preservation, rollback/evidence-rollback,
  root-publication — each excluded class of contract `:123-125` has an
  explicit predicate arm answering ValidUnlisted before any descriptor
  work, and open `no_ff` v0 rows get the typed `UnsupportedLegacyMode`
  refusal (`adapter.rs:88-96`) exactly per contract `:164-165`. The
  categorical disposition is therefore **predicate-enforced**, with 18
  of the excluded rows additionally carrying executed v0 byte/restart
  pins.
- **The counterweight's honest limit:** the class predicates are only
  partially executed per-arm (several arms have no driving row), the
  archive-equivalence mechanism decision is still owed (§2.6 item 5),
  and the C-1 test itself demonstrated why per-row execution matters
  (F-BASELINE's publication block IS byte-identical to a whitelisted
  rule's — one field away). So the PARTIAL label stays load-bearing:
  the activation record restates it, never cites green, and the
  parametric ValidUnlisted-plus-byte-preservation sweep over every
  non-whitelisted fixture (R4b-G §12.7 cheap closure (ii)) is named
  hardening for the next train — cheap, census-neutral, and it would
  convert the 18 class-cites into executed rows.

Weighed conclusion: activating at 17-executed/18-predicate-enforced/
4-unfixtured is **safe for the day-one flows this package opens**,
because the unproven region is exactly the region the code refuses to
migrate; the exposure that remains is refusal-behavior exactness (C-2's
two B-shapes, [P2-1]) and record completeness, not migrated-output
correctness.

### 4.5 The M5b-IMPL settled review and its [P3-1]

Consumed: GO (0 P0/P1, 1 P2 closed by the T-5 narrowing judged above,
3 P3), discharging the J-1 pre-A1 obligation on its frozen tier. Its
[P3-1] (the forged-action gate's ChangeBudget row, filed
existence-first) is bookkeeping riding the ChangeBudget's OPEN
convention — no safety content.

---

## 5. THE WRITER'S OWN SAFETY RECORD — SPOT-VERIFIED

What I judged decisive for a production flip, and verified directly:

1. **The A1 preconditions themselves.** R2-D settled (dual #3 GO/GO at
   `b91bdeb`) and M5b bound proofs green — I re-executed T-6 (2/2) and
   the M5b/no-ff suites at the tuple: `no_ff` determinism 10/10,
   `reverse_no_ff` 5/5, plus `drift` 5/5 and `root_drift` 2/2
   (drift-rejection). All green under the mandated scratch target.
2. **The migration's atomic boundary.** Read in full
   (`store/atomic_upgrade.rs`): staged bytes fsynced and re-read-
   verified, source re-read compared before the replacing rename
   (TOCTOU narrowed), `rename_durable` + post-rename hash
   verification, the four injected fault points exactly matching the
   executed matrix (BeforeStageWrite / AfterStageFsync /
   BeforeAtomicRename / AfterRenameBeforeVerification). Windows
   crash-convergence is by construction: either side of the rename is
   a valid state (v0 → re-attempt; v1 → proceed), which is the correct
   shape given the store's no-op `sync_dir` on Windows.
3. **The settled tuple's per-key evidence** — spot-checked the family
   the merge writer leans on most (`record`, §4.4: 13/13, 9+4
   repeatability partition machine-checked): the cited
   `record.bounded_read` site is real and bounded
   (`authority_record.rs:357-359`); the census constant 165 is frozen
   in code (`fault_expected_keys.rs:174`).
4. **The platform record.** Tuple §9: runs 19/20/21 nine-of-nine green
   with run IDs; run 22 green 1530/0/1 at `b91bdeb`
   (`GwzWindowsMatrix-Classification.md`); and I verified
   `git diff b91bdeb..26f48f5` is **+1,183 lines, zero production
   `src/` lines** (scripts + test tree only), so run 22's evidence
   carries to this pin by construction. The three new C-1 residue
   tests have no native Windows/ARM execution yet — folded into the
   §2.6 item 9 matrix dispatch ([P3-2]).
5. **The E-table's Windows arms.** The anchor package's E10/E14
   activation annotation and E9 writer-class-conditional durability
   are carried in the freeze contract form and were probe-verified
   native (runs 32569434565, 32612243125) before landing; the legacy
   AnchoredPrivateArea suites returned green in run 18 (1472/0/1),
   discharging the tracked acceptance item. Nothing in this package
   touches those arms.

---

## 6. WHAT ACTIVATION EXPOSES ON DAY ONE

### 6.1 The operator policy question (executable-template repos)

Confirmed activation-neutral in code: the typed refusal chain
(`checked_artifact/observation.rs:216` executable→Invalid;
`git/gitbackend/preservation_root.rs:23` "root preservation preparation
requires the exact durable handoff") is production TODAY on the v0
`--abort --preserve` path and is untouched by the package. **Activation
does not silently change the policy.** It does raise the question's
incidence: the no-ff reverse lifecycle leans on preservation handoffs
more heavily than plain v0 fast-forward flows, so repositories
initialized from executable template trees (Ubuntu images ship exactly
this) will meet the refusal more often. The recorded post-A1 policy
question (doc note vs a mode-tolerant arm for the one boundary file,
`GwzArmPreservationHandoffDiagnosis.md`) should ride the activation
record so the support surface is forewarned — [P3-4].

### 6.2 The L2-05 / CI-wiring gap — activation makes unwired gates load-bearing

Verified against `.github/workflows/` at the tuple: wired are the
boundary lane (`check_checked_artifact_boundaries.py` + unittests +
clippy + the T-3/T-4 tripwire scans via `no_ff_wire` on every push),
`check_merge_compatibility_predicates.py` (retained-readers lane), the
matrices, and the retained-reader harness. Wired **nowhere**:
`check_m4_scenario_map.py`, `run_r4bg_aggregate_gates.py`,
`test_v1_lifecycle_privacy_probe.py` (the three R4b-G tools, per the
Correctness axis's J-7 ruling already on the register), and
`check_merge_docs.py`/`test_check_merge_docs.py` (L2-05, resolvable
only where the sibling gwz-cli checkout exists). **At activation these
stop being bookkeeping**: the M4 map is the machine record C-1's
closure rests on; the privacy probe and call-graph guard are the
"standing guard" the J-1 acceptance leaned on (a guard that runs
nowhere stands only on prose); and the merge docs become the public
`--no-ff` documentation surface. Condition [P2-2]: wire them at or
before the activation commit.

### 6.3 MAX_PATH (~173-char `ca1-*` names)

Activation-neutral on the merge path, verified: the v1 lifecycle
publishes through the legacy artifact writers
(`v1_lifecycle/finalization/execute.rs:45/:48` → `artifact::write_atomic`,
markers under `gwz.conf/markers`, `MARKER_DIR` at `artifact/mod.rs:25`)
— no new `ca1-*` writes. The 173-byte `.source` name class lives in the
checked-artifact private area, production today wherever preservation
handoffs run; no-ff raises that traffic (same vector as §6.1). The
exposure and its retirement stay pinned to R2-F relocation
(tuple §11.2) — unmoved by this package, and this GO does not touch it.

---

## 7. FINDINGS

**[P1-1] — Adapter-refusal rows must not lose their v0 recovery at
activation.** `characterization_publication_prefix_v0.rs` proves the v0
lifecycle resumes marker-only, lock-only, boundary, and staging crash
prefixes to Completed **today** (I re-ran it green inside g23). The C-1
dispositions make `adapt_open` refuse F-MARKER/F-LOCK typed
(`PublicationPrefixMismatch`). If the activation's dispatch surfaces
that refusal as the resume outcome, currently-recoverable crash states
become permanent wedges — a day-one regression on the exact rows C-1
just dispositioned. The contract's own text points the safe way:
`:160-161` "An existing mutating v0 command remains on the existing v0
lifecycle and may write v0 only when that path's existing preflight
authorizes it." **Condition:** the activation package pins, with an
executed test, that a post-activation `merge --resume` on an F-MARKER
(and F-LOCK) row completes exactly as today — the typed refusal is the
migration's answer, never the command's. (Spec item; violation in the
landed change is P1 open at round 2.)

**[P2-1] — C-2's two progress shapes meet a new, stricter preflight
with zero fixtures.** §4.2. **Condition (either):** (i) the production
dispatch gates adaptation on the cheap state pre-classification —
non-`Finalizing` open v0 rows skip the adaptation preflight entirely
and take the v0 path by construction (contract-compliant: only
Finalizing shapes are whitelisted) — or (ii) the four C-2 fixtures
(`B-NOT-STARTED`, `B-PREPARING-EMPTY`, `AC-NOPUB-UNBORN`,
`AP-PRESERVED`) land with the activation. If (i), the two archive
shapes still ride as named residual on the record.

**[P2-2] — CI-wire the gates activation makes load-bearing.** §6.2.
`check_m4_scenario_map.py`, the v1_lifecycle privacy probe, and the
call-graph guard into the push lane at or before the activation
commit; `check_merge_docs.py` joins when `--no-ff` docs land (its
gwz-cli-manifest dependency needs the wiring fix the tuple §11.3 item 7
already describes).

**[P2-3] — The activation record set is incomplete until produced.**
§2.6 items 2-6: the subsumption statement, the **operator-signed**
D3/D4 named-residual disposition, the PARTIAL restatement, the
archive-equivalence mechanism decision, the C-2 disposition. All are
record acts; none require engineering beyond [P2-1]'s option (ii). Per
amendment §1 branch (a)/(b), the review may not close without them —
this finding is the L1-19 teeth behind that sentence.

**[P3-1] — Tripwire inversions and the T-5 carrier.** T-1/T-2 invert,
T-3's 16-file pin re-pins, T-4's allowlist re-verifies, T-6 stays; the
saved T-5 candidate pair (digests e4ea14de…/a1cb103c…) rides R2-F's
native-evidence regeneration as a named input — the release gate
executes it before any user meets a v1 record.

**[P3-2] — Native execution debt of the closure tests.** The three C-1
residue tests (+18 helper lines) have no Windows/ARM run yet; covered
by the §2.6 item 9 activation-commit matrix dispatch — pre-attribute
them expected-green in the ledger per convention.

**[P3-3] — v1 records inherit v0-grade Windows record durability**
(no-op `sync_dir`, unbounded record read, pid+seq temp names) until
R2-E/R2-F — the amendment's named residual, restated so the activation
record cannot be read as a durability claim. The R2-F power-loss
companion (tuple §11.2) is the closure vehicle.

**[P3-4] — Day-one support surface notes**: the executable-template
typed refusal's incidence rises with no-ff preservation traffic (§6.1);
MAX_PATH unchanged but same traffic vector (§6.3). Both ride the
activation record as operator-visible notes.

**[P3-5] — Post-battery hardening**: the parametric
ValidUnlisted+byte-preservation sweep over every non-whitelisted
fixture (R4b-G §12.7 (ii)) converts the 18 class-cites into executed
rows for one cheap test; recommended for the activation train or the
first post-A1 package.

---

## 8. WHAT THIS GO DOES NOT LICENSE

Enabling the v1 writer and `--no-ff` per thin-A1 §2 at this tuple, on
the §2 enumeration, with §7's conditions — and nothing else. It does
not license: a production `recover_or_create` caller or any catalog
activation at `.gwz/checked-artifacts` (R2-E/R2-F, behind the
relocation gate — fail-closed in code today, verified); D2
foreign-filter changes (release-gated); any migration-whitelist
widening or second migration entry; v2+ semantics; any release — the
R2-F/R5 native-evidence gates remain the barrier between this writer
and shipped users, and the retained-reader manifest gains its A1
generation only from a published release. The eight surviving §14 stop
conditions bind every post-A1 R2-E/R2-F package unchanged; the ninth
binds them too, in its re-scoped form (§3 item 9).

— Safety axis, round 1 of 2. Round 2 reviews the landed activation
change against §2's enumeration and §7's conditions.
