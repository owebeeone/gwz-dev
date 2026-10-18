# GwzUrlSchemePushPlan — SAFETY-AXIS REVIEW (round 3)

**Review object:** `dev-docs/GwzUrlSchemePushPlan.md` at gwz-dev root `0136094`. Status: plan; decisions D1–D11 taken on 2026-09-14; round-2 remediation applied; no code. Dated 2026-09-14.

**Baseline:**

| repository | commit | state |
|---|---|---|
| gwz-dev | `0136094` | clean |
| gwz-core | `c9c7a98` | tracked files clean |
| gwz-cli | `2c4c0ea` | |
| gwz-py | `334f53f` | |
| gwz-core-evidence | `b2bf115` | |
| taut | `7a5f616` | |
| taut-shape | `74f375c` | |
| taut-shape-rs | `df13036` | |
| taut-shape-py | `864ca0b` | |

The same result was recorded at start and end.

**Untracked files in gwz-core.** At both checks gwz-core had four untracked documents, not the two the tuple names. The two extra are `dev-docs/GwzTransactionalStore-G46.md` and `dev-docs/GwzTransactionalStoreA6.md`, from the same out-of-scope TransactionalStore lane. I treated this as no tuple move:
- every commit is unchanged;
- `git -C gwz-core diff --quiet` succeeded;
- the object cites neither file, and neither was read.

Member code is unchanged since rounds 1 and 2, so those source reads carry forward.

**Inputs:** `GwzUrlSchemePushPlan-RemPlan-2.md`, both round-2 reports, and `git diff f3a395b 0136094 -- dev-docs/GwzUrlSchemePushPlan.md`. No new owner decisions.

**Date:** 2026-09-14

**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; how far disclosure reaches; stuck states the text's own rules allow; whether "never worse than today" survives concrete interleavings; scope creep. Independent, adversarial, read-only. The other axis re-verdicts in parallel; nothing here relies on its round-3 report. Filed verbatim by the lane owner.

**Verdict: GO**
- P2-5 and P3-5 are closed on their original state sequences.
- Every round-1 closure whose supporting text this diff touched still holds.
- No P0, P1 or P2 is open.
- One new P3 (P3-6) does not block.
- No new architectural root cause, so the two-round cap does not fire.

---

## Prior-finding closure table

