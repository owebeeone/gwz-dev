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

---

# Round 2 — focused re-verdict on the delivered package

Date: 2026-08-25
Axis: **Safety** (peer-blind held; no Completeness round-2 material read)

Object: the activation package DELIVERED IN-TREE, UNCOMMITTED, over the
accepted tuple — verified at review start: gwz-core `26f48f5` + **85 M /
3 new = 88** dirty files (`model/version.rs`, `v1_lifecycle/start.rs`,
`tests/g23/a1_activation.rs` the new three), gwz-cli `3cca145` + 3,
gwz-py `929efb0` + 2. Builder's report:
`GwzM5-8A1ActivationPackage-Report.md` — used as a map only; every
ruling below is against the tree, and every gate quoted was re-run by
me under the mandated scratch `CARGO_TARGET_DIR`.

## R2.0 VERDICT

**GO.** The delivered package implements the round-1 §2 enumeration
faithfully — including the one place it deliberately stops short (R4's
ordinary-start floor), which I rule an **accepted named residual**
below. Conditions [P1-1], [P2-1], [P2-2] are MET as delivered ([P2-2]
with a named L2-05 residue); [P2-3] is excluded from this round per the
coordinator (landing-train records, operator reply pending). The
must-not-flip table re-verified 9/9 with F-3 at 0 hits. Every gate the
builder claims green I reproduced green: the full lib census **1583**
(1582 passed / 0 failed / 1 ignored, in four partitions incl.
`root_fault_matrix` 333.46 s release), gwz-cli **139/0**, gwz-py
**330/0 against a native module I rebuilt from the dirty core myself**,
clippy `-D warnings` clean, and the three A1-load-bearing checkers ok.

Round-2 findings: **0 P0 · 0 P1 · 1 P2 · 5 P3.** The one P2 is a
correction to the recorded expected-red ruling, not to the package: the
11 probe-harness failures decompose into **four cure classes**, and the
lane-owner ruling on record cures only one of them — applied alone, the
landing tree stays red (§R2.7).

## R2.1 §2.1 compile gates — SATISFIED, wider fall RULED IN SPEC

All six verified in the tree: `merge/mod.rs:23` bare `mod v1_lifecycle;`
(sentinel const gone — see §R2.7c); `model/mod.rs:7` bare
`pub(crate) mod v1;`; `acceptance/mod.rs:2-3` bare; `record_wire/
mod.rs:13-14` bare `mod open_v0; mod unknown_fields;`;
`store/mod.rs:14` bare `mod atomic_upgrade;` with the production
re-export `upgrade_open_v0` at `:25`. G6 is **REPLACED, not un-gated**:
`_for_r3_tests` is extinct in code (one explanatory comment survives,
`merge/mod.rs:41`); production names are `upgrade_open_v0`,
`prepare_upgrade`, `decode_production{,_v0,_v1}`, `decode_archived`;
the seam-type re-exports at `merge/mod.rs:46-50` and
`record_wire/mod.rs:20-22` stay `#[cfg(test)]` as doors. Fault
injectors (`fail_next_candidate_publication_after`,
`fail_next_evidence_rollback_after` — re-exports cfg-gated at
`merge/mod.rs:33-38`), `archived_fixture_for_test`, the managed test
door, and `handle_merge_with_dependencies` all keep their cfg.

**The wider fall (97 further `#[cfg(test)]` + 28
`#[cfg_attr(test, derive(serde::Serialize))]`) is RULED within the
enumeration, not scope creep.** I counted the diff myself: 124 removed
`#[cfg(test)]` lines, 28 removed / 0 added `cfg_attr(test,
derive(serde::Serialize))`. The 28 are logically part of G2 — a v1
record cannot be *written* by production if its `Serialize` is
test-only — and the 97 are the transitive closure my G6 instruction
already implied ("test-only doors that remain test-only keep their
cfg" defines the complement: v1-facing helpers consumed by the
production surfaces fall with the gates). Behaviour-neutrality is
evidenced at the final tree by my own executions (§R2.8), clippy
clean, the privacy battery's sealed compile probes ok, and F-3 at 0
hits.

