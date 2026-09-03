# GwzM5-8M5d — RemPlan-2

**Status:** **applied** 2026-09-03 to `GwzM5-8M5d-Charter.md` revision 3.
Awaiting Safety re-verdict of S-P1-2. Not implementation.
**Reviewed object:** `dev-docs/GwzM5-8M5d-Charter.md` revision 2 SHA-256
`38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624`
at gwz-dev `3d0740a3c86f5f31a7bd734a0227a34887e1db98`.
**Reviews:** `GwzM5-8M5d-ReviewConsistency-2.md` (**GO**; 0 P0–P2; two
new P3), `GwzM5-8M5d-ReviewSafety-2.md` (**NO-GO**; S-P1-2 still open;
pre-commit if that ID is struck/replaced).
**Lane-owner merge:** dual **GO** on revision 3 SHA-256
`ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`
after `GwzM5-8M5d-ReviewConsistency-3.md` and
`GwzM5-8M5d-ReviewSafety-3.md`. Design accepted. Not implementation.
**Cap:** both reviewers: **no new architectural root cause**. S-P1-2
open is a failed half of the round-1 cure (in-place recipe), not a
third architecture. Cap does not fire.

Consistency round-1 IDs stay **closed**. Safety P1-1, P1-3, P2-1, P2-2
stay **closed**. Do not reopen refuse-before-floor, occupancy, archive
decoder, or unpublished-landing.

## Blocking — take

| ID | Disposition | Closure |
|---|---|---|
| **S-P1-2** in-place git+delete does not clear selected-root / `--preserve` / published-evidence and burns the copy escape | **Strike §3 (b)(2)** entirely. The only named (b) escape is: copy the whole workspace onto a handle-capable volume, then `gwz merge --abort` (add `--preserve` if that was the stuck door). Do not advertise `git merge --abort` + delete `.gwz/merge/<id>.yaml`. Do not refuse start. Do not add reverse-path raw. | §3 lists only the copy-then-abort escape for (b). Followed on overlay + selected-root/evidence (and separately `--preserve`), it leaves no open record and no leftover composition commit / candidate lock / marker / preservation-needed dirty tree. An in-place recipe that deletes the open yaml while those remain is absent. |

## Non-blocking (fold with the same patch)

- **C-P3-1** (round 2) — replace “seven v0 rows out” with “the v0-engine carved files (`finalize.rs`, v0 abort/preserve/archived writers — not `store/gc.rs` / `store/retention.rs`).”
- **C-P3-2** (round 2) — §10.1 §5:180 Replacement = only the after-close sentence. Until-close stays in §7/§9.

## Next

## Next

Dual GO filed. Design accepted at revision 3 SHA-256
`ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`.
Do not implement until authorized. Do not cut 0.14.0.
