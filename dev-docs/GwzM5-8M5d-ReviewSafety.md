# GwzM5-8M5d — SAFETY-AXIS REVIEW

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `cc3635c8419e258ba51b7d8891ff3ed18c589d362aa7f9f77bfc27de6e7f2211`). Controlling DRAFT is the object itself.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98`; gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c` (lock pin, recorded at review start). End-of-review `git rev-parse HEAD`, `shasum -a 256`, and `git -C gwz-core rev-parse HEAD` matched the start tuple; the object did not move.
**Date:** 2026-09-03
**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; disclosure/privacy scale; stuck states reachable under the text’s own rules; whether “never worse than the status quo” survives concrete interleavings; scope creep that widens blast radius. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — three P1 and two P2 findings block. The text lets 0.14 start a merge it cannot abort on handle-less volumes, lets open-v0 refuse land while this binary still writes v0, and deletes the only in-binary abort of leftover v0 without pinning the 0.13.x escape.

I pre-commit to GO on a revision that resolves P1-1, P1-2, P1-3, P2-1, and P2-2 as specified.

---

## 0. Evidence base

Tuple (start and end, cwd `/Users/owebeeone/limbo/gwz-dev`):

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `cc3635c8419e258ba51b7d8891ff3ed18c589d362aa7f9f77bfc27de6e7f2211`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read (cited below by line):

- Object: `dev-docs/GwzM5-8M5d-Charter.md` (full text, 203 lines)
- `.cursor/rules/transactional-apis.mdc` (mutation / recover() / identity)
- `dev-docs/GwzM5-8I2CompatibilityContract.md` §2 (writer floor, 2026-08-25 residual), §4–§5 (open v0 lifecycle / whitelist — the clauses this charter amends), §7 (archive decoder / GC), §8 (retained readers)
- `dev-docs/GwzM5-8DR1-WarnOrRefuse-Charter.md` §0.1, §3.1, §3.6, §4.1, §9 (ship (1) live; overlay handle-fail measured; ordinary starts untouched until ship (2); `--filesystem-strict` start-only)
- `dev-docs/GwzM5-8M5c-Charter.md` **status banner only** (lines 1–6: SUPERSEDED 2026-09-03). M5d’s own citation of “M5c §2.6 (ii)” is taken from the object, not from the superseded body.
- Live gwz-core at the pin: `model/version.rs` (`ACTIVE_WRITER_FLOOR = V0`); `merge/start.rs` (v0 persist vs v1 owner); `v1_lifecycle/start.rs`; `v1_lifecycle/checked.rs` (capability-free abort lease; reverse doors still probe); `v1_lifecycle/service.rs`; `v1_lifecycle/store/rewrite.rs` (`create_open` → `create_merge_store_record`; `commit` raw); `checked_artifact/entry.rs` (`crash_recovery_decision`, `create_merge_store_record`, `root_artifact` / `prepare_parent`); `checked_artifact/observation.rs:82-121` (`acquire` → `durable_identity`); `checked_artifact/identity.rs:312-346` (`name_to_handle_at`); `checked_artifact/capability.rs:73-110` (abort clause, scoped by path); `merge/store/mod.rs` (`discover_open` body-decodes v0; `write_atomic_verified`; `classify_open_record`); `merge/store/gc.rs`; `merge/validate.rs:58-71` (strict flag no-op on v0 start); `merge/runtime/dispatch.rs`; `merge/runtime/mutation_guard.rs` / `open_gate.rs`; `workspace_ops/handle_stage.rs:34-36`; `merge/abort/evidence.rs:664-719` (v0 evidence rollback **raw**); `merge/root/artifact_facts.rs:39-50` and `entry.rs:36-50` (v1 root rollback **checked**); `merge/preserve/checked_bundle.rs`; `merge/v1_lifecycle/reverse/execute/rollback.rs`
- `dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md` lines 24-31, 63-71 (ordinary merge / abort / GC stay capability-free; converting a listed writer onto `CheckedArtifact::acquire` puts that operation on the probe)
- `dev-docs/CurrentProgramCheckpoint.md` around the 2026-09-03 v0.13.0 release note (tag exists) and the M5d charter note — used only to confirm that “0.13.x” names a published artifact today, not as a controlling design

