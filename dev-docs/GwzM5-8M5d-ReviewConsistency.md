# GwzM5-8M5d — CONSISTENCY-AXIS REVIEW

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `cc3635c8419e258ba51b7d8891ff3ed18c589d362aa7f9f77bfc27de6e7f2211`). Controlling DRAFT document: the object itself.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98` (verified at start and end). gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c` (locked at start; verified at end). Object bytes read from the working tree. Committed controlling documents read from `HEAD:` / the gwz-core tree at the locked pin. Tuple did not move.
**Date:** 2026-09-03
**Axis:** CONSISTENCY — the document against its controlling graph (internal contradictions; verbatim agreement with cited contracts; exactness of superseded-clause lists; satisfiability of its own acceptance/build-order sections; unstated impacts on documents it does not cite). Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — 1 P1 and 4 P2 findings block (plus 3 P3). I pre-commit to GO on a revision that resolves P1-1, P2-1, P2-2, P2-3, and P2-4 as specified.

---

## 0. Evidence base

Commands (cwd `/Users/owebeeone/limbo/gwz-dev`), start and end:

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `cc3635c8419e258ba51b7d8891ff3ed18c589d362aa7f9f77bfc27de6e7f2211`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read (object and controlling graph):

- Object: `dev-docs/GwzM5-8M5d-Charter.md` in full (204 lines).
- `dev-docs/GwzM5-8I2CompatibilityContract.md` §1 (registry / “A1 compiles body decoders … v0 and v1”), §2 (creation matrix, dated 2026-08-25 annotation, “Open v0 records may use only the explicit A1 migration path”), §3 (pinned codes), §4 (open-v0 decoder / whitelist / valid-unlisted), §5 (migration eligibility; mutating-v0 lifecycle; archived-v0 archival), §7 (archive decoder), §8 (retained-reader window; “A1 supports v0 through its adapter”), §9 (A1 activates floor + eligible v0 migration + no-ff as one reviewed change).
- `dev-docs/GwzM5-8A1ActivationRecord.md` §2 (M5c residual: ordinary stays v0; floor raise WITH M5c as one reviewed change; retained-reader ordinary=v0 until M5c).
- `dev-docs/GwzM5-8DR1-WarnOrRefuse-Charter.md` §0.1, §3.1 (ordinary/ff-only untouched in ship 1; `--filesystem-strict` on the v1 door only), §3.9, §9 (ship 2 = M5c).
- `.cursor/rules/transactional-apis.mdc` (recover() required where mutation/recovery/identity matter).
- `dev-docs/AgentProcessRules.md` L1-08, L1-09, L1-33, §7.1–§7.2 (amendment / supersession formulas).
- `dev-docs/GwzM5-8M5c-Charter.md`: **banner only** to test the supersede claim, plus the sections this object itself cites (`§2.2`–`§2.6`, `§2.5`) so those citations could be checked verbatim. HEAD banner is still `CHARTERED … awaiting the operator's read`. Working-tree banner is already `SUPERSEDED 2026-09-03 by GwzM5-8M5d-Charter.md`; body after the banner still opens “The operator's decisions that bind this charter”.
- Live gwz-core at `57502e4`: `workspace_ops/merge/model/version.rs:39` `ACTIVE_WRITER_FLOOR = RecordVersion::V0`; `RequestedSemantics::Ordinary => Ok(RecordVersion::V0)` (`version.rs:70`); `start.rs:68–117` (dry-run before fork; V1 only then calls `v1.start` with `filesystem_strict`; V0 falls through to `persist_merge_record`); `runtime/dispatch.rs:330–377` (open v1 → v1 service; `adapt` migration; else v0 lifecycle); `validate.rs:58–71` (`--filesystem-strict` accepted on a v0 start and has **no effect**); `v1_lifecycle/start.rs:87–97` (the only `crash_recovery_decision` / strict-refuse door); `merge/mod.rs:86–90` (`V0_PERSISTENCE_SEAM_FLOOR` re-exports); `store/mod.rs:463` `write_atomic_verified`; `scripts/checks/check_checked_artifact_boundaries.py` F-3 (`V0_PERSISTENCE_SEAM_FLOOR`, lines 346–387 / 1487–1505) and `CAPABILITY_FREE_RAW_WRITER_INVENTORY` (lines 472–573, key-set digest fires on row add/delete/swap); `scripts/checks/merge_docs_manifest.json:331–332` pin `merge_command_no_ff_writes_v1_while_ordinary_stays_v0`; `scripts/retained_readers/manifest.json` decode generation `v0-v1-dual-decode-v0-writer-floor` (v0.11.0: dual decode + whitelist migration + ordinary=v0, residual owned by M5c).
- `dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md` §3 (growth fires / shrinkage fires; carved SET is the amendment’s).

