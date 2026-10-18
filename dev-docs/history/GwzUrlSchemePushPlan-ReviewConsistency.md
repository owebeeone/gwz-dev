# GwzUrlSchemePushPlan — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzUrlSchemePushPlan.md` at gwz-dev root `b64f691`. Status line: "plan; decisions D1–D9 taken on 2026-09-14; no code yet" (DRAFT stage). Dated 2026-09-14. Phased plan plus behaviour contract.

**Baseline:**
- **Repositories:** gwz-dev `b64f691` (clean); gwz-core `c9c7a98` (tracked files clean; two untracked out-of-scope documents); gwz-cli `2c4c0ea`; gwz-py `334f53f`; gwz-core-evidence `b2bf115`; taut `7a5f616`; taut-shape `74f375c`; taut-shape-rs `df13036`; taut-shape-py `864ca0b`.
- **Tuple check:** identical at the start and at the end of the review.
- **How documents were read:** the object, the parent plan and the acceptance note via `git show b64f691:<path>`.
- **How code was read:** from the member working trees, which equal their HEADs while the tuple holds, plus `git grep` / `git show` at `c9c7a98` and at gwz-cli/gwz-py HEAD.

**Date:** 2026-09-14

**Axis:** Consistency. The attack is the document against its controlling graph:
- code facts at `c9c7a98`;
- the parent plan and its acceptance note;
- gwz-core `GWZDesign.md`;
- the gwz-cli and gwz-py surfaces;
- the process rules;
- the document's own sections.

Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — five P2 findings block; five P3 findings do not. Every blocking finding has a bounded text remedy. I pre-commit to GO on a revision that resolves P2-1, P2-2, P2-3, P2-4 and P2-5 as specified.

---

## 0. Evidence base

**Tuple.** `git rev-parse --short HEAD` plus the per-member loop, `git status --short` and `git -C gwz-core status --short`, at start and end. The output was identical both times (listed in Baseline).

**Object.** All 756 lines read. Parent `dev-docs/GwzUrlSchemePlan.md` (412 lines) and `dev-docs/GwzUrlSchemeAcceptance-2026-09-12.md` (148 lines) read in full.

**gwz-core `c9c7a98`:**
- **Read in full:**
  - `src/workspace_ops/publication.rs` 1-343
  - `src/workspace_ops/push_member.rs` 1-803
  - `src/git/gitbackend/push_plan.rs` 1-122
  - `src/git/gitbackend/transport.rs` 1-686
  - `src/workspace_ops/handle_tag.rs` 1-408
  - `src/operation/par_map_per_host.rs` 1-116
  - `src/workspace_ops/tests/g01/tracking_backend.rs` 1-527
- **Line ranges read:**
  - `src/git/url_scheme.rs` 1-60, 140-200
  - `src/workspace_ops/url_scheme_state.rs` 55-100
  - `src/git/gitbackend/transport_support.rs` 180-235
  - `src/git/gitbackend/transport_support/identity.rs` 170-225
  - `src/git/gitbackend/backend.rs` 30-60
  - `src/workspace_ops/handle_create_repo.rs` 760-800
  - `src/git/gitbackend/contract.rs` 224-330, 370-390, 950-1004 (plus grep)
  - `src/workspace_ops/tests/g08.rs` 240-315, 540-640, 1031-1081 (plus grep of backends and `PushRequest {` sites)
  - `src/workspace_ops/tests/g01.rs` 300-420
  - `src/git/gitbackend/types.rs` 339-356
  - `src/protocol/generated.rs` 5036-5047
  - `protocol/gwz.taut.py` 524-560, 583-597, 1011-1100, 1211-1227, 1617-1650, 1848-1910
  - `protocol/regen.py` 1-120 (plus grep of 120-299)
  - `docs/MessageCatalog.md` 41, 82, 1904-1910
  - `docs/generate_message_catalog.py` (grep)
  - `src/workspace_ops/handle_repo_lifecycle.rs` 335-341
  - `dev-docs/GWZDesign.md` 80-130, 1595-1640 (plus grep for push, publication, proof, dry run)
