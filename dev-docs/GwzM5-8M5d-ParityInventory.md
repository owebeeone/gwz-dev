# M5d — functional parity inventory (v0 merge engine → v1 lifecycle)

Date: 2026-09-03

Status: **INVENTORY — evidence only. Adopts nothing, decides nothing, builds
nothing.** Controlling design is `GwzM5-8M5d-Charter.md` (revision 5, accepted
2026-09-03). This document is the safety net for the close: it enumerates every
observable v0 merge behaviour and states, with `path:line` evidence, whether the
v1 lifecycle reproduces it today. Where the charter deliberately drops or
changes a behaviour, the row says so and cites the charter section rather than
filing a gap. Sizing context is `GwzM5-8M5d-Review.md`; that memo and the
charter are context, **the tree is the authority** and every claim below was
read out of the tree at the tuple.

## Why this document exists

The operator's instruction: "make sure all the old functionality translates
(especially things like `--dry-run` where we missed that in v0.8.0)."

The named precedent is real. A dry-run class defect shipped for a long time and
was closed only on 2026-09-02 at gwz-core `22f388d`, reviewed in
`dev-docs/history/GwzDryRunClassFix-Review.md`. Before that fix,
`gwz --dry-run stash drop <id>` destroyed the stash and left the workspace
unable to perform the real drop (`GwzDryRunClassFix-Review.md:29-40`). The
failure class is: **a behaviour that exists on the old path is silently absent
on the new path, and no test asserts it on the new path.** M5d raises
`ACTIVE_WRITER_FLOOR` to `V1` and deletes the v0 lifecycle engine, so that class
applies to an entire engine at once.

## Evidence base

- Umbrella gwz-dev `5d43a0d`; gwz-core `57502e4`; gwz-cli `dccd619`;
  gwz-py `da9fb7a`. All `path:line` cites are at that tuple.
- Method: reading and cheap greps only. No cargo, no builds, no test suites
  (review-cost discipline). Test files are read as **evidence of intended
  behaviour**, which is often the fastest statement of what a path guarantees.
- Repository prefixes are omitted where the path begins `gwz-core/`,
  `gwz-cli/`, `gwz-py/` or `dev-docs/`.

## 0. How to read this

The fork is one `if` in one function.

`merge/start.rs:46 handle_start_durable` serves **every** start. It returns for
dry-run at `start.rs:68-70`; it selects the record version at `start.rs:95` and
routes a `V1` record to `V1Router::start` at `start.rs:110-116`; everything from
`start.rs:118` down is the v0 engine. Today
`ACTIVE_WRITER_FLOOR = RecordVersion::V0` (`model/version.rs:39`) and
`RequestedSemantics::Ordinary → V0` (`version.rs:70`), so only `--no-ff`
(`version.rs:71`) crosses to v1. **"v1 status" below therefore means: what the
v1 lifecycle does today on a `--no-ff` merge.** After the floor raise it is what
every merge does.

Open-record routing is separate: `runtime/dispatch.rs:349-356` classifies the
open record by envelope and sends a `V1` record to `v1.command`. Every
**non-merge** consumer of merge state does *not* do this — see §6, which is the
largest surprise in this inventory.

`ACTIVE_WRITER_FLOOR`'s own doc comment already names four of these gaps:
"root participants, dry-run prediction, the drift and conflict response
surfaces, and the event stream the v0 engine emits" (`version.rs:28-31`).

**Status vocabulary.** PRESENT — v1 already does it, cited. ABSENT — v1 does
not, cited by the absence traced. PARTIAL — differs in the stated way.
UNVERIFIED — not traced cheaply; the reason is stated.

**Verdict vocabulary.** *no work* / *must migrate* / *dropped by charter §N* /
*operator ruling*.

---

## 1. `--dry-run`

The deepest section, per the operator's instruction. The headline is stated
first, then the evidence.

### 1.1 Where dry-run is intercepted, and what that means under floor V1

`handle_start_durable` returns at `start.rs:68-70`:

```rust
if request.meta.dry_run.unwrap_or(false) {
    return handle_dry_run(backend, root, request, context);
}
```

That is **line 68 of 150**, and the version fork is at **line 95**
(`select_record_version(...) == RecordVersion::V1`). The return therefore
precedes, in order: `classify_open_record` (`:75`), `store.discover_open`
(`:78`), `plan_merge` for the real path (`:81`), id minting (`:82`),
`freeze_merge_messages` (`:83`), `create_record` (`:90`), the version selection
(`:95`) and the handoff to `v1.start` (`:110`).

**Raising `ACTIVE_WRITER_FLOOR` to `V1` changes nothing about this
interception.** The floor is read only inside `select_record_version`
(`version.rs:90-92`), which is called only from `start.rs:95` and
`start/record.rs:55-57` — both below the dry-run return. An ordinary
`--dry-run` merge under floor V1 takes exactly the interception it takes today.

Corroborating absence: `grep dry_run` over `merge/v1_lifecycle/` returns **zero
hits**. `handle_start_durable_v1` (`v1_lifecycle/start.rs:75-82`) has no
`dry_run` parameter, and neither does the `V1Router::start` trait method
(`runtime/dispatch.rs:40-47`). The v1 lifecycle has no concept of a dry run and
does not need one, because it is unreachable from one.

### 1.2 Whether the guarantee is whole — the seam question

**It is whole, and it is whole for a weaker reason than the 2026-09-02 fix's
seam.** The fix installed `WorkspaceMutationAccess { Mutating | PlanOnly }`
(`merge/runtime/mutation_guard.rs:36-71`) so that "NO CALLER CAN COMPILE without
stating an answer" (`mutation_guard.rs:27`). The merge lifecycle was **not**
migrated onto it. The review says so in as many words:

> The merge lifecycle's own guards are untouched
> (`GwzDryRunClassFix-Review.md:118-119`)

Merge uses the thinner `guarded_workspace_root` (`mutation_guard.rs:110-128`),
whose dry-run arm early-returns `(None, root)` **before** taking the mutator
lock (`mutation_guard.rs:116-122`), which the same review records at
`GwzDryRunClassFix-Review.md:115`. So the v1 lifecycle sits **outside** the
mutation seam, and merge's dry-run safety rests entirely on the single early
return at `start.rs:68-70` plus the fact that everything reachable from
`handle_dry_run` is an observer (§1.4).

That is a structural guarantee, not a type-level one, and the close **moves the
file it lives in** (§1.5). That is the one thing to watch.

### 1.3 What a dry-run response contains

`handle_dry_run` (`start/response.rs:12-25`) plans, maps every participant to a
`Planned` row, and calls `merge_response`.

Per-row (`start/response.rs:52-82`, `summary`):

| Field | Value on a dry run | Evidence |
|---|---|---|
| `state` | `Planned` (passed in) | `start/response.rs:22` |
| `predicted` | `plan.analysis` | `start/response.rs:69` |
| `prediction_complete` | `Some(plan.prediction_complete)` | `start/response.rs:70` |
| `conflict_paths` | `plan.predicted_conflict_paths` (because state is `Planned`) | `start/response.rs:71-75` |
| `live_commit` | `Some(before_commit)` (`Planned` is neither `Failed` nor `Unattempted`, and `Row::new` leaves `oid: None`) | `start/response.rs:62-66`; `start/prepared.rs:29-37` |
| `resulting_commit` | `None` | `start/response.rs:67` via `Row::new` |
| `continue_eligible` / `abort_eligible` / `pending_action` | `None` | `start/response.rs:76-77, 80` |
| `drift` | empty | `start/response.rs:78` |

Envelope (`start/response.rs:83-140`, `merge_response`): counts are derived from
the row states (`:88-103`); then, **because `context.dry_run` is true**, the
first arm wins unconditionally — `state = Completed`, `aggregate = Accepted`
(`:104-105`). `merge_id: None`, `open: false`, `operation_drift: []`,
`preservation: None`, `publication_step: None`, `record: None`,
`crash_recovery: None` (`:129-138`).

Two consequences worth naming, both verified:

1. **`merge_response`'s other arms are unreachable in production.** Its only
   non-test caller is `handle_dry_run` (`start/response.rs:24`; the other
   references are `#[cfg(test)]` at `start.rs:41-42` and
   `start/tests/execution.rs:336, 345, 372`). Since `context.dry_run` is always
   true there, the `Halted` / `AwaitingResolution` / `Noop` / `Ok` arms
   (`start/response.rs:106-113`) are dead on the shipped path. A dry run of a
   merge predicted to conflict still answers `state: Completed`,
   `aggregate: Accepted` — pinned at `tests/g23/start.rs:196-199` alongside a
   `TrueMerge` prediction at `:281-286`.
2. **`record: None` on dry-run is a documented protocol pin**:
   `gwz-cli/docs/MachineOutput.md:238-239` — "It is null for dry-run, idle
   status, pre-record responses, and unqualified GC."

### 1.4 What a dry-run must not do, and whether any reachable path can mutate

**No lock or lease.** `guarded_workspace_root`'s dry-run arm returns before
`WorkspaceMutatorLock::acquire` (`mutation_guard.rs:116-122`). Pinned
observably: `tests/g23/start.rs:222-223` deletes `.gwz/locks`, and `:254`
asserts it is still absent after a dry-run start.

