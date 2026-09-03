# GwzM5-8 A1 Release R1 — focused State-axis review (Decision 2 A′)

**Verdict: GO WITH CONDITIONS. Gate G1 is SATISFIED at gwz-core `8e40fa8` +
the R1 builder's test package, subject to C1-C4 below (all
documentation/ledger; no production-code condition).**

**Object.** The Decision 2 A′ pre-mutation foreign-filter refusal as it
stands in the tree — the WHOLE landed object, not the R1 delta. Per §1
below, the focused State review the packet mandates
(`GwzM5-8A1DecisionPacket.md`:481-485, :326-338) produced no filed report
at landing; **this document is that owed report.**

**Tuple.**
- gwz-core worktree HEAD `8e40fa8f0084326230a82e4e485112d9a4304931`
  (= `origin/main`; `git diff origin/main --` over
  `recovery_support.rs`, `merge_recovery.rs`, `transport.rs` is empty).
- Landed A′ share introduced by `9939b02d2cc57db84540b9e3f903317f6c088342`
  (2026-08-16, "Land the filter-policy (D1+D2) and R2-F missing-tests
  packages"), fixture follow-up `90d3f8a` (LFS hermeticity, `g12.rs` +32).
- Uncommitted R1 delta: `src/git/tests/g12.rs`, +77 / -0, one test.
  Diff sha256 `748d31b57b3951f89a4a37d81c90dbb28258f14619e125c9cce570f13e436b44`
  (`r1-aprime-final.diff`).
- Pristine source sha256, verified at review start AND after every
  mutation experiment in §5:
  `recovery_support.rs` `7a930417281804f67ae119bc9b57af6b5e48b531bf6361c458db36c9d1734ed5`;
  `merge_recovery.rs` `b224edae96be04fafb194061b486b825545a9bf386fc698cd5ac22ecefa2de83`;
  `g12.rs` `a6a386d9448484d01019d1d92082e9c064475d5f45267b55a9a45d1f8998c6a0`.
- Reviewer made no code edits that survive: final `git status --porcelain`
  is `M src/git/tests/g12.rs` alone and `git diff` is byte-identical to
  `r1-aprime-final.diff`.
- Dependency pin: `git2 0.21.0`, `libgit2-sys 0.18.7+1.9.6` (`Cargo.lock`:214-217, :438-441).

---

## 1. Provenance — was `9939b02`'s A′ share ever reviewed?

**Answer: YES, a focused State review was performed and returned GO. NO
report artifact was ever filed. The ledger DID record the decision's
discharge — the R1 builder's "no dev-doc mentions it" claim is wrong on
two of its three probes and materially wrong on the third.**

### 1.1 The review happened

`9939b02`'s own commit message states it in terms only a completed review
produces — verbatim:

> D1+D2 (**State review GO, F2 same-train condition discharged**):
> creation-time filter neutralization at create_repo and the clone
> funnel …, the A' foreign-filter refusal at both recovery-grade checkout
> sites (typed, pre-mutation, lfs allowlisted), six Unix tests with RED
> evidence, the Windows doctrine sentinel …, and the amendment/ledger
> closure annotations (create_repo claim corrected; OPEN DECISION
> discharged; **F1/F3/F5/F6 residuals recorded**).

Numbered findings F1-F6 with F2 as a discharged same-train *condition* is
the house review shape. Three of those findings are traceable in the tree
by content, annotated into the amendment in the same commit:

| Finding | Where it landed | Content |
| --- | --- | --- |
| **F1 [P3]** | `GwzM5-8ExactEvidencePlatformAmendment.md`:117-121 | "Known spec residual of the D2 probe (State F1, P3): **the attribute stack is read pre-checkout, so attribute coverage introduced by the target state itself escapes the gate**; ~15-LOC hardening recorded as a candidate." |
| **F6** | same file :167-172 | "foreign clean/process commands never execute inside gwz (libgit2 runs no config-command drivers), so in-gwz status cannot manufacture the wedge in-process; the hazard the refusal closes is real-git-visible divergence on covered paths after a raw-byte rewrite" |
| **F3 [P3]** | same file :172-175 | "A dedicated error code (vs the house `DirtyMember` recovery-refusal idiom) is a candidate for a future deliberate wire train (F3)." |

**F1 IS review-focus item (a).** The `.gitattributes` asymmetry the R1
builder empirically re-discovered was found at landing, adjudicated P3,
and recorded in the amendment nine days before R1 started.

### 1.2 The ledger DID record the decision

The A′ OPEN DECISION is explicitly discharged in the amendment at
`GwzM5-8ExactEvidencePlatformAmendment.md`:156-175:

> "The policy was recorded here as an explicit OPEN DECISION;
> **DISCHARGED 2026-08-16 by adopted decision D2 (A′ refined refusal,
> landed in the filter-policy package)**: option (a) — both
> recovery-grade checkout sites now inspect the rewrite set's `filter`
> attributes pre-mutation and refuse with a typed error naming path and
> filter when a foreign non-passthrough driver is configured (`lfs`
> allowlisted by name; True/False/Unspecified attributes pass; non-UTF-8
> driver names refuse fail-closed)."

The tripwire ledger carries the companion update at
`GwzWindowsMatrix-Classification.md`:191-205 ("**Update 2026-08-16 (D1/D2
landed)**").

### 1.3 What is actually missing

Only the **report artifact**. `ed10ab9` (the gwz-dev mirror of `9939b02`)
touched exactly two dev-docs — `GwzM5-8ExactEvidencePlatformAmendment.md`
and `GwzWindowsMatrix-Classification.md` — and no `-Review*.md`. Every
sibling package landing that week filed one
(`GwzM5-8DurableCursor-Review{Code,State}.md`,
`GwzM5-8M5bNoFf-Review{Code,State}.md`,
`GwzM5-8V0ForgedActionGate-ReviewState.md`,
`GwzM5-8PanicConversionP1P3-ReviewCode.md`,
`GwzM5-8OperatorEscapeAmendment-Review{Code,State}.md`,
`GwzM5-8ThinA1Amendment-Review{Safety,Consistency}.md`). `git log --all
--diff-filter=D -- 'dev-docs/*Review*'` shows no such file was ever filed
and later removed. The D1+D2 package is the week's sole exception.

**Consequence for scope:** the packet §4 step-1 obligation is not
satisfiable by a commit-message assertion. **This review therefore scopes
itself to the WHOLE landed object** (§§2-4), not to the R1 delta, and
files as the owed focused State review.

### 1.4 [P2] F-1 — the program checkpoint states a falsehood

`CurrentProgramCheckpoint.md`:977-979 records, as the R1 delivery finding:

> "the code closed; THE LEDGER NEVER RECORDED IT (no dev-doc mentions
> refuse_foreign_filtered_rewrites, 9939b02, or filter-policy)"

