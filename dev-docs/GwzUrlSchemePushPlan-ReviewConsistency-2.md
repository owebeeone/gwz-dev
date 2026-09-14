# GwzUrlSchemePushPlan — CONSISTENCY-AXIS REVIEW, ROUND 2

**Review object:** `dev-docs/GwzUrlSchemePushPlan.md` at gwz-dev root `f3a395b` (1011 lines). Status line: "plan; decisions D1–D11 taken on 2026-09-14; round-1 review remediation applied; no code yet". Dated 2026-09-14. Round 1 reviewed the same file at `b64f691`.

**Baseline:**
- **Repositories:** gwz-dev `f3a395b` (clean); gwz-core `c9c7a98` (tracked files clean; two untracked documents out of scope); gwz-cli `2c4c0ea`; gwz-py `334f53f`; gwz-core-evidence `b2bf115`; taut `7a5f616`; taut-shape `74f375c`; taut-shape-rs `df13036`; taut-shape-py `864ca0b`.
- **Tuple check:** identical at the start and the end of this round.
- **How sources were read:**
  - documents with `git show f3a395b:<path>`;
  - member code from the working trees, which equal their HEADs while the tuple holds;
  - libgit2 1.9.7 from the vendored `libgit2-sys-0.18.8+1.9.7` crate in the local cargo registry, read-only.

**Date:** 2026-09-14

**Axis:** Consistency, round 2. This round does two things:
- Re-verdicts this reviewer's round-1 findings by re-tracing each original counterexample on the corrected tree.
- Checks the changed range against the controlling graph: code at `c9c7a98`, the parent plan and its acceptance note, `GWZDesign.md`, the gwz-cli and gwz-py surfaces, the process rules, the remediation plan's dispositions, and the document's own sections.

Independent, adversarial, read-only. The round-1 reports and the remediation plan are legitimate inputs; the parallel Safety round-2 review was not seen. Filed verbatim by the lane owner.

