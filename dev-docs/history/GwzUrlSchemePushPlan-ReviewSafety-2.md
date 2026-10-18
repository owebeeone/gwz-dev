# GwzUrlSchemePushPlan — SAFETY-AXIS REVIEW (round 2)

**Review object:** `dev-docs/GwzUrlSchemePushPlan.md` at gwz-dev root `f3a395b`. Status: plan; decisions D1–D11 taken on 2026-09-14; round-1 remediation applied; no code. Dated 2026-09-14.
**Baseline:** gwz-dev `f3a395b` (clean); gwz-core `c9c7a98` (tracked files clean; two untracked documents out of scope); gwz-cli `2c4c0ea`; gwz-py `334f53f`; gwz-core-evidence `b2bf115`, taut `7a5f616`, taut-shape `74f375c`, taut-shape-rs `df13036`, taut-shape-py `864ca0b`. The tuple was identical at the start and the end. Members are unchanged since round 1, so round-1 source reads carry forward. This round's reads came from the working trees, which equal their commits.
**Inputs:** `dev-docs/GwzUrlSchemePushPlan-RemPlan.md`, both round-1 reports, owner decisions D10 and D11, and `git diff b64f691 f3a395b -- dev-docs/GwzUrlSchemePushPlan.md`.
**Date:** 2026-09-14
**Axis:** SAFETY — degraded and mixed-version paths, irreversible steps and their preconditions, how far disclosure and credentials reach, stuck states the text's own rules allow, whether "never worse than today" claims survive concrete interleavings, and scope creep. Independent, adversarial, read-only. The other axis re-verdicts in parallel; nothing here relies on its round-2 report. Filed verbatim by the lane owner.

**Verdict: NO-GO**
- All nine round-1 findings are closed on their original state sequences.
- One new P2 (P2-5) blocks: the new invalidation rule decides "same repository" with a relation that treats unequal spellings of one repository as different repositories.
- One new P3 (P3-5) does not block: the replacement remedy for the round-1 P3-2 refusal is a command with side effects.
- Neither is a new architectural root cause, so the two-round cap does not fire. A round limited to non-architectural corrections is permitted.
- I pre-commit to GO on a revision that resolves P2-5 as specified.

---

## Prior-finding closure table