Only the first probe is true. `9939b02` appears in five dev-docs
(`GwzM5-8R2DPhase4Closure.md`:272, `GwzM5-8R2DStep43-Review.md`:175,
`GwzM5-8PanicConversionP1P3-ReviewCode.md`:4,
`GwzM5-8V0ForgedActionGate-ReviewState.md`:3, and the checkpoint itself);
`filter-policy` in five (incl. `GwzM5-8ExactEvidencePlatformAmendment.md`
:109 and :161, `CurrentProgramCheckpoint.md`:1466). Most importantly the
A′ *decision* is discharged in the ledger per §1.2. A program checkpoint
that asserts an un-recorded decision when the decision IS recorded is
itself a ledger defect, and it is the sentence a future auditor would
trust. **Condition C1.**

---

## 2. Conformance — the landed predicate against the packet's A′

Verified independently of the builder's audit, clause by clause, against
`GwzM5-8A1DecisionPacket.md`:278-289 (A′), :262-276 (A, which A′ inherits)
and `GwzM5-8A1ReleasePlan.md`:57-67 (R1.1) / :39 (G1).

| A′ clause | Landed at | Verdict |
| --- | --- | --- |
| Scoped to the true rewrite set | `recovery_support.rs`:51-53 `diff_tree_to_tree(Some(from), Some(to), None)`; call-site trees at `merge_recovery.rs`:157-165 and :233-245 | **CONFORMS** — see §2.1/§2.2 |
| Deletions excluded | `recovery_support.rs`:57-59 | **CONFORMS** (sound; untested — F-5) |
| Bare/unset attributes fall through | `recovery_support.rs`:66-74 (`_ => continue` covers `True`/`False`/`Unspecified`) | **CONFORMS** — see §2.4 |
| `lfs` allowlisted | `recovery_support.rs`:75-77 | **CONFORMS** (ordering ruled in §3(d)) |
| Configured non-passthrough = `filter.<n>.clean` OR `.process` in **effective** config | `recovery_support.rs`:78-89 (memoized) → :103-115 (`get_entry` over both keys on a `Config::snapshot()`, `:97-101`) | **CONFORMS** — snapshot of `repo.config()` is the full effective stack (repo → global → system → programdata), which is what real git consults |
| Typed refusal naming filter **and** path | `recovery_support.rs`:117-123 → `recovery_dirty` :14-16 → `ErrorCode::DirtyMember` | **CONFORMS-WITH-RECORD** — see §2.5 |
| BEFORE any ref or worktree mutation, at BOTH recovery-grade sites | `merge_recovery.rs`:164 (abort) and :244 (rollback) | **CONFORMS** — see §2.3 and §3(e) |

### 2.1 Rewrite-set soundness at `set_branch_target_checked`

The checkout is `checkout.safe().disable_filters(true)`
(`merge_recovery.rs`:250). A SAFE checkout writes a path only where the
worktree still matches the *old* content. `ensure_clean_recovery_state`
(`recovery_support.rs`:342-369, called at `merge_recovery.rs`:200 — before
`current` is even read at :201-205) pins repository state Clean, HEAD
attached to `branch`, and
`status_dirty_outside_checked_artifact_private(&backend.status(path)?)`
false. That predicate (`repository_support.rs`:107-122) carves out **only
untracked** residue under `.gwz/checked-artifacts` — "Any staged,
unstaged, or unresolved entry (anywhere, including the private area) …
remain dirt" (:105-106). Untracked paths are in neither tree, appear in no
`diff_tree_to_tree` delta, and are not removed (`remove_untracked` is not
set on the SAFE builder). Therefore the worktree is tracked-exact at
`current` and the SAFE checkout rewrites **exactly** `diff(current →
target)`. The gate diffs exactly those two trees (`merge_recovery.rs`
:233-244). **Bound holds.**

### 2.2 Rewrite-set soundness at `abort_merge` — item (b), adjudicated

The gate diffs `merge_head.tree() → before.tree()`
(`merge_recovery.rs`:157-165); the checkout is
`force().remove_untracked(false).remove_ignored(false).disable_filters(true)`
(:166-176). A FORCE checkout rewrites path P wherever worktree(P) or
index(P) ≠ before_tree(P). I read `validate_abort_index_and_worktree`
(`recovery_support.rs`:216-235) and **confirm the bound; no hole found.**
The argument, enumerated:

