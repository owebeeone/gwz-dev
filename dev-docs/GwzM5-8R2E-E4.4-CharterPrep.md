# GwzM5-8 R2-E E4.4 — charter prep (read-only investigation)

Date: 2026-09-02. Author: a read-only investigation session for the
implementation lane. **Status: INPUT TO E4.4's CHARTER. Not an amendment,
not a plan change, not a build.** *[SUPERSEDED 2026-09-02: E4.4 does not
start; §6's recommended E4.4a/E4.4b boundary shapes are not built — the
archive is carved out by `GwzM5-8R2E-CapabilityFreeAmendment.md` §3 on the operator's ruling
`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`; §1/§7's enumeration was corrected in four places by that
amendment's dual (see its §1); §2's record-root analysis and §4's tier-2
finding stand as the record of why.]* No file in `gwz-core` was modified; no
build, no test, no commit was run. Every claim below carries a `file:line`
citation opened and read in this session.

Trees read: `gwz-core` at main `7f28907` (E4.2's landing; `git status`
clean) and `gwz-dev/dev-docs` at the same tip. E4.3-B is IN FLIGHT on
branch `e4/e4-3b-record-root` and is **not on main**; the rejected E4.3
candidate is preserved at `origin/probe/e4-3-detach-window-evidence`.

Authorities consumed: `GwzM5-8R2E-RecordRootAmendment.md` (ADOPTED,
§1a/§1b/§4/§6); `GwzM5-8R2E-Plan.md` §1.1 (O8/O13/O14), the Phase-E4
preamble `:331-383` and the E4.4 step `:512-527`;
`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 row `:275`;
`GwzM5-8R2E-E4.1-Review.md` §R2.3 `:690-713` and `[P3-C1]` `:1021`;
`GwzM5-8R2E-E4.2-Review.md` Flag 6 `:402-460`; `GwzM5-8R2E-E5-Review.md`
adjudication G `:330-366`; `GwzM5-8R2E-E7-Acceptance.md` `:175`, `:180`,
`:308`; `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §5.2 `:909-917` and §6.3
`:1110-1140`.

**The frozen row, verbatim** (`GwzM5-8R4bR2ConsumerCheckpoint.md:275`):

> `| terminal archive source-only | `MergeArchive` when missing | terminal/source validation before bootstrap; no creation for both-absent/destination-only |`

---

## 0. Headline answers

1. **The archive does NOT hit the record-root wall** — but it sits exactly
   one ordering decision away from it. §2.
2. **The plain-lease disposition is the wall instead.** Every one of the
   eight raw durable-write call sites is reachable ONLY from a
   capability-free command (abort/preserve on the plain `V1MutationLease`,
   or GC under no v1 lease at all). Options (a) and (b) of the §4 clause
   are both refuted in the tree; only option (c) — amending the frozen
   capability-free list — or a §2-style carve-out remains, and both are
   amendment-tier moves, not build moves. §1.
3. **Tier 2's comparable sub-surface is UNMINTED everywhere in the corpus.**
   Five hits, all of them statements that it is owed. §4.
4. **The row's two clauses are already implementable by construction** —
   the boundary carries a purpose-built, zero-production-caller archive
   validator (`observe_checked_archive_source_v1` →
   `CheckedManagedActionV1::for_archive` →
   `ManagedParentBootstrapRequest::for_archive`) whose type ordering *is*
   "terminal/source validation before bootstrap", and whose two refusal
   variants *are* the both-absent and destination-only shapes. §3.

---

## 1. LEASE-REACHABILITY TABLE

### 1.0 First correction: the O13 counts are REFERENCES, not call sites

The charter prompt asks for "the 2+2+7 sites". Measured against the tree,
the O13 map is a count of *bare identifier references on masked source*
(`scripts/checks/check_checked_artifact_boundaries.py:1187-1196`, the
`re.findall(r"\b" + writer + r"\b", text)` idiom), and it therefore
**counts the `use` lines**. Verified mechanically in this session
(`grep -o … | wc -l`): `archive.rs` `sync_dir` = 2; `store/archive.rs`
`sync_dir` = 7, `rename_noreplace` = 2 — matching the pin at
`check_checked_artifact_boundaries.py:345-349` exactly.

So: **11 references, 3 of which are imports; 8 real durable-write call
sites.** A charter that writes "eleven sites" is wrong and its builder
will discover it at the first grep.

**Second correction: the pin does not measure the whole conversion
surface.** Four further raw mutations live in the same two functions and
are invisible to the O13 pin, because the pin only counts the three
`durable_fs` names:

| # | Site | What |
| --- | --- | --- |
| — | `store/archive.rs:54` | `fs::remove_file(source)` (both-copies reconcile) |
| — | `store/archive.rs:61` | `fs::create_dir_all(&done)` — **this is row `:275`'s "`MergeArchive` when missing", implemented raw** |
| — | `store/archive.rs:77` | `fs::remove_file(source)` (AlreadyExists reconcile) |
| — | `archive.rs:206` | `fs::remove_file(path.as_path())` (archive deletion) |

The conversion surface is therefore **12 mutation points, of which the
O13 pin measures 8.** The `create_dir_all` at `store/archive.rs:61` is the
single most important one for this row and the pin cannot see it. Consequence
for §5: a conversion could retire the O13 rows while leaving raw `std::fs`
mutations resident.

### 1.1 The eight durable-write call sites, with their lease

`service.rs:116-121` is the routing fork, verbatim:

```
let lease = match request {
    V1LifecycleRequest::ResumeStart | V1LifecycleRequest::Continue => {
        V1MutationLease::acquire_activated(root)?
    }
    _ => V1MutationLease::acquire(root)?,
};
```

| Site | Function / arm | Reached via | Lease | Abort's `respond()` reaches it? | GC reaches it outside any v1 lease? |
| --- | --- | --- | --- | --- | --- |
| **A** `archive.rs:207` `sync_dir(done)` | `remove_archive` (`:191-208`) | `gc_archived_with_hook:148` ← `gc_archived:117` | **NONE** — `WorkspaceMutatorLock::acquire` only (`archive.rs:126`); no `V1MutationLease` is ever constructed on this path | no | **YES — this is the GC site** |
| **B** `store/archive.rs:55` `sync_dir(merge_root)` | `archive`, `(true,true)` reconcile arm `:46-58` | `store/mod.rs:70` ← `service/execution.rs:26` or `:44` | **PLAIN** `acquire` (`checked.rs:134-141`) | **YES** | no |
| **C** `store/archive.rs:56` `sync_dir(&done)` | same arm | same | **PLAIN** | **YES** | no |
| **D** `store/archive.rs:63` `rename_noreplace(source, &destination)` | `archive`, `(true,false)` **publication** `:59-84` | same | **PLAIN** | **YES** | no |
| **E** `store/archive.rs:65` `sync_dir(merge_root)` | `(true,false)` Ok arm | same | **PLAIN** | **YES** | no |
| **F** `store/archive.rs:66` `sync_dir(&done)` | same | same | **PLAIN** | **YES** | no |
| **G** `store/archive.rs:78` `sync_dir(merge_root)` | `(true,false)` `AlreadyExists` reconcile `:70-81` | same | **PLAIN** | **YES** | no |
| **H** `store/archive.rs:79` `sync_dir(&done)` | same | same | **PLAIN** | **YES** | no |

### 1.2 The reach traces, driven from the tree

**Sites B–H — the ONLY lease they ever run under is the plain one.**
Four independent gates confine `store::archive` to `V1LifecycleRequest::Archive`:

- `authority/dispatcher.rs:236-241` — a terminal record (`Completed |
  Aborted`) observes only when `request == Archive`; every other request
  returns `Respond(Terminal(state))`.
- `authority/observe/archive.rs:35-46` — the observation refuses unless
  `request.lifecycle() == Archive` and `kind() == ObservationKind::Archive`.
- `authority/resolver.rs:393-395` — `(K::Archive, C::Archive) if request ==
  V1LifecycleRequest::Archive` is the only producer of
  `Respond(ArchiveReady)`.
- `authority/resolver.rs:293` — `NotStartedObservation::Archive =>
  PhysicalActionKind::Archive`, the only producer of the Archive physical
  action, which `service/execution.rs:25-30` turns into `store.archive`.

`Archive` never matches `ResumeStart | Continue`, so it always takes the
`_ =>` arm at `service.rs:120`. **`store::archive` has no activated-lease
reach at all.** It is not a corner: it is the whole function.