"Verified" means the original round-1 counterexample was re-traced against the revised text.

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| **P1-1** | Fixed by D10. Last-known refs never prove a root dependency, and publishing the root reads every dependency. §3.7 now states that gwz does not prune. Closure test in step 3.6 (native). | Sequence: `spike` pushed, then deleted remotely, then a non-pruning fetch, then the root lock records `m@F`, then a default push. Re-traced:<br>• `m` equals its last-known ref, so it is `Noop` and not contacted (§3.5 table, 351).<br>• The root is contacted and not `Noop`, so it is published. D10 (360-363) proves `m@F` with a read before any transfer.<br>• The kept advertisement lacks F and no advertised ref descends from it, so it does not prove availability (334-337). The proof reads again ("Otherwise it reads") and refuses the root with the §3.6 message.<br>• Nothing is pushed.<br>• §3.7 (408-414) now says truthfully that a fetch does not repair a deletion, and that the member push is still skipped. That skip is recorded, and it is not false composition.<br>• The step-3.6 test asserts the bare root ref is unchanged (811-815). | **CLOSED** |
| **P2-1** | Fixed. D10 closes the false-composition half. The last-known ref requires the standard namespace mapping, no other remote writing into that namespace, and a same-repository push URL. Refs written under an earlier configuration remain a member-only consequence in §3.7. Step 3.6 tests. | Sequence: a fork `pushurl` writes `refs/remotes/origin/main = G`, the push URL is removed, the root lock records `m@G`, then a default push. Re-traced:<br>• `m` equals the polluted ref, so `Noop`. This consequence is recorded (§3.7, 415-418).<br>• The root is published. D10 reads the committed URL, G is absent, and the root is refused.<br>• Variants: another remote's refspec writing into `refs/remotes/origin/*` breaks condition 2 (319), so there is no last-known ref and the member is contacted. `+refs/heads/*:refs/heads/*` breaks condition 1 (317-318), with the same result. A rename, a re-point, or refs copied by a local clone skip only the member push, and the root read still refuses.<br>• Step 3.6 covers the fork case (816-818) and the excluded layouts (819-821). Step 3.5 names `git remote rename fork origin` (800-802). | **CLOSED** |
| **P2-2** | Fixed. Under `always`, a selected root that rule 1 makes `Noop` still runs the dependency proof. §3.7 states what the option detects. Step 3.5 tests. | Sequence: root S is published, `m` is rewound to R0 dropping L, local `m` is reset to R0, then `gwz push --check-remotes`. Re-traced:<br>• `m` reads R0, equal to its source, so `Noop`. The root reads S, also `Noop`.<br>• Rule 3 (370-372) still runs the proof after member transfers. The kept advertisement of `m` (R0) does not prove L, so the proof reads, and the root is `Rejected` with today's message.<br>• The root-only variant gives the same result.<br>• §3.7 (419-422) and step 4.1 (874-875) state what the option detects. Tests at 797-799. | **CLOSED** |
| **P2-3** | Fixed. A forced or deleting transfer invalidates kept advertisements and D8 proofs for its repository, across identity repos and remote names; those dependencies are read after all transfers. D9's rationale is corrected. D10 removes the snapshot question in sequence (b). Step 3.3 tests. | Re-traced as written in round 1, where every example uses the same URL spelling:<br>• (a) `app` and `site` both reach `git@github.com:o/r.git`. `app`'s forced push to that URL invalidates `site`'s kept advertisement (340-344), so `site` is read after the transfers and the root is refused.<br>• (b) The default no longer proves from last-known refs (D10). `m`'s forced push invalidates its own D8 proof, so the dependency is read and the root is refused.<br>• (c) Concurrent forced pushes to one URL are invalidated, read and refused.<br>• D9's rationale (468-470) is corrected. Tests at 742-747.<br>• The rule matches repositories with §3.5's "same repository" relation (311-313). My round-1 correction prescribed §3.1's relation for this; that was the wrong direction for invalidation. Unequal spellings of one repository escape the rule: new **P2-5**. | **CLOSED** on the original sequences; the remaining gap is new P2-5 |
| **P2-4** | Fixed by D11. §3.6 states the `ok` to `noop` change with the exit code unchanged, carries the reason in `planned.message`, and says a checked result needs `--check-remotes`. Also `MachineOutput.md` (step 4.1), gwz-py rendering (step 3.8), and JSON goldens (step 3.7). | Sequence: a script requires `aggregate_status == "ok"` after pushing an unchanged workspace. §3.6 (390-398) now documents `noop` rows and aggregate, exit code 0, and that JSON separates checked from assumed only by message text.<br>Under `--check-remotes` rule 3 skips classification (368-369), so every `Noop` row is "already on origin", meaning checked. A script can therefore demand a checked result.<br>The owner accepted the untyped reason (D11, 479-482). A `PlannedChange { action: Noop }` precedent exists (`handle_repo_lifecycle.rs:335-341`). | **CLOSED** (by D11) |
| **P3-1** | Merged into C-P2-1: §2.1 corrected, "forced" defined by the `+` prefix, `--force` has no classification effect, D7 rationale corrected, step 3.1 keyed on the prefix, tests in steps 3.5 and 3.7. | §2.1 (144-155) now matches the code: `invocation.rs:106` sets `destructive`; `invocation.rs:255` sends `refspec: None`; gwz-py passes only `remote`; a forced push exists only as an API `+` refspec.<br>§3.5 (324-326) defines "forced" by the prefix, and step 3.1 (678-680) takes it as input.<br>An implementer can no longer key classification on `policy.destructive`. The step-3.5 tests (792-794) pin both states, and the step-3.7 parse test pins `--force` to `refspec: None`. | **CLOSED** |
| **P3-2** | Fixed. The push-path refusal names "delete the file, or run `gwz materialize --url-scheme manifest`". D3 is narrowed to pushes with an unmaterialized dependency. Step 2.1 test. | Sequence: a malformed `.gwz/url-scheme.yml` plus an unmaterialized dependency, then `gwz push`, then the operator follows the message. The unusable `gwz push --url-scheme manifest` is gone (§3.3, 291-293; step 2.1, 630 and 646-647), so the original sequence is closed.<br>The replacement command materializes the lock. My round-1 correction proposed it; it has side effects: new **P3-5**. | **CLOSED** on the original sequence; new P3-5 |
| **P3-3** | Fixed. §3.3 says a root-only push with a per-remote identity is newly refused, and names `--identity`. Native tests in step 2.1. | Sequence: a root-only `gwz push --target @root --remote-identity origin=KEY` with https member remotes and SSH committed URLs.<br>§3.3 (286-290) now records the new refusal and names `--identity PATH`. With `--identity KEY` the root's SSH push uses KEY, and https reads ignore the invocation default (`identity.rs:206-207`), so the remedy works.<br>Native tests cover the whole and root-only pushes (652-654). | **CLOSED** |
| **P3-4** | Merged into C-P2-5: `GWZDesign.md:96-98` updated unconditionally, the push help source updated (step 3.7), the `publication.rs:1-3` comment updated (step 3.3), and a grep for "available at their destinations". | Step 4.1 (880-888) now updates `GWZDesign.md:96-98` unconditionally and names the proof sources. Under D10 the design sentence (verified at `GWZDesign.md:96-98`) stays true for every publication.<br>Step 3.7 (830-834) edits the source of `docs/commands/push.md` "Publication and authentication" (verified at 77-81). Step 3.3 (731) updates the module comment. | **CLOSED** |

