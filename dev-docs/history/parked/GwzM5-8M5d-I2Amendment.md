# GwzM5-8 M5d — the I2 compatibility amendment, apply-ready

Date: 2026-09-03. Author: the implementation lane (Fable lane, S1 builder).
Status: **DRAFT — not applied.** This document is the *text* of the amendment
to `dev-docs/GwzM5-8I2CompatibilityContract.md`. It is applied at the **M5d
close** (`GwzM5-8M5d-Charter.md` §7, §9 step 4), inside the single reviewed
landing that also raises `ACTIVE_WRITER_FLOOR` to `V1`, ships the production
§2 refuse, deletes the v0 lifecycle engine, and lands the CapabilityFree / F-3
revisions. **Drafting this text does not amend I2.** Until the close commit,
`GwzM5-8I2CompatibilityContract.md` stands exactly as written and mutating v0
stays on today's I2 §5 lifecycle (charter §7). The companion S1 deliverable is
`GwzM5-8M5d-GateRevisions.md` (CapabilityFree §3 and F-3 / J-1).

**Controlling document:** `dev-docs/GwzM5-8M5d-Charter.md`, revision 5,
ACCEPTED by operator decision 2026-09-03. Its §10 is the amendment block this
document renders; §7 is the disposition table; §9 step (1) is this step. This
document **implements** the charter. It does not redesign it, and where it
finds the charter internally inconsistent it says so at §7 (OPEN QUESTIONS)
rather than resolving it.

**Charter SHA note.** The charter's own status header records the revision-5
acceptance hash `df6399662c2c93b3e94072f62cd61856e74fa0b7ef6f6699b685e2ae804e32ec`.
The live file at `dev-docs/GwzM5-8M5d-Charter.md` hashes to
`dcdbdba84b8de0ab0b9d290024942407ba20e6fcd4eb3a4406314326328aaa5b`. The two
differ **by construction**: a document cannot contain its own hash, so the
acceptance hash is of the revision-5 body as hashed *before* the acceptance
stamp naming it was written into the header. No content drift is implied and
none was found — every §10 clause quoted below is quoted from the live file.

## 1. Evidence base

| Object | Value |
|---|---|
| gwz-dev root HEAD | `8535553e0655627654c7b36fe14df21423a2020d` |
| gwz-core HEAD | `57502e461055809503ca9dc0d7cb791b8b332f6c` |
| Amendment target | `dev-docs/GwzM5-8I2CompatibilityContract.md`, 310 lines |
| Controlling charter | `dev-docs/GwzM5-8M5d-Charter.md`, 482 lines, revision 5 |

Every I2 quotation in §3 below was verified against the live file by `grep -n`
at this tuple. Line numbers are stated **as of 2026-09-03** and are a
convenience only: the binding anchor is the § number plus the quoted phrase,
per the citing rule filed in `CurrentProgramCheckpoint.md` (the 2026-08-27
citation-drift note, "cite content-anchored, not line-anchored").

**Note on `dev-docs/` paths.** A dev-docs reorganization is in flight in this
working tree at the drafting time: a large set of settled records is staged as
renames into `dev-docs/history/`. Citations carrying a `history/` segment
reflect that staged location; contents are byte-identical (pure renames).
`GwzM5-8I2CompatibilityContract.md`, `GwzM5-8M5d-Charter.md`,
`GwzM5-8I2ActionJournalContract.md`, `GwzM5-8I2RecordContract.md` and
`CurrentProgramCheckpoint.md` are **not** moved. `GwzM5-8A1ActivationRecord.md`
(checklist item 18) **is** — the close's dated note goes to its new home.

## 2. Citation audit of charter §10.1 and §10.2

All nine §10.1 rows and all five §10.2 bullets were checked against the live
contract. **No stale line cite was found.** Two precision defects were found,
both of the same kind — a *line* range given for a *sentence* boundary that
falls mid-line — and both are implementer traps, because at I2 `:181` and
`:183` superseded and retained text share a physical line.

