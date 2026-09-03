# GWZ v0.11.0 — release notes

Status: ASSEMBLED 2026-08-25 by the implementation lane from the R3
builder's reviewed draft (as corrected in its round 2 per the R3
review's [P2-3]/[P3-8]), the R1.2 C2 residual's user-facing form, and
the landed tree (gwz-core `07e1ac1`, gwz-cli `cf0d16d`+runbook).
FINAL pending two items: the R4 runbook review's GO (the guide it
points to) and the RC evidence pass. The operator publishes this as
the GitHub release body at the tag.

---

## What this release is

The first release cut from the development line at or after the A1
activation. It ships the production v1 merge-record writer and the
`--no-ff` merge start. Unlike v0.10.5 — deliberately based on the
narrow pre-v1 checkpoint, on a branch carrying no code this line
lacks — v0.11.0 is cut from the development line itself and strictly
supersedes it.

## New: `--no-ff`

`gwz merge <source> --no-ff` always creates a merge commit, even
where a fast-forward is possible. It is the counterpart to the
existing `--ff-only`; supplying both together is rejected. A
`--no-ff` start writes a **v1** coordinated merge record and
publishes a two-parent integration commit.

## Merge records: what this release writes and reads

- **Reads:** both v0 and v1 coordinated merge records, dispatched per
  record on the envelope header, so mixed stores decode correctly.
- **Writes v1:** `--no-ff` starts, end to end.
- **Writes v0:** ordinary and custom-message (`-m`) starts, unchanged.

**A v1 record can also appear without `--no-ff`.** An ordinary v0
record left in the `Finalizing`/`Normal` shape is whitelist-eligible
and migrates to v1 when you resume or abort it under this release. So
a workspace where nobody ever typed `--no-ff` can still end up
holding a v1 record — which matters if an older gwz then has to read
that store.

The v0 floor for ordinary starts is deliberate and named.
`ACTIVE_WRITER_FLOOR` ships at `V0`, and the record version is
`max(floor, semantic)`. Ordinary starts continue on v0 until the
production v1 ordinary-start owner lands — root participants, dry-run
prediction, drift/conflict surfaces, and the v0 event stream all need
v1 owners first. **Owner: milestone M5c**, which raises the floor
together with that work as one reviewed change. Not writing v1 for
ordinary starts cannot regress any reader; it is the conservative
side of the transition.

## What older gwz releases do with a v1 record

Three different behaviours, not one. Only the first two are covered
by the retained-reader harness; the rest is read from the released
sources and marked as such.

| Release | Meets a v1 record | Evidence |
| --- | --- | --- |
| **v0.9.2 and earlier** | No coordinated-merge-record surface at all — these builds have no durable-record dispatcher to refuse anything | harness-pinned |
| **v0.10.2** | Decodes v0; a v1 envelope is **unreadable** | harness-pinned |
| **v0.10.3** | Same shape as v0.10.2 — carries no strict-envelope module | source-derived, no retained reader |
| **v0.10.4, v0.10.5** | Decodes v0 and refuses v1 **cleanly and by name** — a typed unsupported-record-version carrying the required semantic wave, not an unreadable record | source-derived, no retained reader |

In short: v0.10.4 and v0.10.5 give the best diagnostic of any older
release. If you share a workspace with someone on an older gwz, avoid
`--no-ff` until they upgrade — and note that an
interrupted-and-resumed ordinary merge can produce a v1 record too.

## New: a pre-mutation refusal for foreign clean filters

Recovery-grade checkouts — the merge abort and branch-retarget paths
— write raw blob bytes. That is only status-clean where every clean
filter covering a rewritten path is idempotent on blob bytes. Builtin
filters are; encrypt-on-clean filters (the git-crypt class) are not.
Where the assumption failed, verification failed *after* the ref
transaction had committed, turning a previously working abort into a
wedge whose only remedy was an undocumented manual move of the
record.

v0.11.0 refuses first. Before any ref moves, gwz checks whether a
path the recovery checkout would rewrite is covered by an attribute
filter outside the allowlist **and** whose `filter.<name>.clean` or
`.process` is present in effective config. If so it fails, having
changed nothing on disk, with a message of this shape (reported under
the existing `DirtyMember` error code — machine consumers keying on
error codes see nothing new):

> recovery checkout would rewrite '<path>' through configured foreign
> filter '<name>' (filter.<name>.clean/process); refusing before any
> ref or worktree mutation