"Verified" means the original counterexample or state sequence was re-traced against the text at `0136094`. Rows for round-1 findings appear only where the diff touched text their closure rested on.

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| **P2-5** | Fix as specified. Any forced or deleting transfer invalidates every kept advertisement and every D8 proof. Every root-lock dependency is then read after member transfers and before the root transfer. The §3.5 relation never limits invalidation. D8 and D9 point to the operation-wide rule. The seam maps several URLs to one store. Step 3.3 tests (i)–(v). | Original sequence: `m` pushes through `publish` = `git@github-work:o/m.git` with `+refs/heads/main:refs/heads/main` under `always`, rewinding o/m `main` from T to Y. The dependency `m@L` has a kept advertisement read via `git@github.com:o/m.git`.<br>• Rule 1 (350-355) now reads: "If this operation makes any forced or deleting transfer, no kept advertisement and no D8 proof counts as evidence for any dependency."<br>• So `m@L` is read after member transfers and before the root transfer. It finds `main` = Y, L is neither advertised nor an ancestor of an advertised ref, and the root is refused before any root push.<br>Variants:<br>• default mode: forced pushes are always contacted (365), and the result is the same;<br>• two members differing only by `.git`: the same;<br>• a non-forced push through one spelling with a concurrent forced push through another: the D8 proof does not count, so the dependency is read.<br>Supporting text:<br>• §3.5 limits the relation to last-known condition 3 (318-323);<br>• D8 (485-487) and D9 (494-495) refer to the operation-wide rule;<br>• the seam maps URLs to shared stores (617-621);<br>• step 3.3 implements the rule (760-762) and lists all five closure tests, each asserting no root `push_prepared` (774-786);<br>• §6 (1009) and §7 (1048-1051) agree. | **CLOSED** |
| **P3-5** | Fix. The push-path refusal names deleting or repairing `.gwz/url-scheme.yml` and does not suggest a bare `gwz materialize`. Step 2.1 test. | Original sequence: a malformed file, an unmaterialized dependency and members with unpushed commits; the operator follows the refusal.<br>• §3.3 (296-300) and step 2.1 (659-660, test 676-678) now name only "delete or repair `.gwz/url-scheme.yml`". They also state why a bare `gwz materialize` is not suggested: it clones missing members and moves or detaches checkouts.<br>• Following the refusal no longer clones the unmaterialized member (`materialize_preflight.rs:53-66`). It no longer detaches members with unpushed work at `origin/<branch>` (`handle_materialize.rs:419-428`). The next `gwz push` is not rejected for detached members. | **CLOSED** |
| **P1-1** | (Round 1, closed in round 2.) Diff touched rule 2's D10 bullets and §3.7's deletion bullet. | Re-traced: `spike` pushed, then deleted remotely, then a non-pruning fetch, then the root lock records `m@F`, then a default push.<br>• `m` equals its last-known ref: `Noop`, not contacted (365).<br>• The root is contacted and proven (377-380), so `m@F` is read once as a dependency (359-361).<br>• F is missing. `m` stays `Noop` (373-375) and the root is refused (§3.6); nothing is pushed.<br>• §3.7 (430-440) keeps the deletion consequence and the refusal. The step-3.6 deleted-branch test (853-857) is unchanged. | **CLOSED** (still holds) |
| **P2-1** | (Round 1, closed in round 2.) Diff touched D10's wording, §3.7's different-repository bullet, and the rename test. | Re-traced: a fork `pushurl` wrote `refs/remotes/origin/main = G`, then was unset; the root lock records `m@G`. `m` is `Noop`; the dependency read of the committed URL finds G missing, and the root is refused.<br>The rename case moved from a contact expectation in step 3.5 to step 3.6 (866-870). It expects member `Noop`, the root refused by the committed-URL read, and the bare root ref unchanged. That matches §3.5's conditions and §3.7 (435-440); my round-2 row cited the superseded contact test. The excluded-layout (861-863) and fork (858-860) native tests are unchanged. | **CLOSED** (still holds) |
| **P2-2** | (Round 1, closed in round 2.) Diff touched rule 3's first bullet and §3.7's detection bullet. | Re-traced: root S published, `m` rewound to R0 dropping L, local `m` reset to R0, then `gwz push --check-remotes`.<br>• Rule 3 still skips classification (385).<br>• A `Noop` root still runs the proof after member transfers (387-389). The kept advertisement at R0 does not prove L, so the proof reads and the root is `Rejected`.<br>• §3.7 (441-444) still says `--check-remotes` detects this. It narrows `gwz tag --push` to tagged root commits, which matches `handle_tag.rs:287-298`. | **CLOSED** (still holds) |
| **P2-3** | (Round 1, closed in round 2.) Its per-repository invalidation was replaced by the operation-wide rule. | Re-traced on the new rule:<br>• (a) Two members on one URL with a forced push: invalidation, then `site` is read and the root refused.<br>• (b) Default mode, one member forced back to Y: D10 excludes last-known refs and the D8 proof does not count, so the dependency is read and the root refused.<br>• (c) Concurrent forced pushes: refused.<br>Tests at 776-784, each with no root `push_prepared`. | **CLOSED** (still holds) |
| **P2-4** | (Round 1, closed by D11 in round 2.) Diff touched rule 3's first bullet and §3.6's summary line. | The closure rests on §3.6's compatibility bullets (408-416, unchanged), D11 (506-509, unchanged) and rule 3 skipping classification (385, reworded but still skipping). Under `always` every `Noop` row is "already on origin", so a script can still demand a checked result. | **CLOSED** (still holds) |
| **P3-2** | (Round 1, closed in round 2.) Its remedy text was replaced by P3-5's fix. | Original sequence: the refusal pointed at the non-existent `gwz push --url-scheme manifest`. The new text (296-300) names no push option, so it stays closed. | **CLOSED** (still holds) |
| **P3-4** | (Round 1, closed in round 2.) Diff reworded step 4.1's GWZDesign update. | Step 4.1 (931-934) still updates `GWZDesign.md:96-98` unconditionally. Its new wording — "not checked for changes or pushed, though publishing the root still reads each dependency" — agrees with rule 2 (358-361, 377-380). The design's proof sentence therefore still describes every publication. | **CLOSED** (still holds) |

## Changed-range analysis

**What changed.** `git diff --stat f3a395b 0136094`: one commit touching the object (236 lines changed), RemPlan-2, both round-2 reports and one gwz commit record. Only the object is in scope. Its hunks, with the disposition each implements:

- **Header and review references** (3-19): bookkeeping.
- **Goal 2** (47) and **the §2.3 note** (220): "publishes the root" became "contacts the root" (Consistency P3-6, and the Safety round-2 residual on already-on-origin roots). **Goal 3** (49-51) and the **Phase 3 milestone** (702-705): "re-reading only where rule 1 requires it" (Consistency residual).
- **§3.3 D3 remedy** (296-300) and **step 2.1** (659-660, 676-678): S-P3-5.
- **§3.5 relation note** (318-323), **operation-wide invalidation** (350-355), **D8** (485-487), **D9** (494-495), **step 1.2 store mapping** (617-621), **step 3.3 implementation and tests** (760-786), **§6** (1009) and **§7 seam note** (1048-1051): S-P2-5.
- **§3.5 rule 2:**
  - the definition of "contacted" (357-361), and the finality of a member's classification after a D10 read (373-375): C-P3-6;
  - "A contacted root is proven, whether rule 1 publishes it or finds it already on origin" (377-380) and the step-3.5 test (843-844): Safety round-2 residual.
- **Rule 3 wording** (385-386): Consistency residual.
- **§3.6:**
  - summary line (400-402): C-P3-6;
  - missing-dependency message (403-407): Safety and Consistency residuals — it adds "publishing the member", and names the read URL only when it differs from the committed URL.
- **§3.7** (420-447):
  - opening sentence and last bullet: Consistency residuals;
  - rewind split from deletion: Safety residual;
  - shared-consequence bullet;
  - tag-push detection narrowed: Consistency residual;
  - "for an ordinary push" on the moved-ahead bullet.
- **D10's rejected alternative**, limited to root publication (501-505): Safety residual.
- **"Depends on 0.1"** for steps 1.1, 1.2, 3.1 and 3.2, plus the dependency summary (983-984): Consistency residual.
- **Step 3.5** unchanged-row reason and summary-line test (827-830) and **step 3.7** golden (890-892): C-P3-6. The rename case leaves step 3.5's contact list (841-842) and becomes a native test in step 3.6 (866-870): C-P3-7.
- **Step 3.7 help text** (880-881), **step 4.1 GWZDesign text** (931-934), **§7 credential-helper note** (1039-1041) and **§7 stale-refs note** (1042-1044): C-P3-6.
- **Step 4.2 dry-run guard** (947-952): C-P3-8.

**Changes outside the dispositions:** none. Every hunk maps to a RemPlan-2 disposition or to its list of residual wording folded in.

**Safety check of the operation-wide invalidation rule (S-P2-5).**
- *Trigger is complete.* A non-forced gwz transfer cannot move a remote ref backwards: libgit2 refuses it client-side (`push.c:337-357`). Deletions are named explicitly.
- *Scope no longer depends on URL identity.* The relation remains only in last-known condition 3, where calling URLs "different" fails toward a contact.
- *Other matching stays safe.* If an implementation matches D8/D9 destinations with the relation, the relation under-approximates "pushed to", which yields fewer D8 hits and more reads. Pre-push advertisements that D9 reuses stay valid, because only forced or deleting transfers remove availability, and those now invalidate everything.
- *Edge cases re-attacked, all safe:*
  - a forced root transfer comes after the proof, so invalidating is harmless;
  - a forced member that rule 1 makes `Noop` transfers nothing, so it rightly invalidates nothing;
  - a failed forced transfer marks the member `Failed` and root publication is not attempted;
  - a failed re-read after invalidation refuses the root.

**Safety check of the Consistency-driven changes.**
- **Rule 2's "contacted" and final member classification.**
  - Finality is the safer of the two readings the Consistency axis offered. gwz never pushes a member because of a D10 read, so a branch someone deleted on purpose is not recreated by default. A root that needs its commit is refused.
  - A forced member is always contacted, so finality never suppresses an explicit forced push.
  - Cost (residual §3): automation that runs only default `gwz push` is refused on every run while a dependency's branch stays deleted, where `c9c7a98` would recreate the branch. This fails closed and the refusal names the remedies.