**No write is reachable.** The complete call graph below `handle_dry_run`:

- `plan_merge` (`plan.rs:21-69`) — `artifact::read_manifest`, `read_lock`,
  `backend.head`, `file_text_and_sha256` (all reads).
- `build_merge_plan` (`plan.rs:141-222`) is parameterised over
  **`PlanningBackend`** (`plan.rs:71-95`), whose eight methods are
  `is_repository`, `status`, `head`, `merge_state`, `merge_analysis`,
  `merge_simulate`, `read_ref`, `read_file_at_commit` — **there is no write verb
  in the trait**. This is the strongest available statement of the guarantee.
- `preflight_member` (`plan.rs:284-...`) and `root::preflight_root`
  (`root/planning.rs:13-...`) are observation-and-refuse only.
- `complete_predictions` (`plan.rs:244-282`), reached **only** under
  `request.meta.dry_run == Some(true)` (`plan.rs:213-215`), calls
  `merge_simulate`, which is an in-memory index merge:
  `git/gitbackend/merge_prepared.rs:364-385` — `open_repo`,
  `ensure_no_integration_in_progress`, `find_commit`, `in_memory_merge_index`,
  `conflict_paths`. Nothing is written; nothing is checked out.
- `enforce_mode` (`plan.rs:224-242`), `summary`, `merge_response` — pure.

**Emitted events on a dry run**: only the shared bracket
`operation_started` / `operation_finished` (`runtime/dispatch.rs:275, 311`).

**Behavioural pins in the tree** (class-(ii) suites, all currently exercising
the v0 path because ordinary starts are v0):
`tests/g23/start.rs:186-213` (HEAD, lock, `merge_state` unchanged),
`:258-298` (predicted `TrueMerge`, `prediction_complete`, conflict path, HEAD,
`.git/index`, worktree file and lock all unchanged),
`tests/g23/root_start.rs:93` and `:123` (explicit root, visible and
non-mutating; conflict predicted without mutation),
`tests/g23/m4_matrix.rs:51` (mixed member+root),
`gwz-cli/tests/local_workflows.rs:440-489` (both flag positions; `Planned`
under `--dry-run` vs `UpToDate` without; HEAD unchanged; stderr empty),
`gwz-py/src/tests/test_merge_cli_cross_driver.py:366-394` (`dry_run_start`
cross-driver parity).

### 1.5 The one real dry-run hazard in M5d