- Allowlisted: `lfs`.
- **If you hit this:** the operation was refused, not half-applied —
  worktree and refs are exactly as they were. See the merge recovery
  guide shipping with this release for the remedy steps.
- **Known cosmetic case, unchanged:** an LFS-managed path rewritten
  by a rollback carries pointer bytes on disk afterwards. Status is
  clean, and an ordinary git checkout restores content.

False positives are possible for configured-but-idempotent custom
filters. That is deliberate: those repositories sit one checkout away
from the wedge this refusal prevents.

**One narrow case survives the refusal, named and tracked.** The gate
reads the attribute stack as it stands *before* the checkout. If
`.gitattributes` itself is inside the rewrite set and the foreign
filter coverage exists only on the *target* side — the checkout
restores the coverage together with the bytes — the rollback or abort
proceeds and succeeds, and afterwards real `git` (not `gwz status`)
may show the covered paths as modified: they hold raw blob bytes
under a now-active clean filter. Reaching this requires rolling back
across a change that *deleted* the covering `.gitattributes`; the
common direction — rolling back a change that added coverage —
refuses safely. The remedy is to force re-materialization of the
affected paths — delete them, then check them out again (the recovery
guide gives the exact sequence). A bare `git checkout -- <paths>` is
**not** sufficient here: git's index considers the raw files current
and silently skips them, so the command reports success while
changing nothing. Closing the gap outright needs a libgit2 capability
our current dependency does not expose; it is tracked with the
planned `renormalize` work.

## New: line-ending pins at repository birth

gwz's raw-byte model compares worktree bytes against blob bytes.
Under line-ending filters (most commonly Windows with
`core.autocrlf=true`, but this is config-driven, not OS-driven),
ordinary text files are CRLF on disk and LF in the blob, so that
comparison cannot be satisfied: the reverse-lane observers classify
the repository `Ambiguous` and rollback never starts, or
re-verification fails on unchanged-but-smudged files. The failure
direction is closed — availability loss, never wrong evidence — but
it was invisible to CI and silent to users.

v0.11.0 pins the two keys that close it, **at creation time only**:

- gwz-created repositories get repo-local `core.autocrlf=false` and
  `core.eol=lf` at init.
- gwz-performed clones materialize with filters disabled and get the
  same two keys pinned immediately afterwards.

Repositories gwz creates from this release forward are blob-exact
from birth on every OS, and `stash` round-trips through them are
filter no-ops.

**Residual — adopted and legacy worktrees.** Repositories created
before this release, or created by something other than gwz, are not
touched. Pinning mid-life manufactures dirt in the worktree, so this
release deliberately does not do it. Those repositories keep the
availability limitation above wherever line-ending filters are
active.

Remedies today: set `core.autocrlf=false` and `core.eol=lf` in the
repository and re-checkout (the recovery guide gives the exact
sequence), or re-clone through gwz. The supported operator-invoked
remedy, `gwz repair --renormalize`, is a **planned follow-up and is
not in this release**; it is deliberately manual because it rewrites
worktree bytes.

One residual is permanent on every platform regardless:
attribute-driven `eol=crlf`, `ident`, and foreign filters are
fail-closed doctrine, not something creation-time pins can fix.

## Also in this release

- **The merge recovery guide** (`docs/MergeRecovery.md` in gwz-cli):
  every refusal and wedge class above, plus the parked-record
  quarantine procedure, documented with exact symptoms and remedy
  steps — the first shipped recovery documentation for the merge
  lifecycle.
- A CRLF lane in the Windows CI matrix, plus a sentinel that rides
  every test lane and fails loudly if the smudge class it guards ever
  silently closes.
- The release workflow now runs the R4b-G mechanical batteries —
  fault, byte-equivalence, unknown-field, privacy — under pinned
  counts on its Linux leg, and the CRLF count-pinned lane on its
  Windows leg.
- The retained-reader manifest registers this release's decode
  generation (and closes a register gap for v0.10.4/v0.10.5),
  describing the shipped behaviour above.

## Not in this release

- Ordinary and custom-message starts writing v1 — milestone M5c.
- `gwz repair --renormalize`.
- Any mid-life line-ending normalization of adopted repositories.
- Two documentation gates (the M4 scenario map and the merge-doc
  consistency checker) still run outside CI, pending a multi-repo
  checkout.
