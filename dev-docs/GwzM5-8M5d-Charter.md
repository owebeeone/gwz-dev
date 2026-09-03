# M5d — v1 is the only merge; best effort on odd filesystems (DR-1 ship 2)

**Status:** CHARTERED 2026-09-03 by the operator. **ACCEPTED at
revision 5**, working-tree SHA-256
`df6399662c2c93b3e94072f62cd61856e74fa0b7ef6f6699b685e2ae804e32ec`,
by **operator decision 2026-09-03** ("we've done enough re-reviews"):
the review loop is closed at round 4 with its findings dispositioned in
`GwzM5-8M5d-RemPlan-3.md` (two taken, one closed-disputed by the §0
ruling, two folded); no further re-verdict is run. Revision 3 had a
dual GO at `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`
after `GwzM5-8M5d-ReviewConsistency-3.md` / `GwzM5-8M5d-ReviewSafety-3.md`;
revisions 4–5 are operator-directed folds on top of it. This accepts
**ship-2 design of this charter only** (not an implementation, not a
tag, not I2.md body, not M6). **Controlling** for ship 2. Build starts
only when the operator authorizes it.
**Revision 3** 2026-09-03 — RemPlan-2 (`GwzM5-8M5d-RemPlan-2.md`).
**Revision 4** 2026-09-03 — operator-directed folds from the
implementation-lane review (`GwzM5-8M5d-ReviewImplementation.md`):
L-P2-1 (decision-time handle probe; one Diagnostic), L-P2-2 (landing
shape: parity, raw create and suite re-pointing may land on `main`; the
single dual landing is floor + refuse + erasure + pins), L-P3-1 (one
optional response field `handles_ok`), L-P3-2 (F-3 ghost-seam option
struck), L-P3-3 (existing open-operation remedy suppressed for a v0
envelope), L-P3-4 (product sentence names the release, not the tag).
The revision-3 acceptance SHA above does **not** cover revision 4.
**Revision 5** 2026-09-03 — RemPlan-3 (`GwzM5-8M5d-RemPlan-3.md`)
applied on the operator's say-so: operator ruling recorded in §0 (0.14
is consistent only with itself; pre-close v0 behaviour is out of
scope); S-P2-2 closed **disputed** by that ruling; S-P2-3 taken (handle
probe on the workspace root, not `.gwz`); C-P2-1 taken (§10.1 I2 §9
replacement matches the landing shape); C-P3-1 / C-P3-2 folded. The
operator closed the review loop at this revision (see Status); no
further re-verdict.
**Supersedes** `GwzM5-8M5c-Charter.md` **for all ship-2 design**, including
leftover-v0 reader/lifecycle. Event / response / `gwz stage` / raw-create
requirements are **restated in this charter §3–§4** and those restatements
control. M5c is historical citation only; no M5c section is current
authority. Home: R2-E phase E8. Design only until a later build is
authorized. No tag of any kind is cut by this charter; the operator asks
for every tag.

Ship (1) (warn-or-refuse, identity bar, `--filesystem-strict`) stays on
`main` untagged. Do not cut 0.14.0 until this charter's close lands. M6
is not in scope.

## 0. Operator decisions (2026-09-03, binding)

> I want to kill v0 entirely. I don't want 0.14.0 to be able to understand
> a v0 partial merge. I want to erase it from main HEAD.

> Code from 0.14 is to have no concept of a v0 merge — ignore it completely
> or at most say that it's a pre-0.14 merge and you need to use a 0.13
> build to continue — nothing else.

> I want 0.14.0 merge to do best effort on every filesystem and say so
> when the filesystem is Unsupported or the `--filesystem-strict` check.

> Balance: get merge v1 done fast, and best effort, but don't worry about
> the odd FS in the 1/100000 case (system crash).

> (i) [stated limit that abort on overlay may newly refuse, cured later]
> is a no-go as a *product* story for start. Do not build reverse-path
> raw as a milestone. Do not engineer reboot-durable recovery on
> handle-less volumes.

> No M6 yet.

> (2026-09-03, after the round-4 re-review, binding) 0.14 must only be
> consistent **with itself**. v0 safety is off the table: leftover v0
> merge behaviour, status-quo staging or abort of a v0 merge, and mixed
> untagged-`main` windows before the close are out of scope. "Worse
> than today's v0" is not a 0.14 defect. I do not want to buy the
> compatibility issue.