- **History:** `git show c9c7a98` (publication.rs diff against v1.0.12 lineage) and `git log --oneline -12`. `git describe` gives `v1.0.12-3-gc9c7a98`.
- **Greps:** every `destructive` use under `src/workspace_ops` and `src/operation`. No hit in the push path.

**gwz-cli `2c4c0ea`:**
- **Source:** `src/clirequest/invocation.rs` 225-275 (plus grep at 106-109); `src/globalargs/render_exit.rs` 70-100; `Cargo.toml` 36-46; `scripts/generate_cli_reference.py` (grep for `--write`/`--check`).
- **Docs:** `docs/commands/push.md` 1-92; `docs/Concepts.md` 99-147; `docs/Releases.md` 45-60; `docs/MachineOutput.md` (grep; exit codes 618-626); headings of QuickStart, Troubleshooting and Concepts.

**gwz-py `334f53f`:**
- **Source:** `src/gwz/cli_shared.py` 280-330; `src/gwz/cli_mutation.py` 205-230; `src/gwz/client.py` (grep: 803, 813); `native/src/dispatch` (grep: `from_cbor` only).
- **Scripts:** `scripts/check_protocol_drift.py` (grep plus 130-175); `run_tests.py` (grep: 5-6, 56-82).
- **Tests:** `src/tests/test_native_git_mutation.py` 30-90.

**Root:**
- `Cargo.toml` 1-40.
- `dev-docs/AgentProcessRules.md`: headings plus L1-04, L1-08, L1-09, L1-10, L1-24, L2-02, L2-03, L3-01, L3-06.
- `dev-docs/GwzProcessOptimization.md` in full.
- `~/.claude/skills/gwz/SKILL.md` (grep).

**Local-only git state (no network):**
- Reflogs of `refs/remotes/origin/main`: gwz-core and gwz-cli `update by push` at 14:21:38; root at 14:22:00.
- `remote.origin.{url,pushurl,fetch}` for all nine repositories: no `pushurl`; fetch refspec `+refs/heads/*:refs/remotes/origin/*`.
- `git rev-list --left-right --count origin/main...HEAD`: root `0 2`, gwz-core `0 1`, gwz-cli `0 0`, gwz-py `0 0`.

**Installed binary.** `gwz --version` reports 1.0.12. `gwz push --help` lists `--force` as "Allow destructive behavior when required".

**Not run:** no builds, no tests, no network operations, no writes.

## 1. Findings

### [P2-1] §2.1 calls `gwz push --force` a forced push; the forced-push inputs of §3.5 and step 3.1 have no defined source

**Location**
- Object lines 112-113: "`gwz push` already has `--force` (a forced push)".
- D7 rationale, lines 330-331: "reads as a kind of `--force` (a forced push on `gwz push`)".
- §3.5 rule 2 table, "forced push" column, lines 256-262.
- Step 3.1 input "whether the push is forced", lines 518-519.
- Step 3.5 test, line 605.
- Step 4.1 documentation.

**Violated invariant.** §2.1 is "Facts this plan rests on". D6's decided outcome is that a branch behind its last-known ref on an ordinary push is `Noop` without contact.

**Evidence**
- `--force` is the global switch mapped to `OperationPolicy.destructive = Allow`: gwz-cli `src/clirequest/invocation.rs:106-109`, and `GWZDesign.md:1632` ("`--force  OperationPolicy.destructive = allow`").
- The push request is built with `refspec: None` (`invocation.rs:255-259`). gwz-py's `handle_push` passes only `remote` (`cli_mutation.py:212-216`).
- No push-path code reads `destructive`. The only readers are create_repo, materialize, merge validation, pull head and bootstrap.
- Force is decided only by a `+` refspec prefix (`publication.rs:44`). `resolve_push_refspec` (`push_member.rs:643-663`) never adds `+`.
- Result: neither CLI can issue a forced push at `c9c7a98`. Only an API caller setting `PushRequest.refspec = "+…"` can.

