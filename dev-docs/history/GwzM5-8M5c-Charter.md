# M5c — the production v1 ordinary-start owner (DR-1 ship (2))

**Status:** **superseded for all ship-2 design** by
`GwzM5-8M5d-Charter.md` as of 2026-09-03. Historical evidence and
already-completed gates remain valid only where M5d says they do. Do
not implement this file. Sections §0–§6 below are **not current
authority**; event / response / `gwz stage` / raw-create requirements
are restated in M5d §3–§4 and those restatements control.

Previously: CHARTERED 2026-09-03 by the lane owner (Fable), awaiting
the operator's read. Home: R2-E phase E8. No tag.

## 0. Historical: operator decisions that bound the superseded M5c charter (2026-09-03, verbatim)

> M5c is not held for evidence. M5c is a record-version milestone (ordinary
> start writes v1 + the ordinary-start surfaces + the one-line floor raise). It
> is not "every merge gets a reboot-durable catalog." Decouple them. v1 records
> do not require catalog activation. Catalog activates only when identity is
> above the bar. I want M5c. I do not want (b) or graded evidence as its
> prerequisite.

> Next ship, unblocked by (1): M5c. Ordinary/custom start writes v1; floor raise
> is the one reviewed change with the surfaces. Same decision point as (1). A
> below-bar ordinary start is still a v1 record and still has no catalog.

> M5c (ship 2), decided now so you do not invent a third path. W3 measures which
> filesystems die at `create_merge_store_record`. When you charter M5c, the
> default is: if the legacy handle probe fails, write the record raw (today's
> v0 publication primitive). Ordinary merge must not newly refuse
> overlay/sshfs. Do not build that raw path in ship (1).

Minted definition (`GwzM5-8A1ActivationRecord.md` §2): "M5c owns: the v1
ordinary-start surfaces (root participants, dry-run prediction, drift/conflict
response, the event stream on v1), and the `version.rs:39` floor raise, which
lands WITH M5c as ONE reviewed change (contract §9's 'one reviewed change'
discipline transfers to the remainder)."

## 1. What the tree says today (read-only mapping, 2026-09-03, gwz-core `57502e4`)

Cites are file:line in gwz-core. Verified by the lane owner where marked ✓.

1. **The fork.** `merge/start.rs::handle_start_durable` serves every start;
   dry-run returns at `:68-70` BEFORE planning and record creation; the plan
   (`plan.rs:140-221`, root preflight `:186-209`) and `create_record`
   (`start/record.rs:76-108`) run for both versions; `:95-117` routes a
   `select_record_version(...) == V1` record to `V1Router::start` →
   `v1_lifecycle::handle_start_durable_v1` (the W3 decision point at its
   head). Everything after the fork is the v0 engine: `persist_merge_record`,
   `execute_durable`, `post_start_state`, `enter_finalizing`,
   `finalize_dispatch`, `complete_and_archive`.
2. **Dry-run prediction is already outside v1.** Intercepted at `start.rs:68`
   before the version is even selected; `v1_lifecycle/` has no `dry_run`
   mention. The floor raise changes nothing here; M5c owes only a PIN.
3. **Root participants are present in v1** through the shared plan and v1's own
   finalization/rollback (`authority/observe/finalization.rs:113-140`,
   `acceptance/v1_candidate.rs:48-120`, `reverse/execute/rollback.rs:70`,
   `v1_lifecycle/tests/finalization_root.rs`). One provenance asymmetry: v0
   reconstructs the lock, v1 replays `accepted.lock.exact_yaml`. One adjacent
   GAP: `handle_stage.rs:34` ✓ discovers an open merge through
   `FileMergeStore::discover_open`, whose decoder installs v0 only — root
   conflict staging under an open v1 record has no path today.
4. **Response gaps on the v1 path.** A COMPLETED v1 start answers through
   `archived_status_response` with `repos: Vec::new()` and default
   `participant_counts`, `preservation: None`, `publication_step: None`
   (`merge/response.rs:64-68` ✓) — the v0 completed start returns full rows.
   `MergeRepoSummary.predicted` / `prediction_complete` are set only by the v0
   start decorations (`start/response.rs:40, 69`); v1 never sets them.
5. **Event gaps on the v1 path.** v0 emits `member_started` and
   `merge_member_finished` per participant (`start/execution.rs:22, 59, 90,
   109`; continue and abort likewise), `operation_state_changed` at every
   durable transition (`store/persistence.rs:17`), `artifact_written` for every
   record persist, the archive, and the publication artifacts
   (`finalize.rs:308-316`). v1 emits ONE `operation_state_changed` at creation
   (`v1_lifecycle/start.rs:114`) and W3's `Diagnostic`. The CLI's human
   progress line and `tests/g00.rs:60-61` depend on the member events.