0.14 self-consistency, as this charter uses it: every new start is v1;
there is no v0 merge engine; an open v0 *file* is the §2 sentence (not
a merge, not idle); `gwz stage` serves open v1; archived `done/` v0 is
projection only. The close still ships floor + refuse + engine deletion
together so that **0.14** does not both write v0 and refuse it.
Intermediate `main` landings are not a product and are not reviewed as
one.

## 1. What 0.14.0 *is*

One merge implementation: **v1**. Ordinary, `--ff-only`, custom-message,
and `--no-ff` starts all write v1 (`ACTIVE_WRITER_FLOOR = V1`). The v0
start / continue / abort / migrate / open-body / `adapt` **lifecycle
engine** is **not in the binary**. The v0 **archive decoder** (I2 §7) is
not that engine; it stays (see §2, §5).

On a normal volume (local APFS / HFS+, Linux ext4 / xfs / f2fs, Windows
NTFS): catalog activates as in ship (1); events, completed repo rows, and
`gwz stage` match today's v0 *shape* (**restated in §4**; that text
controls). That is “v1 done” on the path people use.

On an odd volume (overlay without `nfs_export`, sshfs / FUSE without
export, tmpfs / ramfs, pre-6.9 Linux, anything else that fails the
identity or handle probe): the merge **starts and can complete the
forward path**. One warning (ship (1) sentence, plus the §3 reverse-door
limit on handle-fail volumes). No catalog. The record is written **raw**
if `create_merge_store_record`'s handle probe fails. Reverse doors that
need handles still refuse as §3. Crash recovery on that volume is **not
a product**. A power loss mid-merge there is operator cleanup, not a
recover() grammar.

`--filesystem-strict` remains the **refuse** opt-in (start only, before
lease / record / Git). It is not a second merge mode. It is live on
ordinary starts only in the same reviewed change that raises the floor
(§7, §9). Until that change, mutating v0 commands remain on today's v0
lifecycle.

## 2. Open v0 is not a merge — and is still an open operation

**Precondition of deleting the v0 engine:** tag `v0.13.0` remains a
published, installable artifact and is the retained-reader escape for
leftover **open** v0 for as long as 0.14 is current. Deleting the engine
is **not authorized** if that artifact is not published and installable.
The 0.14 notes that name this (abort-or-finish leftover open v0
**before** upgrade; how to obtain `v0.13.0`) are a **precondition of the
irreversible step**, not a follow-on after erasure.

`discover_open` (and every merge verb) that finds an envelope whose
record version is v0:

- does **not** decode the v0 body;
- does **not** continue, abort, migrate, or project the attempt as a
  merge lifecycle;
- **does** treat the envelope as an **open operation** (third occupancy:
  not a merge lifecycle, not idle);
- emits one typed message, and nothing else:

```text
this is a pre-0.14 merge; use gwz 0.13.0 (the last release before 0.14) to continue or abort
```

(The release is tag `v0.13.0`; the sentence names the release because
users install releases, not tags. The 0.14 notes say where to get it.)

Mutation gate, `gwz commit`, `gwz add`, and `gwz stage` on that occupancy
refuse with the same sentence (or `OpenOperation` plus that sentence).
The existing open-operation remedy text (“use merge status, merge
continue, or merge abort”) is **suppressed** for a v0 envelope: that
remedy is false under 0.14, and the §2 sentence is the whole remedy. The
sites are the merge-verb dispatch, the open-merge gate, the workspace
mutation guard, `gwz stage`, id-less `--status`, `--gc`, **and the merge
start gate itself** (`merge/start.rs`, which today emits the old remedy).
They must **not** return “no merge” / idle. `gwz stage` does not run the
v1 body path and does not stage as if idle. Envelope classification does
not decode the v0 body.

No whitelist. No `open_v0`. No “valid unlisted stays on the v0
lifecycle.” Escape is the `v0.13.0` binary. 0.14.0 must not guess Git
`MERGE_HEAD` / index repair.

**Archived `done/` v0** (completed or aborted): **read-only archive
projection only** (I2 §7 decoder and GC) — enough that old history is
not a silent hole. Not a merge. Not a resume. Consumed only for `done/`
bytes, never for open files. `decode` of archived v0 is **not** “the v0
engine.” If the operator later wants those unread too, that is a
one-line amendment; default is project. GC still never deletes an
unread/unreadable archive (I2 §7).