**Reproduction**
1. Implement step 3.1's "forced" from §2.1 as `policy.destructive == Allow`.
2. Take a member whose local `main` is behind `origin/main` after a fetch, and run `gwz push --force`.
3. The classification "forced, behind" says contacted, so the member is read.
4. The captured plan is `<oid>:refs/heads/main` without `+` (`push_plan.rs:114`), so the remote rejects a non-fast-forward.
5. The member is `Failed`, root publication is "not attempted" (`push_member.rs:363-372`), and the exit code is 1.

D6 prescribes `Noop` without contact for what is in fact an ordinary push. The other reading of §2.1 — making `--force` add `+` — would turn the global "allow destructive" switch into remote rewinds. That is an undecided destructive behaviour change.

**Impact.** A destructive-adjacent path gets two contradictory implementations, and step 3.5's forced test is satisfiable either way. Step 4.1 would document a forced-push flag that does not exist.

**Required correction**
- Correct §2.1: `--force` sets `OperationPolicy.destructive`, which push ignores. A forced push exists only as a `+`-prefixed `PushRequest.refspec` (API only).
- Define "forced" in §3.5 and step 3.1 as "the captured refspec carries `+`" (the `publication.rs:44` rule), and state that `policy.destructive` does not affect classification.
- Correct D7's rationale sentence; the decision itself stands.

**Closure test.** A core test (step 3.5):
- a request with `policy.destructive = Allow` and `refspec: None` on a branch behind its last-known ref gives `Noop` ("behind origin/main …") and zero transport calls;
- the same state with `refspec = "+refs/heads/main:refs/heads/main"` contacts the remote.

Plus a gwz-cli parse test that `push --force` still yields `refspec: None`.

### [P2-2] Step 3.2's `PushRequest` fan-out omits the gwz-cli production literal, so its own gate fails and gwz-cli stops compiling until step 3.6

**Location**
- Step 3.2, lines 533-534: "Every `PushRequest { .. }` literal gains `remote_check: None`; there are three in `handle_tag.rs` alone. The compiler finds them."
- Line 541: "Commits: gwz-core, then gwz-py."
- Dependency summary `3.2 -> {3.6, 3.7}`.
- Conventions, lines 354-355: suites of touched members run green.

**Violated invariant.** Step 3.2 "Depends on nothing" and is parallel-friendly. Every step's gate is satisfiable. Every root lock records a compilable tuple.

**Evidence**
- `PushRequest` is an exhaustive struct `{meta, remote, refspec}` (`src/protocol/generated.rs:5038-5043`).
- Literal sites:
  - gwz-core `handle_tag.rs`: 120, 168, 292.
  - gwz-core `tests/g08.rs`: 24, 95, 170, 296, 553, 572, 675, 740, 824, 953, 992 (11 sites).
  - gwz-cli `src/clirequest/invocation.rs:255`: `gwz_core::PushRequest { remote, refspec: None, meta }`, with no `..Default::default()`.
- gwz-cli depends on gwz-core by path under `[dependencies]` (`gwz-cli/Cargo.toml:43`) and is the root cargo workspace's only member (root `Cargo.toml`).
- gwz-py's `run_tests.py` (5-6, 56-63) runs `cargo build --locked --bin gwz` in the sibling gwz-cli when `GWZ_RUST_BIN` is unset.

**Reproduction**
1. Land step 3.2 as scoped: schema, regeneration, gwz-core literals, gwz-py bindings.
2. Run the touched gwz-py suite. The runner builds gwz-cli, which fails with a missing field `remote_check` at `invocation.rs:255`, so the gate fails.
3. If the gate is bypassed with a pre-built `GWZ_RUST_BIN` (the stale-binary hazard recorded in the parent acceptance note §5), the root locks of steps 3.2, 3.3, 3.4 and 3.5 pair gwz-core with a gwz-cli that does not compile.

**Impact.** The declared gate is unsatisfiable as scoped. Four steps leave broken intermediate tuples, and the step-3.6 owner inherits a compile failure from another lane.