1. **Index.** `validate_recovery_index` (:199-214) recomputes the merge
   with `repo.merge_commits(before, merge_head, None)` (:168-186) and
   requires `comparable_index_entries(current, conflicts) ==
   comparable_index_entries(expected, conflicts)` — a full
   `(path, mode, oid, stage)` listing comparison outside the conflict set.
   An extra, missing, or differently-staged path outside the conflict set
   refuses. So the live index equals the freshly recomputed merge index.
2. **Worktree.** :225-233 refuses on `status.untracked > 0` or any
   `worktree_status != " "` on a path outside `conflicts`.
3. **Conflicts ⊆ diff(merge_head → before).** A conflict requires both
   sides to have changed the path *differently* from the base (identical
   changes auto-resolve), so before(P) ≠ merge_head(P). Modify/delete and
   add/add-with-different-content conflicts likewise differ. Holds.
4. **Auto-merged paths ⊆ diff(merge_head → before).** Changed
   incoming-only → before(P) = base(P) ≠ merge_head(P) ✓. Changed both
   sides but cleanly merged → the two sides differ ✓. Changed local-only
   → the merge writes nothing and index/worktree already equal before(P),
   so the checkout does not rewrite it ✓. Identical change both sides →
   before(P) = merge_head(P), not in the diff, and the merge result equals
   before(P), so again no rewrite ✓.
5. **Deletions.** `Delta::Deleted` (present in `merge_head`, absent in
   `before`) is skipped at :57-59; the checkout removes those paths and
   writes no bytes through any filter ✓.
6. **Renames.** No `find_similar` is applied (options `None`), so renames
   decompose into Added + Deleted; the Added half is probed ✓.
7. **Divergent live-vs-recomputed conflict sets.** A live conflict path
   absent from the recomputed set appears at stage > 0 in `current` and
   stage 0 in `expected`, both outside the exclusion — mismatch, refuse ✓.
8. **Path-encoding mismatches** (`file.path.as_bytes()` vs the raw index
   `Vec<u8>`; case-insensitive filesystems) can only *fail* the
   containment test, i.e. over-refuse. Fail-safe direction ✓.

**Does the argument deserve a structural guard rather than a comment?**
The guard **already exists** but is untied. Steps 3-4 are properties of
git's merge and cannot regress. Step 1-2 are gwz code that can, and
`g12.rs`:737-739 pins step 2 today — it writes `stable.txt` (outside the
conflict path `base.txt`) and asserts `ErrorCode::DirtyMember`. The risk
is not absence of a guard; it is that the guard is named
`checked_native_abort_rejects_drift_and_dirt_then_is_idempotent` and
carries no hint that it is load-bearing for A′'s abort-site scope. That
matters because **this file already contains the precedent for exactly the
wrong edit**: `ensure_clean_recovery_state` was relaxed with a carve-out
for checked-artifact private residue (`recovery_support.rs`:360-367). A
future author adding an analogous carve-out to
`validate_abort_index_and_worktree` would silently narrow A′'s abort
bound, and neither the function nor the test says so — the argument lives
in a comment in a *different file* (`merge_recovery.rs`:149-156).
**F-6 [P3], recommended not binding:** add a two-line note on
`validate_abort_index_and_worktree` naming `merge_recovery.rs`:149-165 as
its A′ consumer, and a cross-reference on `g12.rs`:737-739.

### 2.3 Both sites refuse before mutation

- **abort** (`merge_recovery.rs`:128-184): `transaction`/`lock_ref`
  :143-144 → `validate_expected_native_merge` :145 →
  `validate_abort_index_and_worktree` :146 → **A′ gate :164** →
  `checkout_tree` :175 → `index.write()` :180 → `cleanup_state()` :181 →
  `drop(transaction)` :182. This site never calls `set_target` or
  `commit()` at all — the transaction is purely a lock. Every byte-level
  mutation is downstream of the gate.
- **rollback** (`merge_recovery.rs`:186-262): `transaction`/`lock_ref`
  :197-198 → `ensure_clean_recovery_state` :200 → idempotent arm :206-213
  → drift check :214-221 → **A′ gate :244** → `checkout_tree` :251 →
  `set_target` :253-255 → `commit()` :256 → `verify_merge_result` :257.
  The gate precedes the worktree rewrite *and* the ref move.

### 2.4 Attribute-value handling

`AttrValue::from_bytes` (git2 0.21 `src/attr.rs`:11-22) yields
`True | False | String | Bytes | Unspecified`. The match at
`recovery_support.rs`:66-74 takes `String(name)` as the driver name,
**fails closed** on `Bytes(_)` (non-UTF-8 name — cannot be probed in
config, :68-72), and falls through on everything else. That is correct
against real git semantics: git's `convert.c` looks up a driver only for a
string-valued `filter` attribute; bare `filter`, `-filter` and unset
cannot run one, exactly as the doc comment at :33-35 states. **Conforms.**

### 2.5 "Typed" refusal — record, not finding

The refusal reuses `ErrorCode::DirtyMember`
(`recovery_support.rs`:14-16, :117-123), the house recovery-refusal idiom
shared with `ensure_clean_recovery_state`:364 and
`validate_abort_index_and_worktree`:230. It is a typed `ModelError` whose
message names both path and filter, satisfying the packet's "typed
pre-mutation error naming the filter and path" (:272-273). It is **not
programmatically distinguishable** from sibling `DirtyMember` refusals —
already recorded as F3 at the amendment :172-175 as a future wire-train
candidate. **Direct consequence for G6:** the release notes cannot name
this class by error code; they must name it by message shape. Folded into
condition C2.

### 2.6 The third `disable_filters` site — exclusion CONFIRMED