**Verdict: GO** — all ten round-1 findings on this axis (P2-1 to P2-5, P3-1 to P3-5) are closed on the corrected tree. Three new P3 findings in the changed range (P3-6, P3-7, P3-8) do not block. None of them is a new architectural root cause.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 | **Fix** (merged with Safety P3-1). §2.1, the §3.5 definition of "forced", D7, steps 3.1, 3.5 and 3.7. | **Text:** §2.1 (144-155) says `--force` sets `OperationPolicy.destructive`, which push ignores; a forced push is only a `+` refspec from an API caller. §3.5 (324-326) defines "forced" by that prefix. Step 3.1 (678-680) keys on the prefix. D7's rationale is corrected (458).<br>**Counterexample re-traced:** `gwz push --force` on a branch behind its tracking ref sends `refspec: None`, so it is an ordinary push. Classification gives behind, so `Noop` without contact (D6).<br>**Pinned by:** step 3.5 (792-793), with the `+` variant contacted (794); step 3.7's parse test (841). | Closed |
| P2-2 | **Fix.** Step 3.2 lists the full fan-out, adds a gwz-cli commit, and gates on a build. | **Text:** step 3.2 (698-711) lists:<br>• `handle_tag.rs`: 3 sites;<br>• `tests/g08.rs`: 13 grep matches (11 literals plus two return types);<br>• gwz-cli `invocation.rs:255`;<br>• gwz-py `client.py` 803 and 813.<br>Commits are gwz-core, gwz-cli, then gwz-py. The gate (705-707) is `cargo build --locked --bin gwz`, plus gwz-py `run_tests.py` without `GWZ_RUST_BIN`. §6 row 954 records it.<br>**Counterexample re-traced:** the gwz-cli literal changes in the same step, so the build that gwz-py's runner performs (`run_tests.py:56-63`) compiles. No other `PushRequest {` literal exists in the three repositories. | Closed |
| P2-3 | **Fix.** Step 1.2 enumerates the seam; identity tests move to the native backend; the tracking-backend override moves to step 3.5; native Git2 tests go in the new step 3.6; step 1.2 is re-budgeted to about 450 LOC. | **Text:** step 1.2 (581-612) now provides committed files, refspec sources resolved to object ids, per-path materialization, heads, ancestry with errors, per-URL advertisements that recorded pushes move, recorded reads and pushes, and a read overlap counter. Identity tests run natively (598-600, 652-654). The override is at 772-773. The Git2 push-URL guard is tested natively (822-823).<br>**Counterexample re-traced:** in 1.2(a), root publication now runs. Captured refspecs are `<oid>:refs/heads/main`, so both `dependency_was_published` (`publication.rs:125-131`) and the dedup hit: N+1 reads and N+1 pushes. Every case that could not be expressed in round 1 now can be: unmaterialized dependency, advertisement without the ref, head ahead of the lock, read overlap. | Closed |
| P2-4 | **Fix.** Case 4 runs on a fresh SSH clone; every case runs a dry run first. | **Text:** case 4 (906-909) runs on `gwz clone git@github.com:owebeeone/gwz-dev.git <dir>` and records the clone's path and root commit.<br>**Counterexample re-traced:** the unpublished commits that made "no pushes" false are absent from a clone of the published workspace. Those were the gwz-dev root 2 ahead and gwz-core 1 ahead at the tuple, plus every plan commit. No unreleased binary runs against gwz-dev.<br>The new dry-run wording is a separate defect, P3-8. | Closed |
| P2-5 | **Fix.** D10 keeps `GWZDesign.md:96-98` true; step 4.1 updates it unconditionally; step 3.7 edits the push help source; step 3.3 updates the module comment; §3.6 becomes a compatibility note; `MachineOutput.md` follows. | **Text:**<br>• D10 (472-478) and rule 2 (360-363): every dependency of a published root is proven by this operation's read or accepted push, so the design sentence holds under the default.<br>• Step 4.1 updates the design unconditionally and adds a grep (880-888).<br>• Step 3.7 edits the help source (830-834).<br>• Step 3.3 updates `publication.rs:1-3` (731).<br>• §3.6 (390-398) replaces "as today" with the `ok`→`noop` note.<br>• Step 4.1 carries it into `MachineOutput.md` (885-886).<br>**Counterexample re-traced:** a default root-only push whose dependencies sit at their tracking refs now reads them (N+2, row 213) rather than trusting local refs.<br>The "not contacted" wording that step 4.1 adds is P3-6. | Closed |
| P3-1 | **Fix.** Step 4.1 depends on 2.1, 3.5 and 3.7, and the summary says the same. | **Text:** step 4.1 (890) matches the summary (934).<br>**Counterexample re-traced:** step 4.1 can no longer start before step 3.5 defines the reasons it documents.<br>**Note:** the remediation plan's broader closure test ("every Depends on line matches the summary") does not hold literally. Steps 1.1 and 1.2 say "Depends on nothing" (579, 612) against `0.1 -> {1.1, 1.2}` (930). That mismatch predates round 1 and is not this finding's counterexample (see §3). | Closed |
| P3-2 | **Fix.** Name `docs/generate_message_catalog.py` and gwz-py's `scripts/regen_protocol.py`. | **Text:** step 3.2 (693-697), §2.1 (156-159), §6 row 954 ("the catalog lists it").<br>**Counterexample re-traced:** following step 3.2 now regenerates the `PushRequest` table in `docs/MessageCatalog.md` (1904-1910 at `c9c7a98`). | Closed |
| P3-3 | **Fix.** Tests and §6 rows for the read-URL refusal text, `--dry-run`, `noop` reasons and aggregate, and the gwz-py summary line. | Each round-1 gap now has a step test and a §6 row:<br>• read-URL refusal: step 2.1 test (648), row 949;<br>• dry run: step 3.5 test (791), row 957;<br>• reasons and aggregate: step 3.5 test (786-787), step 3.7 goldens (842-843), rows 957 and 959;<br>• gwz-py summary line: step 3.8 (851), row 960. | Closed |
| P3-4 | **Fix.** An unmaterialized dependency's kept advertisement is keyed with no identity repo. | **Text:** definition (308-310), step 3.3 key (716-717), test (748).<br>**Counterexample re-traced:** the root changed and one unmaterialized dependency is available. Its pre-transfer read is kept under (none, remote, read URL), and the post-push proof reuses it (334-337). Result: one read. | Closed |
| P3-5 | **Fix.** A forced push is always contacted. | **Text:** table row 351 ("equal \| … \| contacted"); test 795-796.<br>**Counterexample re-traced:**<br>1. An API refspec `+refs/heads/main:refs/heads/main`, with the local branch equal to its tracking ref and the remote moved ahead, is contacted.<br>2. The advertisement is at a descendant, so rule 1 does not make it `Noop`.<br>3. It is pushed with `+` and rewinds the remote, as at `c9c7a98`.<br>§3.7's "skipping is still right" (406-407) now covers only ordinary pushes. | Closed |

