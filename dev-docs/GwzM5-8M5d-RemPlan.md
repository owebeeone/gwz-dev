# GwzM5-8M5d — RemPlan

**Status:** **applied** 2026-09-03 to `GwzM5-8M5d-Charter.md` revision 2
and the M5c §7.2 banner. Awaiting same-reviewer re-verdict. Not
implementation.
Round 1 of at most two. Same reviewers re-verdict after the patch.
**Reviewed object:** `dev-docs/GwzM5-8M5d-Charter.md` SHA-256
`cc3635c8419e258ba51b7d8891ff3ed18c589d362aa7f9f77bfc27de6e7f2211`
at gwz-dev `3d0740a3c86f5f31a7bd734a0227a34887e1db98`; gwz-core
`57502e461055809503ca9dc0d7cb791b8b332f6c`.
**Reviews:** `GwzM5-8M5d-ReviewConsistency.md` (NO-GO; pre-commit
C-P1-1, C-P2-1…C-P2-4), `GwzM5-8M5d-ReviewSafety.md` (NO-GO;
pre-commit S-P1-1, S-P1-2, S-P1-3, S-P2-1, S-P2-2).
**Lane-owner merge:** dual **NO-GO**. One charter patch. No code. No
tag. No M6. `GwzM5-8M5d-Review.md` remains out of scope (implementer
sizing, not the object).
**Reviewers (resume):** Consistency
`964b5faf-d29f-4740-a415-074536d16d4f`; Safety
`a928ac33-c2a3-4429-b9bc-0374558a6ad3`.

Blind convergence (same defect, two axes):

- **Refuse-before-floor** — C-P1-1 ≡ S-P1-1.
- **“No production v0 merge type” vs archive projection** — C-P2-2 ≡
  S-P2-1.

Operator F1/F2 from the sizing memo already pointed the same way as
those two. This RemPlan does not reopen them.

## Blocking — take