Out of scope as instructed: `dev-docs/GwzM5-8M5d-Review.md`; `GwxMergeFindingsDraft.md`; `GwzAiReqDd.md`; local clone; production code change; writing a plan; cutting a tag. Targeted tests: NONE.

---

## 1. Findings

### [P1-1] §9 lets open-v0 refuse land while this binary still writes v0

**Root cause:** §9 step (1) authorizes “Compatibility amendment + open-v0 refuse (can land before the floor; `--no-ff` already v1).” §2 then applies that refuse to `discover_open` and every merge verb. Ordinary start still follows `ACTIVE_WRITER_FLOOR`, which is `V0` until step (4). The same main binary therefore creates leftover open v0 and then refuses to continue or abort it.

**Location:** Object §9 lines 194–201; §2 lines 57–73; §7 lines 174–176 (the contrary claim that floor raise + erasure + raw + §4 surfaces land as **one reviewed change**). Live: `version.rs:39` floor is `V0`; `start.rs:95-119` ordinary start persists v0 through `FileMergeStore::write_open`; continue/abort of that file is a merge verb under §2.

**Violated invariant:** A binary must not write a record it cannot drive. Contract §2: “no record is ever created at a version this binary cannot read back” (`version.rs:11-12`). Fail-closed occupancy of an open merge must still have an in-binary abort (capability-free list; `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY` abort clause).

**Reproduction:**

1. Land §9 step (1) on main. Do not land step (4). Floor remains `V0`.
2. `gwz merge <source>` (ordinary). Start writes `gwz.merge-operation/v0` and stops at conflict, exactly as today.
3. `gwz merge --continue` / `gwz merge --abort` on the same binary. §2: do not decode the body; emit only `this is a pre-0.14 merge; use a 0.13.x build to continue or abort`.
4. The v0 engine is still in the tree (step (4) has not deleted it). The message names 0.13.x. This binary *is* the post-0.13.0 main that just wrote the file.

**Impact:** In-progress ordinary merges on main become stuck in the same process that opened them. Git `MERGE_HEAD` / indexes remain. The typed escape is a different tag, not the still-present v0 lifecycle. This is a crash window between landings that §7 pretends does not exist. `--filesystem-strict` stays a no-op on those ordinary starts (`validate.rs:58-63`) while §1 already describes it as the start-only refuse — mixed flag semantics in the same window.

**Required correction:** Strike “can land before the floor.” Open-v0 refuse, floor raise to `V1`, and deletion of the v0 start/continue/abort engine flip in the **same** reviewed change as §7 already requires. Until that commit, mutating v0 commands remain on the v0 lifecycle (today’s I2 §5 rule). `--filesystem-strict` is live on ordinary starts only in that same commit.

**Closure test:** A sentence in §7/§9 that forbids publishing the §2 refuse while `ACTIVE_WRITER_FLOOR` is `V0`. A named counterexample: after any authorized intermediate landing, `gwz merge <source>` then `gwz merge --abort` on that same tree still aborts a v0 ordinary record.

---

### [P1-2] Handle-less start succeeds; reverse-door abort refuses; 0.13 cannot abort the v1 record

**Root cause:** §1/§3 require ordinary start to run on overlay/sshfs via raw `write_atomic_verified` when the handle probe fails. §3 then forbids reverse-path raw as a phase and tells those abort doors to **refuse** with the existing boundary sentence. After the floor raise the record is v1, so the 0.13.x escape in §2 does not apply.