There are exactly three `disable_filters(true)` sites in the tree:
`merge_recovery.rs`:174, `merge_recovery.rs`:250, `transport.rs`:31.
(The five other production checkouts — `merge_prepared.rs`:253/:328,
`refs.rs`:33/:136, `repository.rs`:112/:137 — are *filtered*, i.e.
ordinary git behaviour, and carry no raw-bytes-under-a-filter hazard.)
The clone funnel's exclusion is correct on four independent grounds:

1. `ensure_clone_target_is_empty(path)` (`transport.rs`:21) — no
   pre-existing worktree, no user work, nothing to wedge.
2. No ref transaction and no `verify_merge_result`; A′'s hazard is defined
   as a post-`transaction.commit()` verification failure
   (packet :242-248).
3. `pin_creation_time_filter_neutralization` runs immediately after the
   clone (`transport.rs`:38 → `repository_support.rs`:28-33), so the repo
   is blob-exact from birth by construction — this is Decision **1** B's
   own mechanism, not a recovery edge.
4. The packet enumerates A′'s consumers exhaustively as "two recovery
   checkouts + the renormalize command" (:319-320); the release plan
   repeats it (:63-65).

**[P3] F-9, cross-lane hand-off to the R2 (Decision 1 B) reviewer — NOT a
G1 matter.** The same `disable_filters` at the clone funnel gives a
gwz-cloned **LFS** repository *pointer bytes* on disk where porcelain
`git clone` would smudge real content. The amendment discloses the
pointer-bytes surprise for the *recovery* path only (:175-179); I found no
record of it at the clone edge (`GwzM5-8ExactEvidencePlatformAmendment.md`
mentions the clone funnel once, at :110, without it). R2's object, R2's
call — recorded here only because it is the same mechanism.

---

## 3. The five review-focus items — adjudicated

### (a) The `.gitattributes` asymmetry — **ACCEPTED AS A NAMED RESIDUAL, with conditions C2 + C3**

**The residual is real; I reproduced it first-hand.** Running the
builder's probe in the review worktree (then restoring byte-exactly, §5):

```
PROBE merged_has_attrs=false before_has_attrs=true worktree_has_attrs=false
PROBE set_branch_target_checked -> Ok(GitUpdateResult { updated: true, commit: Some("e29b5207f4daecbe5cdb60a2d92449972991e916") })
PROBE post-state: ref=e29b5207f4daecbe5cdb60a2d92449972991e916 attrs_restored=true secret_bytes="plain-v1\n"
```

`get_attr_bytes(..., AttrCheckFlags::default())` reads FILE_THEN_INDEX —
the live, pre-checkout stack. Where the current side carries no coverage
and only the target side does, the gate sees nothing, the rollback
proceeds, the ref moves, and the post-state is raw blob bytes under a
restored configured `filter.crypt.clean`. The exact precondition A′
exists to refuse.

**Why I accept it rather than condition the fix in.** Four reasons, the
second decisive:

1. **It was already found and adjudicated at landing** — F1 [P3],
   `GwzM5-8ExactEvidencePlatformAmendment.md`:117-121 (§1.1). This review
   is not discovering an unrecorded gap; it is re-deciding a recorded one.

2. **[P2] F-2 — neither named fix is available at this dependency pin,
   and the packet's premise about the API is false.** The packet asserts
   (`GwzM5-8A1DecisionPacket.md`:269-271) that the attribute is queried
   "via git2 0.21's `Repository::get_attr`/`get_attr_bytes`
   (`repo.rs:1112/:1132`, **wrapping `git_attr_get_ext`**)". It does not:
   `get_attr_bytes` calls `raw::git_attr_get` (git2 0.21
   `src/repo.rs`:1142). The distinction is the whole ballgame —
   `git_attr_get_ext` is the entry point that takes `git_attr_options`,
   whose `attr_commit_id` field is the *only* way to read attributes from
   an arbitrary tree. And **`libgit2-sys 0.18.7+1.9.6` binds neither**:
   `grep -c git_attr_get_ext` returns `0`, and there is no
   `git_attr_options` and no `GIT_ATTR_CHECK_INCLUDE_COMMIT` anywhere in
   it. `git2 0.21`'s `AttrCheckFlags` (`src/lib.rs`:1546-1565) exposes
   exactly four flags — `FILE_THEN_INDEX`, `INDEX_THEN_FILE`,
   `INDEX_ONLY`, `NO_SYSTEM` — and no `INCLUDE_HEAD`/`INCLUDE_COMMIT`.
   **"INCLUDE_COMMIT against the target" is therefore not implementable
   without a dependency bump or patch**, which is categorically out of
   scope for a release train's critical-path phase. The remaining shapes
   are (i) blunt — refuse whenever `.gitattributes` is anywhere in the
   rewrite set, costing availability on *every* recovery rollback that
   crosses an attributes change, on the one path where availability
   matters most; or (ii) parse the target tree's `.gitattributes` blobs
   ourselves, re-implementing libgit2's pattern/macro/stacking engine on a
   load-bearing released-line refusal path. Neither is a ~15-LOC change.
   **The amendment's "~15-LOC hardening recorded as a candidate"
   (:120-121) is not achievable as written and must be corrected** — it is
   the sentence that would let a future lane believe this is cheap.