This **amends** `GwzM5-8I2CompatibilityContract.md` as the exact
superseded / retained list in **§10**. Dual review on that amendment
with this charter. A1's “we still read leftover v0” residual closes by
**deletion** of the open lifecycle, not by migration.

## 3. Filesystem — best effort, say so, do not over-build

Same decision point as ship (1) (`crash_recovery_decision`), once per
process, before any lease.

| Volume | Merge | Catalog | Record create | What we say |
|---|---|---|---|---|
| Identity + handles proved | run | activate | checked | nothing new |
| `Unsupported` (no durable identity / remote / volatile) | **run** | none | v1; checked if handles exist | ship (1) warning |
| Handle probe fails (overlay, some FUSE) | **run** | none | **raw** (`write_atomic_verified`) | **one** Diagnostic: the ship (1) sentence with the reverse-door sentence below appended (decided at the decision point, see “Where handle capability is learned”) |
| `--filesystem-strict` and not proved | **refuse** | none | none | existing strict refusal |

Raw create is gated to the uncatalogued / handle-fail arm only. Above
the bar, a boundary `Unsupported` is still an error (anomaly). I/O and
noncanonical parents still fail.

**No second warning** for the raw write itself. The machine form is
`crash_recovery.supported = false` plus **one optional additive response
field** `crash_recovery.handles_ok` (BOOL, absent above the bar), so a
JSON / porcelain consumer can tell that selected-root / `--preserve`
abort may refuse on this volume. Additive field only, no protocol
version bump, per the ship (1) ruling (request flag, response field,
diagnostic event allowed).

**Where handle capability is learned (revision 4, L-P2-1)**

The ship (1) decision runs before any lease and, as built, learns
identity / remote / volatile but not handle capability; the handle probe
is met later at the create door. The gap does not settle it: on Linux
`no_durable_identity` implies the handle fallback already failed, but
`remote` and `volatile` do not (NFS and tmpfs answer `name_to_handle_at`)
and those volumes must **not** carry the reverse-door limit. Therefore
`crash_recovery_decision` gains a **directory-level handle probe** — the
create door's own probe applied to the **workspace root** (the directory
whose identity the create door takes today, and which exists before any
write) — and carries `handles_ok` alongside the gap. It is **not**
applied to `.gwz`: a first merge has no `.gwz` yet, and a missing
private directory is not a filesystem capability gap (revision 5,
S-P2-3). Missing `.gwz` on APFS / ext4 → `handles_ok = true`, checked
create, no reverse-door sentence. Overlay → `handles_ok = false`. NFS /
tmpfs that answer `name_to_handle_at` → `handles_ok = true`, no
reverse-door sentence. One Diagnostic event: the ship (1) sentence, with
the reverse-door sentence appended **only** when `handles_ok` is false.
The create door consults the same result (raw when false; a create-door
handle failure when `handles_ok` is true is still an anomaly error). The
docs-manifest regex and the gwz-cli / gwz-py echo tests that pin the
sentence move in the same landing.

**Reverse doors on handle-fail volumes (product, not a milestone)**

Do **not** refuse start. Do **not** add reverse-path raw as a phase.

- **(a)** Participant-only abort (no checked artifact) still clears the
  v1 record.
- **(b)** Selected-root / `--preserve` / published-evidence abort that
  hits the handle probe **refuses**. The refuse text is **not** the
  existing identity-boundary sentence (that sentence advertises
  `gwz merge --abort` as the escape, which is circular here). New
  sentence, this close, names **one** working escape that needs
  **neither** handles **nor** 0.13.x: copy the whole workspace onto a
  volume that proves handles (APFS / HFS+ / ext4 / xfs / f2fs / NTFS),
  then `gwz merge --abort` (add `--preserve` if that was the stuck
  door). Do **not** delete `.gwz/merge/<id>.yaml` in place, and do
  **not** treat `git merge --abort` as a stand-in for evidence / root
  / `--preserve` rollback.
- Start on those volumes states this limit in the same diagnostic (not
  a second unrelated warning class), which is possible because the
  decision already knows `handles_ok` (above): selected-root /
  `--preserve` abort may refuse until the workspace is on a
  handle-capable volume.