## Changed-range analysis

**What changed** (from `git diff b64f691 f3a395b` on the object and a full read of the revision):
- **Header and direction:** new status line and review links, plus an owner-direction bullet for D10 (30-32).
- **Goals and non-goals:** goals 2 and 3 reworded for D10 and the Noop-root proof. New non-goal: "pruning remote-tracking refs during fetch or pull" (57).
- **§2.1 facts:**
  - push reports no `Noop` today;
  - the pre-transfer read checks access only, and unmaterialized reads are not recorded;
  - libgit2 writes tracking refs without looking at the push URL;
  - fetch never prunes, and `is_ancestor` passes errors through;
  - the unreadable-file remedy text;
  - the real behaviour of `--force` and `+` refspecs;
  - the design's proof promise;
  - the seam's limits.
- **§2.3:** the default column follows D10 (N+3, N+2). New notes on D10's reads and on refspec forms that read the remote.
- **§3.2–§3.4:**
  - §3.2 rule 3: push clients send no scheme.
  - §3.3: the new root-only D4 refusal and its remedies, and the push remedies for D3.
  - §3.4: the byte-identical claim narrowed.
- **§3.5:**
  - New definitions: kept-advertisement key; "same repository, between destinations"; last-known ref with three conditions; forced.
  - Rule 1: kept advertisements count only when they prove availability, and forced or deleting transfers invalidate evidence.
  - Rule 2: a forced push with an equal last-known ref is contacted; an ancestry error counts as unknown; D10 governs dependencies.
  - Rule 3: a `Noop` root is still proven.
- **§3.6:** reasons in `planned.message`; a missing-dependency message that suggests `--check-remotes`; a machine-output compatibility note.
- **§3.7:** rewritten as the risks D5 accepts, narrowed by D10.
- **§4:** D3, D5, D7, D8 and D9 edited; D10 and D11 added.
- **Steps:** 1.2 enumerated and re-budgeted; 2.1 remedies and native tests; 3.1–3.3 detail; 3.4 overlap counter; 3.5 backend override and tests; new step 3.6 (native tests); CLI and gwz-py steps renumbered 3.7 and 3.8; 4.1 unconditional document updates; 4.2 dry run first and a fresh SSH clone for case 4.
- **Also:** the dependency summary, the §6 rows, and §7 notes (private helpers, mixed versions, the seam, pre-existing `GWZDesign.md:1601`).

