# Lane disposal audit — 2026-09-10

Read-only investigation after normal disposal refused all three lanes. No lane
was deleted or detached; no force waiver, merge or ref mutation was performed.

## Findings

All member-repository HEADs in svcchecked, svcdrivers and svcmerge are reachable
from retained refs in the corresponding repositories of the main workspace.
No unmerged member HEAD was found. Ordinary workspace status is clean in all
three lanes; that does not include the ignored-file/stash disposal checks.

Exactly three inspected protected commit objects are absent from the main root
repository:

| Lane | Commit | Changes |
| --- | --- | --- |
| svcchecked | 4a789aa11dad9295c444634fc17cd89bdc82dc79 | Root lock update and generated merge marker |
| svcchecked | d559bbba6e728901f94cccae7e55c64704501ed1 | Generated merge marker |
| svcdrivers | fedcd861e62572151f41efe2a9607955ab276562 | Root lock/integrity update and generated commit marker for tag service composition |

The svcmerge root HEAD is already retained in main. The three missing objects
above change only gwz.conf metadata/markers, not member source code.

All ten stash entries per lane (root 4, CLI 4, core 2) have exact matching stash
commit IDs in the main workspace. All inspected unretained older member history
objects also exist in the corresponding main repository's reflogs: CLI 12,
core 30, Python 5. Three older root stash commits likewise exist in main.
These counts are unique protected object IDs per repository, not the number of
hazard lines (HEAD/ref/reflog can name the same object).

GWZ deliberately requires reachability from eligible persistent roots, rather
than mere presence in an object store or reflog. Thus these copied older entries
still trigger refusal: a surviving reflog/stash entry is not durable retention
under the current policy. See crates/history-check/src/lib.rs in gwz-core.

Ignored-file inspection excluded recognized build/cache directory categories
from byte comparison. Other inspected ignored settings/support files matched
main. The 34 distinct non-runtime paths missing from their old main locations
in each lane are historical test/performance evidence; all 34 have byte-identical
copies in the registered gwz-core-evidence repository at their new archive paths.
Lane-specific .gwz allocation/family/catalog/completed-merge runtime records differ
as expected. Cache/build contents were not certified equivalent or backed up.

Raw inventories and read-only scripts are in scratch/lane-disposal-audit/:
history.json, ignored.json, inspect.py, ignored.py. The ignored scanner's initial
raw counts include overlapping directory entries; the 34-file archive comparison
was deduplicated by path. The history comparison tests reachability/object presence;
it is not a replacement for GWZ's complete-graph preservation verifier.

Decision still required: retain the three unique metadata commit objects before
deletion or explicitly accept their loss; decide whether copied old reflog/stash
history needs durable refs/archive and whether ignored/cache/runtime data can be
discarded. Nothing in this audit grants or applies those waivers.

## Subsequent action

At the user’s request, all three lanes were detached using
`gwz local dispose NAME --keep`. All commands succeeded. Family listing now
contains only root; all three directories and root Git repositories remain.
No history or ignored-file loss waiver was applied.