Not read (out of scope): `dev-docs/GwzM5-8M5d-Review.md`; `GwxMergeFindingsDraft.md`; `GwzAiReqDd.md`; `GwzLocalCloneDesign.md` and local-clone reviews. No tests run.

---

## 1. Findings

### [P1-1] §9 step (1) authorizes open-v0 refuse before the floor, while this binary still writes v0 on every ordinary start

**Location.** Object §9 item 1 (lines 194–195): “Compatibility amendment + open-v0 refuse (can land before the floor; `--no-ff` already v1).” Contradicted by object §7 (lines 174–176): “Floor raise + v0 erasure + raw create + the §4 surfaces land as **one reviewed change**.” Live tree at the locked gwz-core pin: `ACTIVE_WRITER_FLOOR = RecordVersion::V0` (`version.rs:39`); ordinary/custom/ff-only select `RequestedSemantics::Ordinary → V0` (`version.rs:70, 81–85`); `start.rs:95–119` only threads `filesystem_strict` into `v1.start` when the floor/max has already chosen V1, otherwise `persist_merge_record` (v0 engine). `dispatch.rs:349–375` still routes a discovered v0 envelope through `adapt` (migration) or the v0 continue/abort lifecycle.

**Violated invariant.** A sequential script the charter itself says must not be reinvented at implement time may not land a rule that every discovered v0 envelope is refused (`§2`, lines 57–73: no decode, no continue/abort/migrate, no whitelist, no “valid unlisted stays on the v0 lifecycle”) while the same binary’s ordinary start still **creates** that envelope. The parenthetical safety argument (`--no-ff` already v1) is false for the path people use. This is also an internal contradiction with §7’s one-reviewed-change discipline (M5c §2.5’s transferred rule, which this object cites).

**Reproduction.** Execute §9 as written, landing (1) without (4):

1. Wire `discover_open` / `classify_open_record` / every merge verb to emit the §2 sentence on a v0 envelope (the step-(1) refuse).
2. Leave `ACTIVE_WRITER_FLOOR` at `V0` (live; step (4) has not landed).
3. `gwz merge <source>` (ordinary). `select_record_version(Ordinary)` returns `V0`. `start.rs` does not enter `v1.start`. A v0 record is published and, on conflict, left open.
4. `gwz merge --continue` (or abort, status, stage). The just-written envelope is v0, so step (1) refuses: “this is a pre-0.14 merge; use a 0.13.x build to continue or abort.”

The same binary that created the record cannot continue it. A 0.13.x build is the only escape — for a record this 0.14-in-progress tree just wrote.

**Impact.** Any independently-landable (1) strands in-progress ordinary merges. The charter’s “can land before the floor” is an example-order hole with an unrecoverable continue/abort, not a safe document-only sequencing note. `--filesystem-strict` on that ordinary start remains a no-op (`validate.rs:58–63`) until (4), so §8’s “`--filesystem-strict` refuses before writes” is also false in the (1)…(4) interval.

