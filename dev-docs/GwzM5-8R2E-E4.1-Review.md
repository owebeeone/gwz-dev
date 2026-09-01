# GwzM5-8 R2-E Phase E4 Step E4.1 — single-axis peer-blind review (Opus)

**Object.** Branch `e4/e4-1-activation`, two commits on gwz-core `main` `027da5b`:
(a) `7f2862d1bbfd80a76c0a8f673b7849df2b1377af` — "E4.1(a): the catalog-free
hygiene riders"; (b) `1f3ba71fa7621183dd3d5af66636d72e6475523c` — "E4.1(b): the
first production catalog activation".

**Reviewer.** Opus, single axis, peer-blind. **Date:** 2026-09-01.

**Method.** Read-only outside the review worktree. Every claim re-derived in the
tree; every companion recomputed with the checker's own algorithm; every pin
red-probed; every count measured. Worktree left byte-clean at (b)
(`git status --porcelain` empty, HEAD `1f3ba71`).

**Environment.** darwin 25.6.0 / APFS, cargo 1.95.0, rustc 1.95.0,
`CARGO_TARGET_DIR` in scratch, boundary checker under
`/opt/homebrew/bin/python3.13`.

**Authorities consulted.** `GwzM5-8R2E-Plan.md` (§1.1 ledger — O2, O11, O12,
O13; Phase E4 section and its GATE LIFTED note's four riders; the E7.2 scope
clauses); `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §5.2/§5.3;
`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §5.3-item-6 cure (`:1534-1548`),
§6.4, §7.5, §7.6/§7.6.1/§7.6.2, §7.7; `GwzM5-8R2E-E7-Acceptance.md` §4's O12 row
and §5's carrier-table nit-1 row; `GwzM5-8R2E-E7-ReviewCode.md` [P3 F2], [P3 F3],
Q1; `GwzM5-8R2F-RelocationPlan.md` §1; the R1.2 tripwire
`src/checked_artifact/interface_tests/catalog_activation_pin.rs`.

---

## 1. VERDICT

# NO-GO

**One [P1] and one [P2].** Both are in commit (b). Commit (a) is clean and would
be a GO on its own.

> **ESCALATION FIRES.** The lane's rule — any P0/P1/P2 in a single-axis interior
> verdict triggers the Fable escalation — is triggered by **[P1-1]** and
> **[P2-1]**. Stating it explicitly as required.

The load-bearing judgment — *where* the activation goes — is **correct in five of
its six grounds and wrong in the sixth**, and the sixth is the one that decides
whether the refusal lands on the checked feature and nothing else. It does not.
`V1MutationLease::acquire` is reached from an **ordinary** (non-`--no-ff`) merge
through the A1 adaptation path, **after** that path has already committed a
durable v0→v1 record upgrade. On a filesystem the catalog cannot use, that
sequence leaves an ordinary merge permanently unrecoverable, with a remedy
sentence telling the user to do the thing they already did.

This is not a fold-in condition. Curing it is a choice between three
semantics-level options (activate at first catalog *consumption*; probe before
the adaptation upgrade; or make the v1 recovery commands tolerate an
unactivatable catalog), and E0.2b §5.3 item 6 exists precisely because that
choice was deferred to E4-time. It belongs to the step owner and the lane owner,
not to a reviewer's condition list.

Everything else in the package is of a high standard, and much of it is
exemplary — see §8.

---

## 2. FINDINGS

### [P1-1] The activation refusal sits *behind* a durable v0→v1 record upgrade, wedging an ordinary merge with no in-code exit

**Ground 5, as the builder states it, is false.** `v1_lifecycle/checked.rs:140-142`
says: *"`--no-ff` is the only route into v1 at this writer floor
(`model/version.rs`), so a filesystem without durable identity refuses exactly the
checked feature and nothing else."* `capability.rs:44-46` repeats it:
*"the blast radius of `PersistentFilesystemIdentity` is exactly the checked
`--no-ff` v1 path."*

That is true of **starts** (`merge/model/version.rs:39` `ACTIVE_WRITER_FLOOR =
V0`; `NoFf` selects V1 through the `max`; verified). It is **false of the record
already on disk**. Two production routes reach `V1MutationLease::acquire` without
a `--no-ff` start:

1. **`runtime/dispatch.rs:330`** — any `Resume`/`Abort` on an open record whose
   envelope is already v1 goes straight to `v1.command` →
   `start.rs:158/162` → `service::run` → `V1MutationLease::acquire`.
2. **`runtime/dispatch.rs:333` → `adapt_before_mutating` (`dispatch.rs:394-416`)**
   — the **A1 adaptation** path. For a `Resume`/`Abort` on an open **v0** record
   whose `AdaptationPrecheck` is `MayAdapt` (`store/mod.rs:255-262`: purely
   `state == Finalizing && mode == Normal`, i.e. any interruption during an
   *ordinary* merge's finalization), `upgrade_open_v0`
   (`store/atomic_upgrade.rs:57-99`) **durably rewrites** `.gwz/merge/<id>.yaml`
   from the v0 to the v1 envelope (`publish_prepared`: staged temp, `sync_all`,
   atomic rename), returns `Ok(true)`, and only *then* does the command route to
   the v1 lifecycle and hit the new refusal.

**DRIVEN, on the candidate tree at (b).** Probe appended to
`src/workspace_ops/tests/g23/a1_activation.rs`, reusing that file's own
fixtures (`init_one_member_workspace`, `FaultingMergeStore::new(
FinalizationFault::AfterEnteringFinalizing)`, `handle_merge`) — the same shape
its `an_eligible_row_completes_under_v0_when_its_atomic_upgrade_fails` control
arm uses. Obstruction: a foreign file at `.gwz/catalog-final` (any cause that
makes `activate_workspace_catalog` return `Err` reaches the identical site in the
identical order):

```
PROBE pre-state envelope = ("gwz.merge-operation/v0", 0)
PROBE pre-state version=V0 adaptation=MayAdapt mode-was-ordinary
PROBE catalog present before resume: false
PROBE resume #1 = Err(ModelError { code: IoError, message: "checked merge artifact catalog
    rejected catalog recovery roles: reserved role kind, bytes, or recovery attempt is not
    uniquely owned", ... })
PROBE envelope AFTER refused resume = ("gwz.merge-operation/v1", 1)
PROBE classify AFTER refused resume = Some(OpenRecordEnvelope { ..., version: V1,
    adaptation: Skip })
PROBE resume #2 = Err(... same refusal ...)
PROBE abort   = Err(... same refusal ...)
```

The record is durably v1; `adaptation` is now `Skip`; every subsequent command
takes `dispatch.rs:330` and refuses. **There is no in-code exit.**

**CAUSALLY ISOLATED to this commit's one production line.** With
`crate::checked_artifact::entry::activate_workspace_catalog(guard.catalog_mutation_lease())?;`
(`v1_lifecycle/checked.rs:157`) deleted and *nothing else changed*, the identical
scenario runs to completion:

```
PROBE (activation line removed) resume   = Ok(Completed)
PROBE (activation line removed) envelope = ("gwz.merge-operation/v1", 1)
```

**COMPOSED WITH THE REAL CAPABILITY REFUSAL.** Driven on a FAT32 volume
(`hdiutil create -fs "MS-DOS FAT32"`, attached at `/Volumes/GWZPROBE`, detached
and deleted afterwards), pointing `V1MutationLease::acquire_for_test` at a
git-initialised directory on it:

```
PROBE REFUSAL: code=UnsupportedOperation message=checked merge artifact catalog: this
filesystem does not expose the persistent file handles and mount identity that checked
merge artifacts require; run the workspace on a filesystem that does (local ext4 on Linux,
APFS or HFS+ on macOS, NTFS on Windows), or start the merge without --no-ff
(detail: filesystem does not promise persistent object identities)
```

The two transcripts compose: the refusal site is the same
(`activate_workspace_catalog` inside `V1MutationLease::acquire`), and the
adaptation path reaches it after the upgrade has committed.

**Reachability is not exotic.** The catalog's Linux provider admits **only ext4**
(`pre_catalog/provider/platform/linux.rs:143-148`, `require_ext4`), so btrfs, xfs
and zfs — Fedora's and openSUSE's defaults — are refused, as are tmpfs, overlay
and every network mount, on every platform. On such a workspace: an ordinary
merge works; it is interrupted during finalization (power loss, kill, crash);
`gwz merge --resume` upgrades the record and then refuses; the workspace is
bricked for that merge, permanently, and `--abort` cannot clear it.

**What this falsifies, precisely.**

| Authority | Text | Status on this tree |
| --- | --- | --- |
| E0.2b §5.3 **item 6** (`:1536-1541`) | "The refusal is proved to occur **before the operation's first durable mutation**, **or** the partially-mutated state it leaves is proved **convergent on restart**" | **Both disjuncts fail on this path.** The refusal follows a durable record rewrite, and the state left behind is not convergent — restart re-refuses forever. |
| E0.2 §5.2 **Ground 3** | fail-closed "does not mean *refuse operations that need no proof*"; cites the Step-4.2 shape that "bricked P5 … with no in-code exit" as the class the freeze rejects | The package re-creates that class. |
| `dispatch.rs:384-393` (the tree's own **[P1-1]** doctrine, quoted above `adapt_before_mutating`) | "Surfacing that refusal as the resume outcome would turn currently-recoverable states into permanent wedges" | Violated — by a refusal introduced downstream of the adapter rather than inside it. |
| precondition 3's runbook (`docs/OperationModel.md`) | "an ordinary or `--ff-only` merge, merge abort … keep working unchanged" | False. |
| precondition 4's release-notes line (commit (b)'s message) | "Ordinary and `--ff-only` merges … merge abort … are unaffected" | False. |

**Note on precondition 6's driven rows.** They are genuinely driven (see §4 row
6) — but arm 1 (`v1_lifecycle::tests::checked::
the_v1_prologue_refuses_an_unactivatable_catalog_before_any_durable_mutation`)
proves the ordering only for a **fresh start**, where nothing has yet been
written. It cannot see the adaptation path, because that path's first durable
mutation happens in `store/atomic_upgrade.rs`, one dispatch layer above the
partition the row lives in. The row is honest; its scope is narrower than the
precondition's sentence.

---

### [P2-1] The runbook and the release-notes line state a scope the tree does not have

Separable from [P1-1] because these two artifacts *are* preconditions 3 and 4,
they will be read by users and by the release train, and they must be corrected
even if the wedge is cured by re-ordering rather than by narrowing the blast
radius.