6. **The record boundary below the legacy bar.** `create_merge_store_record`
   (`entry.rs:541-561`) → `CheckedArtifact::acquire` (`observation.rs:82`)
   probes `identity::object_identity` on the root and the parent; a failed
   legacy probe renders as `ErrorCode::UnsupportedOperation`, "checked merge
   record '…': durable filesystem identity is unsupported: <remedy>"
   (`observation.rs:372-377` ✓ — the caller's `MergeRecoveryRequired` is
   bypassed on this arm). **On the forward path this CREATE is the only
   checked-boundary write**: the record rewrite (`store/rewrite.rs::commit`,
   record-root exception), the terminal archive (`store/archive.rs`), and the
   marker/lock/boundary publications (`finalization/execute.rs:64-100`) are
   already raw by dated exception. The reverse path (`root/artifact_facts.rs`,
   `preserve/checked_bundle.rs`, `preservation_root/files.rs`) still takes the
   probe, scoped by `CapabilityFreeAmendment.md` §6.
7. **The v0 primitive.** `store/mod.rs:463-498` `write_atomic_verified(path,
   bytes)`: `O_EXCL` temp, `write_all` + `sync_all`, `rename_durable(replace)`,
   `sync_dir`, re-read and byte-compare; private to `merge::store`; no identity
   probe. Fault pin `store/tests.rs:410-420`.
8. **The call-graph gate F-3** (`check_checked_artifact_boundaries.py:346-387,
   1487-1506`): `v1_lifecycle/` (tests included) may not name the v0
   persistence seam — floor `FileMergeStore`, `MergeStore`,
   `archive_merge_record`, `enter_finalizing`, `persist_merge_record`,
   `persist_operation_transition` plus the derived re-exports — and is
   LOAD-BEARING for judgment call J-1; the raw writer set
   `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` may only shrink.