**Changes outside the dispositions, checked individually:**
- **New non-goal: no pruning.** This is D10's rejected alternative stated as scope. Under D10 it does not widen false composition, because tracking refs only decide whether a member is contacted.
- **§3.6: the missing-dependency refusal suggests `gwz push --check-remotes`**, "which pushes those whose remote lacks their commit". Those are the pushes `c9c7a98`'s plain `gwz push` already makes (every captured push connects). libgit2 refuses a non-forced, non-fast-forward update client-side (`push.c:337-357`), so following the suggestion cannot rewind a remote branch. It is not worse than today. It may not resolve a lock commit that no pushed branch contains (§3).
- **§2.3 counts under D10:**
  - Whole workspace with one member and the root changed: 2 reads + (N−1) dependency reads + 2 pushes = N+3.
  - Root only: 1 read + N reads + 1 push = N+2.
  - Both agree with step 3.5 (788-789) and step 4.2 case 6.
- **§3.2 rule 3, "push clients send no scheme":** verified.
  - gwz-cli builds `TransportOptions { url_scheme: None }` for every command (`invocation.rs:62-68`); only clone and materialize apply a scheme (`snapshot_materialize.rs:58`, `workspace.rs:98`).
  - gwz-py reads `GWZ_URL_SCHEME` only for parsers that carry the flag (`cli_shared.py:474-490`).
- **§7 "Mixed versions":** my round-1 analysis, accurately carried over.
- **New citations checked:**
  - `push_plan.rs:111-114`
  - `handle_repo_lifecycle.rs:338`
  - `publication.rs:199-203`
  - `refs.rs:285-296`
  - `contract.rs:224-238`, `312-319`
  - `tracking_backend.rs:198-200`, `413-419`, `524-526`
  - `invocation.rs:106`, `255`
  - `GWZDesign.md:96-98`
  - gwz-cli `docs/commands/push.md` "Publication and authentication"
  - `MachineOutput.md:616-626`
  - libgit2 `push.c:164-212`, `remote.c:582-586`, `1420-1430`
  
  All accurate.

**Does D10 or the invalidation rule open a path the round-1 attacks missed?**
- **D10 (attack failed):** last-known refs are gone from every root-proof path: rule 2 (360-363), §3.7 intro (402-404), D10, step 3.5 (777-778), and the step-3.6 tests. Paths re-attacked:
  - a deleted branch;
  - a fork push URL removed afterwards;
  - a re-pointed or renamed remote;
  - tracking refs copied by a local clone;
  - a behind member (D6): the lock commit is an ancestor of the remote tip, so the read proves it soundly;
  - a root-only push;
  - unmaterialized dependencies (rule 3 reads them).
  
  Each ends in a read-based proof or refusal. What D10 costs: the P1-1 sequence, which `c9c7a98` publishes soundly by recreating the branch, is now refused under the default until `--check-remotes` is run. That is fail-closed, recorded in §3.6 and §3.7, and not a safety defect.
- **Invalidation: the trigger is complete (attack failed).** A non-forced gwz transfer cannot move a remote ref backwards. libgit2 refuses it before transfer when the remote object is missing locally or is not an ancestor (`push.c:337-357`). Deletions are named explicitly. "Forced or deleting" therefore covers every self-inflicted loss of availability.
- **Invalidation: repository matching is incomplete (NEW P2-5).** The rule matches repositories with a relation that treats unequal spellings of one repository as different (host aliases, `.git` suffix, owner case). That relation errs in the safe direction for choosing a read URL and for last-known condition 3. For invalidation it errs in the unsafe direction.
- **Replacement remedy for D3's refusal (NEW P3-5):** it materializes the lock.
- **New architectural root causes:** none. P2-5 corrects the relation the round-1 invalidation rule uses; P3-5 is remedy text.

---

## 0. Evidence base

**Commands:**
- Tuple check at the start, mid-review and the end (the round-1 command): identical, as listed in Baseline.
- `git log --oneline -4`, `git diff --stat b64f691 f3a395b`, and `git show --stat f3a395b`: the commit holds the object, the RemPlan, both round-1 reports and a gwz commit record.
- `git diff b64f691 f3a395b -- dev-docs/GwzUrlSchemePushPlan.md`: removed lines reviewed in full.
- No builds, no tests, no network, no writes.

**Documents read in full:**
- the revised object (1012 lines);
- `GwzUrlSchemePushPlan-RemPlan.md` (105 lines);
- `GwzUrlSchemePushPlan-ReviewConsistency.md` (411 lines).
- My filed round-1 report is present as a new file at `f3a395b`.

