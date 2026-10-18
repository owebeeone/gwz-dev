# GwzM5-8M5d — RemPlan-3

**Status:** **applied** 2026-09-03 on the operator's say-so to
`GwzM5-8M5d-Charter.md` **revision 5**, SHA-256
`df6399662c2c93b3e94072f62cd61856e74fa0b7ef6f6699b685e2ae804e32ec`
(charter text only, not implementation). Awaiting the focused
re-verdict of both axes on that SHA; not a third architectural root
cause.
**Reviewed object:** `dev-docs/GwzM5-8M5d-Charter.md` revision 4 SHA-256
`5ab008ea283efeb8e7e847ba137ef4640e6c163b3c269e94db811f1b6b39b23d`
at gwz-dev `3d0740a3c86f5f31a7bd734a0227a34887e1db98`.
**Reviews:** `GwzM5-8M5d-ReviewConsistency-4.md` (NO-GO; C-P2-1),
`GwzM5-8M5d-ReviewSafety-4.md` (NO-GO; S-P2-2 reopened, S-P2-3).
**Lane-owner merge:** dual **NO-GO**. Revision 3 GO does **not** cover
these bytes. One charter patch. No code. No tag.
**Cap:** both reviewers: **no new architectural root cause**. S-P2-2 is
the same landable-series cause as round-1, reintroduced by L-P2-2; not
a third architecture. Cap does not fire.

Prior IDs that stay **closed:** refuse-before-floor, overlay copy
escape, occupancy, archive decoder. Do not reopen them.

**Operator (2026-09-03, binding):** 0.14 must only be consistent
**with itself.** Anything before that — leftover v0 merge behaviour,
status-quo staging/abort of v0, mixed untagged-`main` windows before
the close — is out of scope. Do not treat “worse than today’s v0”
as a 0.14 defect.

0.14 self-consistency is: every new start is v1; there is no v0 merge
engine; an open v0 *file* is the §2 sentence (not a merge, not idle);
`gwz stage` serves open v1; archived `done/` v0 is projection only.
The close still ships floor + refuse + engine delete together so
**0.14** does not both write v0 and refuse it. Intermediate `main`
landings are not a product.

## Blocking — take

| ID | Disposition | Closure |
|---|---|---|
| **S-P2-2** (reopened) §4 v1-body stage while ordinary still v0 on `main` | **Dispute, evidence: operator 2026-09-03.** Pre-close `main` and v0 merge are out of scope. Do not pin v0 conflict staging. Charter: 0.14 `gwz stage` serves open **v1**; open v0 is the §2 sentence. **Applied:** ruling quoted in charter §0; scope sentences in §4 (`gwz stage` bullet, with a non-pin hygiene note), §7 and §9. | Not a 0.14 P2. Re-verdict: **closed-disputed** by operator ruling. |
| **S-P2-3** decision-time handle probe of missing `.gwz` | **Take.** 0.14 vs itself: first merge on APFS/ext4 must not be classified handle-fail. Learn `handles_ok` from the directory the create door identities today (**workspace root**, which exists before any write); absence of `.gwz` must **not** yield `handles_ok = false`; create-door disagreement when `handles_ok` is true stays an anomaly. **Applied:** charter §3 “Where handle capability is learned”. | Missing `.gwz` on APFS/ext4 → `handles_ok = true`, checked create, no reverse-door sentence. Overlay still `false`. NFS/tmpfs that answer `name_to_handle_at` still `true`, no (b) sentence. All three sentences are in §3. |
| **C-P2-1** §10.1 I2 §9 Replacement still bundles raw create / §4 into the close | **Take.** 0.14's I2 sentence must match 0.14's landing shape: the **close** is floor + production §2 refuse + v0-engine deletion + pin/CapabilityFree/F-3; raw create and §4 surfaces are prior additive `main` landings. **Applied:** charter §10.1 I2 §9:304–305 Replacement rewritten. | Copying the Replacement into I2 at close cannot be read as requiring raw create / §4 to share the close commit. |

## Non-blocking (fold)

- **C-P3-1** — pre-close classifier wording. Out of scope as a 0.14
  product pin. **Applied** as hygiene in charter §9 step (1): the
  classifier is envelope-version dispatch only; the v0 lifecycle still
  body-decodes until (4).
- **C-P3-2** — §7 ship (1) row: warn/strict/identity **structure**
  unchanged; `MergeCrashRecovery` gains optional field 4 `handles_ok`.
  **Applied** in charter §7.

## Next

Applied: `GwzM5-8M5d-Charter.md` revision 5, SHA-256
`df6399662c2c93b3e94072f62cd61856e74fa0b7ef6f6699b685e2ae804e32ec`.
**Operator decision 2026-09-03: "we've done enough re-reviews."** No
re-verdict is run on revision 5. The loop closes here: revision 3
carried the dual GO; revisions 4–5 are operator-directed folds with
the round-4 findings dispositioned above (S-P2-3 and C-P2-1 taken,
S-P2-2 closed-disputed by the §0 ruling, C-P3-1 / C-P3-2 folded).
Revision 5 is the accepted, controlling ship-2 design. Do not
implement until the operator authorizes the build. Do not cut 0.14.0.