**Required correction**
- Include `invocation.rs:255` in step 3.2 with a gwz-cli commit.
- State the full fan-out: 3 + 11 sites in gwz-core, 1 in gwz-cli, and the two gwz-py constructor calls at `client.py:803, 813`.
- List the commits as "gwz-core, gwz-cli, then gwz-py".

**Closure test.** After step 3.2's commits, `cargo build --locked --bin gwz` from the gwz-dev root succeeds, and gwz-py `run_tests.py` without `GWZ_RUST_BIN` is green.

### [P2-3] The step-1.2 seam, as specified, cannot produce the counts and cases that steps 1.2, 2.1, 3.3, 3.4 and 3.5 assign to it

**Location**
- Step 1.2 seam bullets and tests (a)–(c), lines 440-459.
- Step 2.1 "Tests on the step-1.2 seam", lines 480-495.
- Step 3.3 tests, lines 557-565.
- Step 3.4, lines 580-581.
- Step 3.5 tests, lines 596-606.
- §7, lines 741-742: step 1.2 exists so later changes "are measured against a pinned sequence".

**Violated invariant.** L1-03: characterize before changing. Each step's tests must be writable on the seam step 1.2 specifies.

**Evidence** (tracking backend at `c9c7a98` and contract defaults)
1. **No committed-file serving.** The tracking backend has no `read_file_at_commit`, and the contract default is `unsupported_backend` (`contract.rs:312-319`). `root_dependencies` needs it (`publication.rs:243, 247-248`). `push_root` calls it in dry run (`push_member.rs:533-544`) inside the F7 preflight (`push_member.rs:138-163`). Any root-including push on this seam is therefore `Rejected` before any read.
2. **Unresolved refspecs.** `prepare_push` returns the input refspec as given (`tracking_backend.rs:459-466`), for example `refs/heads/main:refs/heads/main`. `dependency_was_published` compares the refspec source with the lock commit (`publication.rs:125-131`); the Git2 `prepare` resolves that source to an object id (`push_plan.rs:111-114`). With an oid lock commit the shortcut cannot hit. Step 1.2 changes only the URL.
3. **Identity options refused.** The default `with_transport` refuses any transport options (`contract.rs:224-238`). Step 2.1's identity test therefore cannot reach the existing refusal message (`identity.rs:200-204`).
4. **Every path materialized.** `is_repository` is always true (198-200), so step 2.1's unmaterialized-dependency test cannot be expressed.
5. **Fixed repository state.** One fixed advertisement (269-274, 469-477), a fixed head (413-419), and `is_ancestor` always true (524-526). Step 3.3's "advertisement without the destination ref", its "head ahead of lock commit" case, and any D8 negative case cannot be expressed.
6. **No read concurrency counter.** Overlap trackers exist only for fetch and push (28-29, 262-267, 440-451), so step 3.4's per-host limit for reads cannot be counted.
7. **The alternative seam records nothing.** "The backend the g08 publication tests build" is the real `Git2Backend` (`g08.rs:3, 243`), which records no calls.
8. **Production guard untested.** Step 3.5's Git2 last-known-ref mapping and push-URL condition — the guard that decides whether a push is skipped in production — has no native-backend test named.

**Reproduction**
1. Implement step 1.2 exactly as bulleted, then write test (a) with two members and the root.
2. `handle_push` returns `Rejected` with an unsupported `read_file_at_commit` error on `@root`, and zero `ls_remote_url` calls.
3. Add only committed-file serving. The test now records 3 pre-transfer reads plus 2 proof reads, which is 2N+1 = 5, not the specified N+1 = 3.
4. Step 2.1's "1.2(c) flips: … a shortcut hit, and N+1 reads" cannot hold either.

**Impact.** Either the pinned baseline is wrong — so step 2.1's "unchanged, byte for byte" and step 3.3's "move to these counts" pin the wrong sequence — or the seam grows silently past its ~300 LOC budget. The production skip guard stays untested.