| ID | Disposition | Closure |
|---|---|---|
| **C-P1-1 / S-P1-1** §9 step (1) production refuse while floor is V0 | Strike “can land before the floor” and the `--no-ff` already-v1 justification. Drafting the I2 amendment text may precede the close; the **production** §2 refuse, `ACTIVE_WRITER_FLOOR = V1`, v0-engine deletion, and `--filesystem-strict` on ordinary starts flip in the **same** reviewed change. Until that commit, mutating v0 stays on today’s v0 lifecycle. | §7 and §9 agree. A reader cannot construct a published main tree with the §2 refuse while `version.rs` floor is still `V0`. Named walk: after any authorized intermediate landing, ordinary start then `--abort` still aborts a v0 record. |
| **C-P2-2 / S-P2-1** §8 “no production v0 merge type” | Replace with: no production v0 **lifecycle engine** (start/continue/abort/migrate/open-body/`adapt`). Open v0: envelope-only §2 sentence, no body decode. Archived `done/` v0: I2 §7 decoder/GC **remain production**, consumed only for `done/` bytes. `decode` of archived v0 is not “the v0 engine.” | §8 can be checked off without deleting the archive decoder, and cannot be read as requiring that deletion. Named rows: open-v0 fixture refused at envelope; archived-v0 fixture still projects; GC never deletes unread/unreadable archives. |
| **C-P2-1** I2 amendment is section-wholes, not L1-08 | Fold a bounded amendment block into the charter (or an attached I2 patch the close duals): quote each **superseded** I2 sentence; quote each **retained** sentence inside §4–§5 (archive archival / I2 §7 pointer); list as in-scope §1:36, §2:102–103, the 2026-08-25 annotation close, §8:289 + 0.14 decode-generation add, §9:304–305, and `GwzM5-8I2CompatibilityPredicates.json`. I2 gets the §7.2 “Amended \<date\> by …” banner. | Every I2 sentence that would contradict §2 refuse is quoted superseded or explicitly retained. An implementer can produce the I2 diff from that list. Predicates JSON and I2 §8 generation add are on the affected-interface list. |
| **C-P2-3** “supersedes M5c in full” vs “kept” §2.2–§2.4 | Scope the supersession: M5c is superseded for all ship-2 design. Event/response/`stage`/raw-create requirements are **restated in M5d §3–§4** and those restatements control; stop saying “kept.” M5c status uses the §7.2 formula; a one-line note that §0–§6 below are not current authority. | No current sentence treats a M5c section as controlling. Dropping M5c would not change M5d meaning. |
| **C-P2-4** CapabilityFree + F-3 unstated | §7 table names both: (1) CapabilityFreeAmendment §3 / raw-writer inventory — revise the carved set when v0-engine files leave production (shrinkage is an amendment, not a checker edit); net may shrink (seven v0 rows out, one `entry.rs` raw primitive in) and that revision is in the S1 dual. (2) F-3 — redefine the seam floor for a tree whose production v0 persistence re-exports are gone (or keep a test-only seam the floor can still derive). Relocating `write_atomic_verified` is not the whole F-3 story. | Deleting `persist_merge_record` / `finalize.rs` is either forbidden or paired with the named amendment/checker change that keeps both gates green. |
| **S-P1-2** handle-less start succeeds; selected-root / `--preserve` abort refuses; 0.13 cannot abort v1 | **Do not refuse start** (operator already barred that as a product story). **Do not add reverse-path raw as a milestone.** Product: (a) participant-only abort still clears; (b) selected-root / `--preserve` / published-evidence abort on handle-fail volumes **refuses with a new sentence** that does **not** advertise `--abort` as the escape; (c) that sentence names a working escape that needs neither handles nor 0.13.x: copy the workspace onto a handle-capable volume and abort, **or** the exact git + delete-open-record procedure. Start warning on those volumes states this limit. Optional same-flag raw abort remains a later cure, not this close. | Named overlay row: ordinary start writes v1 raw; participant-only abort clears; selected-root/`--preserve` abort prints the new escape and leaves Git/record in a state that escape actually clears. A 0.13.0 binary is not an accepted pass for (b). |
| **S-P1-3** engine deletion without pinning the 0.13.x escape / occupancy | Pin `v0.13.0` as the retained-reader escape for leftover **open** v0 for as long as 0.14 is current; §2 names that tag. Deleting the v0 engine is not authorized if that artifact is not published and installable. 0.14 notes (precondition of the irreversible step, not a follow-on) require abort-or-finish of open v0 **before** upgrade. Third occupancy: envelope v0 is **not** a merge lifecycle and **is** an open operation — mutation gate, `gwz commit`/`add`/`stage` refuse with the §2 sentence; they must not return “no merge.” No v0 body decode. | Named leftover-open-v0 fixture: abort/continue/status/commit/add/stage all refuse without body decode and without writing; none behaves as idle. |
| **S-P2-2** §9 as independently landable main steps | Restate §9 as implementation order **inside** one unpublished branch / one landing commit (or a dual-reviewed tuple after (1)–(4)), not as publishable `main` steps. Dual object is one tree: ordinary start is v1, raw create live, §4 surfaces live, open v0 is the §2 refuse, `--filesystem-strict` refuses a below-bar ordinary start before writes. Classifier/amendment **text** may still be drafted early. | §9 contains a sentence that (1)–(4) are not independently published to `main`. |

## Non-blocking (fold)

- **C-P3-1** — §1: “starts and can complete the forward path; reverse doors that need handles still refuse (§3 / S-P1-2 escape).” Do not say “finishes” unqualified.
- **C-P3-2** — §7 row: DR-1 ship-2 pointer and A1 §2 carrier name retarget to M5d (dated note; do not rewrite A1’s historical landing).
- **C-P3-3** — §7 row: `transactional-apis.mdc` excepted for the uncatalogued/handle-fail **crash** arm only, per operator §0; above-bar path unchanged. Does not except live abort (S-P1-2).

## Next

Applied: `GwzM5-8M5d-Charter.md` revision 2, SHA-256
`38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624`;
M5c banner uses the §7.2 formula; I2 exact clauses are §10 of the
charter (I2.md itself waits for the close dual). Same two reviewers
re-verdict. Do not implement. Do not cut 0.14.0. Do not start M6.