`docs/OperationModel.md` ("**The refusal is scoped to the checked feature.**
`gwz repo create`, `init-from-sources`, an ordinary or `--ff-only` merge, merge
abort, GC and the workspace mutation guard all take the same workspace mutator
lock and none of them asks for a durable identity, so they keep working unchanged
on a filesystem that cannot answer") and the release-notes line in commit (b)'s
message assert a scope that is **site-true and behaviour-false**.

Site-true: the six lock sites named in E0.2 §5.2's decision — `handle_create_repo.rs:35`,
`handle_init_from_sources.rs:91`, `merge/abort/mod.rs:44`,
`merge/continue_op/coordinator.rs:13`, `merge/gc.rs:167`,
`merge/runtime/mutation_guard.rs:44` — are untouched. I enumerated all nine
production `WorkspaceMutatorLock::acquire`/`try_acquire` sites and confirmed the
count and the membership.

Behaviour-false, three ways:

1. **`gwz merge --abort` on an open v1 record refuses.** `handle_v1_command`
   (`v1_lifecycle/start.rs:148-167`) routes `Abort` into `service::run`, which
   takes `V1MutationLease::acquire`. A `--no-ff` merge started on a capable
   filesystem and later reached from an incapable one (container overlay mount,
   the workspace remounted over NFS, a kernel downgrade below
   `FS_IOC_GETFSUUID`) cannot be aborted. The docs say abort is unaffected.
2. **`gwz merge --resume`/`--abort` on an *ordinary* merge refuses**, per
   [P1-1].
3. **The Linux admitted set is ext4 only.** The runbook's refused-family sentence
   leads with "network filesystems …, overlay and container filesystems, and
   `tmpfs`" and puts the decisive fact — "on Linux the admitted local filesystem
   is `ext4`" — in a trailing subordinate clause. A btrfs or xfs user reading the
   families list will not conclude that `--no-ff` is refused on their machine. It
   is.

The `--ff-only` and `Status` and `Gc` claims **are** true and I verified them:
`Status` routes to `status::open_status` and never takes the lease; `Gc` on an
open record refuses before the lease; `service::run`'s `V1NextAction::Respond`
arm (`service.rs:101-110`) returns before `V1MutationLease::acquire`.

---

### [P3] findings

| # | Finding | Evidence | Carrier |
| --- | --- | --- | --- |
| **[P3-1]** | **Precondition 4 has no in-tree carrier.** gwz-core stages no release notes and has no CHANGELOG; `RELEASE.md` is a *process* document and `scripts/release.py` reads no commit messages. The verbatim line therefore lives only in commit (b)'s message and this report — nothing binds the release train to it. Grade it a **residual with a named carrier**, not a discharged precondition. It also must not ship as written ([P2-1]). | `RELEASE.md` read in full; `ls` of repo root and `docs/`. | the release train; record on the E7 ledger. |
| **[P3-2]** | **Precondition 1's rendering is not driven in-suite**, and the stated reason is not the binding constraint. Flag 7 says driving it would need an injection key and would move the frozen 165-key census. It would not: a direct-constructor row — `CheckedFsError::unsupported(PlatformCapability::PersistentFilesystemIdentity, "…")` pushed through the door's renderer — costs ~10 lines, needs no key, and moves no census. `contracts.rs:130-136` already constructs `CheckedFsError::unsupported` for exactly this purpose. The sentence **is** correct today (I drove it end-to-end, transcript in [P1-1]); what is missing is the guard. Blocked only by the renderer being inline in `activate_workspace_catalog` rather than a named fn. | the FAT32 transcript; `entry.rs:302-321`. | E4.2. |
| **[P3-3]** | **The legacy cure is Linux/macOS only; `identity.rs`'s Windows arm still surfaces a bare errno.** `identity.rs:550` and `:562` return `std::io::Error::last_os_error()` for `GetFileInformationByHandleEx(FileIdInfo)` / `GetFinalPathNameByHandleW` failure — the very calls whose capability-gap codes the catalog's own Windows provider downgrades (`windows.rs`'s `query_error`: `Some(1 \| 50 \| 87)`). `persistent_identity_error` is `#[cfg(any(linux, macos))]` with no Windows arm, while the module doc at `identity.rs:470-479` claims unqualified that "A substrate gap therefore carries `capability::PERSISTENT_FILESYSTEM_IDENTITY_REMEDY` rather than a bare `errno`". Outside precondition 1's literal text (which cites the Linux sites), so a doc/consistency nit rather than a missed obligation — but the doc overstates. | grep of `last_os_error` / `ErrorKind::Unsupported` in `identity.rs`; the three `query_error` bodies read side by side. | E4.2, or the Windows dispatch. |
| **[P3-4]** | **The lease-reference and forbidden-provisional scans are blind at `entry.rs`** (flag 4, confirmed). `check_checked_artifact_boundaries.py:1267-1270` skips `checked_artifact/entry.rs` when building `masked_sources`, so entry.rs's new `use super::bootstrap::CatalogMutationLeaseV1` moved **no** `CATALOG_LEASE_REFERENCE_SETS` row (`CatalogMutationLeaseV1` **is** a key — verified), and a reintroduced `FORBIDDEN_PROVISIONAL_CATALOG_INTERFACES` spelling in entry.rs would not fire either. Mitigated, strongly: entry.rs is byte-pinned in `PROTECTED_SOURCE_DIGESTS` and its four `ENTRY_*` inventories are **equality** checks, so no edit to entry.rs can pass unreviewed. | script read; key list extracted programmatically. | E4.2 (record as a known scan hole). |
| **[P3-5]** | **The stale `#[allow]` at `operation/workspace_mutator_lock.rs:44-47`** ("R2-C0 freezes the checked catalog borrow before the C1 owner consumes it") now describes a consumed borrow. **Leaving it was right** — precondition 2 requires the file byte-identical, and it is (below). It is silent (an unnecessary `allow` warns nothing; clippy is clean). But the staleness is recorded only in commit (b)'s message and a checker comment, on no ledger row with an owner. | sha256 comparison; clippy exit 0. | E4.7's "A1-era allowances expire with dated annotations" row — same class as `catalog.rs:10-16`. |
| **[P3-6]** | **Record nits.** (i) Commit (b)'s message says "Cap: **469** whole added lines"; the tree measures **472** (`git show --numstat`, and my own whole-line count; still inside 500). (ii) The same message says "**six** inventory moves … (the four `ENTRY_*` inventories, `catalog_mutation_lease`'s reference set)" — that enumerates five. (iii) `capability.rs:44` and `v1_lifecycle/checked.rs:141` cite "`model/version.rs`"; the file is `workspace_ops/merge/model/version.rs` — fine inside `merge/`, loose from `checked_artifact/`. | §5, §6. | the delivery record. |
| **[P3-7]** | **Landing-dispatch risk on the DERIVED Linux/Windows counts.** The `+2` and `+1` arithmetic is right and I cfg-audited every new row (five rows, none carries a `cfg` gate; their modules are `cfg(test)`-only with no platform gate). The *new* risk is different: E4.1 makes the whole `v1_lifecycle::` partition (257) and a large slice of `workspace_ops::tests` depend on the **dispatch filesystem** satisfying the catalog provider — ext4 + `FS_IOC_GETFSUUID` (kernel ≥ 6.8) on Linux; local APFS/HFS+ on macOS; NTFS on Windows. Previously only the `checked_artifact::` catalog suites did. A dispatch whose temp dir is tmpfs or overlay now fails broadly and confusingly. De-risked by prior evidence: the existing Linux `checked_artifact::` 462 already includes real `recover_or_create` drives against the real filesystem, so the environment must already be capable. Named so the dispatch reads it as a *diagnosis*, not a mystery. | provenance blocks; `require_ext4`; the fixtures' `std::env::temp_dir()`. | the landing dispatch. |
| **[P3-8]** | **A "rejects without mutation" operation now durably creates `.gwz/catalog-final`** (flag 5's residual, and it is genuinely a residual, not a defect — see §5 flag 5). The five `reverse_rollback` rows can no longer witness catalog-side mutation across a refusal, and a `--no-ff` refused for drift leaves a new catalog tree behind that the workspace did not have. Disclosed at the site and in the message. | the exclusion-removal probe, §5 flag 5. | E4.2-E4.6 (as consumers arrive). |

---

## 3. THE ACTIVATION SITE — the six grounds, verified one by one

The builder chose `V1MutationLease::acquire` (`v1_lifecycle/checked.rs:154-162`)
calling a new door `activate_workspace_catalog` (`checked_artifact/entry.rs:302`).

| # | Ground as stated | Verdict | What I found in the tree |
| --- | --- | --- | --- |
| 1 | Visibility forces the door inside `checked_artifact` | **HOLDS** | `catalog/bootstrap.rs:233` is `pub(in crate::checked_artifact) fn recover_or_create(`, re-exported at `catalog.rs:48-50`. A caller can only live under `src/checked_artifact/`, which is also exactly the tripwire's scan root — so the pin is truthful only if the namer is there, as the commit argues. |
| 2 | The lease is reachable only via a **held** `WorkspaceMutatorLock`, so §5.2's inside-an-operation property holds by construction | **HOLDS** | `CatalogMutationLeaseV1` is produced only by `WorkspaceMutatorLock::catalog_mutation_lease(&self)` (`operation/workspace_mutator_lock.rs:48-52`), which borrows `&self` off a held guard. `checked.rs:156-157` acquires then activates; the borrow ends before the guard moves into `Self`. |
| 3 | §5.2's frozen text keeps the other eight lock sites capability-free | **HOLDS, with one imprecision** | Nine production sites, enumerated and matched to §5.2 Ground 2's list exactly. Six are kept capability-free as claimed. But the remaining three are **not** "the v1 lifecycle's, and this one is the prologue both take" — `v1_lifecycle/archive.rs:97` and `:126` are also v1_lifecycle sites taking the lock *without* activating. Benign at this step (`:97` is a read-only destination-only serialisation point; `gc_archived` at `:126` has **zero** production callers, confirmed by grep, and carries an explicit `#[allow(dead_code)]` saying so), and `archive.rs`'s conversion is E4.4's row — but the sentence as written elides two of the three. |
| 4 | Both v1 entries share this prologue | **HOLDS** | `V1MutationLease::acquire` has exactly two production callers: `start.rs:81` (creation lease) and `service.rs:112` (mutation loop). Every other caller is `acquire_for_test` under `tests/`. |
| 5 | `ACTIVE_WRITER_FLOOR = V0` + `NoFf → V1` makes `--no-ff` the only v1 route | **FAILS — see [P1-1]** | The floor and the `max` are exactly as claimed (`merge/model/version.rs:39`, `:92`; the existing `no_ff_selects_v1_and_ordinary_follows_the_active_floor` row). The claim is true of **starts**. It is false of the record on disk: `dispatch.rs:330` and `dispatch.rs:333`+`adapt_before_mutating` both reach the v1 lifecycle from a non-`--no-ff` origin. |
| 6 | Probe-verified against real workspaces | **HOLDS** | Reproduced independently: 456 `checked_artifact::`, 257 `v1_lifecycle::`, 1097+1 remainder, all green on real fixtures on APFS; and I drove the real capability refusal on a real FAT32 volume. |

### Adversarial questions the mandate poses

**Is there a production path into v1 mutation that BYPASSES this prologue?**
No — `V1MutationLease::acquire` is the only door into v1 record mutation, and its
two callers are the only production entries. `archive.rs:97` takes the lock but
performs no v1 store mutation on that branch; `archive.rs:126`'s `gc_archived` is
production-unreachable. **The problem is the opposite one**: a path that reaches
the prologue from outside the checked feature, and does durable work first
([P1-1]).

**Is there a non-v1 path that now activates the catalog (scope creep)?**
No. `activate_workspace_catalog` has exactly one caller, pinned by
`ENTRY_REFERENCES["activate_workspace_catalog"] =
{"workspace_ops/merge/v1_lifecycle/checked.rs"}` — an **equality** check.
`catalog_mutation_lease`'s reference set gains exactly that one file. Ordinary
merge, `repo create`, `init-from-sources`, GC and the mutation guard remain
catalog-free and capability-free at their own sites.

**Does activation change behavior for a v0 workspace that never uses `--no-ff`?**
**Yes** — but only through the adaptation path, and that is [P1-1]. A v0
workspace whose merges all complete normally is unaffected: `handle_start_durable_v1`
is entered only when `select_record_version(...) == V1`
(`merge/start.rs:95-108`), and `v1.command` only when `open.version == V1` or
`adapt` returns true.

---

## 4. THE SEVEN PRECONDITIONS — one verdict each

| # | Precondition | Verdict | Evidence / probe |
| --- | --- | --- | --- |
| **0** | Coexistence gate; tripwire `PRODUCTION_CALLER_COUNT` 0→1 in the same commit as the caller, dated comment updated | **MET** | Pin moved to `1` in commit (b), same commit as `entry.rs`'s caller. Comment carries both dated entries (R2-F R1.2 ZERO; R2-E E4.1 ONE), names the caller and the call site, and states that a second namer must move it again. **RED-PROBED**: a second production namer added at `checked_artifact/policy.rs` → `left: 2, right: 1`, message `are ["entry.rs", "policy.rs"], which is not 1`. Reverted. The anti-vacuity anchor (`OWNER_CALL_SHAPED = 3`) is exact: `catalog/bootstrap.rs` holds precisely three `recover_or_create(` occurrences (:233 free fn, :240 method, :236 delegation); `catalog.rs`'s `use` is not call-shaped. |
| **1** | Typed refusal naming the capability; both raw-errno halves surface the actionable sentence | **MET on the activation path; residual on the guard and on Windows** | New `PlatformCapability::PersistentFilesystemIdentity`, distinct from the identity-VALUE contract. Retokenization is **complete**: `PlatformCapability::DurableObjectIdentity` now appears in production at `capability/durable_identity.rs` **only** (3 uses — the three constructors, exactly as the new doc says); all 20 production substrate refusals across the four providers moved. Error propagation verified end to end — `begin_preflight` → `try_new` → `revalidate` (metadata only) then `facts()` → `platform.dir_identity(...)?`, and every hop from there to `activate_workspace_catalog` is a bare `?` on `Result<_, CheckedFsError>`, so the `Some(remedy)` arm is reachable and no arm re-wraps. **DRIVEN by me on a real FAT32 volume** — transcript in [P1-1]. Legacy half: `persistent_identity_error`'s downgrade allowlists are **byte-identical** to the catalog providers' `query_error` (linux `[EOPNOTSUPP, ENOSYS, ENOTTY, EINVAL]`; macos `[ENOTSUP, EINVAL, ENOTTY]`), and `observation.rs:376-380` renders the cause verbatim, so the legacy sentence is actionable too. Residuals: **[P3-2]** (no in-suite guard) and **[P3-3]** (Windows). |
| **2** | Refusal SCOPED: no probe at `try_acquire`; `workspace_mutator_lock.rs` byte-identical to base | **MET — byte-exact** | `git rev-parse 027da5b:src/operation/workspace_mutator_lock.rs` == `git rev-parse HEAD:...` == `84a04789…`; sha256 of both = `0d9b034edab7e66a5e83b4bc86b7325afa001666220a07e428d4fa73f9384e28`, which is **exactly** the value E0.2b §6.4 banks. `git diff 027da5b..HEAD -- src/operation/` is empty. `bootstrap/runtime/mod.rs`'s digest is unmoved at both commits (`1bddf4b4…`), so `try_acquire`'s path is untouched; the probe happens after `WorkspaceMutatorLock::acquire` returns. |
| **3** | Runbook class in `docs/OperationModel.md` + `WorkspaceArtifacts.md` cross-ref: does it name filesystem families and the workaround? | **MET on its literal terms; CONTAMINATED by [P2-1]** | Families: "network filesystems (NFS, SMB/CIFS, SSHFS and other FUSE mounts), overlay and container filesystems, and `tmpfs`; on Linux the admitted local filesystem is `ext4`" ✓. Workaround: "run the workspace on a filesystem that exposes persistent handles, or start the merge without `--no-ff`" ✓. Cross-ref anchor `OperationModel.md#checked-merge-artifacts-and-filesystem-identity` matches the new `##` heading ✓. **But** the "refusal is scoped to the checked feature" paragraph is false ([P2-1]), and the workaround is not a workaround for a merge already open. |
| **4** | Release-notes line recorded for the release train | **RESIDUAL, not discharged** | The line is recorded verbatim at the end of commit (b)'s message. gwz-core genuinely has nowhere to stage it (no CHANGELOG; `RELEASE.md` is process-only; `scripts/release.py` reads no messages) — so the builder's disposition is the best available, but "recorded in a commit message" is not "a release-notes line for the release". Grade **[P3-1]**, residual with a named carrier. And it must not ship as written ([P2-1]). |
| **5** | `platform/unsupported.rs` claim swept | **MET, as a defensible minimal reading** | The value is unchanged (`LinuxExt4FsIocGetFsUuidV1`) but is now named `UNREACHABLE_PROFILE` with the unreachability argued, and the file's refusal carries the new capability. **I verified the ordering argument in the tree, and extended it**: `witness.rs:49-57` calls `revalidate()?` then `platform.dir_identity(...)?` at `:53`/`:54` *before* the struct literal reads `support_profile()` at `:57`, so on that platform `?` returns first. The builder names only `witness.rs`; I checked **every other** `platform.support_profile()` reader — `target.rs:144` and `:220`, `association.rs:149` and `:181`, `filesystem/bound.rs:117` — and each is preceded in its own function by a `platform.dir_identity(...)?`. All four `unsupported.rs` probes (`dir_identity`, `file_identity`, `parent_mode`, `rename_domain`) refuse. The argument holds. Residual, stated: the constant is still literally the Linux variant, the module is compiled on no supported target, and nothing pins the unreachability. The alternatives — a new `SupportedFilesystemProfile` variant (moves a persisted protocol enum) or a fallible `support_profile` (a trait-signature change across all providers) — are both heavier than E4.1's charter. Minimal reading accepted. |
| **6** | Refusal-ordering rows DRIVEN, not asserted | **Rows DRIVE; the precondition is NOT discharged on the adaptation path — see [P1-1]** | **Arm 1** (`v1_lifecycle/tests/checked.rs:31-67`): drives the real prologue via `V1MutationLease::acquire_for_test` (a `#[cfg(test)]` alias that is literally `Self::acquire`) against a real un-bootstrappable target, and asserts the merge store absent and the obstruction byte-unchanged. **Genuinely driven**, not asserted. **Arm 2** (`catalog/bootstrap/tests.rs:693-728`): real house fault injection (`run_next_catalog_fault(Fault::CatalogBootstrapScratchWrite, panic)`), `catch_unwind`, then restart through the **production door**. **Genuinely driven** — it is the house idiom, character-for-character the shape of the existing `resume_after_scratch_interrupt_reissues_the_root_barrier_before_completion` at `:455`, redirected through `entry::activate_workspace_catalog`. Both rows run green (I ran them). **But** the precondition's sentence is a disjunction over *the operation*, and on the A1 adaptation path **neither disjunct holds** — the refusal follows a durable record rewrite and the residue is not convergent. Arm 1 cannot see this because the first durable mutation happens one dispatch layer above its partition. |

---

## 5. THE BUILDER'S ELEVEN FLAGS — disposition table

| # | Flag (builder's numbering) | Disposition |
| --- | --- | --- |
| **1** | Authority mis-citation: E0.2 §5.2 Ground 1 / E0.1(a) cite `identity.rs:267`/`:304-316`, but the catalog-lease probe actually reaches `catalog_lease/witness.rs:49-67` → `HostPlatform::dir_identity`/`rename_domain` in the pre_catalog providers | **CONFIRMED — the builder is right, and this decides precondition 1's discharge.** `witness.rs:50` binds `let platform = HostPlatform;` and `:53`/`:54`/`:60`/`:66-68` call `platform.dir_identity` / `rename_domain`, which dispatch through `provider/platform.rs:33-53` to the `cfg`-selected `imp` — **not** to `checked_artifact/identity.rs`. Same syscalls, different module, and critically a different error type (`CheckedFsError` vs `std::io::Error`). E0.2 §5.2 Ground 1's cite is a mechanism mis-attribution. Curing **both** modules was correct: the providers are the path that actually refuses under activation (precondition 1's discharge), and `identity.rs` is the path §5.3 item 1's own text names. **Record correction owed to the corpus**: §5.2 Ground 1's "which reach `name_to_handle_at` (`identity.rs:267`) and the statx `MNT_ID` rename domain (`identity.rs:304-316`)" should read the pre_catalog providers. |
| **2** | O11's CLOSED NEGATIVE claimed too much: legacy identity is production-reachable at v0.11.0 | **CONFIRMED — a RECORD correction to grade, not a code defect.** `observation.rs:96` calls `durable_identity(&root, &label)` — `identity::object_identity` — **unconditionally**, at the top of every `CheckedArtifact::observe`. Those observers have real production callers, enumerated by the checker's own `ENTRY_REFERENCES`: `workspace_ops/merge/root/artifact_facts.rs`, `git/gitbackend/preservation_root/files.rs`, `workspace_ops/merge/preserve/checked_bundle.rs` — ordinary merge paths. So at v0.11.0 a Linux filesystem without `name_to_handle_at` already fails an **ordinary** merge, with a raw errno. Plan §1.1's "**O11 — CLOSED NEGATIVE at E0.1** (no v0.11.0 production path reaches the identity probe)" is **wrong**. Grade: the ledger row needs a dated correction; the builder's `identity.rs` cure is what the corrected record would have demanded anyway, so no code consequence. Note the asymmetry that matters for [P2-1]: the legacy probe needs `name_to_handle_at` (works on btrfs/xfs); the catalog provider needs **ext4** — a strictly narrower admitted set. |
| **3** | `workspace_mutator_lock.rs:44-47`'s now-stale `#[allow]` left in place because byte-pinned | **CONCUR — leaving it was right.** Precondition 2's evidence is strongest when the digest is byte-identical to base, and it is (§4 row 2). An unnecessary `#[allow]` is silent (unlike `#[expect]`); clippy is clean. **Loudly flagged? Partly** — the commit message says so, and the checker's `CATALOG_LEASE_REFERENCE_SETS` comment records that the accessor gained its first production caller, but nothing at the site says the reason is stale and no ledger row owns it. **[P3-5]**, carrier E4.7. |
| **4** | `CATALOG_LEASE_REFERENCE_SETS`'s `masked_sources` skips `entry.rs`, so the boundary module moved no lease-reference pin | **CONFIRMED, severity P3.** `check_checked_artifact_boundaries.py:1267-1270` skips `checked_artifact/entry.rs` outright; `CatalogMutationLeaseV1` **is** a key in the set (I extracted the key list programmatically). `FORBIDDEN_PROVISIONAL_CATALOG_INTERFACES` shares the same blind spot. Mitigated to P3 by entry.rs being byte-pinned **and** having four equality-checked `ENTRY_*` inventories — it is the most-pinned file in the subsystem. **[P3-4]**. |
| **5** | **THE REVIEWABLE EDGE** — five `reverse_rollback` snapshot rows gained `.gwz/catalog-final` in their exclusion list on the `.gwz/locks` precedent | **AUDITED. The exclusion is NECESSARY, does not weaken what the rows prove, and leaves a disclosed residual.** The rows are five test fns over seven `assert_entry_rejected_without_mutation` call sites (`entry_service.rs:28/37/56`, `entry_service_drift.rs:21/40/54`), sharing one `collect_files` helper — so the diff is one edit, not five. **PROBED**: with the `.gwz/catalog-final` line removed, all four reachable rows **FAIL**. I extracted the diff programmatically: in every failing row the `before` snapshot holds **no** catalog path at all and the `after` snapshot holds the five freshly-created infrastructure files, while the **non-catalog file sets are byte-identical** (`gwz.conf/gwz.lock.yml`, `gwz.conf/gwz.yml`, the marker, `members/a/README.md`, `selected-root.txt`, `staged-drift.txt` — same on both sides in all three captured rows). So the exclusion covers **creation by the prologue**, exactly the `.gwz/locks` precedent, and nothing else. **Could catalog-side mutation now hide inside a "rejects without mutation" assertion? Yes, literally — but nothing else can**: `root_files` is one of nine fields, and `record` / `root_head` / `root_index` / `root_repository_state` / `root_branch` / `root_stashes` / `members` / `bundle` all still compare. What those rows prove is unchanged. Residual **[P3-8]**, disclosed at the site and in the message. |
| **6** | Precondition-5 interpretation | **ACCEPTED** — see §4 row 5. The unreachability argument is sound and I verified it more widely than the builder stated it. |
| **7** | The refusal sentence is not driven end-to-end (no injection key; minting one would move the frozen 165-key census) | **PARTLY REJECTED. Precondition 1 STANDS — I drove the sentence end-to-end myself on a real FAT32 volume (transcript in [P1-1]) — but the builder's stated reason for not driving it is not the binding constraint.** No injection key is needed and no census would move: a direct-constructor row over the same match arms would do it in ~10 lines, and `contracts.rs:130-136` already builds `CheckedFsError::unsupported` for a neighbouring pin. The real obstacle is that the renderer is inline in `activate_workspace_catalog` rather than a named fn. Grade **[P3-2]**, residual with named carrier E4.2 — *not* an accepted impossibility. |
| **8** | nit-1's companion is a source-shape pin, not a behavioral row | **CONCUR — Q1 does not demand a driven row.** The E7 Acceptance §4 O12 row and ReviewCode Q1 specify a *shape* (signature unchanged, cap source, mechanics, refusal vocabulary) and describe the blast radius as "one function body in `observation.rs` (~10 lines), no signature drift, no wire/census/inventory movement, one protected-tree digest" — no test is required. The builder's own reason (the take-cap is observable only in a grow-between-fstat-and-read window with no injection key) is correct on its face here, unlike flag 7. **RED-PROBED**: reverting `observe_leaf_exact` to the bare `Vec::new()` + `read_to_end` shape fails the pin at `capability_permit.rs:483`. Minor: the first three assertions carry no message, only the negative one does. |
| **9** | Beyond-charter scope: ~21 substrate refusals retokenized across four providers; legacy cure extended to macOS/Windows/no-provider arms; docs cross-ref | **NOT scope creep for the retokenization — it is LOAD-BEARING, as claimed.** Without it, `capability.remedy()` returns `None` on the exact path E4.1 activates, and the user reads "checked merge artifact catalog is unsupported: only local ext4 with FS_IOC_GETFSUUID is admitted" — precondition 1 unmet on the activation path. Completeness verified: after the change, `PlatformCapability::DurableObjectIdentity` survives in production **only** at `capability/durable_identity.rs` (3 uses), exactly matching the new doc's claim. The macOS/no-provider legacy extension is in scope by symmetry (§5.3 item 1's text is about the class, and `observation.rs`'s renderer is platform-agnostic). The `WorkspaceArtifacts.md` cross-ref is 7 lines and makes the runbook findable from where the sibling advisory-lock gap is already recorded — accepted. The **Windows** legacy extension is the one that is *incomplete* rather than excessive — **[P3-3]**. |
| **10** | Tripwire fn name unchanged (`the_catalog_owner_gains_its_first_production_caller_only_at_e4_1`) | **CONCUR.** The name remains a true statement of fact after the move (E4.1 *is* the step at which the first caller appeared), the file's own doc and the constant's comment carry the "exact count, not a zero" reframing, and renaming would churn a pin other documents cite by name. |
| **11** | `[P3-6]` fold — "+1 line claimed" | **VERIFIED: exactly +1.** `git show --numstat` for `catalog_activation_pin.rs` in commit (a) is `5 4` — five added, four deleted, net +1, all inside the one `assert_eq!` message. And the rewritten message does lead with what the assertion proves: **RED-PROBE transcript** — `"the production files outside the owner naming \`recover_or_create\` — usually callers, possibly mentions — are ["entry.rs", "policy.rs"], which is not 1"`. Self-diagnosing, as claimed. |

---

## 6. COMPANION RECOMPUTATION TRANSCRIPT

### 6.1 Digests — recomputed with the checker's own algorithm, at each commit

Method: both commits extracted with `git archive | tar -x` into isolated
directories (no mutation of the review worktree); `source_tree_digest` and the
plain-file `sha256` reimplemented from
`check_checked_artifact_boundaries.py:966-983` / `:1037-1043`; the pinned values
parsed out of that commit's own script; all 17 + 7 compared.

```
=== COMMIT-A 7f2862d : PROTECTED_SOURCE_DIGESTS (17) ===   17 × OK
=== COMMIT-A 7f2862d : PROTECTED_SOURCE_TREE_DIGESTS (7) ===  7 × OK
COMMIT-A 7f2862d: 0 mismatches

=== COMMIT-B 1f3ba71 : PROTECTED_SOURCE_DIGESTS (17) ===   17 × OK
=== COMMIT-B 1f3ba71 : PROTECTED_SOURCE_TREE_DIGESTS (7) ===  7 × OK
COMMIT-B 1f3ba71: 0 mismatches
```

The seven re-pins, each recomputed and matching:

| commit | kind | entry | pinned = recomputed |
| --- | --- | --- | --- |
| (a) | source | `checked_artifact/residue.rs` | `8894be425ddd6755…` |
| (a) | tree | `checked_artifact/platform.rs` | `c464666735aae202…` |
| (b) | source | `checked_artifact/capability.rs` | `c9c314e100bf3724…` |
| (b) | source | `checked_artifact/entry.rs` | `ea69805944078830…` |
| (b) | tree | `checked_artifact/capability/pre_catalog.rs` | `d531ce50a0092f8c…` |
| (b) | tree | `checked_artifact/catalog.rs` | `e1a8f1f05f0aeeac…` |
| (b) | tree | `workspace_ops/merge/v1_lifecycle/mod.rs` | `23ee7d417b8a3178…` |

All seventeen non-moved source pins and the four non-moved tree pins recomputed
unchanged at both commits, including `operation/workspace_mutator_lock.rs`
(`0d9b034e…`) and `git/gitbackend/preservation_image.rs` (`b45057e1…`).

### 6.2 Checker, on pristine trees

```
$ cd <commit-a>; python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
CHECKER_EXIT_A=0

$ cd <commit-b>; python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (16 visible entries, 6 classified modules)
CHECKER_EXIT_B=0
```

### 6.3 The inventory moves, against the actual tree

Verified structurally rather than by probe: **every** one of these is an
**equality** comparison in the checker, not a superset test —
`all_entry_items != ENTRY_ITEMS` (`:1245`), `entry_uses != ENTRY_USES` (`:1251`),
`entry_calls != ENTRY_CALLS` (`:1257`), per-symbol `actual != allowed` for
`ENTRY_REFERENCES` (`:1286`), and `actual != allowed` for
`CATALOG_LEASE_REFERENCE_SETS` (`:1313`). A green checker on a pristine tree
therefore proves the moves are **exactly minimal and exactly sufficient** — no
superfluous entry could survive.

| inventory | move | verified against tree |
| --- | --- | --- |
| `ENTRY_REFERENCES` | `+ "activate_workspace_catalog": {"workspace_ops/merge/v1_lifecycle/checked.rs"}` | the only caller (`checked.rs:157`) |
| `ENTRY_ITEMS` | `+ "activate_workspace_catalog"` | `entry.rs:302` `pub(crate) fn` |
| `ENTRY_USES` | `+ 3` (`super::bootstrap::CatalogMutationLeaseV1`, `super::capability::CheckedFsError`, `super::catalog::recover_or_create`) | `entry.rs:10-12` |
| `ENTRY_CALLS` | `+ 4` (`map`, `map_err`, `recover_or_create`, `remedy`) | `entry.rs:303-321` |
| `CATALOG_LEASE_REFERENCE_SETS["catalog_mutation_lease"]` | `+ "workspace_ops/merge/v1_lifecycle/checked.rs"` | `checked.rs:157` |

Commit (b)'s message calls this "six inventory moves … (the four `ENTRY_*`
inventories, `catalog_mutation_lease`'s reference set)" — five, enumerated
([P3-6]). No lease-reference row moved for `entry.rs` because that file is
excluded from the scan ([P3-4]).

### 6.4 Aggregate counts — MEASURED by me, darwin/APFS, at (b)

| partition | claim | my measurement |
| --- | --- | --- |
| `checked_artifact::` | 452 → 454 → **456** darwin MEASURED | `456 passed; 0 failed` ✓ |
| `workspace_ops::merge::v1_lifecycle::` (skip `root_fault_matrix`) | 256 → **257** darwin MEASURED | `257 passed; 0 failed` ✓ |
| lib remainder (skip both) | **1097 passed + 1 ignored**, UNMOVED | `1097 passed; 0 failed; 1 ignored` ✓ |
| `checked_artifact::` linux | 462 → 464 → **466** DERIVED (+2/+2, cfg-independent) | **cfg-audit performed**: the four new `checked_artifact::` rows are `capability_permit.rs:479`, `contracts.rs:246`, `contracts.rs:145`, `catalog/bootstrap/tests.rs:702` — **none carries a `cfg` attribute**, and none of their containing modules carries a platform `cfg`. Arithmetic and cfg-independence ✓. Risk noted at **[P3-7]**. |
| `v1_lifecycle::` linux/windows | re-measured at dispatch | the one new row (`tests/checked.rs:42`) carries no `cfg`; the obstruction is a plain file ✓ |

### 6.5 The dated provenance blocks

Both E4.1 blocks (`run_r4bg_aggregate_gates.py:305-322` and `:322-343`) follow
the two R2-F blocks' form exactly: dated step header naming what moved and by how
many rows, then `darwin X -> Y: MEASURED on this step's own tree (…, 2026-09-01)`
/ `linux X -> Y: DERIVED (+N, cfg-independent), *not* measured, and therefore
FIRST-DISPATCH-EXPECTED … A measured number wins.` / the UNMOVED partitions
restated. ✓ Commit (b)'s block gives no separate linux number for
`v1_lifecycle::`, which is correct: that pin is a plain string in `BATTERIES`
("257 passed"), not a `_fault_count(darwin, linux)` pair.

### 6.6 Gates, re-run by me on the candidate tree

```
cargo fmt --all -- --check            FMT_EXIT=0
cargo clippy --all-targets --all-features -p gwz-core   CLIPPY_EXIT=0  (no warnings)
boundary checker (a) and (b)          exit 0, exit 0
```

---

## 7. PROBE TRANSCRIPTS (all reverted; worktree byte-clean at `1f3ba71`)

| # | Probe | Result |
| --- | --- | --- |
| **P-1** | Tripwire RED: add a second production namer at `checked_artifact/policy.rs` | **RED** — `left: 2, right: 1`, message names `["entry.rs", "policy.rs"]` |
| **P-2** | Path-constant pin RED: rename `CATALOG_PRIVATE_PATH`'s literal to `".gwz/catalog-renamed"` **with a `// was: …` remnant line present** | **RED**, naming `CATALOG_PRIVATE_PATH` and the expected literal — the comment strip defeats the remnant, as the R1.2 [P3-8] precedent requires |
| **P-3** | Bounded-read pin RED: revert `observe_leaf_exact` to bare `Vec::new()` + `read_to_end` | **RED** at `capability_permit.rs:483` |
| **P-4** | Exclusion necessity: remove `.gwz/catalog-final` from `collect_files` | **4 rows RED**; before/after diff is *only* the catalog's five infrastructure files; non-catalog file sets byte-identical |
| **P-5** | `catalog.rs` allow-reason accuracy: strip every `#[allow]` from `catalog.rs`, `cargo check --lib` | **EXACT.** `dead_code` names precisely "methods `retire_admitted_action` and `observe_roaming_anchor_home` are never used"; `unused_imports` names precisely "`CatalogOwnerV1`". Both rewritten reasons are machine-true. |
| **P-6** | **[P1-1] the wedge**, ordinary merge + adaptation | transcript in [P1-1] |
| **P-7** | **[P1-1] causal isolation**, one line removed | `resume = Ok(Completed)` — the wedge is caused by `checked.rs:157` and nothing else |
| **P-8** | **Real capability refusal**, FAT32 volume (created, attached, driven, detached, deleted) | the full user-visible sentence, transcript in [P1-1] |

---

## 8. CAP, PROSE, SCOPE, TRAILERS

**Whole-line added counts, measured independently** (`git show --numstat`,
whole handwritten added lines including comments; nothing generated in either
commit):

| commit | added | deleted | cap | verdict |
| --- | --- | --- | --- | --- |
| (a) `7f2862d` | **148** | 18 | 500 | inside ✓ (builder's 148 confirmed) |
| (b) `1f3ba71` | **472** | 85 | 500 | inside ✓ (the message says 469 — **[P3-6]**) |

**Trailers.** `git log --format=%B 027da5b..1f3ba71 | grep -inE
'co-authored|anthropic|noreply|generated with|claude'` → **exit 1, zero
matches**. Clean.

**Prose audit.** No non-mandated prose. Every doc comment added either restates a
named authority (E0.2 §5.2, E0.2b §5.3 items 5 and 6, E7 Acceptance §4's O12 row,
the plan's E4 gate note riders and its E7.2 scope clauses) or explains a
mechanism at the site that needs it. The two door docs (`entry.rs:274-301`,
`checked.rs:127-153`) are long but every paragraph carries a decision. The
`catalog.rs` allow-reason rewrite is machine-true (P-5). One overstatement found:
`identity.rs:470-479`'s unqualified claim ([P3-3]). One falsified claim:
`checked.rs:140-142` / `capability.rs:44-46` ([P1-1]).

**Scope, file by file.**

*Commit (a) — 8 files, all in scope, no creep:* the checker and driver companions
(the standing ritual); `capability_permit.rs` (Q1's companion); `contracts.rs`
(E4 gate note rider 4); `catalog_activation_pin.rs` (rider 3, `[P3-6]`);
`observation.rs` (rider 1, the Q1 shape); `platform.rs` ([R2-P3-3], one word, per
Code F2's "fold into the train that edits the function"); `residue.rs` (Code
[P3 F3], named by the acceptance as "one class" with Q1).

*Commit (b) — 18 files, all justified:* the two docs (precondition 3); the two
companion scripts; `capability.rs` + the four providers + `identity.rs`
(precondition 1, load-bearing — flag 9); `catalog.rs` (rider 2);
`catalog_activation_pin.rs` (precondition 0); `contracts.rs` (precondition 1's
pin); `entry.rs` + `v1_lifecycle/checked.rs` (O2 itself); the two precondition-6
rows; `reverse_rollback/entry_service.rs` (the disclosed consequence).

**Q1 fidelity against the O12 row's terms, clause by clause** (commit (a),
`observation.rs:248-285`):

| O12 row term | implementation | verdict |
| --- | --- | --- |
| "Signature: unchanged. Zero call-site churn" | `observe_leaf_exact(dir, leaf, code, label) -> ModelResult<LeafObservation>` untouched; no call site edited in either commit | ✓ |
| "cap = the already-identity-checked `opened.len()` fstat" | `let bound = opened.len().saturating_add(1);` where `opened` is the post-open metadata already compared against the pre-open stat | ✓ (`saturating_add` on `u64` is equivalent to the row's `checked` framing — `u64::MAX` is unreachable for a file length) |
| "`try_reserve_exact` + `take(len+1)` per the `platform.rs:219-234` template" | `bytes.try_reserve_exact(capacity)?` then `file.by_ref().take(bound).read_to_end(&mut bytes)` | ✓ |
| "a leaf larger than address space returns `fact: Invalid`" | `let Ok(capacity) = usize::try_from(bound) else { return Ok(LeafObservation { fact: Invalid, identity: None }) };` | ✓ |
| "allocation failure is a typed refusal, never an abort" | `.map_err(\|_\| error(code, label, "read artifact bytes: allocation refused"))?`, with a comment explaining why the sibling constructor is used (no `io::Error` to render) | ✓ |
| "overrun caught by the existing len-mismatch → Invalid arm; no new refusal vocabulary" | over-read by exactly one byte; the existing five-way check at `:273` catches it | ✓ |
| "the stale `:394` terms are retired" | superseded by this shape | ✓ |
| "the bound is the reader's own, NOT inherited by callers" | stated in the header, and `residue.rs`'s survey now imposes its own stat-level budget | ✓ |

`residue.rs`'s F3 reorder does change one behaviour beyond a pure reorder: over-budget
now takes the hard `return Ok(FamilyResidue { foreign: true, ..empty() })` arm
instead of the old per-leaf `foreign = true; continue`. That is the arm
`MAX_FAMILY_ENTRIES` already used two lines above, so the shapes are unified
rather than divergent, and `DirEntry::metadata()` is the same no-follow stat
`observe_leaf_exact` opens against — the comment says so and it is true. Accepted.

---

## 9. THE TWO SCOPE CLAUSES

**"A settled barrier ordinal does not imply its target parent's dirents were
ordered"** and **"converged-by-observation does not imply flushed"** (E7.2's
[R2-P3-1] and its terminal sibling).

**No new consumer code relies on either.** The complete set of new consumer code
is `activate_workspace_catalog` — which calls `recover_or_create`, discards the
retained result with `.map(|_retained| ())`, and maps the error — and one line in
`V1MutationLease::acquire`. Neither reads a barrier ordinal, a settled-ordinal
return, or a terminal converged-restart return; nothing is carried across the
call. Both clauses are additionally **restated in code where E4's consumers will
read them**, at `entry.rs:294-301`, as the plan's E7.2 annotation directs. ✓

---

## 10. ONE-LINE MANDATE DISPOSITIONS

1. **The activation site.** Five of six grounds verified; ground 5 **FALSIFIED** and driven — [P1-1]. No bypass path into v1 mutation; no non-v1 path activates the catalog; a v0 workspace *is* affected, via the A1 adaptation path only.
2. **The seven preconditions.** 0 MET (red-probed) · 1 MET on the activation path, driven by me, two residuals · 2 MET byte-exact · 3 MET on literal terms, contaminated by [P2-1] · 4 RESIDUAL [P3-1] · 5 MET as a defensible minimal reading, ordering argument verified more widely than stated · 6 rows DRIVE but the precondition is NOT discharged on the adaptation path — [P1-1].
3. **Commit (a) fidelity.** Q1 implemented **verbatim**, eight terms checked one by one; [R2-P3-3] and F3 executed per the E7 ReviewCode terms; the path-constant pin red-probed through the comment strip; [P3-6] is exactly +1 line. Commit (a) is clean.
4. **The eleven flags.** 1 CONFIRMED (record correction owed to §5.2 Ground 1) · 2 CONFIRMED (O11's CLOSED NEGATIVE is wrong — record correction) · 3 CONCUR, [P3-5] · 4 CONFIRMED, [P3-4] · 5 AUDITED — exclusion necessary, rows undamaged, [P3-8] · 6 ACCEPTED · 7 **partly REJECTED** — sentence driven by me, reason not binding, [P3-2] · 8 CONCUR · 9 load-bearing, not creep; Windows half incomplete, [P3-3] · 10 CONCUR · 11 VERIFIED +1.
5. **Companions.** All 24 digests recomputed with the checker's own algorithm at both commits — **0 mismatches**; checker green on pristine (a) and (b); five inventory moves verified minimal-and-sufficient by the checker's equality semantics; counts 456 / 257 / 1097+1 all **MEASURED by me**; linux DERIVED arithmetic cfg-audited row by row; provenance blocks follow the R2-F form.
6. **Cap, prose, scope, trailers.** 148 and **472** (message says 469 — [P3-6]); both inside 500; no non-mandated prose; every file justified; **zero AI-attribution trailers**.
7. **The two scope clauses.** No new consumer relies on either; both restated at `entry.rs:294-301`.

---

## 11. WHAT IS GOOD HERE

Recorded because a NO-GO should not obscure it. The companion discipline is the
best I have audited in this lane: 24 digests recomputed to zero mismatches, five
inventory moves that are provably minimal because the checker compares by
equality, three dated provenance blocks in the house form, and an allow-reason
rewrite that names the surviving dead code **exactly** — `retire_admitted_action`
and `observe_roaming_anchor_home`, nothing more, nothing less — which I confirmed
by stripping the attributes and reading the compiler's own list. Precondition 2 is
discharged byte-exactly against the base commit. The Q1 shape is implemented
clause-for-clause without re-deciding anything. Three pins red-probe correctly,
including the comment-strip defence against a `// was:` remnant. And both of the
builder's most uncomfortable flags — the authority mis-citation (flag 1) and
O11's over-claim (flag 2) — are **true**, were surfaced voluntarily, and are the
kind of self-report that makes a review possible at all.

The one thing that went wrong went wrong at the level the step is hardest at: a
scope claim about a *route* rather than a *site*. `--no-ff` is the only way in
through the front door; the A1 adapter is a side door that the writer floor does
not govern.

---
---

# ROUND 2 — the cure

**Object.** Commit (c) `6688f34b08deed1daf65f07b6ce894e8e2c79277`, "E4.1(c): the
[P1-1]/[P2-1] cure — the wedge, closed at the adapter", on top of the reviewed
(a)/(b). **Date:** 2026-09-01. Same reviewer, same rituals: read-only outside the
worktree, every probe reverted, worktree byte-clean at exit, direct exits,
`python3.13` for the checker, `CARGO_TARGET_DIR` in scratch. Terminal round under
the two-round cap.

**Method note.** Where round 1's finding is at stake I re-drove **my own** probe
code rather than reading the builder's new rows, so the cure is graded against
the original scenario and not against the test written to satisfy it. I also
red-probed the cure itself (disable the seam, confirm the defect returns) and
drove the capability refusal on a real incapable filesystem rather than on the
in-tree obstruction stand-in.

---

## R2.1 VERDICT

# GO — WITH ONE BLOCKING PRE-LANDING CONDITION

**The ruled contract R1–R7 is discharged in full, and I drove every clause of
it.** [P1-1] is cured at the root and the cure is causally isolated; [P2-1]'s
three items are each cured and each driven. Nothing about the cure's design,
ordering, locking or scope is wrong.

**One defect remains, and it blocks the landing dispatch:**

> **[P2-C1] — the two new R7 rows do not compile on Windows.** Proven with a
> simulated non-unix build: `error[E0425]: cannot find function
> `invoke_with_store_and_merge_id` in this scope` / `error: could not compile
> `gwz-core` (lib test)`. Fix is one deleted line. Details and both candidate
> fixes at §R2.8.

**Condition, blocking, pre-landing:** make the two new rows' dependency chain
portable — preferred fix, delete the `#[cfg(unix)]` at
`src/workspace_ops/tests/g23/a1_activation.rs:335` — and correct the aggregate
driver's "cfg-independent" clause to say what the rows' gating actually is.
Re-run the boundary checker and the remainder count after the edit (the count
does not move under the preferred fix).

> **ESCALATION.** [P2-C1] is a P2, and the lane's rule is that any P0/P1/P2
> triggers the Fable escalation. I record the fact rather than pre-empt the rule.
> My reading, offered for the operator's use: this is a `#[cfg(test)]`-only,
> fail-loud, one-line defect that cannot reach a user and cannot pass the
> dispatch silently, sitting on a cure whose entire substantive contract I
> verified discharged. Calling that terminal would be disproportionate to the
> defect and would discard a correct cure; landing it unconditionally would ship
> a train whose Windows leg cannot build against a provenance block that asserts
> the opposite. Hence GO-with-condition rather than either pole.

Round-1's eight P3s: all keep their carriers. Two new P3s below.

---

## R2.2 RE-DRIVING MY ROUND-1 PROBES ON (c)

### PR-1 — the wedge scenario, verbatim from round 1. **CURED.**

Same fixture, same `FaultingMergeStore(FinalizationFault::AfterEnteringFinalizing)`,
same obstruction, same prints:

```
PR1 pre  envelope = ("gwz.merge-operation/v0", 0)
PR1 pre  version=V0 adaptation=MayAdapt
PR1 pre  .gwz tree = [".gwz/", ".gwz/locks/", ".gwz/locks/workspace-mutator.lock",
                      ".gwz/merge/", ".gwz/merge/merge_probe_r2_wedge.yaml",
                      ".gwz/stash/", ".gwz/stash/bundles/"]
PR1 resume = Ok((Completed, false, Some(V0)))
PR1 post envelope = ("gwz.merge-operation/v0", 0)
PR1 post open-record = None
PR1 post .gwz tree = [..., ".gwz/merge/done/merge_probe_r2_wedge.yaml", ...]
```

The merge **completes under v0**, `source_version = V0`, the envelope on disk is
**still v0/0**, and the open record is gone. **No durable upgrade precedes a
refusal the v1 lifecycle would raise.** R1 discharged, driven.

### PR-1b — causal isolation of the cure itself. **The seam is what cures it.**

With `forward_lifecycle_viability_window`'s gate disabled and everything else at
(c) untouched, the round-1 defect returns exactly:

```
PR1 resume = Err((IoError, "checked merge artifact catalog rejected catalog recovery
                  roles: reserved role kind, bytes, or recovery attempt is not uniquely owned"))
PR1 post envelope = ("gwz.merge-operation/v1", 1)
PR1 post open-record = Some((V1, Skip))
```

So PR-1's pass is attributable to the seam and to nothing else — not to a
fixture change, not to the R2 split.

### PR-2 — R3, the FAT32-class composition via the obstruction method. **DISCHARGED.**

```
PR2 record now = ("gwz.merge-operation/v1", 1)          <- a genuine open v1 record
PR2 resume = Err((IoError, "checked merge artifact catalog rejected catalog recovery roles: …"))
PR2 record byte-identical = true
PR2 tree ADDED by the refused resume = [".gwz/catalog-final"]   <- my own obstruction plant
PR2 abort = Ok((Aborted, false))
PR2 open after abort = None
PR2 obstruction intact = Some("foreign")
```

Typed refusal ✓, **record byte-identical** ✓, abort clears ✓, and the abort never
touched the catalog it never needed ✓. (The one "added" path is the obstruction I
wrote after taking the before-snapshot, not a write by the operation.)

### PR-3 — the real capability refusal on a real incapable filesystem. **R2 driven where it matters.**

A FAT32 volume (`hdiutil create -fs "MS-DOS FAT32"`, attached, driven, detached,
deleted), pointed at a git-initialised directory on it:

```
PROBE ACTIVATED-LEASE REFUSAL: code=UnsupportedOperation
  message=checked merge artifact catalog: this filesystem does not expose the persistent file
  handles and mount identity that checked merge artifacts require; run the workspace on a
  filesystem that does (on Linux that is ext4 only; APFS or HFS+ on macOS; NTFS on Windows).
  An open merge can be cleared with `gwz merge --abort`, which needs no such filesystem;
  a new merge can be started without --no-ff
  (detail: filesystem does not promise persistent object identities)

PROBE PLAIN LEASE (abort route) on the SAME volume: Ok — capability-free
```

Two things at once, and both are stronger than the in-tree rows: the **cured
remedy sentence** rendered end to end by a genuine capability gap (ext4-only
leading, **both** exits named); and **the plain lease succeeding on a genuinely
incapable volume** — R2 proved against the real capability, not against an
obstruction stand-in.

---

## R2.3 THE R2 SPLIT, ADVERSARIALLY

**Every lease-acquisition site, enumerated.** Exactly three in production:

| site | lease | arm |
| --- | --- | --- |
| `v1_lifecycle/start.rs:83` | `acquire_activated` | the creation lease — this record is being written at v1 |
| `v1_lifecycle/service.rs:118` | `acquire_activated` | `ResumeStart \| Continue` |
| `v1_lifecycle/service.rs:120` | `acquire` (plain) | `_` — `Abort`, `Preserve`, `Status`, `Archive` |

No other production caller of either exists (grep, excluding `_for_test` and
`tests/`).

**Every `V1LifecycleRequest` arm** (`authority/dispatcher.rs:17-24`, six
variants), against the question *does it mutate toward v1 semantics on the plain
lease?*

| variant | lease | reaches `service::run` in production? | mutates toward v1? | verdict |
| --- | --- | --- | --- | --- |
| `ResumeStart` | activated | yes (`start.rs:93`) | yes | correct |
| `Continue` | activated | yes (`start.rs:158`) | yes | correct |
| `Abort` | plain | yes (`start.rs:154`) | reverse — E0.2 §5.2's capability-free list | correct |
| `Preserve` | plain | yes (`start.rs:153`) | reverse | correct |
| `Status` | plain | **no** — `handle_v1_command` routes `Status` to `status::open_status`, and `service::run`'s `V1NextAction::Respond` arm returns before any lease | n/a | unreachable, harmless |
| `Archive` | plain | yes (`archive.rs:83`) | terminal, not forward | correct **today**; see the carried risk below |

**Answer: no arm mutates toward v1 semantics on the plain lease.** The forward
set is exactly `{ResumeStart, Continue}` plus the creation lease, and all three
take `acquire_activated`.

**Is there any path where Abort takes `acquire_activated`? No — and I drove the
whole chain, not just the arm.** `handle_v1_command`'s `Abort`/`Preserve` map to
the reverse requests → the `_` arm → plain lease. Critically, `respond()`
(`start.rs:106-120`) then archives a `Terminal` disposition through
`archive_terminal` → `service::run(…, Archive, …)` → **also** the plain lease. So
the abort chain is capability-free end to end, archive included. PR-2 and the
Abort probe below both run that whole chain on an obstructed workspace and it
completes.

**Carried risk, named for its carrier (new [P3-C1]).** §7.6's nine-row map has
E4.4 giving the terminal-archive row a catalog dependency (managed-parent
bootstrap). If E4.4 converts the `Archive` arm to consume the catalog, it will do
so **on the plain lease**, and because `respond()` routes an *aborted* record
through that same arm, **abort would silently become capability-dependent again**
— undoing R2 through a path nobody is looking at. Nothing in the tree pins R2
against that. Carrier: **E4.4**; the cheapest guard is a row asserting `--abort`
succeeds on an obstructed workspace after E4.4's conversion (the R7(iii) row
already has the shape).

**The double-acquire: deadlock-free, verified three ways.**
1. **Nothing holds the lock when the window opens.** The adapter runs after
   `drop(start_guard)` (`dispatch.rs:321`), on the non-`Start` path only.
2. **`upgrade_open_v0` takes no lock of its own.** The builder asserts this; I
   verified it — `grep -rn "MutatorLock" src/workspace_ops/merge/record_wire/
   src/workspace_ops/merge/store/` returns **nothing** (exit 1). So holding the
   window across the write cannot self-deadlock.
3. **The guard drops before the command re-acquires.** `_window` is a real local
   binding in `adapt_before_mutating`; it drops at that function's end, and
   `v1.command(...)` is called by the *caller* at `dispatch.rs:334/337`, one
   frame up. PR-1's `Ok((Completed, …))` and PR-5's `Completed` both prove the
   re-acquire succeeds in practice.

**Contention is handled, and handled in the right voice.** `acquire(...).ok()?`
turns a held lock into `None` → `Ok(false)` → the v0 route, which takes the same
lock at `continue_op/coordinator.rs:13` and reports contention itself. Verified;
the doc says exactly this.

**Cost (the builder's flag 7).** An adapted resume on a capable filesystem now
acquires the lock twice and runs `recover_or_create` twice. Correctness-neutral —
`recover_or_create` is a converging recover-or-create owner and PR-5 confirms the
second drive succeeds — and the cost is two bounded catalog drives on a recovery
path, not a hot path. Concur that it is worth the wedge it buys.

---

## R2.4 THE ADAPTER GATE

**Is `Resume`-only gating sound? Yes, and I drove the `Abort` half rather than
reasoning about it.**

**PR-4 — Abort on a `MayAdapt` v0 record, on an obstructed workspace:**

```
PR3 pre  version=V0 adaptation=MayAdapt
PR3 abort = Ok((Aborted, false))
PR3 post envelope = ("gwz.merge-operation/v1", 1)     <- YES, it still upgrades
PR3 post open = None                                   <- and the abort clears it
PR3 catalog bytes = Some("foreign")                    <- untouched
```

So: **the upgrade still happens on the Abort path** — the window is skipped, not
the upgrade — **and the post-upgrade abort is catalog-free and green**, on a
workspace whose catalog cannot be activated, with the obstruction byte-intact.
That is the correct design: for `Abort` there is no viability question to ask,
because the destination (the reverse service) is capability-free *by
construction* under R2. The builder's stated ground is verified by drive, not
accepted on assertion.

Residual, small and convergent: an aborted `MayAdapt` record is durably rewritten
to v1 before being aborted, so a process death in that window leaves an open **v1**
record on an incapable filesystem. Not a wedge — `--abort` on it is capability-free
and clears it (the same path PR-2 drives). Recorded, not graded.

**Does a declined adaptation leave a durable trace? No.**

**PR-5 — the declined-viability trace:**

```
PR4 before = [".gwz/", ".gwz/catalog-final", ".gwz/locks/", ".gwz/locks/workspace-mutator.lock",
              ".gwz/merge/", ".gwz/merge/merge_probe_r2_trace.yaml", ".gwz/stash/",
              ".gwz/stash/bundles/"]
PR4 resume ok = true
PR4 after  = [same, plus ".gwz/merge/done/merge_probe_r2_trace.yaml"]
PR4 ADDED (non-merge) = []
PR4 catalog obstruction intact = Some("foreign")
```

**`ADDED (non-merge) = []`** — the failed viability attempt leaves no scratch, no
partial catalog, no staging directory, nothing. The only delta is the v0
lifecycle completing the merge. Mechanism, confirmed in round 1: on an obstructed
target `recover_or_create` refuses at classification before any write; on a
capability-less filesystem it refuses at the first `dir_identity` probe inside
`preflight_catalog_target`, also before any write. Convergent because there is
nothing to converge over.

**PR-6 — the happy path is not collateral damage.** A cure that simply stopped
adapting would also pass PR-1, so I checked the capable case:

```
PR5 resume = Completed source_version=Some(V1)
PR5 envelope = ("gwz.merge-operation/v1", 1)
PR5 catalog is dir = true
```

The A1 migration still happens on a capable filesystem. The cure narrows the
adapter by exactly the unviable cases and nothing more.

---

## R2.5 R1–R7 DISCHARGE TABLE

| # | Ruled requirement | Verdict | Evidence |
| --- | --- | --- | --- |
| **R1** | Adapter must not durably upgrade a record it cannot hand to a viable v1 lifecycle; non-viability pre-upgrade → `Ok(false)`, v0 in command, no durable write first | **DISCHARGED** | `dispatch.rs:406-415` gates on `request.op == Resume`, calls `forward_lifecycle_viability_window` **before** `upgrade_open_v0`, returns `Ok(false)` on `None`. Driven: **PR-1** (completes under v0, envelope still v0/0). Causally isolated: **PR-1b**. Lock held across the write, no self-deadlock (§R2.3). |
| **R2** | Abort capability-free, period; activation off the shared prologue | **DISCHARGED, and stronger than required** | `acquire` plain / `acquire_activated` for start + `ResumeStart\|Continue` only (three production sites, enumerated §R2.3). Driven on an obstruction (**PR-2**, **PR-4**) *and on a genuinely incapable FAT32 volume* (**PR-3**: plain lease `Ok`). The whole abort chain including `respond()`→`archive_terminal` is plain-lease. |
| **R3** | Resume of a genuine v1 record on an unactivatable catalog: typed refusal, no durable mutation, record unchanged, abort clears | **DISCHARGED** | **PR-2**: typed refusal naming the catalog; `record byte-identical = true`; `abort = Ok((Aborted, false))`; open record `None`. |
| **R4** | Post-upgrade non-capability obstruction residual: narrowed not vanished, disclosed at the site, driven, abort the exit | **DISCHARGED** | Disclosed verbatim in `forward_lifecycle_viability_window`'s doc ("**Residual, disclosed (review R4)** … a race, not a capability … Not a wedge: `gwz merge --abort` is capability-free and clears it"). The state R4 leaves *is* the state R3's row starts from, so R3's abort half is the exit, driven. A standalone row would need a catalog loss injected between two lock acquisitions — a race with no injection key, genuinely undrivable. Fold accepted. |
| **R5** | Docs/comments/release-notes cured to behavior-truth; ext4-only leading the Linux family sentence | **DISCHARGED** | Four sites cured (`OperationModel.md`, `capability.rs`, `checked.rs`, the remedy string). Full text audit at §R2.9; ext4-only now **bold and leading**; the remedy's second exit driven on FAT32 (**PR-3**). |
| **R6** | [P3-6]'s nits folded; [P3-2] carried to E4.2 per its own out, with a +1-line pin asserting the remedy names `--abort` | **DISCHARGED** | (i) count basis stated and measured-correct (331, §R2.7); (ii) "the inventory enumeration is five, not six" stated; (iii) both `model/version.rs` cites corrected to `workspace_ops/merge/model/version.rs` (`capability.rs`, `checked.rs`). Pin extended: `contracts.rs:152-159` now requires `--abort` in the remedy — green in the 456 partition. The measured 44-line cost of the [P3-2] extraction justifies the carry; my round-1 ~10-line estimate was for the row alone, not the extraction, and I accept the correction. |
| **R7** | (i) wedge completes under v0, envelope v0; (ii) folded into (iii); (iii) v1 resume/abort; (iv) fresh-start row retargeted + plain-lease line | **DISCHARGED** | (i) `an_interrupted_ordinary_merge_completes_under_v0_when_the_catalog_is_unavailable` — asserts `Completed`, `source_version == V0`, envelope v0/0, open record gone; independently re-driven as **PR-1**. (iii) `a_v1_resume_refuses_without_mutation_and_abort_still_clears_the_record`; independently re-driven as **PR-2**. (iv) `tests/checked.rs:51` retargeted to `acquire_activated_for_test`, plus `assert!(V1MutationLease::acquire_for_test(&workspace.path).is_ok())`. **(ii) fold judged SUFFICIENT — I do not demand the standalone variant**: the refused resume writes nothing (PR-2's byte-identity), so the pre-abort state is identical with or without it; and my **PR-4** drives abort with no prior resume at all, as does PR-3's plain lease on FAT32. *Subject to [P2-C1]: rows (i) and (iii) do not compile on Windows.* |

---

## R2.6 COMPANIONS

**Digests.** All 24 recomputed at (c) with the checker's own algorithm from an
isolated `git archive` extract — **0 mismatches**. The three re-pins:

| kind | entry | pinned = recomputed |
| --- | --- | --- |
| source | `checked_artifact/capability.rs` | `c1760e95fa91d8f5…` |
| source | `checked_artifact/entry.rs` | `1cdf540650ee91a4…` |
| tree | `workspace_ops/merge/v1_lifecycle/mod.rs` | `3146b52783aa17b8…` |

The other 21 recomputed unchanged, `operation/workspace_mutator_lock.rs` still
byte-identical to base `027da5b` at `0d9b034e…` — **precondition 2 survives the
cure**, which matters, because the cure adds a lock site and could easily have
touched that file.

**Checker.** `checked-artifact boundary: ok (16 visible entries, 7 classified
modules)`, exit 0. The line is derived: `len(ENTRY_REFERENCES)` and
`len(union(ENTRY_REFERENCES.values()))`. I recomputed both from the script:
**16 / 7**, and the 7th module is `workspace_ops/merge/runtime/dispatch.rs` —
legitimately added by the cure and inventoried in **both** places it must be:

- `ENTRY_REFERENCES["activate_workspace_catalog"] = {dispatch.rs, checked.rs}`
- `CATALOG_LEASE_REFERENCE_SETS["catalog_mutation_lease"] += dispatch.rs`

Both are **equality** comparisons in the checker, so a green run proves the moves
are exactly minimal and exactly sufficient. Verified against the tree: the door
has exactly two callers (`dispatch.rs:453`, `checked.rs:160`) and
`catalog_mutation_lease` is read at exactly those two files outside the lock's
own.

**Counts, all MEASURED by me on the pristine (c) tree, darwin/APFS:**

| partition | claim | measured |
| --- | --- | --- |
| `checked_artifact::` | 456, UNMOVED | **456 passed; 0 failed** ✓ |
| `v1_lifecycle::` (skip `root_fault_matrix`) | 257, UNMOVED | **257 passed; 0 failed** ✓ |
| lib remainder | 1097 → **1099** | **1099 passed; 0 failed; 1 ignored** ✓ |

*(The first attempt at these was discarded: it was running while I was mutating
the worktree for probes. Every number above comes from a clean re-run on a
`git status --porcelain`-empty tree.)*

**Gates:** `cargo fmt --all -- --check` → 0. `cargo clippy --all-targets
--all-features -p gwz-core -- -D warnings` → 0, no warnings.

**Provenance block.** Follows the two R2-F blocks' form: dated step header naming
what moved and by how many rows, `darwin X -> Y: MEASURED on this step's own
tree (…, 2026-09-01)`, `linux X -> Y: DERIVED (+2, cfg-independent …), *not*
measured, and therefore FIRST-DISPATCH-EXPECTED. A measured number wins.`, then
the UNMOVED partitions restated. ✓ **Except its cfg claim — see [P2-C1].**

---

## R2.7 CAP RIDER-AUDIT

**331 whole added lines, measured by me** (`git show --numstat`, whole
handwritten added lines including comments, nothing generated) — **exactly** the
builder's figure and exactly the re-ruled cap. Cumulative (a)+(b)+(c) = 881.

| file | +/- | contract line | rider? |
| --- | --- | --- | --- |
| `docs/OperationModel.md` | 19/10 | R5 | no |
| `check_checked_artifact_boundaries.py` | 18/7 | companions: 3 digests, 2 inventory rows, their dated comments | no |
| `run_r4bg_aggregate_gates.py` | 17/1 | remainder pin + provenance block | no |
| `capability.rs` | 13/6 | R5 (scope comment) + the remedy's second exit | no |
| `entry.rs` | 13/9 | R5 (door doc corrected to the two callers and the true scope) | no |
| `contracts.rs` | 8/1 | R6's pin extension | no — the array went multi-line under rustfmt (4 entries) plus a 2-line reason citing [P2-1]; the semantic addition is one element |
| `dispatch.rs` | 40/0 | R1 (12 gate + 22 doc + 6 fn) | no |
| `v1_lifecycle/checked.rs` | 34/27 | R2 (the split, both docs, the test alias) | no |
| `v1_lifecycle/service.rs` | 10/1 | R2 (the match + its reason) | no |
| `v1_lifecycle/start.rs` | 3/1 | R2 | no |
| `v1_lifecycle/tests/checked.rs` | 8/4 | R7(iv) | no |
| `tests/g23/a1_activation.rs` | 148/0 | R7(i)+(iii) + four shared helpers | no — all four helpers are consumed by the two rows |

**No rider found.** Every added line traces to R1–R7 or to the standing companion
ritual. Two specific checks: (a) the 27 lines *removed* from `acquire`'s doc were
the falsified ground-5 argument and the nine-lock-site enumeration — removing
them is correct, and the E0.2b §5.3-item-6 ordering clause survives, relocated to
`acquire_activated`; (b) the reduction trail the message claims (−44 for [P3-2]'s
extraction, −24 for the R7(ii) fold, −90 of doc prose) is consistent with a floor
of 331 and with what is on the tree, though only the endpoint is measurable.

---

## R2.8 [P2-C1] — THE NEW DEFECT

**The two new R7 rows do not compile on Windows.**

`invoke_with_store_and_merge_id` is `#[cfg(unix)]`
(`a1_activation.rs:335-336`). The cure's helper `interrupted_ordinary_merge`
(`:579`) calls it and is **not** gated; `open_v1_record_from_an_adapted_crash`
(`:596`) calls that; and both new `#[test]` fns (`:631`, `:669`) are **not**
gated. On `cfg(not(unix))` the callee does not exist.

**Proven, not inferred.** I made the callee absent on darwin exactly as it is on
Windows (`#[cfg(unix)]` → `#[cfg(target_os = "linux")]` on it and on the two
pre-existing rows that use it, so only the *new* callers remain), in an isolated
extract of (c):

```
$ cargo test --lib -p gwz-core --no-run
error[E0425]: cannot find function `invoke_with_store_and_merge_id` in this scope
    |                 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ not found in this scope
error: could not compile `gwz-core` (lib test) due to 1 previous error
```

**Consequences.**
1. The **Windows leg of the package's own named three-platform landing dispatch
   fails at compile time** — the harshest, most confusing failure mode, and the
   one the provenance blocks exist to pre-empt.
2. The driver's new block says the two rows are "**cfg-independent** — both rows
   plant a plain file and drive production dispatch". That is **false**: they are
   unix-gated by dependency, just not by declaration. The linux `+2` happens to
   be right (`cfg(unix)` holds there); the Windows claim is not.
3. The [P1-1]/[P2-1] cure would have **no driven coverage on Windows**, where the
   catalog admits NTFS only and the wedge class is equally reachable.

**Two candidate fixes; the first is strictly better.**

- **(A), preferred — delete one line.** `invoke_with_store_and_merge_id`'s body
  is entirely portable (it installs a fixed-id `IdProvider`); its `#[cfg(unix)]`
  is guilt by association with the overlong-name test at `:522`, which genuinely
  needs it. Removing the gate at `:335` makes the whole chain portable **and
  gives the cure Windows coverage**. Then correct the driver's clause to plain
  "cfg-independent" — now true. Count does not move.
- **(B), conservative — add `#[cfg(unix)]`** to the two rows and their two
  helpers, matching the exact precedent two rows above (`:411-413`, `:522-524`).
  Costs the cure its Windows coverage, and the driver's clause must then say the
  rows are unix-only and the Windows count is `-2` against linux. Note the two
  ungated helpers `obstruct_the_catalog` / `workspace` would become dead on
  Windows, so `clippy -D warnings` forces them under the same gate.

**Why P2 and not P1:** it is `#[cfg(test)]`-only, cannot reach a user, and fails
loudly at compile time rather than silently. **Why P2 and not P3:** it fails a
named gate on a named platform, and the record asserts the opposite.

---

## R2.9 [P2-1] TEXT AUDIT

**`docs/OperationModel.md` §"Checked Merge Artifacts And Filesystem Identity",
claim by claim, against what I drove:**

| claim | verdict |
| --- | --- |
| "**On Linux the admitted filesystem is `ext4` and nothing else** — btrfs, xfs and zfs are refused, as are `tmpfs`, overlay and container filesystems and every network mount…" | **True, and prominent.** Bold, leading the family sentence, refused list explicit. Verified against `require_ext4` (`linux.rs:143-148`). **Round-1 item 3 CURED.** |
| "**What refuses:** `gwz merge --no-ff`, and `gwz merge --resume` of a merge record already at v1." | **True.** PR-2 drives exactly this. |
| "**What never refuses:** `gwz merge --abort`, with or without `--preserve`, on a record of either version…" | **True.** Driven three independent ways: PR-2 (v1 record), PR-4 (v0 `MayAdapt` record), PR-3 (plain lease on a genuinely incapable FAT32 volume). `--preserve` rides the identical `_` arm (`service.rs:120`) — same line, not separately driven. **Round-1 item 1 CURED.** |
| "Nor does an ordinary or `--ff-only` merge, including resuming one interrupted during finalization: … that upgrade is declined before it writes anything and the v0 lifecycle completes the merge itself." | **True.** This is PR-1, sentence for sentence. **Round-1 item 2 CURED.** |
| "Nor do `gwz repo create`, `init-from-sources`, `gwz merge --status`, GC, or the workspace mutation guard, none of which reaches the catalog." | **True.** None takes `acquire_activated`; the door's caller set is equality-pinned to two files; `Status` never reaches `service::run`. |
| "**Workaround:** … A merge already open can be cleared with `gwz merge --abort`; a new one can be started without `--no-ff`." | **True**, and cures the round-1 defect of advising an exit that does not exist for an open merge. |

**The release-notes line** (superseding (b)'s): every clause checked above holds,
and it additionally states the ext4-only posture with the same prominence.
**Site-true and behavior-true.** It is now the fourth artifact carrying the
corrected scope, so the round-1 [P3-1] carrier note stands unchanged: gwz-core
still stages no release notes, so the line still rides the commit message and
this report.

**One residual imprecision, [P3-C2].** The section's opening still says the
catalog asks for its two capabilities "when the merge takes its lock". After the
R2 split that is true of `acquire_activated` and the adapter's window, not of
every lock take — `--abort` takes the same lock and asks nothing. The "What
refuses / What never refuses" lists immediately below give the truth, so no
reader can be misled about scope; it is a leftover from the pre-split sentence.
One-clause fix, non-blocking.

---

## R2.10 THE BUILDER'S CURE FLAGS

**Record note.** Commit (c)'s message carries the R1–R7 structure, the CAP
reduction trail and the superseding release-notes line, but **no numbered
seven-flag list**, and I found no E4.1 delivery document in `gwz-dev/dev-docs/`
(newest files there are this review and the R2-F set). I therefore disposed of
the **three the coordinator named explicitly**, whose substance I can verify in
the tree, and audited the message's own self-reported claims in place of the four
I could not locate. Flagging the gap rather than inventing dispositions.

| flag | disposition |
| --- | --- |
| **2 — the R7(ii) fold** | **CONCUR; standalone variant NOT demanded.** The folded claim is "abort clears a v1 record on an unactivatable catalog". The only thing a standalone no-prior-resume row would add is the absence of the refused resume — and PR-2 proves that resume is a *no-op* on disk (`record byte-identical = true`, no tree delta), so the pre-abort states are identical. Independently, **PR-4 drives abort with no prior resume at all**, and **PR-3** shows the plain lease succeeding on a genuinely incapable volume with no prior anything. The claim is over-covered, not under-covered. |
| **6 — tenth lock site; census correction routed to the landing record** | **CONCUR, with the substance verified.** I counted the production sites at (c): **ten**, the new one being `dispatch.rs:452`. E0.2 §5.2 Ground 2's "nine production call sites" enumeration is now stale by one. Routing the correction to the landing record is the house form — the corpus uses dated corrections-at-the-record for exactly this (plan §1.1's O1 enumeration, §7.6's eight→nine, §8's 18→10) rather than reopening frozen text for a count. **And the DECISION is untouched, which is the part that matters**: §5.2 forbids probing at `try_acquire`, and the window does not — it probes at one dispatch decision, and its refusal is never surfaced (`None` → `Ok(false)`), so no user-visible refusal is added to the ordinary path. Worth stating explicitly so a future reader does not mistake the tenth site for option (i) creeping back. |
| **7 — double-acquire cost** | **CONCUR.** Two lock acquisitions and two `recover_or_create` drives per adapted resume, on a recovery path. Correctness-neutral (converging owner; PR-6 confirms the second drive succeeds), deadlock-free (§R2.3, three independent checks), and the gap between them is exactly the R4 residual the builder discloses at the site. The cost buys the wedge's closure, which is the right trade. |
| **1, 3, 4, 5** | **NOT LOCATED.** No numbered flag list in the commit message or in dev-docs. In their place I audited the message's self-reported claims: the "`upgrade_open_v0` takes no lock of its own" assertion (**verified** — grep exit 1 across `record_wire/` and `store/`); the R4 residual disclosure (**verified at the site**); the "[P3-2]'s extraction plus row measured 44 lines" correction (**accepted** — my round-1 ~10-line estimate covered the row, not the extraction); the cap reduction trail (**endpoint verified at 331**); and the gates list (**all re-run green by me**). If flags 1/3/4/5 raise anything outside those, this review has not seen it. |

---

## R2.11 FINDINGS — ROUND 2

| # | Finding | Grade | Carrier |
| --- | --- | --- | --- |
| **[P2-C1]** | The two new R7 rows do not compile on non-unix; the driver's "cfg-independent" clause is false for Windows. Proven with `error[E0425]`. §R2.8. | **P2 — BLOCKING, pre-landing** | this step, before the landing dispatch |
| **[P3-C1]** | `respond()` archives through the `Archive` arm on the **plain** lease, including after an abort. When E4.4 gives the terminal-archive row its catalog dependency, abort becomes capability-dependent again through a path nobody is watching, silently undoing R2. Nothing pins R2 against it. §R2.3. | P3 | **E4.4** |
| **[P3-C2]** | `OperationModel.md`'s opening still says the capabilities are asked for "when the merge takes its lock"; after the split that is true of a subset of lock takers. The lists below correct it, so no reader is misled. §R2.9. | P3 | this step or E4.2 |
| **[P3-C3]** | **The activation site's own account went stale in the split.** `catalog_activation_pin.rs:36-43` still says the production activation is "called from the checked v1 operation's prologue (`v1_lifecycle/checked.rs`'s `V1MutationLease::acquire`)" — but `acquire` is now the *capability-free* lease and the door's callers are `acquire_activated` and `dispatch.rs`'s window. `catalog.rs:10-14` carries the same stale phrase (that file is tree-digest-pinned, so it costs a digest move; the tripwire file is **not** pinned and cost nothing). The pin's **count is still correct and the test is green** — it counts `recover_or_create` namers, still only `entry.rs`, and it red-probes correctly at (c) (`are ["entry.rs", "policy.rs"], which is not 1`). This is the round-1 [P3-5] class recurring in the one file whose whole subject is where activation lives. | P3 | this step or E4.2 |

**Round-1 findings, status at (c):** [P1-1] **CURED** (R1, driven, causally
isolated). [P2-1] **CURED** (R2/R5; all three items, each driven). [P3-6]
**FOLDED** (all three nits). [P3-2] **carried to E4.2** per its own out, with the
remedy pin extended to `--abort` in the meantime — accepted. [P3-1], [P3-3],
[P3-4], [P3-5], [P3-7], [P3-8] — carriers unchanged, untouched by the cure.

---

## R2.12 CLOSING

The cure is the right cure. It does not paper the wedge over with a refusal
message or a docs caveat; it moves the proof to the only place it can sit — ahead
of the one-way durable write — and it splits the lease so that the exit every
refusal depends on can never itself require the capability. R2 is the more
important half of the two, and it is proved on a genuinely incapable filesystem
rather than on a stand-in. The adapter gate is asymmetric on purpose and the
asymmetry is justified, driven, and disclosed. The residual it cannot remove is
disclosed at the site in the same breath as the thing it does remove.

The one defect is that the rows proving all of this were never compiled for the
platform the package's own dispatch names. That is a one-line fix and a corrected
sentence, and it should be made before the train moves — not because the cure is
in doubt, but because the evidence for it should exist everywhere the cure does.

**Worktree left byte-clean at `6688f34`** (`git status --porcelain` empty); FAT32
probe volume detached and deleted; scratch extracts and the second and third
target directories removed.
