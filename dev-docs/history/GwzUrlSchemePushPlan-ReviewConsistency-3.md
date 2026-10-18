# GwzUrlSchemePushPlan — CONSISTENCY-AXIS REVIEW, ROUND 3

**Review object:** `dev-docs/GwzUrlSchemePushPlan.md` at gwz-dev root `0136094` (1067 lines). Status: "plan; decisions D1–D11 taken on 2026-09-14; round-2 review remediation applied; no code yet". Dated 2026-09-14. This is a bounded re-verdict. Round 2 reviewed the file at `f3a395b` and gave this axis GO.

**Baseline:**
- **Tuple:** gwz-dev `0136094` (clean); gwz-core `c9c7a98`; gwz-cli `2c4c0ea`; gwz-py `334f53f`; gwz-core-evidence `b2bf115`; taut `7a5f616`; taut-shape `74f375c`; taut-shape-rs `df13036`; taut-shape-py `864ca0b`.
- **Start and end:** identical.
- **gwz-core untracked files:** tracked files are clean, but the untracked out-of-scope documents are now four, not the two the brief lists. The two additions are `dev-docs/GwzTransactionalStore-G46.md` and `dev-docs/GwzTransactionalStoreA6.md`, in the same TransactionalStore lane. They were the same at start and end. No commit and no tracked file moved, so the tuple holds.
- **Sources:** documents via `git show 0136094:<path>`; code from the working trees, which equal their HEADs; vendored libgit2 1.9.7 read-only.

**Date:** 2026-09-14

**Axis:** Consistency, round 3, bounded to three things:
- the changed range `git diff f3a395b 0136094 -- dev-docs/GwzUrlSchemePushPlan.md`;
- re-tracing P3-6, P3-7 and P3-8;
- contradictions the diff introduces with unchanged sections or with the code it cites.

Independent, adversarial, read-only. Inputs were the round-2 reports and `GwzUrlSchemePushPlan-RemPlan-2.md`; the Safety round-3 review was not seen. Filed verbatim by the lane owner.