| Charter cite | Verified | Finding |
|---|---|---|
| §1:36 | `:36` | exact |
| §2:102–103 | `:102–103` | exact (sentence spans the line break) |
| §2 annotation lines 83–100 | `:83–100` | exact |
| §4 entire | `:119–156` (heading `:119`, §5 heading `:158`) | exact |
| §5:160–179 | `:160–179` | exact |
| §5:186–204 | `:186–204` | exact |
| §5:180 | sentence spans `:180`–`:181` | **imprecise**: the superseded sentence ends mid-`:181`, and the rest of `:181` is §10.2-**retained** text |
| §5:183–184 | sentence spans mid-`:183`–`:184` | exact as a range, but `:183` opens with §10.2-**retained** text |
| §8:289 | `:289` | exact |
| §9:304–305 | `:304–305` | exact |
| §10.2 retained "§5:181–182" | text spans `:181`–`:183` | **imprecise**: the retained pair ends at "…decoder/projection below." on `:183`, not at `:182` |

**Ruling for the implementer, per the 2026-08-27 note's spirit:** correct the
cite, do not renumber the contract, and do not silently re-point the charter's
dated text. §3 below therefore quotes **sentences**, not lines, and every
replacement is expressed as a sentence-level substitution. Applying it
line-wise from the charter's ranges alone would delete two retained sentences.

The lines at issue, verbatim as they stand (`:178–:184`):

```text
178	Zero whitelist matches is not an error. Open read-only status leaves bytes
179	unchanged and projects v0 source/version with acceptance and recovery absent.
180	An existing mutating v0 command remains on the existing v0 lifecycle and may
181	write v0 only when that path's existing preflight authorizes it. Completed and
182	aborted v0 records remain v0 and use byte-preserving archival. Archived v0 uses
183	only the archive decoder/projection below. A v0 `mode: no_ff` open row remains
184	`UnsupportedLegacyMode` before resume/mutation.
```

Sentence dispositions in that block: `:178–179` superseded (§10.1 §5 row);
`:180`–mid-`:181` superseded (§10.1 §5:180 row); mid-`:181`–mid-`:183`
**retained verbatim** (§10.2); mid-`:183`–`:184` superseded (§10.1 §5:183–184
row).

## 3. The amendment, clause by clause

Each subsection gives: the live I2 text verbatim with its current line cite,
then the exact replacement in a fenced block. Fenced blocks are the text to
insert; they are already in I2's register (declarative present tense, no
first person, backticked identifiers).

### 3.1 Banner (charter §7)

Inserted immediately after the `Status:` line at `:5`, **above** the existing
`Amended 2026-08-10` banner, matching the corpus's newest-first banner order
(`GwzM5-8I2ActionJournalContract.md:7,:18,:23,:32,:39` and
`GwzM5-8I2RecordContract.md:7,:18,:39,:45,:51`).

```text
Amended 2026-09-03 by `GwzM5-8M5d-Charter.md` §10: the open-v0 lifecycle is
deleted rather than migrated. Post-M5d binaries compile open-record body
decoders and canonical lifecycle support only for v1; an open v0 envelope is
classified, never decoded, and every merge verb and mutating command answers
it with one sentence naming release `v0.13.0`. §4 is deleted as a production
path and §5 keeps only its archival retention sentences; the migration
whitelist, the adapter, and `AcceptedWorkspace` migration construction leave
production. §7's archive decoder and GC are retained and not amended. This
document is compatibility-authoritative only as amended by that charter.
```

**Date convention.** `2026-09-03` is the **charter's** date, not the close
date. The corpus convention is "date of the effecting amendment, not of the
edit", verified at `dev-docs/history/GwzM5-8I2Refreeze-ReviewConsistency.md:16`. Charter §7
writes the banner as `Amended <date> by GwzM5-8M5d-Charter.md` **"when the
close lands"**, which is ambiguous between "write it at the close" and "date it
at the close". Recommendation: the charter's date, per the convention.
Recorded as OPEN QUESTION Q1.

