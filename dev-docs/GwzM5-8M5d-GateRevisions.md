# GwzM5-8 M5d — the two gate revisions: CapabilityFree §3 and the F-3 seam floor

Date: 2026-09-03. Author: the implementation lane (Fable lane, S1 builder).
Status: **DRAFT — not applied.** Both revisions land at the **M5d close**
(`GwzM5-8M5d-Charter.md` §7, §9 step 4), inside the single reviewed landing
with the floor raise, the production §2 refuse, and the v0-lifecycle erasure.
Nothing here edits `dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md` or
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py`. Drafting this
text revises neither gate. The companion S1 deliverable is
`GwzM5-8M5d-I2Amendment.md`.

**Controlling document:** `dev-docs/GwzM5-8M5d-Charter.md`, revision 5,
ACCEPTED by operator decision 2026-09-03. Binding here: §4 (the F-3 paragraph),
§7 rows `GwzM5-8R2E-CapabilityFreeAmendment.md §3` and
`F-3 V0_PERSISTENCE_SEAM_FLOOR`, and §9 step (1). The charter's own
acceptance-SHA note is in `GwzM5-8M5d-I2Amendment.md`'s status header and is
not repeated here.

## 1. Evidence base and method

| Object | Value |
|---|---|
| gwz-dev root HEAD | `8535553e0655627654c7b36fe14df21423a2020d` |
| gwz-core HEAD | `57502e461055809503ca9dc0d7cb791b8b332f6c` |
| Checker | `gwz-core/scripts/checks/check_checked_artifact_boundaries.py` |
| Checker tests | `gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py` |
| CapabilityFree amendment | `dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md`, 728 lines |
| Python | `/opt/homebrew/bin/python3.13` |

**Method.** Every count below was produced by importing the live checker module
and applying **its own** `mask_non_code` / `without_test_modules` /
`CAPABILITY_FREE_WRITER_TOKENS` to the live sources — not by grep. No cargo, no
test suite, no partition was run. The method is self-validating: it reproduces
the live key-set pin `867c580f625d7efe0cf72dcc8e0ad01e36268d1478829a469eb0f57953dbd385`
exactly, and it reproduces every one of the twenty inventory rows' pinned
per-token counts exactly. A reviewer can re-derive all of it by reading the
checker and the cited source lines.

**Note on `dev-docs/` paths.** A dev-docs reorganization is in flight in this
working tree at the drafting time: a large set of settled records is staged as
renames into `dev-docs/history/`. Citations below that carry a `history/`
segment reflect that staged location; the file contents are byte-identical
(pure renames). Files central to this step — the M5d charter, the I2
compatibility contract, `GwzM5-8R2E-CapabilityFreeAmendment.md`,
`GwzM5-8R2E-RecordRootAmendment.md`, `GwzM5-8R4bG-Evidence.md`,
`GwzM5-8M5bNoFfDesign.md`, `GwzM5-8R2DSettledTuple.md` and
`CurrentProgramCheckpoint.md` — are **not** moved.

---

# PART A — `GwzM5-8R2E-CapabilityFreeAmendment.md` §3 and the inventory

## A.1 What is authorised

Charter §7, verbatim:

> `GwzM5-8R2E-CapabilityFreeAmendment.md` §3 / `CAPABILITY_FREE_RAW_WRITER_INVENTORY` |
> **revise the carved set** when v0-engine files leave production (shrinkage is
> an amendment, not a checker edit). The v0-engine carved files (`finalize.rs`,
> v0 abort/preserve/archived writers — **not** `store/gc.rs` /
> `store/retention.rs`) may leave; one `entry.rs` raw primitive may enter (S1
> measurement, including adding a token if `write_atomic_verified` is not in
> `CAPABILITY_FREE_WRITER_TOKENS` today). That revision is in the S1 dual

The gate being revised: `check_checked_artifact_boundaries.py:522–528`
(`CAPABILITY_FREE_WRITER_TOKENS`, 13 tokens), `:544–566`
(`CAPABILITY_FREE_RAW_WRITER_INVENTORY`, 20 rows), `:566–573` (the key-set
SHA-256 guard whose message routes to the amendment: *"It is the amendment's,
not a checker edit: revise GwzM5-8R2E-CapabilityFreeAmendment.md §3"*), and the
per-row count loop at `:1562–1596`.

Companion pin, which moves with the set: the P-2 negative door scan at
`gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/capability_free_exception.rs`
— `PURE_CARVED_FILES` (18 entries, `:33–52`) and `MIXED_CARVED_ARMS` (2
entries, `:55–65`). Charter §7 does not name it; it is a strict consequence of
the row set and is treated as part of this revision (see A.6 item 4).

**`write_atomic_verified` is not in `CAPABILITY_FREE_WRITER_TOKENS` today** —
verified: the tuple at `:522–528` is `rename_noreplace`, `rename_durable`,
`sync_dir`, `create_dir_all`, `remove_file`, `write_atomic`, `write_marker`,
`write_lock`, `write_manifest_and_lock`, `write_bundle`,
`publish_workspace_exclude_candidate`, `sync_workspace_boundary`,
`ensure_workspace_exclude`.

## A.2 Row-by-row measurement — which merge files leave, and what remains

Seven of the twenty rows name a file under `src/workspace_ops/merge/`. Each is
measured below: whether the file leaves production entirely or survives with
its v0 half deleted, and — for survivors — whether the **surviving v1 code
still names any `CAPABILITY_FREE_WRITER_TOKENS` identifier**. That last
question is what decides delete-the-row versus re-count-the-row, and it is
answered with call sites, not assertion.

### A.2.1 `workspace_ops/merge/finalize.rs` — WHOLE-FILE DELETION

Pinned counts `{"write_atomic": 2, "publish_workspace_exclude_candidate": 2}`;
measured identical at `finalize.rs:246`, `:249` (`write_atomic`) and `:9`
(`use` line), `:252` (`publish_workspace_exclude_candidate`). 398 lines.

Reachability, measured — every production caller of `finalize::` is v0
lifecycle: `merge/start.rs:140`, `merge/continue_op/coordinator.rs:21,:123`,
`merge/finalize_dispatch.rs:9`, `merge/publication.rs:172`, and the v0 adapter
at `merge/record_wire/open_v0/descriptor.rs:272` and
`open_v0/structural.rs:210`. All of those leave with the engine. Declared at
`merge/mod.rs:4` (`mod finalize;`) and re-exported at `:43` under `#[cfg(test)]`.

**Disposition: row DELETED.**

### A.2.2 `workspace_ops/merge/store/archived.rs` — WHOLE-FILE DELETION

Pinned counts `{"rename_durable": 1, "sync_dir": 2, "create_dir_all": 1,
"remove_file": 1}`; measured identical at `:54`, `:56`, `:57`, `:44`, `:52`.
65 lines. Its two functions are `load` (`:14`, already carrying a
`dead_code` allowance and marked "MEASURED REDUNDANT" by the E4.7 sweep) and
`archive` (`:27`), reached only through `store::persistence::archive_merge_record`
(`store/persistence.rs:32`) from v0 finalization (`finalize_support.rs:99`,
`finalize_dispatch.rs:34`) and both v0 abort forms (`abort/mod.rs:111,:189,:218`).

