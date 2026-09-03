# GWZ Merge Wedge Runbook — released v0 line (v0.10.5)

Date: 2026-08-16

Status: **DRAFT — pending review.** This runbook discharges the documentation
debt named in `GwzM5-8OperatorEscapeDesign.md` §9.1: "the released v0 line can
wedge today with no tool escape and no documentation (runbook owed to current
users independent of A1)". Its manual procedure is the user-facing rendering
of that design's §4.4, restricted to what actually exists on the released
line. Every command, flag, file path, and quoted error message below was
verified against the `v0.10.5` tags of `gwz-core` and `gwz-cli`. Nothing in
this document refers to unreleased machinery: there is no `--quarantine`, no
`--restore`, and no `--force-abandon` flag on the released line — if you have
read a design document mentioning those, they do not exist in the binary you
are running. Steps whose end-to-end behavior was confirmed by reading the
released code but not by executing a staged wedge are marked
**[UNVERIFIED — needs a staged reproduction]**.

Audience: users of `gwz` v0.10.x whose coordinated merge is stuck. This is
user documentation, not a program document. Work top to bottom: diagnose
first, try the tool's own exits in order, and only then touch anything by
hand.

---

## 1. What a wedged merge looks like

While a coordinated merge is open, GWZ deliberately blocks most mutating
commands. Every blocked command fails with exactly this error (error code
`open_operation` in `--json` output):

```
merge 'MERGE_ID' is open; this command is blocked until it is recovered; use merge status, merge continue, or merge abort
```

Blocked while a merge is open: `gwz pull`, `gwz push`, `gwz commit`,
`gwz capture`, `gwz snapshot` (creation), `gwz materialize`, `gwz forall`,
`gwz init` (updating an existing workspace), all `gwz repo` mutations
(add/create/sync/clone/attach/detach), `gwz branch` and `gwz tag` mutations,
all `gwz stash` mutations (push/apply/pop/drop), and starting another
`gwz merge`.

Still available: `gwz status`, `gwz ls`, `gwz diff`, the list forms of
`branch`/`tag`/`stash`/`snapshot`, `gwz merge --status`,
`gwz merge --continue`, `gwz merge --abort` (with or without `--preserve`),
and `gwz stage` restricted to the merge's own conflicted repositories
(anything else gets: `merge 'MERGE_ID' is open; add may target only its
conflicted participants; use merge status to inspect the allowed
repositories`).

This blocking is normal and healthy while you resolve conflicts. You are
**wedged** only when the tool's own exits — `--continue`, `--abort`,
`--abort --preserve` — all refuse, and the refusal points at something that
no longer exists (a deleted repository, a pruned commit, a dropped stash).
Section 4 tells you which situation you are in from the exact error text.

There is no destructive override on the released line. `gwz merge` combined
with the global `--force` flag is rejected outright:

```
merge does not support a force/destructive policy
```

## 2. First response — always do this before anything else

Do these four things before you attempt any fix, in this order.

**2.1 Stop other activity.** Finish or stop any other `gwz` or `git` commands
running in this workspace, in any terminal. If a command appears hung, note
that a concurrent GWZ mutator holds an advisory lock at
`.gwz/locks/workspace-mutator.lock`; a competing command fails with
`workspace mutator lock is already held`. The lock is released automatically
when the holding process exits — killing a stuck process is enough; never
delete the lock file.

**2.2 Look before touching.** From the workspace root:

```sh
gwz merge --status
gwz merge --status --json > merge-status.json
```

`--status` is read-only and always safe to run. For an open merge it prints,
among other lines:

```
state: <executing|awaiting-resolution|halted|finalizing|preserving|rolling-back|recovery-required|completed|aborted>
merge: <merge-id> (open)
recovery commands:
  inspect:  gwz merge --status
  continue: gwz merge --continue
  abort:    gwz merge --abort
  preserve: gwz merge --abort --preserve
participants:
  <path> (<member-id>)  <participant-state>
    recorded: branch <branch>; before <commit>; result <commit or ->
    live: commit <commit or unknown>
    recovery: continue <eligible|blocked>; abort <eligible|blocked>
    drift: <kind>: <message>
```

Keep `merge-status.json`. The `recorded:` lines carry the exact branch names
and commit ids every later repair step needs.

