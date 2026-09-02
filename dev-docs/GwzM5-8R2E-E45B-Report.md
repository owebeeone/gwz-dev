# GwzM5-8 R2-E E4.5-B — builder's STOP-AND-REPORT (Opus, 2026-09-02)

Filed verbatim by the lane owner. Charter: `scratchpad/e45b-brief.md` (staged
from `GwzM5-8R2E-E45-6B-CharterPrep.md` and the capability-free amendment's
§7 E4.5/6-B disposition), base gwz-core main `f563446`. Outcome: stop-trigger
(5) fired — no commit; the demonstration diff is preserved out of tree.
The lane owner's mechanism verification and the operator decision packet are
recorded in `CurrentProgramCheckpoint.md` (position 2026-09-02).

---

**STOP AND REPORT — E4.5-B does not land.** Stop-trigger (5) fired, driven and ablated. `e4/e4-5b-marker` remains at `f563446` with **no commit**; both the worktree and the main checkout are byte-clean at that sha, no probe artifacts.

## The wall (the RR §1a / E4.3 signature, measured not argued)

Converting `execute.rs:45` onto the checked boundary makes `gwz merge --abort` fail after an interrupted marker publication, where the raw `write_atomic` leaves it working.

Transcript, on the `v1_lifecycle` finalization fixture (real git, real lease):

| drive | raw path (base) | converted arm |
| --- | --- | --- |
| crash at the marker step, then `--abort` | `Ok(Aborted)` | `Ok(RecoveryRequired)`, origin `RollingBack` |
| then `--abort` again | — | `Err(RecoveryEvidenceMismatch, "live rollback state is neither the exact before nor after state")` |
| then `--resume` (production `ForwardRuntime`) | — | `Err(MergePhaseUnsupported, "reverse-lifecycle recovery verification is owned by the preservation/rollback phase")` |

The crash is injected inside the checked publication (`run_next_checked_artifact_at(BeforeManagedPublication, panic)`) — the canonical power-loss instant, after the authority record and the staged goal are published and before the leaf is. On-disk residue at that instant, verbatim:

```
.gwz/checked-artifacts/ca1-<action>-<owner>-<id>.goal
.gwz/checked-artifacts/ca1-<action>-<owner>.authority
```

**Ablation proves causality**: delete that residue family before the abort and the same interruption gives `Ok(Aborted)`.

**Mechanism, pinned to the site.** The leaf itself is fine — `observe(marker)` = `Missing`, and `classify_file(.., missing_is_baseline = true)` (`abort/evidence.rs:313-318,:356`) still reads `Baseline`. What refuses is the *directional residue*: `artifact_facts::classify_remove(root, marker, candidate.marker_yaml)` returns **`Ambiguous`**, because `classification.rs:175-177` sees an authority record whose `matches_request` was minted for the FORWARD pair `(Missing → marker_yaml)` while the abort asks the REVERSE pair `(Bytes(marker_yaml) → Missing)`. `abort/evidence.rs:307` maps that to `FileState::Other`, the evidence shape stops being exact, and the rollback observation drives the record into recovery.

This is exactly the hazard **charter prep §2.4** describes ("the residue vocabulary is directional… the charter cannot assume the abort's existing `Recoverable` tolerance covers the new window") — but §2.5's summary table applied only the *detach-window* axis to `:45` and marked it CLEAR. §2.1's claim that `:45` is window-free is true and irrelevant: `MissingReplace` never detaches, and the wall is not a detach window. The marker is the first *publication* leaf carrying a checked writer on **both** the forward and the reverse arm, which is what makes the residue directional at all.

**Cure class**: abort-side observer reconciliation in `abort/evidence.rs::file_states` — prep §2.2 cure (ii), prep §7.1 option (i), sized 450–600 LOC, on the capability-free plain lease. The operator's ruling routes exactly that to DR-1 ("Route (C) …, reader-side record reconciliation, and O14 … to one phase-end design round… Do not start (C) inside E4"). Not mine to build.