## Changed-range analysis

**What changed.** `git diff --stat b64f691 f3a395b` shows one commit, `f3a395b`, touching five files:
- the object, which grew from 756 to 1011 lines;
- the remediation plan (105 lines);
- both round-1 reports (411 and 484 lines);
- one 88-line gwz commit record.

Only the object is in scope.

**Object hunks and the dispositions they implement:**
- **Header, status and review references** (3-14): bookkeeping.
- **Opening sections** (30-57): the D10 owner-direction bullet (30-32), goals 2-3 and the Phase 2 sentence (40-49), and the pruning non-goal (57). These implement D10, Safety P1-1 and this axis's round-1 residual notes.
- **§2.1** (68-179):
  - push options: C-P2-1 and S-P3-1;
  - generators: C-P3-2;
  - seam facts: C-P2-3;
  - the design promise: C-P2-5 and S-P3-4;
  - tracking refs and no pruning: S-P1-1;
  - "no `Noop` today": S-P2-4;
  - remedy text: S-P3-2;
  - the invocation-wide identity default: S-P3-3.

  Every new citation was verified (§0).
- **§2.3 default column and notes** (211-220): D10, and the Safety residual on `:` and shorthand refspecs.
- **§3.2 rule 3** (260-264) and **§3.4** (299-301): residual notes.
- **§3.3** (281-294): S-P3-2 and S-P3-3.
- **§3.5** (306-374): C-P2-1, C-P3-4 and C-P3-5; S-P1-1 through D10; S-P2-1 through the last-known-ref conditions; S-P2-2 through rule 3; S-P2-3 through invalidation.
- **§3.6** (378-398): D11, S-P2-4 and C-P2-5.
- **§3.7** (400-424): S-P1-1, S-P2-1 and S-P2-2.
- **§4** (443-482): wording of D3, D5, D7, D8 and D9; D10 and D11 added.
- **Steps and closing sections:** steps 1.2, 2.1, 3.1 to 3.8, 4.1 and 4.2, the dependency summary, §6 and §7 follow the remediation plan's mapping. The new step 3.6 and the renumbering (old 3.6 → 3.7, old 3.7 → 3.8) are applied consistently wherever those steps are referenced.

**Changes outside the dispositions:**
- §7 "Mixed versions" (990-992) is not in the remediation plan. It repeats a failed attack from the Safety round-1 report and adds no rule.
- §3.7's clause that `gwz tag --push` detects an unsound published root (421-422) goes beyond Safety P2-2 and beyond what the tag path does (see §3).
- Nothing else.

**New findings.** Three, all in text the remediation introduced or reworded:
- **P3-6:** D10's reads versus "not contacted" and rule 1.
- **P3-7:** the rename test versus §3.5 and §3.7.
- **P3-8:** the step-4.2 dry-run guard.