- **A contacted root found already on origin is still proven.** This only adds proofs, and matches `c9c7a98`, which proves before its no-op root push. It closes my round-2 residual.
- **§3.7 split of rewinds from deletions.**
  - The rewind bullet ("A fetch updates the tracking ref of a branch that still exists") holds only for a forced fetch refspec. libgit2 skips a non-fast-forward tracking update for a non-forced refspec (`remote.c:1877-1883`), and §3.5 condition 1 does not require `+`. **New P3-6.** No false composition: D10 still reads.
  - The re-pointed-repository bullet ("the same consequence as a deleted branch") overstates how long the state lasts for branches the current repository has. It errs toward caution (residual §3).
- **Step 4.2 dry-run guard.**
  - On fresh clones it prevents accidental publication.
  - It cannot see a remote that moved ahead since the last fetch. The `--check-remotes` push then makes an ordinary push that libgit2 refuses client-side after connecting (`push.c:337-357`). No ref changes; the case exits 1.
  - This fails closed (residual §3).

**Architectural classification.** No new architectural root cause. P3-6 corrects a condition or its wording inside the decided last-known-ref rule. The two-round cap does not fire.

---

## 0. Evidence base

**Commands run.**
- Tuple check at start and end, using the command from earlier rounds plus `git -C gwz-core diff --quiet`. Results are identical, as recorded in Baseline.
- `git log --oneline -4` and `git diff --stat f3a395b 0136094`.
- `git diff f3a395b 0136094 -- dev-docs/GwzUrlSchemePushPlan.md`: all 500 diff lines read.
- No builds, tests, network operations or writes.

**Documents read in full.**
- The object at `0136094` (1068 lines).
- `GwzUrlSchemePushPlan-RemPlan-2.md` (71 lines).
- `GwzUrlSchemePushPlan-ReviewConsistency-2.md` (277 lines).
- My round-2 findings as issued; the filed copy was not re-read.

**Code read this round.**
- libgit2 1.9.7 (vendored `libgit2-sys-0.18.8+1.9.7`) `remote.c` 1855-1900, plus a grep for force handling. For an existing tracking ref, a non-forced refspec and a fetched head that does not descend from the old value, the update is skipped (1877-1883).

**Carried forward** (same commits in every member):
- libgit2 `push.c:164-212` (tracking-ref updates on push) and `push.c:337-357` (client-side non-fast-forward refusal); `remote.c:582-586` and `1420-1430` (prune).
- gwz-core:
  - `publication.rs` in full;
  - `push_member.rs` in full;
  - `transport.rs:69-104`, `453-526`;
  - `handle_materialize.rs:385-445`;
  - `materialize_preflight.rs:30-120`;
  - `url_scheme_state.rs:109-117`;
  - `identity.rs:195-208`;
  - `handle_tag.rs:287-298`.
- gwz-cli `invocation.rs:50-80`, `255` and gwz-py `cli_shared.py:453-505`.

## 1. Findings

Every round-1 and round-2 ID is closed (see the closure table). New IDs continue the sequence.

### [P3-6] §3.7 says a fetch ends the rewind case, but §3.5 admits non-forced fetch refspecs, for which libgit2 skips the tracking-ref update

**Location.**
- §3.7 "Remote rewound" (427-429).
- §3.5 last-known ref, condition 1 (327-328): "the remote's fetch refspecs map `refs/heads/*` exactly into that namespace". It does not require `+`.
- Step 4.1 Troubleshooting bullet (925-927), which covers deletions only.

**Facts.**
- gwz fetches with the remote's configured refspecs (`transport.rs:89-99`).
- libgit2 1.9.7 skips the update when three things hold (`remote.c:1877-1883`):
  - the tracking ref exists;
  - the refspec is not forced;
  - the fetched head does not descend from the tracking ref's current value.
- `git clone` and `git remote add` write the forced form `+refs/heads/*:refs/remotes/<name>/*`, so the default layout is unaffected. A non-forced `refs/heads/*:refs/remotes/origin/*`, written by hand or by a tool, still satisfies condition 1.

**State sequence.**
1. Member `m` has `origin` with fetch refspec `refs/heads/*:refs/remotes/origin/*` (no `+`). `m` is published at L, so `refs/remotes/origin/main = L`.
2. Someone force-pushes `origin` `main` to R0, dropping L.
3. The operator runs `gwz pull` or `git fetch`. The tracking ref stays L, because the update is skipped.
4. Every later default `gwz push` finds `m` equal to its last-known ref, so it stays `Noop` "up to date with origin/main as of the last fetch or push" indefinitely.
   - A pruning fetch does not help, because the branch exists.
   - A root that needs L is refused by the D10 read; no false composition.
   - `--check-remotes` reads `m`, its ordinary push is rejected as non-fast-forward, and the problem surfaces.