**Required correction**
- Enumerate the seam in step 1.2: committed manifest and lock per commit; `prepare_push` resolving sources to object ids; per-path materialization, heads, ancestry and per-URL advertisements; identity-option emulation (or route that test explicitly to a Git2 fixture); `ls_remote_url` overlap tracking; and an override of step 3.5's last-known-ref query.
- Re-budget step 1.2.
- Add a native (local bare remote) test for the Git2 mapping and the push-URL condition.

**Closure test**
- Test 1.2(a) on the seam records exactly N+1 `ls_remote_url` and N+1 `push_prepared` calls.
- A native test shows that a remote whose `pushurl` names a different repository, with an equal tracking ref, is contacted.

### [P2-4] Acceptance case 4 runs an unreleased binary's push against gwz-dev, where "no pushes" is unsatisfiable

**Location**
- Step 4.2, lines 657-662: a scratch-installed binary; "Each push has nothing to publish unless the owner names a real publication".
- Case 4, lines 669-670: "gwz-dev itself (SSH), whole push with `--check-remotes`: N+1 port-22 sessions and no pushes".
- Header, lines 11-12: installed 1.0.12 for every workspace operation; also AgentProcessRules L3-01.

**Violated invariant.** Acceptance cases are satisfiable as written, and none of them publishes by accident.

**Evidence**
- At the tuple, the gwz-dev root is 2 commits ahead of `origin/main` and gwz-core is 1 ahead.
- The header records the base as unpushed.
- Every step adds member commits plus a root lock commit.
- The plan has no push step. The parent acceptance note §6-7 deferred pushes to the release.

**Reproduction**
1. Finish steps 0.1–4.1.
2. Run case 4 in gwz-dev with the scratch-installed binary.
3. Rule 3 reads all 9 destinations. Rule 1 finds the new gwz-core, gwz-cli, gwz-py and root commits absent from their advertisements, so all four are pushed.
4. The expectation "no pushes" fails, and unreleased work is published by an unreleased binary outside any release step.

**Impact.** The SSH-workspace acceptance evidence cannot be recorded as specified. Running the case causes an unplanned publication.

**Required correction.** Choose one:
- run case 4 on a fresh SSH clone of the published workspace (`gwz clone git@github.com:owebeeone/gwz-dev.git <dir>`, so the root read is also port 22); or
- add the precondition that gwz-dev has been pushed with installed 1.0.12, confirmed by a `--dry-run` showing every row equal to `origin/main`.

**Closure.** The acceptance note records the case-4 workspace path and commit, plus a `--dry-run` showing nothing to publish immediately before the socket-polled push.

### [P2-5] Phase 3 weakens GWZDesign's root-publication proof guarantee without naming the superseded text; step 4.1's trigger misses it

**Location**
- §3.5 rule 2, second bullet, lines 265-267.
- §3.7, lines 291-296.
- Step 4.1, lines 650-651: update GWZDesign only "if [it] names the committed URL as what the proof reads, or describes the second read".
- §3.6, lines 284-285: aggregate `Noop` "as today".

**Violated invariant.** L1-08: name the superseded text. L1-24: documentation is part of the compatibility surface. The plan must agree with its controlling design.

**Evidence**
- `GWZDesign.md:96-98`: "It waits for successful member transfers and proof that all member objects named by the committed root lock are available at their destinations, including partial and root-only pushes."
  - That paragraph names neither the committed URL nor a second read, so neither of step 4.1's triggers fires.
  - Under the default mode a root-only push proves dependencies from last-known refs without contacting them. §3.7 says such a root "can name a commit its member remote no longer advertises".
- gwz-cli `docs/commands/push.md:77-80` repeats the guarantee ("root-only pushes also require proof … available"). Step 4.1's list (Concepts, Troubleshooting, QuickStart) omits it.
- On "as today": at `c9c7a98` push never emits `Noop` rows (`push_member.rs:447-493, 784-803`). Step 0.2 recorded a nothing-to-publish push as every row `ok`.
  - Phase 3 turns row and aggregate status for unchanged repositories from `Ok` to `Noop`.
  - The exit code stays 0 (`render_exit.rs:82-88`; `MachineOutput.md:622-626`).
  - This is a machine-visible change that the plan presents as unchanged.