### 3.2 §1:36 — installed decoders

Live text, `GwzM5-8I2CompatibilityContract.md` §1 `:36`:

> A1 compiles body decoders and canonical lifecycle support only for v0 and v1.

Replacement (one sentence out, three in):

```text
Post-M5d binaries compile open-record body decoders and canonical lifecycle
support only for v1. The v0 archive decoder remains and is not that lifecycle
(§7). An open v0 record is classified from its envelope only; its body is never
decoded.
```

The v0 row of the `:28–34` allocation table is **unchanged**: envelope
classification still requires `gwz.merge-operation/v0` / `0` to be an allocated
exact pair. Charter §10 lists no change to that table and none is made.

### 3.3 §2:102–103 — open v0 records

Live text, §2, second sentence of the `:102–108` paragraph:

> Open v0 records may use only the explicit A1 migration path.

Replacement:

```text
An open v0 record is not a merge. It is classified from its envelope and
refused with the §2 sentence of `GwzM5-8M5d-Charter.md`; there is no migration
path, no whitelist, and no adapter.
```

### 3.4 §2:106–107 — the migration `writer_version` sentence (§10.2-carried)

**This deletion is carried by charter §10.2, not by any §10.1 row.** §10.2
retains "`writer_version` meaning **except** the 'V0-to-v1 migration therefore
sets…' sentence (that sentence dies with migration)."

Live text, §2 `:106–107`:

> V0-to-v1 migration therefore sets the A1 binary version.

Disposition: **deleted**, no replacement. The sentences around it
(`:104–106` "`writer_version` means the last binary that successfully wrote the
record. Every mutating migration or current-version rewrite sets it to the
current binary version in the same atomic record write."; `:107–108`
"Read-only status and archive projection leave it byte-exact…") are retained
verbatim by §10.2. The retained `:104–106` sentence still says "migration",
which post-close names nothing — see OPEN QUESTION Q2.

### 3.5 §2 dated annotation, `:83–100`

Charter §10.1 does not supersede the annotation's text; it **closes** it.
Append inside the same blockquote, after `:100` ("> `GwzM5-8A1ActivationRecord.md`."):

```text
> **[Closed 2026-09-03 by `GwzM5-8M5d-Charter.md` §7 / §10.1.]** The residual
> above is discharged by DELETION, not migration. The carrier is **M5d**, not
> M5c: M5d's close raises `ACTIVE_WRITER_FLOOR` to `V1` in one reviewed change
> together with the v0-lifecycle erasure and the open-v0 refuse. From that
> commit the annotation is historical; the creation-matrix row
> `A1 | ordinary/custom | v1` above is the shipped fact, and the
> release-time retained-reader manifest describes it.
```

The frozen creation-matrix table at `:71–77` is **unchanged** (charter §7 row
"Creation matrix A1 row"; §10.2 bullet 4).

### 3.6 §3 — the `UnsupportedLegacyMode` clause (§10.2-carried)

**Also carried by §10.2, not §10.1.** §10.2 retains "I2 §3 pinned codes,
**except** that open-v0 resume is no longer the `UnsupportedLegacyMode` no_ff
path".

Live text, §3 `:115–117`:

> A terminal archived v0 `mode: no_ff` is decoded read-only from actual durable
> result evidence; only open migration/resume uses `UnsupportedLegacyMode`.

Replacement (first clause retained verbatim, second clause replaced):

```text
A terminal archived v0 `mode: no_ff` is decoded read-only from actual durable
result evidence. No open-v0 path uses `UnsupportedLegacyMode`: an open v0
envelope is refused as an open operation with the §2 sentence, before any
resume or mutation. The code allocation itself is unchanged; the protocol
remains additive-only, and this charter allocates exactly one optional
response field, `MergeCrashRecovery.handles_ok`.
```

### 3.7 §4 entire, `:119–156` — "Open v0 compatibility result"