`handle_dry_run`, `merge_response`, `summary` and `decorate_start_response` all
live in **`merge/start/response.rs`**, and `Row` lives in
**`merge/start/prepared.rs`** — both inside `merge/start/`, the module the
sizing memo classes as the v0 engine's start half
(`GwzM5-8M5d-Review.md:16-17`, "Delete … 4,484 whole files + ~3,300 halves").
`start.rs` itself is the shared entry that must survive. **The dry-run
implementation is therefore not "unchanged by the floor raise"; it is code that
the deletion pass will be standing on top of, and it must be deliberately
re-homed rather than deleted with its neighbours.** Charter §4's dry-run bullet
asks only for a pin ("pin that an ordinary dry-run under floor V1 predicts and
writes nothing"); it does not name the relocation.

### 1.6 Dry-run rows

| # | Behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| D-1 | `dry_run` is accepted only for `merge start`; every other op refuses `MergeValidationFailed "dry_run is accepted only for merge start"` | `merge/validate.rs:85-87` | PRESENT | shared validator, called above the fork at `runtime/dispatch.rs:300` | no work |
| D-2 | A dry-run start takes **no** workspace mutator lock and does not create `.gwz/locks` | `merge/runtime/mutation_guard.rs:116-122`; pin `tests/g23/start.rs:222-223, 254` | PRESENT | same code, above the fork (`runtime/dispatch.rs:289-296`) | no work |
| D-3 | A dry-run start still enforces the open-merge gate before planning | `mutation_guard.rs:117` → `runtime/open_gate.rs:12-30` → `operation/open_merge_gate.rs:83-99` | PARTIAL | the gate discovers through `FileMergeStore::discover_open` (`open_gate.rs:22, 24`), whose decoder is `decode_production_v0` (`store/mod.rs:419`); a **v1** open record yields `UnsupportedRecordVersion`, not `OpenOperation` — see §6 O-2 | must migrate |
| D-4 | Dry-run is intercepted **before** version selection, record creation and the v1 fork | `merge/start.rs:68-70` vs `:90`, `:95`, `:110` | PRESENT (structural) | `grep dry_run merge/v1_lifecycle/` → zero hits; `handle_start_durable_v1` takes no `dry_run` (`v1_lifecycle/start.rs:75-82`); `V1Router::start` has no such parameter (`runtime/dispatch.rs:40-47`) | no work + pin (charter §4) |
| D-5 | A dry-run plan runs the extra prediction pass that a real start does not | `merge/plan.rs:213-215` → `complete_predictions` `plan.rs:244-282` | PRESENT | shared plan, above the fork | no work |
| D-6 | Prediction uses an **in-memory** merge index; no checkout, no index write | `git/gitbackend/merge_prepared.rs:364-385` | PRESENT | same backend call | no work |
| D-7 | Every backend call reachable from a dry run is an observer — the planning trait has no write verb | `merge/plan.rs:71-95` (`PlanningBackend`, 8 read methods) | PRESENT | v1 never entered | no work |
| D-8 | Dry-run rows carry `predicted`, `prediction_complete`, predicted `conflict_paths`, `live_commit = before_commit`, `resulting_commit = None` | `merge/start/response.rs:52-82`; `start/prepared.rs:29-37` | PRESENT | v1 never entered | no work |
| D-9 | A dry run answers `state: Completed`, `aggregate: Accepted`, `open: false` **regardless** of predicted conflicts | `merge/start/response.rs:104-105, 130-131`; pin `tests/g23/start.rs:196-199` with `:281-286` | PRESENT | v1 never entered | no work |
| D-10 | A dry run answers `merge_id: None`, `record: None`, `preservation: None`, `publication_step: None`, `operation_drift: []`, `crash_recovery: None` | `merge/start/response.rs:129-138`; doc pin `gwz-cli/docs/MachineOutput.md:238-239` | PRESENT | v1 never entered | no work |
| D-11 | `merge_response` / `summary` are dry-run-only production code; their `Halted`/`AwaitingResolution`/`Noop`/`Ok` arms are unreachable on the shipped path | only non-test caller is `merge/start/response.rs:24`; other refs `#[cfg(test)]` at `merge/start.rs:41-42` | PRESENT | — | no work (documentation value only) |
| D-12 | The dry-run implementation lives inside the module the close deletes | `handle_dry_run`/`merge_response`/`summary`/`decorate_start_response` in `merge/start/response.rs:12, 83, 52, 35`; `Row` in `merge/start/prepared.rs:21-38` | n/a | — | **must migrate** (not named by charter §4) |
| D-13 | Dry-run merge sits **outside** the `WorkspaceMutationAccess` seam the 2026-09-02 fix installed | `mutation_guard.rs:110-128` vs `:73-106`; `GwzDryRunClassFix-Review.md:115, 118-119` | PARTIAL (by design of that fix) | v1 lifecycle likewise outside; nothing in `v1_lifecycle/` states a dry-run answer | operator ruling — migrate merge onto the seam, or accept the structural guarantee and pin it |
| D-14 | `--dry-run --filesystem-strict` on a below-bar volume takes no crash-recovery decision, so the flag has no effect | `merge/validate.rs:64-71` accepts it on Start; `start.rs:68-70` returns before `start.rs:113` threads it to `v1.start` | PRESENT (unchanged) | the strict refusal lives at `v1_lifecycle/start.rs:92-95`, unreachable from a dry run | operator ruling — charter §8 says both "strict refuses before writes" and "dry-run predicts, writes nothing"; today the dry run wins |
| D-15 | Human output has **no** dry-run banner; the only signal is `state` plus `Planned` rows | `gwz-cli/src/merge_render.rs:3-13, 88-102` | PRESENT | renderer is version-blind | no work |
| D-16 | The JSON `crash_recovery` key is absent on a dry run | `gwz-cli/src/merge_render.rs:246-248`; doc `gwz-cli/docs/MachineOutput.md:335-338` | PRESENT (behaviour) | — | doc must change: `MachineOutput.md:336-337` gives the reason as "every start that writes a v0 record (ordinary and `--ff-only` merges)", which becomes false |

**Dry-run finding, stated plainly.** The interception at `start.rs:68-70`
precedes the version fork at `start.rs:95`, so an ordinary `--dry-run` merge
under floor `V1` takes the same path it takes today and the "predicts, writes
nothing" guarantee survives the floor raise intact — the whole reachable call
graph is parameterised over a read-only planning trait (`plan.rs:71-95`), no
mutator lock is taken (`mutation_guard.rs:116-122`), and `v1_lifecycle/` has no
`dry_run` mention at all. The guarantee is **structural, not type-level**: merge
was never migrated onto the `WorkspaceMutationAccess` seam the 2026-09-02 fix
installed (`GwzDryRunClassFix-Review.md:118-119`), so it rests on that one early
return. The two things to watch are that the dry-run code lives inside the
module being deleted (D-12) and that the dry-run open-merge gate reads through
the v0-only decoder and will start answering `UnsupportedRecordVersion` instead
of `OpenOperation` (D-3 / §6).

---

## 2. Merge modes and flags

| # | Behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| F-1 | Ordinary / `--ff-only` / `-m` starts write **v0**; `--no-ff` writes **v1** | `model/version.rs:39, 70-71, 90-92`; `merge/start.rs:95-97` | n/a — this is the change | — | dropped by charter §1 (floor `V1`) |
| F-2 | `--ff-only` refuses at plan time when any participant needs a merge commit: `MergeValidationFailed "merge requires a merge commit but --ff-only was requested"`, member-scoped | `merge/plan.rs:224-242` | PRESENT | shared plan, above the fork | no work |
| F-3 | `--ff-only` second-gate behaviour after planning | v0 has **no** mode check at execution; a changed analysis becomes `MergeDrift "member '<id>' merge result changed during preparation"` (`merge/start/prepared.rs:100-106, 114-122`) | PARTIAL | v1 adds `validate_mode` → `MergeValidationFailed "ff_only cannot prepare a true merge"` (`v1_lifecycle/authority/observe/forward.rs:220-228`) | no work — v1 is stricter and better; note the wording change |
| F-4 | `--no-ff` forces a merge commit | v0 cannot: `ExecutionBackend::prepare_merge` calls `prepare_merge_upstream_checked` with no mode (`merge/start/execution.rs:159-166`) | PRESENT | `prepared_mode` → `GitPreparedMergeMode::ForceMergeCommit` (`v1_lifecycle/.../forward.rs:230-236`) | no work |
| F-5 | `-m` body is validated once, then frozen into every participant's `commit_message` at record creation | `runtime/dispatch.rs:283-287`; `merge/validate.rs:27-29`; `merge/start/record.rs:19-36` | PRESENT | `create_record` is shared (`start/record.rs:48-57`); v1 consumes `row.commit_message` from the record (`merge/validate.rs:23-25` states this) | must retain — `start/record.rs` is inside the deleted module |
| F-6 | `--filesystem-strict` is accepted only on a start: `InvalidRequest "--filesystem-strict is accepted only when starting a merge"` | `merge/validate.rs:64-71`; drivers mirror at `gwz-cli/src/clirequest/merge.rs:24-28`, `gwz-py/src/gwz/cli_merge.py:123-127` | PRESENT | shared validator | no work |
| F-7 | `--filesystem-strict` actually refuses a below-bar start | **ABSENT on v0** — `merge/start.rs:113` is the only read and it feeds `v1.start` only | PRESENT | `v1_lifecycle/start.rs:87-95` | no work — the floor raise fixes it (`GwzM5-8M5d-Review.md:F5`; charter §1) |
| F-8 | `--continue` | `merge/continue_op/coordinator.rs:5-133` | PRESENT with differences | `v1_lifecycle/start.rs:212-239` (`V1LifecycleRequest::Continue`) | see §3 (events), §4 (response) |
| F-9 | `--abort` | `merge/abort/mod.rs:36-191` | PRESENT with differences | `v1_lifecycle/start.rs:216` (`V1LifecycleRequest::Abort`) | see §3, §4 |
| F-10 | `--abort --preserve` | `merge/abort/mod.rs:45-49` → `merge/preserve.rs:64-...` | PRESENT | `v1_lifecycle/start.rs:215` (`V1LifecycleRequest::Preserve`); the reverse service re-uses `merge::preserve` (`v1_lifecycle/reverse/execute/preservation.rs:7`) | no work |
| F-11 | A **plain** `--abort` on a `Preserving` record refuses with a named remedy: `MergeRecoveryRequired "merge preservation is incomplete; retry \`gwz merge --abort --preserve\` …"` | `merge/abort/mod.rs:50-58` | UNVERIFIED | `Preserving` is handled inside the v1 reducer/reverse router (`v1_lifecycle/reverse.rs:120`, `transition/reverse_entry.rs:129`, `transition/reduce/mod.rs:255`), but no equivalent operator-facing refusal sentence was found on the `Abort` (non-preserve) arm; tracing the reducer's answer for `Preserving + Abort` is more than a cheap read | must verify, then migrate or rule |
| F-12 | `--abort` on an already-`Aborted` **open** record re-archives idempotently and returns the record | `merge/abort/mod.rs:110-113` | UNVERIFIED | archive-pending retry on v1 goes through `V1ResponseDisposition::ArchiveReady` (`v1_lifecycle/start.rs:167-171`); whether it is reachable for an `Aborted` open record was not traced | must verify |
| F-13 | `--continue` / `--abort` when there is **no open record**, or the id names a closed one | `merge/continue_op/execution.rs:528-546` (`OperationNotFound "there is no open merge to continue"`; completed record returned; else `MergeRecoveryRequired`); `merge/abort/mod.rs:193-219` (`OperationNotFound "no coordinated merge is open"`; idempotent re-archive) | **ABSENT** | `runtime/dispatch.rs:349-356` routes to `v1.command` **only when `classify_open_record` finds a record**; with none, the request falls to `continue_op::handle_continue` / `abort::handle_abort` at `dispatch.rs:366-381` — both v0-engine functions the close deletes | **must migrate** (not named by charter §4/§9) |
| F-14 | `--status` with no id and no open merge → idle response | `merge/status/snapshot.rs:234-238` → `merge/response.rs:7-29` | PRESENT | `merge/status/` is retained (`GwzM5-8M5d-Review.md:1`, kept list) | no work |
| F-15 | `--status` with no id and an open v1 record → live open status | `merge/status/snapshot.rs:236` | PRESENT | `runtime/dispatch.rs:349-352` → `v1_lifecycle/status.rs:9-38` | no work |
| F-16 | `--status <id>` on an archived record → archived projection, either envelope | `merge/status/snapshot.rs:240-254`; `record_wire::decode_archived` handles both | PRESENT | archive decoder retained (charter §5) | no work |
| F-17 | `--gc` while a merge is open refuses: `OpenOperation "cannot collect archived merge records while merge '<id>' is open"` | `merge/gc.rs:186-194` | PRESENT | identical sentence at `v1_lifecycle/start.rs:208-211` | no work |
| F-18 | `--gc` takes the workspace mutator lock before answering | `merge/gc.rs:185` | PARTIAL | the v1 open-merge refusal returns without taking a lock (`v1_lifecycle/start.rs:208-211`) | no work (refusal only) — note the discipline difference |
| F-19 | `--gc` with no id applies retention and returns the idle response | `merge/gc.rs:195-198` | PRESENT | `store/gc.rs` / `store/retention.rs` explicitly **not** carved out (charter §7) | no work |
| F-20 | `--gc <id>` verifies then deletes backup refs, collects, and attaches the archived projection | `merge/gc.rs:199-221` | PRESENT | reads via `decode_archived_common` (both envelopes) at `gc.rs:206` | no work |
| F-21 | Driver-side exclusions: one lifecycle op only; `--ff-only` + `--no-ff` rejected; strict-with-lifecycle rejected — all `InvalidRequest` | `gwz-cli/src/clirequest/merge.rs:7-28`; `gwz-py/src/gwz/cli_merge.py:106-127` | PRESENT | version-blind | no work |
| F-22 | Merge rejects unrelated policies (`--sync`, `--remote`, `--jobs`, `--max-per-host`, `--progress-interval`, `--partial`, `--force`) | `merge/validate.rs:88-109` | PRESENT | shared validator | no work |
| F-23 | `gwz branch --merge <ref>` deprecated alias constructs a merge start | `gwz-cli/src/globalargs/parser.rs:373-378` → `gwz-cli/src/clirequest/merge.rs:63-73` | PRESENT | version-blind | no work |
| F-24 | `--preserve` without `--abort` is accepted by the driver and refused by core on a Start | `gwz-cli/src/tests/g01/commands.rs:306-309`; `merge/validate.rs:12` | PRESENT | shared validator | no work |
| F-25 | `--status` / `--gc` accept an optional merge id (`num_args = 0..=1`) | `gwz-cli/src/globalargs/parser.rs:396-402, 408-414`; `gwz-py/src/gwz/cli_merge.py:61-79` | PRESENT | version-blind | no work |

---

## 3. Events

The census below is exhaustive over non-test call sites. The v1 result is a
single grep: **`emitter` / `EventEmitter` / `EventSink` / `OperationEvent`
appear in `merge/v1_lifecycle/` in exactly one file** — `v1_lifecycle/start.rs`
— at lines `26, 81, 96, 114, 139, 142, 200, 227`. `service::run`'s signature
(`v1_lifecycle/service.rs`) takes no emitter, so everything below it is
event-blind by construction.

Emitter API (`gwz-core/src/operation/push_event.rs:325-561`):
`operation_state_changed:442`, `operation_started:454`, `operation_finished:465`,
`member_started:476`, `member_progress:487`, `member_finished:524`,
`merge_member_finished:535`, `artifact_written:549`, and the generic
`emit:362` (the only route to `EventKind::Diagnostic`).

| # | Behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| E-1 | `OperationStarted` first, `OperationFinished` last, exactly one of each, gapless sequence | `runtime/dispatch.rs:275, 311`; pins `merge/runtime/tests/dispatch.rs:53-71, 235-273` | PRESENT | same bracket — it is **above** the version fork | no work |
| E-2 | `member_started` once per participant, **before** that participant's observation/apply | `merge/start/execution.rs:22`; continue `continue_op/coordinator.rs:53` and `continue_op/reconciliation.rs:54`; abort `abort/mod.rs:148` and `abort/participants.rs:37` | **ABSENT** | zero hits for `member_started` anywhere under `merge/v1_lifecycle/` | must migrate (charter §4) |
| E-3 | `merge_member_finished` once per participant, carrying the `MergeRepoSummary` projection, after that participant's durable row | shared helper `merge/mod.rs:95-108`; call sites `start/execution.rs:59, 90, 109`, `continue_op/coordinator.rs:71, 76, 83, 90`, `continue_op/execution.rs:480`, `continue_op/reconciliation.rs:64`, `abort/mod.rs:153`, `abort/participants.rs:64` | **ABSENT** | zero hits under `merge/v1_lifecycle/` | must migrate (charter §4) |
| E-4 | `artifact_written` for every open-record persist (`.gwz/merge/<id>.yaml`) | `merge/store/persistence.rs:21-30` (`:28`) | **ABSENT** | `v1_lifecycle/store/rewrite.rs:43 create_open` and `:84 commit` write with no emission; no emitter in the file | must migrate (charter §4) |
| E-5 | `artifact_written` for the archive (`.gwz/merge/done/<id>.yaml`) | `merge/store/persistence.rs:32-41` (`:39`) | **ABSENT** | `v1_lifecycle/store/archive.rs` has no emitter | must migrate (charter §4) |
| E-6 | Four `artifact_written` for publication, in order: `git:@root/<commit>`, candidate marker path, `LOCK_PATH`, `.git/info/exclude` | `merge/finalize.rs:308-316`; pin `tests/g23/finalization.rs:298-338`; doc `gwz-cli/docs/MachineOutput.md:396-406` | **ABSENT** | v1 writes the same four artifacts silently: `v1_lifecycle/finalization/execute.rs:55-108` (`EvidenceCommit`, `WriteMarker`, `WriteLock`, `WriteBoundary`, `StageIndex`) | must migrate (charter §4) |
| E-7 | `operation_state_changed` at **every** durable state transition the store commits | `merge/store/persistence.rs:8-19` (`:17`), reached from ~12 `persist_operation_transition` / `enter_finalizing` sites; plus `merge/start.rs:120` | **PARTIAL** | exactly one, at creation: `v1_lifecycle/start.rs:114`. Every later v1 transition (`Executing → AwaitingResolution → Finalizing → Completed`, and the whole reverse path) is silent | must migrate (charter §4) |
| E-8 | Ordering discipline: an event never precedes the durable write it reports | pinned as an exact interleave at `merge/start/tests/execution.rs:414-437`, with counts at `:443` (5 `ArtifactWritten`) and `:453` (2 `merge_member` payloads) | **ABSENT** | no events to order | must migrate (charter §4) |
| E-9 | Continue-arm per-participant events, including the `durable` failure arm's direct `merge_member_finished` | `continue_op/coordinator.rs:53, 71, 76, 83, 90`; pins `merge/start/tests/resolution_race.rs:119-147` | **ABSENT** | as E-2/E-3 | must migrate (charter §4, "Continue and abort arms get the same treatment") |
| E-10 | Abort-arm per-participant events | `abort/mod.rs:148, 153`; `abort/participants.rs:37, 64` | **ABSENT** | as E-2/E-3 | must migrate (charter §4) |
| E-11 | One `EventKind::Diagnostic` / `Severity::Warn`, no member, per below-bar crash-recovery decision | **none on v0** — the v0 start never reaches the decision point | PRESENT (v1-only) | `v1_lifecycle/start.rs:138-150`, called at `:96` (start) and `:227` (continue); pins `tests/g23/crash_recovery.rs:150, 288, 345` | no work — v1 addition, ship (1) |
| E-12 | The human progress line and the JSONL stream depend on member events | `gwz-cli/src/globalargs/dispatch.rs:13-17`; `gwz-cli/src/progress_detail.rs:196-254`; doc `gwz-cli/docs/MachineOutput.md:384-386` ("`MemberFinished` carries the durable merge participant outcome") | consequence of E-2/E-3 | — | must migrate |
| E-13 | Dispatch-level invariants that **require** member events on a successful merge | `merge/runtime/tests/dispatch.rs:197` (at least one `MemberStarted`, `:257-261`; at least one `MemberFinished`, `:262-266`; gapless sequence `:267-273`) and `:303` (which uses a sink that mutates the worktree the instant `MemberStarted` is delivered, `:22-33`) | would fail under floor `V1` | — | must migrate before the floor flip |

---

## 4. Responses

`MergeResponse` has 11 fields (`gwz-core/src/protocol/generated.rs:4987-4999`).
Two builders matter: the v0 completed start uses the **live** record
(`merge/start.rs:149` → `merge/response.rs:104-191`), and the v1 completed start
uses the **archived projection** (`v1_lifecycle/start.rs:167-171` →
`v1_lifecycle/status.rs:111-117` → `merge/response.rs:35-75`).

The known example, verified: **a completed v1 start returns an archived status
response with `repos: Vec::new()` and default `participant_counts`**
(`merge/response.rs:64-68`).

| # | Field / behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| R-1 | `repos[]` — per-repo rows on a **completed** start | `merge/start.rs:149` → `merge/response.rs:117-141` (rows from `selected_targets`) | **ABSENT** | `merge/response.rs:64` — `repos: Vec::new()` | must migrate (charter §4) |
| R-2 | `participant_counts` on a completed start | `merge/response.rs:113-116, 126-129, 165` | **ABSENT** | `merge/response.rs:64` — `MergeParticipantCounts::default()` | must migrate (charter §4) |
| R-3 | `publication_step` on a completed start | `merge/response.rs:169` | **ABSENT** | `merge/response.rs:68` — `publication_step: None` | must migrate (charter §4) |
| R-4 | `preservation[]` (participant rows + `@root` rows) on a completed start | `merge/response.rs:130-153, 168` | **ABSENT** | `merge/response.rs:67` — `preservation: None` | must migrate (charter §4) |
| R-5 | `operation_drift` on a completed start | `merge/response.rs:167` | **ABSENT** | `merge/response.rs:66` — `operation_drift: Vec::new()`; documented as intended for archive-only status at `gwz-cli/docs/MachineOutput.md:249-251` | must migrate (charter §4) + doc reconcile (see OQ-3) |
| R-6 | Start decorations `predicted` / `prediction_complete` on every start row | `merge/start/response.rs:35-51` (`:40-41`), applied at `start.rs:144, 149` | **ABSENT** | `MergeParticipantRecord::to_protocol` sets both `None` (`merge/response.rs:258-259`); `v1_lifecycle/status.rs:16-37` does not restore them | must migrate (charter §4) |
| R-7 | `live_commit` on a **conflicted** start row is the recorded `before_commit` | `merge/start/response.rs:42-48` (`:46`) | PARTIAL | v1 fills `live_commit` from the live observation instead (`v1_lifecycle/status.rs:28`) — usually the same value, but a different provenance and not decoration | must migrate (charter §4 names `live_commit` explicitly) |
| R-8 | `continue_eligible` / `abort_eligible` / `pending_action` / live `drift` on an **open** response | `merge/response.rs:198-223` (`MergeStatusSnapshot::to_response`) | PRESENT | `v1_lifecycle/status.rs:23-35` sets the same five fields from `observe_status_view` | no work |
| R-9 | `open` is true while the record sits in `.gwz/merge`, even when its state is terminal | `merge/response.rs:198-203` (`response.open = true`) | PRESENT | the v1 impl uses the `always` arm (`merge/response.rs:164, 190`) | no work |
| R-10 | `record` projection on a completed start | v0: `source_version: V0`, `archived: false`, `terminal_outcome: None`, `acceptance: None` (`merge/model/record_projection.rs:3-11`) | PARTIAL (deliberate) | v1: `archived: true`, `terminal_outcome: Some`, `acceptance: Some`, `source_version: V1` (`merge/model/record_projection.rs:40-60`, attached at `merge/response.rs:100`) | dropped by charter §4 ("projected from the **archived** record") — but the doc pin at `gwz-cli/docs/MachineOutput.md:243-247` must be re-stated |
| R-11 | `crash_recovery` is **absent** on an ordinary start | today ordinary is v0 and never decides; `merge/start/response.rs:138` and `merge/response.rs:171` set `None` | PARTIAL (shape change) | `v1_lifecycle/start.rs:127` sets `Some(...)` on **every** v1 start | doc must change: `gwz-cli/docs/MachineOutput.md:335-338` says the object is absent on "every start that writes a v0 record (ordinary and `--ff-only` merges)" |
| R-12 | `aggregate_status` → exit code | `merge/response.rs:235-243`; `gwz-cli/src/globalargs/render_exit.rs:22-35` | PRESENT | the same `aggregate_status(state)` serves both `to_response` and `to_v1_response` (`merge/response.rs:159, 178-191`) | no work — residual is which state the v1 reducer writes |
| R-13 | A **halted / conflicted / stopped** start response carries the live snapshot **plus** start decorations | `merge/start.rs:141-145` (`snapshot_status(...).to_response(...)` then `decorate_start_response`) | PARTIAL | v1's `Stopped` disposition answers `open_status` (`v1_lifecycle/start.rs:172-175`) — live fields yes, decorations no | must migrate (charter §4) |
| R-14 | Idle response shape for `--status` with nothing open | `merge/response.rs:7-29` | PRESENT | retained module | no work |
| R-15 | Envelope errors are member-scoped (`member_id`, `member_path`, `target_kind`) | `merge/response.rs:264-272`; `merge/start/response.rs:142-151` | PRESENT | v1 uses `member_error` throughout (e.g. `v1_lifecycle/.../forward.rs:112, 244`) | no work |

---

## 5. `gwz stage` under an open merge

Every row here is the same single defect with different consequences:
`handle_stage.rs:34` discovers the open record through
`merge::FileMergeStore.discover_open`, whose decoder is `decode_production_v0`
(`merge/store/mod.rs:419`) over the `V0_ONLY` installed set
(`merge/store/mod.rs:353-357`). A v1 envelope classifies as
`HeaderClassificationError::Unsupported` (`record_wire/header.rs:159-163`) and
renders as `ErrorCode::UnsupportedRecordVersion`
(`merge/store/compatibility_errors.rs:25-28, 105-128`) with the message
`"merge record '<id>' uses schema 'gwz.merge-operation/v1' version 1, which
requires A1 (v1 integration/acceptance/no-ff); use a compatible newer GWZ"`.

Secondly, the whole merge-scoped stage arm is **typed on the v0 record**:
`handle_open_merge_stage(record: &merge::MergeOperationRecord)`
(`handle_stage.rs:134-140`), `selected_open_merge_targets(record: &merge::
MergeOperationRecord)` (`handle_stage.rs:269-272`),
`merge::root::open_merge_stage_member_paths(..., record: &MergeOperationRecord)`
(`merge/root.rs:31-42`), and `frozen_manifest(..., record: &MergeOperationRecord)`
(`merge/root/reconciliation.rs:13-17`). `MergeOperationRecordV1` is a distinct
struct, so none of these can serve a v1 record.

| # | Behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| S-1 | `gwz stage` under an open merge switches to the merge-scoped arm rather than the ordinary one | `workspace_ops/handle_stage.rs:34-36` | **ABSENT** | v0-only decoder as above; the call errors instead of answering `Some(record)` | must migrate (charter §4) |
| S-2 | Stage member paths come from the **frozen** manifest at the root participant's `before_commit`, not the live manifest | `handle_stage.rs:141` → `merge/root.rs:31-42` → `merge/root/reconciliation.rs:13-53` | **ABSENT** | v0-typed | must migrate (charter §4) |
| S-3 | Selection tokens are restricted to frozen participants; an unknown token refuses `OpenOperation "merge '<id>' is open; selected add target '<t>' is not a frozen merge participant"` | `handle_stage.rs:269-308` | **ABSENT** | v0-typed | must migrate (charter §4) |
| S-4 | Root conflict staging: `@root` maps to `member_path: None` and stages the workspace root | `handle_stage.rs:326-333`; pins `handle_stage.rs:399-421` | **ABSENT** | v0-typed | must migrate (charter §4, "Root conflict staging works") |
| S-5 | Under an open merge, staging uses `stage_paths_allowing_other_conflicts` rather than `stage_paths` | `handle_stage.rs:188` vs `:126` | **ABSENT** | unreachable | must migrate |
| S-6 | A dry-run stage under an open merge validates materialization and stages nothing | `handle_stage.rs:165, 180-182` | **ABSENT** | unreachable | must migrate |
| S-7 | An unmaterialized participant refuses `MemberNotFound "merge participant '<p>' is not materialized"` (distinct from the ordinary arm's "member '<p>' is not materialized; cannot stage") | `handle_stage.rs:171-179` vs `:112-118` | **ABSENT** | unreachable | must migrate |
| S-8 | `gwz add` targets are gated to conflicted `NativeMerge` participants; otherwise `OpenOperation "merge '<id>' is open; add may target only its conflicted participants; …"` | `merge/runtime/open_gate.rs:32-66` (`:37` is the v0-only discovery) | **ABSENT** | same decoder defect; also reads `record.participants` off the v0 struct | must migrate |
| S-9 | The ordinary (no open merge) stage arm refreshes `.git/info/exclude` for a root stage, and never under dry-run | `handle_stage.rs:96-100` | PRESENT | unaffected by the merge version | no work |
| S-10 | `gwz stage` takes the mutation guard with the request's dry-run answer | `handle_stage.rs:27-32` (`acquire_workspace_mutation_guard`) | PARTIAL | the guard's own discovery is the O-3 defect below | must migrate |

---

## 6. Other verbs that observe an open merge

**This section is the largest finding not named by charter §4.**

Merge's own verbs are routed by envelope (`runtime/dispatch.rs:349-356`).
Nothing else is. Every other consumer of merge state discovers through
`FileMergeStore::discover_open`, i.e. the v0-only body decoder. Today that is
survivable because only `--no-ff` writes v1; under floor `V1` it is the normal
case.

| # | Behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| O-1 | The open-merge gate table: `Block` for branch-mutate, capture, commit, forall, init-update, materialize, pull, push, repo-mutate, snapshot, stash-mutate, tag-mutate and merge-start; `Allow` for the read verbs and `merge --status/--continue/--abort/--gc`; `Conditional` for stage; `NotGated` for clone/init-new | `gwz-core/src/operation/open_merge_gate.rs:45-79` | PRESENT (the **decision** is version-blind) | — | no work |
| O-2 | Gate **discovery** finds the open record | `merge/runtime/open_gate.rs:20-25` — `store.discover_open` | **ABSENT for v1** | `merge/store/mod.rs:82-99` → `read_record` `:385-389` → `decode_production_v0` `:419`; a v1 envelope errors (`record_wire/header.rs:159-163`) | **must migrate** |
| O-3 | `acquire_workspace_mutation_guard` discovery, used by `gwz commit`, `gwz stage`, `gwz snapshot`/`capture`, `gwz tag`, the conf gate, and gwz-cli's `forall` guard | `merge/runtime/mutation_guard.rs:94-99`; callers `workspace_ops/handle_commit.rs:37`, `handle_stage.rs:27`, `handle_materialize.rs:27, 96`, `handle_tag.rs:32`, `workspace_bootstrap/conf_gate.rs:296`, `gwz-cli/src/globalargs/dispatch.rs:28` | **ABSENT for v1** | same decoder | **must migrate** |
| O-4 | The merge **start** gate: an ordinary start while a merge is open refuses `OpenOperation` | `runtime/dispatch.rs:289-299` → `mutation_guard.rs:110-128` → `open_gate.rs:12-30`; pins `tests/g23/fixtures.rs:329-360` (`assert_eq!(error.code, ErrorCode::OpenOperation)` at `:356`, over **both** dry-run and real, `:337`) and `tests/g23/open_operation_gate.rs:4, 22, 45, 60` | **ABSENT for v1** | same decoder; the refusal becomes `UnsupportedRecordVersion "… use a compatible newer GWZ"` | **must migrate** |
| O-5 | `handle_start_durable`'s own `OpenOperation` refusal (`"merge '<id>' is open; use merge status, merge continue, or merge abort"`) | `merge/start.rs:75-80, 152-157` | shadowed | on the authoritative path `enforce_start_gate` is `true` (`runtime/dispatch.rs:221`), so `guarded_workspace_root` (O-4) fires first; `start.rs:75-80` is reachable only through the `#[cfg(test)]` dependency-injected seam (`runtime/dispatch.rs:227-248`) | observation — charter §2/§10.3 names this site for the v0-refuse text; note it is not the live gate |
| O-6 | Recovery-root resolution finds the workspace by envelope first, then by the v0 store | `runtime/dispatch.rs:491-514` (`discover_open_envelope_before_manifest` at `:507`, then `discover_open_before_manifest` at `:510`) | PRESENT | envelope-first already | no work — the v0 fallback leaves with the engine |
| O-7 | The refusal text a blocked verb prints: `OpenOperation "merge '<id>' is open; this command is blocked until it is recovered; use merge status, merge continue, or merge abort"` | `gwz-core/src/operation/open_merge_gate.rs:93-99` | **ABSENT for v1** | never reached — O-2/O-3 error first | **must migrate**; charter §2 separately suppresses this remedy for a v0 envelope |
| O-8 | The cross-driver open-merge-gate matrix (four states × dry-run × both drivers) asserts `OpenOperation` and byte-identical workspaces | `gwz-py/src/tests/test_merge_cli_cross_driver.py:145-163` (hand-written **v0** record fixture), `:193-249` | fixture is v0-only | — | must migrate (test re-point) |
| O-9 | `gwz add` target gate — see S-8 | `merge/runtime/open_gate.rs:32-66` | **ABSENT for v1** | same decoder | must migrate |
| O-10 | `gwz-cli` deliberately does **not** pre-gate merge itself, so the operation bracket events fire even on refusal | `gwz-cli/src/globalargs/open_merge_gate.rs:43-46` | PRESENT | version-blind | no work |

---

## 7. Error codes and refusal messages

Codes present anywhere under `merge/` (production and tests), by frequency, are
led by `MergeRecoveryRequired` (131), `MergeRecordUnreadable` (115),
`MergeDrift` (78) and `GitCommandFailed` (59). The rows below are the ones whose
**reachability or wording changes** at the close.

| # | Code and message shape | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| X-1 | `UnsupportedRecordVersion` — "merge record '<id>' uses schema 'gwz.merge-operation/v1' version 1, which requires A1 …; use a compatible newer GWZ" | `merge/store/compatibility_errors.rs:105-128`, raised from `merge/store/mod.rs:419` | becomes the **wrong** answer at six non-merge sites | O-2, O-3, O-4, S-1, S-8 | **must migrate** — a 0.14 binary telling the user to get a newer GWZ for a record it just wrote is the sharpest single defect in this inventory |
| X-2 | `OpenOperation` — the blocked-command remedy | `gwz-core/src/operation/open_merge_gate.rs:93-99` | unreachable for a v1 record | X-1 | must migrate |
| X-3 | `OpenOperation` — the merge start gate remedy | `merge/start.rs:152-157` | shadowed (O-5) | `runtime/dispatch.rs:290` | charter §2 suppresses it for a v0 envelope; note it is not the live text |
| X-4 | `UnsupportedLegacyMode` — "merge record '<id>' carries v0 mode 'no_ff'; no supported v0 writer produces no-ff records, so v0 refuses it before resume or mutation" | `merge/continue_op/execution.rs:26-35` | becomes unreachable | the v0 continue leaves production | dropped by charter §10.1 (I2 §5:183-184: "Do not keep a special `no_ff` resume path") |
| X-5 | `MergeValidationFailed` — "merge requires a merge commit but --ff-only was requested" (plan time) | `merge/plan.rs:235-239` | PRESENT | shared plan | no work |
| X-5b | `MergeValidationFailed` — "ff_only cannot prepare a true merge" (run time) | none on v0 | v1-only | `v1_lifecycle/authority/observe/forward.rs:222-225` | no work — new, stricter |
| X-6 | `MergeRecoveryRequired` — "merge preservation is incomplete; retry `gwz merge --abort --preserve` …" | `merge/abort/mod.rs:53-56` | UNVERIFIED | see F-11 | must verify |
| X-7 | `OperationNotFound` — "there is no open merge to continue" / "no coordinated merge is open" | `merge/continue_op/execution.rs:535-538`; `merge/abort/mod.rs:201-204` | **ABSENT** | F-13 — these arms are v0-engine code and are unreachable from `v1.command` | **must migrate** |
| X-8 | `MergeRecoveryRequired` — "merge '<id>' in state {state:?} cannot be continued" / "merge in state {state:?} cannot be aborted" | `merge/continue_op/execution.rs:548-553`; `merge/abort/mod.rs:100-103, 208-214` | **ABSENT** | F-13 | **must migrate** |
| X-9 | `UnsupportedOperation` — "checked <label>: durable filesystem identity is unsupported: <cause>" on record create | reachable today only from a `--no-ff` start (`checked_artifact/entry.rs:541-561` → `checked_artifact/observation.rs:372-377`) | becomes reachable on **ordinary** starts | `v1_lifecycle/store/rewrite.rs:63` is the only caller of `create_merge_store_record` | cured by charter §3 (raw create on the handle-fail arm) — this row is the reason §3 exists |
| X-10 | `MergeDrift` — "member '<id>' changed after merge planning" / "…merge result changed during preparation" | `merge/start/prepared.rs:102-106, 114-122` | PARTIAL | v1 routes semantic drift to `ambiguity` (`v1_lifecycle/.../forward.rs:110`) and preparation failure to a `Failed` row (`:114, 238-250`); wording and shape differ | must verify wording expectations, then rule |
| X-11 | `MergeIdMismatch` — "requested merge does not match the open merge '<id>'" | `merge/validate.rs:74-82` | PRESENT | `v1_lifecycle/start.rs:203` calls the same validator | no work |
| X-12 | `MergeRecoveryRequired` — "multiple merge records exist under '<dir>'" | `merge/store/mod.rs:91-97` (v0 path) and `:248-254` (envelope path) | PRESENT | the envelope classifier raises the same code and text | no work |
| X-13 | `MergeRecoveryRequired` — status contention ("merge record '<id>' changed during both status attempts") | `merge/status/snapshot.rs:220-225` | PARTIAL | v1 has its own: "checked v1 source '<id>' changed during both status attempts" (`v1_lifecycle/status.rs:104-109`) | no work — note the wording change |
| X-14 | `ArchivedRecordUnreadable` — "archived merge record '<id>' is unreadable: archive envelope or terminal state is contradictory" | `merge/store/compatibility_errors.rs:96-103` | PRESENT | archive decoder retained (charter §5) | no work |

---

## 8. Machine output

| # | Behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| M-1 | The human renderer never prints `crash_recovery` | `gwz-cli/src/merge_render.rs:242-245`; pin `gwz-cli/src/tests/g02.rs:359` | PRESENT | renderer is version-blind | no work |
| M-2 | Human output of a completed start lists participants | `gwz-cli/src/merge_render.rs:88-90` (`participants:` only when `repos` is non-empty), `:156-183` (`participants: total 0` when every count is zero) | **ABSENT under v1** | consequence of R-1/R-2 — a completed v1 start prints `participants: total 0` and no participant block | must migrate (charter §4) |
| M-3 | Human `record:` line prints the record version and archived flag | `gwz-cli/src/merge_render.rs:20-25`; `debug_kebab` `:193-204`; fixture `gwz-cli/tests/fixtures/merge_status_human.txt:5` (`record: v1 (open)`) | PARTIAL | ordinary merges will print `v1` where they printed `v0`; a completed start will print `(archived)` where it printed `(open)` | must migrate (fixture + doc) |
| M-4 | The JSON `crash_recovery` key is **inserted only when present** | `gwz-cli/src/merge_render.rs:246-248`; `gwz-py/src/gwz/cli_render_parts/machine.py:114-116` | PARTIAL | R-11 — the key appears on every start under floor `V1` | doc `gwz-cli/docs/MachineOutput.md:335-338` must flip |
| M-5 | JSON `record.source_version` is `"V0"` or `"V1"` | `gwz-cli/src/merge_render.rs:260-273`; doc `gwz-cli/docs/MachineOutput.md:243` | PARTIAL | ordinary merges emit `"V1"` | doc must change; the enum keeps both values for archive reads (charter §5) |
| M-6 | Merge is never rendered in porcelain | `gwz-cli/src/clirequest/invocation.rs:19-29` (porcelain is a `status` flag only); `gwz-cli/src/globalargs/render_exit.rs:18` does not special-case merge | PRESENT | version-blind | no work |
| M-7 | JSONL `MemberFinished` carries the durable merge participant outcome | doc `gwz-cli/docs/MachineOutput.md:384-386`; emitter `gwz-core/src/operation/push_event.rs:535-547` | **ABSENT under v1** | E-3 | must migrate (charter §4) |
| M-8 | JSONL publication artifacts arrive in a documented order | doc `gwz-cli/docs/MachineOutput.md:396-406`; `merge/finalize.rs:308-316` | **ABSENT under v1** | E-6 | must migrate (charter §4) |
| M-9 | The JSONL stream contains exactly one final `kind: "response"` object | doc `gwz-cli/docs/MachineOutput.md:409-410`; `gwz-cli/src/append_branch_summary/machine.rs:19-31` | PRESENT | version-blind | no work |
| M-10 | gwz-py renders the same human and JSON shapes | `gwz-py/src/gwz/cli_render_parts/merge.py:8-126`; `.../machine.py:72-131` | follows core | — | no work; py fixtures re-point with the Rust ones |
| M-11 | The canonical cross-driver fixture pins the whole merge JSON | `gwz-core/protocol/fixtures/cli_parity/merge_response.json`, asserted at `gwz-cli/src/tests/g02.rs:199-207` and `gwz-py/src/tests/test_cli_merge.py:254-261`; doc `gwz-cli/docs/MachineOutput.md:340-347` | PRESENT (already a v1 projection) | `gwz-cli/src/tests/g02.rs:483` uses `MergeRecordVersion::V1` | no work |
| M-12 | The protocol wire fingerprint gate trips on any protocol change | `gwz-py/scripts/check_protocol_drift.py:26-28` | n/a | — | must re-pin when `MergeCrashRecovery.handles_ok` is added (charter §3, additive only) |
| M-13 | Docs manifest pins "Ordinary and custom-message starts continue to write v0 records" | `gwz-core/scripts/checks/merge_docs_manifest.json:331-333` → `gwz-cli/docs/commands/merge.md:382-384` | must become false | — | must migrate (charter §5) |
| M-14 | The retained-reader manifest describes the shipped generation as "ordinary and custom-message starts still create v0" | `gwz-core/scripts/retained_readers/manifest.json:26-28` | must become false | — | must migrate (charter §5) |
| M-15 | Driver v0 fixtures: renderer projection, stage gate, py gate matrix, py native lifecycle | `gwz-cli/src/tests/g02.rs:292`; `gwz-cli/src/tests/m2c.rs:58-59`; `gwz-py/src/tests/test_merge_cli_cross_driver.py:149-150`; `gwz-py/src/tests/test_native_merge_operations.py:143-144` | hand-written v0 records | — | must migrate (re-point or convert to the §2 refuse fixture) |
| M-16 | **No driver test asserts that an ordinary merge writes a v0 record.** The v0 claim lives in docs, not tests | see M-13, M-14 and M-15 (all four v0 touchpoints are fixtures, not assertions about a merge the driver performed) | — | — | observation — the driver corpus is cheaper to flip than the memo implies |

---

## 9. Git and workspace side effects

| # | Effect | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| G-1 | The open record is written atomically and read back byte-verified | `merge/store/mod.rs:121-155` (`write_open`), `:463-499` (`write_atomic_verified`) | PARTIAL (different door, same file) | `v1_lifecycle/store/rewrite.rs:43-75 create_open` publishes through `checked_artifact::entry::create_merge_store_record` (`:63`), then re-reads and compares (`:65-70`); `:84 commit` keeps `rename_durable(replace)` under the record-root exception (`:77-83`) | no work — charter §3 governs the below-bar arm |
| G-2 | `.gwz/merge` parents are bootstrapped before the first record | `merge/store/mod.rs:135-137` (`prepare_merge_store_parents`) | PARTIAL | bootstrapped by the creation lease instead (`v1_lifecycle/start.rs:102-113`); an unbootstrapped parent is refused, not created (`v1_lifecycle/store/rewrite.rs:35-41`) | no work |
| G-3 | Unknown record fields survive a rewrite | `merge/store/mod.rs:140-144` (`carry_unknown`, `:502-...`) | PRESENT | `v1_lifecycle/store/unknown.rs:12-...` overlays an `UnknownFieldManifest` | no work |
| G-4 | The terminal record is renamed into `.gwz/merge/done/<id>.yaml` | `merge/store/mod.rs:157-159` → `store/archived.rs` | PRESENT | `v1_lifecycle/store/archive.rs` | no work — the **event** is absent (E-5) |
| G-5 | Each participant's merge is prepared then executed against the recorded `(branch, before, source)` triple, leaving `MERGE_HEAD` and conflicted index stages on conflict | `merge/start/prepared.rs:84-140`; `merge/start/execution.rs:151-194` | PRESENT | `v1_lifecycle/authority/observe/forward.rs` (prepare) + `v1_lifecycle/forward/execute.rs` (execute) | no work |
| G-6 | A conflicted participant's pristine conflict snapshot is captured into the record | `merge/start/execution.rs:35-51` | PRESENT | `v1_lifecycle/.../forward.rs:326, 398-406, 493` | no work |
| G-7 | The pending action is made durable **before** the git merge and cleared **after** it | `merge/start/execution.rs:31-32` then `:53-58` | PRESENT (in shape) | v1's journal/transition kernel commits before executing (`v1_lifecycle/service/execution.rs`, `store/rewrite.rs:84`) | no work — the **event ordering** pin (E-8) is what is lost |
| G-8 | Publication writes the evidence commit, the marker, the lock, the `.git/info/exclude` boundary, and stages the index | `merge/finalize.rs:272-318`; `merge/finalize_dispatch.rs`; `merge/finalize_support.rs` | PRESENT | `v1_lifecycle/finalization/execute.rs:55-108` (`EvidenceCommit`, `WriteMarker`, `WriteLock`, `WriteBoundary`, `StageIndex`) | no work — events absent (E-6) |
| G-9 | Preservation creates backup refs and stashes, per participant and for the root | `merge/preserve.rs:109-150` | PRESENT | the v1 reverse path re-uses `merge::preserve` (`v1_lifecycle/reverse/execute/preservation.rs:7`, `.../reverse/preservation/phase.rs:13, 406, 437`) | no work |
| G-10 | GC deletes backup refs by exact commit before collecting | `merge/gc.rs:208, 215-220` | PRESENT | retained module | no work |
| G-11 | Lock discipline: v0 `--continue` / `--abort` take `WorkspaceMutatorLock::acquire` themselves | `merge/continue_op/coordinator.rs:13`; `merge/abort/mod.rs:44` | PARTIAL | v1 takes a `V1MutationLease` inside `service::run`; the start guard is explicitly dropped before the handoff (`merge/start.rs:98-106`) | no work — documented at `merge/start.rs:98-105` |
| G-12 | A dry-run start writes nothing at all | §1.4 | PRESENT | §1.1 | no work + pin |

---

## 10. Root participant, drift, and publication/preservation ordering

| # | Behaviour | v0 evidence | v1 status | v1 evidence | Verdict |
|---|---|---|---|---|---|
| P-1 | `@root` is planned only when explicitly selected, with the frozen lock/manifest commit digests | `merge/plan.rs:155-210`; `merge/root/planning.rs:13-...` | PRESENT | shared plan, above the fork | no work |
| P-2 | A detached or unborn workspace root refuses `BranchDetachedHead "workspace root must be on an attached branch to publish merge evidence"` when any participant is not up to date | `merge/plan.rs:56-67` | PRESENT | shared plan | no work |
| P-3 | Root drift during planning refuses `MergeDrift "workspace root changed while its merge plan was being frozen"` | `merge/plan.rs:200-208` | PRESENT | shared plan | no work |
| P-4 | v1 supports root participants through finalization, acceptance and rollback | — | PRESENT | `v1_lifecycle/authority/observe/finalization.rs:113, 177, 432`; `.../reverse/rollback_prefix.rs:72, 120, 177-181`; `merge/acceptance/v1_candidate.rs`; suite `v1_lifecycle/tests/finalization_root.rs`, `tests/c7_matrix/root_rows.rs` | no work |
| P-5 | Provenance of the published lock: v0 reconstructs it, v1 replays `accepted.lock.exact_yaml` | `merge/finalize.rs:280-296` | PARTIAL (known) | `v1_lifecycle/authority/observe/finalization.rs:273` | dropped by charter §1/§4 (shape parity, not byte-identical provenance) — recorded in `GwzM5-8M5c-Charter.md:54-58` |
| P-6 | Root conflict staging under an open merge | S-4 | **ABSENT** | S-1..S-4 | must migrate (charter §4) |
| P-7 | Abort ordering: evidence rollback → verify evidence baseline → participant rollback → restore baseline → verify baseline → `Aborted` → archive | `merge/abort/mod.rs:175-189` | UNVERIFIED | the v1 reverse service has its own phase kernel (`v1_lifecycle/reverse.rs`, `.../reverse/execute/rollback.rs`, `.../transition/reduce/mod.rs`); proving the same external ordering is more than a cheap read | must verify (charter §4's "same ordering discipline" applies to events; the git ordering is not restated) |
| P-8 | Preserve-then-abort ordering: enter `Preserving` → per-plan backup ref → root prefix persist → stash → abort | `merge/preserve.rs:99-150` | PRESENT (by module reuse) | `v1_lifecycle/authority/observe/reverse/preservation/phase.rs`, `.../phase/steps.rs`, `.../cursor.rs` all drive `merge::preserve` | no work |
| P-9 | Drift is cleared from the record for specific root-candidate kinds during abort | `merge/abort/mod.rs:116-138` | UNVERIFIED | not traced | must verify |
| P-10 | `operation_drift` and per-participant `drift` are re-observed live on an open status | `merge/response.rs:204-221`; `merge/status/snapshot.rs:285-...` | PRESENT | `v1_lifecycle/status.rs:18-35` uses the same `observe_status_view` with `MergeStatusRecordView::from_v1` | no work |
| P-11 | Drift on a **completed** start response | `merge/response.rs:167` | **ABSENT** | R-5 | must migrate (charter §4) |

---

## 11. Ranked work list — ABSENT and PARTIAL rows by user-visible severity

Rank 1 is "a user who upgrades will hit this on an ordinary merge and be
misdirected or blocked". Rank 5 is cosmetic or documentation-only.

### Rank 1 — misdirection or hard block on the ordinary path

1. **O-2 / O-3 / O-4 / O-7 / S-1 / S-8 / X-1** — the open-merge gate, the
   workspace mutation guard, the merge start gate and `gwz stage` / `gwz add`
   all discover through the v0-only decoder. Under floor `V1`, `gwz commit`,
   `push`, `pull`, `snapshot`, `capture`, `forall`, `branch`/`repo`/`stash`/`tag`
   mutate, `init --update`, `materialize`, a second `gwz merge`, and `gwz stage`
   all answer `UnsupportedRecordVersion … use a compatible newer GWZ` instead of
   `OpenOperation … use merge status, merge continue, or merge abort`. The user
   is told to upgrade a binary that just wrote the record.
   Sites: `merge/runtime/open_gate.rs:22, 24, 37`;
   `merge/runtime/mutation_guard.rs:95`; `workspace_ops/handle_stage.rs:34`.
2. **S-1 … S-7, P-6** — `gwz stage` under an open merge has no v1 path at all,
   and the whole merge-scoped arm is typed on `MergeOperationRecord`
   (`handle_stage.rs:134, 269`; `merge/root.rs:31`;
   `merge/root/reconciliation.rs:13`). A conflicted ordinary merge becomes
   unresolvable without this. Charter §4 names it.
3. **F-13 / X-7 / X-8** — the "no open merge" and "wrong state" answers for
   `--continue` and `--abort` live in the v0 engine
   (`merge/continue_op/execution.rs:528-553`; `merge/abort/mod.rs:193-219`) and
   are unreachable from `v1.command` (`runtime/dispatch.rs:349`). Deleting the
   engine removes them. Not named by the charter.

### Rank 2 — the merge appears to have done nothing

4. **R-1, R-2, R-3, R-4, R-5, R-11 (M-2)** — a completed v1 start returns
   `repos: []`, default counts, `preservation: None`, `publication_step: None`,
   `operation_drift: []` (`merge/response.rs:64-68`), so `gwz merge <ref>`
   prints `participants: total 0` and no participant block
   (`gwz-cli/src/merge_render.rs:88-90, 174-175`). Charter §4.
5. **E-2, E-3, E-9, E-10, M-7** — no `member_started` and no
   `merge_member_finished` anywhere on v1, so the human progress line goes
   silent and every JSONL consumer loses the per-participant outcome
   (`gwz-cli/src/progress_detail.rs:196-254`;
   `gwz-cli/docs/MachineOutput.md:384-386`). Charter §4.
6. **E-4, E-5, E-6, M-8** — no `artifact_written` on v1 for the record, the
   archive, or the four publication artifacts, whose order is documented
   (`gwz-cli/docs/MachineOutput.md:396-406`). Charter §4.
7. **E-7** — `operation_state_changed` collapses from every durable transition
   to one at creation (`v1_lifecycle/start.rs:114`). Charter §4.

### Rank 3 — degraded but not misleading

8. **R-6, R-7, R-13** — start decorations `predicted` / `prediction_complete` /
   conflicted-row `live_commit` are never applied on v1
   (`merge/response.rs:258-259`). A conflicted ordinary start loses its
   prediction columns. Charter §4.
9. **E-8, E-13** — the write-before-event ordering pin
   (`merge/start/tests/execution.rs:414-437`) and the dispatch invariants
   (`merge/runtime/tests/dispatch.rs:257-266`) have nothing to assert on v1 and
   will fail at the floor flip if parity has not landed first.
10. **D-3** — the dry-run open-merge gate inherits the Rank-1 defect: a
    `--dry-run` start while an ordinary merge is open misreports.
11. **D-12** — the dry-run implementation lives in `merge/start/response.rs` and
    `merge/start/prepared.rs`, inside the deleted module. Not named by the
    charter; a silent deletion here is exactly the v0.8.0 class.
12. **F-11 / X-6, F-12, P-7, P-9, X-10** — unverified reverse-path behaviours:
    the `Preserving` plain-abort refusal, the idempotent re-archive of a
    terminal `Aborted` record, abort ordering, drift clearing, and drift
    wording.

### Rank 4 — protocol and lock discipline

13. **R-11 / M-4** — `crash_recovery` moves from absent to present on every
    start. Additive in shape, but a machine-visible change and a documented
    reversal (`gwz-cli/docs/MachineOutput.md:335-338`).
14. **R-10 / M-3 / M-5** — `record.archived` flips `false → true` and
    `source_version` `V0 → V1` on a completed ordinary start. Deliberate
    (charter §4) but doc-visible.
15. **G-11, F-18** — lock/lease discipline differences between the engines.
16. **X-13, F-3/X-5b** — changed refusal wording (`status contention`,
    `ff_only`).

### Rank 5 — documentation, pins and fixtures

17. **M-13** `merge_docs_manifest.json:331-333` → `merge.md:382-384`;
    **M-14** `retained_readers/manifest.json:26-28`;
    **M-15** four driver v0 fixtures; **M-12** the protocol fingerprint;
    **D-16** the dry-run `crash_recovery` doc sentence; **R-5/OQ-3** the
    archive-only status sentence.
18. **X-4** — `UnsupportedLegacyMode` becomes unreachable. Deliberate
    (charter §10.1).
19. **O-5** — `merge/start.rs:75-80` is a shadowed gate; the charter's §2 edit
    there is correct but is not the live refusal path.

### Row census

Rows per section: §1.6 dry-run 16 (D), §2 modes and flags 25 (F), §3 events 13
(E), §4 responses 15 (R), §5 `gwz stage` 10 (S), §6 other verbs 10 (O), §7 error
codes 15 (X, including X-5b), §8 machine output 16 (M), §9 git side effects 12
(G), §10 root/drift/ordering 11 (P).

| Verdict class | Count | Rows |
|---|---|---|
| no work | 69 | D-1/2/4-11/15; F-2/3/4/6/7/10/14-25; E-1/11; R-8/9/12/14/15; S-9; O-1/6/10; X-5/5b/11/12/13/14; M-1/6/9/10/11; G-1…G-12; P-1/2/3/4/8/10 |
| must migrate | 56 | D-3/12/16; F-5/13; E-2…E-10/12/13; R-1…R-7/11/13; S-1…S-8/10; O-2/3/4/7/8/9; X-1/2/7/8; M-2/3/4/5/7/8/12/13/14/15; P-6/11 |
| must verify (UNVERIFIED — a cheap-to-medium trace, not a grep) | 6 | F-11; F-12; X-6; X-10; P-7; P-9 |
| deliberately dropped or changed by the charter (section cited in the row) | 5 | F-1 (§1); R-10 (§4); X-4 (§10.1); X-9 (§3); P-5 (§1/§4) |
| needs an operator ruling | 2 | D-13; D-14 |
| observation only (no verdict) | 5 | F-8; F-9; O-5; X-3; M-16 |
| **Total rows** | **143** | |

By v1 status: **PRESENT 68, ABSENT 40, PARTIAL 21, UNVERIFIED 5**, plus 9 rows
carrying no v1 status (the change itself, a shadowed site, an observation, or a
doc/pin/fixture obligation).

---

## 12. Open questions

**OQ-1 — Should the non-merge gates become envelope-aware in the same landing
as the floor raise, or before it?** Charter §9 keeps the production §2 refuse
and the floor flip together in landing (4), and §10.3 names
"`discover_open` envelope-only, mutation gate / `open_gate` / `handle_stage`
occupancy" as affected interfaces. But those are named for the **open-v0
refuse** direction. The **open-v1 serve** direction (O-2, O-3, O-4, S-1, S-8) is
a separate change, it is behaviour-preserving today (nothing ordinary writes
v1), and if it does not land with or before the flip, the flip ships the Rank-1
defect. Recommendation: fold it into charter §9 step (2) as an explicit
deliverable, not into the close.

**OQ-2 — Who answers `gwz merge --continue` / `--abort` when nothing is
open?** Today `merge/continue_op/execution.rs:528-546` and
`merge/abort/mod.rs:193-219` do, and both are v0-engine files.
`runtime/dispatch.rs:349` only routes to `v1.command` when a record exists. The
close must re-home four behaviours: "there is no open merge to continue", "no
coordinated merge is open", the completed-record answer for a named id, and the
idempotent re-archive of an already-`Aborted` id. Charter §6 does not list them
and §8's acceptance does not exercise them.

**OQ-3 — Does charter §4's "completed start returns per-repo rows projected
from the archived record" contradict `MachineOutput.md:249-251`?** That doc
sentence says "Archive-only status does not inspect repositories. Its `repos`
and `operation_drift` arrays are empty." If a completed v1 start returns rows
from the archived bytes, the sentence must be narrowed to `--status <archived-id>`
or the response must be distinguished from archive-only status. This is a
protocol-doc decision, not an implementation one.

**OQ-4 — Should `--dry-run --filesystem-strict` refuse on a below-bar volume?**
Today it predicts: the dry-run return at `merge/start.rs:68-70` precedes the
decision point at `v1_lifecycle/start.rs:87-95`, so the flag has no effect on a
dry run under any floor. Charter §8 asserts both "`--filesystem-strict` refuses
before writes" and "`--dry-run` predicts, writes nothing" without ordering them.
Default if unruled: keep today's behaviour and pin it.

**OQ-5 — Should merge migrate onto the `WorkspaceMutationAccess` seam?** Merge
is the one mutating verb family the 2026-09-02 fix did not migrate
(`GwzDryRunClassFix-Review.md:118-119`), and its dry-run arm still takes no
mutator lock while every migrated handler's `PlanOnly` arm does
(`mutation_guard.rs:31-35` vs `:116-122`). The current guarantee is structural
(one early return) rather than type-level. That is defensible; it is also
exactly the shape the fix was written to retire. Out of M5d scope unless the
operator wants it folded in.

**OQ-6 — Is the ordering of git side effects on abort part of "shape parity"?**
Charter §4 pins event shape, ordering discipline and per-participant counts. It
does not pin the **git** ordering of the reverse path (P-7). If the v1 reverse
service reorders evidence rollback against participant rollback, no charter
clause forbids it and no row in this inventory can call it a defect. A ruling
would let P-7 and P-9 be closed by reading rather than by measurement.

**OQ-7 — Which of the six UNVERIFIED rows must be closed before the close
lands?** F-11/X-6, F-12, P-7, P-9, X-10, G-3-adjacent. Each is a cheap-to-medium
trace through the v1 reducer, but none is a grep. Recommendation: close F-11 and
F-12 (operator-facing refusals) before the floor flip; defer P-7, P-9 and X-10
to the close's dual as read-only review items.