9. **Ordinary = v0 is pinned in many places**: `version.rs:135-158`;
   `g23/a1_activation.rs:221, 487, 519, 531, 611, 674, 679`; the whole
   `g23/characterization_*_v0.rs`, `archive_equivalence_v0.rs`,
   `compatibility_*_v0.rs` corpus (15 fault windows assert the v0 envelope);
   `g23/start.rs` (11), `root_*.rs` (24), `continue_*`, `abort_recovery`,
   `preserve`, `finalization`, `drift`, `gc`, `m4_matrix`; the retained-reader
   manifest `scripts/retained_readers/manifest.json:26-29` ("ordinary and
   custom-message starts still create v0 … owned by milestone M5c") validated
   by `retained-readers.yml`; `merge_docs_manifest.json:331-333` pinning
   `gwz-cli/docs/commands/merge.md:381-384` ("Ordinary and custom-message
   starts continue to write v0 records"); doc cross-refs at
   `capability.rs:63`, `v1_lifecycle/checked.rs:155`.
10. **Contract obligations** (`GwzM5-8I2CompatibilityContract.md` §2/§9;
    `GwzM5-8A1ActivationRecord.md` §2): the floor raise lands WITH the surfaces
    as ONE reviewed change; complete only after TWO independent reviews find
    no P0–P3 and the record/protocol contracts, R0 fixtures, retained-reader
    manifest and merge plan agree; the frozen creation matrix already says
    `ordinary/custom = v1` (the 2026-08-25 annotation closes, the table is not
    edited); open v0 records migrate only through `record_wire::open_v0`'s
    seven whitelisted shapes (unchanged); completed/aborted v0 records stay
    v0; a raised floor governs CREATION only.

## 2. Design decisions (lane owner's, inside the operator's rulings)

### 2.1 The raw record write below the legacy bar
- **Gated, never silent.** `entry::create_merge_store_record` gains a mode:
  `RecordPublication::Checked` (today) or
  `RecordPublication::RawBelowLegacyBar`. Only the UNCATALOGUED creation lease
  (W3's below-bar path) passes the raw mode. Above the bar an `Unsupported`
  from the boundary stays an error — it signals an anomaly, not a capability.
- **Trigger.** The raw path is taken only when `CheckedArtifact::acquire` fails
  on its identity probe (`observation.rs:96/105`, the `unsupported` arm). Any
  other failure (parent missing/noncanonical, I/O) keeps today's behaviour and
  codes.
- **Primitive.** `write_atomic_verified` moves, unchanged in semantics, to a
  neutral module both callers can name (`crate::workspace_ops::merge::store`
  keeps a re-export for the v0 writer; `checked_artifact::entry` calls the
  neutral home) — this keeps F-3 untouched: nothing under `v1_lifecycle/`
  names the v0 seam, and `entry.rs` is outside the scan. The fault pin at
  `store/tests.rs:410` moves with it.
- **No second warning.** The process already emitted W3's one `Diagnostic`;
  the raw write is that decision's consequence. The response's
  `crash_recovery` already says `supported: false`.
- **Test seam.** A `cfg(test)` hook in `checked_artifact/identity.rs` that
  makes the legacy probe answer `persistent_identity_unsupported()` for the
  next acquire, so the raw path is driven on ext4/APFS CI hosts; plus R0-L's
  real overlay row as the natural measurement.
- **What the raw record loses.** Exactly what the operator ruled: crash
  recovery. The record's later rewrites and archive are raw already; the
  merge's own verification is unchanged.

### 2.2 Event-stream parity is SHAPE parity
The v1 forward loop emits, per participant, `member_started` before its
observation/apply and `merge_member_finished` (with the `MergeRepoSummary`
projection) after its durable row; `operation_state_changed` at every durable
state transition the store commits; `artifact_written` for the record create,
each record commit, the archive, and the publication artifacts. Same kinds,
same ordering discipline, same per-participant count as v0 for the same
fixture; message text may differ. Continue and abort arms get the same
treatment (v0 emits them there too).

### 2.3 Response parity
A completed v1 start (and continue) returns the per-repo rows,
`participant_counts`, `publication_step` and `preservation` projected from the
ARCHIVED record (contract §7: the archived projection reads only the exact
done-record bytes — the projection is widened, not synthesised). The start
decorations (`predicted`, `prediction_complete`, `live_commit` for a conflicted
row) are applied to the v1 start response exactly as `decorate_start_response`
applies them to v0. This also fixes today's `--no-ff` completed response.

### 2.4 `gwz stage` under an open v1 record
`handle_stage.rs` discovers the open record version-agnostically
(`discover_open_envelope_before_manifest`, the same route `dispatch.rs` uses)
and derives the stage member paths from either record body; root conflict
staging works for an ordinary v1 merge.

### 2.5 The floor raise and the v0 corpus
`ACTIVE_WRITER_FLOOR = RecordVersion::V1` is the one-line change, landed WITH
2.1–2.4 as one reviewed change. The v0 characterization corpus keeps its
meaning under a `cfg(test)` writer-floor override (`with_writer_floor(V0, ..)`)
because it characterizes the v0 LIFECYCLE that legacy records on disk still
route to — those suites are not deleted and not rewritten to v1. Suites that
assert the SHIPPED default (`a1_activation.rs` rows, `g23/start.rs` envelope
assertions, `version.rs` rows) flip to v1. The retained-reader manifest entry
is re-described (shipped: ordinary = v1), `merge_docs_manifest.json:331` and
`merge.md:381` say v1, and the doc cross-refs move. Dry-run gets a pin: an
ordinary dry-run under floor V1 still returns predictions and writes nothing.

### 2.6 The reverse path on handle-less filesystems — decision item
After the raise, an ordinary merge's ABORT that must re-verify checked
artifacts — a selected root's manifest and lock, or a preservation bundle —
takes the legacy probe (`root/artifact_facts.rs`, `preserve/checked_bundle.rs`)
and would refuse on overlayfs/sshfs where the v0 abort did not. The plain abort
(no checked artifact) is unaffected. Options: (i) stated limit for M5c with a
measured row, cured next; (ii) widen the raw fallback to those two door
families in M5c (~250 LOC more, touches the reconciliation-adjacent doors the
operator parked). **Recommendation: (i)** — the operator's "must not newly
refuse" is read as the merge's forward path; the abort case is narrow (root
participated and published, or `--preserve`), and (ii) reaches into parked
territory. The operator decides (§6).

## 3. What M5c does NOT do
No catalog activation requirement for v1 records; no (b); no graded evidence;
no per-attempt recording; no protocol version bump (any new response fields
are additive, as in ship (1)); no change to the open-v0 migration whitelist;
no change to archived/completed v0 records; no marker/lock/boundary
conversion; no reconciliation S2+.

## 4. Phases and steps (each < 500 LOC aspirational; single-axis review by reading; the floor step takes the dual)

**Phase M5c-A — v1 forward parity** (milestone: a `--no-ff` merge today is
indistinguishable from a v0 merge to every driver and test that watches events
and responses; no floor change; every step verifiable through `--no-ff` now).

| Step | Scope | Files | LOC | Deps |
|---|---|---|---|---|
| A1 | Event parity §2.2 on the forward, continue and abort arms; g23 rows comparing event SHAPES v0 vs v1 on one fixture (counts of `MemberStarted`/`MemberFinished` per participant, `OperationStateChanged` per transition, `ArtifactWritten` set) | `v1_lifecycle/forward.rs`, `service.rs`, `store/rewrite.rs` (emit after commit), `archive.rs`, `finalization/execute.rs`, `reverse/*`; `tests/g23/` | ~350 | — |
| A2 | Response parity §2.3: archived projection widened to rows/counts/publication/preservation; start decorations on v1; rows for completed `--no-ff` today | `merge/response.rs`, `v1_lifecycle/status.rs`, `start.rs`, `start/response.rs`; tests | ~300 | — (∥ A1) |
| A3 | `gwz stage` under an open v1 record §2.4; root stage member paths from a v1 body; tests | `workspace_ops/handle_stage.rs`, `merge/root.rs`, `record_wire` reader; `tests/g23/root_stage.rs` | ~250 | — (∥ A1, A2) |

**Phase M5c-B — the record below the legacy bar** (milestone: an ordinary
merge on overlayfs/sshfs starts, warns once, writes its record raw, completes).

| Step | Scope | Files | LOC | Deps |
|---|---|---|---|---|
| B1 | §2.1: primitive relocated; `create_merge_store_record` mode; uncatalogued lease passes raw mode; identity seam; rows: below-bar + handle-less → raw record + completion; above-bar boundary `Unsupported` still errors; F-3 and digests re-pinned; measured on R0-L's overlay row (a Rust row if the matrix can mount overlay, else the Python evidence cited) | `checked_artifact/entry.rs`, `identity.rs` (seam), `merge/store/mod.rs` + new neutral module, `v1_lifecycle/checked.rs`, `store/rewrite.rs`; `scripts/checks/*` | ~300 | ship (1) only (∥ Phase A) |
| B2 | §2.6 measured rows for the reverse doors on a handle-less volume (under the identity seam): plain abort succeeds; root/preserve re-verify refuses with the boundary's sentence — then the operator's (i)/(ii) | tests only under (i) | ~150 | B1 |

**Phase M5c-C — the floor raise, ONE reviewed change** (milestone: ordinary
and custom-message starts write v1; every pin, manifest and doc agrees; dual
review).

| Step | Scope | Files | LOC | Deps |
|---|---|---|---|---|
| C1 | `ACTIVE_WRITER_FLOOR = V1`; `with_writer_floor` test seam; the v0 corpus wrapped; shipped-default pins flipped; retained-reader manifest re-described; `merge_docs_manifest.json` + `merge.md` + doc cross-refs; dry-run pin; the six R4b-G batteries (fault, compatibility, byte-equivalence PARTIAL as recorded, unknown-field, privacy, call-graph) run once at the landing; **dual review** (Code + Safety) per contract §9 through the review-loop; a1_activation rows re-reasoned one by one | `model/version.rs`, `tests/g23/*`, `scripts/retained_readers/manifest.json`, `scripts/checks/merge_docs_manifest.json`, `gwz-cli/docs/commands/merge.md`, `capability.rs`, `checked.rs` | ~450 (mostly pins) | A1–A3, B1 (B2 decided) |
| C2 | Release readiness for 0.14.0: release notes finalised from the tree; SKILL.md; matrices dispatched; the operator asked for the tag | dev-docs, `~/.claude/skills/gwz/SKILL.md` | ~100 | C1 |

Order: A1 ∥ A2 ∥ A3 ∥ B1 → B2 → C1 → C2. Total ≈ 1,800 LOC over six steps.
Every landing: `cargo fmt` per crate, lane gate, digests, pins + `check_pins.py`,
matrix dispatch where production code crosses platforms, docs gate; reviewers
verify by reading and reproduce one measured claim; partitions are measured
once at landing. CI pollers keep one waiter per run.

## 5. Acceptance of M5c
On any filesystem: `gwz merge <source>` (ordinary, `--ff-only`, `-m`) writes a
v1 record; above the bar the catalog activates silently; below it one warning
and no catalog; on a handle-less volume the record is written raw and the merge
completes; `gwz merge --dry-run` predicts and writes nothing; events and
responses match v0's shape for the same fixture; `gwz stage` works under an
open ordinary merge; completed responses carry repo rows; the v0 corpus still
passes under its floor override; the retained-reader manifest, docs manifest
and merge docs agree that ordinary = v1; the dual on C1 finds no P0–P3. No tag
until the operator asks.

## 6. Open decisions for the operator (one line each; defaults stated)
1. §2.6 reverse doors on handle-less filesystems: (i) stated limit + measured
   row (default) or (ii) widen the raw fallback in M5c.
2. Event parity as SHAPE parity (default) rather than byte-identical messages.
3. The v0 corpus kept under a test-only floor override (default) rather than
   converted or deleted.
4. Phase A may start now, in parallel with B1, before C1's dual (default yes:
   every A step is verifiable through `--no-ff` today and changes no default).
5. Whether 0.14.0 is cut at C2 or ship (1) is released first — the operator's
   call, asked before any tag.
