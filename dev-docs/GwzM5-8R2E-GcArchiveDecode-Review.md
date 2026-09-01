# GC v1-archive decode — single-axis peer-blind review (Opus, interior tier)

**Reviewer lane:** interior tier, single axis, peer-blind.
**Date:** 2026-09-02.
**Order under review:** the operator's standalone launch item
(`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`):

> * GC: `gc.rs` `decode_production_v0` on archive bytes — completed `--no-ff`
>   since 0.11.0 is un-GC-able. Read-side, no probe. Fix it.

with the binding constraint from the same ruling: *"Ordinary merge / commit /
create / abort / GC stay capability-free."*

**Candidate:** branch `gc/v1-archive-decode`, base `main` @ `7f28907`.

| commit | subject | ins/del | files | cap |
| --- | --- | --- | --- | --- |
| (b) `4686c54c53bd24417896ede5c4984ee7d240a5e0` | `fix: GC reads an archived record over both installed envelopes` | 222 / 22 | 10 | 222-MEASURED (re-ruled 200 → 222) |
| (c) `98f5f9025da21459c90883467377dcc0773432b6` | `test: pin that a completed --no-ff archive is collected end to end` | 86 / 6 | 2 | 150 |

**Review worktree:** `…/scratchpad/gc-wt`, byte-clean at (c) on entry and on
exit (`git status --porcelain` empty, `HEAD` = `98f5f90`). Every probe and
every ablation was run in **separate materialized trees**
(`git archive <rev> | tar -x`), so the review worktree was never mutated at
all. Reviewer target dir: `…/scratchpad/gc-review-target` (own; deleted at
close). Checker: `/opt/homebrew/bin/python3.13`. Direct exit codes throughout.

---

# VERDICT: **GO-WITH-CONDITIONS**

**Escalation fires** — one **[P2]**.

The defect is real, the diagnosis is exact, and the cure is correct at all
three named sites: I reproduced the shipped failure at each site on `7f28907`
and the cure at each site on (c), including the id-less retention sweep under
**production** semantics. The projection is field-for-field sound, no v1 record
ever claims to be v0 downstream, GC remains capability-free by grep and by
chain, and no newly-decodable archive shape becomes deletable — the shared
decoder is in fact **stricter** than the v0 path it mirrors. The builder's
self-corrected FALSE ALARM is verified correct on real bytes.

The condition is that **one of the three cured sites carries no test row at
all**. Reverting the `store/retention.rs` hunk on the delivered tree leaves the
entire g23 battery green — 129 passed, 0 failed. The other two sites fail
closed under the same ablation. That site can silently regress into exactly the
defect this lane exists to remove.

| finding | grade | one line |
| --- | --- | --- |
| [P2-1] | P2 | The `store/retention.rs` cure is unpinned; the whole g23 suite passes with the hunk reverted. |
| [P3-1] | P3 | `(b)` claims refusal happens "before any ref is observed"; true of the decode refusal, not of the archive-projection refusal. |
| [P3-2] | P3 | `no_ff_wire.rs`'s new comment over-claims `--no-ff` as "the only production door to a v1 archive". |
| [P3-3] | P3 | `(b)`'s headline equates "archived under v1" with "every `--no-ff` merge", under-counting the blast radius for the same reason. |
| [P3-4] | P3 | Undisclosed reach: the cure also widens what *ordinary retention* sweeps, on every v0-lifecycle archive operation, not only on `--gc`. Release-note-worthy. |
| [P3-5] | P3 | Handoff says "the other ten `read_record` callers"; it is nine. Documentary only — the figure is not in the tree. |
| [O-1] | obs | The GC response transiently claims `source_version: V0` before `attach_archived_record_projection` overwrites it. Correct today, implicit. |

Nothing above blocks the cure landing. [P2-1] is a **guard** gap, not a
correctness gap.

---

# 1. THE DEFECT AND THE CURE, DRIVEN

All transcripts below are from probe modules compiled into materialized copies
of `7f28907` and `98f5f90`. `p_completed_v1` runs a real `--no-ff` merge to
completion through ordinary dispatch; `p_aborted_v1` runs a `--no-ff` start on
the mixed fixture, aborts it, and injects one canonical backup ref the way the
shipped v0 rows inject theirs.

## 1.1 The live refusal, cited

`decode_production_v0` (`record_wire/decode.rs:163-185`) classifies with
`InstalledMergeRecordVersions::V0_ONLY` (`header.rs:34-38`, `{v0: true, v1:
false}`). `classify_merge_record_header` finds the `gwz.merge-operation/v1`
allocation (`header.rs:79-84`, `dispatch: Some(V1)`, `required_wave: A1`) and
falls to:

```
159:    match allocation.dispatch {
160:        Some(MergeRecordDispatch::V0) if installed.v0 => Ok(MergeRecordDispatch::V0),
161:        Some(MergeRecordDispatch::V1) if installed.v1 => Ok(MergeRecordDispatch::V1),
162:        _ => Err(unsupported(header, allocation.required_wave)),
163:    }
```

