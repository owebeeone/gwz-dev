# GwzM5-8M5d — CONSISTENCY-AXIS REVIEW

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` revision 4 (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `5ab008ea283efeb8e7e847ba137ef4640e6c163b3c269e94db811f1b6b39b23d`). Controlling DRAFT document: the object itself. Prior GO was revision 3 at `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`; this SHA is not that acceptance.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98` (verified at start and end). gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c` (verified at end). Object read from the working tree. I2.md body not rewritten (amendment text remains charter §10). Tuple did not move.
**Date:** 2026-09-03
**Axis:** CONSISTENCY — delta vs revision 3 against the controlling graph (internal contradictions; I2 via §10; ship (1) DR-1; L1-08; CapabilityFree; F-3; transactional-apis.mdc). Independent, adversarial, read-only. Filed verbatim by the lane owner.

**Verdict: NO-GO** — 1 P2 blocks (plus 2 P3). Round-1 P0–P2 and round-2 P3-1/P3-2 stay closed. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| **R1 P1-1** | Production §2 refuse never on `main` while floor is `V0` | Original walk (published refuse, ordinary still writes v0, continue/abort refused) remains forbidden: §9:344–347 “the production §2 refuse and the floor flip are never on `main` apart”; §9.1 refuse arm may not land while floor is `V0`; after every pre-(4) landing, ordinary start then `--abort` still aborts a v0 record. Revision 4 *does* put (1)–(3) on `main`; that does not reconstruct the refuse-before-floor walk. | **CLOSED-stays** |
| **R1 P2-1** | §10 exact I2 superseded/retained list | §10.1 quotes and §10.2 archive retention unchanged in substance. New stale **replacement** for I2 §9:304–305 is a new finding (P2-1 this round), not a reopen of the missing-clause hole. | **CLOSED-stays** |
| **R1 P2-2** | No v0 lifecycle engine; archive decoder remains | §1:58–61, §8:332–337 unchanged. | **CLOSED-stays** |
| **R1 P2-3** | M5c superseded; restatements control | Banner lines 21–25; §4:223–224. | **CLOSED-stays** |
| **R1 P2-4** | Name CapabilityFree + F-3 | §7 rows still name both. Ghost-seam *option* struck (L-P3-2); redefine-the-floor remains, so the original unstated-pin walk stays closed. | **CLOSED-stays** |
| **R2 P3-1** | Not “seven v0 rows”; not GC writers | §7:300 still names `finalize.rs` / v0 abort/preserve/archived, **not** `store/gc.rs` / `store/retention.rs`. | **CLOSED-stays** |
| **R2 P3-2** | §10.1 §5:180 Replacement copyable at close | Still only the after-close refuse sentence (§10.1). Until-close remains in §7:312–313 / §9. | **CLOSED-stays** |

---

## Changed-range analysis

Revision 3 → 4 (banner names the folds; `GwzM5-8M5d-ReviewImplementation.md` not read). Actual bytes:

1. **Status** — records revision-3 acceptance SHA and that it does **not** cover revision 4.
2. **§2 sentence** — `use gwz 0.13.0 (the last release before 0.14)` instead of “use tag v0.13.0”; tag still identified in the following paragraph.
3. **§2 occupancy** — existing open-operation remedy (“use merge status / continue / abort”) **suppressed** for a v0 envelope; sites include `--gc`, id-less `--status`, and `merge/start.rs`.
4. **§3 L-P2-1** — `crash_recovery_decision` gains a directory-level handle probe on `.gwz` before any write; carries `handles_ok`; reverse-door sentence appended **only** when `handles_ok` is false; remote/volatile must not inherit the reverse-door limit; create door consults the same result.
5. **§3 L-P3-1** — optional additive `crash_recovery.handles_ok` (BOOL, absent above the bar); no protocol version bump.
6. **§4/§7 F-3** — test-only / hollow seam **struck**; floor = the neutral primitive’s module; only `entry.rs` calls it; J-1 ruling recorded in the S1 dual.
7. **§7/§9 landing shape (L-P2-2)** — **material change.** Revision 3: (1)–(4) not independently published to `main`. Revision 4: (1)–(3) **are** landable `main` steps (header-only classifier; §4 parity + `cfg(test)` V1 override + class-(ii) re-point; raw create + handle probe + `handles_ok`). The **close** (4) is still one dual: floor + production refuse + engine erasure + pin/doc/CapabilityFree/F-3.
8. **§10.2/§10.3** — `handles_ok` allocation; start-gate remedy; `crash_recovery_decision` probe; docs-manifest / echo tests.

**NEW ARCHITECTURAL root causes:** none. Allowing additive (2)/(3) onto `main` while binding refuse to the floor is a sequencing fold, not a new architecture. The blocking defect is that §10’s I2 replacement was not updated with that fold (**text**, L1-08).

---

## 0. Evidence base

Commands (cwd `/Users/owebeeone/limbo/gwz-dev`), start and end:

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `5ab008ea283efeb8e7e847ba137ef4640e6c163b3c269e94db811f1b6b39b23d`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read: charter revision 4 in full. Controlling spots: I2 Compatibility via §10 (I2.md body out of scope); DR-1 §2/`§3.7`/`§4.1` (`CrashRecoveryDecision` today has no `handles_ok`; taut `MergeCrashRecovery` fields 1–3 only, `gwz.taut.py:1315–1322`); live `crash_recovery_decision` (`entry.rs:195–323`) learns identity/remote/volatile, not handles; `create_merge_store_record` (`entry.rs:541–560`); F-3 still derives `V0_PERSISTENCE_SEAM_FLOOR` from `merge/mod.rs` re-exports. Targeted tests: none. Not read: `GwzM5-8M5d-ReviewImplementation.md`, `GwzM5-8M5d-Review.md`.

---

## 1. Findings

### [P2-1] §10.1 still freezes I2 §9 as “one reviewed change with raw create and §4 surfaces,” which this revision’s §7/§9 contradict

**Location.** §10.1 row I2 §9:304–305 Replacement: “**M5d** activates the v1 writer floor … in one reviewed change **with raw create, §4 surfaces, and the production §2 refuse**.” §7:305–311: the close is floor + refuse + erasure + pins; “Raw create, the §4 surfaces, and the class-(ii) suite re-pointing **may land on `main` before the close** as their own reviewed steps.” §9:341–376: (1)–(3) are landable `main` steps; (4) is the close without those additives as part of the same landing.

**Violated invariant.** L1-08: the I2 replacement is what an implementer copies at close. It must not assert a “one reviewed change” set that the same charter’s landing shape splits. After (2)/(3) have already landed, that I2 sentence would be a false composition of the activation (the same class of lie the 2026-08-25 floor annotation existed to prevent).

**Reproduction.** Follow §9: land (2) and (3) on `main`, then produce the I2 diff from §10.1. Accepted I2 says raw create and §4 surfaces landed with the floor/refuse close. Git history on `main` says they did not.

**Impact.** Compatibility-contract history disagrees with the authorized `main` topology. Dual on “the amendment with this charter” cannot freeze both.

**Required correction.** Rewrite the I2 §9:304–305 Replacement to match §7/§9: M5d’s **close** is one reviewed change of floor + production §2 refuse + v0-engine deletion + pin/CapabilityFree/F-3; raw create and §4 surfaces are prior additive `main` landings, not members of that close. Do not say “with raw create, §4 surfaces.”

**Closure test.** §10.1 Replacement and §7/§9 describe the same landing sets. Copying the Replacement into I2 at close cannot be read as requiring raw create / §4 to share the close commit.

### [P3-1] Step (1) “header-only envelope classifier (no v0 body decode)” can be read as stripping production v0 body decode before (4)

**Location.** §9.1: classifier “no v0 body decode, behaviour-preserving today” may land; refuse arm may not; “ordinary start then `--abort` still aborts a v0 record.”

**Violated invariant.** Live v0 continue/abort **body-decodes**. “No v0 body decode” is the **close** §2 rule. If applied to the whole binary at step (1), abort of a self-created v0 record fails — the P1-1 walk by another door — despite “behaviour-preserving” and the abort-still-works sentence.

**Impact.** Bounded: those two sentences already forbid the broken reading. The classifier clause should say envelope-version dispatch only; the v0 engine still body-decodes until (4).

**Required correction.** One phrase: the landed classifier does not decode the v0 **body**; the v0 **lifecycle** still does, until (4).

**Closure test.** Step (1) cannot be implemented as production-wide “never decode a v0 body.”

### [P3-2] `handles_ok` amends ship (1) §3.7’s frozen `MergeCrashRecovery` (fields 1–3) without a §7 disposition row on that slice

**Location.** §3:156–161 and §10.2:412–414 allocate `MergeCrashRecovery.handles_ok`. Live taut and DR-1 §3.7 specify fields 1–3 only. §7 ship (1) row: “**mechanism** unchanged (warn / strict / identity bar).”

**Violated invariant.** Additive wire field on a ship-(1)-frozen message is an amendment of DR-1 §3.7 (and `gwz.taut.py`), even though the identity bar is unchanged. L1-08 wants that slice named. §10.2/§10.3 name the taut field; §7 does not.

**Impact.** Bounded: implementers following §10.3 will still regen protocol. A reader of §7 alone thinks ship (1) protocol is untouched.

**Required correction.** §7 ship (1) row: identity bar / strict / warn **structure** unchanged; §3.7 message gains optional field 4 `handles_ok` (additive, no version bump).

**Closure test.** §7 and §10.2 agree that `MergeCrashRecovery` is amended, not unchanged.

---

## 2. Invariant analysis

Attacks on the delta that **failed** (do not reopen prior IDs):

- **Refuse-before-floor.** (1)–(3) on `main` is a real topology change vs revision 3. The P1-1 invariant (refuse and floor never published apart) is still written and is the dual object of (4). Ordinary remains on I2 §5 until (4).
- **Remote/volatile vs reverse-door.** Live `crash_recovery_decision` (`entry.rs:311–322`) can be `Unsupported` with gap remote/volatile while `name_to_handle_at` works. §3’s claim that those volumes must not carry the reverse-door limit, and that `no_durable_identity` on Linux already implies handle fallback failure, matches that split. §1’s “plus the §3 reverse-door limit **on handle-fail volumes**” is qualified; not a contradiction with tmpfs in the odd-volume list.
- **Create-door vs decision-time probe.** Live create is `CheckedArtifact::acquire` on the record relative (`entry.rs:541–560`); today the decision does not probe handles (`entry.rs:302–323`). Adding the create door’s probe at decision time, then treating a later handle failure when `handles_ok` is true as anomaly, is internally specified. Overlay first-merge `.gwz` existence is implementer path choice under that spec, not a charter contradiction.
- **F-3 ghost seam.** Striking the test-only hollow re-export still satisfies P2-4 (floor redefined; deletion paired with checker change). Live F-3 still derives from `store::{…}` re-exports; §7 says S1 records the J-1 ruling when that derivation goes away.
- **CapabilityFree / transactional-apis / engine vs archive.** Unchanged and still consistent.
- **Ship (1) identity bar.** `Supported` vs `Unsupported` for catalog is not rewritten; `--filesystem-strict` still refuses before lease. Appending reverse-door text and `handles_ok` are ship-2 additives (P3-2 is only the missing §7 row).
- **`--no-ff` overlay after (3) before (4).** Raw create on `main` changes ship (1) §4.1’s remaining handle refuse for `--no-ff` only; ordinary stays v0. That is the authorized additive landing, not refuse-before-floor.

---

## 3. Risks and next action

P3-1 and P3-2 do not block. No new architectural root cause; the two-round rem cap is not triggered on this axis.

**Next action:** one text patch aligning the §10.1 I2 §9 Replacement with §7/§9 (P2-1), optionally folding P3-1/P3-2. Do not implement. Do not cut 0.14.0. Dual GO still requires both axes on the corrected SHA-256, not `ec4eb011…`.
