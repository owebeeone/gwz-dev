# GwzM5-8M5d — SAFETY-AXIS REVIEW (round 4)

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` revision 4 (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `5ab008ea283efeb8e7e847ba137ef4640e6c163b3c269e94db811f1b6b39b23d`). Controlling DRAFT is the object itself. Prior GO was revision 3 SHA-256 `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`; the banner correctly states that SHA does not cover these bytes.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98`; gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c`. End-of-review `git rev-parse HEAD`, `shasum -a 256`, and `git -C gwz-core rev-parse HEAD` matched the start tuple; the object did not move.
**Date:** 2026-09-03
**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; disclosure/privacy scale; stuck states reachable under the text’s own rules; whether “never worse than the status quo” survives concrete interleavings; scope creep that widens blast radius. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — P2-2 is **reopened**; one new P2 (P2-3) on the decision-time handle probe. 0 P0 · 0 P1. P1-1, P1-2, P1-3, and P2-1 stay closed. No new architectural root cause; the cap does not fire.

I pre-commit to GO on a revision that resolves P2-2 and P2-3 as specified.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P1-1 | Production §2 refuse and floor `V1` never on `main` apart; before (4), ordinary start then `--abort` still aborts v0 | §7 lines 305–315 and §9 lines 344–363 still forbid publishing the refuse arm while the floor is `V0`. Header-only classifier may land; refuse may not. Original same-binary “create v0 then refuse abort” walk remains unconstructable | **CLOSED-stays** |
| P1-2 | Only (b) escape is copy onto a handle-capable volume then `gwz merge --abort` / `--preserve`; no in-place git+delete | §3 lines 187–197 unchanged in substance; in-place recipe still prohibited. Decision-time `handles_ok` only *when* the reverse-door sentence is appended (NFS/tmpfs with handles no longer get a false (b) warning) | **CLOSED-stays** |
| P1-3 | `v0.13.0` pin; third occupancy; gate/commit/add/stage refuse; not idle | Occupancy held. L-P3-3 *tightens* it: old “use merge status/continue/abort” remedy suppressed, including `merge/start.rs`. Sentence now names the 0.13.0 *release* (L-P3-4); tag remains the artifact pin | **CLOSED-stays** |
| P2-1 | No v0 lifecycle engine; I2 §7 archive decoder remains | §1 / §5 / §8 / §10.2 split unchanged | **CLOSED-stays** |
| P2-2 | (1)–(4) not independently published to `main`; dual object is one tree after all four | **Undone.** §7 lines 308–312 and §9 lines 341–376 make (2) and (3) landable `main` steps. Original walk (land §4 `gwz stage` v1-body / raw create while ordinary is still v0) is **constructable again** | **REOPENED** |

---

## Changed-range analysis

Revision 3 `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8` → revision 4 `5ab008ea283efeb8e7e847ba137ef4640e6c163b3c269e94db811f1b6b39b23d`. HEADs unchanged. Banner: operator-directed folds from an implementation-lane review (that file not read).

| Fold | What changed | Safety |
|---|---|---|
| L-P2-1 | `crash_recovery_decision` gains a directory-level handle probe on `.gwz`; `handles_ok`; reverse-door sentence only when false; NFS/tmpfs with handles excluded from (b) warning; create door consults the same result | New probe target vs live create door (workspace-root identity today, `entry.rs:302-323`). See P2-3 |
| L-P2-2 | Raw create, §4 surfaces, class-(ii) suite re-pointing **may land on `main` before the close**; close dual is floor+refuse+erasure+pins only | Reopens P2-2 |
| L-P3-1 | Additive `crash_recovery.handles_ok` | Machine form of (b) limit; no secrets; not a defect |
| L-P3-2 | F-3 ghost-seam option struck; floor = neutral module, only `entry.rs` calls it | Call-graph checker; not a runtime stuck state |
| L-P3-3 | Suppress false open-operation remedy on v0 envelopes, including start gate | Occupancy tighter; does not reopen P1-3 |
| L-P3-4 | User sentence names release 0.13.0, not the tag | Diagnosability; pin remains `v0.13.0` |

**New architectural root causes:** none. L-P2-2 is the **same** landable-series cause as round-1 P2-2, reintroduced after revision 3 closed it. L-P2-1 is an extra probe at the existing decision point, not a new coordinator/recover()/identity store. Cap does not fire.

---

## 0. Evidence base

Tuple (start and end, cwd `/Users/owebeeone/limbo/gwz-dev`):

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `5ab008ea283efeb8e7e847ba137ef4640e6c163b3c269e94db811f1b6b39b23d`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read: object revision 4 (full, 438 lines). Live pin (unchanged): `crash_recovery_decision` probes workspace admission on `root`, not `.gwz` (`entry.rs:302-323`); `handle_stage.rs` still `FileMergeStore.discover_open` then v0 body for conflict staging. Revision 3 GO report in prior context. Targeted tests: NONE.

Out of scope: `GwzM5-8M5d-ReviewImplementation.md`; `GwzM5-8M5d-Review.md`; local clone; production code; I2.md body; tagging.

---

## 1. Findings

### [P2-2] (reopened) §4 surfaces and raw create are landable on `main` while ordinary is still v0

**Root cause:** Revision 4 withdraws the revision-3 rule that (1)–(4) are not independently published to `main`. §4’s production `gwz stage` path is “v1 body” with open v0 defined as the §2 refuse — but the refuse is not live until (4). Landing (2) as written makes that §4 sentence true on `main` while ordinary opens are still v0 merges.

**Location:** §7 lines 305–312; §9 lines 341–376, especially 354–369; §4 lines 243–246 (“derive stage member paths from the **v1** body. (Open **v0** occupancy is §2 refuse, not this path.)”). Live: `handle_stage.rs:34-36` and `enforce_open_merge_stage_targets` body-decode v0.

**Violated invariant:** A published `main` must not be worse than today’s ordinary merge. Until (4), mutating v0 remains on the v0 lifecycle (§7, §1). Stage of a conflicted ordinary merge is part of that lifecycle. Dual on “the close” that assumes §4 is “already live” does not review the mixed tree (2)-on-main + floor still `V0`.

**Reproduction (original P2-2 walk, constructable again):**

1. Land §9 step (2) on `main` as authorized: event/response/`stage` parity per §4, class-(ii) suites re-pointed under a test-only `V1` override.
2. Do not land (4). `ACTIVE_WRITER_FLOOR` is still `V0`. Ordinary `gwz merge <source>` writes v0 and stops at conflict (today).
3. `gwz stage` of those conflicted members: production code that implements §4 literally discovers “version-agnostically” and reads a **v1** body. The open file is v0. Either the v0 merge is invisible (idle staging / commit path) or stage errors. Both are worse than status quo (`discover_open` + v0 participant paths).
4. Independently, land (3): overlay `--no-ff` starts (today it refuses at `create_merge_store_record`). Ordinary overlay remains v0 and abortable. The `--no-ff` overlay (b) door is the closed P1-2 product if the diagnostic ships with (3); it is not this finding. This finding is ordinary v0 + §4 stage.

**Impact:** Untagged `main` users (this workspace) can lose conflict staging of in-progress ordinary merges without any 0.14.0 close, notes, or `v0.13.0` story. That is a mixed-version *inside one branch*, not 0.13 vs 0.14. The close dual never sees it if (2) already landed.

**Required correction:** Keep (4) as the only production flip of ordinary-start version and of §2 refuse. If (2)/(3) may land on `main`, pin explicitly: until (4), production `discover_open` / `gwz stage` / ordinary persist **still serve open v0 as today’s merge**; §4 v1-body stage is `--no-ff` and/or `cfg(test)` floor override only. Do not implement §4’s “open v0 is §2 refuse, not this path” on `main` before (4).

**Closure test:** A charter sentence that a `main` tree with (2) and/or (3) landed and (4) not landed still stages a conflicted ordinary v0 merge as today. The round-1 walk must be unconstructable again.

---

### [P2-3] Decision-time handle probe of `.gwz` is unspecified when `.gwz` is absent

**Root cause:** L-P2-1 adds a directory-level handle probe “the create door's own probe applied to `.gwz` before any write” and sets `handles_ok` from it. First merge often has no `.gwz` yet (live start bootstraps `.gwz/merge` at the lease). The create door today takes `durable_identity` on the **workspace root** (`observation.rs` acquire), then traverses. Opening `.gwz` as a directory and mapping `ENOENT` to `handles_ok = false` is a permitted reading and is wrong on APFS/ext4.

**Location:** §3 lines 164–179, table row handle-fail; “Raw when false”. Live: `crash_recovery_decision` does not open `.gwz`; first merge `prepare_merge_start_parents_uncatalogued` / bootstrap creates `.gwz/merge`.

**Violated invariant:** Above-bar volumes must not take the handle-fail arm (raw create + reverse-door diagnostic). `handles_ok` false is only for volumes that will fail the create-door handle probe after parents exist. Missing private dir is not a filesystem capability gap.

**Reproduction:**

1. Fresh workspace on APFS/ext4, no `.gwz` yet. Identity probe on `root` is `Supported`.
2. Implement the probe as `name_to_handle_at` / acquire on path `.gwz`. `ENOENT` → `handles_ok = false` (not specified otherwise).
3. Start: catalog may still activate (`Supported`); create door “consults the same result (raw when false)” → **raw** record on the common path; Diagnostic appends the (b) reverse-door limit.
4. Selected-root abort on that same APFS volume: `CheckedArtifact::acquire` succeeds (parents now exist, handles work). The warning was false. Durability of the open record was the handle-fail primitive, not the checked create the table’s first row requires.

**Impact:** First merge on a normal volume is treated as overlay: weaker publication, false “abort may refuse; copy the workspace” product. Not stuck (abort actually works), but it is worse than status quo and it lies about (b). A create-door failure when `handles_ok` is true is already an anomaly; the inverse (false `handles_ok` on a handle-capable volume) is not called out.

**Required correction:** Learn `handles_ok` from the same directory the create door identities today (workspace root), or from an ancestor that exists before any write. Absence of `.gwz` must not yield `handles_ok = false`. Create-door disagreement when `handles_ok` is true stays an anomaly.

**Closure test:** A sentence: missing `.gwz` on APFS/ext4 → `handles_ok = true` (or inherit root), checked create, no reverse-door sentence. Overlay still `handles_ok = false`. NFS/tmpfs that answer `name_to_handle_at` still `handles_ok = true` with no (b) sentence.

---

## 2. Invariant analysis

- **Refuse-before-floor (P1-1).** Still holds. Header-only classifier “behaviour-preserving today” may land; v0 continue/abort still body-decode in the v0 store until (4).
- **Overlay (b) copy escape (P1-2).** Still holds. Appending the (b) sentence only when `handles_ok` is false is a **fix** relative to revision 3 (which warned on all odd volumes, including NFS/tmpfs that have handles).
- **Leftover v0 occupancy (P1-3).** Holds; start-gate remedy suppression is the right fail-closed direction.
- **Archive decoder (P2-1).** Holds.
- **Landable (2)/(3) vs ordinary v0 (P2-2).** Fails again. Revision 3 closed it; this delta reopens the original stage/v0 window. `--no-ff` overlay start after (3) is the operator’s start-must-run story plus the closed P1-2 escape, not a reopen of P1-2, **if** (3)’s diagnostic is honest (`handles_ok`).
- **Raw create + kill inventing progress.** Unchanged; not reopened.
- **`handles_ok` not recorded on the v1 body.** Continue still re-decides (ship 1). A workspace copied onto APFS gets `handles_ok` true on the next abort. Compatible with P1-2. Failed as a new stuck-state attack.
- **Additive `handles_ok` disclosure.** Capability bit, not merge content. Failed.

---

## 3. Risks and next action

Residual: sshfs unmeasured; Windows `require_ntfs`; `cfg(test)` floor override must not leak to production (charter already says test-only); copy escape still operator `cp`.

Next action: one bounded text patch (1) pinning production v0 stage/discover until (4) so P2-2’s walk is unconstructable, and (2) pinning the handle probe to an existing ancestor so missing `.gwz` is not `handles_ok = false`. Do not implement. Do not treat revision 3’s GO as covering these bytes.