**Location:** Object §0 lines 28–31; §1 lines 46–52; §3 lines 105–114; §8 line 189; §9 line 202. Live: W3 measured overlay without `nfs_export` as `name_to_handle_at` `EOPNOTSUPP` (DR-1 charter §9). Today `--no-ff` dies at `create_merge_store_record` (`entry.rs:541-560` → `CheckedArtifact::acquire` → `durable_identity` / `name_to_handle_at`). Ordinary v0 start and abort do **not**: `FileMergeStore::write_open` uses `write_atomic_verified` (`store/mod.rs:146, 463-498`); v0 evidence rollback is a documented raw carve-out (`abort/evidence.rs:664-719`). v1 reverse doors are checked: `root/artifact_facts.rs:39-50` → `replace_merge_root_artifact` → `root_artifact` → `acquire`; `--preserve` via `preserve/checked_bundle.rs`; `service.rs:124-130` and `checked.rs:134-138` state the residual. `rewrite.rs:commit` is already raw; the refuse is the *re-verify*, not the record rewrite.

**Violated invariant:** Ordinary merge / abort stay capability-free except the path-scoped residual, and that residual must not make overlay newly worse than the v0 status quo the operator ordered not to regress on start (`CapabilityFreeAmendment` Ground 2; DR-1 §0.1 “ordinary merge must not newly refuse overlay/sshfs”; `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY` still names `gwz merge --abort` as the escape). A refuse that names abort as the escape, then refuses abort, is circular.

**Reproduction:**

1. Workspace on overlay without `nfs_export` (the measured W3 row).
2. M5d close: `gwz merge <source>` ordinary. Handle probe fails; raw create; merge runs; one warning.
3. The attempt includes `@root` and has published evidence, **or** the user passes `--abort --preserve`. (Plain participant-only abort is not this finding; `checked.rs:132-133` and the object’s “plain abort already works” hold for that subset.)
4. `gwz merge --abort` takes the capability-free lease, then `observe_merge_root_artifact` / `write_checked` / bundle classify calls `CheckedArtifact::acquire`. `name_to_handle_at` fails. Refuse with the boundary sentence, which tells the user to abort.
5. Open v1 record remains. Git state remains. A 0.13.0 binary sees envelope v1, routes to `handle_v1_command` / reverse service, and hits the same doors. §2’s “use 0.13.x” is not an escape here.
6. Status quo on the same overlay: that merge was v0; abort restored lock/marker/boundary with `artifact::write_atomic` and persisted with `write_atomic_verified`. It cleared.

**Impact:** Reachable stuck merge, worse than status quo, with no in-binary and no 0.13.x abort. Docker/CI overlay is not the “1/100000 system crash” the operator waived. Copying the tree onto APFS/ext4 is not stated. Hand repair of `MERGE_HEAD` is what §2 forbids the tool to guess, and the charter gives the operator no recipe either. `--filesystem-strict` does not help after start (`validate.rs`: flag is start-only; continue/abort must not consult it).

**Required correction:** One of these, named as the product, not “skip if not free”:

1. Mandatory, not optional: if start used raw create, the root-artifact and `--preserve` abort doors use the same `entry.rs` raw mode (the object already allows this when it is the same flag and no new coordinator — make that a shall, not a may).
2. Or: those starts still run, but the warning states that abort of a selected-root / `--preserve` / published-evidence attempt on this volume will refuse, and names a working escape that does not need handles or 0.13.x (exact git + delete-open-record procedure, or “copy the workspace to a handle-capable volume and abort”).
3. Do not use the current boundary sentence as the refuse text; it still advertises `--abort` as the escape.

Do not “fix” this by refusing start. The operator already called that a no-go as a product story for start.

**Closure test:** A charter sentence plus a named overlay row: ordinary start on handle-fail volume writes v1 raw and completes or stops; then (a) participant-only abort clears the record, and (b) selected-root or `--preserve` abort either clears via raw mode or prints the new escape and leaves Git/record in a state that escape actually clears. Repeating (b) with a 0.13.0 binary is not an accepted pass.

---

### [P1-3] Deleting the v0 engine leaves leftover open v0 abortable only by a 0.13.x binary the charter does not pin