3. **[P2] F-3 — the realized harm is milder than the code comment
   claims, per the landing review's own F6.** `recovery_support.rs`:24-29
   states the mechanism as "the exact post-verification then fails only
   AFTER `transaction.commit()`, and the retry re-fails in the idempotent
   arm — a retry-proof recovery wedge". My probe shows
   `set_branch_target_checked` returning **`Ok`** in precisely that shape:
   `verify_merge_result` did not fail. The reason is recorded at the
   amendment :167-172 (F6) — gwz's status is libgit2-based
   (`repository.rs`:173 `repo.statuses(...)`), and libgit2 does not
   execute config-command filter drivers, so in-gwz verification cannot
   see the divergence. The hazard the residual leaves open is therefore
   **real-git-visible divergence on covered paths after a raw-byte
   rewrite** (F6's own words), whose remedy is a porcelain re-checkout —
   not the retry-proof in-gwz wedge. That is an availability/UX class,
   materially milder. *(I do not re-adjudicate what libgit2 does under
   `filter.<n>.required=true`, which is a different arm and outside this
   object.)* The code comment was never reconciled with F6 and now
   overstates its own subject.

4. **Reachability requires a narrow conjunction, and the common direction
   over-refuses safely.** The wedge needs: `.gitattributes` in the rewrite
   set, coverage present on the *target* side and absent on the current
   side, the driver configured, and a covered path also in the rewrite
   set. Because the gate reads the *current* side, the ordinary shape —
   rolling back a branch that ADDED coverage — refuses (over-refusal, safe
   direction). Only the inverse — rolling back to a state whose coverage
   the merge REMOVED — slips through, which is what the probe had to
   construct explicitly (`git rm --cached .gitattributes` on the feature
   branch). A′ still strictly improves on the pre-A′ status quo, where
   *every* configured-foreign-filter recovery checkout was exposed.

**Conditions attached to this acceptance:**

**C2 [binding, before the tag].** File the residual verbatim — in
`CurrentProgramCheckpoint.md`'s residual register and, in its user-facing
form, in the vNEXT release notes under G6
(`GwzM5-8A1ReleasePlan.md`:44). The exact residual text:

> **A′ NAMED RESIDUAL — target-side attribute coverage (`.gitattributes`
> asymmetry).** The Decision 2 A′ foreign-filter refusal
> (`refuse_foreign_filtered_rewrites`,
> `src/git/gitbackend/recovery_support.rs`:46-95) reads the *pre-checkout*
> attribute stack (`git2::AttrCheckFlags::default()` =
> `FILE_THEN_INDEX`). When `.gitattributes` is itself inside the rewrite
> set and the foreign `filter` coverage exists ONLY on the target side —
> i.e. the recovery checkout restores the coverage together with the
> bytes — the gate does not fire: the recovery-grade rollback or abort
> proceeds, the ref moves, and the covered path is left holding raw blob
> bytes under a now-active configured clean driver. The reachable harm is
> divergence visible to real `git` on those paths (gwz's own
> libgit2-based status cannot see it — it does not run config-command
> drivers; see F6, `GwzM5-8ExactEvidencePlatformAmendment.md`:167-172);
> the remedy is a porcelain re-checkout of the affected paths. Reaching
> it requires the coverage to arrive WITH the checkout (e.g. rolling back
> across a merge that deleted the covering `.gitattributes`); the common
> direction — rolling back a change that ADDED coverage — over-refuses
> and is safe. **Not closable at the current dependency pin:** reading
> attributes from an arbitrary tree needs `git_attr_get_ext` /
> `git_attr_options.attr_commit_id`, which `libgit2-sys 0.18.7+1.9.6`
> does not bind and `git2 0.21`'s `AttrCheckFlags` does not expose. First
> recorded as State F1 [P3] at landing (`9939b02`, 2026-08-16);
> re-confirmed empirically and re-accepted at R1
> (`GwzM5-8A1ReleaseR1-ReviewState.md` §3(a)). Owner: the §R6
> `gwz repair --renormalize` package, which already shares this
> predicate and is the natural place to harden it.

**C3 [binding, before the tag; test-only, interior single-axis].** Pin the
residual with a **doctrine sentinel** in `g12.rs`, following the house
ritual already established by
`doctrine_sentinel_adopted_crlf_worktree_classifies_ambiguous_in_the_reverse_observer`
(`g12.rs`:1083-1111). Prose in a dev-doc is the weaker artifact: it is
what let F1 sit unread for nine days and be re-discovered from scratch by
R1. The sentinel should assert *today's* behaviour (the rollback
PROCEEDS in the target-side-only-coverage shape), state in its doc
comment that going red means someone closed the residual and the frozen
texts — this report's C2 text, the amendment :117-121, the release notes —
must move with it. The R1 builder's probe (`g12.rs.withprobe`) is already
the fixture; converting it costs ~40 test-only lines and touches no
production path.

### (b) The `abort_merge` rewrite-set bound — **CONFIRMED; guard exists but is untied**

Fully adjudicated in §2.2. The bound holds under all eight cases I
enumerated; no hole found. The structural guard exists at `g12.rs`:737-739
and does not need to be created — it needs to be *tied* to the argument.
**F-6 [P3], recommended.**

### (c) `AttrCheckFlags::default()` semantics — **CONFIRMED as intended; the mandate's third option does not exist**

`AttrCheckFlags::default()` is `FILE_THEN_INDEX` (git2 0.21
`src/lib.rs`:1561-1565).

- **FILE_THEN_INDEX is right.** At the rollback site the worktree and
  index are pinned identical at `current` (§2.1), so the choice is
  immaterial there. At the abort site they can differ, and the worktree
  copy is the one real `git` consults for a present file — matching it is
  correct for a gate whose whole purpose is predicting what real git will
  see. `INDEX_ONLY` would be wrong for the same reason.
- **`NO_SYSTEM` is correctly NOT wanted.** The system/global
  `gitattributes` genuinely apply on the user's machine; excluding them
  would make the gate blind to coverage that will be live post-checkout.
  Note the config probe is symmetric — `Config::snapshot()`
  (`recovery_support.rs`:97-101) is likewise the full effective stack.