**Required correction.** Delete “can land before the floor” and the `--no-ff` already-v1 justification. Bind open-v0 refuse to the floor raise: (1) may draft the I2 amendment text, but the refuse **must not** land in production before (4). Restate §7 and §9 so they agree: v0 erasure and `ACTIVE_WRITER_FLOOR = V1` are one reviewed change. If internal plumbing of the refuse helper is wanted early, name it test-only / unreachable from production dispatch until (4).

**Closure test.** A reader of §9 cannot construct a production landing of the §2 refuse while `version.rs:39` is still `V0`. A walk of ordinary start → open v0 on disk → continue, on any tree that has landed (1) and not (4), is either impossible by the text or fails this review.

---

### [P2-1] The I2 “amendment” is declared, not exact — L1-08 (section wholes, surviving §5 archive sentences unlisted, live sentences outside §4–§5 unlisted)

**Location.** Object §2 (lines 80–84) and §7 table row “`GwzM5-8I2CompatibilityContract.md` §4–§5 | **amend** (open v0 → refuse; no whitelist; no v0 lifecycle).” The paraphrase in quotes at §2 line 82 is not a clause of I2. I2 §5:180 is: “An existing mutating v0 command remains on the existing v0 lifecycle and may write v0 only when that path’s existing preflight authorizes it.” Same paragraph, I2 §5:181–182, which this object needs to **keep**: “Completed and aborted v0 records remain v0 and use byte-preserving archival. Archived v0 uses only the archive decoder/projection below.”

**Violated invariant.** L1-08: a bounded amendment “names the superseded text, states the new controlling rule, explains why, lists affected interfaces and tests.” AgentProcessRules §7.1: an amendment names **exact superseded clauses**. Naming “§4–§5” as wholes is not that. I2 sentences **outside** the claimed scope still require the killed behavior, verbatim:

| Live I2 sentence (HEAD) | Why it still binds if only §4–§5 are “amended” |
| --- | --- |
| §1:36 “A1 compiles body decoders and canonical lifecycle support only for v0 and v1.” | Open-v0 **body** decoder and v0 **lifecycle** are what §2 deletes; archive decoder is what §2 keeps. The sentence cannot stay as written. |
| §2:102–103 “Open v0 records may use only the explicit A1 migration path.” | Directly contradicts §2 refuse / no migrate. Not in the amend list. The 2026-08-25 annotation close (§7 table) does not touch this frozen sentence. |
| §5:181–182 archived-v0 archival (above) | Sits inside the section declared amended-as-a-whole; an implementer of a §4–§5 rewrite can delete the archive sentences this charter’s default depends on. |
| §8:289 “A1 supports v0 through its adapter and installed v1.” | 0.14 is a new durable decode generation (I2 §8 first bullet: retain latest published pre-change release per materially distinct decode generation). Killing open-v0 decode/migration is that generation. Not listed. |
| §9:304–305 “A1 alone activates the v1 writer floor, eligible v0 migration, and public no-ff surface in one reviewed change.” | Eligible v0 migration is still live A1; this charter deletes it. Not listed. |
| I2 preamble: `gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` is the checked whitelist registry | Affected interface; not listed. §5 names g23 / `version.rs` / docs / retained-reader, not this registry or its checker. |

This is not a silent reinterpretation of the *fact* of amendment (the object does say “amends”). It is L1-08 inexactness: a later I2 patch cannot tell which sentences die, which survive, and which neighboring sections must move.

**Reproduction.** Draft the I2 patch this charter authorizes. Keep every I2 sentence not named as superseded. The resulting I2 still contains §2:102–103 (migration path), §1:36 (v0 lifecycle support), §8:289 (adapter), §9:304–305 (eligible migration). Those sentences and §2 refuse cannot both be true. Alternatively, rewrite §4–§5 as wholes and drop §5:181–182: archived-v0 projection (object §2:75–78, I2 §7, R4b normal-build archive decoder) loses its compatibility-contract home.

**Impact.** Dual review of “that amendment with this charter” cannot freeze a compatibility rule the implementer can apply. Whitelist deletion, archive-projection retention, and refuse-without-decode will be re-litigated at I2-patch time — the class of defect L1-08 exists to prevent.

