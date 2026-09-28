# GWZ core session plan, revision 2 (TR1.4a) — acceptance verdict

Date: 2026-09-27. Status: **accepted for TR1.4a's scope at SHA-256 `58ab341a92c68a0b08f6383b60b23dc6e299d428fb4259409e7547eb556ea0f2` after [Consistency-1](GwzCoreSessionPlan-ReviewConsistency-1.md) and [Safety-1](GwzCoreSessionPlan-ReviewSafety-1.md) reported GO; this accepts the plan text only, for the sections and steps TR1.4a revises**.
- This is the session plan's G1 for those parts, as the transport plan's TR1.4a states.
- Steps marked "TR1.4b revises this" stay draft until TR1.4b's review.
- Steps marked "TR1.4b extends this" may merge on this acceptance. TR1.4b's extension of them is reviewed as a re-freeze, and a re-freeze is dual (§2.2).
- It authorizes no implementation, activation or release.

The same two reviewers re-verdicted revision 2 with their context intact.
- Each verified the object, the five HEADs and every input hash at the start and the end.
- Each confirmed that the supplied diff is exactly the change from revision 1.
- Neither read the other's round-2 report.
- Both read controlling documents at HEAD, and ran only inspection commands and `diff`.

| Axis | Verdict | Round-1 findings | New |
| --- | --- | --- | --- |
| Consistency | GO | All 5 closed (P3-1 to P3-5) | none |
| Safety | GO | All 7 closed (P2-1, P2-2, P3-1 to P3-5) | P3-6, P3-7 |

Both reviewers confirmed that every hunk of revision 2 maps to a disposition in the [remediation plan](GwzCoreSessionPlan-RemPlan.md), a residual note it took, or one of the drafter's eight disclosed deviations. Safety judged both forms that differ from its remedies to resolve its findings:
- **S-P2-1's per-call legacy context,** with process-wide budgets on the legacy path. Safety called it the safer form: per-call budgets would have unbounded the legacy path.
- **S-P3-5's "extends" marker,** with a reviewed re-freeze.

Neither reviewer classified anything as architectural. Both cleared their remaining items to land in the status edit.

## Corrections applied after the GO

1. **Safety P3-6: C8's default.** On 2026-09-27 the operator decided C8: the contract's revision 5 gives `cancellation` field 8, and the candidate projection's tags 3–7 stay. Revision 5's dual review is under way. C8 records the decision, §6.2 cross-references it, and CS1.1 waits on revision 5's acceptance.
2. **Safety P3-7: the candidate rule.** §2.4's rule now binds every step, not only schema steps. Until activation, once TR3.1 has landed, every step builds the candidate before it merges, and its review package records the build.
3. **Safety residuals:**
   - §2.4 lists the regenerator's prerequisites, which `candidate-generator.json` pins: the taut checkout at its pinned revision, taut-proto, rustfmt and the gwz-transport owner schema;
   - CS3.4's bound on the credential spawn is a time and kill bound, not a `gh` slot;
   - CS6.5 states that once the crate default goes, gwz-core's public handler API takes an explicit context (R9);
   - §2.2 states that a re-freeze is dual.
4. **Consistency residual.** §2.2 and §7 name TR1.4a's scope as revisions 1 and 2.

The plan then carries the accepted status and hashes `b57f7eae8b4d2932acd392de6704111319442015bf47a01a809c538c75476428`.

## Recorded

- **New edges for the transport plan's sketch:** TR3.1 now gates Phase 1's CS1.1, and C8 (contract revision 5) gates CS1.1. The program checkpoint records both.
- **Transitional double budget.** Between CS4.7 and CS4.8, a gwz-py process that uses both paths at once holds two supervisors and two sets of caps, at most twice today's. This is a development window only; the release follows CS6.5.
- **Not in the object:** `EVIDENCE.md` still names `D:/gwz-tests` for Windows fixtures, which is stale. Recorded for a separate fix.
- **Found after the GO, while applying correction 3.** The candidate protocol is composed from a pinned older core schema, not from today's. `candidate-generator.json` pins `retained-old-schema-sha256` to `protocol/gwz.taut.py` at gwz-core `54618449` (`551fe930…`); today's schema hashes `423cb73b…`, and the regenerator refuses any other.
  - So the candidate lacks `GwzErrorCode` 73 and 74 by design, not because it is stale, as CS1.1's accepted text says.
  - CS1.1's regeneration must also move that pin to the new schema. That is a candidate-harness change, so CS1.1's files gain `candidate-generator.json`.
  - TR1.4b's revision, or CS1.1's own dual freeze review, corrects the text.

## Next action

- **The paired GWZDesign and GWZRequirements paragraphs are flipped from DRAFT**, as G0 requires before CS1.1. This is done together with the reuse design's "On GO" edits to the same paragraphs.
- **CS1.1 starts** once TR3.1 lands and contract revision 5 is accepted.
- **TR1.4b follows** TR1.2's closure and TR1.3's GO.