**Architectural classification.** None of the three is a new architectural root cause. Each reconciles wording or a test expectation with a rule that is already decided: D10, the §3.5 conditions, rule 3. D10 and D11 are owner decisions that narrow Phase 3's rules without changing its structure. This axis does not engage the two-round cap.

## 0. Evidence base

**Tuple.** The round-1 command, run at the start and end of round 2; output identical, with root `f3a395b`.

**Documents read:**
- The object at `f3a395b`: all 1011 lines, plus `git diff b64f691 f3a395b -- dev-docs/GwzUrlSchemePushPlan.md` in full.
- `dev-docs/GwzUrlSchemePushPlan-RemPlan.md` at `f3a395b`: all 105 lines.
- `dev-docs/GwzUrlSchemePushPlan-ReviewSafety.md` at `f3a395b`: all 484 lines.
- This axis's round-1 findings as issued. The filed copy was not re-read.
- `git diff --stat b64f691 f3a395b` and `git log --oneline b64f691..f3a395b` (one commit).

**New citations verified.** No repository has changed since round 1.
- **gwz-core:**
  - `transport_support.rs` 60-95: the fetch options set no prune option (66-90).
  - `refs.rs` 280-300: `is_ancestor` maps libgit2 errors through (285-296).
  - `url_scheme_state.rs` 100-120: the refusal text reads "delete the file, or run with --url-scheme manifest to clear it" (109-117).
  - `transport_support/identity.rs` 204-208: an invocation-wide default ignores https (206-207).
  - `handle_repo_lifecycle.rs` 315-345: a non-dry-run `Noop` row carries `planned.action = Noop` and a message (335-341).
  - `protocol/gwz.taut.py` 1632-1634: the `planned` field.
  - `handle_materialize.rs` 423-435: lock materialization checks out the member's branch at the target, and falls back to a detached commit.
- **Vendored libgit2 1.9.7:**
  - `push.c` 160-215, `git_push_update_tips`: maps each ref through the matching fetch refspec; writes "update by push"; deletes the tracking ref for a zero object id; never reads the push URL.
  - `remote.c` 575-590: prune configuration is `remote.<name>.prune`, then `fetch.prune`.
  - `remote.c` 1415-1435: prune runs only when requested or configured.
- **gwz-cli:** `docs/MachineOutput.md` 612-628 (exit codes); a grep for `unsupported_member` in `src` finds nothing.

**Carried from round 1** (same commits): every code, protocol, documentation and process read listed in round-1 §0.

**Not run:** no builds, no tests, no network operations, no writes.

## 1. Findings

### [P3-6] D10's dependency reads are not reconciled with "not contacted", or with rule 1, for members classified `Noop`

**Location**
- **§3.5 rule 2:** the table's "`Noop`, not contacted" cells (351-352) and "Contacted repositories go through rule 1" (364), against the D10 bullet (360-363).
- **§2.3 and tests that count D10 reads:** row 2 of the second table, N+3 (212, 215-217); step 3.5 test (788-789); step 4.2 case 6, "one read for each other dependency" (912-914).
- **§3.6:** reasons (378-382) and summary line (383-385).
- **§3.7:** opening sentence, "these refs decide only whether a member is contacted" (403).
- **§7:** "Stale last-known refs" (987-989), and the credential-helper note limited to "Rule 3's" reads (985-986).
- **Steps:** step 3.7's help text (833) and step 4.1's `GWZDesign.md` text (882-884), both "unchanged repositories are not contacted by default".

**Violated invariant.** The contract's statements about which repositories are contacted, and each row's reason, must match the reads the contract itself requires. The help text and the controlling design must not state behaviour the contract contradicts (L1-24).