Superseded in full: structural-validity-then-whitelist-or-valid-unlisted
dispatch (`:121–126`), the core-owned equality registry (`:128–134`), A1's
seven whitelisted shapes (`:136–144`), durable-phase selection (`:146–150`),
and adapter construction of `AcceptedWorkspace` (`:152–156`). §10.2 retains
**nothing** from §4, so the section becomes empty.

**Recommendation: retain §4 as a numbered dated stub; do NOT remove the
section.** Replace `:121–156` with:

```text
> **[Deleted as a production path 2026-09-03 by `GwzM5-8M5d-Charter.md` §10.1
> / §10.4. Section number retained deliberately.]** Structural-validity-then-
> whitelist dispatch, valid-unlisted legacy-v0 handling, A1's seven migration
> whitelist shapes, durable-phase selection for migration, and adapter
> construction of `AcceptedWorkspace` are not production paths post-M5d. An
> open v0 record is the §2 refuse of that charter. `GwzM5-8I2Compatibility
> Predicates.json` is no longer a live matcher for open files; it is retained
> as a historical registry and its checker is re-pointed (charter §10.3).
> Nothing in this section is current authority. The archive decoder is §7 and
> is unaffected.
```

Why a stub and not removal:

1. **Section numbers are the corpus's stable anchor.** Live cites to I2 §5,
   §7, §8 and §9 exist in `GwzM5-8R4bG-Evidence.md`, `GwzM5-8M5bNoFfDesign.md`,
   `GwzM5-8R2DSettledTuple.md`, `history/GwzM5-8M5bImplSettled-Review.md`, and in the
   controlling charter's own §10. Removing §4 and renumbering would invalidate
   every one of them at once. The 2026-08-27 citing rule is built on § number
   plus anchor phrase; deleting a § number is the one edit that rule cannot
   absorb.
2. **The corpus already has this mechanism.** I2's own 2026-08-25 annotation
   and the sibling contracts' banners ("The frozen sentences below are **not
   rewritten**: each amended clause carries a dated §-local annotation")
   preserve structure and state the replacement beside it.
3. **A removed section is silent about why.** A reader arriving from a dated
   2026-08 citation needs to land on the disposition, not on §5.

The charter does not rule on this; §10.2's heading ("do not delete in a §4–§5
**rewrite**") presupposes a rewrite, not a section removal. The reviewers rule.

### 3.8 §5, `:158–204` — "Migration eligibility and atomic boundary"

§5 does **not** become empty. Four sentence groups are superseded and one pair
is retained verbatim.

**(a) `:160–179` — eligibility, adapter, zero-whitelist status projection.**
Live text runs from "An open v0 record is eligible only when all are true:"
(`:160`) through "…projects v0 source/version with acceptance and recovery
absent." (`:179`), covering the four-item eligibility list, the
`RecoveryRequired`/`Preserving`/`RollingBack` exclusion, the
representation-only paragraph, and the zero-match status projection.

**(b) `:180`–mid-`:181` — the mutating-command carve.** Live text:

> An existing mutating v0 command remains on the existing v0 lifecycle and may
> write v0 only when that path's existing preflight authorizes it.

**(c) mid-`:183`–`:184` — the `no_ff` resume path.** Live text:

> A v0 `mode: no_ff` open row remains `UnsupportedLegacyMode` before
> resume/mutation.

**(d) `:186–204` — rejection, migration sequence, fault injection, A2–A4.**
Live text runs from "An unreadable/contradictory row, unknown-field collision…"
(`:186`) through "…Existing v1+ records never auto-upgrade." (`:204`).

**RETAINED VERBATIM, mid-`:181`–mid-`:183`** (charter §10.2, cited there as
"§5:181–182"; the true span ends on `:183`):

> Completed and aborted v0 records remain v0 and use byte-preserving archival.
> Archived v0 uses only the archive decoder/projection below.

Replacement for the whole of `:160–204`, retained pair kept in place:

```text
There is no open-v0 migration. Eligibility, the migration whitelist, the
adapter, the representation-only migration write, and its atomic sequence are
deleted as production paths (charter §10.1). An open v0 envelope is the §2
refuse: read-only status emits that sentence and does not project a v0 merge
body, and a mutating command that finds one refuses with the same sentence and
writes nothing — it does not write v0, and it does not remain on the v0
lifecycle. Every open v0 envelope takes that one arm, `mode: no_ff` included;
there is no special `no_ff` resume path and `UnsupportedLegacyMode` names no
open-v0 route.

Completed and aborted v0 records remain v0 and use byte-preserving archival.
Archived v0 uses only the archive decoder/projection below.

A2–A4 retain the archive decoder and add no v0-to-v1 migration. Existing v1+
records never auto-upgrade.
```

**What §5 is after the amendment:** its heading, three replacement paragraphs
(the last of which is the retained pair, kept verbatim, plus the A2–A4
successor sentence). Retitling the section is **not** authorised by charter
§10 and is not done; §5's title still reads "Migration eligibility and atomic
boundary" over text that says there is no migration. Recorded as OPEN
QUESTION Q3.

### 3.9 §8:289 — retained-reader support, and the new decode generation

Live text, §8 `:289`:

> A1 supports v0 through its adapter and installed v1; A2 adds v2;

The clause this hangs on, §8 first bullet, `:262–263`, verified verbatim:

> - retain the latest successfully published pre-change release for each
>   materially distinct durable decode generation;

Replacement for the `:289` clause:

```text
A1 supports v0 through its adapter and installed v1; post-M5d 0.14 supports an
open v0 envelope as refuse only, installed v1 lifecycle, and archived v0
through the §7 decoder; A2 adds v2;
```

Addition to §8's selection rules, appended as a new bullet after `:272`:

```text
- treat post-M5d 0.14 as a NEW materially distinct durable decode generation:
  it stops decoding open v0 bodies, which the first bullet's rule makes a
  change of generation. The latest successfully published pre-change release
  for the superseded generation is `v0.13.0`, and it is retained on that
  ground. Its retention is also a precondition of the erasure itself (charter
  §2): deleting the v0 lifecycle engine is not authorized unless `v0.13.0` is a
  published, installable artifact and is the named escape for leftover open v0.
```

The `v0.9.2` pre-record lane and the `v0.10.2` durable-v0 baseline pins
(`:264–267`) are **unchanged**; nothing in charter §10 touches them.

### 3.10 §9:304–305 — activation

Live text, §9 `:304–305`:

> A1 alone activates the v1 writer floor, eligible v0 migration, and public
> no-ff surface in one reviewed change.

Replacement:

```text
A1 activated the public no-ff surface and the version-selection function; it
did not raise the writer floor. M5d's close activates the v1 writer floor for
ordinary and custom starts, deletes eligible v0 migration and the v0 lifecycle
engine, ships the production open-v0 refuse, and revises the record pins,
`GwzM5-8R2E-CapabilityFreeAmendment.md` §3 and the F-3 seam floor, in one
reviewed change. Raw record create and the event / response / `gwz stage`
parity surfaces are prior additive landings on `main`, not members of that
close.
```

§9's opening paragraph `:296–299` ("R3 may implement… Production creation
remains v0 and production migration has no call path.") is **not listed by
charter §10** and is therefore not rewritten here. It is already covered by
the `:7–13` banner's "§9 below describes the superseded test-only form". After
the close its second sentence is false on its face. Recorded as OPEN
QUESTION Q4.

## 4. What survives, stated positively

A reviewer checking the amended contract should find exactly this and nothing
more, in §4 and §5:

| Section | After the amendment |
|---|---|
| §1 | Allocation table unchanged (v0 stays allocated, for envelope classification). Classification list `:40–51` unchanged. `:36` replaced per §3.2. |
| §2 | Creation-matrix table frozen and unchanged. Annotation `:83–100` closed by an appended dated bracket. "Existing v1+ records never change version." / "Archived records never migrate." retained. `writer_version` paragraph retained less its migration sentence. |
| §3 | Pinned codes unchanged; the `UnsupportedLegacyMode` open-resume clause replaced. Protocol additive-only, one new optional field `MergeCrashRecovery.handles_ok`. |
| §4 | **Empty of production content.** Heading retained; body is the dated deletion stub of §3.7. |
| §5 | Heading retained; three paragraphs: the deletion statement + §2-refuse rule, the two **retained verbatim** archival sentences, the A2–A4 successor sentence. |
| §6 | Unchanged. |
| §7 | **Entire section retained and not amended** (charter §7 row, §10.2 bullet 3): archive decoder, gap computation, `legacy_complete`, deletion ownership gate, and "Unsupported or corrupt archives are never deleted or rewritten." |
| §8 | Selection rules unchanged plus one new bullet; `:289` clause replaced. |
| §9 | `:304–305` replaced; opening paragraph untouched (Q4). |

## 5. §10.3 close-time checklist — affected interfaces

Worked through at the close (charter §9 step 4), each item paired with the I2
edit above. Paths are gwz-dev-relative. Items marked **UNVERIFIED** were not
traced to a call site in this step and are named from the charter alone.