**Code read this round** (members unchanged since round 1; round-1 reads of `publication.rs`, `push_member.rs`, `transport.rs`, `transport_support.rs`, `identity.rs`, `url_scheme.rs`, `url_scheme_state.rs`, `handle_tag.rs`, `par_map_per_host.rs`, `tracking_backend.rs`, `g08.rs` and libgit2 still apply):
- gwz-cli `src/clirequest/invocation.rs` 50-80 (scheme unset for every command); `grep url_scheme` over `src/clirequest` (only `snapshot_materialize.rs:58` and `workspace.rs:98` apply a scheme); `docs/commands/push.md` 70-92; `docs/commands/materialize.md` 1-40 ("With no target flag, GWZ uses the workspace lock").
- gwz-core `src/workspace_ops/materialize_preflight.rs` 30-120: a materialized clean member gets `Checkout`; a dirty member refuses unless destructive.
- gwz-core `src/workspace_ops/handle_materialize.rs` 385-445 plus grep:
  - for a lock target, a branch member is attached at `<fetch remote>/<branch>`;
  - on `DivergedMember` it is detached at that target: "unpushed work not at the target — DETACH at the target".
- gwz-core `src/workspace_ops/handle_repo_lifecycle.rs` 330-342; `src/git/gitbackend/contract.rs` 305-322; `dev-docs/GWZDesign.md` 92-100.
- gwz-py `src/gwz/cli_shared.py` 453-505 (`requested_url_scheme` only for parsers carrying the flag).
- libgit2 1.9.7 `push.c` 337-357 (the client-side non-fast-forward check for non-forced refspecs).

## 1. Findings

Round-1 IDs are closed; see the closure table. New IDs continue the round-1 sequence.

### [P2-5] Forced-transfer invalidation matches repositories with a relation that treats unequal spellings of one repository as different, so a forced push through one spelling leaves stale evidence counting for another

**Location**
- §3.5 definition "Same repository, between destinations" (311-313).
- Rule 1, "Forced or deleting transfers invalidate evidence" (340-344).
- D8 (459-463) and D9 (464-471).
- Step 3.3 (728-730) and its tests (742-747).

**Violated invariant.** A root is published only when every lock commit is available from its member's committed repository after this operation's transfers. Round-1 P2-3's closure requires that a forced or deleting transfer to a repository invalidate all evidence about that repository.

**Mechanism.**
- Rule 1 invalidates the kept advertisements and D8 proofs "of that repository ... whichever identity repo or remote name reached it".
- §3.5 defines repository identity for this purpose as: the URLs are equal, or `scheme_only_difference` holds in either direction.
- That relation is deliberately narrower than repository identity (§3.1, D1). These all count as different repositories:
  - an unknown host, including SSH host aliases such as `git@github-work:o/r.git`;
  - a different `.git` suffix (`git@github.com:o/r` against `git@github.com:o/r.git`);
  - owner or repository case differences.
- GitHub serves each of those pairs from one repository.
- Narrow sameness fails safe when choosing a read URL and when testing last-known condition 3: the result is an extra read or a contact. For invalidation it fails unsafe: evidence survives.
- My round-1 correction for P2-3 prescribed §3.1's relation here; that was the wrong direction.

**State sequence** (reachable through the protocol refspec, like round-1 P2-3):
1. Member `m` has a committed fetch remote `origin` = `git@github.com:o/m.git`. The manifest's push remote for `m` is `publish` = `git@github-work:o/m.git`, where `github-work` is an `~/.ssh/config` host alias for github.com holding the push key. Both reach o/m.
2. The root lock names `m@L`. On o/m, `main` = T contains L, and no other ref advertises L.
3. An API caller sends `refspec "+refs/heads/main:refs/heads/main"`, selecting `m` and `@root`. Either mode works; with the default, a forced push is always contacted. `m`'s local `main` is Y, a rewrite without L.
4. Pre-transfer reads:
   - `m`'s destination (`m`, `publish`, `git@github-work:o/m.git`) shows `main` = T;
   - the dependency (`m`, `origin`, `git@github.com:o/m.git`) shows `main` = T. Its read URL comes from §3.2 rule 1, because `origin` has no push URL and equals the committed URL.