**Reproduction.** Land Phase 3 and step 4.1 as written:
- `GWZDesign.md:96-98` still promises proof of availability at destinations;
- a default root-only push publishes on last-known refs;
- a reviewer checking code against GWZDesign flags the default as a violation, or a consumer relies on the stated barrier.

**Impact.** The controlling design asserts a stronger dependency barrier than the shipped default provides. The owner-accepted risk (D5) is invisible in the design and in the command reference.

**Required correction**
- Add a superseded-clause list covering:
  - `GWZDesign.md:96-98`, qualified as: proof by this operation's read, by an accepted push (D8), or by default from last-known refs (D5), with `--check-remotes` to force reads;
  - the push help source behind `docs/commands/push.md` "Publication and authentication" (edited in step 3.6).
- Replace "as today" in §3.6 with an explicit compatibility note: unchanged repositories report `Noop` rows and aggregate, the exit code is unchanged, and the note carries into `MachineOutput.md` in step 4.1.
- Make step 4.1's GWZDesign update unconditional.

**Closure.** Step 4.1's diff qualifies `GWZDesign.md:96-98` and the push help source, and `MachineOutput.md` states the push `Noop` meaning.

### [P3-1] Step 4.1's stated dependencies contradict the dependency summary

**Location.** Step 4.1, line 654: "Depends on 2.1 and 3.6". Summary, line 694: `{2.2, 3.5, 3.6, 3.7} -> 4.1`.

**Invariant.** L1-04: no package starts without its prerequisites. The operator's rule that agents can pick up steps independently requires one unambiguous graph.

**Evidence.** Step 3.6 needs 3.5 only "for the end-to-end assertions" (line 621), so 3.6 can land before 3.5. Step 4.1 documents "what each `Noop` reason means" (lines 643-644), and those reasons are defined in step 3.5.

**Reproduction.** An agent starts step 4.1 once 2.1 and 3.6 land, which the step text allows. It documents reasons that are not implemented yet; the summary forbids this.

**Correction.** Make the step text and the summary agree: 4.1 depends on 2.1, 3.5 and 3.6.

**Closure.** Every "Depends on" line matches the summary graph.

### [P3-2] Step 3.2 attributes message-catalog regeneration to `protocol/regen.py`, which does not produce it

**Location.** Lines 531-532.

**Invariant.** L2-02 and L1-24: regenerate every generated surface.

**Evidence**
- `regen.py` produces `generated.rs`, `cbor.rs`, the corpus and the checked-artifact outputs (docstring lines 18-30).
- `docs/MessageCatalog.md` is generated by `docs/generate_message_catalog.py` (line 173) and has a `PushRequest` field table (lines 1904-1910).
- That generator has no check mode and no test or CI invocation in gwz-core.
- The parent plan §2.7 (lines 198-199) named both commands.

**Impact.** Following step 3.2 leaves the catalog's `PushRequest` table without `remote_check`, and no gate notices.

**Correction.** Add `python docs/generate_message_catalog.py`, and name gwz-py's `python scripts/regen_protocol.py`.

**Closure.** After step 3.2, the catalog's `PushRequest` table lists `remote_check`.

### [P3-3] Test lists and §6 are not reconciled with the §3 contract

**Location, evidence and gaps**
1. §3.3 says the proof refusal names the read URL (lines 213-215). Step 2.1 implements it (line 476), but neither its tests nor §6 cover it.
2. §3.5 says "`--dry-run` shows the classification and still makes no connections" (line 274). There is no test and no §6 row.
3. §3.6 requires `Noop` reasons in JSON and a `Noop` aggregate. Step 3.5's tests (lines 597-606) assert transport calls only.
4. The §6 row at line 715 claims a gwz-py "human summary" test, but step 3.7 (lines 624-627) never implements the §3.6 summary line. That is an L2-03 parity gap.

**Impact.** Regressions in these operator-visible rules pass every listed test.