**Verdict: GO.** P3-6, P3-7 and P3-8 are closed on `0136094`, and no earlier closure is reopened. One new P3 in the changed range (P3-9) does not block. No P0, P1 or P2 is open, and there is no new architectural root cause.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P3-6 | **Fix.**<br>• Rule 2 defines "contacted" and makes a member's classification final: a D10 dependency read never changes its row or reason.<br>• The summary line says "not checked for changes".<br>• The push help (step 3.7) and the GWZDesign text (step 4.1) say unchanged repositories are not checked for changes or pushed, though publishing the root still reads each dependency.<br>• §7's credential-helper note covers D10 reads. | **Original sequence re-traced:** N = 8, one member and the root changed, default push.<br>1. The seven unchanged members classify as "`Noop`, not contacted" (365). "Contacted" now means read for their own push decision, and pushed only if rule 1 finds something to send (358-361).<br>2. The contacted root is proven (377-380), so D10 reads those seven as dependencies: N+3 in total (row 217; step 3.5 test 827-830; case 6, 965-967).<br>3. Their rows keep "up to date with origin/<branch> as of the last fetch or push", because classification is final for the row and the push decision (373-375).<br>4. The summary line now reads "were not checked for changes" (400-402). A dependency read does not contradict that.<br>5. The contradicting second reading, in which the read makes the member contacted and pushed, is excluded (373-375). This matches step 3.6 (853-857) and §3.7 (430-440).<br>**Other surfaces now agree:** help text (880-881), GWZDesign text (931-934), §3.7's opening (420-422), §7 (1039-1044).<br>**Closure tests:** step 3.5 (827-830); step 3.7 golden (890-892). | Closed |
| P3-7 | **Fix.** The rename case moves to native step 3.6, with the outcome §3.7 states, and leaves step 3.5's contact list. | **Text:** step 3.5's contact list (841-842) no longer names the rename; step 3.6 tests it natively (866-870).<br>**Re-traced against §3.5's conditions (324-330).** After `git remote rename fork origin`:<br>• the moved refs lie under `refs/remotes/origin/`;<br>• the rewritten refspec maps `refs/heads/*` into that namespace;<br>• no other remote writes there;<br>• there is no push URL.<br>So a last-known ref exists, and a member equal to it is `Noop`.<br>**The contacted root's dependency read.** The member's push and fetch URLs both name the fork, so §3.2 rules 1 and 2 fall through to rule 4, the committed URL. The fork-only commit is missing there, and the root is refused.<br>That is exactly step 3.6's expectation, and it matches §3.7 (435-440) and §6 row 1012. | Closed |
| P3-8 | **Fix.**<br>• A default `--dry-run` precedes each socket-polled push.<br>• Continue only when every row is `Noop` "up to date", not "behind".<br>• For case 6, only the named repositories may be planned.<br>• A `--check-remotes` dry run is stated to confirm nothing. | **Leg 1** (a `--check-remotes --dry-run` shows planned pushes, so cases 2 to 5 stopped): the guard is now explicitly a default dry run, and 949-950 says a `--check-remotes` dry run cannot confirm anything.<br>**Leg 2** (a default dry run hid a behind member that `--check-remotes` would push): "not behind" now stops the case (950-951).<br>**Leg 3** (case 6's named publication was halted by an unqualified stop): 951-952 admits the named repositories. The dry-run output is recorded (948).<br>**Correction to my round-2 evidence:** round 2 cited `handle_materialize.rs:423` as a checkout "at the lock target". In fact, lock and clone materialization attach a branch member at `<fetch remote>/<branch>`, and fall back to the lock commit only when no upstream ref resolves (`handle_materialize.rs:396-420`). A fresh clone therefore lands "up to date". A "behind" member appears only after a later fetch without an integrating pull. The revised guard stops both. | Closed |

## Changed-range analysis

**What changed.** `git diff --stat f3a395b 0136094` shows one commit, `0136094`, touching five files:
- the object: +146 and −90 lines, now 1067 lines;
- `GwzUrlSchemePushPlan-RemPlan-2.md` (71 lines);
- both round-2 reports (277 and 255 lines);
- one 88-line gwz commit record.

Only the object is in scope.

**Hunks and the dispositions they implement:**
- **Header** (3-19): status and review list. Bookkeeping, and accurate.
- **"Contacted" and "once" wording:**
  - goals 2 and 3 (45-51), the §2.3 note (220-222) and the Phase 3 milestone (702-705);
  - these implement the Safety residual that a contacted root is proven, and my round-2 residual on "at most once".
- **S-P3-5, the malformed-file remedy:** §3.3 (296-300) and step 2.1 (659-660, 676-678). The claim that a bare `gwz materialize` "clones missing members, and moves or detaches checkouts" was verified:
  - `materialize_preflight.rs:44-70`;
  - `handle_materialize.rs:392-428`;
  - gwz-cli `docs/commands/materialize.md:10`.
- **S-P2-5, operation-wide invalidation:**
  - the §3.5 definition (318-323) and rule 1 (350-355);
  - D8 (485-487) and D9 (494-495);
  - the step-1.2 store mapping (617-621) and step 3.3 (760-762, 774-786);
  - §6 row 1009 and §7 (1048-1051).
- **C-P3-6 and the contacted-root rule:** rule 2 (357-381).
- **My round-2 residual, "at most once":** rule 3 (385-386).
- **§3.6:**
  - the summary line (400-402) implements C-P3-6;
  - the missing-dependency refusal (403-407) implements the Safety residual and my round-2 residual that the read URL is named only when it differs.
- **§3.7** (420-447):
  - the opening sentence: C-P3-6;
  - rewind versus deletion: Safety residual;
  - `gwz tag --push`, and the root's contact including its dependency reads: my round-2 residuals.
- **D10's rejected alternative** (503-505): Safety residual.
- **Dependencies:** steps 1.1, 1.2, 3.1 and 3.2 now depend on 0.1 (606, 641, 718, 741), and the summary agrees (983-984). My round-2 residual.
- **C-P3-6, C-P3-7, C-P3-8 and the contacted-root rule across the steps:**
  - step 3.5 (816-817, 827-830, 841-844) and step 3.6 (866-870);
  - step 3.7 (880-881, 890-892) and step 4.1 (931-934);
  - step 4.2 (947-952);
  - §6 rows 1011-1012 and §7 (1039-1044).

**Changes outside the dispositions.** None. Every hunk maps to a RemPlan-2 disposition or to its list of folded residuals.

**Earlier closures.** No hunk reopens round-1 P2-1 to P2-5 or P3-1 to P3-5. Each of these is unchanged, or only extended:
- §2.1;
- the §3.5 "Forced" definition and its table row;
- step 3.2's fan-out, gate and regeneration commands;
- the step-1.2 characterization tests;
- case 4's fresh SSH clone;
- the §3.6 compatibility note;
- step 4.1's unconditional design update;
- the kept-advertisement key.

**New contradictions.** One new finding, P3-9: a test added in this diff is order-dependent on the seam as specified. The remaining wording items are below the bar (§3). The attacks that failed are in §2.

**Architectural classification.** P3-9 is a gap in the test-seam specification, not a new architectural root cause. Operation-wide invalidation and the contacted-root proof are rule corrections inside §3.5. Phases and step numbers are unchanged.

## 0. Evidence base

**Tuple.** The round-1 command, run at the start and end of round 3: identical output (see Baseline).

**Documents read:**
- `GwzUrlSchemePushPlan-RemPlan-2.md` at `0136094`: all 71 lines.
- `git diff f3a395b 0136094 -- dev-docs/GwzUrlSchemePushPlan.md`: in full.
- `GwzUrlSchemePushPlan-ReviewSafety-2.md` at `0136094`: all 255 lines.
- The revised object with line numbers: 43-54, 214-226, 280-519 and 595-1064. That covers every changed hunk in context.
- `git diff --stat f3a395b 0136094` and `git log --oneline f3a395b..0136094`.

**Code verified this round:**
- gwz-core `src/workspace_ops/handle_materialize.rs` 385-440: lock and clone materialization attach a branch member at `<fetch remote>/<branch>`, falling back to the lock commit, and detach on divergence.
- gwz-core `src/workspace_ops/materialize_preflight.rs` 40-70: an unmaterialized member is planned for clone; a dirty member is refused unless destructive.
- gwz-cli `docs/commands/materialize.md:10`: "no target flag, GWZ uses the workspace lock".
- Vendored libgit2 1.9.7 `push.c` 325-362: for a non-forced refspec, the push is refused before transfer when the remote object is missing locally or is not the merge base.

**Carried from rounds 1 and 2** (same commits):
- gwz-core `push_member.rs` 275-317 (parallel member pushes), 291 (host key `git_host(&plan.url)`) and 318-373 (root not attempted after a member failure);
- gwz-core `par_map_per_host.rs` 56-106;
- `publication.rs`; the tracking backend;
- libgit2 `push.c` 164-212 and `remote.c`'s prune code.

**Not run:** no builds, no tests, no network operations, no writes.

## 1. Findings

### [P3-9] Step 3.3's concurrent-spelling invalidation test is order-dependent on the step-1.2 seam

**Location**
- Step 3.3's test list: the header (774-775), "each case ending with the root refused and no root `push_prepared` recorded", and the case at 782-783, "a non-forced push through one spelling and a concurrent forced push through another, where the non-forced push's D8 proof must not count".
- Step 1.2's seam (617-621): "advertisement stores for `ls_remote_url` that a recorded push can move, with several URLs able to map to one store".
- Rule 1's operation-wide invalidation (350-355).

**Violated invariant.** A step's tests must be deterministic and satisfiable on the seam step 1.2 specifies.

**Evidence**
- **Transfer order is not fixed.** Member transfers run through `par_map_per_host` (`push_member.rs:275-317`), which groups them by host (`push_member.rs:291`):
  - members on different host spellings run in different groups;
  - members on one host run up to the per-host limit at once (`par_map_per_host.rs:56-106`).
- **The native backend refuses one of the two orders.** A non-forced push that lands after the forced rewrite is refused before transfer. libgit2 requires the remote object to exist locally and to be the merge base (`push.c:337-357`).
- **The seam says nothing about this.** Its bullet only lets a recorded push move a store. It does not say that a non-forced push onto a store whose ref does not descend to the pushed object is refused.

**State sequence** (seam as specified)
1. **Setup.** Store S, reached both as `…/o/r.git` and as `…/o/r`, has `main` = T.
   - Member A pushes X, a fast-forward over T, without `+`.
   - Member B pushes `+Y`, where Y does not contain X.
   - The root lock names A@X.
2. **Order B then A.** S becomes Y. The seam then moves S to X, with no fast-forward check. Both member transfers are accepted.
3. **The proof.** Invalidation (350-355) discards A's D8 proof and re-reads every dependency.
   - S shows X, so A@X is available.
   - The root is proven and pushed, and a root `push_prepared` is recorded.
   - The test's expected "root refused, no root `push_prepared`" fails.
4. **Order A then B.** S ends at Y, X is missing, and the root is refused. The test passes.
5. **On the native backend.** In order B then A, A's push is refused: the member is `Failed` and the root is not attempted (`push_member.rs:318-373`). The product refuses the root in both orders; only the seam diverges.

**Impact.** The invalidation suite is the closure evidence for Safety round-2 P2-5. It would contain a test whose result depends on thread scheduling, and which passes vacuously in one order. There is no product consequence.

**Required correction.** Choose one:
- add to step 1.2's seam that a recorded non-forced push is refused, as libgit2 refuses it, when the store's ref is not an ancestor of the pushed object (using the ancestry table); or
- have case (iv) fix the transfer order and run it in both orders.

**Closure test.** Case (iv) runs in both transfer orders, and each run ends with the root refused and no root `push_prepared` recorded.

## 2. Invariant analysis

These attacks failed on `0136094`:
- **P3-6, P3-7 and P3-8** were re-traced on their original sequences (closure table).
- **The root proof path after the diff:**
  - every contacted root is proven by this operation's reads or accepted pushes, whether rule 1 publishes it or finds it already on origin (377-380);
  - last-known refs never prove a dependency;
  - any forced or deleting transfer discards all kept evidence and forces reads after the member transfers (350-355);
  - a stale advertisement reached through a host alias or a `.git`-suffix spelling can no longer count, because invalidation no longer depends on URL identity.
- **The "same repository, between destinations" relation** now appears only in last-known condition 3 (330). There, "different" means no last-known ref, so the member is contacted. Invalidation, D8 and D9 all point to the operation-wide rule (350-355, 485-487, 494-495).
- **The §2.3 counts are unchanged and agree with the new wording.**
  - Nothing to publish under the default: the root is not contacted, so 0 (216, 220).
  - One member and the root changed: N+3. Root only: N+2.
  - The counts assume CLI refspecs (223-225), which cannot be forced (§2.1), so invalidation never adds reads to them.
- **The §3.3 claim about `gwz materialize` matches the code.** It clones missing members, attaches branch members at the upstream tip or the lock commit, and detaches on divergence.
- **The §3.7 split between rewind and deletion matches fetch behaviour.** The default refspec is forced, so a fetch updates an existing branch's tracking ref; gwz never prunes (verified in rounds 1 and 2).
- **Every "Depends on" line matches the summary**, including 1.1, 1.2, 3.1 and 3.2 → 0.1 (606, 641, 718, 741; 983-984).
- **The "Reviews" header** states the round-2 verdicts accurately (11-19).

## 3. Risks and next action

**Residual wording below the finding bar:**
- **Invalidation reads with no proof to feed.** Rule 1 (352-354) and step 3.3 (760-762) require reading every root-lock dependency "before the root transfer", unconditionally. For an API forced push that proves no root — the root is unselected, or not contacted under the default (376) — those reads serve nothing. Scope them to operations that prove the root.
- **Goal 2's "once"** (45-48) lacks goal 3's rule-1 qualifier.
- **"The aggregate is `noop` when nothing was pushed"** (§3.6, 411-412) has exceptions. A refused root reports `rejected` with nothing pushed: see rule 3 (387-389), and a contacted root already on origin (377-380, step 3.5 test 843-844).
- **"Publishing the root still reads each dependency"** is narrower than rule 2. The help text (880-881), the GWZDesign text (931-934) and step 4.1 (920-922) say it, but rule 2 also proves a contacted root that is already on origin.
- **§3.7's "the same consequence as a deleted branch"** (435-438) is inexact both ways. For a push URL since removed, a fetch from the committed repository repairs a branch that exists there. For a renamed or re-pointed remote, a pruning fetch does not end it while the remote points at the other repository. D10's read governs root publication in both cases.
- **Step 3.6's rename fixture** (866-868) must remove or rename the existing `origin` first, because `git remote rename` refuses a target name that already exists.

**Next action.** This axis gives GO at `0136094`. With a Safety GO on the same revision, the plan can proceed to acceptance. P3-9 and the residual wording can be folded in before steps 1.2 and 3.3 are implemented, without another Consistency round.