5. `m`'s forced push to `publish` succeeds. o/m `main` = Y, and L is no longer advertised.
6. Invalidation does not fire: `git@github-work:o/m.git` and `git@github.com:o/m.git` are neither equal nor scheme-only different, because `github-work` is not a known host.
7. The proof:
   - D8 does not apply, because `dependency_was_published` requires `plan.remote == dependency.remote` (`publication.rs:123`).
   - D9: this operation "did not push to" the dependency's destination by key, and its kept advertisement (`main` = T) proves L by ancestry.
   - The root is published naming L, which o/m no longer advertises.
8. At `c9c7a98` the same request re-reads `git@github.com:o/m.git` after transfers, sees `main` = Y, and refuses.

**Variants with the same root cause:**
- two members whose remotes differ only by `.git` suffix or owner case;
- concurrent pushes where member A's non-forced push through one spelling earns a D8 proof, and member B's forced push through another spelling rewrites the same ref. A's D8 proof is not invalidated.

**Impact.** False composition caused by this operation's own forced transfer. Round-1 P2-3's closure does not hold when the repository is reached through two spellings, and the step-3.3 tests (742-747) use a single spelling, so they cannot detect this.

**Required correction.**
- Make invalidation independent of URL identity. If this operation makes any forced or deleting transfer, every kept advertisement and every D8 proof in the operation stops counting. Every root-lock dependency is then read after all member transfers and before the root transfer. The extra reads fall only on API forced or deleting pushes.
- State that the §3.5 "same repository" relation may be used only where treating two URLs as different fails toward contact or read (read-URL choice, last-known condition 3), never to limit invalidation.
- Correct the D8 and D9 texts to point to the operation-wide rule.

**Closure test** (step 3.3, on a seam that maps several URLs to one advertisement store; the step-1.2 seam lists advertisements per URL and needs that mapping):
- (i) Sequence above, with an alias push URL and `always`: the root is refused and no root `push_prepared` is recorded.
- (ii) The same under the default.
- (iii) Two members whose URLs differ only by `.git`, one forced push: the root is refused.
- (iv) A concurrent non-forced push through one spelling and a forced push through another: the D8 proof does not count and the root is refused.
- (v) The non-forced variant of (i) reuses the kept advertisement with no extra read.

### [P3-5] The replacement remedy for D3's refusal, `gwz materialize --url-scheme manifest`, materializes the lock: it clones missing members and detaches members with unpushed work, which blocks the push the operator was attempting

**Location**
- §3.3, malformed `.gwz/url-scheme.yml` (291-293).
- D3 (443-447).
- Step 2.1: implementation (630) and test (646-647).

**Facts.**
- `gwz materialize` with no target flag uses the workspace lock (gwz-cli `docs/commands/materialize.md`: "With no target flag, GWZ uses the workspace lock").
- For a materialized clean member, the plan is `Checkout` (`materialize_preflight.rs:44-78`). A dirty member refuses unless destructive (47-52).
- Execution attaches a branch member at `<fetch remote>/<branch>`. When the local branch holds "unpushed work not at the target", it detaches HEAD at that target (`handle_materialize.rs:392-428`).
- Unmaterialized members are planned for clone (`materialize_preflight.rs:53-66`). This refusal fires only when a dependency is unmaterialized, so there is always one to clone.
- A detached member fails push planning: "push refspec is required for detached members" (`push_member.rs:650-655`). The preflight then rejects the whole push (`push_member.rs:154-163`).

**State sequence.**
1. A workspace has an unmaterialized lock member, a malformed `.gwz/url-scheme.yml`, and members carrying commits not yet pushed.
2. `gwz push` refuses and names `gwz materialize --url-scheme manifest`.
3. The operator runs it:
   - the unmaterialized member is cloned over its manifest URL;
   - every member with unpushed commits is detached at `origin/<branch>`;
   - the file is removed.
4. The operator re-runs `gwz push`: preflight rejects it because members are detached.

No commit is lost; the branches keep them. The operator must re-attach every member before pushing, and has also materialized a member they had left out on purpose.

**Correction.**
- On the push path, name deletion or repair of `.gwz/url-scheme.yml` as the remedy.
- If a gwz command is named at all, state that `gwz materialize` materializes the lock (clones missing members, moves or detaches checkouts).