**Root cause:** §2 makes open v0 not a merge. Escape is “a 0.13.x binary.” §5/§8 delete the v0 start/continue/abort engine from production. That deletion is irreversible in 0.14+. The text never states the preconditions of that irreversibility: 0.13.x remains a published, installable artifact; the user is told to abort leftover opens *before* upgrading; 0.14 names how to obtain 0.13.0. v0.13.0 is released today; nothing in this charter keeps that true when 0.14.0 is cut, and untagged main users get no release notes until after step (4).

**Location:** Object §0 lines 15–20; §2 lines 57–73; §5 lines 144–148; §8 lines 186–188; §9 lines 202–203 (release notes / tag *after* erasure). I2 §5 today: “An existing mutating v0 command remains on the existing v0 lifecycle.” Live `dispatch.rs:349-355`: v0 open records stay on the v0 route, which is what keeps leftover v0 abortable in this binary.

**Violated invariant:** An irreversible step names its preconditions and a still-working escape. Mixed-version leftover state is not worse than the status quo without a reachable abort. I2 §8: a same-version newly writable shape must reject before mutation on retained generations — 0.14 vs *open v0* may refuse, but the refuse is not an abort, and the named older generation must actually exist.

**Reproduction:**

1. On 0.13.0 or current main, start an ordinary merge; leave it open (conflict).
2. Install 0.14.0 built from this charter’s close. The v0 engine is gone.
3. `gwz merge --abort` / `--continue` / `--status` emit only the §2 sentence.
4. If the user cannot reinstall 0.13.0 (installer that only ships latest; air-gapped; yanked/unlisted crate; they no longer have the old binary), the open v0 file and Git merge state remain for the life of 0.14+. 0.14 “must not guess Git `MERGE_HEAD` / index repair” (§2 lines 72–73), so there is no in-band cleanup.
5. Independently of reinstall: `FileMergeStore.discover_open` today body-decodes v0 and feeds the mutation gate and `gwz stage` (`mutation_guard.rs:95-98`; `open_gate.rs:22-29`; `handle_stage.rs:34-36`). §2 says discover_open does **not** decode the body and does **not** project the attempt as a merge, but **does** emit the typed message. The charter never says what the gate and stage do with that. Two compositions, both bad: (i) `discover_open` errors → every gated mutation including conflict `gwz stage` freezes until 0.13.x; (ii) `discover_open` returns `None` to honour “not a merge” → `gwz commit` / `gwz add` proceed while `MERGE_HEAD` is set.

**Impact:** Leftover open files can remain forever if 0.13.x is gone. Even when 0.13.0 is still on GitHub, first-run 0.14 does not tell the user the tag URL or “abort before upgrade.” Composition (ii) is false occupancy of the open-merge gate (data-loss adjacent). Composition (i) is a workspace-wide freeze whose only key is an unpinned old binary. Status quo: this binary aborts leftover v0.

**Required correction:**

- Pin as a precondition of engine deletion: a published 0.13.x remains the retained-reader escape for leftover *open* v0 for as long as 0.14 is current; the §2 sentence names that tag (today: `v0.13.0`) and that it must be used to continue **or abort**.
- Require abort-or-finish of open v0 before upgrade in the 0.14 notes that §9 currently schedules *after* erasure — those notes are a precondition of the irreversible step, not a follow-on.
- Name a third occupancy state: envelope v0 is **not** a merge lifecycle and **is** an open operation. Mutation gate and non-merge verbs refuse with the §2 sentence (or `OpenOperation` plus that sentence). They must not return “no merge.” `gwz stage` under leftover v0 does not run the v1 body path and does not stage as if idle. Envelope classification does not decode the v0 body.

**Closure test:** Charter sentences for the three bullets. A named leftover-open-v0 fixture: 0.14 abort/continue/status/commit/add/stage all refuse without body decode and without writing; none of them behaves as “no merge.” A sentence that deleting the v0 engine is not authorized if 0.13.x is not a published installable artifact.