## R2.2 §2.2 runtime gates — R1/R2/R3 SATISFIED; R4 RULED

- **R1+R2 in one edit, verified:** `validate.rs:12-25` carries the
  T-1 inversion comment and the COUPLED paragraph naming §2.2 R2; the
  NoFf refusal is gone. `runtime/dispatch.rs:256-262` carries the twin
  COUPLED comment; the `mode != Some(NoFf)` exclusion is gone and
  every start validates its message before creation. The designed M5b
  marker is inverted and renamed:
  `custom_messages_and_no_ff_both_validate_after_activation`
  (`validate.rs:215`), and the end-to-end coupled-pair test rides in
  `g23::a1_activation` (executed, §R2.8).
- **R3 verified:** `classify_merge_record_header`
  (`record_wire/header.rs:140`) over the closed five-row ALLOCATIONS
  table (`:74-104`) — v0/v1 dispatch installed under
  `InstalledMergeRecordVersions::PRODUCTION` (`:31`), v2-v4
  `dispatch: None` with frozen waves A2/A3/A4, the
  recognized-schema-wrong-number arm present
  (`RecognizedSchemaVersionMismatch`, `:50-54`). `V0_ONLY` retained as
  the v0 store's own set (`header.rs:36`, consumed at
  `decode.rs:119` and `store/mod.rs:341`). **T-2 inverted at both
  sites**: header dispatch (`decode.rs:97`) and the archive decoder
  (`record_wire/archive/mod.rs:59`).