**Required correction.** Attach (or fold in) a bounded amendment block that (a) quotes each superseded I2 sentence, (b) quotes each **retained** sentence inside §4–§5 (archive archival / I2 §7 pointer), (c) lists §1:36, §2:102–103, the dated §2 annotation close, §8:289 plus the new 0.14 decode-generation obligation, §9:304–305, and the predicates registry as in-scope, (d) names affected checkers/tests (compatibility checker, `adapt_open` suite, retained-reader fixture for open v0). Give I2 the §7.2 “Amended \<date\> by …” banner trail, not only a table row in this charter.

**Closure test.** Every I2 sentence that would contradict §2 refuse is either quoted as superseded or explicitly retained. An implementer can produce the I2 diff from that list without inventing scope. The predicates JSON and I2 §8 decode-generation add are on the affected-interface list.

---

### [P2-2] §8 “No production v0 merge type remains” is unsatisfiable with §2’s archived-v0 projection

**Location.** Object §8 (lines 186–187): “An open v0 file yields **only** the §2 sentence. No production v0 merge type remains.” Object §2 (lines 75–78): “**Archived `done/` v0** (completed or aborted): **read-only archive projection only**.” I2 §7 (and the R4b banner on I2: archived-v0 decoder is normal-build, consumed by ordinary status and GC) still requires a production v0 archive decoder. Object §1 (lines 38–39) is the narrower, coherent claim: “The v0 start / continue / abort / migrate **engine** is **not in the binary**.”

**Violated invariant.** An acceptance criterion must be satisfiable together with the rest of the same document. `RecordVersion::V0` / `gwz.merge-operation/v0` is the production merge-record type the archive decoder names. Deleting that type to satisfy §8 breaks §2 archive projection and I2 §7. Keeping the type fails §8 as written.

**Reproduction.** Implementer A deletes `RecordVersion::V0` and the v0 archive decoder to pass §8’s last sentence; archived `done/` v0 becomes a silent hole, violating §2’s default. Implementer B keeps the I2 §7 archive decoder; a production v0 type remains, and §8’s acceptance line is false.

**Impact.** §8 cannot be used as a close checklist. Status/GC of historical v0 archives vs “erase v0 from main HEAD” will be re-argued at landing.

**Required correction.** Replace “No production v0 merge type remains” with an acceptance line that matches §1/§2: no production v0 **lifecycle engine** (start/continue/abort/migrate/`open_v0`/`adapt`); open v0 yields only the §2 sentence; archived v0 remains I2 §7 projection (or, if the operator wants unread archives, say so as the one-line amendment §2 already contemplates). Do not use “merge type” for both the engine and the archive envelope.

**Closure test.** §8 can be checked off without deleting the archived-v0 decoder, and cannot be read as requiring that deletion.

---

### [P2-3] “Supersedes M5c in full” is inexact: the same object keeps M5c §2.2–§2.4, and the only effecting stamp leaves the M5c body looking live

**Location.** Object banner (lines 4–5): “**Supersedes** `GwzM5-8M5c-Charter.md` in full.” Object §1 (line 43) and §4: events / responses / `gwz stage` “match today’s v0 *shape* (**M5c §2.2–2.4, kept**).” Object §7 table: M5c is “**superseded**; do not implement.” AgentProcessRules §7.2 required formula: `Status: **superseded for <exact scope> by <NewDocument.md> as of <date>. Historical evidence and already-completed gates remain valid only where the new document says they do**.`

HEAD M5c banner (committed): still `CHARTERED 2026-09-03 … awaiting the operator's read`. Working-tree M5c banner (adjacent dirt, read only to test the claim): `SUPERSEDED 2026-09-03 by GwzM5-8M5d-Charter.md. Do not implement this file.` Immediately under that banner, M5c §0 still titles itself “The operator's decisions that **bind this charter**”; §2–§6 (design, phases, acceptance, open decisions) are unmarked historical.

