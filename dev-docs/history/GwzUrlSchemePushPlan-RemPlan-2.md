# GwzUrlSchemePushPlan — remediation plan, round 2

Date: 2026-09-14. Lane owner: the main session.

**Reviewed object.** `dev-docs/GwzUrlSchemePushPlan.md` at root `f3a395b`,
with gwz-core at `c9c7a98`.

**Reports, both filed verbatim:**

- `GwzUrlSchemePushPlan-ReviewConsistency-2.md`: **GO.** All ten round-1
  findings on this axis are closed. New P3-6, P3-7 and P3-8 do not block.
- `GwzUrlSchemePushPlan-ReviewSafety-2.md`: **NO-GO.** All nine round-1
  findings on this axis are closed. New P2-5 blocks; new P3-5 does not.

Neither report classifies a new architectural root cause, so this second
remediation round is within the two-round cap.

**Revision.** One patch to the plan, committed together with this file and both
round-2 reports. Finding IDs carry an axis prefix: `C-` for Consistency, `S-`
for Safety.

## Dispositions

| ID | disposition | where in the revision | closure test |
|---|---|---|---|
| S-P2-5 | **Fix, as the reviewer specified.** Any forced or deleting transfer in the operation invalidates every kept advertisement and every D8 proof, and every root-lock dependency is then read after all member transfers and before the root transfer. The "same repository, between destinations" relation is limited to places where calling two URLs different fails toward a contact or a read (the last-known ref's push-URL condition), and never limits invalidation. D8 and D9 point to the operation-wide rule. The step-1.2 seam can map several URLs to one advertisement store. | §3.5 definition and rule 1, D8, D9, steps 1.2 and 3.3, §6, §7 | Step 3.3, each ending with the root refused and no root `push_prepared` recorded: (i) a push remote reaching the dependency's repository through an SSH host alias, with a forced refspec, under `always`; (ii) the same under the default; (iii) two members whose URLs differ only by a `.git` suffix, with one forced push; (iv) a non-forced push through one spelling and a concurrent forced push through another, where the D8 proof must not count. Then (v): the non-forced variant of (i) reuses the kept advertisement with no extra read. |
| S-P3-5 | **Fix.** The push-path refusal names deleting or repairing `.gwz/url-scheme.yml`, and does not suggest a bare `gwz materialize`, which materializes the lock. This supersedes the remedy text of round-1 S-P3-2. | §3.3, step 2.1 | Step 2.1: the refusal names the file remedy and no bare `gwz materialize`. |
| C-P3-6 | **Fix.** Rule 2 now says what "contacted" means: read for its own push decision, and pushed if rule 1 finds anything to send. A D10 dependency read never changes a member's classification, row or reason. Wording updates: the summary line says "not checked for changes"; the push help (step 3.7) and the GWZDesign text (step 4.1) say unchanged repositories are not checked for changes or pushed, though publishing the root still reads each dependency; §7's credential-helper note includes D10 reads. | §3.5 rule 2, §3.6, §3.7, §7, steps 3.5, 3.7 and 4.1 | Step 3.5: with one member and the root changed, 11 transport calls, the unchanged rows' reason text, and the summary line text. Step 3.7: a JSON golden for the same case. |
| C-P3-7 | **Fix.** The rename case moves to step 3.6, with the outcome §3.7 states: the member is `Noop`, and the dependency read of the committed URL refuses a root that needs a commit only the fork holds. It is removed from step 3.5's contact list. This supersedes round-1 S-P2-1's note on the rename test. | steps 3.5 and 3.6, §6 | Step 3.6, native: a fork remote renamed to `origin`, a member equal to the moved ref, and a root lock naming a fork-only commit give member `Noop`, root refused, and the bare root ref unchanged. |
| C-P3-8 | **Fix.** Before each socket-polled push, step 4.2 runs a default `--dry-run` and continues only when every row is `Noop` "up to date" (not "behind"); for case 6, only the named repositories may be planned. It says that `--check-remotes --dry-run` skips classification and cannot confirm anything. | step 4.2 | The acceptance note records each case's default dry-run output before its push. |

## Residual wording folded in (no finding IDs)

**From the Safety round-2 report, §3:**

- The invalidation read happens "after all member transfers and before the root
  transfer", and the step-3.3 invalidation tests assert that no root
  `push_prepared` is recorded.
- The missing-dependency refusal also suggests publishing the member, not only
  `--check-remotes`.
- §3.7 separates a rewind, which a fetch repairs, from a deletion, which it does
  not.
- D10's rejected alternative is limited to root publication, matching §3.7.
- A contacted root that rule 1 finds already on origin is still proven, in both
  modes (§3.5 rule 2), with a step-3.5 test.

**From the Consistency round-2 report, §3:**

- The "at most once" wording in goal 3, rule 3 and the Phase 3 milestone now
  names rule 1's re-reads.
- §3.7's last bullet says a root's contact includes its dependency reads.
- §3.7 says `gwz tag --push` detects an unsound root only when it pushes a tag
  on that root commit.
- §3.6 names the read URL only when it differs from the committed URL, matching
  §3.3.
- Steps 1.1, 1.2, 3.1 and 3.2 depend on 0.1, matching the dependency summary.
- The seam's limit on invalidation across URL spellings is removed by S-P2-5's
  store mapping.

## Structural changes

None. The rules change inside §3.5, and one test moves from step 3.5 to step
3.6. No step is added or renumbered.

## Re-verdict

- **Safety** re-verdicts P2-5 and P3-5 on the revised tuple, with a
  changed-range analysis. Filed as `GwzUrlSchemePushPlan-ReviewSafety-3.md`.
- **Consistency** confirms P3-6, P3-7 and P3-8, and checks the changed range, so
  that both axes report on the same revision. Filed as
  `GwzUrlSchemePushPlan-ReviewConsistency-3.md`.