Note the division of labour that makes this safe: `archived.rs` is the v0
terminal **archive writer** (moving `open/` → `done/`). The I2 §7 archive
**decoder** and the GC that reads `done/` bytes are elsewhere and are retained.
Post-close no 0.14 binary ever writes a v0 archive, because no 0.14 binary ever
completes or aborts a v0 merge.

**Disposition: row DELETED.**

### A.2.3 `workspace_ops/merge/abort/evidence.rs` — MIXED, v1 half survives

789 lines. `mod v1_rollback { … }` occupies `:24–522`; the v0 arm
`pub(super) fn rollback_evidence<A: AbortRuntime, S: MergeStore>` occupies
`:668–720`.

Pinned counts `{"remove_file": 1, "write_atomic": 1,
"publish_workspace_exclude_candidate": 1}`. Measured token positions:

| Token | Line | Source |
|---|---|---|
| `publish_workspace_exclude_candidate` | `:687` | `super::super::super::publish_workspace_exclude_candidate(` |
| `write_atomic` | `:693` | `artifact::write_atomic(` |
| `remove_file` | `:700` | `match fs::remove_file(&marker_path) {` |

**All three sit inside `rollback_evidence` (`:668–720`) — the v0 arm.** Zero
tokens fall inside `mod v1_rollback` (`:24–522`) or in `semantic_mismatch`
(`:524–532`), `preflight_evidence` (`:553–662`), or `verify_evidence_baseline`
(`:762–789`).

The v1 region has live consumers that survive the close, so the file survives:
`v1_lifecycle/authority/observe/reverse/rollback.rs:311` and
`v1_lifecycle/authority/observe/reverse/preservation/entry.rs:87` call
`preflight_v1_evidence`; `execute_v1_evidence_rollback` /
`observe_v1_evidence_rollback` are re-exported through `abort/mod.rs:13`.
`rollback_evidence` itself has exactly one production caller,
`abort/mod.rs:176`, inside the v0 abort walk that leaves.

**Answer to the deciding question: NO. After the v0 arm leaves, the surviving
v1 code in this file names ZERO `CAPABILITY_FREE_WRITER_TOKENS` identifiers.
Disposition: row DELETED, not re-counted.**

### A.2.4 `workspace_ops/merge/abort/preflight.rs` — MIXED, v1 half survives

415 lines. `mod v1_rollback { … }` occupies `:26–272`; the v0 arm
`pub(super) fn restore_baseline(root: &Path, record: &MergeOperationRecord)`
occupies `:350–415`.

Pinned counts `{"write_atomic": 2}`. Measured: `:413` and `:414`, both inside
`restore_baseline`. Zero tokens inside `mod v1_rollback`, `preflight`
(`:282–326`) or `verify_baseline` (`:328–344`).

`preflight_v1_rollback` is re-exported at `abort/mod.rs:21` and consumed by
`v1_lifecycle/tests/reverse_rollback/entry.rs:32,:118` and the v1 observe
tree; the file survives. `restore_baseline`'s only production caller is
`abort/mod.rs:180`, in the v0 walk.

**Answer: NO surviving-v1 token. Disposition: row DELETED.**

### A.2.5 `workspace_ops/merge/preserve/artifacts.rs` — MIXED, v1 half survives

718 lines. Contrary to the P-2 test's classification of this file as **PURE**
(`capability_free_exception.rs:44`), it is **mixed by v0/v1 arm** — the P-2
list is about *door* references, not about v0/v1, so both statements are true
at once and neither is wrong. Verified structure:

- v0 arms holding every token: `prepare_root_for_stash` (`:160–181`),
  `restore_root_publication` (`:187–254`), `persist_stash_bundle` (`:296–358`).
- v1 arms, no tokens: `v1_root_preservation_spec` (`:488–589`), `clean_form`
  (`:592–622`), `managed_fact` (`:625–645`), `blob_oid` (`:648–661`),
  `v1_preservation_image` (`:679–697`), `attach_v1` (`:700–709`), `v1_error`
  (`:712–718`).

Pinned counts `{"remove_file": 1, "write_atomic": 3, "write_bundle": 1,
"publish_workspace_exclude_candidate": 1}`. Measured token positions:

| Token | Line | Enclosing arm |
|---|---|---|
| `write_atomic` | `:174` | `prepare_root_for_stash` (v0) |
| `write_atomic` | `:206` | `restore_root_publication` (v0) |
| `remove_file` | `:215` | `restore_root_publication` (v0) |
| `write_atomic` | `:230` | `restore_root_publication` (v0) |
| `publish_workspace_exclude_candidate` | `:237` | `restore_root_publication` (v0) |
| `write_bundle` | `:357` | `persist_stash_bundle` (v0) |

All six callers are `merge/preserve.rs` (`:127`, `:130`, `:145`, `:182`) — the
v0 preserve orchestrator. The v1 consumers of the surviving half are
`v1_lifecycle/authority/observe/reverse/preservation/cursor.rs:305,:332,:497`
and `.../preservation/phase.rs:108`.

**Answer: NO surviving-v1 token. Disposition: row DELETED.**

### A.2.6 `workspace_ops/merge/store/gc.rs` — STAYS WHOLE

22 lines. Pinned `{"sync_dir": 1, "remove_file": 1}`; measured `:21` and `:20`.
`gc::collect` is the live GC deletion writer under `WorkspaceMutatorLock`, is
I2 §7's GC, and is explicitly excluded by charter §7. Its `:19` comment already
carries the CAPABILITY-FREE EXCEPTION marker naming the amendment.

**Disposition: row UNCHANGED.** One close-time caution: `gc.rs:1` is
`use super::*`, so it inherits `sync_dir`, `DONE_DIR`, `done_path`,
`path_exists`, `io_error`, `validate_merge_id` and `read_seam_record` from
`store/mod.rs`. Those symbols must survive the v0-store carve or `gc.rs`'s
count moves and the row fires (see A.6 item 5).

### A.2.7 `workspace_ops/merge/store/retention.rs` — STAYS WHOLE

78 lines. Pinned `{"sync_dir": 1, "remove_file": 1}`; measured `:54` and `:50`.
Same GC lock, same charter exclusion. **Row UNCHANGED.**

### A.2.8 The thirteen non-merge rows — UNCHANGED

`stash/mod.rs`, `workspace_ops/handle_{branch,commit,create_repo,
init_from_sources,materialize,repo_lifecycle,stage}.rs`,
`workspace_ops/handle_stash/commands.rs`,
`workspace_ops/merge/v1_lifecycle/archive.rs`,
`workspace_ops/merge/v1_lifecycle/store/archive.rs`,
`workspace_ops/pull_head_member_preflight.rs`,
`workspace_ops/sync_workspace_boundary.rs`. None is a v0-engine file; charter
§7 authorises no change to any of them. `handle_stage.rs` is the one to watch:
charter §4 changes what `gwz stage` **discovers**, and its pinned
`{"ensure_workspace_exclude": 1}` must be re-measured at close, not assumed.