**State sequence.** N = 8; one member M and the root changed; default push. This is §2.3 row 2 and step 4.2 case 6.
1. **Classification, before any read.** M is ahead, so contacted. The root is ahead, so contacted. The other seven members are equal: "`Noop`, not contacted" (351).
2. **D10 reads.** The root is published, so D10 proves every root-lock dependency with a read before any transfer (360-363). The seven unchanged members' destinations are read, at their push URLs by §3.2 rule 1, for N+3 in total (212).
3. **Output under §3.6.** The seven rows carry the assumed reason "up to date with origin/<branch> as of the last fetch or push", although this operation read them. The summary line says they were "not contacted". Its own example, "7 repositories unchanged …", describes exactly this seven-of-nine split.
4. **A second reading.** Rule 2's "Contacted repositories go through rule 1" and rule 1's "answers later questions from the kept advertisement" support a reading in which the D10 read makes the member contacted.
   - In step 3.6's deleted-branch fixture the advertisement lacks the source, so under that reading rule 1 no longer marks the member `Noop`.
   - The member would be pushed, recreating the branch and letting the root publish.
   - Step 3.6 (811-815) and §3.7 (408-414) require the opposite: the member stays `Noop` and the root is refused.
5. **§7's note is incomplete.** Under D10 the default also reads unmaterialized dependencies when it publishes the root, so the credential-helper note limited to "Rule 3's" reads understates when prompts can occur.

**Impact**
- In the mainline case where the operation contacts those repositories, three places would say "not contacted": the human output, the push help and `GWZDesign.md` (via step 4.1). The socket count for case 6 contradicts the printed line.
- The contract admits an implementation that step 3.6 would then fail.

**Why P3.** The misstatement concerns which repositories are contacted, not the publication guarantee. Under both readings D10 still refuses an unproven root, and step 3.6 pins the intended reading.

**Required correction**
- **§3.5:** state that a member's classification is final for its row and its push decision, and that a D10 dependency read never changes it. Alternatively, decide the opposite and change step 3.6 and §3.7.
- **Summary line:** count only repositories this operation did not read, or reword it as "not checked for changes".
- **Help and design text:** qualify step 3.7 and step 4.1 as "not contacted unless the root is published".
- **§7:** extend the credential-helper note to D10 reads.

**Closure test.** A step-3.5 seam test for "one member and the root changed" asserts:
- 11 transport calls;
- the seven unchanged rows' reason text;
- the summary line's text, or its absence.

A step-3.7 JSON golden covers the same case.

### [P3-7] Step 3.5's rename test expects contact, which the §3.5 conditions and §3.7 rule out

**Location**
- Step 3.5 test (800-802): "a remote renamed with `git remote rename fork origin` … all contact the remote".
- §3.5 last-known-ref conditions (314-320).
- §3.7 third bullet (415-418).
- Step 3.5 backend bullet: "The tracking backend gains an override" (772-773).

**Violated invariant.** A step's tests must be satisfiable by an implementation of the contract, and a seam test must exercise the rule it names.

**Evidence**
- `git remote rename fork origin` requires that no `origin` exists. It renames `refs/remotes/fork/*` to `refs/remotes/origin/*` and rewrites the default fetch refspec. Safety round-1 P2-1 lists this rename among the writers the condition cannot see.
- After the rename, every §3.5 condition holds for `origin`:
  - the tracking refs lie under `refs/remotes/origin/`;
  - the fetch refspec maps `refs/heads/*` exactly there;
  - no other remote writes there;
  - there is no push URL.

  So a last-known ref exists, and an equal branch is "`Noop`, not contacted" (351).
- §3.7 (415-418) lists "a remote since re-pointed or renamed" as having "the same consequence" (the member is not pushed). It adds that the conditions "exclude what the current configuration shows, not the history of the ref".
- The test is assigned to the step-1.2 seam, where the last-known ref comes from the fake override (772-773). There a rename is only configuration, so the test passes only if the fake is hand-set to answer `None`, which exercises nothing of the Git2 condition.
- Step 3.6's native list (808-823) has no rename case.