- **`INCLUDE_HEAD` is not wanted and, in this git2, is not available.**
  It is absent from git2 0.21's `AttrCheckFlags` entirely; only
  `libgit2-sys` defines the raw constant
  (`GIT_ATTR_CHECK_INCLUDE_HEAD: u32 = 1 << 3`), reachable solely via
  `AttrCheckFlags::from_bits_retain(8)` — reaching around the safe API.
  Even if used, it would help only the abort site (where HEAD is the
  restore target) and do nothing at the rollback site (where HEAD *is*
  the side already read). A hacky half-fix; **not recommended**, recorded
  here only so a future lane does not rediscover it as a shortcut.

This flag is indeed what produces (a), and (a) is decided above.

### (d) `lfs` allowlist before the config probe — **ACCEPTABLE-WITH-RECORD, not a finding**

`recovery_support.rs`:75-77 short-circuits on the attribute *name* before
any config read, so a driver named `lfs` whose `filter.lfs.clean` is
`git-crypt clean` is unconditionally allowed. Ruled acceptable:

1. **It is exactly what the packet specifies** — "attribute value outside
   the allowlist (`lfs`)" (:273, :279-281), a name-keyed allowlist. Adding
   an authentication step would be a deviation from the reviewed
   specification, not conformance to it.
2. **No meaningful escalation.** Realizing it requires control of both
   `.gitattributes` and repository config; anyone with config write access
   already has strictly stronger primitives (`core.fsmonitor` and friends
   execute arbitrary commands on ordinary git operations). The gate is not
   a security boundary and is not claimed to be one.
3. **It is load-bearing for fixture hermeticity, deliberately and with
   its reasoning recorded.** `pin_hermetic_lfs_passthrough`
   (`g12.rs`:864-871) sets `filter.lfs.clean = cat` to neutralise CI's
   ambient system `git-lfs`, and its doc comment (:857-863) states the
   reliance explicitly and correctly: "The gate … allowlists the driver
   NAME — `if name == "lfs" { continue; }` — and returns before it probes
   `filter.<name>.clean`/`.process` at all, so its decision for `lfs` is
   identical with or without these pins." Changing the ordering would
   silently break that hermeticity argument on the CI runners.
4. Already recorded in the ledger as "`lfs` allowlisted by name"
   (`GwzM5-8ExactEvidencePlatformAmendment.md`:164-165).

The allowlist rests on a property of the *real* driver (LFS pointer blobs
round-trip clean), with the name as its proxy; a repurposed name breaks
the proxy. That is worth knowing and is recorded here. Any stricter form
(sniffing the configured command for `git-lfs`) would be brittle and is
**argued against**.

### (e) Refusal position after `transaction.lock_ref` — **CONFORMS; no mutation under the packet's contract**

At `merge_recovery.rs`:143-144 (abort) and :197-198 (rollback) the ref
lock is taken before the gate runs. Ruling: **this is pre-mutation.**

- **The packet's own boundary is `set_target`, not `lock_ref`**: "Before
  the checkout (and before `transaction.set_target`)"
  (`GwzM5-8A1DecisionPacket.md`:263-264). The refusal precedes both, at
  both sites. Its own message says exactly the right thing — "refusing
  before any ref or worktree mutation" (`recovery_support.rs`:120).
- **Nothing durable changes.** No ref value, no reflog entry, no index
  write, no worktree byte. `git_transaction_lock_ref` creates a transient
  `.lock` file which libgit2 removes on rollback.
- **The `drop(transaction)` path is a rollback, not a commit.** git2's
  `Transaction::drop` frees the transaction, and an un-committed
  transaction releases its locks. On the A′ refusal path the `return
  Err(...)` at `recovery_support.rs`:71/:91 propagates through `?` at
  `merge_recovery.rs`:164/:244 and the `transaction` binding drops at
  scope exit — the same discipline as the pre-existing drift return at
  :214-221. The explicit `drop(transaction)` calls at :182 and :207 are
  ordering devices for the *success* paths (so the subsequent verification
  does not contend with the lock), not error-path cleanup.
- **The abort site never commits at all** — no `set_target`, no
  `commit()`; its transaction is purely a lock, and every mutation
  (:175 checkout, :180 index write, :181 `cleanup_state`) is downstream.
- **Window enlargement is quantitative, not categorical.** The only
  residual effect is a longer stale-lock window on abnormal process
  termination. `lock_ref` already preceded *all* sibling validations
  pre-A′; A′ adds one tree diff, O(rewrite-set) cached attribute lookups
  and one config snapshot — inside a window that at the rollback site
  already contains `ensure_clean_recovery_state`'s full `backend.status()`
  filesystem walk. Negligible.

**[P3] F-8, recommended.** `GwzM5-8A1ReleasePlan.md`:62-63 phrases the
requirement as "Refuse … BEFORE the ref transaction", which the code does
not literally satisfy and which is *stricter than the packet it derives
from*. Align the plan's wording to the packet's — and to the refusal
message's own — "before any ref or worktree mutation", so no future
auditor reads a conformance failure into a conforming implementation.

---

## 4. The builder's test — verified as the missing sentinel

`checked_rollback_proceeds_when_the_configured_filter_is_outside_the_rewrite_set`
(`g12.rs`:961-1036, +77 / -0).

**Degeneracy-prevention preconditions — both present and both correct:**

1. `g12.rs`:1009-1022 asserts
   `get_attr("quiet.txt", "filter", AttrCheckFlags::default()) ==
   Some("crypt")` — the covered path really is covered *in the same stack
   the gate reads*. Without it the test could pass because the attribute
   never applied at all.