**Disposition I recommend**: `:45` joins `:278`/`:279` in the `[R2-P3-1]` dated residual, on a *distinct and stronger* ground — not a detach window but a **directional-residue window**: an interrupted checked publication mints a forward-pair authority record that the shipped reverse classifier refuses, stranding `gwz merge --abort`. That is amendment-tier (the adopted amendment says `:45` converts), so it goes back to the lane owner.

## The parent bootstrap (brief item A) — do not build it, on two independent grounds

**(A1) The prescribed placement is structurally impossible.** `CheckedManagedActionV1::for_durable_merge` (`coordinator/identity.rs:259-266`) takes a `CheckedOwnerRecordObservation`, and `CheckedActionOwnerV1::permits_managed_request` (`identity.rs:112-130`) permits `DurableMerge` **only** under `MergeRecordV0|V1`, which `from_record_observation` (`:86-100`) mints only from non-empty durable record bytes (`:72-74`). `acquire_for_merge_start` / `start.rs:83-84` runs *before* `create_open` (`start.rs:85`), so no record exists there. A `DurableMerge` session at the brief's cite cannot be constructed.

**The doctrine question, answered.** `managed.rs:118-119`'s "one reproducible pre-record request" does **not** forbid a second sealed start-time request as such — it forbids a second *pre-record* one, and the mechanism above makes a `DurableMerge` request post-record by construction, bound to the record's own bytes (a stronger reproducibility binding than `for_merge_start`'s fixed purpose set). The only placement that would satisfy the brief's cite is a pre-record derivation from not-yet-durable bytes, and **that** the doctrine does forbid: a retry after a crash re-serialises a different record, deriving a different resident action — precisely the failure the sentence names. So: the doctrine forbids the chartered placement; it permits a post-`create_open` one, which is a different step.

**(A2) The premise is falsified by measurement.** The brief and the adopted amendment both say refuse-when-missing "would REGRESS a fresh workspace's first `--no-ff` publication". It would not. `gwz.conf/markers` is installed by **workspace creation**, not by any merge: `write_manifest_and_lock` (`artifact/mod.rs:502-515`) ends in `refresh_conf_integrity_marker`, which writes `gwz.conf/markers/conf-integrity.yml` (`artifact/conf_integrity.rs:51,:178-202`) through `write_atomic`'s `create_dir_all`. Driven on a real workspace built by `handle_init_from_sources`: the directory is present with `conf-integrity.yml` **before any merge**, and the first `--no-ff` publication then runs green through the converted boundary (that row passed). Prep §7.3 stop-trigger 1 does **not** fire; route (a) regresses nothing in an ordinary topology; the bootstrap is unnecessary machinery on a false premise, and would have added a *new* crash window (record durable, parent not yet) that today does not exist.

## What was verified green before the wall

- **Reachability re-driven at `f563446`**, identical to prep §1.2 at every join: `execute::publication` ← `finalization.rs:53` (sole); `FinalizationRuntime::new` ← `forward.rs:69` (sole production, all others under `tests/`, exactly the prep's list); `ForwardRuntime::new` ← `start.rs:89`, `start.rs:160`; production `service::run` = `start.rs:90/:161/:165`, `archive.rs:79`; lease fork `service.rs:119-124` (`acquire_activated` at `:121`, plain at `:123`); `reverse.rs:78-82` and `forward.rs:71-76` refusals intact; `service.rs:44` sealed with no production caller. **`acquire_activated` and nothing else.**
- **Expected fact = `Missing`, proved**: `WriteMarker` issues only from `(Baseline, Pre)` (`live.rs:64-66`), and `Baseline` requires `marker_absent` (`acceptance/publication.rs:90,:99-100`). `classify_exact:150` → `MissingReplace`, `transition.rs:91-93` → no detach. Stop-triggers (2) and prep §7.3 (3) do **not** fire.
- **Inventory clean**: `finalization/execute.rs` is absent from `CAPABILITY_FREE_RAW_WRITER_INVENTORY` (20 rows, verified) and from `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` (3 rows; it names no `durable_fs` token). Prep §7.3 stop-trigger 2 does not fire.
- **Pins never at risk**: no new `recover_or_create` namer (`PRODUCTION_CALLER_COUNT` stays 1), no new `entry.rs` door, no carved-file edits, record-root tripwire untouched. Stop-trigger (3) did not fire — the write is expressible by widening the *existing* workspace door.
- **Rows that passed** in the demonstration: the in-action checked fault refuses naming the boundary and writes nothing; the parent-missing refusal is typed (`MergeRecoveryRequired`, names "parent") and creates no `gwz.conf/markers`; the fresh-workspace first `--no-ff` publishes; re-issue after the interruption converges to `Completed` resuming exactly at `WriteMarker`. Only the abort half failed.

## Numbered flags

1. Brief item A's premise (raw `write_atomic` is what creates the marker parent) is **false against `f563446`** — workspace creation's conf-integrity refresh creates it. Measured.
2. Brief item A's placement cite (`acquire_for_merge_start` / `start.rs:83-85`) is **structurally unbuildable** — `DurableMerge` requires a record-derived owner.
3. Brief item B's route ("via `artifact_facts` / the existing workspace door") is **not expressible as written**: `write_checked` → `replace_merge_root_artifact` hard-codes `CheckedArtifactFact::Bytes`. The demonstration widened that existing door's `expected` to `Option<&[u8]>` (matching `replace_merge_preservation_workspace`'s established shape) with the parent gate on the creating arm only — no new door, no new `ENTRY_ITEMS` row.
4. Charter prep §2.5 marks `:45` CLEAR without applying its own §2.4 directional-residue hazard to it. §2.4 is the wall.
5. Cite drift (content unchanged, re-pointed): `bootstrap/managed.rs` all cites **+1** (the pins package's dated bracket at `:43`); `service.rs:116-121/:118/:120` → **`:119-124`/`:121`/`:123`**; `start.rs:83-85` → `:83-84`; `identity.rs` `for_durable_merge` `:262-266` → **`:259-266`**.
6. `observation.rs:155-158`: `parent_is_canonical` still carries `#[cfg_attr(not(test), allow(dead_code, reason = "v1 bundle lifecycle remains disabled until A1"))]` although it has two production callers. E4.7 allowance-sweep item.
7. `artifact_facts::classify_write` cannot express a `Missing` expected fact either — measured: on the interrupted marker it returns `Ambiguous`. Any observer cure must widen the classify side too.
8. `GwzM5-8R2E-CapabilityFreeAmendment.md` §7's E4.5/6-B disposition needs correcting on **both** counts: the fresh-workspace premise, and `:45`'s "CLEAR of the window" conclusion.
9. Worth auditing for the same directional-residue hazard: the preservation-bundle leaves, which also carry checked writers on more than one arm.
10. Fixture consequence for whoever re-charters: `v1_lifecycle/tests/finalization.rs::fixture` seeds `gwz.conf` but not `gwz.conf/markers`; a conversion must seed the parent there (six sibling fixtures do the same).
11. Lib test count at `f563446`, measured: **1839**. The demonstration added 4 rows (3 in `v1_lifecycle::`, 1 in `g23::`).
12. Nothing lands, so **no pin moved**: `run_r4bg_aggregate_gates.py` is untouched (checked_artifact 457/467, v1_lifecycle 266, remainder 1114/1115, g23 130 all stand), no digest was moved, no provenance block written.

## Cap and evidence

Demonstration size: **404 added whole handwritten lines / 11 removed** across 8 files (production ≈ 130 of them) — the 500 cap was never the binding constraint; the wall was. The full working diff is preserved, probe scaffolding stripped, at:

`…/scratchpad/e45b-demonstration-NOT-FOR-LANDING.patch`

Applying it to `f563446` and running `cargo test --lib -- an_interrupted_checked_marker_publication_converges_and_leaves_abort_working` reproduces the wall in ~1s; the other three E4.5-B rows pass.

## Gates at the final sha (`f563446`, byte-clean), direct exit codes

`cargo fmt --check` 0 · `cargo check --all-targets` 0 · `cargo clippy --all-targets -D warnings` 0 · boundary checker (python3.13) 0 ("ok (18 visible entries, 8 classified modules)") · its 69-test suite 0 (`Ran 69 tests … OK`). No push, no tag, no trailer, no commit.