**Reproduction**
1. Implement the Git2 query exactly per lines 314-320.
2. Build the rename fixture natively: a fork remote with fetched tracking refs, then `git remote rename fork origin`, with the member branch equal to the moved ref.
3. Run a default push. The member classifies as equal and is not contacted, which contradicts the test.

On the seam the same test passes only with a hand-set `None`.

**Impact**
- The test is vacuous on the seam, or contradicts the contract when run natively.
- It invites an unspecified provenance-style condition, the alternative D10 records as rejected (476-477).
- §6 overstates coverage.
- No publication risk: the D10 read still proves any root that depends on the member. By §3.2 rule 4, that read goes to the committed URL.

**Required correction.** Choose one:
- make the expectation match §3.7 — the member is `Noop`, and a root that needs a commit only the fork holds is refused by the dependency read — and move the case to step 3.6's native list; or
- if contact is wanted, add the condition to §3.5 and remove "renamed" from §3.7's accepted consequences.

**Closure test.** A native step-3.6 case:
- **Setup:** a fork remote renamed to `origin`; the member equal to the moved ref; the root lock naming a commit only the fork holds.
- **Expected:** the member is `Noop`, the root is refused by the dependency read, and the bare root ref is unchanged.

### [P3-8] Step 4.2's `--dry-run` guard cannot be applied to the `--check-remotes` cases or to case 6

**Location**
- **The guard, step 4.2 (897-899):** "Each push has nothing to publish unless the owner names a real publication; run `--dry-run` first and stop if it shows anything to publish".
- **Cases:** 2 to 5 use `--check-remotes` (902-911); case 6 is a named publication (912-914).
- **Rules the guard depends on:**
  - rule 3: "The option skips the classification" (368);
  - dry run: "`--dry-run` shows the classification and still makes no connections" (374);
  - D6: a branch behind its tracking ref is `Noop` (352).

**Violated invariant.** Acceptance cases must be satisfiable as written, and a guard must predict what the socket-polled push will do.

**Reproduction**
1. **Reading the guard literally for case 2.** `gwz push --check-remotes --dry-run` skips classification (368) and makes no reads (374). Every row is therefore a planned push, as a dry run is today (`push_member.rs:447-467`, `545-563`). The guard sees something to publish and stops, so cases 2 to 5 never run.
2. **Running a default `--dry-run` instead.** A member whose branch is behind `origin/<branch>` shows `Noop` "behind …" (D6, 352), and the guard passes. Then the `--check-remotes` push:
   - reads that member;
   - finds its advertisement at a descendant, so rule 1 (331-332) does not make it `Noop`;
   - sends an ordinary push, which the remote rejects as non-fast-forward;
   - marks the member `Failed` and does not attempt the root (`push_member.rs:318-373`), exiting 1.

   The expectation "no pushes" fails. This is reachable whenever a member's remote branch is ahead of the published lock, because lock materialization checks out the member's branch at the lock target (`handle_materialize.rs:423`).
3. **Case 6.** The named publication's dry run necessarily shows something to publish, and the unqualified "stop" clause halts it.

**Impact.** As written, acceptance either stops on four of the six mandatory cases, or proceeds on a guard that misses a push `--check-remotes` will attempt. The worst outcome is a rejected push: no remote change and no publication.

**Required correction.** Define the guard per case:
- run a default `--dry-run` first;
- continue only if every row is `Noop` "up to date with origin/<branch>" (not "behind") — or, for case 6, only the named repositories are planned;
- state that a `--check-remotes --dry-run` makes no reads and cannot confirm there is nothing to publish.

**Closure.** Before each socket-polled push, the acceptance note records that case's default dry-run output, showing only "up to date" rows (for case 6, only the named rows).

## 2. Invariant analysis

The following attacks failed on the corrected tree.