**Violated invariant.** Full supersession and “kept” controlling sections cannot both be true. M5d §4 restates those surfaces **more thinly** than M5c §2.2 (M5c: `member_started` **before observation/apply**, `merge_member_finished` with `MergeRepoSummary`, same ordering discipline, same per-participant count on the same fixture; M5d: kinds only, “message text need not match”). If M5c is superseded in full, those details die. If they are “kept,” M5c is superseded **except** §2.2–2.4 (and §2.1’s raw-write mechanics, which §3 silently re-homes). §7.2 forbids a banner that contradicts a still-binding body.

**Reproduction.** An implementer of event parity reads M5d §4 (kinds only) and M5c §2.2 (ordering + `MergeRepoSummary` + counts), both “kept.” The two checklists disagree. A later reader of working-tree M5c sees SUPERSEDED on line 3 and binding operator decisions on line 11.

**Impact.** Ship-2 event/response/`stage` acceptance is not a single authority. The supersede claim cannot be executed with the §7.2 trail the process requires.

**Required correction.** Scope the supersession: e.g. “superseded for all ship-2 design, including leftover-v0 reader/lifecycle; event/response/`stage`/`raw-create` requirements are **restated in this charter §3–§4** and those restatements control (M5c is historical citation only).” Stop saying “kept.” Require the M5c status line to use the §7.2 formula, and a one-line note that §0–§6 below are not current authority. Do not leave “bind this charter” standing under a SUPERSEDED banner.

**Closure test.** No current sentence treats a M5c section as controlling. M5d’s restatement is complete enough that dropping M5c would not change ship-2 event/response/`stage`/raw-create meaning. M5c’s status matches §7.2.

---

### [P2-4] v0-engine deletion is unsatisfiable against two fail-closed inventories this charter does not cite

**Location.** Object §5 (lines 143–148): production loses the v0 lifecycle; g23 v0-engine suites “leave production (delete or move to a fixture that only proves the §2 refuse).” Object §7 table does not list `GwzM5-8R2E-CapabilityFreeAmendment.md` or F-3’s seam floor. Live pins at gwz-core `57502e4`:

- `CAPABILITY_FREE_RAW_WRITER_INVENTORY` carved **production** rows for the v0 engine: `workspace_ops/merge/finalize.rs`, `abort/evidence.rs`, `abort/preflight.rs`, `preserve/artifacts.rs`, `store/archived.rs` (checker lines 554–558). Key-set SHA-256 `867c580f…` (lines 566–573): “a row added, DELETED or swapped. It is the amendment’s, not a checker edit: revise `GwzM5-8R2E-CapabilityFreeAmendment.md` §3.” Amendment §3: shrinkage fires — a carved arm may not vanish without revising that amendment.
- F-3 `V0_PERSISTENCE_SEAM_FLOOR` (`check_checked_artifact_boundaries.py:378–387`) requires `merge/mod.rs` to keep re-exporting `FileMergeStore`, `MergeStore`, `archive_merge_record`, `enter_finalizing`, `persist_merge_record`, `persist_operation_transition`. Missing any token: “v0 persistence seam inventory is underivable.” Those symbols **are** the v0 engine seam `start.rs:119` and `dispatch.rs` v0 arms use.

Object §4 (lines 137–140) correctly plans relocating `write_atomic_verified` so `v1_lifecycle/` still does not **name** the seam. That is a different pin. Deleting the seam’s production callers/re-exports is not covered.

**Violated invariant.** A charter that authorizes deleting a production engine must name every fail-closed inventory that currently requires that engine’s files and symbols to exist. Unstated impact on CapabilityFree (“sets only shrink”) and on F-3’s derivable seam floor.

**Reproduction.** Delete or `cfg(test)`-move `finalize.rs` / v0 abort / `store/archived.rs` as §5 allows. The capability-free key-set digest exits non-zero until the amendment is revised. Remove `persist_merge_record` from `merge/mod.rs`’s `use store::{…}` because nothing production calls it: F-3 reports the seam underivable. Both gates are red on a tree that did only what §5 says.