- **R4 — THE RULING: the `V0` floor for ordinary starts is an
  ACCEPTED NAMED RESIDUAL, not a hard A1 gate.** Verified in the
  tree: `select_record_version = max(ACTIVE_WRITER_FLOOR, semantic)`
  is production and wired at the creation site
  (`start/record.rs:55-57`, replacing the hard-coded v0 envelope);
  `RequestedSemantics::NoFf → V1` (`version.rs:71`), so `--no-ff`
  writes v1 end-to-end today (executed:
  `the_production_writer_floor_writes_a_v1_record_for_no_ff`,
  `no_ff_start_publishes_a_two_parent_integration_commit`); v2-v4
  reject before creation with their waves (executed); the floor is
  `V0` at `version.rs:39` with the partial-engagement doc comment;
  the `max` is pinned by
  `raising_the_floor_to_v1_makes_every_installed_semantic_v1`.
  Grounds for the ruling: (i) thin-A1's operative decision text — "A1
  enables the v1 writer and `--no-ff` on the accepted R4b lifecycle"
  — is delivered: the v1 writer exists in production (creation,
  decode, dispatch, migration, projections, archive). (ii) The
  under-flip is the conservative side of contract §2's A1 row: not
  writing v1 for ordinary starts cannot regress any reader, whereas
  the builder MEASURED the engaged-V1 alternative breaking every
  ordinary start (no production v1 owner for root participants,
  dry-run prediction, drift/conflict response, the v0 event stream) —
  landing that would have been the unsafe act. (iii) The hybrid state
  is per-record coherent: every reader dispatches on the envelope;
  mixed stores are the retained-reader window's own model; a new v0
  row crashing into a whitelisted shape migrates on resume (proven —
  §R2.5, plus the builder's positive Boundary/Staging evidence); and
  no new v0 *shape* is introduced (`NoFf→V1` means `mode: no_ff`
  never serializes into a v0 record — T-3 guards it). (iv) The
  remainder is a one-line constant behind a named blocking owner.
  **Binding conditions attached to this ruling** (severity P2 if
  missing at landing; they join the [P2-3] record set): (a) the
  ordinary-start v1 owner becomes a first-class named milestone on
  the register, and the floor raise lands WITH it as one reviewed
  change — contract §9's "one reviewed change" discipline transfers
  to the remainder; (b) a dated annotation lands on the frozen
  contract §2 creation-matrix A1 row (or its errata ledger) recording
  the partial engagement and pointing at `version.rs:39` — frozen
  text must not diverge silently; (c) the release-time
  retained-reader "A1 generation" manifest work describes the SHIPPED
  behaviour (ordinary=v0, no-ff=v1), not the contract row.

## R2.3 Round-1 conditions

- **[P1-1] — MET.** `dispatch.rs:394-416 adapt_before_mutating`:
  preflight only on `Resume|Abort`, only when the [P2-1] precheck says
  `MayAdapt`, and **every non-`Upgraded` answer — `ValidUnlisted` and
  `Err(_)` alike — returns `Ok(false)`, leaving the v0 lifecycle in
  command**; the doc comment carries the [P1-1] reasoning. The test
  `post_activation_resume_completes_the_refused_v0_crash_prefixes`
  (`g23/a1_activation.rs:164-218`) drives F-MARKER and F-LOCK through
  post-activation production dispatch, asserts `version==V0` and
  `adaptation==MayAdapt` (so the refusal arm is genuinely traversed),
  and requires `Completed` with no open record. Executed green;
  `characterization_publication_prefix_v0` green inside g23 119/0.
- **[P2-1] — MET via option (i).** `store/mod.rs:250-258 precheck`
  reads exactly two scalar fields (state, mode) and calls no
  validator; `classify_open_record` (`:206`) uses
  `decode_production` — for v0 bodies the SAME strict decoder the v0
  store already used at the tuple, so the skip path adds no new
  refusal surface. The walk test
  (`a1_activation.rs:233-302`) pins six non-Finalizing states plus
  FfOnly/NoFf to `Skip` and Finalizing+Normal to `MayAdapt`.
  B-NOT-STARTED / B-PREPARING-EMPTY cannot reach
  `validate_v0_structure` through the new path; the two archive
  shapes ride as named residual. A NoFf v0 open row Skips into the v0
  path where the §3.5a forged-mode gate refuses it — T-6 executed
  green (§R2.8), so the skip does not bypass that refusal.
- **[P2-2] — MET in substance; L2-05 residue ACCEPTED as recorded.**
  `checked-artifact-boundary.yml:+40-48` adds the push-lane step
  running `check_m4_scenario_map.py` and
  `run_r4bg_aggregate_gates.py privacy call-graph` — through the
  runner, so counts are pinned. The L2-05 non-wiring comment
  (`:+49-58`) records the true mechanical blocker (manifest resolves
  `gwz-cli/docs/*` from the workspace root; the job checks out
  gwz-core alone; cites tuple §11.3 item 7). RULED: satisfies [P2-2]
  for this round because (i) the three gates activation made
  load-bearing ARE wired, (ii) the highest-risk drift class for the
  new public flag — stale CLI docs — is separately CI-guarded by
  gwz-cli's own `g00` docs-match test (I ran the cli suite green),
  and (iii) I executed `check_merge_docs.py` locally: "merge document
  consistency: ok (11 sources, 147 assertions)". The L2-05 CI-wiring
  item STAYS OPEN on the register ([P3-R2-3]) — accepted, not
  discharged.
- **[P2-3] — excluded from this round per the coordinator.** The
  landing record set (subsumption statement, operator-signed D3/D4
  disposition, PARTIAL restatement, archive-equivalence mechanism
  decision, C-2 named-residual record) plus this round's additions:
  the R4 ruling's conditions (a)-(c) above and the §R2.7 harness
  cures.

## R2.4 Must-not-flip — 9/9 RE-VERIFIED INDEPENDENTLY, F-3 at 0 hits

1. `recover_or_create`: the entire `checked_artifact/` diff is ONE
   file — `coordinator/identity.rs`, and the diff is exactly the
   one-line `#[cfg(test)]` removal on the
   `CheckedOwnerRecordVersion::V1 → OwnerVariantV1::MergeRecordV1`
   match arm (G1-forced; the arm must exist once production can
   observe v1 owner records). No new callers (grep re-run); the
   `catalog.rs:10-16` R2-E allowance intact verbatim.
2. D2 files (`git/gitbackend/recovery_support.rs`,
   `merge_recovery.rs`): zero diff.
3. `RecordVersion` still closed `{V0,V1}` (`model/v1/canonical.rs`);
   v2-v4 reject before creation (executed).
4. T-6 executed: `v0_resume_rejects_forged_no_ff_mode_row` and
   `…_two_parent_action_over_fast_forwardable_pair` — verbatim tail:
   "test result: ok. 2 passed; 0 failed" (0.30 s).
5. `durable_fs.rs`: zero diff (no-op Windows `sync_dir` intact);
   pid+sequence temp naming intact. The one added `fs::read` (the
   envelope probe in `classify_open_record`) is the same unbounded
   class as the v0 record read it precedes — rides [P3-3] unchanged.
6. `observation.rs` and `preservation_root.rs`: zero diff; the
   executable-Invalid classification and the durable-handoff refusal
   stand at their round-1 coordinates.
7. Release gates: `release.yml` and
   `scripts/retained_readers/manifest.json` zero diff — no A1
   generation named pre-release, R2-F/R5 unmoved.
8. `protocol/generated.rs`: zero diff — no new wire.
9. Census: `EXPECTED_KEY_COUNT: usize = 165` intact
   (`fault_expected_keys.rs:174`); `checked_artifact::` partition
   400/0.

**F-3 at 0 hits, proven positively:** the boundary checker
*accumulates* findings before exiting (`check(…)` collects, prints
all, returns 1 — `check_checked_artifact_boundaries.py:1239-1244`).
Its output on the delivered tree is exactly six lines — the header
plus the five digest findings — with **zero** "v1 lifecycle names the
v0 persistence seam" lines (violation message at `:991`), so the
v1→v0 persistence scan ran and found nothing. The privacy battery's
sealed compile probes: "ok … (105.4s, 'OK')".

## R2.5 The moved behaviour pin — ACCEPTED, one compensating obligation NAMED

`g23::finalization::resumed_finalization_persists_each_phase_before_a_nested_mutation_fault`:
the rewritten final window is behaviour-truthful — the crash row is
`CommittingEvidence`, one of the frozen seven whitelisted shapes, so
post-activation production resume migrates it and the v1 lifecycle
completes it; the v0 injector is a v0-finalizer seam the v1 path never
consults. The new assertion (`source_version == V1` + no open record)
pins whitelist eligibility, so a regression returns the row to the v0
path and trips it. The premise that the test's first three windows
keep their v0 coverage is verified in code: the DI seam
(`handle_merge_with_dependencies`, `dispatch.rs:207-225`) routes
through `AbsentV1Router`, which never migrates and fails closed
(`dispatch.rs:98-101`), while only the production entries carry
`AuthorityV1Router` → `adapt_before_mutating` (`:201`, `:66-73`).

**The named compensating obligation:** the builder's consequence is
real, and one arm is now uncovered — the **eligible-row
upgrade-failure fallback**: on a whitelisted Finalizing row whose
atomic upgrade FAILS (I/O, race), `adapt_before_mutating`'s
`Err(_) → Ok(false)` puts the v0 lifecycle back in command, and no
executed test drives that composed path ([P1-1]'s rows are
refusal-class, never eligible). Fail-safe by construction and the v0
completion of these shapes is proven through the DI windows, but the
composed production path (eligible → upgrade fails → v0 completes)
deserves one executed test. The production call hardcodes
`AtomicUpgradeFault::None`, so the test needs a filesystem-level
fault. **Carrier: R2-E** (or the landing train if cheap) —
[P3-R2-2].

## R2.6 [P3-1] tripwires — VERIFIED

T-1 inverted at the gate (`validate.rs:12-25`) with the renamed marker
(`custom_messages_and_no_ff_both_validate_after_activation`,
`validate.rs:215`) and the end-to-end coupled-pair test. T-2 inverted
at both sites (§R2.2). T-3 re-pinned 16→19 — I counted 19 entries in
the live pin, the three additions annotated (`model/version.rs`,
`v1_lifecycle/start.rs`, `tests/g23/a1_activation.rs`). T-4 executed
by me: `force_merge_commit_construction_sites_stay_v1_lifecycle_only`
— "ok. 1 passed; 0 failed" (construction still v1_lifecycle-only).
T-6 untouched and executed green.

## R2.7 Expected-red — CONFIRMED, with one attribution CORRECTION [P2-R2-1]

- **The five boundary-checker digests: exact.** My run printed
  precisely the five findings the report lists — compiler-root
  manifest `merge/mod.rs`; protected sources `preserve/artifacts.rs`,
  `preserve/plan.rs`; protected trees
  `v1_lifecycle/authority/observe.rs`, `v1_lifecycle/mod.rs` — and
  nothing else. Landing re-pins are the lane owner's recorded duty.
- **The three driver count markers: reproduced.** 255
  (v1_lifecycle−root_fault_matrix), 926 (remainder), 119 (g23) — all
  green; only the expected strings are stale (254/917/114).
- **The probe harness: 11/69 reproduced by name — but the recorded
  single-cause ruling is INCOMPLETE.** The 11 decompose into FOUR
  cure classes:
  (a) **probe-compile dead-code** — the `run_compiler_probe` family
  (`test_check_checked_artifact_boundaries.py:27-59` runs clippy
  `-D warnings` on a mutated copy; the injected unused probe fns now
  fail dead-code since G1's blanket allow expired). The recorded
  lane-owner fix — emit `#[allow(dead_code)]` with the injected probe
  text — cures exactly this class, and **does not weaken F-3**: the
  allow silences only the lint on the injected item so the probe
  compiles and the CHECKER remains the rejector; the checker's
  textual seam scan ignores allow attributes, and for *used*
  violations nothing changes.
  (b) **stale-digest class** — `test_current_source_inventory_is_
  classified` (`:63`), `test_compiler_root_manifest_allows_non_
  target_metadata_change` (`:620`), `test_comments_and_strings_do_
  not_create_false_references` (`:1208`) fail only because the
  checker fails on the five stale digests; cured by the landing
  re-pins, not by (a).
  (c) **the sentinel** — `test_v1_compiler_root_has_a_positive_
  sentinel` (`:668`) asserts the LITERAL lines
  `const _: &str = v1_lifecycle::COMPILER_ROOT_SENTINEL;` and
  `pub(super) const COMPILER_ROOT_SENTINEL…` — **G1 removed both**
  (grep: zero hits in `src/`). Not cured by (a) or (b). My round-1
  G1 coordinate listed the sentinel as falling with the cfg — the
  enumeration was under-specified against the boundary checker's
  witness contract, which I did not enumerate; the cure decision is
  the boundary lane's. **Cheapest sound cure: reinstate the sentinel
  un-gated** — a production `const _` reference costs nothing,
  restores the positive-compile witness, and satisfies both my G1
  (the cfg fell) and the harness verbatim. Amending the harness test
  instead is a checker-contract change needing that lane's ruling.
  (d) **harness string drift** — `test_v0_persistence_seam_inventory_
  must_stay_derivable` (`:1177`) performs exact-string surgery on the
  store re-export block, which the package reshaped
  (`merge/mod.rs:81-85` now carries `AdaptationPrecheck`,
  `OpenRecordEnvelope`, `classify_open_record`,
  `discover_open_envelope_before_manifest`); the replace is a no-op,
  the "underivable" message never fires, the test fails. Cure: update
  the harness's surgery string (boundary lane's file).
  **Consequence: applying only the recorded fix (a) leaves (c) and
  (d) red at landing** — and the new CI step (§R2.3) runs this very
  battery on every push, so the landing push itself would go red.
  Graded **[P2-R2-1]**, landing-train class.

## R2.8 Executions (verbatim tails, all under the mandated scratch target)

```
gwz-core lib, four partitions (census 1583 = 1582 + 1 ignored):
  v1_lifecycle:: --skip root_fault_matrix   ok. 255 passed; 0 failed …  363.75s
  root_fault_matrix (release)               ok. 1 passed; 0 failed; … 1582 filtered out; finished in 333.46s
  checked_artifact::                        ok. 400 passed; 0 failed …  37.80s
  remainder (--skip both)                   ok. 926 passed; 0 failed; 1 ignored …  53.03s
g23::                                       ok. 119 passed; 0 failed …  39.78s
g23::a1_activation (all five named)         ok. 5 passed; 0 failed …  0.99s
T-6 forged-action pair                      ok. 2 passed; 0 failed …  0.30s
T-4 construction-site scan                  ok. 1 passed; 0 failed …  0.05s
clippy --all-targets --all-features -D warnings   Finished `dev` profile … in 19.68s   [clean]
check_m4_scenario_map.py        M4 scenario map: ok (39 scenario rows, 41 named tests, 13 registry rows all claimed)
check_merge_compatibility_...   validated 7 migration rules and 7 runtime bindings
check_merge_docs.py             merge document consistency: ok (11 sources, 147 assertions)
aggregate privacy               ok    sealed v1 lifecycle compile probes (105.4s, 'OK')
aggregate call-graph            FAIL (expected-red: 5 digests + 11/69 probe harness; release boundary suite ok)
boundary checker                6 lines: the 5 listed digests, zero persistence-seam findings (F-3 = 0 hits)
gwz-cli                         80+26+25+4+2+2 = 139 passed; 0 failed
gwz-py (native module REBUILT by me from the dirty core via maturin,
        .so 2026-08-25 00:39)   330 passed in 24.18s
LOC re-counted: gwz-core tracked +1109/−609, new files 743 (= +1852/−609, matching the report)
```

## R2.9 Round-2 findings

**[P2-R2-1]** The probe-harness expected-red ruling on record is
incomplete: four cure classes (§R2.7), of which the recorded
allow-emit fix cures one; the sentinel (reinstate un-gated
recommended) and the seam-surgery string need boundary-lane edits, and
the digest class needs the landing re-pins — else the landing push
goes red on the newly wired CI step. Landing-train duty; the F-3
property itself is intact and the allow-emit fix is verified sound.

**[P3-R2-1]** `AtomicUpgradeFault`'s injected variants are now
production-compiled (`store/atomic_upgrade.rs:20-27`), inert — the
single production caller passes `None` (`dispatch.rs:405-411`).
Hygiene: a cfg-split signature or sealed constructor at R2-E.

**[P3-R2-2]** The eligible-row upgrade-failure fallback (§R2.5) —
one executed test, carrier R2-E.

**[P3-R2-3]** L2-05 CI-wiring stays OPEN on the register with its
blocker now recorded in the workflow comment; the multi-repo checkout
(tuple §11.3 item 7) is the cure. Accepted for this round.

**[P3-R2-4]** Record note: the builder's "gwz-py 330 passed" tail
cannot be shown to have exercised the activated core — the imported
`_gwz_core.abi3.so` predated the package (2026-08-24 11:10). Cured in
this review: I rebuilt via maturin against the dirty core and
reproduced 330/0. The landing train's own py gate should run against
a fresh build as a matter of course.

**[P3-R2-5]** Prose nits for the landing pass: `classify_open_record`'s
doc comment says "without decoding its body" while the v0 arm fully
decodes (same decoder as the store — behaviourally moot, textually
wrong); the R4 ruling's conditions (a)-(c) (§R2.2) join the [P2-3]
record set.

## R2.10 Verdict line

**GO — round 2, Safety axis: the delivered A1 activation package is
accepted against the round-1 enumeration; R4's ordinary-start V0 floor
is an accepted named residual with binding record conditions; 0 P0 ·
0 P1 · 1 P2 (landing-train: the four-class probe-harness cure) ·
5 P3.**