---

### [P2-1] “No production v0 merge type” is false composition with retained archive projection

**Root cause:** §1/§5/§8 delete the v0 engine and claim “No production v0 merge type remains.” §2 keeps archived `done/` v0 as “read-only archive projection only,” which I2 §7 implements by decoding the v0 body, computing gaps, and gating GC on that decode. Those are production types and production call paths. §5’s “g23 suites that are the v0 engine: leave production (delete or move…)” does not carve the archive decoder out of that deletion.

**Location:** Object §1 lines 38–39; §2 lines 75–78; §5 lines 144–148; §8 line 187. Controlling I2 §7 (archive decoder, `legacy_complete` / gaps, “Unsupported or corrupt archives are never deleted or rewritten”). Live: `decode_archived_common` still dispatches v0 (`record_wire/decode.rs:121-126`); `store/gc.rs:15-18` reads both envelopes before unlink; `FileMergeStore::discover_open` / `read_record` still `decode_production_v0`.

**Violated invariant:** Deletion of “the v0 merge” must not silently unhook archive status/GC (history hole, contradicting §2) and must not keep an open-v0 body decoder in those paths (contradicting §2 “does not decode the v0 body”). I2 §7 remains in force; this charter does not amend it.

**Reproduction:** Implementer follows §5 “delete the v0 engine” and removes `decode_production_v0` / the v0 archive arm. `gwz merge --status <id>` on a `done/*.yaml` v0 is unreadable (silent hole). `gwz merge --gc` sees unsupported/corrupt and, per I2 §7, never deletes — archives accumulate, unread. Alternatively they keep the decoder for GC and also leave it on `discover_open`, so open v0 is still a decoded merge type.

**Impact:** Either unread history (the thing §2 exists to avoid) or a remaining v0 merge type behind a slogan. Not active corruption if I2 §7’s “never delete unreadable” holds, but the charter’s deletion instruction will make one of the two happen unless it names the survivor.

**Required correction:** Split the types in §5/§8: the v0 *lifecycle* (start/continue/abort/migrate/open body) is deleted; the v0 *archive decoder and GC projection* (I2 §7) remain production code, consumed only for `done/` bytes, never for open files. Open v0 stops at envelope classification.

**Closure test:** A charter sentence that `decode` of archived v0 is not “the v0 engine.” A named row: open v0 fixture is refused at envelope without body decode; archived v0 fixture still projects under I2 §7 and is not a silent hole; GC still refuses to delete an unread/unreadable archive.

---

### [P2-2] One “reviewed change” still sequences crash windows for raw create, surfaces, and the flag

**Root cause:** §7 says floor + erasure + raw create + §4 surfaces land as one reviewed change. §9 then orders four steps, allows (2)∥(3), and only constrains (4) to follow (1)–(3). That is a publishable series on main: raw `create_merge_store_record` mode, event/stage parity, and (if P1-1 is not cured) refuse, each visible without the others. Coordinated workspace writes get a crash window between those landings that nobody dual-reviews as a tuple.

**Location:** Object §7 lines 174–176; §9 lines 190–203; §1 line 54 (`--filesystem-strict` as the refuse opt-in); §4 (surfaces that “stay”). Live: ordinary starts do not reach `handle_start_durable_v1` until the floor moves (`start.rs:95-117`); the flag is accepted and ignored on v0 starts (`validate.rs:58-63`); `create_open` is still always checked (`rewrite.rs:63`).

**Violated invariant:** transactional-apis.mdc: mutation/recovery/publication/identity expose one commit point, not “check then path-rename” as a series of main states. Dual on “the landing” is meaningless if main can sit between (3) and (4).

**Reproduction:** Land §9 step (3) (raw create in `entry.rs`) before step (4). `--no-ff` on overlay starts (new). Ordinary still v0 (old). `--filesystem-strict` still no-op on ordinary. Kill between landings: some volumes have v1 raw `--no-ff` records and v0 ordinary records; docs already claim the flag is a start-wide refuse. Land step (2) without (4): `gwz stage` is told to read a v1 body (`§4` line 132–134) while ordinary opens are still v0 — stage either misses the open merge or fails closed on the v0-only store (`handle_stage.rs:34` today).