- **Round-1 counterexamples.** Every original counterexample was re-traced and no longer reproduces (closure table).
- **New §2.1 citations are exact.** Checked: `transport_support.rs:66-90`, `refs.rs:285-296`, `url_scheme_state.rs:109-117`, `identity.rs:206-207`, `handle_repo_lifecycle.rs:338`, `MachineOutput.md:616-626`, `push.c:164-212`, `remote.c:582-586` and `1420-1430`, `push_plan.rs:18`, `31-43`, `76-87`, `111-114`, and `publication.rs:86-99`, `199-203`.
  - The claim that push reports no `Noop` today holds for CLI pushes: gwz-cli never sets `unsupported_member`, so a push whose rows are all skipped is reachable only through the API.
- **§2.3 second table re-derived under D10.**
  - Default, "one member and the root changed": 2 reads, plus 7 dependency reads, plus 2 pushes = N+3.
  - Default, "root only, root changed": 1 + N + 1 = N+2.
  - Default, "nothing to publish": the root is not contacted, so 0.
  - With the option, "nothing to publish": N+1, where rule 3's proof of a `Noop` root reuses the eight member reads.
  - Step 3.5's counts and step 4.2 cases 2, 4 and 6 agree.
- **Can the default still publish a root on last-known refs? No.**
  - Rule 2's bullet (360-363) and D10 (472-475) require a read or an accepted push.
  - Last-known refs only classify members.
  - Step 3.6's deleted-branch and removed-fork-URL fixtures pin the refusals.
- **D11 needs no protocol change.** `planned: PlannedChange { action: Noop, message }` already appears in a non-dry-run response (`handle_repo_lifecycle.rs:335-341`). The §1 non-goal stands.
- **The "deleting transfer" half of the invalidation rule needs no test.** It cannot co-occur with a root proof in one push:
  - `PushRequest.refspec` applies to every selected target;
  - a root deletion publishes no lock (`publication.rs:49-51`, `234-236`).
- **The last-known-ref conditions agree with libgit2.**
  - Tracking refs are written through the fetch refspec with no push-URL check, and deleted for a zero object id.
  - Fetch never prunes unless configured.
  - This matches §2.1 and §3.7.
- **The step-1.2 seam supports the new tests of steps 3.3 and 3.5.** It can express two members sharing a URL, forced refspecs moving advertisements, ancestry errors, unmaterialized keys and read overlap. The rename test is the exception (P3-7).
- **Every step 3.x and 4.x dependency line matches the summary.** Step 3.6 depends on step 3.5.
- **Plan format holds.** Phases are unchanged, every budget is at most 450 LOC, and parallel starts are preserved.

## 3. Risks and next action

**Residual risks below the finding bar:**
- **"At most once" has exceptions.** Goal 3, rule 3 and the milestone promise at most one read per destination, but rule 1 re-reads in two deliberate cases:
  - a dependency whose kept advertisement does not prove availability;
  - a repository that received a forced transfer.
- **"Costs one contact" is incomplete for the root.** §3.7's last bullet does not account for this:
  - a contacted root triggers D10's dependency reads in the same round (215-217), so the cost is N+1 contacts;
  - whether a default-mode root that rule 1 marks `Noop` still runs the proof is unspecified; rule 3 covers only the option.
- **`gwz tag --push` does not check the current root.** §3.7's fourth bullet implies it does, but the tag path checks the locks of the tagged root commits it pushes (`handle_tag.rs:93-95`, `287-298`, `383-408`). It detects an unsound root only when that commit carries a tag being pushed.
- **The refusal wording differs between sections.** §3.3 names the read URL only when it differs from the committed URL (278-280); §3.6 names it always (386-387).
- **Pre-existing dependency mismatch.** Steps 1.1 and 1.2 say "Depends on nothing" (579, 612), while the summary shows `0.1 -> {1.1, 1.2}` (930).
- **Seam limit on invalidation.** Invalidation across scheme-different URLs of one repository cannot be expressed with per-URL advertisements. The tested sequences use identical URLs.

**Next action.** This axis gives GO. The lane owner folds P3-6, P3-7 and P3-8 into the plan text before implementation starts; closing them needs no further Consistency round.