**Correction.** Add a test and a §6 row for each item. Add the summary line to step 3.7, or narrow the row.

**Closure.** Every bullet of §3.3, §3.5 and §3.6 maps to a §6 row.

### [P3-4] Step 3.3's key "as today" cannot hold unmaterialized reads, which contradicts rule 3's "read once"

**Location.** Step 3.3, lines 543-544. §3.5 rule 3, lines 270-272. D9.

**Evidence**
- `ReadPreflight` keys on `(PathBuf, String, String)` (`publication.rs:15-35`).
- Unmaterialized dependencies are read without an identity repository and never recorded (`publication.rs:199-203`).
- The post-push proof reads them again (`publication.rs:79-85`).
- D9 answers only from a kept advertisement; "anything else is read".

**Reproduction.** Take a workspace with gwz-core-evidence unmaterialized and run `gwz push --check-remotes` with the root changed. That dependency is read twice, but rule 3 and the §2.3 note say once. The step-1.2 seam (`is_repository` always true) cannot catch this.

**Correction.** Keep unmaterialized advertisements, for example keyed with `Option<PathBuf>`. Or narrow rule 3 and D9 to materialized dependencies and amend the §2.3 note.

**Closure.** With `always`, a seam test with one unmaterialized dependency records exactly one read for it.

### [P3-5] §3.5's forced/equal cell contradicts §3.7's "skipping is still right" when the remote moved ahead

**Location.** §3.5 table row "equal", line 258. §3.7 first bullet, lines 291-292.

**Evidence.** A forced update sets the destination to the source regardless of the remote's state. Today `perform_push` sends the `+` refspec (`transport.rs:498-507`) and the remote rewinds. Under the default, a local branch equal to its last-known ref is `Noop` and nothing is sent. This is reachable through the API only (see P2-1).

**Reproduction**
1. Push X.
2. Another clone pushes Y on top.
3. Without fetching, submit a `PushRequest` with `refspec "+refs/heads/main:refs/heads/main"`.
4. The default mode reports `Noop` ("up to date with origin/main as of the last fetch or push"), and the remote stays at Y. Today it rewinds to X.

**Impact.** An explicit forced update is silently not performed. The risk list the owner accepted (D5, §3.7 second bullet) does not include this case, and no test covers it.

**Correction.** Either contact the remote for every forced push, or record this case in §3.7 as part of the accepted risk and add a step-3.5 test.

**Closure.** A step-3.5 test with a forced request, an equal last-known ref and an advertisement at a descendant commit asserts the chosen outcome.

## 2. Invariant analysis

These attacks failed. They are what the pre-commitment rests on.

- **§2.1 citations exact.** Every cited file and line matches at `c9c7a98`:
  - `push_plan.rs:12-17`
  - `push_member.rs:222-257, 233-237, 306-311, 533-541, 764`
  - `transport.rs:459-472, 471, 485-490`
  - `publication.rs:11-35, 68-108, 92, 110-133, 116-133, 143-153, 187-191, 192-216, 289-297`
  - `url_scheme.rs:161-177`
  - `url_scheme_state.rs:72-91`
  - `handle_tag.rs:125-130, 151, 163-175, 287-298`
  - `transport_support.rs:195-235`
  - `backend.rs:44`
  - `identity.rs:200-204`
  - `handle_create_repo.rs:779`
  - `par_map_per_host.rs:39`
  - `tracking_backend.rs:421-423, 453-467, 469-477`

  The reflog times and the "no gwz-dev pushurl" statement (D2) hold. The contract has `read_ref` and `is_ancestor` but no last-known-ref or ahead/behind query, and `GitStatus` has neither field. The only false fact is `--force` (P2-1).