**Consequence.** For this layout, §3.7's statement that a fetch ends the rewind case is false. The member-push skip lasts like a deletion, and neither §3.7 nor the step-4.1 Troubleshooting text names a remedy that works.

**Correction.** Choose one:
- add "the refspec is forced (`+`)" to §3.5 condition 1, so a non-forced layout has no last-known ref and its member is contacted; or
- qualify §3.7's rewind bullet to forced fetch refspecs (the ones `git clone` and `git remote add` write), and name `--check-remotes` as the remedy for other layouts.

**Test.** A step-3.6 native case with a non-forced fetch refspec and a rewound bare remote.
- With the condition fix: the member is contacted.
- With the wording fix: after a fetch the member is still `Noop`, and `--check-remotes` reports the non-fast-forward rejection.

## 2. Invariant analysis

**Attacks that failed** (the GO rests on these):
- **Root publication is proven by this operation.**
  - Every path to a published root ends in a read or in a D8 proof that survives invalidation.
  - Last-known refs only classify members (377-380, 420-422, D10).
  - Re-traced here: the P1-1, P2-1, P2-3 and P2-5 sequences, including the host-alias, `.git`-suffix and concurrent-D8 variants.
- **The invalidation rule is complete.**
  - The trigger covers every self-inflicted loss of availability (`push.c:337-357`, plus deletions).
  - Its scope is the whole operation.
  - Its read ordering, "after all member transfers and before the root transfer", keeps the proof ahead of the root push, and the tests assert that no root push happens.
- **The remaining relation fails safe.** Last-known condition 3 fails toward contact, and D8/D9 destination matching fails toward more reads.
- **Final member classification.** A D10 read never triggers a push, so default mode does not recreate deliberately deleted branches. The root is refused, and a forced push is still always contacted.
- **Already-on-origin roots.** A contacted root found already on origin is proven, and a `Noop` root under `--check-remotes` is proven. Neither change removes a proof.
- **The §3.6 suggestion.** "Publish the member" or `--check-remotes`. The latter makes the same pushes as `c9c7a98`'s plain push, and non-forced rewinds are refused client-side.
- **The step-4.2 guard.** It stops accidental publication on fresh clones. A stale-ahead remote leads to a refused ordinary push, not a ref change.
- **The P3-5 remedy.** Deleting or repairing the file has no effect on checkouts or clones.
- **Unchanged since round 2:**
  - mixed versions (§7);
  - credentials: no new host or credential path, and the §7 helper note now covers D10 reads;
  - D11 carries its reason in `planned.message`, and every `Noop` row under `always` is checked.

**Invariant that did not hold:** the rewind-repair claim for non-forced fetch refspecs (P3-6, member pushes only).

## 3. Risks and next action

**Residual items below the finding bar:**
- **Default-only automation.** A script that runs only default `gwz push` is refused on every run while a dependency's branch stays deleted on the remote (final classification, no pruning). `c9c7a98` would recreate the branch. The refusal names the remedies.
- **Troubleshooting wording.** Step 4.1 (925-927) says a deleted branch lasts until "… a push or `--check-remotes`". Under rule 2's final classification, a default `gwz push` never recreates it. Say "a push that recreates the branch (for example `git push`), or `--check-remotes`", as §3.7 does.
- **Re-pointed or renamed remotes.** §3.7 (435-438) says a ref written by another repository has "the same consequence as a deleted branch". For a branch the current repository also has, a fetch through a forced refspec repairs it, as for a rewind. The text errs toward caution.
- **Guard cases that do not start from a fresh clone.** Step 4.2's guard reads only local tracking refs. For case 5's scratch member and case 6, fetch before the dry run. Otherwise a remote that moved ahead makes the socket-polled push attempt a refused non-fast-forward push, which adds a session and exits 1.
- **Deleting the preference file.** Deleting `.gwz/url-scheme.yml` (§3.3) sends unmaterialized dependency reads and later clones back to manifest URLs; repairing it keeps the preference. This fails closed.

**Next action.** The Safety axis gives GO on `0136094`. The lane owner can fold P3-6 and the residual wording above into the plan text before implementation. No further Safety round is required for them.