`header.rs:159-163` — the `_` arm. This is reached through
`.map_err(RecordDecodeError::Header)?` at `decode.rs:168-169`, i.e. **before**
the `MergeRecordDispatch::V1` mirror arm at `decode.rs:177-182`. The builder's
doc comment on `decode_archived_common` says exactly this ("the live refusal,
not the unreachable mirror arm after it") and it is accurate.

## 1.2 Site 1 — `merge/gc.rs:188`, `handle_gc`'s retention preflight

Driven end to end through the live dispatch (`runtime/dispatch.rs:361 →
gc::handle_gc`).

```
BASE 7f28907
  PROBE completed-archive schema=Some("gwz.merge-operation/v1") publication.candidate=true accepted_workspace=true state=Some("completed")
  PROBE site1 COMPLETED => Err(code=ArchivedRecordUnreadable, detail=archived merge record 'merge_op_probe-gc-done_start_0001' is unreadable)
  PROBE site1 COMPLETED archive_present=true
  PROBE site1 ABORTED  => Err(code=ArchivedRecordUnreadable, detail=archived merge record 'merge_op_probe-gc-abort_start_0001' is unreadable)
  PROBE site1 ABORTED  archive_present=true backup_ref_present=true

(c) 98f5f90
  PROBE site1 COMPLETED => Ok(state=Completed, source_version=Some(V1), terminal=Some(Completed), acceptance=Some(SupportedPersisted))
  PROBE site1 COMPLETED archive_present=false
  PROBE site1 ABORTED  => Ok(state=Aborted, source_version=Some(V1), terminal=Some(Aborted), acceptance=Some(NotAccepted))
  PROBE site1 ABORTED  archive_present=false backup_ref_present=false
```

The order's own case — a COMPLETED `--no-ff` merge — is un-GC-able at base and
collected at (c), archive unlinked and the owned backup ref deleted under the
checked rule.

## 1.3 Site 2 — `store/gc.rs:15` → `store/mod.rs` `read_record`

Isolated: `FileMergeStore.gc(root, Some(id))` called directly, so `handle_gc`'s
own decode is not standing in front of it.

```
BASE 7f28907
  PROBE site2 store.gc(Some(id)) => Err(code=UnsupportedRecordVersion, detail=merge record '…' uses schema 'gwz.merge-operation/v1' version 1, which requires A1 (v1 integration/acceptance/no-ff); use a compatible newer GWZ)
  PROBE site2 archive_present=true

(c) 98f5f90
  PROBE site2 store.gc(Some(id)) => Ok
  PROBE site2 archive_present=false
```

This is an **independent** defect: curing site 1 alone would still have left
the archive undeletable at the store's last read before unlink. Both sites had
to move, and both did.

## 1.4 Site 3 — `store/retention.rs:8`, the id-less `gwz merge --gc` sweep

This is the site the builder describes as "retained forever". It needed care to
drive honestly, because `validated_future_cleanup` is `#[cfg(not(test))] ->
None` in production but `#[cfg(test)]` routes through `decode_archived`, which
**masks** the defect under `cargo test`. Both configurations were run:

* **cfg A** = the tree as shipped (`#[cfg(test)]` arm live).
* **cfg B** = `validated_future_cleanup` forced to `None`, i.e. production
  semantics, on both trees.

Fixture: ONE **real** completed `--no-ff` v1 archive (written first, oldest)
plus twenty v0-envelope fillers derived from the same bytes.
`ORDINARY_RETENTION` is 20 (`store/mod.rs:32`), so a sweep that *classifies*
the v1 archive has 21 ordinary rows and deletes exactly the oldest — the v1
one; a sweep that cannot classify it `continue`s past it, leaves 20 ordinary
rows, and deletes nothing.

```
BASE 7f28907, cfg B (PRODUCTION semantics)
  PROBE site3-real before=21 after=21 v1_archive_survives=true outcome=Ok
  PROBE site3-real VERDICT: the v1 archive was NOT classified — retained past the ordinary cap

(c) 98f5f90, cfg B (PRODUCTION semantics)
  PROBE site3-real before=21 after=20 v1_archive_survives=false outcome=Ok
  PROBE site3-real VERDICT: the v1 archive WAS classified as ordinary and collected by the cap
```

**The builder's "retained forever" claim is exactly right**, and the cure is
real at this site.

The masking is itself measured, and matters for [P2-1]:

```
BASE 7f28907, cfg A (as shipped)
  PROBE site3-real before=21 after=20 v1_archive_survives=false
  PROBE site3-real VERDICT: the v1 archive WAS classified as ordinary and collected by the cap
```

Under `cargo test`, base and (c) are **indistinguishable** at this site for the
ordinary shape. No test written against a plain v1 archive can guard this cure.

---

# 2. THE SHARED DECODER, ADVERSARIALLY

## 2.1 The 16-field projection, field by field

`MergeOperationRecord` is `v0::MergeOperationRecordV0`
(`model/mod.rs:14` re-export). Its full field list is `model/v0.rs:35-57` —
**exactly sixteen** fields. `MergeOperationRecordV1` (`model/v1/record.rs:19-50`)
has twenty-one: the same sixteen names in the same order, plus the five v1-only
fields between `operation_drift` and `extensions`.

| # | v0 field | type | v1 source | verdict |
| --- | --- | --- | --- | --- |
| 1 | `schema` | `String` | `record.schema` | verbatim |
| 2 | `record_schema_version` | `u32` | `record.record_schema_version` | verbatim |
| 3 | `writer_version` | `String` | `record.writer_version` | verbatim |
| 4 | `workspace_id` | `String` | `record.workspace_id` | verbatim |
| 5 | `merge_id` | `String` | `record.merge_id` | verbatim |
| 6 | `operation_id` | `String` | `record.operation_id` | verbatim |
| 7 | `state` | `OperationState` | `record.state` | verbatim |
| 8 | `source_ref` | `String` | `record.source_ref` | verbatim |
| 9 | `mode` | `MergeExecutionMode` | `record.mode` | verbatim |
| 10 | `created_at` | `String` | `record.created_at` | verbatim |
| 11 | `baseline` | `MergeBaseline` | `record.baseline` | verbatim |
| 12 | `selected_targets` | `Vec<String>` | `record.selected_targets` | verbatim |
| 13 | `participants` | `BTreeMap<String, MergeParticipantRecord>` | `record.participants` | verbatim |
| 14 | `publication` | `Option<PublicationProgress>` | `record.publication` | verbatim |
| 15 | `operation_drift` | `Vec<OperationDrift>` | `record.operation_drift` | verbatim |
| 16 | `extensions` | `BTreeMap<String, Value>` | `record.extensions` | verbatim |

The nested types are not merely *shaped* the same — they are **the same types**.
`model/v1/record.rs:6-9` imports `MergeBaseline`, `MergeExecutionMode`,
`MergeParticipantRecord`, `OperationDrift`, `OperationState` and
`PublicationProgress` from the shared model root, which re-exports them out of
`v0.rs`. There is no conversion anywhere in the projection, and no `move`
semantics loss: every field is moved, none cloned, none defaulted.

**Nothing dropped, nothing defaulted, nothing mis-bound.** The five v1-only
fields (`accepted_workspace`, `recovery_context`, `pending_rollback`,
`pending_preservation`, `preservation_publication_handoff`) are correctly *not*
projected: they are the archive-PROJECTION's inputs, not retention's, and
`from_v1` proves it (§2.3).

**The compile-error property holds.** The v0 side is an exhaustive struct
literal with no `..base` and no `Default`, so a new v0 field is a compile error
until it is mapped; the v1 side is sixteen named field accesses, so a removed,
renamed or retyped v1 common field is a compile error. The one residual: a
field added to v1 *only* is silently excluded — which is correct by definition,
since "v1-only" is what excludes it.

## 2.2 `extensions` and the exact-reread doctrine

v1's `extensions` is `#[serde(default, flatten)]` and the five v1-only fields
are **declared**, so they are never swept into `extensions` during the v1
decode. The projected `extensions` therefore carries only genuinely-unknown
root keys — the correct common half.

Nothing on this path re-serializes the projected record. `decode_archived_common`
also returns the *full* `raw` document `Value` (every v1-only field included),
and `read_seam_record` returns `(raw, record)`; all three archived callers
discard `raw` and none writes. `store::gc::collect` discards the whole tuple,
`retention::enforce` binds `Ok((_, record))`, `handle_gc` binds `(_, _, record)`.
**No archived read on this path can lose or rewrite a byte** — the only durable
actions downstream are `fs::remove_file` and `sync_dir`.

Separately, `decode_v1_body` still runs `UnknownFieldManifest::extract_v1(&raw)`
(`decode.rs:242-247`) inside the shared decoder, so the unknown-field discipline
is not bypassed by the new entry point; the manifest is simply dropped by the
projection, which reads no unknown field.

## 2.3 `preflight_backup_artifacts` and `cleanup::from_v1`

`preflight_backup_artifacts` (`merge/gc.rs:206-255`) reads exactly:

* `record.state.is_open()` — `OperationState`, shared type;
* `record.merge_id` — `String`;
* `record.participants` → `path`, `target_kind`, `preservation` —
  `MergeParticipantRecord`, shared type;
* `record.publication.root_preservation` — `Vec<PreservationEvidence>` through
  `PublicationProgress`, shared type.

All four are common-half fields carried verbatim at identical types. There is
no v1→v0 conversion anywhere in this call.

The load-bearing check is whether the **common half is sufficient** to enumerate
what a v1 archive owns. `cleanup::from_v1` (`record_wire/archive/cleanup.rs:128-169`)
— the archive projection's own worklist derivation, the authority on v1 ref
ownership — iterates **exactly the same two containers**: `record.participants[]
.preservation` and `record.publication.root_preservation`. **No v1-only field
contributes a backup ref.** So three independent enumerations agree:

| consumer | predicate | source containers |
| --- | --- | --- |
| `preflight_backup_artifacts` | full evidence audit + `observe_direct_ref` | participants + publication |
| `cleanup::from_v1` | canonical-name/OID/duplicate audit | participants + publication |
| `retention::enforce` | `.any(row.backup_ref.is_some())` | participants + publication |

Retention's predicate is the weakest of the three and is fail-**safe** in the
direction that matters: it is true whenever any row names a ref, so every case
where `from_v1` would produce an owner *or* error out on a malformed name is
retained. `from_v1` ⊆ retention.

## 2.4 Could a v1 record's common half claim to be v0 downstream?

**No.** `schema` and `record_schema_version` are carried **verbatim**, so a
projected v1 record still says `gwz.merge-operation/v1` / `1`. That is what
makes `validate_record(&record, Some(path), true)` re-classify it as `V1`
against `PRODUCTION` and pass — and what makes it refuse under `V0_ONLY`. The
envelope is never laundered.

At the response surface, `handle_gc` does build a response through
`post_gc_record(record).to_response(context)`, whose macro sets `record:
Some(project_open_v0(self))` → `source_version: V0, archived: false`. That value
is then **unconditionally overwritten** by
`attach_archived_record_projection` (`response.rs:98`,
`response.record = Some(project_archived(archived))`), which takes
`source_version` from the v1 archive projection. Driven:

```
(c)  PROBE projection response.record.source_version=V1 archived=true terminal=Some(Completed) acceptance=Some(SupportedPersisted)
```

There is a genuine consistency gate here worth naming: `attach_archived_record_projection`
(`response.rs:89-97`) refuses with `InternalError` unless the response's
`state` — which comes from the **common half** — matches the terminal outcome
the **v1 projection** derived. A common-half/v1-half disagreement fails the
whole GC rather than shipping a mixed answer. That is a good property and it is
not stated in the commit message.

Filed as **[O-1]**: the correctness of "no v0 claim escapes" rests on that
overwrite being unconditional. It is, today. A comment at `merge/gc.rs:192-196`
noting that the `project_open_v0` value is a placeholder the next call replaces
would make the invariant legible; nothing is wrong as written.

---

# 3. THE `read_record` SPLIT

`read_record(path, location)` is now a thin wrapper —
`read_seam_record(path, location, false)` (`store/mod.rs:380-385`) — and the
`false` branch is character-for-character the base body:
`decode_production_v0(&bytes).map_err(…)?.into_production_parts()` followed by
`validate_record(&record, Some(path), …)`.

**Base had eleven `read_record` call sites; two were converted; NINE remain**
(the handoff says "ten" — see [P3-5]). Every one is v0-only and byte-identical
in behaviour:

| # | site | location | reachable from `--gc`? |
| --- | --- | --- | --- |
| 1 | `store/archived.rs:14` (`archived::load`) | `Archived` | no (tests only) |
| 2 | `store/archived.rs:29` (`archive`, destination-only) | `Archived` | no |
| 3 | `store/archived.rs:37` (`archive`, source read) | `Open` | no |
| 4 | `store/archived.rs:41` (`archive`, destination compare) | `Archived` | no |
| 5 | `store/archived.rs:53` (`archive`, post-rename verify) | `Archived` | no |
| 6 | `store/mod.rs:81` (`discover_open`) | `Open` | **yes** — see below |
| 7 | `store/mod.rs:103` (`load`) | either | no |
| 8 | `store/mod.rs:136` (`write_open`, unknown-carry) | `Open` | no |
| 9 | `store/mod.rs:142` (`write_open`, verify) | `Open` | no |

`validate_record`'s v1 arm refuses **typed, not by panic**. Old:

```rust
Ok(MergeRecordDispatch::V1) => unreachable!("the v0-only installed set never dispatches v1")
```

New (`store/mod.rs:355-361`):

```rust
Ok(super::record_wire::MergeRecordDispatch::V1) if both_installed_envelopes => {}
Ok(super::record_wire::MergeRecordDispatch::V1) | Err(_) => {
    return Err(unreadable(path, "unsupported merge record schema"));
}
```

With `both_installed_envelopes == false` the installed set is `V0_ONLY`, so
`Ok(V1)` is unreachable exactly as before — but it is now a typed refusal
rather than a panic, which is what the panic audit required. The match is
exhaustive. `write_open`'s call (`store/mod.rs:117`) passes `false`: unchanged.

**No other v0-only classification remains on a GC-reachable read.** Trace:

* `dispatch_merge` → `classify_open_record` (`store/mod.rs:211-239`) uses
  `decode_production` — the **PRODUCTION** set, version-agnostic. An open v1
  record routes to `v1.command` before `handle_gc` is ever entered, where
  `MergeOp::Gc` refuses typed `OpenOperation`
  (`v1_lifecycle/start.rs:147-150`). So call site #6's v0-only read cannot meet
  a v1 record — it is guarded upstream by the envelope classifier.
* `resolve_recovery_root` prefers `discover_open_envelope_before_manifest`
  (envelope-only) and documents exactly this reason (`dispatch.rs:471-476`).
* `acquire_canonical_merge_locations` (`record_wire/location.rs:105-160`) reads
  raw bytes with no envelope classification at all.
* `decode_archived` (`record_wire/archive/mod.rs:49-64`) already installs
  `PRODUCTION`.
* The three converted sites install both.

---

# 4. CAPABILITY-FREE — VERIFIED, NOT REASONED

**By grep.** Zero hits for `CheckedArtifact`, `entry::`, `artifact_facts` or
`durable_identity` in all four named files:

```
0  src/workspace_ops/merge/gc.rs
0  src/workspace_ops/merge/store/gc.rs
0  src/workspace_ops/merge/store/retention.rs
0  src/workspace_ops/merge/record_wire/decode.rs
```

**By chain.** Every transitive callee of `--gc`, swept for the same tokens:

| chain member | hits | disposition |
| --- | --- | --- |
| `runtime/dispatch.rs` | 1 | `dispatch.rs:453`, `entry::activate_workspace_catalog`, inside `forward_lifecycle_viability_window` — called only from `adapt_before_mutating`, which returns immediately unless `op ∈ {Resume, Abort}`. **Unreachable from `Gc`.** |
| `store/mod.rs` | 1 | `store/mod.rs:131`, `entry::prepare_merge_store_parents`, inside `write_open` only. GC never writes a record. **Unreachable from `Gc`.** |
| `merge/gc.rs`, `store/gc.rs`, `store/retention.rs`, `record_wire/decode.rs`, `record_wire/location.rs`, `record_wire/archive/mod.rs`, `response.rs` | 0 | — |
| `operation/workspace_mutator_lock.rs` | 3 | the lock itself — see below |

`handle_gc` holds exactly one guard, `WorkspaceMutatorLock::acquire(root)` at
`merge/gc.rs:167`, and nothing else. That lock resolves to
`checked_artifact::try_acquire_workspace_runtime` →
`RuntimeBootstrap::try_acquire` (`checked_artifact/bootstrap/runtime/mod.rs:106-172`),
whose whole body is `retain_ambient_directory`, `revalidate_workspace_repository`,
`open_or_create_file`, `try_advisory_lock`, `ensure_child_directory` and
`revalidate_runtime_tree`. It **never** constructs a `CheckedArtifactObservation`.
A `CatalogMutationLeaseV1` is *borrowable* from the returned lease but is not
taken on this path.

**`observation.rs:96` and `:105`** are the two `durable_identity` calls inside
`CheckedArtifactObservation::acquire` (`checked_artifact/observation.rs:82-120`).
The only caller anywhere in the tree is `CheckedArchiveObservation::acquire` at
`v1_lifecycle/authority/observe/archive.rs:156`, which sits under the v1
lifecycle's archive family — reached from `archive_terminal` on a Resume/Abort
terminal disposition, and from the dead `gc_archived` family. **No `--gc`
invocation reaches either.** Confirmed: the entire `--gc` route is
`dispatch → gc::handle_gc → {lock, discover_open, acquire_canonical_merge_locations,
decode_archived_common, preflight_backup_artifacts, decode_archived, response,
delete_backup_ref_checked, store.gc}`, and `store.gc → gc::collect →
{read_seam_record, fs::remove_file, sync_dir, retention::enforce}` — raw durable
primitives only, which is precisely the ruling's carve-out.

**The dead family is untouched.** `git diff 7f28907..98f5f90 --stat --
src/workspace_ops/merge/v1_lifecycle/` returns exactly one file,
`tests/no_ff_wire.rs`, +3 lines. `gc_archived` / `gc_archived_with_hook` /
`remove_archive` behind `v1_lifecycle/archive.rs:108-111` are byte-identical and
still dead. **The fix is not O8's route**, as claimed.

The ruling is satisfied as written.

---

# 5. CONTRACT SAFETY (§7)

`GwzM5-8I2CompatibilityContract.md` §7:243-250 —

> Archive decoding/status remains live-independent. Deletion has a separate
> ownership gate. An archive with no backup refs may be deleted after
> validation. […] Ordinary retention remains archive-only for classification
> and exempts records owning backup refs. Unsupported or corrupt archives are
> never deleted or rewritten.

## 5.1 Unsupported envelopes still refuse

`v2`/`v3`/`v4` carry `dispatch: None` (`header.rs:85-100`), so
`classify_merge_record_header` errors for them under **every** installed set,
`PRODUCTION` included. Widening from `V0_ONLY` to `PRODUCTION` moves exactly
one envelope — v1 — and nothing else. A v1 archive written by a *newer* wave
still classifies unsupported, because a newer wave means a different schema
string.

## 5.2 The corrupt-archive row, and the shapes it does not cover

(b)'s second row (`explicit_gc_refuses_an_unreadable_v1_archive_and_retains_every_ref`)
strips `participants` from a v1 archive; `participants` has no `#[serde(default)]`,
so the v1 body decode fails, the by-id route returns `ArchivedRecordUnreadable`,
and both the archive and its backup ref are retained. That row is real and it
passes.

I drove two further corrupt shapes the row does not reach, plus a **v0 control**:

```
(c) — v1 COMPLETED archive with `accepted_workspace` stripped
  PROBE c7-v1 by_id=Err(ArchivedRecordUnreadable) archive_after_by_id=true
  PROBE c7-v1 idless sweep: before=21 after=21 corrupt_v1_archive_survives=true

(c) — v1 archive whose publication-candidate marker bytes contradict the record
  PROBE c7b by_id=Err(ArchivedRecordUnreadable) archive_after_by_id=true
  PROBE c7b idless sweep: before=21 after=21 projection_refused_v1_archive_survives=true

(c) — v0 CONTROL: a v0 archive whose candidate marker the projection refuses
  PROBE c7-v0 by_id=Err(ArchivedRecordUnreadable) archive_after_by_id=true
  PROBE c7-v0 idless sweep: before=21 after=20 corrupt_v0_archive_survives=false
```

**The answer to the mandate's question is: no newly-decodable shape becomes
deletable that the contract meant to protect — and the v1 path is strictly
stricter than the v0 path it mirrors.**

The mechanism is `validate_v1_record`, which the shared decoder runs
(`decode.rs:237-241`) exactly as `decode_archived` does:

* a `Completed` v1 record with no `accepted_workspace` is refused by
  `validate_v1_record` itself, so it never reaches the retention predicate;
* a corrupted candidate marker is caught by `candidate_hashes_are_exact`
  (`model/v1/validate/publication.rs:212-231`), which digests
  `candidate.marker_yaml` and compares it against the record's own
  `candidate_hashes` — so the shared decoder refuses this shape too, and does
  so **independently of the `#[cfg(test)]` helper**, in both configurations.

The v0 control is the honest counterweight: ordinary retention *does* delete a
projection-refused **v0** archive today, because `retention::enforce` classifies
with the weak `.any(backup_ref.is_some())` predicate and never runs
`decode_archived` on a record its reader accepted. That is shipped v0
behaviour, unchanged by this commit and not introduced by it. The fix does not
extend it to v1; if anything it under-extends it, because the v1 reader carries
semantic validation the v0 reader has never had.

## 5.3 Frozen registry and the O8 corpus

```
$ python3.13 scripts/checks/check_merge_compatibility_predicates.py \
      dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .
validated 7 migration rules, 7 runtime bindings, and 10 archive shapes
EXIT=0
```

7 / 7 / 10 — unmoved. `git diff --stat 7f28907..98f5f90 -- '*archive_corpus*'`
is empty; `dev-docs/GwzM5-8I2CompatibilityPredicates.json` and
`src/workspace_ops/tests/g23/archive_equivalence_v0.rs` are untouched.

---

# 6. THE FALSE ALARM

The builder's earlier flag — *"completed archives still refuse via
`archive/v1.rs:11-19`"* — was self-corrected in (c) as a FALSE ALARM taken from
a record dump truncated before the field and never re-tested after the second
read site was cured. **The correction is right.** On real bytes from a real
completed `--no-ff` merge, at BOTH revisions:

```
PROBE completed-archive schema=Some("gwz.merge-operation/v1")
                        publication.candidate=true
                        accepted_workspace=true
                        state=Some("completed")
```

`accepted_workspace` **is** persisted. `archive/v1.rs:11-14` therefore takes its
`Some(accepted) => SupportedPersisted` arm, and the archive projects cleanly —
confirmed at the response surface (`acceptance=Some(SupportedPersisted)`,
§1.2). The `None => return Err(())` arm at `archive/v1.rs:18` is live, and I
drove its counterfactual to prove the field is load-bearing rather than merely
absent from the audit:

```
(c)  PROBE false-alarm counterfactual: accepted_workspace_removed=true
     outcome=Err(ArchivedRecordUnreadable: …) archive_present=true
```

So the original inference named a real refusal arm and was wrong only about
whether the field reaches it. **No projection change was needed and none
landed**: `git diff 7f28907..98f5f90` touches six production files, none of
them under `record_wire/archive/` or `model/`; (c) itself is 86 lines of test
and gate script with **zero production lines**. The discarded "delegation of the
v1 arm to the legacy acceptance projection" left no residue in the tree.

**Is anything in the delivered tree still resting on the truncated-dump
inference?** No. I re-derived every claim that inference touched from real
archive bytes, and the row `explicit_gc_collects_a_completed_no_ff_archive`
pins the on-disk shape (v1 schema, `publication.candidate` present,
`accepted_workspace` present) so no future reader has to re-derive it. The
process lesson is correctly recorded in (c)'s message.

---

# 7. COMPANIONS, RECOMPUTED

## 7.1 The protected tree digests

Recomputed independently with the checker's own algorithm re-implemented from
`check_checked_artifact_boundaries.py:1025-1041` (mod.rs → whole parent dir,
otherwise the stem dir; `sha256` over `len(rel)‖rel‖len(bytes)‖bytes` in
`relative_to(source).as_posix()` order). All **seven** entries recomputed on the
(c) tree; all seven match the pinned values:

```
checked_artifact/bootstrap/runtime/catalog_lease.rs: 91ac3dfada76860dda1d41a0c3cad66f6836229680773b1b1644e4aabe20b0b2
checked_artifact/capability/path.rs:                 23e46dbde50a0530c331c34dd68a9d40096394c6817075d3f66ad3f0e27a91c6
checked_artifact/capability/pre_catalog.rs:          4b7cef22b64d668f9864ca734a71942d6d2a1e892fc5833259cc6c4e11800eaf
checked_artifact/catalog.rs:                         5c76c6a4d87444684f7e44ab67be496847196491be30b17e3507dcd0f1765329
checked_artifact/platform.rs:                        c464666735aae2028fa75f9d6063eb6122f95ea1e3f0a39b3e4f18cd9293d094
workspace_ops/merge/v1_lifecycle/authority/observe.rs: d16fa8bf67f8656c56b3c51d6625712efcc970dfd51afefa77557df5b3fcae38
workspace_ops/merge/v1_lifecycle/mod.rs:             8ae37597982a723d6627c0398f4adec754d815441ff6a070c700d4dc856d6104
```

The one re-pinned entry, `v1_lifecycle/mod.rs`, is a **tree** digest covering
the whole `v1_lifecycle/` directory, which is why a +3-line edit to
`v1_lifecycle/tests/no_ff_wire.rs` moves it. Mandatory companion; correct value.
Confirmed live: `check_checked_artifact_boundaries.py` → `checked-artifact
boundary: ok (18 visible entries, 8 classified modules)`, exit 0.

## 7.2 The NoFf-mention surface pin

`no_ff_mode_mentions_stay_inside_the_pinned_surface`
(`v1_lifecycle/tests/no_ff_wire.rs:189`) asserts the **exact sorted list** of
files whose source mentions the `NoFf` variant token (the variant is spelled at
runtime so the suite is not its own hit). Adding `crate::MergeMode::NoFf` to
`g23/gc.rs` makes it a hit, so the file must join the list or the pin fails
closed. Mandatory companion, correctly placed in sorted position, with a dated
JOINED comment matching the file's existing convention. See [P3-2] on the
comment's wording.

## 7.3 The g23 provenance erratum

**Verified against history, not taken on trust.** R2-E E4.1 commit (c) is
`6688f34`. Its numstat shows `src/workspace_ops/tests/g23/a1_activation.rs`
`148 / 0`, adding exactly two `#[test]` functions
(`an_interrupted_ordinary_merge_completes_under_v0_when_the_catalog_is_unavailable`
and `a_v1_resume_refuses_without_mutation_and_abort_still_clears_the_record`).
Its own gate-script block reads:

> R2-E E4.1 commit (c) […] moves the LIB REMAINDER **and only it**, by two rows
> in `workspace_ops::tests::g23::a1_activation` […]

`g23::a1_activation` is under `workspace_ops::tests::g23::`, so the g23 marker
should have moved by +2 in the same commit and did not. Measured directly from
a `--list`-verified base binary: **base `7f28907` g23 = 126**, while the pin
read `124`. Two short, exactly as the erratum states.

Arithmetic: 124 (stale) + 2 (E4.1(c)'s uncounted rows) + 2 ((b)'s rows) + 1
((c)'s row) = **129**, and 129 is what I measure at (c). Correct.

**Form:** the erratum is a NEW dated note in the g23 battery's own comment
block. `git diff 7f28907..98f5f90 -- scripts/checks/run_r4bg_aggregate_gates.py`
shows the E4.1 docstring block is **not touched** — the two additions to
`_fault_count`'s docstring are appended *after* the E4.2 block, and the only
rewritten lines are (b)'s own comment, edited by (c) within the same lane. The
dated record of E4.1 is intact. This is the right shape.

## 7.4 Counts, from a `--list`-verified snapshot of my own build

Snapshot: `…/scratchpad/snap-c/lib-c`, sha256
`598c7acb471cad34eb3f077e9fe152e0b386c801b275e628f2f14d936091c171`, copied out
of my own target dir so no other worktree's binary can be run by accident.
`--list` reports **1832 tests, 0 benchmarks**, of which 129 under
`workspace_ops::tests::g23::`, 457 under `checked_artifact::` and 261 under
`workspace_ops::merge::v1_lifecycle::` (the battery skips `root_fault_matrix`,
hence 260).

| battery | filter | pinned | MEASURED |
| --- | --- | --- | --- |
| lib remainder | `--skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` | 1113 passed (darwin) | **1113 passed; 0 failed; 1 ignored** ✓ |
| checked-artifact census | `checked_artifact::` | 457 passed | **457 passed; 0 failed** ✓ |
| v1 lifecycle fault/restart | `workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix` | 260 passed | **260 passed; 0 failed** ✓ |
| g23 marker | `workspace_ops::tests::g23::` | 129 passed | **129 passed; 0 failed** ✓ |
| unknown-field | `workspace_ops::merge::record_wire::` | 75 passed | **75 passed; 0 failed** ✓ |

Base inventory for the derivation, measured the same way on a materialized
`7f28907`: **1829 tests**, g23 **126**, `checked_artifact::` **457**,
`v1_lifecycle::` **261**. Remainder 1829 − 457 − 261 = 1111 = 1110 passed + 1
ignored, which is the base pin. At (c): 1832 − 457 − 261 = 1114 = 1113 + 1
ignored. **The +3 delta is entirely in g23 and entirely this lane's**, and
`checked_artifact::` / `v1_lifecycle::` are independently confirmed UNMOVED.
(The linux numbers stay DERIVED / FIRST-DISPATCH-EXPECTED, correctly labelled.
`v1_lifecycle::` is 262 on `main` after E4.3-B landed today; this branch
predates it and will rebase.)

`root_fault_matrix` in the release profile (pinned "1 passed") was **not**
re-measured — it needs a release build and the diff cannot reach it.

## 7.5 Gates

| gate | result |
| --- | --- |
| `cargo fmt --check` | exit 0, no output |
| `cargo check --all-targets` | `Finished dev profile` — no warnings, exit 0 |
| `cargo clippy --all-targets -- -D warnings` | `Finished dev profile` — exit 0 |
| `check_checked_artifact_boundaries.py` | `checked-artifact boundary: ok (18 visible entries, 8 classified modules)`, exit 0 |
| `check_merge_compatibility_predicates.py` | `validated 7 migration rules, 7 runtime bindings, and 10 archive shapes`, exit 0 |
| `check_merge_docs.py` | `merge document consistency: ok (12 sources, 155 assertions)`, exit 0 |
| `test_merge_compatibility_predicates.py` | `Ran 27 tests … OK` |

Three compile-probe suites — `test_check_checked_artifact_boundaries.py`,
`test_release_boundary.py`, `test_v1_lifecycle_privacy_probe.py` — each drive
several full `cargo clippy --all-targets --all-features` builds of a copied
crate and did not complete inside this review's window on a heavily contended
host. They are **not decision-bearing here**: the diff's only checker change is
a single digest string literal, which I recomputed independently (§7.1), and
`check_checked_artifact_boundaries.py` itself — the gate those suites test —
passes. They should be run at the landing dispatch as usual.

**Environmental, identical at base, NOT findings:**

* `test_check_merge_docs.py` — 1 failure, 12 `source_missing` findings, all
  pointing at `…/scratchpad/gwz-cli/docs/CLI.md` etc. The standalone worktree
  has no sibling `gwz-cli/`. This is precisely the caveat the order recorded.
  (Note the checker *binary* itself passes; only the suite's own root
  resolution fails.)
* `check_m4_scenario_map.py` — `M4 scenario map: unreadable ([Errno 2] … no
  such file … dev-docs/GwzM5-8R4bG-Evidence.md)`. Same class: the map lives in
  the gwz-dev workspace's dev-docs, outside this checkout — the script's own
  J-7 scope disclosure says so. The diff touches neither the map nor the
  scenario rows.
* `tests/protocol.rs` — `30 passed; 3 failed` at (c):
  `generated_protocol_is_current`, `generated_checked_artifact_protocol_is_current`,
  `log_addition_preserves_the_complete_pre_log_wire_projection`.
  **Re-run on a materialized `7f28907`: `30 passed; 3 failed`, the same three.**
  The diff touches no file under `src/protocol/`, `src/checked_artifact/protocol/`
  or `protocol/`, and neither `Cargo.toml` nor `Cargo.lock`. Pre-existing.

---

# 8. CAPS, RIDER AUDIT, TRAILERS

## 8.1 Caps

`git show --numstat` totals: **(b) 222 insertions / 22 deletions**, **(c) 86
insertions / 6 deletions**. Both within their caps (222-MEASURED and 150).

## 8.2 Rider audit — (b), line by line

| lines | file | classification |
| --- | --- | --- |
| 49 / 0 | `record_wire/decode.rs` | **CURE** — `decode_archived_common` + its doc comment. |
| 34 / 13 | `store/mod.rs` | **CURE** — the `read_record` → `read_seam_record` split and `validate_record`'s installed-set parameter. |
| 2 / 3 | `merge/gc.rs` | **CURE** — site 1. |
| 4 / 1 | `store/gc.rs` | **CURE** — site 2 (+3-line comment). |
| 4 / 1 | `store/retention.rs` | **CURE** — site 3 (+3-line comment). |
| 2 / 1 | `record_wire/mod.rs` | **CURE** — the re-export. |
| 104 / 0 | `tests/g23/gc.rs` | **ROWS** — two rows plus the `v1_archive_owning_a_backup_ref` / `archived_path` helpers. Mandatory: a cure with no row is unpinned. |
| 3 / 0 | `v1_lifecycle/tests/no_ff_wire.rs` | **COMPANION, forced** — the surface pin fails closed otherwise. |
| 1 / 1 | `check_checked_artifact_boundaries.py` | **COMPANION, forced** — the tree digest fails closed otherwise. |
| 19 / 2 | `run_r4bg_aggregate_gates.py` | **COMPANION, forced** — 2 count lines + a dated provenance block; the batteries fail closed otherwise. |

**No riders.** Every line is the three-site cure, a row for it, or a pin that
would otherwise fail closed. No opportunistic refactor, no unrelated comment
sweep, no doc drive-by.

## 8.3 Rider audit — (c)

69 lines of `tests/g23/gc.rs` (the completed-archive helper + row) and 17/6 of
gate script (counts + the dated block + the erratum note). **Zero production
lines** — verified by `git show --numstat`. No riders. (c) is not itself a rider
on (b): it discharges the order's *own named case*, which (b)'s aborted-archive
rows did not.

## 8.4 Trailer sweep

`git interpret-trailers --parse` returns **empty for both commits** — no
trailers of any kind. `git log 7f28907..98f5f90 --format=%B` grepped
case-insensitively for `co-authored`, `claude`, `anthropic`, `noreply`,
`generated with`: **no matches**. Clean.

---

# 9. FINDINGS

## [P2-1] — The `store/retention.rs` cure carries no row. ESCALATION.

**Driven by hunk ablation on the delivered tree.** Each hunk reverted in
isolation on a materialized (c), then the whole g23 battery run:

| ablation | g23 result | pinned? |
| --- | --- | --- |
| `merge/gc.rs` → `decode_production_v0` | **127 passed, 2 failed** (`explicit_gc_collects_a_completed_no_ff_archive`, `explicit_gc_collects_a_v1_archive_and_deletes_its_backup_ref`) | YES |
| `store/gc.rs` → `read_record(…, Archived)` | **127 passed, 2 failed** (the same two) | YES |
| `store/retention.rs` → `read_record(…, Archived)` | **129 passed, 0 failed** | **NO** |

The third site — the one the commit message describes as having fallen to its
"unknown archive: retain it" arm *forever* — can be reverted to the defective
read and the entire suite stays green. Nothing in the tree guards it.

**Why the obvious row does not work, and what would.** `validated_future_cleanup`
is `#[cfg(not(test))] -> None` but `#[cfg(test)]` routes through
`decode_archived`. So under `cargo test`, a plain v1 archive is classified on
*both* the `Ok` arm (fixed) and the `Err → validated_future_cleanup` arm
(defective) — measured in §1.4: base cfg A deletes the archive just as (c)
does. A guard therefore needs a shape that **`decode_archived_common` accepts
and `decode_archived` refuses**, with no `backup_ref`. The difference set is
narrow but non-empty — `decode_archived` additionally runs `v1::project` and
`cleanup::from_v1`; the two shapes I tried are both caught earlier by
`validate_v1_record` (§5.2), so the remaining candidates live in
`cleanup::from_v1`'s stash-identity and duplicate-owner arms
(`archive/cleanup.rs:207-232`), which `validate_v1_record` does not duplicate.

**Condition (lane owner's choice of remedy):**

1. land a discriminating row for the retention site as above; **or**
2. drive the site directly at the unit layer (`retention::enforce` over a
   planted `done/` tree, with the helper's test arm neutralised for that row);
   **or**
3. record a **dated disclosure** at `store/retention.rs:8-11` naming the
   `#[cfg(test)] validated_future_cleanup` mask as the reason this site cannot
   be pinned by the ordinary shape, so the next reader does not have to
   rediscover it.

Whichever is chosen, the commit message's three-site claim should say which
sites are pinned and which is not. The cure itself is correct and should land.

## [P3-1] — "before any ref is observed" is true of one refusal class only

(b)'s message: *"an unreadable, contradictory, unknown-envelope or v1-invalid
archive still refuses typed **before any ref is observed**, and both the archive
and the refs it owns are retained."*

`handle_gc`'s order is: `decode_archived_common` (188) → `preflight_backup_artifacts`
(190, which calls `observe_direct_ref` on every recorded ref) → `decode_archived`
(191). A refusal from the **archive projection** therefore happens *after* the
refs have been observed. The observation is read-only and the retention half of
the claim is unaffected — nothing is mutated until `delete_backup_ref_checked`
at 199 — but the sentence as written is broader than the code. Suggest "before
any ref is deleted", which is true of every class.

## [P3-2] — `no_ff_wire.rs`'s new comment over-claims the door

> `--no-ff` is the only production door to a v1 archive, so the GC rows must
> start one.

`ACTIVE_WRITER_FLOOR` is `V0` and only `RequestedSemantics::NoFf` selects V1 at
*start* (`model/version.rs:39`, `:71`) — but E4.1(c) established a second
production door: `adapt_before_mutating` durably upgrades an interrupted
ordinary merge at `Finalizing` to v1 via `upgrade_open_v0`, and that record
archives as v1. The justification for *this* pin entry stands (a fresh test can
only reach v1 through `--no-ff`); the absolute claim does not. Suggest "the only
door a fresh start has".

## [P3-3] — the headline under-counts the blast radius

(b)'s opening — *"Every merge archived under `gwz.merge-operation/v1` — every
`--no-ff` merge since 0.11.0 — was un-collectable"* — equates the two by an
em-dash. The leading clause is exactly right and general; the appositive is
narrower than the truth, for the [P3-2] reason. An adapted ordinary merge's
archive was equally un-collectable. Comment-only; it makes the fix *more*
valuable, not less.

## [P3-4] — undisclosed reach: the cure also changes ordinary retention

`retention::enforce` has three callers: `store/gc.rs:5` (the id-less `--gc`
sweep) **and `store/archived.rs:30` and `:59` — every v0-lifecycle archive
operation.** So the moment this ships, any workspace whose `done/` holds more
than `ORDINARY_RETENTION` (20) archives will start sweeping its v1 archives on
the next ordinary merge that archives, not only on an explicit `gwz merge --gc`.

This is policy-*consistent* — those archives own no backup refs, no ref or stash
is deleted, and it is the same cap that has always applied to v0 — and it is
arguably the whole point of curing the site. But it is a user-visible change in
what a plain `gwz merge` does to `.gwz/merge/done/`, and neither commit message
nor any comment says so. (The v1 lifecycle archives through its own writer and
does not call `retention::enforce`, so a v1 merge does not sweep on its own
completion; the trigger is a later v0-lifecycle archive or an explicit `--gc`.)

**Condition:** one release-note sentence, and a line in the commit message or at
`store/retention.rs`.

## [P3-5] — "the other ten `read_record` callers" is nine

Base `7f28907` has **eleven** `read_record` call sites under
`src/workspace_ops/merge/store/`; two were converted; **nine** remain
(enumerated in §3). The figure appears only in the handoff, not in the tree, so
there is nothing to fix in the commit — recorded so the count is not carried
forward.

## [O-1] — the transient `source_version: V0`

See §2.4. Correct today because `attach_archived_record_projection` overwrites
`response.record` unconditionally. A one-line comment at `merge/gc.rs:192` would
make the invariant legible. No action required.

---

# 10. FLAG DISPOSITION

The builder's numbered flag list (flags 1–6 of (b), 1–4 of (c)) was **not in the
review package** — peer-blind delivery was the two commits, the worktree, and
the ruling. I therefore dispositioned the substantive claims that *are* on the
record, i.e. the ones any future reader will meet in the commit messages, which
is where the flags' content lands.

| claim (source) | disposition |
| --- | --- |
| (b) "every merge archived under v1 … was un-collectable" | **UPHELD** for the leading clause; see [P3-3] for the appositive. |
| (b) "`V0_ONLY` classifies a v1 envelope Unsupported … the live refusal, not the unreachable mirror arm" | **UPHELD** — `header.rs:159-163`, reached at `decode.rs:168-169` before the mirror arm at `:177`. |
| (b) "the id-less retention sweep fell to its 'unknown archive: retain it' arm forever" | **UPHELD** — driven under production semantics, §1.4. |
| (b) "the half both envelopes hold identically, field for field and type for type" | **UPHELD** — §2.1, all sixteen, same types, no conversions. |
| (b) "every refusal is kept … both the archive and the refs it owns are retained" | **UPHELD** in substance, with [P3-1] on "before any ref is observed". Strengthened: `validate_v1_record` inside the shared decoder keeps two further corrupt shapes unreadable at all three sites (§5.2). |
| (b) "read-side and capability-free: no checked-boundary call, no durable-identity probe, no CheckedArtifact acquisition" | **UPHELD** — §4, by grep and by chain, including the `WorkspaceMutatorLock` trace. |
| (b) "the dead `gc_archived` family … is untouched and stays dead" | **UPHELD** — one file changed in `v1_lifecycle/`, and it is a test surface pin. |
| (c) "NO production change was needed, and this commit deliberately makes none" | **UPHELD** — `git show --numstat` on (c): two files, zero production lines. |
| (c) "a completed `--no-ff` merge DOES persist `accepted_workspace`" (the FALSE ALARM correction) | **UPHELD** — §6, on real bytes, at both revisions; and the counterfactual drives the refusal arm the original inference named. |
| (c) "a delegation … was built, measured, found unnecessary, and discarded rather than landed" | **UPHELD** — no residue: `record_wire/archive/` and `model/` are byte-identical to base. |
| (c) the g23 provenance erratum | **UPHELD and independently re-measured** — base g23 = 126 against a pin of 124; E4.1(c) = `6688f34` added the two rows while writing "and only it". §7.3. |

**The process lesson (the truncated dump).** Nothing in the delivered tree still
rests on that inference. Every claim it touched is now either pinned by the (c)
row or re-derived here from real archive bytes. The lesson is correctly recorded
in the commit message rather than in a doc that would drift from it. The one
place the same failure mode could recur is [P2-1]: a site whose behaviour no row
observes is a site whose next claim will again be an inference.

---

# 11. WHAT I RAN

Probe modules were compiled into materialized copies of each revision
(`git archive <rev> | tar -x`); the review worktree was never modified.

* `zzprobe.rs` — 7 rows: sites 1/2/3 at both revisions, the response-projection
  claim, the FALSE-ALARM counterfactual, and the real-archive retention fixture
  (1 real v1 archive + 20 v0 fillers against `ORDINARY_RETENTION = 20`).
* `zzprobe2.rs` / `zzprobe3.rs` — contract §7: a `Completed` v1 archive with
  `accepted_workspace` stripped, a v1 archive with contradicting candidate
  marker bytes, and the **v0 control** for both.
* cfg A / cfg B — `validated_future_cleanup` as shipped, and forced to the
  production `None` arm, on both revisions.
* Three single-hunk ablations on (c), each followed by the full g23 battery.
* Five pin partitions against a `--list`-verified snapshot binary
  (sha256 `598c7acb…`), plus the base inventory measured the same way.
* `tests/protocol.rs` re-run on a materialized `7f28907` to classify its three
  failures as pre-existing.

**Exit state:** review worktree byte-clean at `98f5f90`
(`git status --porcelain` empty). All reviewer target dirs and materialized
trees removed.