**Impact:** Mixed-version *inside one untagged main*, not only 0.13 vs 0.14. Surfaces and the flag lie about ordinary starts. This is the same class of defect as P1-1 with a different root (step splitting vs refuse-before-floor). If P1-1’s correction already forces a single commit for refuse+floor+erasure, this finding still requires raw create and §4 surfaces in that same commit, matching §7.

**Required correction:** Restate §9 as implementation order *inside* one landing commit (or an un-published branch), not as landable main steps. The dual reviews the tuple after (1)–(4), not after (1) or (3) alone.

**Closure test:** §9 contains a sentence that (1)–(4) are not independently published to `main`. The dual’s object is one tree where ordinary start is v1, raw create is live, §4 surfaces are live, open v0 is the §2 refuse, and `--filesystem-strict` refuses a below-bar ordinary start before writes.

---

## 2. Invariant analysis

Attacks that **held** (findings above): mixed-version leftover v0 after decoder deletion; §9/§7 sequencing; handle-less start + reverse-door refuse vs v0 overlay abort; archive-decoder vs “no v0 type”; flag/surface crash windows.

Attacks that **failed** (and a GO would rest on):

- **Raw create + kill during write inventing progress.** `write_atomic_verified` (`store/mod.rs:463-498`) publishes via rename of a complete temp; `record_files` lists only `*.yaml`, not `*.yaml.{pid}.{seq}.tmp`. A kill before rename leaves no open record (start again). A kill after rename leaves a complete file `commit` can reload. Torn published bytes on a non-atomic FUSE rename decode as unreadable, not as a merge — fail-closed, matching the operator’s waiver of reboot-durable recovery on handle-less volumes. The charter’s “no `recover()` of a half-written raw record” does not authorize inventing progress. Not a finding.
- **Plain abort on overlay.** Participant-only reverse path takes `V1MutationLease::acquire` (no catalog) and `store.commit` (raw). That subset is not worse than status quo. P1-2 is scoped to root / `--preserve` / published-evidence re-verify.
- **`--no-ff` already v1.** Raising the floor with `max(floor, semantic)` is already proven (`version.rs:160-167`). Ordinary follows the floor; `--no-ff` does not need the refuse-before-floor window. Not a defect by itself.
- **Dry-run.** Object §4/§8: intercepted before the version fork; pin only. Live `start.rs:68-70` matches. Failed.
- **Privacy / disclosure scale.** Archive projection of completed v0 is the opposite of a new leak; §2’s “one-line amendment” to unread them later is opt-in. Open v0 is not projected as a merge. No finding.
- **Operator waiver of crash recovery on odd volumes.** Explicit in §0/§1. Does not waive live abort, leftover v0, or sequenced main landings.
- **0.13.0 presently exists.** Checkpoint records `v0.13.0` published 2026-09-03. P1-3 is that the charter does not *keep* that escape as a precondition of deletion, not that it is already gone.

`transactional-apis.mdc`’s recover() demand on odd volumes is the operator-accepted exception for *crash*, not a license for P1-2’s live abort refuse.

---

## 3. Risks and next action

Residual below the finding bar: sshfs remains unmeasured (DR-1 §9); Windows `require_ntfs` stays until a Windows-verified step (object §3, ship (1) unchanged); a below-bar workspace moved onto an above-bar volume still re-decides at continue (ship (1) §3.1, not reopened here); I2 retained-reader matrix must re-describe 0.14 vs open v0 as the §2 refuse (object §5 — direction is right; execution is implementation).

Next action: one bounded text patch on the charter addressing P1-1, P1-2, P1-3, P2-1, and P2-2. Do not implement, do not tag, do not split the landing on main until that revision is GO on this axis.