2. `g12.rs`:1023-1027 asserts `{merged}:quiet.txt == {before}:quiet.txt` —
   the covered path really is identical in both candidate trees, hence
   genuinely outside `diff(current → target)`. Without it the test could
   pass for an unrelated reason.

Plus `configure_filter_driver(&repo, "filter.crypt.clean", …)` at :1005 —
the driver really is configured, so the config probe *would* say
"configured" if the scope reached it.

**A third, unlabelled non-degeneracy assertion is also present:**
`g12.rs`:1034 asserts `loud.txt` reads `plain-v1\n` post-rollback, which
proves the rewrite set was non-empty and actually exercised (`merged`
carries `plain-v2` from the feature side). **[P3] F-10, cosmetic:** label
it as such in the comment block; a future reader trimming "redundant"
assertions could remove the only proof the test is not vacuous.

**Mutation verification — re-run independently, not accepted on
assertion.** I applied four separate mutations to
`recovery_support.rs`, ran the whole `g12` module after each, and restored
from a sha256-pinned backup between every one (§5):

| # | Mutation | `g12` result | Which test flipped |
| --- | --- | --- | --- |
| M1 | rewrite set widened to the whole target tree: `diff_tree_to_tree(Some(from), Some(to), None)` → `(None, Some(to), None)` | `FAILED. 24 passed; 1 failed` | **`checked_rollback_proceeds_when_the_configured_filter_is_outside_the_rewrite_set`** — and only it |
| M2 | deletion-skip clause (:57-59) removed | `ok. 25 passed; 0 failed` | **NOTHING** |
| M3 | `lfs` allowlist (:75-77) removed | `FAILED. 24 passed; 1 failed` | `checked_rollback_proceeds_over_lfs_and_unconfigured_filter_attributes` |
| M4 | config probe short-circuited to always-configured (A-strict) | `FAILED. 24 passed; 1 failed` | `checked_rollback_proceeds_over_lfs_and_unconfigured_filter_attributes` |

M1 confirms the builder's claim exactly and independently: under the
widening, **the two refusal tests still pass** — they cannot detect it —
and the new test is the sole detector across all 25 `g12` tests. It is the
missing sentinel. Under M1 the failure is at `g12.rs:1031`, the `.unwrap()`
on the rollback that should have succeeded.

**[P3] F-5 — new coverage finding from M2.** Deleting the `Delta::Deleted`
exclusion flips nothing: the "deletions write no bytes through any filter"
clause of A′ is **entirely untested**. It is sound (§2.2 step 5) and its
regression direction is fail-safe (over-refusal on deleted paths), which
is why this is P3 and not a condition. Recommended: extend the existing
refusal fixture with a path deleted between the two trees and covered by
the configured driver, asserting the rollback still proceeds.

---

## 5. Gates run — verbatim

All in the R1 worktree
`/private/tmp/…/scratchpad/r1-aprime` at `8e40fa8` + the +77 delta, with a
reviewer-private `CARGO_TARGET_DIR`.

**G-1 `cargo test --lib g12` (pristine):**
```
test result: ok. 25 passed; 0 failed; 0 ignored; 0 measured; 1560 filtered out; finished in 0.76s
```
including all four pre-existing A′ tests plus the new one:
```
test git::tests::g12::checked_rollback_refuses_configured_foreign_filter_before_any_mutation ... ok
test git::tests::g12::checked_rollback_proceeds_over_lfs_and_unconfigured_filter_attributes ... ok
test git::tests::g12::checked_rollback_proceeds_when_the_configured_filter_is_outside_the_rewrite_set ... ok
test git::tests::g12::checked_native_abort_refuses_configured_foreign_filter_before_any_mutation ... ok
```

**G-2 `cargo test --lib` (full):**
```
test result: ok. 1584 passed; 0 failed; 1 ignored; 0 measured; 0 filtered out; finished in 848.90s
```

**G-3 `cargo fmt --check`:** clean (exit 0, no output).

**G-4 `cargo clippy --all-targets --all-features`:** no `error` and no
`warning` lines.

**G-5 `python3.13 scripts/checks/check_checked_artifact_boundaries.py`:**
```
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```
*(Note: the repo's default `python3` on this host is < 3.11 and the script
dies on `import tomllib`. Not a finding against R1 — recorded so the R3
release-lane battery wiring (G4) pins an interpreter version.)*

**G-6 mutation matrix M1-M4:** table in §4.

**G-7 asymmetry probe:** output quoted in §3(a).

**G-8 post-experiment restoration:**
```
7a930417281804f67ae119bc9b57af6b5e48b531bf6361c458db36c9d1734ed5  src/git/gitbackend/recovery_support.rs
b224edae96be04fafb194061b486b825545a9bf386fc698cd5ac22ecefa2de83  src/git/gitbackend/merge_recovery.rs
a6a386d9448484d01019d1d92082e9c064475d5f45267b55a9a45d1f8998c6a0  src/git/tests/g12.rs
 M src/git/tests/g12.rs
WORKTREE IDENTICAL to r1-aprime-final.diff
```
and a re-run after restoration:
```
test result: ok. 25 passed; 0 failed; 0 ignored; 0 measured; 1560 filtered out; finished in 1.60s
```
All shas equal the review-start pins. No pins moved; no count pin exists
that a new test would disturb (`grep` over `scripts/` and `.github/` for
test-count pins returns nothing).

---

## 6. Findings register