## A.3 Correction to the sizing document: five rows leave, not seven

`GwzM5-8M5d-Review.md` §3 F3 states "Deleting the engine removes seven v0
rows", and §4's step table repeats "7 rows out, 1 in". **Measured: five.** The
sizing document counted all seven `workspace_ops/merge/…` rows outside
`v1_lifecycle/`; charter §7 revision 5 excludes two of them by name — *"not
`store/gc.rs` / `store/retention.rs`"* — because they are I2 §7 GC, which
charter §7's last row retains and does not amend.

The charter is controlling; the sizing document is an input. The correct count
is **five out, one in, net −4, 20 → 16 rows**. Recorded here rather than by
editing the sizing document, which is a dated record.

## A.4 The relocated raw primitive — where its inventory row sits

### A.4.1 What is being relocated

`write_atomic_verified` is at
`gwz-core/src/workspace_ops/merge/store/mod.rs:463–498`, a private `fn` inside
`merge::store`, with exactly two references in the tree:
`store/mod.rs:146` (the sole production caller, inside
`impl MergeStore for FileMergeStore`'s open-record write) and
`store/tests.rs:416`. Its body names four `CAPABILITY_FREE_WRITER_TOKENS`
identifiers: `create_dir_all` (`:467`), `remove_file` (`:485`, `:489`),
`rename_durable` (`:488`), `sync_dir` (`:492`) — plus the module's `use` line
at `:10`, `use crate::durable_fs::{rename_durable, sync_dir};`.

**`workspace_ops/merge/store/mod.rs` is NOT an inventory row today.** That is
the load-bearing fact for this subsection: the primitive's four raw calls are
currently **unpinned by this inventory** (the record-root writer is RR's carve
— `check_checked_artifact_boundaries.py:463` names
`GwzM5-8R2E-RecordRootAmendment.md §2/§3` for `v1_lifecycle/store/rewrite.rs`,
and the checker's `:503–504` comment says so explicitly).

Post-close its only production caller is the raw-create arm of
`checked_artifact/entry.rs::create_merge_store_record` (`entry.rs:541–560`),
landed at charter §9 step (3). Measured today, `entry.rs` names **zero**
`CAPABILITY_FREE_WRITER_TOKENS` identifiers under masking: its only occurrences
of `create_dir_all` are at `:172` and `:491`, both inside doc comments, which
`mask_non_code` blanks.

### A.4.2 The scanner constraint

The per-row loop at `:1562–1596` reads the row's own file and counts
`CAPABILITY_FREE_WRITER_TOKENS` identifiers **lexically present in that file**.
So a row can only sit where the tokens are. Three shapes are coherent:

| Shape | Row(s) | Token added? | What it pins | What it leaves unpinned |
|---|---|---|---|---|
| **(A)** | the neutral module only | no | the four primitives inside the relocated function | that `entry.rs` still calls it raw — a conversion of the arm to the checked door would pass this gate |
| **(B)** | `checked_artifact/entry.rs` only | **yes**, `write_atomic_verified` | that `entry.rs` names the raw primitive exactly N times, fail-closed both ways | the neutral module's four internal primitives — exactly as `store/mod.rs`'s are unpinned today |
| **(C)** | both | yes | both | nothing new |

**Recommendation: (B).** Three reasons, and they are about what the amendment
is *for*.

1. The property the CapabilityFree amendment exists to defend is *"this
   capability-free operation keeps its raw publication primitive; converting it
   would put the operation on the durable-identity probe."* The operation is
   an ordinary merge start on a handle-fail volume, and the arm that decides
   raw-versus-checked is in `entry.rs`, not inside the primitive. (B) pins the
   decision; (A) pins only the implementation of the thing decided upon.
2. (B) is exactly what charter §7 authorises — *"one `entry.rs` raw primitive
   may enter … including adding a token if `write_atomic_verified` is not in
   `CAPABILITY_FREE_WRITER_TOKENS` today"*. (C) adds a second row the charter
   does not authorise.
3. (B) is a strict improvement over today and introduces no regression: the
   neutral module's four internal primitives are unpinned by this inventory
   **before** the change too, because `merge/store/mod.rs` is not a row. (B)
   moves the pin from nothing to something.

### A.4.3 The exact token-tuple edit

Insert into `CAPABILITY_FREE_WRITER_TOKENS` at `:522–528`, keeping the
existing class-comment layout:

```python
CAPABILITY_FREE_WRITER_TOKENS = (
    "rename_noreplace", "rename_durable", "sync_dir",  # `durable_fs`
    "create_dir_all", "remove_file",  # `std::fs`-direct
    "write_atomic", "write_marker", "write_lock", "write_manifest_and_lock",  # the
    "write_bundle", "publish_workspace_exclude_candidate",  # `write_atomic` family
    "sync_workspace_boundary", "ensure_workspace_exclude",
    # M5d: the relocated verified record writer. It is the raw publication
    # primitive of the ordinary merge start's record create on a handle-fail
    # volume (GwzM5-8M5d-Charter.md §3), so the arm that names it is carved by
    # the same ground as the rest of this vocabulary. Added because the arm's
    # own file names NO other token: `entry.rs` reaches the primitive through
    # this one name and through nothing else.
    "write_atomic_verified",
)
```

The addition is safe against the existing nineteen unchanged rows: measured,
`write_atomic_verified` appears in exactly two files in the whole tree
(`merge/store/mod.rs`, `merge/store/tests.rs`), neither of which is an
inventory row, so no existing row's count moves.

### A.4.4 The neutral module's own name

Charter §4 requires the primitive to move to "a neutral module". A.4 does not
choose the path — that is charter §9 step (3)'s landing. But Part B's floor
depends on the choice, so S1 fixes one **constraint** on it:

> The neutral module's own leaf identifier must not be an identifier that
> `v1_lifecycle/` already names.

This rules out `crate::durable_fs` as the module: `durable_fs` is named inside
`v1_lifecycle/` today by all three O13 rows
(`check_checked_artifact_boundaries.py:446–450`: `v1_lifecycle/archive.rs`,
`v1_lifecycle/store/archive.rs`, `v1_lifecycle/store/rewrite.rs`), and the F-3
replacement property (Part B) asserts `v1_lifecycle/` names the floor nowhere.
A new dedicated module — e.g. `gwz-core/src/verified_write.rs` exporting
`pub(crate) fn write_atomic_verified` — satisfies it. The floor set is then
`{"verified_write", "write_atomic_verified"}` and neither identifier appears
under `v1_lifecycle/` today. Recorded as OPEN QUESTION Q10.

## A.5 The post-close key set and its provisional pin

Five rows out (A.2.1–A.2.5), one row in (A.4), thirteen plus two unchanged.
**Sixteen keys, sorted, exactly as the pin hashes them:**

```text
checked_artifact/entry.rs
stash/mod.rs
workspace_ops/handle_branch.rs
workspace_ops/handle_commit.rs
workspace_ops/handle_create_repo.rs
workspace_ops/handle_init_from_sources.rs
workspace_ops/handle_materialize.rs
workspace_ops/handle_repo_lifecycle.rs
workspace_ops/handle_stage.rs
workspace_ops/handle_stash/commands.rs
workspace_ops/merge/store/gc.rs
workspace_ops/merge/store/retention.rs
workspace_ops/merge/v1_lifecycle/archive.rs
workspace_ops/merge/v1_lifecycle/store/archive.rs
workspace_ops/pull_head_member_preflight.rs
workspace_ops/sync_workspace_boundary.rs
```

SHA-256 over `"\n".join(sorted(keys))`, computed with `python3.13`:

```text
8fb45d6f48f65c928cd6a0d7130c637ea59965f360755a526de7fc7a74567131
```

**This pin is PROVISIONAL.** It holds if and only if the key set lands exactly
as listed. The close **recomputes it from the real tree** and substitutes the
measured value; it does not copy this one on trust. Two alternates are recorded
so a reviewer can tell instantly which shape a close actually took:

| Shape | Keys | SHA-256 |
|---|---|---|
| **(B), recommended** — `checked_artifact/entry.rs` enters | 16 | `8fb45d6f48f65c928cd6a0d7130c637ea59965f360755a526de7fc7a74567131` |
| (A) — a neutral module `verified_write.rs` enters instead | 16 | `f6349be668920aa779d24ead67c4e68cb0f941861f50aa142d9214be2b2a6fd0` |
| nothing enters | 15 | `3db5d529b0a9afb0a1744d8535128108ad2623305a3a0dede8e05b227f2ff8b3` |

Any other value means the set is not what this document measured, and the
divergence must be explained before the close lands. Per-token **counts** are
deliberately not pinned here for the entering row: `entry.rs`'s
`write_atomic_verified` count is 1 or 2 depending on whether step (3) calls it
fully qualified or through a `use` (the inventory counts pin references, `use`
lines included — checker comment `:494–495`). The close measures it.

## A.6 Consequential edits that ride with the set

1. **The guard's expected digest**, `:568`: `867c580f…` → the close-measured
   value.
2. **The five deleted rows**, `:554–558` and `:560`, removed from the dict
   literal at `:544–566`.
3. **The entering row**, added in sorted position (first, since
   `checked_artifact/` sorts before `stash/`):

```python
    "checked_artifact/entry.rs": (":275 the ordinary merge start's RAW record create on a handle-fail volume", {"write_atomic_verified": 1}),
```

4. **The P-2 negative door scan**,
   `src/workspace_ops/merge/v1_lifecycle/tests/capability_free_exception.rs`:
   `PURE_CARVED_FILES` (`:33–52`) drops `workspace_ops/merge/finalize.rs`,
   `workspace_ops/merge/preserve/artifacts.rs`,
   `workspace_ops/merge/store/archived.rs` and gains
   `checked_artifact/entry.rs` — **except** that `entry.rs` is the checked
   boundary's own module and certainly names the door vocabulary, so it can
   **not** join `PURE_CARVED_FILES`; it is a MIXED file whose carved arm is the
   raw-create arm. `MIXED_CARVED_ARMS` (`:55–65`) drops both existing entries
   (`rollback_evidence`, `restore_baseline` — their files leave the set) and
   gains one signature naming the raw arm inside
   `create_merge_store_record`. The list sizes in the doc comment (`:31`,
   "Eighteen files are PURE", "Two are MIXED") and the `[&str; 18]` array
   length move with it. Also note `:110–117`: the test's positive control for
   the `GIT_CHECKED_SEAM` exclusion reads `workspace_ops/merge/finalize.rs`,
   which is **deleted** by this close — that assertion must be re-pointed to
   another file naming `commit_gwz_paths_checked` (measured: `execute.rs:31`)
   or the test panics on a missing path.
5. **`store/mod.rs`'s import line** `:10`: after `write_atomic_verified` and
   `archived.rs` both leave, `rename_durable` has no remaining user in that
   module and the import narrows to `use crate::durable_fs::sync_dir;` (needed
   by `gc.rs` / `retention.rs` through `use super::*`). Neither row's count
   changes — both name `sync_dir` in their own file — but a careless narrowing
   that drops `sync_dir` breaks both.

## A.7 The §3 amendment prose

To be inserted into `dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md` §3, after
the "*[Corrected 2026-09-02, post-adoption…]*" bracket and before the "Per
carved file a negative scan in E4.3-B's P-2 idiom" sentence, in that section's
existing dated-bracket voice:

```text
*[Revised at the M5d close, 2026-09-03, under `GwzM5-8M5d-Charter.md` §7 (the
`GwzM5-8R2E-CapabilityFreeAmendment.md` §3 row) and drafted at S1 in
`GwzM5-8M5d-GateRevisions.md` Part A. The set SHRINKS by five rows and GROWS by
one; the count is 20 → 16 and the key-set digest moves with it.

WHAT LEAVES, and why it is a carve-out and not a conversion. M5d deletes the v0
merge lifecycle engine from production: 0.14 has no v0 start, continue, abort,
migrate or finalize, and an open v0 envelope is refused with one sentence
naming release `v0.13.0`. Five carved rows are that engine's writers and leave
with it — `merge/finalize.rs` and `merge/store/archived.rs` as whole files, and
`merge/abort/evidence.rs`, `merge/abort/preflight.rs` and
`merge/preserve/artifacts.rs` as the v0 arms of files whose v1 halves stay.
MEASURED at gwz-core `57502e4`: in each of those three mixed files every
counted token sits inside the departing v0 arm — evidence.rs `:687`/`:693`/`:700`
in `rollback_evidence` (`:668-720`), preflight.rs `:413`/`:414` in
`restore_baseline` (`:350-415`), artifacts.rs `:174`/`:206`/`:215`/`:230`/`:237`/`:357`
in `prepare_root_for_stash`, `restore_root_publication` and
`persist_stash_bundle` — and NONE inside the surviving `mod v1_rollback` /
`v1_*` regions. So each of the three drops to zero counted tokens and its row
is DELETED rather than re-counted.

This shrinkage is NOT the shrinkage §3 forbids. The clause "shrinkage fires (a
conversion of a carved arm may not land without revising this amendment)"
forbids CONVERTING a carved writer to the checked door, which would place a
capability-free operation on the durable-identity probe that E0.2 §5.2's list
keeps it off. Nothing here converts anything: the arms are DELETED, together
with the operations that reached them, and no capability-free operation is
moved onto the probe. The rule that a shrink is decided in this amendment and
not by a checker commit is honoured exactly — this is that decision, dated and
routed as the checker's own failure message directs.

`merge/store/gc.rs` and `merge/store/retention.rs` do NOT leave. They are the
live GC deletion writer and its retention enforcement under
`WorkspaceMutatorLock`, they are `GwzM5-8I2CompatibilityContract.md` §7, which
M5d retains and does not amend, and charter §7 excludes them by name. Their
rows and counts are unchanged. (`GwzM5-8M5d-Review.md`'s "seven v0 rows" is
superseded by the charter on this point: five.)

WHAT ENTERS. One row, `checked_artifact/entry.rs`, for the ordinary merge
start's RAW record create on a handle-fail volume (charter §3: below the
handle bar the record is written raw through `write_atomic_verified`, and there
is no second warning for the raw write). The primitive is relocated out of
`merge::store` — which is deleted — into a neutral module, and `entry.rs` is
its only production caller. Because `entry.rs` names no other primitive in this
vocabulary, `write_atomic_verified` is ADDED to
`CAPABILITY_FREE_WRITER_TOKENS`, which charter §7 authorises on this S1
measurement. The row is fail-closed in both directions on the same ground as
every other: a conversion of that arm to the checked door SHRINKS it and must
be re-decided here, because merge start is on the capability-free list; a
second raw writer in that file GROWS it and is not blessed.

The row's arm was previously unpinned by this inventory in either home:
`merge/store/mod.rs`, where `write_atomic_verified` lives today, is not and
never was a row of this map. The revision therefore adds pin coverage and
removes none.]*
```

---

# PART B — the F-3 seam floor, redefined, and the J-1 ruling

## B.1 What is authorised

Charter §7, verbatim:

> F-3 `V0_PERSISTENCE_SEAM_FLOOR` | **redefine** for a tree whose production v0
> persistence re-exports are gone: floor = the neutral raw primitive's module;
> only `entry.rs` calls it. No test-only or hollow seam kept for derivation
> (§4). Relocating `write_atomic_verified` is not sufficient; the J-1 ruling
> that the checker comment requires is recorded in the S1 dual

Charter §4, verbatim (the F-3 paragraph):

> F-3 / call-graph: `v1_lifecycle/` must not name the v0 store seam. Relocate
> `write_atomic_verified` to a neutral module; **only** `entry.rs` names the
> relocated primitive among production callers of raw create. Relocating that
> primitive is **not** the whole F-3 story: when production v0 persistence
> re-exports (`persist_merge_record`, `FileMergeStore`, …) leave, F-3's seam
> floor is **redefined** for that tree: the floor becomes the neutral raw
> primitive's module, and the property stays the same masked-identifier scan —
> `v1_lifecycle/` names that module nowhere; only `entry.rs` calls it. No
> hollow re-export is kept in production so a regex can derive a floor
> (revision 4, L-P3-2: that would be the ghost seam §5 forbids for suites).

## B.2 The two properties, side by side

### B.2.1 What the current scan enforces

Constants: `check_checked_artifact_boundaries.py:376` `V1_LIFECYCLE_TREE`,
`:377` `V0_STORE_REEXPORT`, `:378–387` `V0_PERSISTENCE_SEAM_FLOOR` (six names).
Derivation: `:1378–1390` `v0_persistence_seam`. Check site: `:1487–1506`.

Mechanically, in order:

1. **Derive** the seam by regexing `use store::{…};` out of masked
   `src/workspace_ops/merge/mod.rs`. Measured today: **13 names** —
   `AdaptationPrecheck`, `AtomicUpgradeFault`, `AtomicUpgradeOutcome`,
   `FileMergeStore`, `MergeStore`, `OpenRecordEnvelope`, `archive_merge_record`,
   `classify_open_record`, `discover_open_envelope_before_manifest`,
   `enter_finalizing`, `persist_merge_record`, `persist_operation_transition`,
   `upgrade_open_v0` (from `merge/mod.rs:56` and `:86–90`).
2. **Floor check** (`:1488–1493`): `V0_PERSISTENCE_SEAM_FLOOR - seam` must be
   empty. Measured today: empty. This is the "fails the derivation closed if
   that re-export shape is restructured away" half.
3. **Zero-hit scan** (`:1494–1506`): for every `*.rs` under
   `workspace_ops/merge/v1_lifecycle` reached by `rglob` — **131 files today,
   test files included**, because `rglob` is used rather than
   `production_rust_files` — assert that masked source contains no
   bare-identifier occurrence of any name in `seam | FLOOR` (19 distinct names
   today), excluding definitions (`text[start-3:start] != "fn "`).

The property, stated: *no code under `v1_lifecycle/`, production or test, names
any item that `merge/mod.rs` re-exports out of the v0 record store, nor any of
six floor names, and the re-export shape from which that list is derived cannot
be dismantled without the checker saying so.*

What it does **not** catch: a call into the v0 store made by a path that is not
re-exported through `merge/mod.rs`'s `use store::{…}` block (a
fully-qualified `crate::workspace_ops::merge::store::…` path is not in the
derived set and is not in the floor); anything outside `v1_lifecycle/`; and
aliasing (`use … as X`, then `X(…)` — the regex takes the first whitespace
token of each item, so `foo as bar` derives `foo`, and the alias `bar` is
uncovered).

### B.2.2 What the replacement enforces

Property, stated: *no code under `v1_lifecycle/`, production or test, names the
neutral raw primitive or its module; and among all production files in the
crate, exactly one — `checked_artifact/entry.rs` — names the primitive, exactly
N times.*

What it does **not** catch: anything about the v0 store, which no longer
exists as a production lifecycle; a `v1_lifecycle/` file naming a **surviving**
non-persistence store item (`classify_open_record`, `OpenRecordEnvelope`,
`discover_open_envelope_before_manifest`); a new store re-export added after
the close. See B.4, which does not paper over this.

### B.2.3 Why the current scan cannot simply stand

Not a matter of taste — measured. Four of the six floor names have **no
production caller outside the v0 lifecycle engine**, so they leave with it:

| Floor name | Production callers today | Post-close |
|---|---|---|
| `persist_merge_record` | `finalize.rs:223,:268,:307`, `finalize_support.rs:43,:87,:114,:130`, `finalize_dispatch.rs:202,:239`, `start.rs:119`, `abort/mod.rs:151`, `abort/evidence.rs:719` | **gone** — every caller is the v0 engine |
| `persist_operation_transition` | `finalize_support.rs:98`, `start.rs:147`, `preserve.rs:100`, `abort/mod.rs:158,:167,:182`, `continue_op/coordinator.rs:44,:98,:130` | **gone** |
| `enter_finalizing` | `start.rs:138`, `continue_op/coordinator.rs:121` (the two `v1_lifecycle/` hits at `transition/reduce/mod.rs:90` and `authority/observe/finalization.rs:99` are **string literals**, masked to nothing) | **gone** |
| `archive_merge_record` | `finalize_support.rs:99`, `finalize_dispatch.rs:34`, `abort/mod.rs:111,:189,:218` | **gone** |
| `MergeStore` | trait; used by `runtime/dispatch.rs`, `runtime/open_gate.rs`, `runtime/mutation_guard.rs`, `recovery.rs`, `handle_stage.rs` | **UNCERTAIN** — those sites survive as envelope discovery |
| `FileMergeStore` | `handle_stage.rs:34`, `open_gate.rs:20,:36`, `dispatch.rs:207`, `mutation_guard.rs:84,:94`, `recovery.rs:80,:112`, `store/atomic_upgrade.rs:65` | **UNCERTAIN** — same |

So at the close `V0_PERSISTENCE_SEAM_FLOOR - seam` becomes non-empty for at
least four names and the checker fires *"v0 persistence seam inventory is
underivable: workspace_ops/merge/mod.rs no longer re-exports [...]"*
(`:1490–1493`). **The current scan fails at the close by construction.** That
is the mechanical proof of charter §4's "Deleting `persist_merge_record` /
`finalize.rs` is paired with that checker change (§7)": the pairing is not a
preference, it is the only way the close is green.

**Honest qualification of the charter's premise.** Charter §4 writes "when
production v0 persistence re-exports (`persist_merge_record`, `FileMergeStore`,
…) leave". Measured, `FileMergeStore` and `MergeStore` are **not** purely v0
lifecycle: they are the store abstraction that `gwz stage`, the merge-verb
dispatch, the open-merge gate and the mutation guard use to *discover* an open
record — the exact sites charter §2 and §10.3 keep, in envelope-only form.
Whether those two names leave the re-export list is an implementation choice at
step (4), not a fact this step can assert. Four names leave with certainty; two
may survive in a non-persisting role. Recorded as OPEN QUESTION Q11.

## B.3 The concrete checker change

Constants at `:376–387` become:

```python
V1_LIFECYCLE_TREE = "workspace_ops/merge/v1_lifecycle"

# --- M5d: the neutral raw-primitive floor (replaces the derived v0 seam) ----
#
# The subject of the old scan is DELETED. M5d removes the v0 merge lifecycle
# engine from production, so `merge/mod.rs` no longer re-exports
# `persist_merge_record`, `persist_operation_transition`, `enter_finalizing` or
# `archive_merge_record`, and a derivation over that block can no longer stand.
# Per GwzM5-8M5d-Charter.md §4 no hollow or test-only re-export is kept in
# production merely so a regex has something to derive from -- that is the
# ghost seam §5 forbids.
#
# STILL LOAD-BEARING FOR JUDGMENT CALL J-1, by succession. The J-1 ruling of
# 2026-09-03 (dev-docs/GwzM5-8M5d-GateRevisions.md Part B, §B.5) retires the
# derived scan because its subject is gone and names THIS property its
# successor. Removing or weakening THIS scan re-opens J-1 exactly as removing
# its predecessor would, and must not be done without the R4b-G lane owner's
# ruling.
#
# The property, in two halves:
#   (1) no file under `v1_lifecycle/` -- production or test -- names the
#       neutral raw primitive or the module it lives in; and
#   (2) among production files crate-wide, `checked_artifact/entry.rs` is the
#       ONLY one that names the primitive, and it names it an EXACT number of
#       times, so a conversion of the raw-create arm to the checked door fails
#       closed here rather than passing silently.
#
# Bare-identifier counting on MASKED source is what keeps this exact, for the
# same reason it did before: `mask_non_code` blanks string and comment
# contents, so an action-name literal or a doc comment naming the primitive is
# not a call. (Measured at the drafting tuple: `entry.rs` names
# `create_dir_all` twice, at :172 and :491, BOTH inside doc comments -- a naive
# grep reads two raw writers where masked source reads zero.) `fn <name>`
# definitions are excluded the same way, so the primitive's own definition in
# the neutral module is not counted as a call to it.
NEUTRAL_RAW_WRITE_FLOOR = frozenset({"verified_write", "write_atomic_verified"})
NEUTRAL_RAW_WRITE_MODULE = "verified_write.rs"
NEUTRAL_RAW_WRITE_CALLERS = {"checked_artifact/entry.rs": 1}
```

Check site, replacing `:1487–1506`:

```python
    # (0) anti-vacuity: the floor must still exist, or every absence below is
    # an artefact of a moved file rather than a property of the tree.
    if not (source / NEUTRAL_RAW_WRITE_MODULE).is_file():
        findings.append(
            "the neutral raw-write primitive's module is GONE: "
            f"{NEUTRAL_RAW_WRITE_MODULE}. GwzM5-8M5d-Charter.md §4 puts the merge "
            "record's raw create there; relocating it again moves this floor"
        )
    # (1) `v1_lifecycle/` names the floor nowhere -- test files included, as
    # the derived scan did (rglob, not production_rust_files).
    for path in sorted((source / V1_LIFECYCLE_TREE).rglob("*.rs")):
        relative = path.relative_to(source).as_posix()
        text = mask_non_code(path.read_text(encoding="utf-8"))
        for token in sorted(NEUTRAL_RAW_WRITE_FLOOR):
            if any(
                text[max(0, match.start() - 3) : match.start()] != "fn "
                for match in re.finditer(r"\b" + re.escape(token) + r"\b", text)
            ):
                findings.append(
                    "v1 lifecycle names the neutral raw write primitive: "
                    f"{relative} ({token})"
                )
    # (2) only `entry.rs` calls it, an exact number of times.
    for path in production_rust_files(source):
        relative = path.relative_to(source).as_posix()
        if relative == NEUTRAL_RAW_WRITE_MODULE:
            continue
        text = mask_non_code(path.read_text(encoding="utf-8"))
        actual = len(
            [
                match
                for match in re.finditer(r"\bwrite_atomic_verified\b", text)
                if text[max(0, match.start() - 3) : match.start()] != "fn "
            ]
        )
        expected = NEUTRAL_RAW_WRITE_CALLERS.get(relative, 0)
        if actual != expected:
            findings.append(
                "raw record-write caller outside the single permitted door: "
                f"{relative} (expected={expected} actual={actual}). Only "
                "checked_artifact/entry.rs may call it (GwzM5-8M5d-Charter.md §4); a "
                "count that SHRANK is a conversion of the carved raw-create arm and "
                "belongs in GwzM5-8R2E-CapabilityFreeAmendment.md §3, not in a commit"
            )
```

**Why the masked-source bare-identifier technique still matters.** It is not
inherited habit; it is load-bearing twice over in the new property, and one of
the two cases is measured in today's tree.

1. **Doc comments.** `entry.rs` names `create_dir_all` at `:172` and `:491`,
   both inside `///` doc comments explaining that the writer must *not* use it.
   Unmasked, that file reads as a two-call raw writer; masked, it reads zero.
   The new scan asserts an **exact** caller count, so a technique that counted
   prose would make the pin unmeetable from day one — precisely the failure
   mode the current F-3 comment records for `enter_finalizing`'s ten string
   literals under `v1_lifecycle/`.
2. **The definition exclusion.** The floor now contains the primitive's own
   name, and the primitive's own `fn write_atomic_verified` lives in the
   neutral module. Without the `fn ` exclusion, half (2) would have to
   special-case the module by name in two places instead of one; with it, the
   definition is not a call anywhere, which is the same invariant the raw-rename
   scan at `:1470–1486` relies on.
3. **String literals.** The v1 transition tables name actions as strings
   (`v1_lifecycle/transition/reduce/mod.rs:90`,
   `authority/observe/finalization.rs:99` name `"enter_finalizing"`). If a
   future action name is spelled after the primitive, masking is what keeps it
   from reading as a violation.

Tests to move with it, `test_check_checked_artifact_boundaries.py`:
`:1181` `test_v1_lifecycle_v0_persistence_call_compiles_and_is_rejected`
(the compiler probe — keep the probe shape, re-point the injected call),
`:1192` `test_v1_lifecycle_v0_persistence_call_in_test_code_is_rejected`
(keeps the "test code counts too" coverage), `:1203`
`test_v0_persistence_seam_inventory_must_stay_derivable` (**retires** with the
derivation; replaced by an anti-vacuity test on step (0)), `:1234`
`test_v1_lifecycle_action_name_literals_are_not_persistence_calls` (keeps the
masking proof). One test is **added**: a second caller of the primitive outside
`entry.rs` is rejected, which is the half the old scan never had.

## B.4 Coverage delta — honestly

**Not equivalent. Two things are lost and two are gained.**

**LOST — L1, the auto-widening derivation.** The current scan's stated virtue
is *"a newly exported v0 persistence item is covered the day it is added"*
(`:360–363`). The replacement's floor is a fixed two-name set. If, after the
close, `merge/mod.rs` grows a new store re-export, the replacement covers it on
no day at all until someone edits the floor. This is a real reduction in
class-level coverage, and it is the direct cost of charter §4's refusal to keep
a seam alive for a regex to derive from.

**LOST — L2, and this is the concrete one.** The current derived set contains
names that **survive** the close and are exactly the surviving envelope
machinery: `classify_open_record`, `OpenRecordEnvelope` and
`discover_open_envelope_before_manifest` are re-exported at `merge/mod.rs:86–90`
today and are what charter §2 / §10.3 require post-close (`discover_open`
envelope-only). Today, a `v1_lifecycle/` file naming `classify_open_record`
fires "v1 lifecycle names the v0 persistence seam". Under the replacement it
fires nothing. So a concrete regression exists: **after the close, v1 lifecycle
code could reach into the surviving open-record store module and no gate would
say so.** This is not hypothetical coverage — it is a live name in the live
derived set at gwz-core `57502e4`.

**GAINED — G1, a caller-side property that never existed.** The current scan
only ever looked *inside* `v1_lifecycle/`. Half (2) of the replacement is
crate-wide over every production file and pins an **exact** count, so a second
raw record writer anywhere in production, or a conversion of the one permitted
arm, fails closed. Nothing in the tree asserts that today.

**GAINED — G2, the primitive is covered at all.** `write_atomic_verified` is in
**no** checker set today — not the F-3 floor, not
`CAPABILITY_FREE_WRITER_TOKENS`, not `V1_LIFECYCLE_RAW_DURABLE_WRITERS`, and
its home file `merge/store/mod.rs` is not an inventory row. After the close it
is the merge record's only raw publication primitive on a handle-fail volume
(charter §3). The replacement plus Part A's row are what bring it under a gate.

**Mitigation offered for L2, not adopted here.** The derivation half could be
*re-pointed* rather than retired: keep `v0_persistence_seam` deriving from the
surviving `merge/mod.rs use store::{…}` block, with the floor reduced to the
names measured to survive, and keep asserting `v1_lifecycle/` names none of
them — then add the neutral-primitive property on top. That restores L1 and L2
in full and adds G1/G2. It is arguably **not** the ghost seam charter §4
forbids, because the surviving re-exports are load-bearing production
(envelope discovery, `gwz stage`, GC), not hollow. But charter §4 says the
floor *becomes* the neutral primitive's module, and adding a second retained
scan is scope this step does not own. Recorded as OPEN QUESTION Q12 for the
reviewers, who can rule it in at no design cost — the code is the existing
`:1378–1390` and `:1494–1506` with a smaller floor set.

## B.5 The J-1 ruling

### B.5.1 What J-1 actually says — located and cited

J-1 is **not** a ruling about the F-3 scan. It is the R4b-G evidence review's
judgment call about **M5b-IMPL merging ahead of R4b-G**, and the F-3 scan
entered it only as the answer to one of its three legs. Located at
`dev-docs/GwzM5-8R4bG-Evidence.md` §9.3, `:829–899`, headed:

> **J-1 — M5b-IMPL merged ahead of R4b-G. The single largest call.**

Its unmitigated leg, `:851–856`:

> **What is not mitigated.** Reason (c) stands unaddressed: "M5b's own
> unreachability argument leans on R4b-G's call-graph gate (§6 item 4)" — and
> the call-graph gate's v1→v0 half (W2/F-3) **does not exist**. M5b's
> unreachability argument is currently leaning on an absent gate.

The dated bracket that closes it, `:856–865`:

> **[2026-08-24: reason (c) is now addressed. The gate exists — F-3's masked-
> token scan in `check_checked_artifact_boundaries.py`, wired into the
> per-commit CI lane. It lands as a **standing guard going forward**, not as a
> retroactive claim about the merge that already happened … The guard's own
> comment says it is load-bearing for J-1 and must not be weakened without the
> lane owner's ruling.]**

The adjudication and its ratification, `GwzM5-8R4bG-Evidence.md:887–899` and
`CurrentProgramCheckpoint.md:643–664`: J-1 was ruled **ACCEPTED WITH RECORD**
by the lane owner on four legs — leg 3 being *"the leaned-on call-graph gate
(F-3) lands with this remediation as a standing guard rather than a
retroactive claim"* — and **RATIFIED by both axes** of the R4b-G dual on
2026-08-24, with the named before-A1 remedy not triggered and the M5b-IMPL
settled review recorded as owed pre-A1 (subsequently GO,
`dev-docs/history/GwzM5-8M5bImplSettled-Review.md`).

The requirement this step must satisfy is the checker comment's, `:353–358`:

> LOAD-BEARING FOR JUDGMENT CALL J-1. … Removing or weakening it re-opens J-1
> and must not be done without the R4b-G lane owner's ruling.

Authority to issue it: the R4b-G lane owner is this program's **implementation
lane**, which authored the J-1 adjudication of record
(`CurrentProgramCheckpoint.md:643`, "LANE-OWNER ADJUDICATION OF J-1, on record
for the dual") and authors this document.

### B.5.2 The ruling

To be quoted verbatim into the checker's replacement comment block, into
`CurrentProgramCheckpoint.md`'s J-1 record as a dated bracket, and into the
M5d close's acceptance record:

> **[J-1 SUCCESSION RULING — 2026-09-03, R4b-G lane owner (the implementation
> lane), issued at M5d S1 under the requirement stated in
> `check_checked_artifact_boundaries.py:353-358`.]**
>
> The derived v0-persistence seam scan is **RETIRED, not weakened, because its
> subject is deleted.** M5d's close removes the v0 merge lifecycle engine from
> production, so `workspace_ops/merge/mod.rs` no longer re-exports
> `persist_merge_record`, `persist_operation_transition`, `enter_finalizing` or
> `archive_merge_record`. Measured at gwz-core `57502e4`, those four names have
> no production caller outside that engine. The scan's own floor check
> therefore fires at the close by construction — `V0_PERSISTENCE_SEAM_FLOOR -
> seam` is non-empty and the checker reports the inventory underivable — so the
> derivation cannot be left standing, and `GwzM5-8M5d-Charter.md` §4 forbids
> keeping a hollow or test-only re-export in production merely so a regex has
> something to derive from.
>
> The gate is **succeeded, not withdrawn.** Its replacement is the
> neutral-raw-primitive property: no file under `v1_lifecycle/` — production or
> test — names the relocated raw record writer or its module, and among
> production files crate-wide exactly one, `checked_artifact/entry.rs`, names
> it, an exact number of times. It is the same masked-source bare-identifier
> scan with the same `fn <name>` definition exclusion and the same `rglob`
> breadth that reaches the ten `#[path]`-mounted test files inside the tree; it
> lands in the same reviewed change that deletes the engine, so no commit
> exists in which the engine is gone and no successor gate is present.
>
> **What J-1's leg 3 asked for is preserved.** Leg 3 accepted M5b-IMPL's
> ordering deviation on the ground that the call-graph gate M5b's
> unreachability argument leans on would exist as a standing guard going
> forward, not as a retroactive claim. That ground survives this succession
> intact and is in fact strengthened on the direction M5b cared about: M5b's
> reason (c) is about v1 lifecycle code not reaching backwards into v0
> persistence, and after this close there **is** no v0 persistence for it to
> reach — the class of violation the guard watched for is not merely
> unobserved, it is unconstructable. The successor additionally pins, for the
> first time, that no production file except `entry.rs` calls the raw record
> writer, which the retired scan never asserted.
>
> **The reduction in coverage is stated, not glossed.** The retired scan was
> auto-widening — a newly exported v0 persistence item was covered the day it
> was added — and its derived set today includes three names that survive the
> close (`classify_open_record`, `OpenRecordEnvelope`,
> `discover_open_envelope_before_manifest`). The successor covers neither case.
> That loss is recorded here rather than argued away, and
> `GwzM5-8M5d-GateRevisions.md` §B.4 offers the reviewers a re-pointed
> derivation that would restore it; this ruling does not require it and does
> not forbid it.
>
> **Scope.** This ruling authorises the succession of the F-3 scan and nothing
> else. It does not re-open, re-ratify or disturb the 2026-08-24 J-1
> adjudication (`GwzM5-8R4bG-Evidence.md` §9.3;
> `CurrentProgramCheckpoint.md:643-664`), which stands as ACCEPTED WITH RECORD
> and RATIFIED BY BOTH AXES, with the M5b-IMPL settled review discharged. It
> takes effect only when the M5d close lands; until then the derived scan is
> the live gate and is not to be touched.

## B.6 OPEN QUESTIONS

**Q9 — Part A's shrinkage and the amendment's own words.** §3 of the
CapabilityFree amendment says shrinkage "fires (a conversion of a carved arm
may not land without revising this amendment — P-1's message class)", and
`GwzM5-8M5d-Review.md` §3 F3 says "Operator OK needed since the amendment's own
words forbid growth". A.7 argues the deletion of an arm is categorically not
the conversion the clause forbids, and that charter §7's "may leave … may
enter" is the operator-level authorisation for both directions. If the
reviewers or the operator read the clause as requiring a separate operator
ruling for the **entering** row as well, that ruling is owed before the close.

**Q10 — the neutral module's path is not chosen here.** A.4.4 fixes only the
constraint (its leaf identifier must not be one `v1_lifecycle/` already names,
which excludes `durable_fs`). B.3's constants use `verified_write` /
`gwz-core/src/verified_write.rs` as a placeholder so the code shape is concrete.
Charter §9 step (3) chooses the real path; the close substitutes it in
`NEUTRAL_RAW_WRITE_FLOOR`, `NEUTRAL_RAW_WRITE_MODULE`, and the alternate SHA in
A.5 if it lands as shape (A).

**Q11 — the charter's premise is only partly measured true.** Charter §4 names
`FileMergeStore` among the "production v0 persistence re-exports" that leave.
Measured (B.2.3), `FileMergeStore` and `MergeStore` have surviving production
consumers at `handle_stage.rs:34`, `runtime/open_gate.rs:20,:36`,
`runtime/dispatch.rs:207`, `runtime/mutation_guard.rs:84,:94` and
`recovery.rs:80,:112` — the very sites charter §2/§10.3 keep as envelope-only
discovery. Four of six floor names leave with certainty; these two may survive
in a non-persisting role. The ruling in B.5.2 is written to be correct either
way (it rests on the four certain names, which alone make the floor check
fire), but a reviewer comparing it against charter §4's sentence should know
the sentence is broader than the measurement.

**Q12 — should the derivation be re-pointed rather than retired?** B.4's
mitigation restores coverage losses L1 and L2 at the cost of one retained
function and one smaller floor set. Charter §4 says the floor *becomes* the
neutral primitive's module and forbids a **hollow** seam; the surviving
re-exports are not hollow. Adopting it is additive coverage the charter neither
requires nor forbids. This step declines to widen its own scope; the reviewers
can rule it in.

**Q13 — the P-2 test's positive control reads a file this close deletes.**
`capability_free_exception.rs:110–117` asserts that
`workspace_ops/merge/finalize.rs` contains `commit_gwz_paths_checked`, as the
live control for the `GIT_CHECKED_SEAM` exclusion. `finalize.rs` is deleted by
this close, so `carved()` panics on a missing path. A.6 item 4 names the
re-point (`execute.rs:31` is the other measured home of that seam), but the
choice of control file is the close's, and if the reviewers want the control
re-derived rather than re-pointed that is a larger edit to that test.

**Q14 — `entry.rs` cannot be a PURE carved file.** A.6 item 4: the P-2 scan
asserts PURE files name **no** checked-boundary door vocabulary, and
`checked_artifact/entry.rs` is the boundary's own module — it certainly names
`CheckedArtifact` (`entry.rs:546`). So the entering inventory row's file must
join `MIXED_CARVED_ARMS` with a region signature naming the raw arm inside
`create_merge_store_record`, not `PURE_CARVED_FILES`. That region does not
exist yet — charter §9 step (3) creates it — so its exact signature string is
unfixable at S1 and is the close's to write.