**2.3 Preserve the evidence.** Copy the merge's records and preservation
artifacts aside before any repair attempt, so no later step (yours or the
tool's) can destroy the only copy of something:

```sh
cp -R .gwz/merge /tmp/gwz-merge-backup-$(date +%Y%m%d%H%M%S)
cp -R .gwz/stash/bundles /tmp/gwz-bundles-backup-$(date +%Y%m%d%H%M%S) 2>/dev/null || true
git for-each-ref 'refs/gwz/**' > /tmp/gwz-root-refs.txt
git stash list > /tmp/gwz-root-stashes.txt
```

and in **every** member repository directory that still exists:

```sh
git for-each-ref 'refs/gwz/**'
git stash list
```

Save those outputs. Nothing below is permitted until this exists.

**2.4 Never do these, at any point:**

- Never edit any file under `.gwz/merge/` in place. A hand-edited record has
  undefined behavior against the tool's integrity checks.
- Never delete a file under `.gwz/merge/` (including `done/`). Move, never
  delete.
- Never delete `refs/gwz/merge/...` backup refs, native stashes whose message
  begins with `gwz:stash_`, or files under `.gwz/stash/bundles/`. They may be
  the only surviving copy of your work.
- Never run history-destroying git commands (`git gc --prune`, forced
  re-clones over an existing checkout, `git stash drop`) in a member while
  its merge is unresolved.

## 3. The tool's own exits — try these in order

A stuck-looking merge is usually recoverable with the released commands. Try
them in this order; each is safe to re-run.

**3.1 `gwz merge --continue`** — legal from states `executing`,
`awaiting-resolution`, `halted`, `recovery-required`, and `finalizing` (in
`finalizing` it retries publication; on an open record already in state
`completed` it just finishes the archive step). It refuses from `preserving`
and `rolling-back` with:

```
merge 'MERGE_ID' in state Preserving cannot be continued
merge 'MERGE_ID' in state RollingBack cannot be continued
```

Before it acts, `--continue` re-observes every repository. If anything moved,
it refuses with a drift error naming the repository and what to restore
(section 4). Fix exactly what the message names, then re-run.

- Conflicts: resolve the conflicted files in the member, stage them with
  `gwz stage <paths>` (only the merge's conflicted repositories are
  accepted), then `gwz merge --continue`.

**3.2 `gwz merge --abort`** — verified rollback of every repository to its
recorded before-state, then archive. Legal from every open state except
`completed` (`merge in state Completed cannot be aborted` — run `--continue`
instead to finish it) and `preserving` (see 3.3). On a record already in
state `aborted` it finishes the archive step idempotently. Abort refuses if
any participant cannot be exactly restored; the refusal names the repository
and the reason (section 4).

**3.3 `gwz merge --abort --preserve`** — first snapshots your post-merge
commits and dirty worktree state into per-repository backup refs
(`refs/gwz/merge/<merge-id>/<member-id>/head`, and `.../root/head` for the
workspace root), a native stash whose message is exactly
`gwz:stash_<merge-id>: merge preservation`, and a coordinated bundle at
`.gwz/stash/bundles/stash_<merge-id>.yaml` — then rolls back. If a plain
`--abort` was refused because a repository holds work the rollback would
destroy, use this. It is the **only** exit from state `preserving`; a plain
abort there answers:

```
merge preservation is incomplete; retry `gwz merge --abort --preserve` so every preservation artifact is reconciled and verified before rollback
```

From state `rolling-back`, `--abort --preserve` is refused
(`illegal merge operation transition: RollingBack -> Preserving`); only plain
`--abort` resumes there.

After a successful preserve-abort, recover your work with the stash commands
(section 8).

**3.4 If a command reports the lock is held** (`workspace mutator lock is
already held`): another gwz process is running or died without exiting.
Wait, or find and kill it, then retry. This is not a wedge.

Only when the right command from this list refuses, and the thing it tells
you to restore is *gone for good*, continue to section 4.

## 4. Diagnosis — match the error message

Find your error below. Messages are quoted verbatim from the released code;
`...` marks variable parts. Participant-level errors are prefixed
`participant '<member-id>' at '<path>': ...` in drift listings, and carry the
member id/path in `--json` errors.

| You see | Meaning | Go to |
| --- | --- | --- |
| `participant '<id>' at '<path>' is missing; restore it at the recorded path before recovery` (drift kind `repository-missing`) | A member repository directory was deleted or moved | §5 (W1) |
| `participant is not ready to continue; blockers: [RepositoryMissing]` | Same as above, reported by `--continue` | §5 (W1) |
| `recorded <role> object <oid> is missing; restore the object before recovery` (drift kind `object-missing`) | A recorded commit was pruned from a member (force-push + gc, re-clone, `.git` damage) | §5 (W2) |
| `restore this repository to its recorded before commit and clean state, or abort` / `preserve or remove post-merge work and restore the recorded result before recovery` (drift kinds `head-advanced`, `head-rewound`, `head-diverged`) | A member branch moved after the merge recorded it | §5 (W2) |
| `restore the recorded target branch before continuing or aborting` / `restore the target ref to its recorded commit before continuing or aborting` | Branch switched or ref rewritten in a member | §5 (W2) |
| `finish or abort the unrelated merge before merge recovery` / `finish or abort the unrelated <op> operation before merge recovery` | A foreign git merge/rebase/etc. is active in a member | §5 (W2) — usually fixable |
| `restore the expected MERGE_HEAD before recovery` / `restore the recorded native merge metadata before recovery` | The native conflict state in a member was altered | §5 (W2) |
| `restore the recorded clean index before recovery` / `preserve or remove unrelated worktree changes before recovery` (kinds `index-modified`, `worktree-modified`) | Uncommitted local changes in the way — normally fixable by stashing/committing them with plain git | §5 (W2), usually resolvable |
| `preserve-abort cannot verify the original conflict worktree...; preserve partial resolution manually before aborting` / `preserve-abort refuses edited conflict files; preserve partial resolution manually before aborting` / `preserve-abort refuses modified conflict resolution state: ...` | You edited conflicted files, and preserve-abort will not guess | §6.4 |
| `recorded preservation ref is missing` / `preservation ref target changed` / `recorded preservation stash is missing` / `preservation stash identity changed` / `multiple native stashes use this merge preservation id` / `repository contains new work after an unrecorded preservation stash` / `preservation ref failed verification` / `preservation stash failed verification` | A preservation artifact from an earlier `--abort --preserve` was deleted or altered | §5 (W3) |
| `workspace artifact 'gwz.conf/gwz.lock.yml' changed from the recorded merge baseline` / `workspace artifact 'gwz.conf/gwz.yml' changed from the recorded merge baseline` | The workspace lock/manifest files were changed mid-merge | §5 (W4) |
| `workspace root candidate artifacts do not match an allowed publication prefix` / `workspace root HEAD does not match the recorded merge publication state` / `workspace root candidate artifacts changed before preservation` | The root repository or the merge's candidate files (`gwz.conf/gwz.lock.yml`, `gwz.conf/markers/<id>.yaml`) were altered during publication | §5 (W4) |
| `merge record at '<path>' is unreadable: ...` (code `merge_record_unreadable`) | The merge record file itself is damaged | §5 (W5) |
| `multiple merge records exist under '<workspace>/.gwz/merge'` | More than one `*.yaml` sits directly in `.gwz/merge/` (backup copies count!) | §5 (W5) |
| `merge record '<id>' uses schema '...' version N, which requires A1 (v1 integration/acceptance/no-ff); use a compatible newer GWZ` (code `unsupported_record_version`) | The record was written by a newer GWZ | Upgrade the binary. Not a wedge — do not touch anything. |
| `workspace artifact '<file>' does not match the abort baseline` | Abort finished rolling back but the workspace lock/manifest no longer match the baseline | §5 (W4) |
| `archived merge record '<id>' does not match the open record` | Both `.gwz/merge/<id>.yaml` and `.gwz/merge/done/<id>.yaml` exist with different content | §7.3 (W6) — human decision required |
| `preservation ref '<name>' no longer points to recorded commit '<commit>'` (from `gwz merge --gc <id>`) | Someone moved a backup ref; GC refuses to delete it | §8.2 (W6) |
| `cannot collect archived merge records while merge '<id>' is open` | `--gc` while a merge is open — finish or park the open merge first | §3 / §6 |

If your repositories are intact and the message names something you can put
back (a branch, a commit that still exists somewhere, uncommitted changes you
can stash with plain git), **do that and re-run the tool command**. That is
the designed recovery path and it preserves all guarantees. The rest of this
runbook is for when the named thing is unrecoverable.

## 5. The wedge classes

- **W1 — member repository permanently missing.** A repository the merge
  touched was deleted and cannot be restored at its recorded path (no remote
  to re-clone, or the merge commits were never pushed). `--continue` is
  always blocked. Plain `--abort` still succeeds if that participant shows
  `up-to-date` or `unattempted` in `--status` and has no pending action
  **[UNVERIFIED — needs a staged reproduction]**; in every other participant
  state the abort is blocked and you are wedged. If you can re-clone the
  repository at the same path with the recorded commits present, do that
  instead — then `--continue` or `--abort` work normally.
- **W2 — member repository mangled beyond exact recognition.** The recorded
  commits (`before`, `result`, `source`, `expected merge head` — printed by
  `--status`) no longer exist in the member, typically after a force-push
  plus prune or a fresh re-clone over the old checkout. Every re-observation
  reports `object-missing`/`head-rewound`/`head-diverged` drift forever.
- **W3 — preservation artifacts destroyed.** An earlier `--abort --preserve`
  recorded a backup ref or native stash, and it was since deleted
  (`git stash drop`, ref deletion) or altered. The preserve-abort retry
  refuses with one of the preservation messages in §4 and will keep refusing:
  the recorded evidence can never be re-verified.
- **W4 — root/publication drift.** The merge froze the workspace root's
  state (lock file `gwz.conf/gwz.lock.yml`, manifest `gwz.conf/gwz.yml`,
  marker `gwz.conf/markers/<id>.yaml`, root branch/commit) and something
  outside the merge changed them. Recoverable whenever you can restore the
  named file bytes or root commit (the record carries the expected digests;
  `merge-status.json` shows them); a wedge when the objects are pruned or the
  original bytes are unrecoverable.
- **W5 — record-plane damage.** The record file itself is unreadable, or
  more than one `*.yaml` exists directly under `.gwz/merge/`. **In this
  state every gated command dies at discovery — including `gwz status`,
  `gwz ls`, and all `gwz merge` operations.** Only the manual procedure in
  §6 applies. A common self-inflicted cause: copying a record to
  `record.yaml.bak` is safe (`.bak` is invisible), but copying it to
  `backup.yaml` in the same directory bricks the workspace with the
  "multiple merge records" error.
- **W6 — archive/GC refusals.** `.gwz/merge/done/` divergence or a `--gc`
  refusal. These never block the workspace (only open records gate
  commands); treat them at leisure via §7.3 and §8.2.

For W1-W4, first decide honestly: can the world be put back? If yes, put it
back and use §3. If no, the merge can never finish and can never verify a
rollback — proceed to §6.

## 6. Manual escape — park the record by hand

This is the §4.4 procedure of the escape design, as it applies to the
released line. It does **not** repair anything; it moves the merge's record
out of the tool's "open" slot so the workspace stops being blocked, while
keeping every byte of evidence. Your member repositories stay exactly as
they are — including half-merged branches — and cleaning those up afterwards
is a separate, deliberate step (§7).

Use this only when: the right tool exit from §3 refuses, the refusal names
something permanently unrecoverable (§5), or the tool cannot run at all (W5).

**6.1 Stop all GWZ and git activity in the workspace.** The tool's locks
cannot protect a hand operation.

**6.2 Snapshot everything first** (§2.3). If you skipped it, do it now.
Nothing below is permitted until the backup exists.

**6.3 Park the open record(s).** Move — never delete, never edit in place —
every `*.yaml` directly under `.gwz/merge/` into a `quarantine/`
subdirectory:

```sh
mkdir -p .gwz/merge/quarantine
mv .gwz/merge/<merge-id>.yaml .gwz/merge/quarantine/
```

Why this works on the released line: open-record discovery reads only
`*.yaml` files directly inside `.gwz/merge/` — subdirectories (like the
tool's own `done/`) are never consulted. Moving the record out empties the
open slot; the gate lifts immediately.

Rules that keep this safe:

- The subdirectory name must not itself end in `.yaml`.
- Move the file within the same filesystem (`mv`, not copy-then-delete), and
  leave no `*.yaml` copy behind in `.gwz/merge/`.
- In the W5 "multiple merge records" case, move **all** the `*.yaml` files
  there into `quarantine/`, under distinct names if they collide. If one of
  them is the real record and the others are strays you created, you may
  move the real one back afterwards — restore rules in §6.5.
- If `.gwz/merge/` itself is unreadable (permissions, filesystem damage),
  fix the directory's readability first; a tool cannot run, and this runbook
  cannot help, below the plane it stands on. That case is repair-by-hand of
  the directory, then re-diagnose.

**6.4 Special case: unpreservable conflict edits.** If you are here because
preserve-abort refused with `preserve partial resolution manually before
aborting`, your half-finished conflict resolutions exist only in that
member's worktree. Before any abort or park, save them yourself in the
member repository, e.g.:

```sh
cd <member-path>
git branch gwz-rescue-<merge-id>-partial   # if there are commits to keep
git stash push -u -m "rescue: partial conflict resolution for <merge-id>"
```

Then re-run `gwz merge --abort` (a plain abort of a pristine conflict state
is normally still possible once your edits are stashed away
**[UNVERIFIED — needs a staged reproduction]**), or park the record.

**6.5 Verify the workspace is unblocked, and what you now own.**

```sh
gwz merge --status
```

Expected output now begins:

```
action: merge
status: ...
state: idle
No coordinated merge is open.
```

and a previously blocked command (e.g. `gwz status`) runs again. From here:

- A new `gwz merge` may be started. Merge ids embed the process id and a
  timestamp, so a new merge will not collide with the parked one's leftover
  refs.
- **Your member branches may still sit at merged-but-never-published
  commits.** The parked merge published nothing; what looks "done" in a
  member is not reflected in the workspace lock. Decide per member (§7).
- **Restoring the record** (the inverse: `mv` the untouched file back from
  `quarantine/` into `.gwz/merge/`) re-enters the normal lifecycle — do this
  only if you have since repaired the world (e.g. re-created the missing
  repository) and want `--continue`/`--abort` to finish properly, only with
  a byte-identical file, and only while no other merge is open. Never move
  back a record you edited, and never move the strays back with it.

## 7. After parking — member cleanup, by hand and by choice

The parked merge left per-repository residue. This section is the complete
inventory. GWZ owns exactly two kinds of artifact inside a member
repository, and **nothing else in a member is GWZ's**:

1. backup refs matching `refs/gwz/merge/<merge-id>/<member-id>/head`
   (`.../root/head` in the workspace root), and
2. the native stash whose message is exactly
   `gwz:stash_<merge-id>: merge preservation`.

**7.1 Your branches (your data, your call).** For each participant the
parked record had integrated, `merge-status.json` (§2.2) records
`target_branch`, `before_commit`, and `resulting_commit`. If you want a
member branch back where it was before the merge:

```sh
cd <member-path>
git log --oneline <before_commit>..HEAD   # see what you would discard
git reset --hard <before_commit>
```

This **destroys** the merge commits on that branch (they remain reachable
via the reflog and any `refs/gwz/merge/...` backup ref until git prunes
them). If instead you want to keep the merged result, simply leave the
branch alone. This runbook never instructs deletion of your branches.

**7.2 GWZ's leftovers (safe to remove once you are done with them).** Only
after you have decided §7.1 per member, and only if you no longer need the
insurance:

```sh
git update-ref -d refs/gwz/merge/<merge-id>/<member-id>/head
git stash list                    # find: gwz:stash_<merge-id>: merge preservation
git stash drop stash@{<n>}        # destroys that saved worktree state
```

Both steps destroy recovery evidence — the backup ref is what makes §7.1
reversible, and the stash may hold uncommitted work that exists nowhere
else. Prefer restoring the stash first (§8.1). When in doubt, leave them;
they are namespaced and inert.

**7.3 W6(a): open/archive copies disagree.** If you hit
`archived merge record '<id>' does not match the open record`: the tool
keeps both copies precisely so a human can decide. The open copy
(`.gwz/merge/<id>.yaml`) is authoritative for an interrupted archive
*unless it is the damaged one*. Compare them (`diff`), move the copy you
reject into `.gwz/merge/quarantine/` under a distinct name — never delete
it — and re-run the interrupted command (`gwz merge --abort` for an
aborted record, `gwz merge --continue` for a completed one) to let the tool
finish archiving the survivor. **[UNVERIFIED — needs a staged
reproduction]**

**7.4 Never touch:** anything else under a member's `.git/`; the archives
under `.gwz/merge/done/`; parked records under `.gwz/merge/quarantine/`
(copy out to inspect); `.gwz/stash/bundles/` (a bundle may be the only
record of what a preservation stash contained).

## 8. Recovering preserved work, and normal cleanup

**8.1 After a successful `--abort --preserve`.** Your commits survive in the
backup refs and your dirty worktree state in the coordinated stash. Once the
merge record is closed (the workspace is unblocked):

```sh
gwz stash list                      # shows stash_<merge-id>
gwz stash list --expanded           # per-member detail from the bundle
gwz stash apply stash_<merge-id>    # restore worktree state (keeps the stash)
gwz stash pop stash_<merge-id>      # restore and consume
```

Preserved *commits* are on the backup refs; to bring them onto a branch:

```sh
cd <member-path>
git log --oneline refs/gwz/merge/<merge-id>/<member-id>/head
git cherry-pick <commits...>        # or: git branch rescued refs/gwz/merge/<merge-id>/<member-id>/head
```

**8.2 Archive retention and `--gc`.** Closed merges are archived under
`.gwz/merge/done/`. Archives holding preservation evidence are retained
until you collect them explicitly; plain archives are thinned automatically
past the 20 newest. When you no longer need a preserved merge's insurance:

```sh
gwz merge --gc <merge-id>
```

deletes that archive's backup refs (each checked against its recorded
commit first) and then the archive file. Native stashes and bundle files
are never deleted by `--gc` — drop the stash yourself afterwards
(`gwz stash drop stash_<merge-id>`), which also cleans up the bundle's
pending rows. `--gc` refusals:

- `cannot collect archived merge records while merge '<id>' is open` —
  close or park the open merge first.
- `preservation ref '<name>' no longer points to recorded commit '<commit>'`
  — someone retargeted a backup ref; GC will not delete a ref it cannot
  verify, and retains the archive. Your options: restore the ref to the
  recorded commit (`git update-ref <name> <commit>`) and re-run `--gc`, or
  accept permanent retention — it is untidy, not harmful. Deleting the ref
  by hand also unblocks a re-run, at the cost of whatever that ref pointed
  to **[UNVERIFIED — needs a staged reproduction]**.
- A missing member repository blocks `--gc <id>` of an archive that records
  evidence there (the ref check cannot run); the archive is retained. Park
  this as permanent retention or restore the repository first.
  **[UNVERIFIED — needs a staged reproduction; the exact error text comes
  from the git layer.]**

## 9. Verification — how you know the workspace is healthy

After any procedure above, in order:

```sh
gwz merge --status        # expect: state: idle / "No coordinated merge is open."
ls .gwz/merge/            # expect: no *.yaml directly here (only done/, quarantine/, locks aside)
gwz status                # runs (gate lifted); per-member summary looks sane
gwz ls                    # members enumerate
```

Then per touched member: `git -C <member-path> status` is clean (or dirty
only with changes you expect), and `git -C <member-path> log --oneline -3`
shows the branch where you decided it should be (§7.1). If you restored a
record and resumed the lifecycle instead, re-run the §3 exit you chose and
confirm it reaches `state: completed` or `state: aborted` followed by the
record appearing under `.gwz/merge/done/`.

## 10. When nothing works — what to attach to a bug report

Capture, from the workspace root:

1. `gwz --version` output, and your OS/filesystem (network filesystems are
   relevant — advisory locking caveats apply).
2. The exact command you ran and its complete output, plus the same command
   with `--json`.
3. `gwz merge --status --json` output (or its error, in W5).
4. The full `.gwz/merge/` tree **as files** — open records, `done/`,
   `quarantine/` — copied, not moved.
5. Your §2.3 evidence: `git for-each-ref 'refs/gwz/**'` and `git stash list`
   from the root and every member, and the `.gwz/stash/bundles/` copies.
6. For each participant the record names: whether the directory exists, and
   `git -C <path> cat-file -t <commit>` for each recorded commit
   (`before`, `result`, `source`) — this tells the maintainer which objects
   are truly gone.
7. What you already did, in order, including any manual moves.

Merge records contain branch names, commit ids, commit messages, and file
paths (conflict lists) — review before sharing outside your organization.

## 11. Honest limits of this runbook

- **A deleted member repository with integrated or conflicted state cannot
  be aborted by the tool, ever, on this line.** Parking the record (§6) is
  the only exit, and it consciously trades away the guarantee that "aborted
  means verified restored". The parked record honestly remembers what was
  never rolled back.
- **`.gwz/merge/` itself unreadable** (permissions, disk damage): no tool
  path exists; nothing here helps until the directory is readable again.
- **Open/archive copy divergence (§7.3) is deliberately left to a human.**
  Choosing the surviving copy rewrites terminal history; no heuristic is
  offered, and you should involve whoever operates your workspace tooling
  before choosing.
- **Hand-edited records are permanently out.** If a record was edited and
  put back in the open slot, outcomes are undefined; park it and treat the
  merge as escaped. Restore only byte-identical files.
- **Resetting user branches (§7.1) is always your decision.** The runbook
  gives the recorded coordinates; it cannot know whether your team already
  built on top of the half-merged result. When shared branches are involved,
  coordinate before resetting.
- This runbook covers the released v0.10.5 surface only. Newer releases may
  add first-class escape commands; if your `gwz merge --help` shows flags
  this document does not mention, prefer the newer documentation.

---

### Appendix A — where the released line keeps merge state

| Path | What it is |
| --- | --- |
| `.gwz/merge/<merge-id>.yaml` | The open merge record (exactly one may exist) |
| `.gwz/merge/done/<merge-id>.yaml` | Archived (closed) merge records |
| `.gwz/merge/quarantine/` | Not created by the tool; the manual parking spot from §6 (any subdirectory name not ending in `.yaml` works; this name matches the escape design so later tooling can recognize it) |
| `.gwz/locks/workspace-mutator.lock` | Advisory lock file; never delete |
| `.gwz/stash/bundles/stash_<merge-id>.yaml` | Coordinated-stash bundle written by `--abort --preserve` |
| `refs/gwz/merge/<merge-id>/<member-id>/head` (in members), `.../root/head` (in the root) | Preservation backup refs |
| native stash `gwz:stash_<merge-id>: merge preservation` (in members/root) | Preservation stash |
| `gwz.conf/gwz.yml`, `gwz.conf/gwz.lock.yml`, `gwz.conf/markers/<id>.yaml` | Workspace manifest, lock, and merge marker — the "baseline"/"candidate" files drift errors refer to |

### Appendix B — provenance map to the escape design

For reviewers: runbook classes W1-W6 correspond to
`GwzM5-8OperatorEscapeDesign.md` §3.3 classes as W1=U1, W2=U2, W3=U3 (v0
analogue), W4=U6 (v0 analogue), W5=U7, W6=U8. The design's U4/U5 involve v1
reverse-journal machinery that does not exist at the v0.10.5 tag and have no
v0 rendering. §6 here renders design §4.4 (manual surgery); §7.2's closed
member inventory renders design §7.1's "never touch member user data" rail;
the evidence-first rule in §2.3 renders design doctrine rule 3. The design's
`--quarantine`/`--restore`/`--force-abandon` operations are future work
([Q3]/[Q9] there) and are deliberately absent here.

### Appendix C — staged-reproduction checklist (UNVERIFIED items)

Behaviors asserted from code reading that should be exercised in a staged
wedge before this runbook leaves DRAFT:

1. §5 W1: plain `--abort` succeeding past a *deleted* member whose
   participant state is `up-to-date`/`unattempted` with no pending action.
2. §6.4: plain `--abort` of a conflicted participant after the user's
   partial edits are stashed away (pristine-conflict check passing again).
3. §7.3: the open-vs-archive divergence flow end to end (reproduce the
   `does not match the open record` refusal; confirm moving the rejected
   copy out lets `--abort`/`--continue` finish the archive).
4. §8.2: `--gc <id>` after hand-deleting (not retargeting) a backup ref —
   confirm the checked delete treats a missing ref as already-done and
   proceeds.
5. §8.2: `--gc <id>` with a missing member repository — capture the exact
   git-layer error text for the table in §4.
6. General: capture real rendered output of a blocked command, a W1
   `--status`, and a W5 discovery failure, and paste them into §1/§2.2/§5
   verbatim in place of the reconstructed excerpts.