**Impact.** The close described in §5/§8 cannot land. The missing work is not “flip pins”; it is an amendment-tier revision of CapabilityFree §3 plus a redesign of F-3’s floor (the scan today **requires the v0 seam to exist** so it can prove `v1_lifecycle/` does not name it).

**Required correction.** List as affected, with dispositions: (1) CapabilityFreeAmendment §3 / `CAPABILITY_FREE_RAW_WRITER_INVENTORY` — revise the carved set when v0-engine files leave production (shrinkage is an amendment, not a checker edit); (2) F-3 — redefine the seam floor for a tree whose production v0 persistence re-exports are gone (or keep a deliberately empty/test-only seam module the floor can still derive). Do not claim F-3 is handled solely by relocating `write_atomic_verified`.

**Closure test.** §7’s disposition table names both pins. A hypothetical deletion of `persist_merge_record` and `finalize.rs` is either forbidden by this charter or paired with the named amendment/checker change that keeps both gates green.

---

### [P3-1] §1 “the merge starts and finishes” on odd volumes overclaims §3’s reverse-door refuse

**Location.** Object §0 (lines 28–31): “(i) [stated limit that abort on overlay may newly refuse, cured later] is a no-go as a *product* story for start.” Object §1 (lines 46–48): on an odd volume “the merge **starts and finishes**.” Object §3 (lines 109–114): if `--preserve` / root-artifact abort doors hit the handle probe, “**refuse with the existing boundary sentence**.”

**Violated invariant.** The operator barred (i) as a product story **for start**, and barred reverse-path-raw as a milestone. §3 correctly refuses to build that milestone. §1’s “starts and finishes” still tells the start story as if abort parked-doors cannot newly refuse — which is (i).

**Impact.** Bounded: start/raw-create is the product; parked abort doors are a disclosed limit. As written, §1 and §3 disagree about what “finishes” includes.

**Required correction.** In §1, “starts and can complete the forward path; reverse doors that need handles still refuse with the boundary sentence (§3).” Do not say “finishes” without that qualifier.

**Closure test.** §1 and §3 name the same abort-door limit.

---

### [P3-2] Ship (1) “unchanged” is scoped to warn/strict/bar, but the landed DR-1 charter and A1 record still name M5c as ship 2 / residual owner

**Location.** Object §7: “Ship (1) charter | unchanged (warn / strict / identity bar).” DR-1 charter (HEAD) line 4: “Ship (2) of two; ship (2) is M5c”; §9:401: “Ship (2) = M5c is chartered next.” A1 record §2: M5c owns the floor raise and ordinary-start surfaces; retained-reader ordinary=v0 “until M5c lands.”

**Violated invariant.** Unstated impact on documents this charter needs later readers to stop treating as current for ship 2. The parenthetical saves the **mechanism** claim; it does not retarget the ship-2 pointer.

**Impact.** A later agent opening DR-1 or the A1 record still starts M5c.

**Required correction.** One disposition row: DR-1 ship-2 pointer and A1 §2 carrier name retarget to M5d (dated note; do not rewrite A1’s historical landing). The v0.11.0 retained-reader **generation description** may stay (it describes 0.11.0); the “owned by M5c” residual sentence needs a dated successor.

**Closure test.** No current ship-2 pointer names M5c as the document to implement.

---

### [P3-3] Odd-volume “not a recover() grammar” does not name `.cursor/rules/transactional-apis.mdc` as excepted

**Location.** Object §1 (lines 50–52) and §3 (lines 107–108): crash recovery on handle-less volumes is not a product; do not build `recover()` of a half-written raw record. Standing rule: when an API covers mutation, recovery, publication, or identity — “One **commit point** and a **recover()** that is a function of recorded intent plus observed state.” Merge is that API. Operator §0 explicitly accepts the 1/100000 crash on odd FS.

**Violated invariant.** A standing always-apply rule and this charter bind an implementer in opposite directions unless the exception is named.

