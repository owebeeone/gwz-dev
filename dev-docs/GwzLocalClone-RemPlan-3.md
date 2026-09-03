# GwzLocalClone — RemPlan-3

**Status:** remplan for a **non-architectural** example reorder (design
revision 5). Not implementation. Third remediation confined to this
class is permitted; a **new architectural** root cause on the next
re-verdict stops the lane.
**Reviewed object:** `dev-docs/GwzLocalCloneDesign.md` SHA-256
`8ce70d8add0163839952ef2ac84623040d75be2a0d7c284ee1511b6dde770e3e`
at gwz-dev `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`.
**Reviews:** `GwzLocalClone-ReviewConsistency-3.md` (NO-GO; pre-commit
P2-10), `GwzLocalClone-ReviewSafety-3.md` (**GO**; 0 P0–P2).
**Lane-owner merge:** not dual GO. One design patch. No code.
**Cap:** both reviewers: **no new architectural root cause**. P2-10 is
§8 sequential-satisfiability (same class as closed C-P2-6). Cap does
not fire.

Safety round-1/round-2 IDs stay **closed**. Consistency round-1/round-2
IDs stay **closed**. Do not reopen recover/install/`--remote`.

## Consistency-only

| ID | Disposition | Closure |
|---|---|---|
| C-P2-10 §8.5 illegal after §8.4 `disband` | Move `gwz local disband` **after** §8.5 (new §8.6 teardown). §8.4 keeps dispose C / `--force` / `--keep` / `dispose root` / cwd refuse. After 8.6, `--remote hub` is `missing_remote`; trees still exist. Do not mark 8.5 as a separate vignette that a sequential reader can still execute after 8.4. | Numbered script: after 8.1–8.3, 8.5’s three commands succeed (family `--remote hub` / `--remote B`); disband runs last; after it, `--remote hub` is `missing_remote` and trees still exist. |

## Non-blocking (fold)

- C-P3-6 — omit `gwz.conf/gwz.yml` during copy; write it last. Lock
  recapture may create `gwz.lock.yml` earlier.
- C-P3-7 — `force_hazards=F(5, List(STR))` (taut list spelling, not
  `repeated=True`).

## Next

One revision of `GwzLocalCloneDesign.md` applying the row above (and
the two P3s). Same Consistency reviewer re-verdicts P2-10 (Safety
already GO on revision 4; focused re-verdict on Consistency unless
the example move creates a new safety surface — it must not). Do not
implement from revision 4.