**Test.** Step 2.1: the push-path refusal names deletion of the file and does not recommend a bare `gwz materialize`, or it carries that warning.

## 2. Invariant analysis

**Attacks that failed** (the pre-commitment rests on these):
- **Root publication is proven in the operation (D10).** Every path to a published root ends in a read or in an accepted push not made stale by invalidation, as shown in the closure-table traces for P1-1 and P2-1 and the Changed-range analysis. Last-known refs appear in no proof path. Rule 3's `Noop`-root proof uses the same evidence.
- **Kept-advertisement reuse.** It counts only when it proves availability, exactly as `publication.rs:86-99` does. Errors from `is_ancestor` count as unknown (357-358), so the proof reads. D9 still accepts only a rewind by someone else during the operation.
- **The invalidation trigger.** It covers every self-inflicted loss of availability (`push.c:337-357` plus "deleting"). Only its repository matching fails (P2-5).
- **The last-known ref.** All three conditions (317-320) fail toward "no last-known ref", which means contacted. A forced push with an equal ref is now contacted (351), which removes round-1 C-P3-5's silently skipped forced update. `--force` has no classification effect, and forced is decided by the `+` prefix.
- **Accepted member-push skips (§3.7).**
  - Remote moved ahead: correct to skip.
  - Rewound or deleted branch, or a ref from another repository: the member push is skipped, and any root needing that commit is refused.
  - A published root that has since become unsound: `--check-remotes` and `gwz tag --push` detect it.
  - A last-known ref left behind: one contact.
  
  None of these produces false composition.
- **Detection by `--check-remotes`.** Rule 3's `Noop`-root proof refuses the round-1 P2-2 sequence and its root-only variant.
- **Mixed versions.** A core that predates `remote_check` either contacts everything or rejects the request. An old client gets `changed`. Neither CLI sends a URL scheme on push (verified), so rule 3's precedence behaves as stated.
- **Machine output (D11).** `planned.message` carries the reason (precedent at `handle_repo_lifecycle.rs:335-341`). Under `always` no row is assumed, and the exit code is unchanged.
- **Credentials.** D10 restores today's read count for root publication through the §3.2 read URLs. No new host or credential path is added. `--identity` as the D4 remedy affects only SSH destinations (`identity.rs:206-207`).
- **The suggestion in §3.6.** `--check-remotes` makes the same pushes as `c9c7a98`'s plain push. Non-forced rewinds are refused client-side, so it is not worse than today.

**Invariants that did not hold:**
- evidence invalidation across unequal URL spellings of one repository (P2-5);
- a remedy free of side effects for the D3 refusal (P3-5).

## 3. Risks and next action

**Residual items below the finding bar:**
- **"After all transfers"** (rule 1, 343-344; step 3.3, 729-730) should read "after all member transfers and before the root transfer". The step-3.3 invalidation tests should also assert that no root push is recorded, not only that the root is refused.
- **The §3.6 missing-dependency suggestion** (`--check-remotes`) does not resolve a lock commit that no branch the member pushes contains. Keep today's "publish the member" guidance beside it.
- **§3.7's second bullet** says a fetch does not repair a rewind. A non-pruning fetch force-updates the tracking ref of a branch that still exists, so a fetch repairs rewinds; only deletions persist. The error is on the cautious side.
- **D10's rejected alternative** ("accepting the deleted-branch and re-pointed-remote cases as risk") reads more broadly than §3.7, which keeps the member-skip consequence of those cases under D5. The skip is not false composition, but the two sentences should agree.
- **A root the default contacts that rule 1 then finds "already on origin"** (pushed by another clone) is not proven. This operation publishes nothing, and rule 3 or `gwz tag --push` detects it.
- **A member classified `Noop`** keeps its "as of the last fetch or push" reason even after D10's dependency read in the same operation showed its destination lacks the commit. The root refusal names the member and the read URL.

**Next action.** The plan owner revises §3.5 rule 1, D8, D9 and step 3.3 so that invalidation covers the whole operation, with the P2-5 closure tests (i)–(v). Fold in P3-5 and, optionally, the residual wording above. Then request a bounded single-axis re-review of P2-5 on the revised tuple. That round is limited to non-architectural corrections and is permitted under the two-round cap.