**Impact.** Bounded: the operator text is the product authority here; an implementer who also obeys the always-apply rule will try to build the recover() this charter forbids.

**Required correction.** In §7, one row: transactional-apis.mdc — excepted for the uncatalogued/handle-fail arm only, per operator §0; above-bar path unchanged.

**Closure test.** The exception is in the disposition table with that scope.

---

## 2. Invariant analysis

Attacks that **failed** (the GO case would rest on these as well as on closing §1):

- **Ship (1) warn / strict / identity bar as a mechanism.** Live `v1_lifecycle/start.rs:87–97` is the only `crash_recovery_decision` / `filesystem_strict` refuse door. Ordinary starts do not consult it today (`validate.rs:58–63`; `start.rs` v0 fallthrough). Raising the floor routes ordinary through that same door. “Ship (1) unchanged (warn / strict / identity bar)” is true of the v1 door. The defect is §9(1) leaving ordinary on the v0 fallthrough after refuse lands (P1-1), not a silent rewrite of ship (1)’s bar.
- **I2 creation-matrix A1 row.** Frozen table already has `ordinary/custom = v1`. Closing the 2026-08-25 “floor still V0” annotation is the right move for that row. The remaining §2 frozen sentence is the migration-path line (P2-1), not the table.
- **F-3 relocate of `write_atomic_verified`.** The primitive lives in `store/mod.rs:463` (uses `rename_durable`). Moving it to a neutral module that `entry.rs` calls, with `v1_lifecycle/` still not naming `persist_merge_record` et al., matches F-3’s **name-the-seam** scan. That plan is consistent. The unstated failure is deleting the seam the floor still requires (P2-4).
- **§2’s own open-vs-archive split.** Two paragraphs: open v0 → refuse without body decode; archived v0 → I2 §7 projection. Internally coherent. The contradiction is only with §8’s “no production v0 merge type” (P2-2).
- **Docs-manifest ordinary=v0 pin.** Object §5 names `merge_docs_manifest.json` / `merge.md` “ordinary = v1.” The live pin `merge_command_no_ff_writes_v1_while_ordinary_stays_v0` (`merge_docs_manifest.json:331–332`) is therefore a stated flip, not an unstated impact. Assertion-id rename is implementer mechanical work.
- **I2 §8 old-reader vs new v1.** “Old binaries vs new v1 records unchanged in direction” matches I2 §8 (reject before mutation on retained decode generations). Stated.
- **Dry-run under floor V1.** Live `start.rs:68–70` intercepts dry-run before the version fork. §4’s pin is satisfiable at (4).
- **§9 items (2) and (3) before (4).** Event/response/`stage` parity on `--no-ff` now, and raw-create plumbing in `entry.rs` before the floor, are the same shape M5c used and do not by themselves refuse self-created v0. The lethal step is (1)’s production refuse (P1-1).
- **I2 §4 whitelist / valid-unlisted.** The object does **not** silently pretend those clauses already mean refuse. It says it amends them. The remaining defect is exactness (P2-1), not a hidden reinterpretation that never utters “amend.”

`ACTIVE_WRITER_FLOOR` at the locked pin is `V0`. Ordinary starts still write v0. That is the fact §9 step (1) is not allowed to ignore.

---

## 3. Risks and next action

Residual below the bar: protocol `ErrorCode` for the §2 sentence is unnamed (reuse of `UnsupportedLegacyMode` would be the wrong I2 §3 allocation; not filed as P2 because the object does not claim a new wire field and additive protocol is explicitly out of scope). `RequestedSemantics::Ordinary => V0` remains correct under a V1 floor (`max`), so it is not a hidden second floor.

**Next action:** remediate P1-1 and P2-1…P2-4 in one text patch of this charter (and, for P2-1/P2-3, the I2 amendment block + M5c §7.2 banner). Do not implement, do not cut 0.14.0, do not start M6. Re-verdict this axis on the corrected bytes of `GwzM5-8M5d-Charter.md` at a new SHA-256.