**`V1LifecycleRequest::Archive` is issued at exactly ONE production site**
(`grep -rn "V1LifecycleRequest::Archive" src/`, non-test): `archive.rs:83`,
inside `archive_terminal`. And `archive_terminal` is called at exactly one
production site: `start.rs:118`, inside `respond()`. `respond()` is called
from `start.rs:98` (after `ResumeStart`) and `start.rs:169` (after
`Continue | Abort | Preserve`).

**So all four mutating arms reach sites B–H, and every one of them does so
on the plain lease.** This is the E4.1 review's own driven finding
(`GwzM5-8R2E-E4.1-Review.md:706-713`: *"`respond()` … then archives a
`Terminal` disposition through `archive_terminal` → `service::run(…,
Archive, …)` → also the plain lease. So the abort chain is capability-free
end to end, archive included. PR-2 and the Abort probe below both run that
whole chain on an obstructed workspace and it completes."*), carried
forward as `[P3-C1]` (`:1021`) with carrier E4.4.

**ADVERSARIAL FINDING — the amendment's "one corner" does not protect this
row; it *is* this row.** `GwzM5-8R2E-RecordRootAmendment.md:71-75` reasons
that under the E4.3 conversion *"every STANDARD-PATH v1 abort needs an
admitted filesystem (the one corner: an abort of an already-terminal record
responds Terminal before the service loop commits anything,
`service.rs:101-108`, and carries no probe)"*. Traced in the tree, that
corner is precisely the archive path:

```
handle_v1_command (start.rs:151-169)  --Abort-->
  service::run(Abort)                  -->  next_action (dispatcher.rs:236-241)
     terminal && request != Archive     -->  Respond(Terminal(state))   [NO LEASE]
  respond() (start.rs:115-120)          -->  Terminal(_) matches
    archive_terminal (start.rs:118)     -->  service::run(Archive) (archive.rs:79-85)
                                        -->  PLAIN lease (service.rs:120)
                                        -->  store.archive  -->  sites B-H
```

An abort of an already-terminal, not-yet-archived v1 record — the exact
case the amendment cites as probe-free — would, under the E4.4 conversion,
require an admitted filesystem. E4.4 therefore inherits §1b's defect class
**at full strength, with no corner-case relief**.

**Site A — outside every v1 lease.** `gc_archived_with_hook` takes
`WorkspaceMutatorLock::acquire(root)` at `archive.rs:126` and constructs no
`V1MutationLease`. It has **zero production callers**: the
`#[allow(dead_code, reason = …)]` at `archive.rs:108-111` says so in the
tree (*"v1 archive GC keeps its typed open-record refusal, so this family
has no production caller until that route lands"*), and the only callers
are `tests/gc.rs` (`:18,:34,:47,:65,:112,:130,:167,:197,:211`). Landing
that route is E4.4's, per plan `:512` and O8 (`plan:53`).

**The live gap that O8 route closes, on the record.** Today a `--no-ff` v1
merge that completes writes a v1-envelope archive at
`.gwz/merge/done/<id>.yaml` (`store/archive.rs:29-34`), and
`gwz merge --gc <id>` routes to `gc::handle_gc` (`runtime/dispatch.rs:361`)
which decodes the archived bytes with `decode_production_v0`
(`gc.rs:188`) — a decoder that explicitly refuses a v1 envelope
(`record_wire/decode.rs:128-133`, *"the v0 decoder received a v1 record"*).
So a v1-produced archive is currently un-GC-able. That is a shipped
consequence of E4.1's activation, not a new E4.4 risk, but E4.4's charter
should name it as the thing the O8 route fixes.

### 1.3 Disposition per the §4 PLAIN-LEASE PROBE CLAUSE

The clause (`RecordRootAmendment.md:186-197`) offers three dispositions.
Assessed honestly against the tree:

**Sites B–H:**

| Option | Verdict | Ground |
| --- | --- | --- |
| **(a) split the shared function by lease** | **NOT AVAILABLE, and dishonest if forced** | There is no second lease to split against: `store::archive` runs on the plain lease from *every* arm. The only splittable axis is the *originating* disposition, which `respond()` knows (`start.rs:115`) but `archive_terminal` deliberately discards — it re-enters `service::run` with a fresh lease and a record that carries no memory of which arm made it terminal. Forcing the split would leave the leaf with TWO writers (boundary for completed starts, raw for aborts), which is exactly the "no legacy raw writer" breach row `:280` forbids, and would require the `(true,true)` / destination-only reconcilers to close both writers' crash shapes. **Flag this as a trap in the charter.** |
| **(b) prove reverse-arm reach structurally absent** | **REFUTED IN THE TREE** | Not merely present — it is the *only* reach for the abort case. Driven at `GwzM5-8R2E-E4.1-Review.md:706-713` (PR-2, PR-3 on a genuinely incapable FAT32 volume). |
| **(c) amend the capability-free list at the amendment tier** | **The only honest route inside the clause's own three** | E0.2 §5.2 (`SemanticsAmendment-DRAFT.md:914-917`) is frozen text: *"an ORDINARY merge, `gwz repo create`, `init-from-sources`, abort, GC and the mutation guard continue to work; a checked-artifact action refuses, typed."* Amending it falsifies shipped v0.12-lineage text at `capability.rs:53-57` (*"`gwz merge --abort`, which needs no such filesystem"*) and the E4.1 runbook/release-note sentences the amendment's P-3 (`:180-182`) kept true by construction. That is a user-visible regression, not a note. |

**A fourth disposition the clause does not enumerate, which the tree points
at: (d) an ARCHIVE carve-out at the amendment tier**, the §2 shape. Its
ground would be different from the record root's (no discovery-dead
window — see §2), and narrower: *the archive's only consumers are on the
frozen capability-free list.* It is a real option and E4.4's charter should
name it as one, so its builder is not forced to invent it at delivery.

**A fifth, which is a design change, not a conversion: (e) archive lazily.**
Stop archiving inside `respond()`; leave the terminal record open and let a
later capable command archive it. This preserves both the conversion and
capability-free abort, but it (i) breaks `respond()`'s stated contract
(`start.rs:101-106`: *"A terminal record is archived before it is
projected"*), (ii) changes the response a completed/aborted merge returns
from the archived projection to an open one, and (iii) leaves a terminal
record where `classify_open_record` keeps finding it. Amendment tier, not
500 LOC.

**(f) try-boundary-then-fall-back-to-raw** is the obvious tempting shape
and must be named and pre-rejected: it keeps both writers alive
permanently, which is the row `:280` breach in a different costume.

**Site A** — a **gap in the §4 clause's letter**, which E4.4 must close
rather than exploit. The clause binds paths *"reachable from the PLAIN
`V1MutationLease`"*. `remove_archive` is reachable from **no**
`V1MutationLease` at all (`archive.rs:126` takes only the
`WorkspaceMutatorLock`), so the clause's letter does not reach it. Its
**purpose** plainly does: GC is named in the *same sentence* of the frozen
capability-free list as abort (`SemanticsAmendment-DRAFT.md:914-917`).
E4.4's charter must restate the clause's purpose for site A explicitly.
Same three-plus-two options apply; same assessment.

**Summary of the honest disposition:** for all eight sites, the honest
answer is **(c) or (d) at the amendment tier**, not (a) and not (b).
E4.4 as a build cannot dispose of this by itself.

### 1.4 Why "just convert the leaf, skip the parent" does not escape

A charter author may reason that the probe comes from the *managed-parent*
bootstrap and can be avoided by converting only the leaf write. It cannot:

- `CheckedArtifact::acquire` (`observation.rs:82-115`) calls
  `durable_identity` at `:96` (artifact root) and `:105` (parent dir);
  `durable_identity` (`:363-365`) is `identity::object_identity`
  (`identity.rs:154-156`), and its failure is `unsupported(label, cause)`
  → `ErrorCode::UnsupportedOperation` (`observation.rs:376-379`). **Every**
  checked leaf action carries the probe.
- And the parent route cannot be skipped either, even when `.gwz/merge/done`
  already exists: `admit_merge_start_managed_parents` builds
  `RetainedManagedParentProviderV1::from_retained_catalog(&catalog)`
  (`coordinator/execution.rs:135-138`) *before* it can return `Ok(None)` for
  a proof-only plan (`:139-142`), and the catalog comes from
  `recover_or_create` (`entry.rs:352,:358`). There is no probe-free path
  through the door.

---

## 2. RECORD-ROOT ANALYSIS FOR THE ARCHIVE LEAVES

### 2.1 Move or copy? — MOVE, and atomically

`store/archive.rs:63` publishes with `rename_noreplace(source,
&destination)` — a single directory operation, in the only arm that
publishes (`(true,false)`, `:59-84`). The other two write arms complete a
move whose publication half already happened: `(true,true)` at `:46-58`
verifies byte-equality then `fs::remove_file(source)` at `:54`; the
`AlreadyExists` arm at `:70-81` does the same at `:77`.

**There is no window in which the terminal record exists in neither place.**
The rename is atomic with respect to directory-entry visibility; the record
is at `.gwz/merge/<id>.yaml` before it and at `.gwz/merge/done/<id>.yaml`
after it. The `sync_dir` pair at `:65-66` is durability, not visibility.
This is the same property `RecordRootAmendment.md:56-57` credits to the
retired `rename_durable(temp, path, replace=true)`.

### 2.2 Does the archive leaf become its own discovery root? — NO

**It never needs to be one.** The two-leaf structure gives the archive
exactly what the record rewrite lacked: during the transition, the OPEN
record is still there and is still the discovery root. The rewrite's window
was fatal because the open record *was* the thing being detached; here the
open record is the thing being *superseded*, and a supersession has a second
copy by definition.

Discovery, as shipped:

- `classify_open_record` (`store/mod.rs:211-250`) enumerates via
  `record_files` (`:419-435`), which reads `root.join(MERGE_DIR)`
  (`MERGE_DIR = ".gwz/merge"`, `:30`) and keeps only entries whose extension
  is `yaml` (`:429`). `done/` is a subdirectory with no `.yaml` extension —
  **invisible to enumeration**. `discover_open_envelope_before_manifest`
  (`:270-292`) inherits that.
- The archive is reached **by id**: `acquire_canonical_merge_locations`
  (`record_wire/location.rs:105-160`) requires an explicit `merge_id`
  (`:109`) and reads both leaves (`:124-134`) with a
  before/after parent-identity re-check (`:139-155`).
- `select_canonical_status_source` (`status/snapshot.rs:52-70`) returns
  `Archived` when open is absent and archived is exact (`:63`), and
  `handle_status` with **no** id reports **idle** (`:233-237`). That is the
  correct shipped semantics: an archived merge is not an open operation, so
  "no open merge" is the truth, not a discovery failure — unlike §1a's
  window, where an *open* operation had vanished.

### 2.3 What reconciles an archive leaf today

`archive_terminal` (`archive.rs:70-104`) is the reconciler, and it is
driven over all three shapes at
`tests/archive.rs:11-40`
(`terminal_archive_restarts_from_source_both_and_destination_only`):

| Shape | Path | Site |
| --- | --- | --- |
| source-only | `open_record_present` true (`archive.rs:77`) → `service::run(Archive)` → `NotStarted(Archive)` → `execute_owned` → `store::archive` `(true,false)` → publish | D–F |
| **both present** | same entry → observation returns `ExactTerminalCopy` (`observe/archive.rs:74-100`) → `Completed(Archive)` → `resolver.rs:393-395` `Respond(ArchiveReady)` → `complete_response` (`execution.rs:43-45`) → `store::archive` `(true,true)` → delete source | B–C |
| destination-only | `open_record_present` false → `WorkspaceMutatorLock::acquire` (`archive.rs:97`) → re-check (`:98-102`) → `acquire_archived` (`:103`), **read only, no write** | none |

Plus `store/archive.rs`'s own `(false,true)` arm at `:42-45` (source
vanished between observation and execution: validate destination, return
`ReconciledDestination`, create nothing) and the `(false,false)` refusal at
`:38-41`.

**NOT** `classify_open_record` (it never sees `done/`). **NOT**
`gc_archived` — that *deletes* an already-reconciled archive and refuses
outright while any open record exists (`archive.rs:127-132`).

### 2.4 Can the boundary's detach-then-publish shape serve this row?

**YES — conditionally, and the condition is an ORDERING that must be pinned.**

The move is intrinsically **two** checked actions, because the two leaves
sit under two different managed parents: `MergeStore` = `.gwz/merge` and
`MergeArchive` = `.gwz/merge/done` (`bootstrap/managed.rs:82-88`), with
`MergeArchive::minimum_retained_parent_count() == 2` (`:92-97`). A
`CheckedArtifact` is acquired per (policy, relative path)
(`observation.rs:82-115`); no single checked action spans both.

**Half one — publish the destination.** `replace_exact(&CheckedArtifactFact::Missing,
source_bytes)` on `.gwz/merge/done/<id>.yaml`. This is the E4.2 create
shape (`entry.rs:371-392`). `replace_exact` detaches **only** when
`authority.retained_source` is `RetainedSource::Existing(_)`
(`transition.rs:87-99`); an expected-`Missing` action takes the else branch
at `:89-99` and never enters `detach_existing`. This is the amendment's
own carve-out-exempt case (`RecordRootAmendment.md:62-63`: *"E4.2's create
side was safe: `MissingReplace` never detaches"*).

**Half two — remove the source.** `remove_exact(&Bytes(source_bytes))` on
`.gwz/merge/<id>.yaml`. This **does** detach (`transition.rs:161` →
`detach_existing:179-320`, publishing the source into the private area at
`:283-296` under a name derived from the authority). Its crash window
leaves *source-in-private + destination-present* — which is the
**destination-only** shape, already the archive's normal terminal steady
state, already reconciled at `archive.rs:94-103` and driven at
`tests/archive.rs:31-39`. Not discovery-dead.

**The new intermediate the conversion introduces** — BOTH copies durable at
once — is likewise a shipped, reconciled, driven shape
(`store/archive.rs:46-58`, driven at `tests/archive.rs:23-29`). But note
the behavioural change: **today production never produces it.** Only a
foreign or concurrent actor does, via the `AlreadyExists` arm at `:70-81`.
After the conversion, the lifecycle itself produces it on every archive.
That is a real change of what the `(true,true)` arm is *for*, and it must
be named and re-driven, not assumed.

### 2.5 THE WALL IS ONE ORDERING DECISION AWAY

If a builder decomposes the move **remove-source-first, then
publish-destination**, the crash window is *source-in-private +
destination-absent* — and that is `RecordRootAmendment.md` §1a's
discovery-dead window, reproduced exactly:

- `classify_open_record` (`store/mod.rs:211-250`) reports **no open merge**;
- `acquire_canonical_merge_locations` (`record_wire/location.rs:105-160`)
  finds neither leaf; `select_canonical_status_source`
  (`status/snapshot.rs:57-61`) returns `OperationNotFound`;
- `gwz merge --abort` cannot discover the id;
- the only surviving copy is content-sufficient but discovery-invisible
  private residue in `.gwz/checked-artifacts`, git-status-dirt-exempt
  (`policy.rs:33-45`; `git/gitbackend/repository_support.rs:122-151`;
  `preservation_image.rs:236-248`), and ignored by recovery support
  (`recovery_support.rs:360-363`).

**Answer to the charter's question:** the archive row does **not** hit the
record-root wall — but it is not immune to it either. Publish-before-detach
is the load-bearing invariant, and it must be written into the charter as a
pinned, driven constraint with its own negative tripwire, not left to the
builder's judgement.

---

## 3. THE ROW'S CLAUSES, MAPPED TO MECHANISMS

### 3.1 "terminal/source validation before bootstrap"

**Today (raw), the ordering holds:**

| What | Where |
| --- | --- |
| terminal state gate | `store/archive.rs:15-24` (lease covers + `Completed \| Aborted`); one layer up at `observe/archive.rs:38-41` |
| source exactness | `store/archive.rs:100-112` `require_exact_source` (re-read, re-open, `same_source_as`); `observe/archive.rs:50-67` (canonical Open path, kind check, lineage check) |
| **the bootstrap** | `store/archive.rs:61` `fs::create_dir_all(&done)` |

`require_exact_source` at `:60` precedes `create_dir_all` at `:61`; the
state gate at `:15-24` precedes everything. The row's frozen ordering is
satisfied by the raw path today.

**Under the managed-parent door: the mechanism is ALREADY BUILT and has
ZERO production callers.** This is the most consequential finding in this
section for scoping E4.4.

`observe_checked_archive_source_v1` (`record_wire/checked_owner.rs:151-182`)
constructs a `CheckedArchiveSourceObservation` (`:31-33`, whose field is
private and whose only constructors are in this module — doc at `:28-30`:
*"Only record-wire arbitration can construct this value"*) if and only if:

| Gate | Line | Refusal |
| --- | --- | --- |
| archived leaf ABSENT | `:154-156` | `DestinationPresent` |
| open leaf EXACT | `:157-159` | `Absent` |
| open path kind is `Open` | `:160-162` | `Identity` |
| bytes bounded (≤ 16 MiB, `:9`) | `:163` (`require_bounded:218-223`) | `Bounds` |
| decodes as v1 | `:164-165` | `Decode` |
| **state NOT open** | `:166-168` | `NotTerminal` |
| decodes as an ARCHIVED record | `:169-170` | `InvalidTerminal` |
| file stem == merge_id | `:178-180` | `Identity` |

The v0 twin is `observe_checked_archive_source_v0` / `…_leaves`
(`:112-141`), same shape.

That observation is the **only** input to
`CheckedManagedActionV1::for_archive` (`coordinator/identity.rs:267-282`),
which mints `ValidatedArchiveSourceV1` (`bootstrap/managed.rs:285-313`;
`from_exact_record_owner` at `:290-302` refuses a zero digest), which is
the **only** authority `ManagedParentBootstrapRequest::for_archive`
(`:145-152`) accepts, and whose `try_new` `allowed` match admits
`Archive` authority for exactly the single `MergeArchive` purpose
(`:198-201`). Doc at `bootstrap/managed.rs:281-283`: *"Opaque result of
terminal/source archive arbitration. There is deliberately no general
constructor."*

**"Validation before bootstrap" is therefore a TYPE ordering, not a runtime
check**: the bootstrap request is unconstructible without the validated
observation. That is a stronger discharge of the frozen clause than the raw
path achieves.

**Production-caller status (grepped, non-test):** `observe_checked_archive_source_v1`
and `_v0` are named only by `interface_tests/coordinator_remediation.rs`
(`:188-189`, `:216`, `:239`, `:256`, `:270`, `:289`, `:297-298`) and the
re-export chains (`record_wire/mod.rs:55,:61`; `merge/mod.rs:67,:72-73`;
`workspace_ops/mod.rs:51,:56-57`). `CheckedManagedActionV1::for_archive` and
`ManagedParentBootstrapRequest::for_archive` are named only by
`interface_tests/coordinator_remediation.rs:189,:298`,
`interface_tests/managed_plan_binding.rs:504,:521` and
`bootstrap/managed/tests_purpose_policy.rs:171,:200`. **Zero production
callers for all four.** E4.4 is their consumer.

### 3.2 "no creation for both-absent/destination-only" — the two refusal shapes

**Today:**

| Shape | Refusal / no-op | Where |
| --- | --- | --- |
| both absent | `OperationNotFound`, "archived merge record '…' was not found" | `archive_terminal:97-103` → `acquire_archived` → `CanonicalArchiveAcquisition::acquire` `archive.rs:34-39` |
| both absent (inner) | `MergeRecoveryRequired`, "source and destination are absent" | `store/archive.rs:38-41` |
| destination-only | read-only success, no service run, no write, no bootstrap | `archive_terminal:94-103` |
| destination-only (inner) | validate then `ReconciledDestination`, no creation | `store/archive.rs:42-45` (`require_exact_destination:114-126`) |

**Under the door: both refusals become the ABSENCE of a constructible
request.** `CheckedOwnerObservationError` (`checked_owner.rs:35-44`) is
already shaped for exactly these two:

- `DestinationPresent` (`:154-156`) — covers **destination-only** *and*
  both-present. Correct in both: if the destination exists, the
  `MergeArchive` parent trivially exists, so no bootstrap is owed.
- `Absent` (`:157-159`) — with destination-absence already established one
  line above, this is precisely **both-absent**.

Neither yields an observation → `for_archive` cannot be called →
`ManagedParentBootstrapRequest::for_archive` cannot exist → no
`MergeArchive` bootstrap can be scheduled. `CanonicalRecordLeaf` is binary
(`Absent | Exact`, `location.rs:59-66`), and a non-regular or symlinked
leaf errors inside `read_leaf` rather than presenting as `Absent` (driven at
`tests/archive.rs:99`), so the refusal side is conservative.

### 3.3 The sibling pattern E4.4 mirrors, and where the probe lands in it

E4.2's shape, end to end:

```
V1MutationLease::acquire_for_merge_start   (checked.rs:175-183)
  -> entry::bootstrap_merge_start_parents  (entry.rs:345-363)
       -> recover_or_create(admission)     (entry.rs:352)     <-- PROBE
       -> admit_merge_start_managed_parents(coordinator/execution.rs:130-142)
       -> recover_or_create(execution)     (entry.rs:358)     <-- PROBE
       -> execute_merge_start_managed_parents(coordinator/execution.rs:161-172)
... then the leaf:
  store/rewrite.rs::create_open (:43-75) -> entry::create_merge_store_record (entry.rs:371-392)
       -> CheckedArtifact::acquire (observation.rs:82)         <-- PROBE
       -> parent_is_canonical() refusal (entry.rs:384-390)
       -> replace_exact(&Missing, goal)  (entry.rs:391)
```

E4.4's twin would be an `entry.rs` door taking the
`CheckedArchiveSourceObservation`, an `admit_/execute_merge_archive_managed_parent`
pair beside `coordinator/execution.rs:130-172`, and a lease — and **that
lease is where the probe lands on the plain path**, which is §1.3's problem.

Note also `execute_merge_start_managed_parents`' doc at
`coordinator/execution.rs:153-160`, which is O14's C1(iii) statement and
which E4.4 must reproduce for its own seam: *"admission plus the facade
re-proof — the DURABILITY gate. The AUTHORITY gate stays closed."*
(`GwzM5-8R2E-E4.2-Review.md:435-452`, Flag 6 / C1; plan §1.1 O14
`:120-155`).

---

## 4. TIER-2 ENCUMBRANCE STATUS

### 4.1 Is the amendment done anywhere? — NO

Corpus grep for `"comparable sub-surface"` (`dev-docs/`, all `.md`) returns
**five** hits. Every one of them states it is **unminted and owed**:

| Where | What it says |
| --- | --- |
| `GwzM5-8R2E-Plan.md:520-527` | *"before E4.4 executes tier 2, a comparable sub-surface must be minted BY AMENDMENT WITH DUAL REVIEW, not by the implementing step; deliberately unminted at E5 per §6.3's rejected-alternative warning. Queued for the E7 dual."* |
| `GwzM5-8R2E-E5-Review.md:345-366` (adjudication G(iii)) | *"Tier 2 as literally written in amendment §6.3 … is therefore **not satisfiable at all** without a defined comparable sub-surface"*; *"Was the builder right not to mint one? YES, emphatically."* |
| `GwzM5-8R2E-E7-ReviewState.md:170`, `:268-274` | *"deliberately unminted, and rightly"*; encumbrance-naming completeness is `[P3 F7]`. |
| `GwzM5-8R2E-E7-ReviewCode.md:19` | *"comparable sub-surface deliberately unminted"*. |
| `CurrentProgramCheckpoint.md:2180` | *"deliberately NOT minted by the builder"*. |
| `GwzM5-8R2E-E7-Acceptance.md:175`, `:308` | O8 split close: tier 2 **RE-OWNED, carrier E4.4**, *"two-part encumbrance: R2-F-gated AND sub-surface-by-amendment-first"*. |

The only text that *defines* tier 2 is the unsatisfiable original at
`GwzM5-8R2E-SemanticsAmendment-DRAFT.md:1120-1140`. **No document in the
corpus mints a comparable sub-surface.**

### 4.2 The unsatisfiability, re-verified in the tree

`ArchivedMergeProjection` has exactly three fields
(`model/archive_projection.rs:24-28`): `source_version`,
`terminal_outcome`, `acceptance`. A v1-produced archive projects
`ArchivedAcceptanceProjection::SupportedPersisted`
(`record_wire/archive/v1.rs:12-14`); a v0-produced archive of the same
scenario projects `LegacyComplete` (`record_wire/archive/v0.rs:80`,
`:127`). Two of the three fields differ **by construction**, and only
`terminal_outcome` can be equal. The E5 review's finding stands, verified.

### 4.3 Which route is coherent with the plan text? — EXCLUDE, don't run it

**E4.4's charter must exclude tier-2 execution explicitly and defer it to a
named predecessor amendment.** Three grounds, in descending force:

1. **The plan says so.** `:524-526`: *"a comparable sub-surface must be
   minted BY AMENDMENT WITH DUAL REVIEW, **not by the implementing step**"*.
   Running the amendment inside E4.4 makes the implementing step the
   minter — the thing the sentence forbids.
2. **The conflict-of-interest ground is the whole point.** §6.3's rejected
   alternative (`SemanticsAmendment-DRAFT.md:1130-1134`) exists to stop a
   lane *"satisfy[ing] the clause by comparing only v0-origin archives and
   reporting the O8 clause met"*, and the E5 adjudication
   (`E5-Review.md:359-366`) extends it: *"minting a sub-surface that
   excludes `source_version` and the acceptance discriminant would be the
   same move in a different costume — narrowing the comparison until the
   clause looks met, chosen by the lane that benefits."* A charter that
   bundles the amendment into the step that benefits from it reproduces
   exactly that conflict.
3. **Review-tier arithmetic.** Plan §2 (`:196-205`) budgets **two** duals
   (E0, E7), both spent. The RecordRootAmendment took a third on the
   amendment-tier line's own authority (`RecordRootAmendment.md:10-14`:
   *"this dual sits OUTSIDE plan §2's two-dual budget on that line's
   authority"*). A tier-2 sub-surface amendment would be a **fourth** on the
   same authority — available, but it is an amendment *package* with its own
   dual, sized and reviewed as one. Folding it into a `<500`-LOC conversion
   step is not a scoping choice; it is a tier violation.

**Recommendation:** E4.4's charter carries a dated exclusion sentence
naming the predecessor package (working name:
`GwzM5-8R2E-ArchiveEquivalenceSubSurfaceAmendment.md`), states that E4.4's
delivery may NOT report O8's tier-2 clause as met or partially met, and
records that the encumbrance's other half (R2-F gating) is **satisfied**
(the relocation landed at gwz-core `027da5b` / `bb52dc0`, plan `:156-174`).
The tier-2 half is the only live encumbrance.

---

## 5. TRIPWIRE AND PIN IMPACT

### 5.1 The `recover_or_create` namer tripwire

`interface_tests/catalog_activation_pin.rs`, `PRODUCTION_CALLER_COUNT = 1`
(`:45`). Mechanism: it scans `src/checked_artifact/` for production `.rs`
files that **name** `recover_or_create`, subtracts `OWNER_FILES`
(`catalog.rs`, `catalog/bootstrap.rs`, `:49`), and counts **FILES, not call
sites** (`scan:78-101`; header doc `:12-20`).

**Consequence for E4.4, stated plainly:**

- If the archive parent-bootstrap door lives in **`entry.rs`** (the E4.2
  shape), the tripwire does **NOT** move. This is why E4.2 added two more
  `recover_or_create` calls (`entry.rs:352,:358`) and the count stayed at 1
  — recorded in the checker at
  `check_checked_artifact_boundaries.py:371-374` and in the plan at `:480`
  (*"tripwire UNMOVED at 1"*).
- If E4.4 puts a `recover_or_create` namer in **any other** production file
  under `src/checked_artifact/`, the tripwire **FIRES** and must be moved
  1 → 2 in the same reviewed commit, as `:41-44` requires (*"A SECOND
  production namer is E4.2-E4.6 work and must move this pin again,
  deliberately"*).
- **Recommendation: keep the door in `entry.rs`.** A tripwire move should be
  a deliberate decision, not a side effect of file placement.
- The pin also asserts its own exhaustiveness premise (`:107-116`): the scan
  root is exhaustive only while the entry point stays
  `pub(in crate::checked_artifact)` (`catalog/bootstrap.rs:233`). E4.4 must
  not widen it.

### 5.2 O13 rows — which retire, which stay

`V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`
(`check_checked_artifact_boundaries.py:345-349`):

| Row | E4.4 disposition | Authority |
| --- | --- | --- |
| `workspace_ops/merge/v1_lifecycle/archive.rs: {sync_dir: 2}` | **RETIRE-ON-CONVERSION — E4.4's** | `RecordRootAmendment.md:218-227` (*"This sentence does NOT cover the two archive files … E4.4's charter author may not cite this section against that duty"*); `E7-Acceptance.md:180` (*"the archive rows keep retire-on-conversion until E4.4"*); plan `:88-95` |
| `workspace_ops/merge/v1_lifecycle/store/archive.rs: {rename_noreplace: 2, sync_dir: 7}` | **RETIRE-ON-CONVERSION — E4.4's** | same |
| `workspace_ops/merge/v1_lifecycle/store/rewrite.rs: {rename_durable: 2, sync_dir: 2}` | **PERMANENT-DOCUMENTED — DO NOT TOUCH** | `RecordRootAmendment.md` §3 P-1 `:147-161`. **Fail-closed BOTH directions**: growth fires, and *shrinkage also fires*. E4.4 touching `store/rewrite.rs` re-opens the amendment. |

Checker arms E4.4 will trip if it does not move the pin in its own commit:
`raw_writer_files - expected_files` → "gained a raw durable_fs writer"
(`:1197-1201`); `expected_files - raw_writer_files` → "entry no longer names
durable_fs and must be retired from the pin deliberately" (`:1202-1206`);
count mismatch → "count moved and must move the pin with it"
(`:1207-1214`). A partial conversion is measurable — that is exactly what
E4.2's file-set → count-map widening bought.

**ADVERSARIAL WARNING for the charter (from §1.0).** The map is populated
only for files matching `\bdurable_fs\b` (`:1188-1190`). A conversion that
drops the `durable_fs` import while leaving `fs::remove_file`
(`store/archive.rs:54,:77`; `archive.rs:206`) or `fs::create_dir_all`
(`store/archive.rs:61`) resident would **retire the O13 rows with raw
mutations still in place** — the pin would report success. The tree-digest
backstop (`PROTECTED_SOURCE_TREE_DIGESTS`; amendment §3 P-1 scope note
`:156-161`) notices the edit but does not classify it. E4.4's charter must
require the `std::fs` mutations to be converted or removed too, or state
their survival explicitly with a reason.

### 5.3 Other pins and allowances E4.4 moves

| Item | Where | Why it moves |
| --- | --- | --- |
| `ENTRY_ITEMS` | `check_checked_artifact_boundaries.py:421-455` | add the archive door name(s) |
| `ENTRY_REFERENCES` | `:352-419` | add the caller file(s): `workspace_ops/merge/v1_lifecycle/store/archive.rs`, and `.../checked.rs` if a lease is added |
| `ENTRY_USES` | `:457-470` | add the coordinator's archive-session import |
| `gc_archived`'s dead-code allowance | `v1_lifecycle/archive.rs:108-111` | **expires** when the O8 route lands ("no production caller until that route lands") |
| `is_absent`'s dead-code allowance | `record_wire/location.rs:69` | goes stale the moment `observe_checked_archive_source_v1` gains a production caller |
| `bootstrap/managed.rs:44-47` allow-reason ("shrink as E4.3-E4.6 convert") | | **E4.3-B's** (*"owner: E4.3-B, named"*), per `RecordRootAmendment.md:236-237`. E4.4 must not silently re-own it. |
| `MergeArchive` plan pins | `interface_tests/managed_plan_binding.rs:468,:488-512`; `bootstrap/managed/tests_purpose_policy.rs:165-213` | already pin the archive purpose row; verify unmoved |
| Per-OS count pins + `PROTECTED_SOURCE_TREE_DIGESTS` + driver fault markers | plan `:305-309` (ritual 5) | move with any lib-test addition |
| O14 C1(iii) seam statement | binding on E4.3–E4.6, plan `:145-151` | E4.4 states which gate it opens (durability) and which stays closed (authority) |

---

## 6. RECOMMENDED CHARTER SHAPE FOR E4.4

**STOP FIRST — the honest recommendation, stated before the shape.** §1
finds that all eight raw durable-write sites are reachable only from
commands on E0.2 §5.2's frozen capability-free list (abort/preserve on the
plain lease; GC under no v1 lease at all), that disposition (a) is not
available and (b) is refuted in the tree, and that §2's record-root wall is
*not* the obstacle here. **E4.4 as a conversion build cannot dispose of the
§4 clause by itself.** The coherent sequence is therefore: charter E4.4 as
a **two-part step with an amendment-tier predecessor for the lease
disposition**, or charter it as a **decision packet** that returns the
disposition to the operator the way E4.3 did — but with the analysis done
*before* the build, which is what §4 exists to force. The paragraph below
assumes the first.

**The shape.** E4.4 converts §10 row `:275` — terminal archive
source-only — onto the checked boundary **in two independently reviewable
sub-steps split by file and by concern**, baselined on main after E4.3-B
lands its P-1/P-2 pins (currently `e4/e4-3b-record-root`, not on main;
today's main is `7f28907`), each `<500` LOC per plan `:331-333`, each
single-axis peer-blind with the matrix at the landing and T-6-class
tripwires re-verified. **E4.4a — `store/archive.rs`, the publication
(sites B–H, 7 durable + 4 raw `std::fs` mutation points):** routes the
archive through `record_wire::observe_checked_archive_source_v1` →
`CheckedManagedActionV1::for_archive` → a new `entry.rs` door (keeping the
`recover_or_create` namer count at 1, §5.1) → `replace_exact(&Missing,
source_bytes)` on the destination **then** `remove_exact(&Bytes(source))`
on the source, in that order and never the reverse; retires the
`store/archive.rs` O13 row including its `fs::remove_file` and
`fs::create_dir_all`; carries the O14 C1(iii) seam statement.
**E4.4b — `archive.rs::remove_archive` + the O8 `gc_archived` production
route (site A):** routes deletion through the boundary's `remove_exact`
and gives `gc_archived` its dispatch route, retiring the `archive.rs` O13
row and the `:108-111` dead-code allowance, and closing the live gap that a
v1-produced archive is currently un-GC-able (`gc.rs:188` →
`decode.rs:128-133`). **Lease disposition, per site:** identical for all
eight — plain lease / no lease, on the frozen capability-free list, so
neither 4(a) nor 4(b) is honest, and the charter must carry **option (c)
(amend the capability-free list) or a §2-style archive carve-out (d)** as an
**amendment-tier predecessor**, decided before either sub-step's build
starts, with (a), (e) and (f) pre-named and pre-rejected in the charter text
so the builder does not re-derive them. **Cap:** E4.4a `<500` LOC,
E4.4b `<400` LOC (the O8 route is a dispatch arm plus its refusals);
neither may absorb the amendment. **Driven rows the charter names:**
publish-before-detach ordering (a negative tripwire in P-2's idiom asserting
`store/archive.rs`'s converted path does NOT call `remove_exact` before
`replace_exact`); the both-copies intermediate now produced by the
lifecycle itself (extend
`terminal_archive_restarts_from_source_both_and_destination_only`,
`tests/archive.rs:11`, with a fault-injected crash between the two halves);
both-absent and destination-only proving **no** `MergeArchive` bootstrap
fires; and — the `[P3-C1]` guard the E4.1 review asked for
(`E4.1-Review.md:713-721`) — **`gwz merge --abort` succeeding end to end on
an obstructed/non-admitted workspace after the conversion**, in the R7(iii)
row's shape. **Explicit exclusions:** tier-2 archive equivalence is NOT
executed (§4; the charter names the predecessor amendment and forbids
reporting O8's tier-2 clause met or partially met); `store/rewrite.rs` is
NOT touched in either direction (P-1 fails closed on shrinkage);
`bootstrap/managed.rs:44-47` stays E4.3-B's.

### STOP-AND-REPORT triggers to write into the charter

1. **The lease disposition is not settled before the build.** If E4.4's
   charter reaches a builder without a landed amendment-tier disposition of
   §1.3, the builder STOPS and reports — this is E4.3's `[1c]` failure
   (`RecordRootAmendment.md:81-89`) reproduced, and §4 exists to prevent it.
2. **Any decomposition that detaches the source before the destination is
   durable.** STOP. That is §1a's discovery-dead window (§2.5), and it is a
   carve-out question at the amendment tier, not a build fix.
3. **The `--abort`-on-obstructed-workspace row goes red.** STOP. R2 is being
   silently undone through `[P3-C1]`'s named path.
4. **A `recover_or_create` namer is needed outside `entry.rs`.** STOP and
   report before moving `PRODUCTION_CALLER_COUNT` — the pin's own text
   (`:41-44`) makes a second namer a reviewed decision.
5. **The O13 rows would retire with raw `std::fs` mutations still resident**
   (§5.2's warning). STOP: the pin will report success on an incomplete
   conversion.
6. **Any pressure to execute, narrow, or partially report tier 2.** STOP.
   §6.3's rejected-alternative warning and `E5-Review.md:359-366` name this
   as the failure mode by construction.
7. **`store/rewrite.rs`'s counts move in either direction.** STOP —
   re-opening the RecordRootAmendment is an operator-level event.

### One line for the charter author, on the record-root question

The archive does **not** hit the structural wall §1a describes: it moves
atomically today (`store/archive.rs:63`), it has two leaves so the open
record remains the discovery root throughout, and all three crash shapes
already have shipped, driven reconcilers (`archive.rs:70-104`,
`tests/archive.rs:11-40`). **The wall E4.4 hits is the plain-lease one, and
it hits it harder than E4.3 did** — because for the archive there is no
capability-free corner left over: the corner §1b relied on is precisely the
path that calls `archive_terminal`.

---

## 7. Does the plain-lease wall generalize to E4.5/E4.6?

Added 2026-09-02 at the coordinator's follow-up, same read-only rules. Scope:
the four remaining §10 rows — `:276`/`:277` (E4.5) and `:278`/`:279` (E4.6).

### 7.0 The two facts that make this section short

**(i) `CheckedArtifact::acquire` always probes; `prepare_parent` never does.**
`acquire` (`observation.rs:82-115`) calls `durable_identity` at `:96` (artifact
root) and `:105` (parent dir) → `identity::object_identity`
(`identity.rs:154-156`) → `unsupported(…)` = `ErrorCode::UnsupportedOperation`
(`observation.rs:376-379`). By contrast `CheckedArtifact::prepare_parent`
(`observation.rs:17-63`) uses only `symlink_metadata` / `create_dir` /
`sync_parent` — **no identity probe**. So `entry::prepare_merge_store_parents`
(`entry.rs:150-157`), the one boundary call the **v0** merge store already makes
(`store/mod.rs:131`), is probe-free. Every *other* `entry.rs` door
(`root_artifact` `:159-166`, `preservation_bundle` `:168-175`,
`preservation_workspace` `:177-184`, `preservation_git_directory` `:186-192`)
goes through `acquire` and probes.

**(ii) §5.2's decision sentence is scoped to OPERATIONS, and it already
contemplates refusals.** Verbatim (`SemanticsAmendment-DRAFT.md:914-917`):
*"On a Linux filesystem that refuses persistent file handles, an ORDINARY
merge, `gwz repo create`, `init-from-sources`, abort, GC and the mutation guard
continue to work; a checked-artifact action refuses, typed."* So converting a
consumer and having its checked action refuse is **not per se** a §5.2
violation — the tension arises only when the refusal lands inside one of the
six named operations. Ground 2 (`:948-960`) supplies the purpose: option (i)
*"would cost a Linux user every mutation, not just the checked ones"*, and it
enumerates the mutator-lock's ten production sites.

**A reading question I flag rather than resolve.** "the mutation guard" in that
list is `acquire_workspace_mutation_guard` (`mutation_guard.rs:26-51`, taking
`WorkspaceMutatorLock::acquire` at `:44`), which is the lock taken by
`gwz commit` (`handle_commit.rs:37`), `gwz stage` (`handle_stage.rs:27`),
`gwz materialize` (`handle_materialize.rs:27,:100`), `gwz tag`
(`handle_tag.rs:32`) and `gwz stash` (`handle_stash.rs:43`, via
`guarded_workspace_root`). Whether the clause protects **the lock acquisition**
(narrow — supported by Ground 1, which keeps the probe out of `try_acquire`) or
**the operations that run under it** (broad — supported by Ground 2's
"every mutation" purpose) decides whether rows `:277`/`:278`/`:279` are in
tension for `gwz commit`/`stage`/`materialize`/`stash` as well. **I could not
determine this from the text; it is a genuine ambiguity and a charter-level
question for E4.5/E4.6.**

### 7.1 Row `:276` — ordinary or merge stash bundle | `PreservationBundles` when missing

**(1) Who writes it, under what.** Leaf: `.gwz/stash/bundles/<id>.yaml`
(`stash/mod.rs:13`, path built at `:290-295`).

| Operation | Writer | Lock / lease | Converted? |
| --- | --- | --- | --- |
| `gwz stash` push/pop/apply/drop | `stash::write_bundle` (`stash/mod.rs:260-262`) → `artifact::write_atomic` (`artifact/mod.rs:492-495`) | `guarded_workspace_root` → `WorkspaceMutatorLock` (`handle_stash.rs:43`; `mutation_guard.rs:44`) | **NO — raw** |
| ORDINARY / v0 merge preserve | `preserve/artifacts.rs:353` `crate::stash::write_bundle` | v0 merge lane's `WorkspaceMutatorLock` | **NO — raw** |
| **v1 preserve (`--abort --preserve`)** | `checked_bundle.rs:64-113` `v1_write_bundle_checked` → `entry::replace_merge_preservation_bundle` (`entry.rs:139-148`) → `preservation_bundle()` (`:168-175`) → `CheckedArtifact::acquire` | **PLAIN `V1MutationLease`** (request `Preserve` → `service.rs:120`), from `v1_lifecycle/reverse/execute/preservation.rs:74` | **YES — already probing** |

Parent bootstrap, two shapes today: `entry::prepare_merge_store_parents`
(probe-free, `entry.rs:150-157`) from the v0 store (`store/mod.rs:131`), and
E4.2's `ManagedParentPurpose::PreservationBundles` inside
`ManagedParentBootstrapRequest::for_merge_start` (`bootstrap/managed.rs:114-124`),
which **does** probe — on the v1 CREATION lease (`checked.rs:175-183`), a
`--no-ff` start, not on the capability-free list.

**(2) Would conversion put the probe on a §5.2-named operation?** **YES, twice.**
`abort --preserve` (abort is named) already does — see the third row above; and
converting the ordinary `gwz stash` and v0-merge writers puts it on
*"an ORDINARY merge"* (named) and on `gwz stash` (only via the mutation-guard
reading, §7.0(ii)).

**(3) Already reaching a `CheckedArtifact`?** **YES** — the v1 preserve arm,
today, on the plain lease.

### 7.2 Row `:277` — commit or merge marker | `RootPreservationMarkers` when missing

**(1) Who writes it, under what.** Parent: `gwz.conf/markers`
(`artifact/mod.rs:32`).

| Operation | Writer | Lock / lease | Converted? |
| --- | --- | --- | --- |
| `gwz commit` | `artifact::write_marker` (`handle_commit.rs:213`; `artifact/mod.rs:455-460`) — **and the raw when-missing bootstrap `fs::create_dir_all(root.join(artifact::MARKER_DIR))` at `handle_commit.rs:334`, which IS this row's "when missing" clause implemented raw** | `acquire_workspace_mutation_guard` (`handle_commit.rs:37`) | **NO — raw** |
| ORDINARY / v0 merge publication | `finalize.rs:245` `artifact::write_atomic(&marker_path, …)` | v0 lane's `WorkspaceMutatorLock` (`continue_op/coordinator.rs:13`, or the start guard) | **NO — raw** |
| **v1** merge publication | `v1_lifecycle/finalization/execute.rs:45` `artifact::write_atomic` | activated lease (`ResumeStart`/`Continue`) | **NO — raw** |
| v0 merge preserve | `preserve/artifacts.rs:226` `crate::artifact::write_atomic(&marker_path, …)` | v0 lane's lock | **NO — raw** |
| **v1 root preserve** | `preservation_root.rs:477` `files::replace_relative` (`GitRootManagedObject::MarkerWorktree`) → `entry::replace_merge_preservation_workspace` (`entry.rs:100-107`) → `preservation_workspace()` (`:177-184`) → `acquire` | **PLAIN lease** — reached from `v1_lifecycle/reverse/execute/preservation.rs:87,:125` via `backend.execute_root_preservation_step_checked` | **YES — already probing** |
| **v1 abort marker removal** | `abort/evidence.rs:160` `artifact_facts::remove_exact` → `entry::remove_merge_root_artifact` (`entry.rs:65-72`) | **PLAIN lease** (see `:278`) | **YES — already probing** |

**(2) §5.2-named operations hit by conversion?** **YES.** *"an ORDINARY merge"*
(`finalize.rs:245`, `preserve/artifacts.rs:226`) and *"abort"* (already, via the
last two rows). `gwz commit` only under the broad mutation-guard reading. Note
the v1 forward publication (`finalization/execute.rs:45`) is on the **activated**
lease and is therefore the one clean conversion target in this row.

**(3) Already reaching a `CheckedArtifact`?** **YES** — the v1 root-preserve and
v1 abort arms, today, on the plain lease.

### 7.3 Row `:278` — workspace marker/lock checked mutation | checked workspace artifact action

**This row is already HALF CONVERTED, and the converted half is the abort half.**

Leaves: `gwz.conf/gwz.lock.yml` (`artifact/mod.rs:30`), the boundary text, and
the commit-marker leaf.

| Direction | Writer | Lock / lease | Converted? |
| --- | --- | --- | --- |
| FORWARD — `gwz commit` | `artifact::write_lock` (`handle_commit.rs:200`; `artifact/mod.rs:390-393`) + `write_marker` (`:213`) | mutation guard | **NO — raw** |
| FORWARD — v0 merge publication | `finalize.rs:245,:248` | v0 lane's lock | **NO — raw** |
| FORWARD — v1 merge publication | `v1_lifecycle/finalization/execute.rs:45,:48` | activated lease | **NO — raw** |
| REVERSE — v0 abort | `abort/preflight.rs:409-410` `artifact::write_atomic` (manifest + lock) | `abort/mod.rs:44` `WorkspaceMutatorLock` | **NO — raw** |
| **REVERSE — v1 evidence rollback** | `abort/evidence.rs:148` (Boundary), `:154` (Lock), `:160` (Marker) → `root/artifact_facts.rs:39-89` → `entry::replace_/remove_merge_root_artifact` (`entry.rs:43-82`) → `root_artifact()` (`:159-166`) → `acquire` | **PLAIN `V1MutationLease`** | **YES — already probing** |
| **REVERSE — v1 selected-root metadata rollback** | `root/abort.rs:380` `artifact_facts::write_checked` (`RootMetadataRollbackStepV1`) | **PLAIN lease** | **YES — already probing** |
| **OBSERVE — v1 abort classification** | `abort/evidence.rs:82-83`, `:285`, `:300`, `:314` → `artifact_facts::observe` / `classify_write` / `classify_remove` → the same three doors | **PLAIN lease** | **YES — already probing** |

Reach into the v1 lifecycle, traced: `preflight_v1_evidence` ←
`v1_lifecycle/authority/observe/reverse/rollback.rs:311` and
`…/preservation/entry.rs:87`; `observe_v1_evidence_rollback` ←
`…/reverse/rollback.rs:449`, `…/rollback_prefix.rs:87`,
`…/rolling_back_recovery.rs:54`; `execute_v1_evidence_rollback` ←
`v1_lifecycle/reverse/execute/rollback.rs:62`. All of these run inside
`service::run`'s loop under the lease chosen at `service.rs:116-121`, and the
requests that reach them are `Abort` / `Preserve` → the `_ =>` arm → **plain**.

**THE DECISIVE ANSWER TO QUESTION (3): the capability-free list is ALREADY
narrower than its text.** A v1 abort of a **post-publication** record
constructs `CheckedArtifact`s and runs the durable-identity probe **today**, on
the plain lease. The `RecordRootAmendment` §1b parenthetical is precise about
the boundary of this: it says a **pre-publication** abort constructs no
`CheckedArtifact` (`abort/evidence.rs:39-41`; `rollback_prefix.rs:76-97,155-184`).
The gate is at `abort/evidence.rs:39-45` — `publication.candidate.is_some() &&
publication.composition_commit.is_some() && !publication.evidence_rolled_back`.
Below that gate, no boundary object; above it, the probe.

**WHAT I COULD NOT DETERMINE.** Whether that already-probing post-publication
v1 abort path has ever been driven on a genuinely non-admitted filesystem.
E4.1's evidence drove the **plain lease** returning `Ok` on a real FAT32 volume
(`GwzM5-8R2E-E4.1-Review.md:818`, PR-3), and its §R2.3 table asserts the abort
chain is "capability-free end to end" — but that assertion is about the
**lease**, not about `abort/evidence.rs`'s boundary calls, and I found no
evidence row driving a post-publication v1 abort on such a volume. So I state
it as an **unpinned exposure**, not as a defect: the shipped remedy sentence
(`capability.rs:53-57`, *"`gwz merge --abort`, which needs no such
filesystem"*) may already be false for post-publication v1 records. **This
should be driven before E4.5/E4.6 are chartered, and it is cheap: one row.**

**(2) §5.2-named operations hit by further conversion?** **YES** — *"abort"*
(v0 abort's `preflight.rs:409-410`), *"an ORDINARY merge"* (`finalize.rs:245,248`),
and `gwz commit` under the broad reading. The v1 forward publication is again
the one clean target (activated lease).

### 7.4 Row `:279` — `.git/info/exclude` | checked Git-directory artifact action

**(1) Who writes it, under what.** The ONLY writer is `ensure_workspace_exclude`
(`sync_workspace_boundary.rs:33-45`), publishing via
`crate::artifact::write_atomic(&workspace_exclude_path(root), updated)` at
`:41` — **raw**. Its production callers, with locks:

| Operation | Call site | Lock |
| --- | --- | --- |
| **`gwz repo create`** | `handle_create_repo.rs:51,:211,:336,:426` | `WorkspaceMutatorLock::acquire` (`handle_create_repo.rs:35`) |
| **`init-from-sources`** | `handle_init_from_sources.rs:203` | `WorkspaceMutatorLock::acquire` (`:91`) |
| `gwz materialize` | `handle_materialize.rs:128,:404,:456` | mutation guard (`:27`, `:100`) |
| pull/head member preflight | `pull_head_member_preflight.rs:186,:492` | caller's guard |
| `gwz stage` / `gwz commit` | `handle_stage.rs:94`, `handle_commit.rs:26` (doc + boundary refresh) | mutation guard |
| ORDINARY merge finalize | `merge/finalize.rs:315` (`emitter.artifact_written(".git/info/exclude")`) | v0 lane's lock |

**Converted state: OBSERVE ONLY.** `entry.rs` has
`observe_merge_preservation_git_directory` (`:92-98` → `preservation_git_directory`
`:186-192` → `acquire`, **probing**), reached from
`preservation_root/files.rs:28-35` `observe_boundary` ←
`preservation_root.rs:20,:177,:250,:284` — the v1 preserve path, **plain lease**.
There is **no** `replace_merge_preservation_git_directory` door: `grep -n
"git_directory" src/checked_artifact/entry.rs` returns only `:92`, `:186`,
`:188`, `:336`. **The write half of `:279` is entirely unconverted.**

**(2) §5.2-named operations hit by conversion?** **YES, and this is the worst
row of the four.** `gwz repo create` **and** `init-from-sources` are named
*by name* in §5.2's sentence, and both are the operation that writes this leaf.
Converting `:279`'s write side puts the durable-identity probe directly inside
two operations the frozen decision promises keep working. There is no
lease-splitting escape here at all — neither operation has a v1/v0 fork to
split against; `handle_create_repo.rs:35` and `handle_init_from_sources.rs:91`
take the bare `WorkspaceMutatorLock`.

**(3) Already reaching a `CheckedArtifact`?** **YES on the read side** (v1
preserve's `observe_boundary`, plain lease); **NO on the write side**.

### 7.5 The one-paragraph answer

**The §10 table's remaining conversion set is STRUCTURALLY in tension with
§5.2's capability-free decision, and E4.4 is emphatically not the only row with
plain-lease/no-lease reach — it is the third-worst of the four.** Every
remaining row writes its leaf from at least one operation §5.2 names: `:276`
from an ordinary merge and from `abort --preserve` (`preserve/artifacts.rs:353`;
`v1_lifecycle/reverse/execute/preservation.rs:74`); `:277` from an ordinary
merge and `gwz commit`, whose raw `create_dir_all` at `handle_commit.rs:334`
*is* the row's "when missing" clause; `:278` from an ordinary merge, `gwz
commit` and both aborts (`finalize.rs:245,248`; `handle_commit.rs:200,213`;
`abort/preflight.rs:409-410`); and `:279` — the sharpest — solely from
`gwz repo create` and `init-from-sources` (`handle_create_repo.rs:51,211,336,426`;
`handle_init_from_sources.rs:203`), both named verbatim in the frozen sentence,
with no v1/v0 fork to split against. So completing O1 as written requires
**either amending the capability-free list (a frozen-text move at the amendment
tier, falsifying the shipped remedy sentence at `capability.rs:53-57`) or
carving out most of the table** the way `RecordRootAmendment` §2 carved out one
leaf — the third option, converting only the arms that already hold an activated
lease, leaves every row half-converted with two writers on one leaf, which is
the row `:280` breach. The one genuine relief is that §5.2 already contemplates
*"a checked-artifact action refuses, typed"*, so the question is not whether
checked actions may refuse but whether the named operations may — and on that
the tree has already answered once without a decision being taken: a v1 abort
of a post-publication record **runs the probe today** (`abort/evidence.rs:148-165`
→ `root/artifact_facts.rs` → `entry.rs:159-166` → `observation.rs:96,105`), so
the capability-free list is narrower in the tree than in its text, and nobody
has driven that path on a non-admitted filesystem. **Recommendation: the
capability-free list's disposition should be decided ONCE, at the amendment
tier, for rows `:275`-`:279` together — not four times, once per step charter,
by four builders discovering it at delivery.** E4.4's charter is the right place
to raise it because E4.4 is next, but the decision it needs is the same decision
E4.5 and E4.6 need, and taking it row-by-row is how E4.3's contradiction
happened.

### 7.6 Per-row table, in brief

| Row | Writers today (operation → lock/lease) | Already probing? | §5.2-named operation hit by conversion? | Escape by lease-split? |
| --- | --- | --- | --- | --- |
| `:275` archive (E4.4) | v1 archive via `respond()` from start/resume/**abort**/preserve → **PLAIN lease**; `remove_archive` via GC → **no v1 lease** | no | **YES** — abort, GC | **NO** — one lease only |
| `:276` stash bundles (E4.5) | `gwz stash` → mutation guard (raw); v0 merge preserve → lock (raw); **v1 preserve → PLAIN lease (converted)** | **YES** (v1 preserve) | **YES** — ordinary merge, abort --preserve | partial — v1 creation lease is activated, but preserve is plain |
| `:277` markers (E4.5) | `gwz commit` → guard (raw, incl. the `create_dir_all` bootstrap); v0+v1 publication → raw; **v1 root preserve + v1 abort → PLAIN lease (converted)** | **YES** (v1 preserve, v1 abort) | **YES** — ordinary merge, abort; commit under the broad reading | partial — v1 forward publication is activated |
| `:278` marker/lock (E4.6) | FORWARD raw (`gwz commit`, v0+v1 publication, v0 abort); **REVERSE v1 abort/rollback CONVERTED on the PLAIN lease** | **YES — the abort half already probes** | **YES** — ordinary merge, abort; commit under the broad reading | partial — v1 forward publication is activated |
| `:279` `.git/info/exclude` (E4.6) | `ensure_workspace_exclude` → raw, from **`gwz repo create`** and **`init-from-sources`** (bare `WorkspaceMutatorLock`) + materialize/stage/commit; **observe half converted, PLAIN lease; no write door exists** | **YES on read only** | **YES — two operations named verbatim** | **NO** — no fork exists |

### 7.7 Stated plainly: what I could not determine

1. **Whether the already-probing post-publication v1 abort has been driven on a
   non-admitted filesystem.** E4.1's PR-3 drove the *lease*, not
   `abort/evidence.rs`'s boundary calls (`E4.1-Review.md:818`). Unpinned.
2. **Whether §5.2's "the mutation guard" protects the lock acquisition (narrow)
   or the operations under it (broad).** Ground 1 supports narrow, Ground 2's
   "every mutation" purpose supports broad; the decision sentence does not say.
   This changes the verdict for `gwz commit`/`stage`/`materialize`/`stash` on
   rows `:277`/`:278`/`:279`.
3. **Whether `preserve/artifacts.rs:353`'s bundle write is reachable from a v0
   merge in a shipped configuration** — I confirmed the call site and that it is
   raw, but did not trace its full v0/v1 gating. Treat the `:276` "ordinary
   merge" hit as probable, not proven.
4. **Row `:277`/`:278` overlap.** The commit-marker leaf appears in both rows'
   surfaces (`:277` names the parent "when missing", `:278` names the "checked
   workspace artifact action"). I did not attempt to adjudicate the boundary
   between them; E4.5's and E4.6's charters will have to.
