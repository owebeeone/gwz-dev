# GwzM5-8 A1 Release R4 — single-axis review of the merge wedge-and-recovery runbook

Date: 2026-08-25. Reviewer: the R4 single-axis reviewer (command-sequence
safety). Object: the gwz-cli worktree at `cf0d16d` plus the R4 package —
`docs/MergeRecovery.md` (new, 498 lines) and indexing edits in `mkdocs.yml`,
`docs/README.md`, `docs/Troubleshooting.md`, `docs/commands/merge.md`.
Truth base: gwz-core main at **`07e1ac1`** (the R3 landing), re-verified
directly rather than accepted from the builder's `a6ef094`/`bed072a` citations.

**Mandate flavour, restated so the findings read in the right register:** these
commands get pasted into wedged production repositories by stressed operators.
I reviewed every fenced block as the person who gets paged when one of them
destroys a user's work. Where I could execute a git-semantics claim, I executed
it rather than reasoned about it; the runs are reproduced verbatim below.

---

## 0. Verdict

# NO-GO

Not because the page is bad — it is, in structure and prose, the best operator
document this program has produced, and most of it should ship unchanged. It is
NO-GO because **two of its command blocks are wrong in a specific and
unusually dangerous way: they fail while reporting success.**

- **[P0-1]** Class A's mandatory follow-up and class B's *only* remedy —
  `git checkout -- <paths>` — is a **no-op** in the exact state both classes
  describe, and the runbook's own verification line (`# expect no output`)
  **passes on the unrepaired worktree**. Proven on both a size-preserving and a
  size-changing (git-crypt-shaped) filter. Class B's *detection* premise fails
  the same way: `git status --short` is silent in the case it diagnoses.
- **[P0-2]** Class C's normalize block **destroys a mid-conflict member's hand
  resolution and its MERGE_HEAD**, even though the runbook fences that case in
  prose — because git's own refusal on `git rm --cached -r .` is defeated by
  `git reset --hard` on the very next line of the same paste-block.

Both are small, local, provable, and cheap to fix. The remediation list in §8
is six edits. With them applied this is a GO.

Findings: **2 × [P0], 4 × [P1], 5 × [P2], 7 × [P3].** Register in §7.