| # | Interface | File | Change |
|---|---|---|---|
| 1 | Migration whitelist registry | `gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` | No longer a live matcher for open files; retained as a historical registry. Charter §7 row: "revise with the I2 amendment". |
| 2 | Its checker | `gwz-core/scripts/checks/` — the predicates checker **UNVERIFIED** (not located in this step; locate by grep for the JSON's basename at close) | Stop asserting bidirectional rule/corpus coverage against a production matcher, or retire. |
| 3 | Open-v0 adapter + whitelist suites | `gwz-core/src/workspace_ops/merge/record_wire/open_v0/` (`descriptor.rs`, `structural.rs`, and siblings), `merge/store/atomic_upgrade.rs`, `merge/mod.rs:49–56` `#[cfg(test)] pub(crate) use record_wire::{…}` and `pub(crate) use store::{AtomicUpgradeFault, AtomicUpgradeOutcome, upgrade_open_v0};` | Leave production, or become the §2 refuse fixture. |
| 4 | Retained-reader fixture, open v0 | `gwz-core/scripts/retained_readers/manifest.json` | Open-v0 fixture expectation becomes the §2 refuse against 0.14; `v0.13.0` is the generation that continues/aborts it. Add the 0.14 generation per §3.9. |
| 5 | Writer floor | `gwz-core/src/workspace_ops/merge/model/version.rs:39` `ACTIVE_WRITER_FLOOR` | `V0` → `V1`; the doc comment that states `V0` moves with it. |
| 6 | Merge verb dispatch | `gwz-core/src/workspace_ops/merge/runtime/dispatch.rs:207` (`FileMergeStore`, `MergeStore`, `RecordVersion`, `discover_open_before_manifest` at `:4`) | Open-v0 arm becomes the §2 refuse; must not return "no merge" / idle. |
| 7 | Open-merge gate | `gwz-core/src/workspace_ops/merge/runtime/open_gate.rs:20,:36` | Same. |
| 8 | Workspace mutation guard | `gwz-core/src/workspace_ops/merge/runtime/mutation_guard.rs:84,:94` | Same; `gwz commit` / `gwz add` refuse with the §2 sentence or `OpenOperation` plus it. |
| 9 | `gwz stage` | `gwz-core/src/workspace_ops/handle_stage.rs:34` | Discover version-agnostically; open v0 is the §2 refuse, not the v1 body path and not idle. |
| 10 | Merge start gate | `gwz-core/src/workspace_ops/merge/start.rs` | The existing open-operation remedy ("use merge status, merge continue, or merge abort") is **suppressed** for a v0 envelope (charter §2). |
| 11 | `discover_open` | `gwz-core/src/workspace_ops/merge/store/mod.rs` (`MergeStore::discover_open`, `FileMergeStore` at `:79–81`) | Envelope-only for v0; no body decode. `classify_open_record` / `OpenRecordEnvelope` / `discover_open_envelope_before_manifest` (already re-exported at `merge/mod.rs:86–90`) are the surviving classification seam. |
| 12 | id-less `--status` / `--gc` | `gwz-core/src/workspace_ops/merge/store/gc.rs`, `store/retention.rs` | Unchanged as GC (I2 §7); the id-less arms must not project a v0 open merge. |
| 13 | Crash-recovery decision | `crash_recovery_decision` **UNVERIFIED** (charter §3/§10.3 names it; not located in this step) | Gains the directory-level handle probe on the **workspace root** and carries `handles_ok`. |
| 14 | Response field | `gwz-core/protocol/gwz.taut.py` `MergeCrashRecovery` (fields 1-3 today, verified at `dev-docs/GwzM5-8DR1-WarnOrRefuse-Charter.md:300-301`), plus the generated sides: `gwz-core/src/protocol/generated.rs`, `gwz-core/tests/protocol.rs`, `gwz-py/protocol/generated/*`, `gwz-py` `scripts/check_protocol_drift.py`, `test_protocol.py` | Optional **field 4** `handles_ok` (BOOL, absent above the bar), additive, no version bump. |
| 15 | Docs manifest + echo tests | `merge_docs_manifest.json` warning regex; gwz-cli / gwz-py sentence echo tests **UNVERIFIED** | Move in the same landing as the diagnostic sentence (charter §3). |
| 16 | CapabilityFree §3 | `dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md` §3 and `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:522–580` | Per `GwzM5-8M5d-GateRevisions.md` Part A. |
| 17 | F-3 seam floor | `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:355–390`, `:1378–1390`, `:1487–1506`; tests `scripts/checks/test_check_checked_artifact_boundaries.py:1161–1242` | Per `GwzM5-8M5d-GateRevisions.md` Part B, with the J-1 ruling. |
| 18 | A1 activation record | `dev-docs/history/GwzM5-8A1ActivationRecord.md` | Dated note retargeting the ship-2 / floor-raise **carrier name** to M5d. Do not rewrite A1's historical landing (charter §7). |
| 19 | Ship (1) / DR-1 pointer | `dev-docs/GwzM5-8DR1-WarnOrRefuse-Charter.md` §3.7 (`:295-302`) | `MergeCrashRecovery` gains optional field 4 `handles_ok`; the ship-2 **pointer** retargets to this charter by a dated note on that file. |
| 20 | Docs cross-refs | `merge.md`, `capability.rs` / `checked.rs` cross-refs (charter §5) | ordinary = v1; open v0 = use `v0.13.0`. |

## 6. Apply order at the close

Ordering matters because the corpus's own annotation mechanism re-drifts line
cites (2026-08-27 note).

1. Apply §3.2–§3.10 **bottom-up** (§9 first, §8, §5, §4, §3, §2, §1). Editing
   upward first would invalidate every unapplied cite below it mid-edit.
2. Apply the §3.1 banner **last**. It is an insertion near `:5` and shifts
   every line in the file.
3. Re-measure the resulting line numbers and file a dated citation-drift note
   in `dev-docs/CurrentProgramCheckpoint.md` — **not** in I2 — following the
   2026-08-27 precedent and its stated reason ("a ~50-line insertion there
   would re-drift every line cite below `:82` a second time"). This amendment
   is a net **deletion** of roughly 75 lines from `:119–204` plus roughly 25
   inserted, so the drift below §5 is large and negative and must be recorded.
4. Do **not** re-point the existing dated cites in `GwzM5-8R4bG-Evidence.md`,
   `GwzM5-8M5bNoFfDesign.md`, `GwzM5-8R2DSettledTuple.md` or the settled
   reviews. They are dated records, left as written; the checkpoint note is the
   resolution mechanism.

## 7. OPEN QUESTIONS

Recorded, not resolved. Q2, Q5 and Q6 are candidate charter inconsistencies.

**Q1 — banner date.** Charter §7 gives the banner as `Amended <date> by
GwzM5-8M5d-Charter.md` "when the close lands". The corpus convention, verified
at `dev-docs/history/GwzM5-8I2Refreeze-ReviewConsistency.md:16`, is "date of the effecting
amendment, not of the edit", which gives 2026-09-03 (the charter's date). §3.1
uses 2026-09-03. If the reviewers read the charter as requiring the close
date, §3.1's first line is the only edit.

**Q2 — §10.2 retains a sentence that names a deleted concept.** §10.2 retains
I2 §2's `writer_version` meaning "except the 'V0-to-v1 migration therefore
sets…' sentence". The immediately preceding retained sentence reads "Every
mutating **migration** or current-version rewrite sets it to the current binary
version in the same atomic record write." Post-close no migration exists, so
the retained sentence names nothing. Options: (a) retain verbatim as §10.2
literally directs and accept a dangling term; (b) narrow it to "Every
current-version rewrite sets it…". (b) is a text change §10.2 does not
authorise. §3.4 takes (a). Reviewers rule.

**Q3 — §5's title after the amendment.** The section is titled "Migration
eligibility and atomic boundary" and, after the amendment, contains no
migration and no atomic boundary. Charter §10 authorises no retitle and §3.8
does not perform one. Options: leave it (title is a stable citation anchor);
or annotate the heading. Reviewers rule.

**Q4 — I2 §9's opening paragraph.** `:296–299` says "Production creation
remains v0 and production migration has no call path." Charter §10.1 supersedes
only `:304–305`. After the close the first clause is false and the second is
true for a new reason (there is no migration at all). The `:7–13` banner
already declares §9's form superseded. Options: (a) leave untouched, as §10
lists it nowhere and §10's preamble says implementers must not invent scope;
(b) append a one-line dated bracket. §3.10 takes (a) and flags it. Reviewers
rule; (b) is the smaller reader hazard.

**Q5 — I2 §1's ordered classification has no arm for the §2 refuse.** §1
`:44–51` says an exact allocated **and installed** pair dispatches to that
version's decoder (step 3), and an exact allocated but **uninstalled** pair is
`UnsupportedRecordVersion` reporting its required wave (step 4). Post-close,
`gwz.merge-operation/v0` / `0` is allocated, its **archive** decoder is
installed, and its **open-record** decoder is not. Charter §2 requires an open
v0 envelope to yield the §2 sentence (typed message, "or `OpenOperation` plus
that sentence"), which is neither step 3 nor step 4's
`UnsupportedRecordVersion`. Charter §10.1's §1:36 row does not amend the
classification list, and §10.2 retains I2 §3's pinned codes. This looks like a
gap between charter §2 and I2 §1's frozen classification. Resolving it is a
design act (which code, on which step) and is not performed here.

**Q6 — §10.2 carries two edits that §10.1 does not list.** The
`writer_version` migration sentence (§3.4) and the I2 §3
`UnsupportedLegacyMode` clause (§3.6) are both changed only by exception
clauses inside §10.2, a section headed "**Retained** (do not delete in a §4–§5
rewrite)". An implementer reading §10.1 as the complete change list would miss
both. §3 renders them as explicit clauses. Flagged so the reviewers can
confirm the reading is the charter's intent and not a widening.

**Q7 — `GwzM5-8I2CompatibilityPredicates.json`'s checker was not located.**
Charter §10.3 bullet 1 names "its checker". Item 2 of §5's checklist is
UNVERIFIED: no checker for that JSON was traced in this step. Either it is
named by a path this step did not search, or the checker is the whitelist
suite itself. The close must locate it before claiming §10.3 discharged.

**Q8 — §4's stub is a recommendation, not a charter ruling.** §3.7 recommends
retaining §4 as a dated stub over removing the section. The charter's §10.2
heading ("do not delete in a §4–§5 rewrite") is the only evidence of intent
and it is weak. The reviewers rule; if they rule for removal, §3.7's fenced
block is dropped and §6 step 3's drift measurement grows by 38 lines.