| ID | Sev | Item | Status |
| --- | --- | --- | --- |
| F-1 | **P2** | `CurrentProgramCheckpoint.md`:977-979 asserts the ledger never recorded A′; it did (amendment :156-175), and a State review did run (`9939b02` message; F1/F3/F6 traceable) | **C1** |
| F-2 | **P2** | Packet :269-271's `git_attr_get_ext` claim is false (`git2` `repo.rs`:1142 calls `git_attr_get`); `libgit2-sys 0.18.7` binds no `git_attr_get_ext`/`git_attr_options`/`INCLUDE_COMMIT`. Amendment :120-121's "~15-LOC hardening" is therefore unachievable as written | **C4** |
| F-3 | **P2** | `recovery_support.rs`:24-29 states an in-gwz post-commit wedge that F6 (amendment :167-172) already corrected and that my probe shows does not occur; release notes must not repeat it | **C2** (notes) + C4 (comment, recommended) |
| F-4 | **P2** | The `.gitattributes` target-side asymmetry | **ACCEPTED as a named residual; C2 + C3** |
| F-5 | P3 | The `Delta::Deleted` exclusion is untested (M2 flips nothing) | Recommended |
| F-6 | P3 | The abort rewrite-set bound's guard (`g12.rs`:737-739) is untied to the A′ argument, next to a carve-out precedent (`recovery_support.rs`:360-367) | Recommended |
| F-7 | P3 | `lfs` allowlist short-circuits before the config probe | **Acceptable-with-record**; no action |
| F-8 | P3 | Release plan :62-63 "BEFORE the ref transaction" is stricter than the packet's own boundary and than the shipped code | Recommended |
| F-9 | P3 | Clone-funnel `disable_filters` leaves LFS pointer bytes in gwz-cloned repos, unrecorded | **Hand-off to R2**; not G1 |
| F-10 | P3 | The new test's non-vacuity assertion (`g12.rs`:1034) is unlabelled | Recommended |

**No P0. No P1. No finding requires a production-code change.**

### Conditions on this GO

- **C1 [binding, R1.2].** Correct `CurrentProgramCheckpoint.md`:977-979:
  the ledger DID record the D2 discharge
  (`GwzM5-8ExactEvidencePlatformAmendment.md`:156-175) and a State review
  DID run with findings F1/F2/F3/F5/F6; what is missing is the report
  artifact, and this document is it. Annotate the decision ledger
  "decided: A′, landed at `9939b02`, reviewed-at-landing (report
  unfiled), re-reviewed and GO at `8e40fa8`
  (`GwzM5-8A1ReleaseR1-ReviewState.md`)".
- **C2 [binding, before the tag].** File the §3(a) residual text verbatim
  in the checkpoint's residual register, and its user-facing form in the
  vNEXT release notes under G6 — describing the class by message shape,
  not error code (§2.5), and per F6's mechanism, not the code comment's
  (F-3).
- **C3 [binding, before the tag; test-only].** Land the (a) doctrine
  sentinel in `g12.rs` per §3(a).
- **C4 [binding, docs].** Correct `GwzM5-8A1DecisionPacket.md`:269-271
  (`git_attr_get_ext` → `git_attr_get`) and
  `GwzM5-8ExactEvidencePlatformAmendment.md`:120-121 (strike "~15-LOC
  hardening recorded as a candidate"; replace with the C2 residual's
  "not closable at the current dependency pin" statement and the R6
  ownership). Reconciling `recovery_support.rs`:24-29 with F6 is
  **recommended**, not binding — it is a comment on a released-line file
  and can ride R6.

---

## 7. G1 verdict for the release train

**G1 — "Decision 2 A′ landed: pre-mutation typed refusal of configured
non-passthrough foreign clean filters (allowlist `lfs`) at the
recovery-grade checkouts" (`GwzM5-8A1ReleasePlan.md`:39) — is
SATISFIED at `8e40fa8` + the builder's test package.**

Every clause of the gate is met (§2): the predicate is scoped to the true
rewrite set at both sites with the bound proved at each (§2.1, §2.2);
deletions are excluded soundly; bare/unset attributes fall through
correctly; `lfs` is allowlisted as specified; "configured
non-passthrough" is `filter.<n>.clean OR .process` over the effective
config; the refusal is typed and names filter and path; and it precedes
every ref and worktree mutation at both recovery-grade checkouts. The
third `disable_filters` site is correctly excluded on four independent
grounds (§2.6). The suite proves each refinement — and, with the R1
builder's addition, now proves the scoping refinement too, which nothing
in the tree did before (§4, M1).

**The §4 forbidden ordering is cleared.** The packet's
one-must-not-happen — "cutting a release that carries the amendment's
`disable_filters` code with Decision 2 still open"
(`GwzM5-8A1DecisionPacket.md`:494-496) — does not apply: Decision 2 is
closed (adopted A′, landed `9939b02`, discharge annotated at the amendment
:156-175), and its implementation is verified here. vNEXT (v0.11.0) may
proceed past this gate.

**What the GO does not cover.** The named residual of §3(a) survives into
the release, deliberately and on the record. G1 is a gate on the refusal
*mechanism*, and the mechanism conforms; the residual is a *scope* limit
of the specification the gate encodes — recorded at landing, re-confirmed
here, and not closable at the current dependency pin. C2 and C3 exist so
that it ships **named and tripwired** rather than buried in one sentence
of a nine-day-old amendment annotation, which is exactly the failure mode
that made this review necessary.

---

*Filed by the R1 focused State-axis reviewer, 2026-08-25, against
gwz-core `8e40fa8` + `r1-aprime-final.diff`
(`748d31b57b3951f89a4a37d81c90dbb28258f14619e125c9cce570f13e436b44`).
No code edits, no git operations, no other dev-docs file touched.*