**Scope limit, stated plainly (the R2 precedent):** the mandate asks me to
judge nine narrowings under a section titled *"Where the DRAFT was too
dangerous"*. **That section does not exist in the delivered object.**
`docs/MergeRecovery.md` is 498 lines and contains no such heading; the phrase
appears nowhere in the worktree or in `gwz-dev/dev-docs/`. As with R2's unfiled
anchor texts, the builder's narrowing list was not filed to disk. I do not sign
nine texts I have not read. I rule in §5 on the three whose **substance** is
observable in the delivered page (#2 force-abandon, #5 the hardened
`reset --hard`, #8 the `.gitattributes` prohibition), and I record the other six
as unreviewed.

---

## 1. Command-block rulings — every fenced block, explicit

Eighteen fenced blocks. Five are error-text displays (D); thirteen are command
blocks (C). Rulings are against the class's *actual* state, not the happy path.

| # | Line | Kind | Content | Ruling |
| --- | --- | --- | --- | --- |
| 1 | :49-51 | D | class A refusal text | **PASS** — byte-exact |
| 2 | :91-94 | C | `git config --show-origin --get filter.crypt.{clean,process}` | **PASS with [P3-1]** — `--get` under-reports |
| 3 | :100-104 | C | class A re-materialize (`status`/`checkout --`/`status`) | **FAIL [P0-1]** — no-op that self-certifies |
| 4 | :137-140 | C | class B detection (`status --short` → `M`) | **FAIL [P0-1]** — silent in the diagnosed case |
| 5 | :178-182 | C | class B recovery (`status`/`checkout --`/`status`) | **FAIL [P0-1]** — no-op that self-certifies |
| 6 | :194-196 | D | class C refusal text | **PASS** — byte-exact |
| 7 | :240-247 | C | class C normalize block | **FAIL [P0-2] + [P1-1] + [P1-2]** |
| 8 | :269-271 | D | open-merge gate message | **PASS** — byte-exact |
| 9 | :286-289 | D | multiple-records message | **PASS** — byte-exact |
| 10 | :314-324 | C | step 2 evidence copy | **PASS with [P3-4]** — no verification step |
| 11 | :329-332 | C | step 2 `merge --status` capture | **PASS with [P2-4]** — unobtainable in the sub-case that motivates the section |
| 12 | :339-342 | C | step 3 `mkdir` + `mv` to `quarantine/` | **PASS** — mechanism confirmed against `07e1ac1` |
| 13 | :387-391 | C | step 5 `status`/`log`/`reset --hard <before>` | **PASS** — correctly gated (see §5, #5) |
| 14 | :397-399 | C | step 5 `git branch recovered-work <backup-ref>` | **PASS** — ref semantics correct |
| 15 | :415-418 | C | step 7 `mv` back out of `quarantine/` | **PASS with conditions** — see §6, ruling 2 |
| 16 | :453-455 | C | class E `git lfs checkout` | **FAIL [P2-2]** — cannot work in the leading class-E state |
| 17 | :459-462 | C | class E `rm` + `checkout --` | **PASS** — and this is the form classes A and B should have used |
| 18 | :469-484 | C | class F evidence collection | **PASS with [P3-8]** — read-only, correct |

---

## 2. [P0-1] — the class A/B remedy is a no-op that certifies its own success

### The mechanism

Git trusts its index stat cache. After a filters-off recovery checkout, the
index carries the size and mtime of the **raw** file that was written. Once the
racily-clean window closes (index mtime no longer equal to the file mtime in
the same second — i.e. one second later, or after any index refresh), git
considers the path up to date and **never re-runs the clean filter**. Three
consequences, in order of severity:

1. `git status --short` prints **nothing** — class B's entire entry point
   (`docs/MergeRecovery.md`:133-141, "real git disagrees … `M
   config/secrets.env`") is a detector that is silent in the case it diagnoses.
2. `git checkout -- <paths>` is **skipped as up-to-date** and leaves the
   divergent bytes in place.
3. The runbook's verification line `git status --short   # expect no output`
   (:103, :181) **passes on the unrepaired worktree**.

### The runs

Size-preserving filter (rot13 clean/smudge), settled stat cache:

```
REMEDY 1: runbook class A/B  'git checkout -- <path>'
  content: cynvagrkginyhr   status:[]          <-- NOT repaired; status silent
REMEDY 2: runbook class E    'rm <path>; git checkout -- <path>'
  content: plaintextvalue   status:[]          <-- repaired
REMEDY 3: 'git add --renormalize .'
  content: cynvagrkginyhr   status:[M  secret.txt]   <-- STAGES the divergence
REMEDY 4: 'git rm --cached -r . ; git reset --hard'   (the class C block)
  content: plaintextvalue   status:[]          <-- repaired
REMEDY 5: 'git checkout-index -f -a'
  content: cynvagrkginyhr   status:[]          <-- NOT repaired
```

Size-**changing** filter (base64 clean/smudge — the git-crypt shape the runbook
names by example), settled stat cache:

```
status before remedy: []                        <-- silent
after 'git checkout -- ': content=cGxhaW50ZXh0LXYxCg==   <-- NOT repaired
status after: []                                <-- "expect no output" PASSES
```

The size difference does **not** save it: the filters-off checkout writes the
index entry with the raw file's own size, so the stat matches. In my first
(unsettled) run the same commands *did* report `M` — that was the racily-clean
same-second rule, and it is the only thing that ever makes this work. Real
operators are never inside that window: `gwz merge --abort` writes the index
itself, and the operator then restores the config key and looks.

Confirmed working alternatives: `rm <paths>` then `git checkout -- <paths>`
(the class E form, :459-462), and `touch <paths>` first (verified: status
reports ` M`, and the subsequent checkout repairs).

### Why this is the most serious finding on the page

Class A is the **A′ refusal** — the Decision 2 gate that G1 exists to ship. The
runbook is the user-facing half of that gate: refuse safely, then tell the
operator how to finish the job by hand. Class B is the **C2 named residual**
(`CurrentProgramCheckpoint.md`:1280-1310), whose entire accepted-risk argument
rests on one sentence: *"the remedy is a porcelain re-checkout of the affected
paths."* That sentence is what makes the residual acceptable rather than
blocking. **The porcelain re-checkout, as the runbook spells it, does not
re-checkout.** For git-crypt the realized outcome is a covered secret sitting
on disk in the wrong form, under a moved branch, with a green `git status` and
a runbook that told the operator they were done.

`GwzM5-8A1ReleaseR1-ReviewState.md` §3(a):351-368 (F-3) softened the residual's
severity from "retry-proof in-gwz wedge" to "real-git-visible divergence …
whose remedy is a porcelain re-checkout — an availability/UX class, materially
milder". That softening is load-bearing for the acceptance. It holds only if
the remedy works. **[P0-1] does not reopen the C2 acceptance — the residual is
still real-git-*repairable* — but it does mean the shipped remedy text must be
corrected before the tag, and the C2 user-facing leg landing in R5's release
notes must carry the corrected form, not `git checkout -- <paths>`.**

Note also that §"Why GWZ reports clean" (:168-171) tells the operator "GWZ's own
status does not execute configured filter commands … **Use real `git` to
check.**" The missing half of that sentence is that real `git` cannot see it
either until the stat cache is defeated.

### Required fix

- Class A (:100-104) and class B (:178-182): adopt the class E form.
  `rm <paths>` then `git checkout -- <paths>`.
- Replace both `# expect no output` verifications: `git status` cannot verify
  this class. Verify by content, or by `touch <paths> && git status --short`
  (which *is* a valid post-check because the touch defeats the cache).
- Add one sentence to :168-171: real `git` will also report clean until the
  file is removed or touched, which is why the `rm` is not optional.

---

## 3. [P0-2] / [P1-1] / [P1-2] — the class C normalize block

### [P0-2] It destroys a mid-conflict member, and git's own refusal does not stop it

The runbook fences the case in prose (:234-236): *"Apply this only to a member
whose own git state is clean — no conflicts, no uncommitted changes. If the
member is mid-conflict, do not normalize it; go to case F."* The fence is
correct and it is defeated by the block's own structure. My run — a member with
a real conflict, a staged hand resolution, and further unstaged edits:

```
>>> RUNNING THE RUNBOOK CLASS-C BLOCK <<<
error: the following file has staged content different from both the
file and the HEAD:
    f.txt
(use -f to force removal)
--- AFTER ---
worktree f.txt: [line1 / MAIN]      <-- hand resolution DESTROYED
MERGE_HEAD now: GONE                 <-- the merge was aborted
conflict stages now: []              <-- stages 1/2/3 gone
```

`git rm --cached -r .` **refused and exited non-zero** — git protected the
operator. `git reset --hard` on the next line ran anyway and destroyed
everything. A pasted fenced block has no `set -e`, and the runbook nowhere says
"stop if a line fails".

**This is not an exotic case.** Class C is entered from a coordinated merge that
will not abort, and `ParticipantState::Conflicted` is routed through the very
same refusal that produces the class-C message
(`abort/preflight.rs`:62-74 → `require_rollback_form`). A conflicted member is a
*leading* class-C shape, not an edge one.

The block's gating is also inverted:

- the exclusion is prose **above** the block, not a check **inside** it;
- the block's first line checks `git stash list` with the comment
  `# check first: reset --hard discards work` — a non-sequitur, since
  `reset --hard` does not discard stashes — while the checks that matter
  (`git status --short`, `git ls-files -u`) appear nowhere;
- the "`git reset --hard` discards uncommitted changes … Commit or stash
  anything you want to keep before running it" warning is **below** the block
  (:249-250), i.e. read after pasting.

Staged-but-uncommitted work in a *clean-looking* member is also lost, verified:

```
before:  M  f.txt   A  new.txt   ?? untracked.txt
after:   ?? new.txt   ?? untracked.txt
f.txt: [line1 / MAIN]   new.txt: demoted to untracked   untracked.txt: survived
```

Untracked and ignored files survive (good). Staged modifications are destroyed;
newly-added files survive on disk but silently lose their staging.

### [P1-1] The remedy is provably ineffective for the triggers the same section names

:220-224 tells the operator that "the attribute-driven forms (`eol=crlf`,
`ident`, a foreign `filter=`) apply everywhere" are class-C triggers. The remedy
sets only `core.autocrlf` and `core.eol`. With `* eol=crlf` in `.gitattributes`:

```
bytes on disk: 610d 0a62 0d0a
>>> runbook class-C block <<<
bytes after block: 610d 0a62 0d0a     <-- unchanged, still CRLF
status after: []                       <-- "expect no output" PASSES
```

Green check, unchanged worktree, identical refusal on retry, no next step.
(For completeness: the **config-driven** case works correctly —
`610d0a620d0a` → `610a620a`, status clean. The block is a correct config
renormalizer; it is simply not an attribute one, and it cannot be.)

### [P1-2] The block passes through a state that arms `git clean` as total destruction

After `git rm --cached -r .` alone — the state left by any interruption between
the two lines:

```
D  f.txt      D  sub/g.txt
?? f.txt      ?? sub/
git ls-files -> []
```

Every file is simultaneously staged-deleted and untracked; the index is empty.
To a stressed operator this reads as *the repository just deleted itself*. The
obvious reflexes from there — `git clean -fdx` (now a whole-worktree delete,
since everything is untracked), `git commit -am wip` (commits a total
deletion), `git stash` — are all catastrophic. The runbook nowhere says:

- that `git rm --cached -r .` is **index-only** and does not touch the files
  (this is the one command on the page that *looks* like `rm -r`, and it is
  never explained — mandate item 1(c)); or
- "if you stop between these two lines, run `git reset --hard` to put the index
  back."

### Ruling on the block as a whole (builder review-focus item 1)

The **idiom is sound and correctly chosen** — `git rm --cached -r .` followed by
`git reset --hard` is the documented git recipe for refreshing a working tree
after changing line-ending settings, and I verified it does exactly that. The
builder did not invent it and it is not source-frozen prose. What is wrong is
everything around it: the gating, the ordering, the explanation, the scope
claim, and the verification. See §8 for the required shape.

---

## 4. Truth against the tree — spot verification at `07e1ac1`

### R3's landing changed nothing the runbook cites — verified, not assumed

`git show --name-only 07e1ac1` touches `.github/workflows/release.yml`,
`Cargo.toml`, `Cargo.lock`, `scripts/checks/merge_docs_manifest.json`,
`scripts/release.py`, `scripts/retained_readers/*`,
`test_retained_reader_harness.py`. No `store/`, no `open_merge_gate.rs`, no
`recovery_support.rs`, no `abort/`, no `preserve*`, no `stash`, no `version.rs`.
The version bump and release.yml are inert with respect to every claim below.

### Confirmed correct

- **Class A message** — byte-exact against `foreign_filter_refusal`
  (`src/git/gitbackend/recovery_support.rs`:117-124). `DirtyMember` via
  `recovery_dirty` :14-16 ✓. `'<non-utf8 filter name>'` :69 ✓. `lfs`
  allowlisted **by name, before the config probe** :73-75 ✓. Deletions skipped
  (`delta.status() == Deleted → continue`, :56-58) — so "for every path the
  checkout **would write**" (:72-73) is precisely right ✓. The refusal fires on
  *presence* of `filter.<n>.clean` **or** `.process` in the effective config
  snapshot (:102-115), which is exactly what §"The deliberate false positives"
  (:119-127) describes ✓.
- **Class C message** — byte-exact (`abort/preflight.rs`:206), code
  `MergeRecoveryRequired` with `.with_member(member_id, path)` (:269-271), so
  the `--json` `member_id`/`member_path` claim (:198) holds ✓.
- **Class C mechanism (builder item 7 — the `mod v1_rollback` bound)** —
  **CONFIRMED.** All three production callers of the blob-exact comparator on
  the abort path are inside `mod v1_rollback`: `abort/participants.rs` (module
  :79-341) at :186 and :246, and `abort/preflight.rs` (module :26-272) at :179.
  The comparison is genuinely raw-byte — `read_worktree` reads with `std::fs`
  (`preservation_image.rs`:491-530) and compares `live == expected` against the
  tree's blob (:167-199). The preimage machinery is v1-only (`preserve.rs`:26,
  `model/v1/*`). So :200-206 ("compares worktree bytes against the recorded
  commits exactly … the smudged form matches neither recorded commit") is
  accurate ✓.
- **Class D invisibility — CONFIRMED, both entry points.**
  `FileMergeStore::discover_open` (`store/mod.rs`:78) and
  `classify_open_record` (:213) both route through `record_files` (:419-435),
  which is a single **non-recursive** `read_dir` filtered on
  `extension == "yaml"`. A `quarantine/` subdirectory is invisible exactly as
  `done/` is (`DONE_DIR = ".gwz/merge/done"` :31 is likewise a subdirectory of
  `MERGE_DIR` :30). Atomic-write temporaries are `*.tmp` (:445) and are not
  candidates ✓.
- **Open-merge gate sets.** The runbook's **blocked** list (:274-276) is
  complete and exactly correct against the 13 `Block` rows
  (`operation/open_merge_gate.rs`:64-76): `BranchMutate, Capture, Commit,
  Forall, InitUpdate, Materialize, Pull, Push, RepoMutate, Snapshot,
  StashMutate, TagMutate, MergeStart` ✓. The **available** list (:276-278) is
  correct as far as it goes — see [P2-1] and [P3-3].
- **Backup-ref format** `refs/gwz/merge/<merge-id>/<owner-key>/head`, owner key
  = member id or `root` ✓ (`preserve/plan.rs`:115 and :90;
  `preserve/artifacts.rs`:282; `record_wire/archive/cleanup.rs`:216;
  `gc.rs`:78). The ref is created at the live head at preservation time
  (`gitbackend/preservation.rs`:6-45), so the runbook's framing "a backup ref is
  a branch point" (:394-395) and its `git branch recovered-work …` rescue
  (:398) are **correct** ✓ — this is the best-gated destructive step on the page.
- **Stash bundles** `.gwz/stash/bundles` ✓ (`stash/mod.rs`:13
  `STASH_BUNDLE_DIR`).
- **`gwz add` narrowing** ✓ — `enforce_open_merge_stage_targets`
  (`merge/runtime/open_gate.rs`:32-63) admits only participants whose
  `conflict_role == NativeMerge`, and refuses with "add may target only its
  conflicted participants". The runbook's user-level paraphrase (:277-278) is
  fair.
- **`gwz: <Code>: <message>`** ✓ (`gwz-cli/src/lib.rs`:162/:185/:193 with
  `clirequest/common.rs`:145-150). Minor: `code` is `Option`, so a code-less
  error prints `gwz: <message>` — the runbook's "GWZ prints human errors as"
  (:41) is true of the gated cases it documents.
- **Class D messages** — open-merge gate (:270) byte-exact against
  `open_merge_gate.rs`:96-98 ✓; multiple-records (:288) byte-exact against
  `store/mod.rs`:88-91 and :245-248 ✓; `MergeRecordUnreadable` is a real code ✓.
- **Class A's derivation (builder review-focus item 2) — CONFIRMED as a state
  derivation.** Unset the driver → the filters-off recovery checkout writes
  stored bytes → restore the key ⇒ stored bytes on disk under restored coverage
  with a configured driver, which is class B's shape exactly (:159-165). The
  follow-up **is** mandatory and the runbook is right to say "Do not skip it"
  (:108). Only the command it prescribes is wrong — [P0-1].

### [P1-3] Class C's "Ordinary merges are not affected" is FALSE at `07e1ac1`

:225-226 reads: *"**Ordinary merges are not affected.** Starts without
`--no-ff` write v0 records and do not use this comparison."*

The second clause is right about the **writer**: `model/version.rs`:133-158 —
`--no-ff` → `V1`, `Normal`/`FfOnly` → `ACTIVE_WRITER_FLOOR` = `V0` (:39). The
bolded conclusion does not follow, because an ordinary record does not stay on
the v0 lifecycle:

- `adapt_before_mutating` (`merge/runtime/dispatch.rs`:394-416) migrates an
  open v0 record onto the **v1 lifecycle** on `Resume | Abort`;
- gated on `AdaptationPrecheck::MayAdapt` = state `Finalizing` **and mode
  `Normal`** (`store/mod.rs`:254-261) — i.e. an *ordinary* start, by
  construction;
- `adapt_open_v0` (`record_wire/open_v0/adapter.rs`:80-110; whitelist
  `classify_open_v0` :42-77) admits a one-member participant in state
  `FastForwarded` or `UpToDate`;
- `abort/preflight.rs`:75-88 routes `FastForwarded` straight into
  `require_rollback_form(observe_v1_participant_rollback(…, ResetIntegrated))`
  — the raw-byte comparator. In a CRLF worktree both legs return false →
  `Ambiguous` → **the class-C message, from an ordinary merge.**

Mitigation: the diagnosis table routes on the **message** (:32-43), and the
message is identical, so the operator still lands in class C. The harm is that
the bullet then tells them they are in the wrong place — under stress, that is
exactly the push that sends someone hunting for a more destructive answer.
Misdiagnosis is the norm; the runbook's own rule 1 (:41-43) is the right
instinct and this bullet contradicts it.

Replace with the honest bound:

> **This is the `--no-ff` recovery path.** `--no-ff` starts always use it; an
> ordinary start can also reach it once its record is migrated during abort.
> Match on the message, not on how you started the merge.

### [P2-5] The runbook makes v0/v1 truth claims and is outside the truth gate

`gwz-core/scripts/checks/merge_docs_manifest.json` pins 11 sources — four of
them gwz-cli docs — and `check_merge_docs.py`:123-199 iterates `sources` only,
applying `global_forbidden` per listed source. `docs/MergeRecovery.md` is not a
source, so it is **not covered at all**, while asserting exactly the class of
sentence the gate exists to pin: "A `--no-ff` start writes a v1 coordinated
merge record" (:200) and "Starts without `--no-ff` write v0 records" (:226).

When M5c raises `ACTIVE_WRITER_FLOOR`, both sentences become false and nothing
fails. The gate is the existing mechanism for precisely this and the new page
should be a source in it.

Re-run against the R4 worktree (symlinked workspace: live gwz-core + the R4
gwz-cli + live dev-docs):

```
merge document consistency: ok (11 sources, 147 assertions)
```

Identical to the unmodified-gwz-cli baseline. **R4 breaks nothing** — the
finding is missing coverage, not a regression. (Consistent with G4's recorded
position: this is a workspace-root-reading checker and stays un-wired in
`release.yml` with its blocker recorded.)

### [P2-1] `gwz merge --gc` is listed as "Still available" and it deletes evidence

The gate claim is true — `MergeGc → Allow` (`open_merge_gate.rs`:62). But:

- `store/gc.rs`:3-18 — `--gc <id>` does `fs::remove_file(done/<id>.yaml)`;
- `--gc` with no id runs `store/retention.rs`:5-45, deleting **every archive
  past `ORDINARY_RETENTION = 20`** (`store/mod.rs`:32) that owns no backup ref.

The same section then instructs the operator to compare
`.gwz/merge/done/<id>.yaml` (step 4, :365-370) and says "Never touch …
`.gwz/merge/done/`, the archived records" (step 6, :405). Naming `--gc`
unqualified under "Still available", inside a procedure whose rule 3 is *never
delete*, is a live footgun. Drop it from the list, or annotate it: "`gwz merge
--gc` runs, but it deletes archived records — do not run it during this
procedure."

### [P2-2] Class E's primary command cannot work in the leading class-E state

:429-433 names "After GWZ clones or materializes a repository that uses Git
LFS" as the first symptom. That clone runs `CheckoutBuilder::disable_filters(true)`
(`transport.rs`:31 — one of exactly three such sites, confirmed by R1 §2.6),
so the LFS smudge filter never ran and **the LFS objects were never fetched**
into `.git/lfs/objects`. `git lfs checkout` materializes only objects that are
already local; it does not download. The correct command for that state is
`git lfs pull` (or `git lfs fetch` then `git lfs checkout`).

The runbook's *secondary* form (`rm <paths>; git checkout -- <paths>`,
:459-462) **does** work, because the smudge filter fetches on demand — so the
two are in the wrong order of preference, the same inversion as [P0-1].

For the second stated symptom ("after a recovery checkout touched LFS-managed
paths") the objects are usually already local and `git lfs checkout` is fine;
the fix is therefore to split the two symptoms, not to delete the command.

This is R1's cross-lane hand-off `[P3] F-9`
(`GwzM5-8A1ReleaseR1-ReviewState.md`:285-292) reaching user documentation for
the first time — and it lands with the wrong command.

*Execution caveat:* git-lfs is not installed on this host, so unlike every other
claim in this report I did not run this one. The mechanism is documented
git-lfs behaviour; I flag rather than assert a run.

### [P2-3] Rule 3 and step 5 contradict each other on deletion authority

- Rule 3 (:23-25): "No procedure here deletes a merge record, a `refs/gwz/`
  ref, a `gwz:`-prefixed stash, or a stash bundle. **Those may be the only
  surviving copy of your work.**"
- Step 5 (:373-381): "…and **the only things you should ever remove** there on
  GWZ's behalf: refs named `refs/gwz/merge/…/head`; one native git stash whose
  message begins `gwz:stash_<merge-id>:`."

Each is defensible alone. Together, at speed, step 5 reads as a licence to
delete the two artifacts rule 3 calls the last copy — and it sits immediately
above a `git reset --hard` whose own safety note (:394-399) depends on that
backup ref still existing. One clause fixes it: *"…and nothing in this runbook
asks you to remove them; if you ever do, only after the work they hold is
somewhere else."*

Related: the escape design freezes the stash matcher as the **exact** message
`gwz:stash_<merge-id>: merge preservation` (§4.4 item 5; validated at
`model/v1/validate/preservation.rs`:310). The runbook widened it to "begins
`gwz:stash_<merge-id>:`". Ordinary `gwz stash` uses the same `gwz:<stash_id>:`
prefix shape (`handle_stash/shared.rs`:247-249, `commands.rs`:38-39), so the
embedded merge id still disambiguates in practice — but a delete-authorisation
list should carry the frozen exact form, not a widened prefix.

### [P2-4] Step 5's `<before-commit>` can be unobtainable exactly when it is needed

Step 2 sources the pre-merge commits from `gwz merge --status` (:326-332),
hedged with "**If** `gwz merge --status` still runs". In the sub-case the
section itself introduces (:285-292 — discovery fails on multiple records or
`MergeRecordUnreadable`) it does not run; after step 3 parks the record it still
will not. Step 5 then asks for `<before-commit>` with no source. The values are
in the parked record's YAML (step 6 says to copy it out to read it) and the
backup ref is a usable branch point — the runbook should say both, explicitly.
As written, step 5 can be un-executable in precisely the case that motivated the
procedure.

---

## 5. The nine narrowings — scope limit and rulings on the three named

**Six of nine are unreviewed.** The section they live in is not in the delivered
object (§0). I record them as unreviewed rather than signing them; if the lane
owner wants them adjudicated, file the text and I will rule.

### #2 — force-abandon omitted. **Silence is the WRONG treatment. Add one sentence.**

Omitting the *operation* is right and not close: force-abandon does not exist in
0.11.0, it is v1-only by design (`GwzM5-8OperatorEscapeDesign.md` §4.3
preconditions; §6.2 excludes v0), and it needs a wire amendment that has not
landed. Documenting it would promise a command.

Being silent about the *absence* is wrong, because the page manufactures the
search itself:

- it opens "It covers **every** refusal and stuck state GWZ 0.11.0 can leave you
  in" (:4-5);
- class D then says "It does not finish the merge and it does not undo it"
  (:303-304);
- step 7 says an un-repairable record is "**abandoned**" (:427) — using the word
  while never saying that abandonment has no command.

An operator who has just been told the tool will neither finish nor undo their
merge goes looking for the thing that terminalizes it, and the places they look
are `gwz merge --help` and the issue tracker. Add one sentence — in class D's
"What it means", or as step 7's closing line — in plain user voice:

> There is deliberately no command that force-abandons a merge record. A record
> that cannot be completed or undone stays parked; parking it **is** the end
> state, not a step toward one.

One sentence, closes the search, pre-commits nothing: it is a statement about
today's tool, not about the roadmap. This is also the sentence that keeps
§9.1's "no tool escape and no documentation" finding from being half-closed.

### #5 — the hardened `reset --hard`. **Gates are NOT sufficient. This is [P0-2]/[P1-2].**

Two sites, hardened to different standards:

- **Class D step 5 (:387-399) is the good one.** `git status` and `git log
  --oneline -5` are inside the same block, the "discards work" warning is in
  the surrounding prose, and it names the backup ref as a branch point with a
  literal `git branch recovered-work …` rescue command. That gating is adequate
  and should be the template.
- **Class C (:240-250) is the bad one.** Exclusion in prose above the block;
  the in-block check is `git stash list` — the wrong artifact, with a comment
  that misdescribes what `reset --hard` discards; the "discards work" warning
  below the block; git's own refusal on the preceding line defeated by the next
  line. Empirically it destroys a mid-conflict resolution and MERGE_HEAD (§3).

Required shape for class C — a hard stop *inside* the block:

```sh
git -C <member> status --short      # STOP if this prints anything
git -C <member> ls-files -u         # STOP if this prints anything: you are
                                    # mid-conflict — go to case F
```

plus: state that `git rm --cached -r .` is index-only and does not touch your
files; add "if you stop between these two lines, run `git reset --hard` to put
the index back"; and add the attribute-driven caveat from [P1-1].

### #8 — the `.gitattributes`-edit prohibition. **RIGHT CALL. Keep it verbatim; promote it.**

:114-117: *"Do not instead edit `.gitattributes` in the worktree to hide the
coverage from the check. That suppresses the refusal without changing what gets
written, and lands you in case B with no record of why."*

Correct, and mechanically true: the gate reads the **pre-checkout** attribute
stack via `AttrCheckFlags::default()` = `FILE_THEN_INDEX` (R1 §3(a):309-314;
C2 residual), so a worktree edit does suppress the refusal, while
`disable_filters` still writes raw bytes. It is the highest-value prohibition on
the page, because it is the *obvious* workaround and it silently converts a safe
refusal into the class that — per [P0-1] — nothing can currently detect. It also
names the consequence rather than merely forbidding, which is what makes
prohibitions survive contact with a stressed operator.

Two improvements: promote "never edit `.gitattributes` to get past a refusal"
into the Three Rules (rule 3 currently says "never hand-edit" only about
records), and note that the same edit is the obvious workaround for class C too.

---

## 6. The two rulings requested

### Ruling 1 — class D step 7's sidecar-less restore: **SHIPS, with two required changes.**

Ship it. The mechanism is real and reversible: `record_files` enumerates only
`*.yaml` directly under `.gwz/merge/`, so moving the file back restores it to
the open slot exactly and the ordinary lifecycle resumes with no adapter
involvement. The design anticipates the hand-parked shape and commits to
recognising it — "recognized by later tool versions (minus the sidecar, which
`--status` reports as 'quarantined without sidecar')" (§4.4 item 3) — so this is
a supported shape, not an improvisation. Parking with **no** documented restore
would be strictly worse: it would make an explicitly reversible operation read
as terminal, which is the same failure mode as #2's silence.

Two required changes:

1. **"byte-for-byte the one you parked" (:419-420) is an instruction with no
   instrument.** The tool's restore is gated on a sidecar digest (§4.2
   *Restore*); the hand procedure has no sidecar and therefore nothing to
   compare against — unless step 2's `cp -a` copy is named as the base. Make it
   mechanical:
   `diff <workspace>/.gwz/merge/quarantine/<merge-id>.yaml /somewhere/outside/merge-backup/<merge-id>.yaml`
   — and do not move it back unless that prints nothing.
2. **Add the missing precondition as a check, not a condition.** :421 says "only
   when no other merge is open" — true but incomplete. `write_open`
   (`store/mod.rs`:119-129), `discover_open` (:83-93) and `classify_open_record`
   (:240-249) all fail hard on a second `*.yaml` in the open slot, so restoring
   while any other `*.yaml` is present converts a working workspace into the
   multiple-records wedge. Say: "`ls <workspace>/.gwz/merge/*.yaml` must print
   nothing before you move it back."

Change nothing else. The hand-edited-record prohibition (:424-427) is correct,
matches [Q4]'s formally-shut door, and should ship verbatim.

### Ruling 2 — the `quarantine/` naming pre-commitment while [Q7] is open: **SHIP THE DIRECTORY NAME.**

[Q7] (`GwzM5-8OperatorEscapeDesign.md`:766-768) is open over
`--quarantine`/`--restore`/`--force-abandon` versus `--park`/`--shelve`/
`--abandon`. It is explicitly about **CLI surface** — "frozen into
protocol/help text at freeze time". The runbook names only the **directory**,
in `mkdir`/`mv` commands the operator types by hand. Two different naming
decisions; only one of them is open.

The directory name should ship, for a reason stronger than convenience: **it is
already the name the tool will look for.** §4.2 fixes
`.gwz/merge/quarantine/<merge-id>.yaml` as the mechanism, and §4.4 item 3
promises that hand-parked records are recognised by later versions. If the
runbook told operators any other name, every record parked by a 0.11.0 operator
would be invisible to the tool that eventually ships — a compatibility break
manufactured by documentation. **Pre-committing here is the low-risk choice; the
risk is in not committing.**

Two conditions:

1. **The page must not name a future flag — and today it does not.** Verified:
   `quarantine` appears only as a path component (:339-341, :347, :369-370,
   :407, :415-416), never as `--quarantine`. That is exactly the right line;
   whatever [Q7] decides, this page stays true. Hold it.
2. **Settle the verb.** The page alternates "parks the record" (:304), "Park the
   record" (:334), "leave it parked" (:427) against the directory
   `quarantine/`. Harmless today, but if [Q7] picks `--park` the two
   vocabularies swap roles and the page reads inconsistently. Pick **park** as
   the verb — it is the better user word and the page already leads with it —
   and let `quarantine/` be the directory's name without commentary, the way
   `done/` is used at :348 and :405 without explanation.

---

## 7. Findings register

| ID | Sev | Where | Finding |
| --- | --- | --- | --- |
| P0-1 | **P0** | :100-104, :137-140, :178-182, :168-171 | `git checkout -- <paths>` is a no-op in the diagnosed state; `git status` is silent; the `# expect no output` verification certifies the failure. Class A follow-up and class B remedy both wrong. |
| P0-2 | **P0** | :234-250 | Class C block destroys a mid-conflict member's resolution and MERGE_HEAD; git's own refusal is defeated by the next line of the same paste-block. Fence is prose-above, checks are inside-and-wrong, warning is below. |
| P1-1 | P1 | :220-224 vs :240-247 | Remedy is provably ineffective for the attribute-driven triggers the same section names; the block's final line certifies the failure as success. |
| P1-2 | P1 | :244-245 | Intermediate state (empty index, everything untracked) arms `git clean -fdx` as whole-tree deletion; `git rm --cached` never explained as index-only; no interruption-recovery note. |
| P1-3 | P1 | :225-226 | "Ordinary merges are not affected" is false: `adapt_before_mutating` migrates Normal-mode `Finalizing` v0 records onto the v1 lifecycle on abort, reaching the raw-byte comparator. |
| P1-4 | P1 | — | *(Confirmation, not a defect.)* Builder item 7's `mod v1_rollback` bound holds and validates the class-C mechanism — but it bounds the comparator, not which record versions reach it. Hence P1-3. |
| P2-1 | P2 | :277 | `gwz merge --gc` listed "Still available" without saying it deletes archived records that steps 4 and 6 depend on. |
| P2-2 | P2 | :453-455 | `git lfs checkout` cannot work after a gwz clone (objects never fetched under `disable_filters`); `git lfs pull` is the command. Secondary form is the working one. |
| P2-3 | P2 | :23-25 vs :373-381 | Rule 3 ("never delete, may be the only copy") contradicts step 5 ("the only things you should ever remove"); stash matcher widened from the design's frozen exact form. |
| P2-4 | P2 | :326-332, :387-391 | Step 5's `<before-commit>` has no source in the discovery-failure sub-case; the record YAML and the backup ref are never named as alternatives. |
| P2-5 | P2 | :200, :226 | New page makes v0/v1 truth claims and is not a source in `merge_docs_manifest.json`; goes stale silently when M5c raises the floor. Gate re-run green (11 sources, 147 assertions) — missing coverage, not a regression. |
| P3-1 | P3 | :91-94 | `--get` returns only the highest-priority value; refusal fires on either key at any level. Use `--get-all`, unset every occurrence of both. No literal unset command given (inconsistent with class C's literal `reset --hard`). |
| P3-2 | P3 | :357-358 | `record_files` selects on extension without `is_file()`; a *directory* named `*.yaml` is enumerated and then fails as `MergeRecordUnreadable`. Say "any `*.yaml` name, file or directory". |
| P3-3 | P3 | :276-278 | "Still available" omits `InitExistingPlan` (`Allow`). Harmless omission from a positive list; recorded for completeness. Blocked list is complete and exact. |
| P3-4 | P3 | :314-324 | `cp -a` backups have no verification step, under a heading reading "Nothing further is permitted until this exists." |
| P3-5 | P3 | throughout | Four headings repeat across classes (`How it presents`, `What it means`, `What was changed`, `Recovery`); MkDocs renders a sidebar of identical entries. All seven in-page anchors resolve (verified mechanically). |
| P3-6 | P3 | — | `g00` (3 tests) covers help text and `CLI.md` staleness only; it does not touch the new page, the nav entry, or the links. The "3/3" claim is true but is not evidence about this change. |
| P3-7 | P3 | :260-261 | Names an unshipped command by exact future spelling (`gwz repair --renormalize`) while §R6 timing is still OPEN (release plan §8 item 3). Unlike `quarantine/`, this has no compatibility argument. Soften to "a command that performs this normalization for you is planned; it is not part of 0.11.0." |
| P3-8 | P3 | :486-493 | Class F's "do not" list omits `git rm --cached` — the one command on this page that produces the [P1-2] state. Add it. |

---

## 8. Remediation list — six edits, then GO

1. **[P0-1]** Class A (:100-104) and class B (:178-182): adopt the class E form
   — `rm <paths>` then `git checkout -- <paths>`. Replace both `# expect no
   output` verifications with a content check (or `touch` + status). Add to
   :168-171 that real `git` is also blind here until the file is removed or
   touched.
2. **[P0-2] / [P1-1] / [P1-2]** Rewrite the class C block per §5 #5: the two
   STOP checks inside the block, `git rm --cached -r .` explained as index-only,
   the interruption-recovery line, the "discards work" warning moved above the
   block, and the attribute-driven caveat.
3. **[P1-3]** Replace the "Ordinary merges are not affected" bullet with the
   honest v1-recovery-path bound (§4).
4. **[P2-1] / [P2-2] / [P2-3] / [P2-4]** Four one-to-three-line edits: annotate
   or drop `--gc`; split class E's two symptoms and lead with `git lfs pull`;
   add the rule-3/step-5 reconciling clause and restore the frozen stash
   matcher; name the record YAML and the backup ref as `<before-commit>`
   sources.
5. **§5 #2 and §6** Add the force-abandon sentence; apply the two step-7
   restore changes; settle "park" as the verb.
6. **[P2-5]** Add `gwz-cli/docs/MergeRecovery.md` to
   `merge_docs_manifest.json` with its v0/v1 sentences as `required`
   assertions, so the M5c floor raise cannot silently falsify them. *(gwz-core
   change — schedule with R5 or R6, not a blocker for the page itself.)*

P3s are lane-owner discretion.

---

## 9. Audience fitness

- **Diagnosis flow: right structure, and it is first.** "Which Case Am I In"
  (:30-43) is the third heading, above every remedy, keyed on symptom text with
  links. The match-on-message-not-code rule (:41-43) is correct and important
  given R1 §2.5's finding that `DirtyMember` is shared across unrelated
  refusals. This is the single best structural decision on the page.
- **The mapping is unambiguous except where a case body contradicts the table**
  — [P1-3] is the one place a correctly-routed operator is told they are in the
  wrong case.
- **"What was changed" discipline: excellent, and consistently applied.** A
  (:79-82, "**Nothing.**"), B (:157-165), C (:228-230, "**Nothing.**"), D
  (:294-304), E (:446). Every class states what was and was not mutated before
  any remedy. This should survive remediation untouched.
- **Class F catches the fall-through**, and its "do not" list (:486-493) is the
  strongest safety text on the page — with the [P3-8] gap.
- **Tone: clean.** No dev-doc names, no `[Qn]`, no review vocabulary, no
  "DRAFT", no `U7`/`U8(a)` labels, no shas. The escape design stays
  un-referenced as required. One wobble: "an availability limit, not a
  correctness failure" (:208-209) is internal register — "GWZ is refusing
  because it cannot prove the state, not because something is broken" says it in
  user voice.
- The prose is genuinely good — "Nothing on this page happens automatically"
  (:12-13); "Failing closed costs you one manual step; guessing wrong costs a
  silently divergent worktree under a moved branch" (:126-127); "The protection
  is gone, not the problem" (:352). **That quality is exactly why [P0-1] is
  dangerous.** An operator will trust this page completely.

---

## 10. Indexing — CORRECT, ship as-is

All four edits verified against the diff:

- **`mkdocs.yml`:67** — nav entry in the same Guides section, after
  Troubleshooting, before Agent Bootstrap. Correct: it is a guide, and it sits
  adjacent to its funnel.
- **`docs/README.md`:20** (task-table row: "Get out of a merge that will not
  finish or close") **and :35-36** (guide-list entry). Two entries, but they are
  the two different indexes that file maintains — not duplication.
- **`docs/Troubleshooting.md`:179-196** — a funnel section with three symptoms,
  a pointer, and a do-not-delete line. Right depth: it does not restate the
  procedure.
- **`docs/commands/merge.md`:268-271** — a three-line pointer placed in the
  rollback section, immediately after the "rerun the same command" paragraph.
  Correct: pointer only, no duplication.
- Relative link `../MergeRecovery.md` from `docs/commands/` is correct; all
  seven in-page anchors resolve (checked mechanically against the heading set).

**Gates re-run by me, in the R4 worktree:**

```
running 3 tests
test tests::g00::usage_text_covers_standard_help_and_commands ... ok
test tests::g00::root_help_advertises_hosted_docs_near_top_and_at_end ... ok
test tests::g00::cli_reference_doc_matches_generated_clap_help ... ok
test result: ok. 3 passed; 0 failed; 0 ignored; 0 measured; 77 filtered out

merge document consistency: ok (11 sources, 147 assertions)
```

Built against `gwz-core v0.11.0` at `07e1ac1`. The **g00 3/3 claim is
verified.** See [P3-6] for what it does and does not prove, and [P2-5] for the
docs-gate coverage gap.

---

## 11. What this review did not cover

- The six unnamed narrowings (§0 scope limit) — text not filed.
- Rendered-site verification (no mkdocs build run); anchors and links checked
  statically.
- The class-E LFS leg was not executed — git-lfs is not installed on this host
  ([P2-2] flags the mechanism, and says so).
- Prose copy-editing beyond the safety-relevant register notes in §9.
- Anything in gwz-core outside the claims the runbook makes. R1's and R2's
  conformance findings are consumed, not re-adjudicated.