- **First §2.3 table re-derived from the call graph.**
  - `c9c7a98`, SSH workspace: N+1 pre-transfer reads, 0 dedup reads, N pushes, 0 proof reads (shortcut), 1 root push = 2N+2.
  - https clone: N+1 https reads, N SSH dedup misses (URL key), N https pushes, N SSH proof reads (`plan.url != dependency.url`), 1 https push = 2N+2 https + 2N SSH (34).
  - 1.0.12: the pre-`c9c7a98` shortcut matched only `+` refspecs (commit diff), adding N proof reads = 3N+2 (26, as measured).
  - Root-only: 1 + N + N + 1 = 2N+2 in every column.
  - Step 0.2's socket order (443×9, 22×8, 443×8, 22×8, 443×1) is exactly the code order.
- **Second §2.3 table re-derived from §3.5.**
  - Check once: 9, 11 and 10.
  - Default: 0, 4 and 2.
  - The option column equals check once and matches rule 3.
  - Step 4.2 counts (case 2: 9 on port 443; case 4: 9 on port 22; case 6: 4) agree, apart from P2-4's precondition.
- **False-composition attack on §3.1, §3.2, D8, D9 and rule 2 failed.**
  - `user@`, credential-bearing, `ssh://`, port and case variants never become read URLs.
  - `derive` is exact on known hosts. Forks and `DISABLE` push URLs fall through.
  - D8 and D9 are keyed to the same URL.
  - Last-known refs are used only when the push URL is the same repository as the fetch URL.

  No P0 or P1.
- **Agreement with the parent plan holds.**
  - Parent §2.3: pull and push use the configured remote.
  - Parent §1 non-goal: no remote rewriting (object §3.4).
  - Parent §2.8: repo sync keeps the manifest URL.
  - D4 extends the parent's identity refusal.
  - Precedence matches `resolve_url_scheme`.
- **Existing push tests survive Phase 3.** The g08 native pushes and gwz-py's `test_native_push_dry_run_and_push_to_local_bare_remote` use fresh bare remotes with no tracking refs, so they classify as "no last-known ref", are contacted, and still report `Ok`. A `Noop` aggregate still exits 0.
- **Noop reason carrier exists.** `planned: PlannedChange { action: Noop, message }` is already used (`handle_repo_lifecycle.rs:335-341`), so §1's "no other protocol change" is satisfiable.
- **Surface claims hold.**
  - gwz-py's drift pin path and dated-note style;
  - `generate_cli_reference.py --write/--check`;
  - the gwz-py push registration in `cli_mutation.py`;
  - tag publication "no dedup, empty published map".
- **Plan format holds apart from P2-2 and P3-1.** Phases are milestones, foundational first. Steps are single goals with sub-500 LOC budgets. Steps 1.1, 1.2, 3.1 and 3.2 start in parallel.

## 3. Risks and next action

Residual items below the finding bar:
- **Proof keyed by remote name.** D9 keys proofs by remote name. With `--remote X` pointing at the committed fetch URL under another name, the pre-push advertisement is used, so root publication is refused, a safe-direction false refusal. Today a re-read succeeds.
- **"Byte-identical" SSH reads.** §3.4's claim holds only when member remotes equal the committed URLs. An scp remote under an `ssh://` manifest URL changes the read URL.
- **D3 wording.** D3's "refuses push" is broader than step 2.1's lazy resolution, which refuses only with an unmaterialized dependency.
- **Scheme precedence.** §3.2 rule 3 ("the way materialize resolves it") omits `GWZ_URL_SCHEME`, which the CLI applies only to clone and materialize.
- **Switch recipe.** Step 4.1's recipe for switching an existing SSH workspace does not record `.gwz/url-scheme.yml`, so unmaterialized dependencies keep reading over SSH.
- **Goal 3 wording.** Goal 3's "contacts each destination once" conflicts with the N+3 rows, where changed repositories are read and pushed. The milestone's "read once" is the accurate form.
- **Pre-existing design mismatch.** `GWZDesign.md:1601` lists `--refspec` for push, which gwz-cli lacks.

**Next action.** The drafter revises the object for P2-1 through P2-5, which are text-only changes to §2.1, §3.5, §3.6, and steps 1.2, 3.1, 3.2, 3.5, 4.1 and 4.2. Fold in P3-1 through P3-5, then file a bounded remediation re-review against this report.