- Optional same-flag raw abort remains a **later cure**, not this close.
- A 0.13.0 binary is **not** an accepted escape for an open **v1**
  record.

**What we do not build on odd volumes**

- Catalog / nonce / reboot-durable identity.
- `recover()` of a half-written raw record after a crash.
- Reverse-path raw as a **phase**.
- Graded evidence, per-attempt decision field, Windows
  `FILE_SUPPORTS_OPEN_BY_FILE_ID` (keeps `require_ntfs` until a
  Windows-verified step).

`.cursor/rules/transactional-apis.mdc` is **excepted** for the
uncatalogued / handle-fail **crash** arm only (operator §0). The
above-bar path is unchanged. The exception does **not** cover live
abort (this section's reverse-door product).

## 4. Surfaces that stay (common path — do not skip)

Raising the floor without these makes v1 look broken on APFS/ext4.
These restatements control; they are not “M5c kept.”

- **Events (shape parity):** the v1 forward loop emits, per participant,
  `member_started` **before** that participant's observation/apply and
  `merge_member_finished` (with the `MergeRepoSummary` projection)
  after its durable row; `operation_state_changed` at every durable
  state transition the store commits; `artifact_written` for record
  create, each record commit, the archive, and the publication
  artifacts. Same kinds, same ordering discipline, same
  per-participant count as v0 for the same fixture; message text may
  differ. Continue and abort arms get the same treatment.
- **Responses:** a completed v1 start (and continue) returns the
  per-repo rows, `participant_counts`, `publication_step`, and
  `preservation` projected from the **archived** record (I2 §7: the
  archived projection reads only the exact done-record bytes). Start
  decorations (`predicted`, `prediction_complete`, `live_commit` for a
  conflicted row) apply to the v1 start response as
  `decorate_start_response` applies them to v0. This also fixes today's
  empty `--no-ff` completed response.
- **`gwz stage`:** discover the open record version-agnostically and
  derive stage member paths from the **v1** body. Root conflict
  staging works. (Open **v0** occupancy is §2 refuse, not this path.)
  This is a **0.14** property. Whether a pre-close `main` tree still
  stages a leftover v0 merge as today is out of scope and not pinned
  (operator §0, revision 5). Hygiene, not a pin: an implementer landing
  (2) early may keep today's v0 stage arm beside the v1 one until (4).
- **Dry-run:** still intercepted before the version fork; pin that an
  ordinary dry-run under floor V1 predicts and writes nothing.

F-3 / call-graph: `v1_lifecycle/` must not name the v0 store seam.
Relocate `write_atomic_verified` to a neutral module; **only**
`entry.rs` names the relocated primitive among production callers of
raw create. Relocating that primitive is **not** the whole F-3 story:
when production v0 persistence re-exports (`persist_merge_record`,
`FileMergeStore`, …) leave, F-3's seam floor is **redefined** for that
tree: the floor becomes the neutral raw primitive's module, and the
property stays the same masked-identifier scan — `v1_lifecycle/` names
that module nowhere; only `entry.rs` calls it. No hollow re-export is
kept in production so a regex can derive a floor (revision 4, L-P3-2:
that would be the ghost seam §5 forbids for suites). Deleting
`persist_merge_record` / `finalize.rs` is paired with that checker
change (§7). The v0
*lifecycle engine* is deleted from production; the primitive and the
archive decoder are not “the v0 merge.”

## 5. What happens to the v0 corpus and pins

- Production code: no v0 **lifecycle engine**, no `with_writer_floor(V0)`
  as a shipped path. The I2 §7 archive decoder and GC projection
  **remain production**.
- g23 suites that **are** the v0 engine: leave production (delete or
  move to a fixture that only proves the §2 refuse). Do not wrap them
  to keep a ghost engine. Do not delete the archived-v0 decoder suite.
- Suites that assert the **shipped default** (`a1_activation`,
  `g23/start` envelope, `version.rs`): flip to v1.
- Retained-reader manifest: 0.14 vs an *open* v0 fixture = the §2
  refuse. Old binaries vs new v1 records unchanged in direction.
  `v0.13.0` is the named leftover-open escape.
- `merge_docs_manifest.json`, `merge.md`, `capability.rs` /
  `checked.rs` cross-refs: ordinary = v1; open v0 = use `v0.13.0`.

## 6. What M5d does not do

M6 / v2. Protocol version bump (additive fields only). Catalog on
every volume. Converting archived v0 into v1. Marker/lock/boundary
conversion. Reconciliation S2+. Local clone. A 0.14.0 tag until the
operator asks. Implementing M5c as written. Reverse-path raw as a
milestone. `recover()` of a half-written raw record.

## 7. Contract and review

| Object | Disposition |
|---|---|
| `GwzM5-8M5c-Charter.md` | **superseded for all ship-2 design** by this charter as of 2026-09-03; do not implement; §0–§6 of that file are not current authority |
| `GwzM5-8I2CompatibilityContract.md` | **amend** per §10 (not “§4–§5 as wholes”). Dual (Consistency + Safety) on the amendment with this charter. I2 banner: `Amended <date> by GwzM5-8M5d-Charter.md` when the close lands |
| `GwzM5-8I2CompatibilityPredicates.json` | affected interface: whitelist registry is no longer consulted for open v0; revise with the I2 amendment |
| `GwzM5-8A1ActivationRecord.md` residual | closed by this charter (deletion, not migration). Dated note retargets the ship-2 / floor-raise **carrier name** to M5d; do not rewrite A1's historical landing |
| Creation matrix A1 row | already `ordinary/custom = v1`; the 2026-08-25 “floor still V0” annotation **closes** (historical after the floor raise) |
| Ship (1) charter | warn / strict / identity-bar **structure** unchanged. DR-1 §3.7's `MergeCrashRecovery` gains optional field 4 `handles_ok` (additive, no version bump; §3, §10.2) — 0.14 speaking about itself. Ship-2 **pointer** retargets to this charter (dated note on the DR-1 file when the close lands) |
| I2 record body / catalog tuple | unchanged |
| `GwzM5-8R2E-CapabilityFreeAmendment.md` §3 / `CAPABILITY_FREE_RAW_WRITER_INVENTORY` | **revise the carved set** when v0-engine files leave production (shrinkage is an amendment, not a checker edit). The v0-engine carved files (`finalize.rs`, v0 abort/preserve/archived writers — **not** `store/gc.rs` / `store/retention.rs`) may leave; one `entry.rs` raw primitive may enter (S1 measurement, including adding a token if `write_atomic_verified` is not in `CAPABILITY_FREE_WRITER_TOKENS` today). That revision is in the S1 dual |
| F-3 `V0_PERSISTENCE_SEAM_FLOOR` | **redefine** for a tree whose production v0 persistence re-exports are gone: floor = the neutral raw primitive's module; only `entry.rs` calls it. No test-only or hollow seam kept for derivation (§4). Relocating `write_atomic_verified` is not sufficient; the J-1 ruling that the checker comment requires is recorded in the S1 dual |
| `transactional-apis.mdc` | excepted for uncatalogued/handle-fail **crash** only (§3); not live abort |
| I2 §7 archive decoder / GC | **retained**; not amended |

Floor raise + v0-lifecycle erasure + the **production** §2 refuse +
`--filesystem-strict` on ordinary starts + the pin / doc / CapabilityFree
/ F-3 revisions land as **one reviewed change** (the close). Dual on
that landing and on the compatibility amendment. Raw create, the §4
surfaces, and the class-(ii) suite re-pointing may land on `main`
**before** the close as their own reviewed steps (§9, revision 4): they
are additive, leave ordinary starts on v0, and are measurable on
`--no-ff`, which 0.13.0 already ships. Until the close commit, mutating
v0 stays on today's I2 §5 lifecycle. Drafting §10's I2 text and the
header-only envelope classifier may precede the close; the production
refuse **must not**. What a pre-close `main` tree does with a leftover
v0 merge beyond that is not a 0.14 property and is not reviewed as one
(operator §0, revision 5).

## 8. Acceptance

On any filesystem GWZ can write a file on: `gwz merge <source>`
(ordinary, `--ff-only`, `-m`, `--no-ff`) writes a **v1** record and
completes or stops as a v1 merge. Above the bar, catalog silent.
Below it, one warning, no catalog. Handle-less: raw create, forward
path still runs; participant-only abort clears; selected-root /
`--preserve` abort prints the §3 escape and leaves Git/record in a
state that escape actually clears (a 0.13.0 binary is not an accepted
pass for that door). `--filesystem-strict` refuses before writes.
`--dry-run` predicts, writes nothing. Events and completed responses
have v0's *shape* (§4). `gwz stage` works under an open v1 merge.

An open v0 file yields **only** the §2 sentence, with no body decode.
It is an open operation: abort / continue / status / commit / add /
stage all refuse without writing; none behaves as idle. No production
v0 **lifecycle engine** remains. The archived-v0 decoder remains; §8
is checked off without deleting it, and cannot be read as requiring
that deletion. Named leftover-open-v0 fixture as above. Named
archived-v0 fixture still projects under I2 §7; GC never deletes
unread/unreadable archives.

No tag until the operator asks.

## 9. Build order (revision 4: additive steps land on `main`; the close is one landing)

Aspirational, so the close is not re-invented at implement time. The
Safety round-1 walk (a published `main` carrying the §2 refuse while
`ACTIVE_WRITER_FLOOR` is `V0`) stays unconstructable: **the production
§2 refuse and the floor flip are never on `main` apart** — they are
inside the single close landing (4) together with engine erasure and
the pin / doc / CapabilityFree / F-3 revisions. The dual's object for
(4) is one tree where: ordinary start is v1; open v0 is the production
§2 refuse; `--filesystem-strict` refuses a below-bar ordinary start
before writes; CapabilityFree §3 and F-3 match that tree; raw create
and the §4 surfaces are already live from (2) and (3).

Steps (1)–(3) are **landable `main` steps** with their own by-reading
reviews, because each leaves ordinary starts on today's v0 lifecycle
and is measurable on `--no-ff`. How such a pre-close `main` tree treats
a leftover v0 merge (staging, abort) is not a 0.14 property and is not
pinned (operator §0, revision 5); the only invariant kept across those
landings is the one above — refuse and floor never apart:

1. Draft the I2 amendment text (§10), the CapabilityFree §3 revision
   and the F-3 redefinition text (S1 dual). The **header-only envelope
   classifier** may land in production: it is envelope-version dispatch
   only and decodes no v0 body itself; the v0 **lifecycle** still
   body-decodes until (4). The **refuse arm** may not land while the
   floor is `V0`.
2. Event / response / `stage` parity (§4), verified on `--no-ff` and,
   for the ordinary-path suites, under a `cfg(test)` writer-floor
   override at `V1` (no such seam exists today; ~30 lines). The
   class-(ii) product suites (ordinary-path behaviour, ~9,300 lines)
   are re-pointed to v1 under that override in their own landings, so
   parity gaps surface before (4), not inside it.
3. Raw create in `entry.rs` (handle-fail / uncatalogued only), with the
   decision-time handle probe and the `handles_ok` field (§3).
4. The close: `ACTIVE_WRITER_FLOOR = V1`; production §2 refuse; delete
   the v0 lifecycle engine; flip pins and docs; CapabilityFree + F-3
   revisions land; dual.

(2) and (3) may run in parallel. (4) does not land before (1)–(3).
Reverse-path raw is **not** a step. Release notes that require
abort-or-finish of open v0 before upgrade, and that name `v0.13.0`, are
a precondition of (4), not a follow-on. The 0.14.0 tag remains
operator-asked after the dual GO.

## 10. I2 amendment block (L1-08 — exact clauses)

When the close lands, I2 is `Amended <date> by GwzM5-8M5d-Charter.md`.
This document remains authoritative only as amended for open-v0
lifecycle, writer-floor residual, and the 0.14 decode generation.
Implementers produce the I2 diff from this list without inventing
scope.

### 10.1 Superseded (quote → replacement)

| I2 location | Superseded text | Replacement |
|---|---|---|
| §1:36 | “A1 compiles body decoders and canonical lifecycle support only for v0 and v1.” | Post-M5d binaries compile **open-record** body decoders and canonical **lifecycle** support only for v1. The v0 **archive** decoder remains (I2 §7). Open v0 is envelope classification only (§2 of this charter). |
| §2:102–103 | “Open v0 records may use only the explicit A1 migration path.” | Open v0 records are not a merge. Envelope-only §2 refuse; no migrate, no whitelist, no adapter. |
| §2 dated annotation 2026-08-25 (lines 83–100) | Remainder “owned by milestone **M5c**” and “until M5c lands.” Frozen creation-matrix **table** unchanged. | Annotation **closes** when `ACTIVE_WRITER_FLOOR = V1`. Carrier is **M5d**. After close the annotation is historical; the table row `ordinary/custom = v1` is the shipped fact. |
| §4 entire (“Open v0 compatibility result”) | Structural validity then whitelist **or** valid-unlisted legacy-v0 handling; A1's seven whitelist shapes; adapter constructs `AcceptedWorkspace`. | Deleted as a production path. Open v0 → §2 of this charter. The predicates JSON is no longer a live matcher for open files. |
| §5:160–179, 186–204 (eligibility, adapter, zero-whitelist status projection, migration sequence, A2–A4 retain the same eligible migration) | Open v0 eligible only on whitelist match; migration is representation-only; “Zero whitelist matches is not an error. Open read-only status leaves bytes unchanged and projects v0…” | Deleted as a production path. Open read-only status on a v0 **envelope** emits the §2 sentence and does not project a v0 merge body. |
| §5:180 | “An existing mutating v0 command remains on the existing v0 lifecycle and may write v0 only when that path's existing preflight authorizes it.” | A mutating command that finds open v0 refuses with this charter’s §2 sentence; it does not write v0. |
| §5:183–184 | “A v0 `mode: no_ff` open row remains `UnsupportedLegacyMode` before resume/mutation.” | Subsumed: every open v0 envelope is the §2 refuse (no body decode). Do not keep a special `no_ff` resume path. |
| §8:289 | “A1 supports v0 through its adapter and installed v1” | 0.14 (post-M5d) supports open v0 as envelope refuse only; installed v1 lifecycle; archived v0 via I2 §7. This is a **new durable decode generation** vs `v0.13.0` (I2 §8 first bullet: retain latest published pre-change release per materially distinct decode generation — that release is `v0.13.0`). |
| §9:304–305 | “A1 alone activates the v1 writer floor, eligible v0 migration, and public no-ff surface in one reviewed change.” | **M5d**'s close activates the v1 writer floor (ordinary included), **deletes** eligible v0 migration and the v0 lifecycle engine, ships the production §2 refuse, and revises the pins / CapabilityFree §3 / F-3, in **one reviewed change**; the public no-ff surface is kept. Raw create and the §4 surfaces are **prior additive `main` landings**, not members of that close (charter §7, §9). |

### 10.2 Retained (do not delete in a §4–§5 rewrite)

- §2: “Existing v1+ records never change version.” “Archived records never migrate.” `writer_version` meaning except the “V0-to-v1 migration therefore sets…” sentence (that sentence dies with migration).
- §5:181–182: “Completed and aborted v0 records remain v0 and use byte-preserving archival. Archived v0 uses only the archive decoder/projection below.”
- I2 §7 entire (archive decoder, gaps, `legacy_complete`, “Unsupported or corrupt archives are never deleted or rewritten”).
- Creation-matrix table (A1 `ordinary/custom = v1`).
- I2 §3 pinned codes, except that open-v0 resume is no longer the
  `UnsupportedLegacyMode` no_ff path; protocol remains additive-only
  (this charter allocates exactly one optional response field,
  `MergeCrashRecovery.handles_ok`, §3; no version bump; no record or
  catalog format change).

### 10.3 Affected interfaces and tests

- `gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` and its
  checker (whitelist no longer a production open-v0 path).
- `adapt_open` / whitelist suites: leave production or become the §2
  refuse fixture.
- Retained-reader fixture for **open** v0 = §2 refuse against 0.14;
  `v0.13.0` remains the generation that continues/aborts it.
- `version.rs` `ACTIVE_WRITER_FLOOR`, `dispatch.rs` open-v0 arm,
  `discover_open` envelope-only, mutation gate / `open_gate` /
  `handle_stage` occupancy, and the merge **start gate** in
  `merge/start.rs` (old remedy text suppressed, §2).
- `crash_recovery_decision` (directory-level handle probe, §3);
  `gwz.taut.py` `MergeCrashRecovery.handles_ok` (additive); the
  docs-manifest warning regex and the gwz-cli / gwz-py echo tests.
- CapabilityFree §3 and F-3 as §7.

### 10.4 Why

Operator: 0.14 must not understand a v0 partial merge. Whitelist
migration and “valid unlisted stays on the v0 lifecycle” are the
behaviors that sentence forbids. Archive projection is the default
that keeps history from becoming a silent hole.
