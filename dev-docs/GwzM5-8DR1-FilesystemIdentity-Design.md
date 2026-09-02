# GwzM5-8 DR-1 — filesystem identity: inventory, work-arounds, and the graded-evidence design

Date: 2026-09-03. Author: the implementation lane. Status: **DRAFT — for the
design round's dual** (amendment-tier line, `GwzM5-8DR1-Charter.md:18-19`).
Object under investigation: gwz-core `ffd4f95` (v0.13.0), workspace root
`97805d0`. Every claim below cites `file:line` at that commit. Read-only
investigation; no production change is proposed as landed here.

Home: R2-E phase E8 per `GwzM5-8DR1-Charter.md:6-10` (lane owner's default; the
operator has not named it).

---

## 0. The ask, and the one-paragraph answer

**The ask, verbatim** (`GwzM5-8DR1-Charter.md:1`, `:11-12`): *"run the DR to see
what we can do to work-around the filesystem type checks."* Context: since
v0.13.0 the checked-artifact catalog is activated on a `--no-ff` merge start and
refuses on any filesystem without persistent filesystem identity — on Linux
**ext4 only**, enforced by a superblock magic-number test
(`capability/pre_catalog/provider/platform/linux.rs:136-151`) taken *before* the
`FS_IOC_GETFSUUID` ioctl. Fedora's btrfs and RHEL's xfs are refused.

**The answer.** The lane owner's conceptual answer — move the filesystem
dependence out of the writer and into the evidence, probe by attempt, grade the
identity, never refuse on the straight line, degrade in the classifier, bind the
catalog to a gwz-minted instance id — is **the right destination and is
confirmed by the tree**, with one substantive correction and two additions that
the tree forces:

1. **Correction — attempt-based `FS_IOC_GETFSUUID` does not rescue btrfs.**
   Verified against the kernel, not assumed: the ioctl is generic VFS code
   (`fs/ioctl.c:766-777` at v6.9) that answers from `sb->s_uuid` and returns
   `-ENOTTY` whenever `sb->s_uuid_len == 0`. ext4 (`fs/ext4/super.c:5331`), xfs
   (`fs/xfs/xfs_mount.c:66`) and f2fs (`fs/f2fs/super.c:4502`) publish the UUID
   through `super_set_uuid`; **btrfs does not** — it still raw-`memcpy`s into
   `sb->s_uuid` without setting `s_uuid_len`
   (`fs/btrfs/disk-io.c:3364` at v6.9 and v6.12; `:3553`/`:3555` on master), so
   `FS_IOC_GETFSUUID` returns `ENOTTY` on btrfs on every kernel checked. Option
   (a) alone buys xfs and f2fs and **leaves Fedora refused**. It therefore does
   not answer the operator's ask.
2. **Addition — the UUID requirement, not the magic number, is the real gate.**
   The catalog's durable tuple *requires* an external volume UUID
   (`capability/durable_identity.rs:8-12`, `:39-47`); the shipped **legacy**
   probe does not (`identity.rs:312-346` uses `name_to_handle_at` +
   `fstatfs.f_fsid`, with no filesystem-type test at all) and consequently
   already admits btrfs/xfs/zfs on the abort path
   (`GwzM5-8R2E-CapabilityFreeAmendment.md:397-403`;
   `docs/OperationModel.md:141-142`). **Two identity regimes already ship and
   already disagree.** The smallest honest relaxation is to stop requiring the
   UUID as an admission condition, bind the catalog to a gwz-minted instance id
   with the volume fact as *corroboration*, and let the Linux catalog admit
   exactly what the legacy probe admits.
3. **Addition — tier 1 is a leaf-only tier.** "Content identity, available
   everywhere" is true of files and false of directories: `parent_is_current`
   (`observation.rs:162-172`), the rename-domain equality gate
   (`residue.rs:89-101`) and the action-directory checks
   (`provider/namespace_mutation.rs:165-172`) compare *directories*, which have
   no digest. The tier lattice must be **per object class** — directories get
   tier 3 or tier 2 and never tier 1 — or the design promises something it
   cannot deliver.

Consequently: ship **(a0)+(b)** — remove the two gratuitous `require_ext4` calls,
then re-base the Linux catalog identity on the persistent handle plus a minted
instance id — as the smallest honest relaxation that actually answers the ask
(btrfs, xfs, zfs, f2fs, ext4), and run **(c)**, the graded-evidence design, as
the phased program that removes the last refusal. **(d)**, a
`--allow-weak-identity` override, is not recommended: it is subsumed by (c) and
it moves the guarantee question onto the user.

---

## 1. Inventory — every filesystem-type or identity check in gwz-core

"Persistence horizon" is what the *consumer* needs, not what the probe returns.

### 1.1 The catalog's own provider tree (`capability/pre_catalog/provider/platform/`)

| # | Site | Admits / refuses | Evidence used | Guarantee bought | Consumed by | Horizon |
|---|---|---|---|---|---|---|
| I1 | `platform/linux.rs:136-151` `require_ext4` | **refuses everything but ext4**; `fstatfs.f_type != 0xEF53` → `Unsupported(PersistentFilesystemIdentity, "only local ext4 with FS_IOC_GETFSUUID is admitted")` | superblock **magic number** | *proxy* for "the UUID ioctl will answer and the substrate is not tmpfs/overlay/network" | I2 (its only real dependant), and gratuitously I4/I5 | across reboot (via I2) |
| I2 | `platform/linux.rs:153-171` `filesystem_uuid` | refuses on ioctl failure, or `len != 16`, or all-zero | `FS_IOC_GETFSUUID` ioctl (`:15`, `_IOR(0x15,0,17)` = `struct fsuuid2`) | a remount-stable **external volume UUID** | `DurableObjectIdentityV1::LinuxExt4` (I8) | across reboot **and remount** |
| I3 | `platform/linux.rs:173-203` `persistent_handle` | refuses on `name_to_handle_at` failure, `handle_type <= 0`, or length outside `1..=128` | `name_to_handle_at(fd, "", AT_EMPTY_PATH)` | an object identity that **survives rename and reboot** | I8 → C1/C2/C9/C10 | across reboot |
| I4 | `platform/linux.rs:52-67` `parent_mode` | calls `require_ext4` (`:54`), then `FS_IOC_GETFLAGS` for `FS_CASEFOLD_FL` | ioctl | per-directory case-fold mode | `catalog_lease/target.rs:153`, `:221` | within run (re-probed on every revalidate) |
| I5 | `platform/linux.rs:69-90` `rename_domain` | calls `require_ext4` (`:70`), then `statx(MNT_ID)`; refuses if the mask is absent | `statx` **`STATX_MNT_ID`** (VFS-generic, not fs-specific) | "a rename stays on one filesystem" | `catalog_lease/target.rs:154`, `:222` | **within run only** |
| I6 | `platform/macos.rs:118-173` `volume_attributes` | refuses unless `VOL_CAP_FMT_PERSISTENTOBJECTIDS` is both valid and set (`:149-157`), **and** `statfs.f_flags & MNT_LOCAL != 0` (`:166-171`) | `fgetattrlist` volume attrs + `fstatfs` | persistent object ids + **locality** | I8 | across reboot |
| I7 | `platform/windows.rs:100-140` `require_ntfs` | refuses unless `GetVolumeInformationByHandleW` reports exactly `"NTFS"` (`:135-140`) | **filesystem name string** | proxy for 128-bit file ids + volume GUID | I8 | across reboot |
| I8 | `capability/durable_identity.rs:34-85` | value contract: rejects zero UUID / non-positive handle type / handle length outside `1..=128` / empty volume GUID / zero file id | the values I2/I3/I6/I7 returned | *well-formedness*, distinct from substrate capability (`capability.rs:77-87`) | everything in §2 | n/a |
| I9 | `platform/unsupported.rs:27-52` | refuses every probe on any other OS | — | fail-closed | — | n/a |

`require_ext4` (I1) is called from exactly three places — `:54`, `:70`, `:126`.
**Two of them buy nothing**: `parent_mode`'s `FS_IOC_GETFLAGS` and
`rename_domain`'s `statx(MNT_ID)` both refuse on their own when the substrate
cannot answer (`:56-61`, `:83-88`), and `MNT_ID` is a VFS field present on every
filesystem. Only `identity`'s call at `:126` sits in front of something the magic
number is a proxy for, and even there `filesystem_uuid` refuses by itself
(`:158-169`). **`require_ext4` has no consumer that its successors do not
already protect** — except as an implicit refusal of tmpfs/overlay/network,
which is a *different* property wearing an identity probe's clothes (§3.1, §5).

### 1.2 The legacy probe (`checked_artifact/identity.rs`) — the second regime

| # | Site | Admits / refuses | Evidence | Guarantee | Horizon |
|---|---|---|---|---|---|
| I10 | `identity.rs:312-346` | **no filesystem-type test**; refuses only if `name_to_handle_at` fails or the handle length is out of range | `name_to_handle_at` + `unix::mounted_filesystem_id` (`:279-290`, `fstatfs.f_fsid`, **first 8 bytes only**) | durable object identity with a *truncated, undocumented* volume corroborator | across reboot |
| I11 | `identity.rs:356-367` `rename_domain` | refuses if `STATX_MNT_ID` mask is absent | `statx(MNT_ID)` | one rename domain | within run |
| I12 | `identity.rs:405-486` (macOS) | gated on `VOL_CAP_FMT_PERSISTENTOBJECTIDS` (`:468-472`) — **no `MNT_LOCAL` test**, unlike I6 | `fgetattrlist` | persistent object ids | across reboot |
| I13 | `identity.rs:60-66` `name_digest` | — | `SHA256(DurableObjectIdentity::encode())[..16]` | the durable identity is **committed to the directory namespace as a filename** | across reboot |

I10's `f_fsid` is a weaker corroborator than I2's external UUID: 8 bytes of a
16-byte value, with no kernel stability contract, and on tmpfs it is
`uuid_to_fsid(s_uuid)` of a per-mount UUID (`mm/shmem.c:3550`) — i.e. volatile.
**The legacy family is already at a lower identity tier than the catalog today**,
undeclared. Any graded design must reconcile the two, and
`GwzM5-8R2E-SemanticsAmendment-DRAFT.md:939-947` records that they are separate
module trees on purpose.

### 1.3 Other filesystem-property gates

| # | Site | Property | Behaviour | Horizon |
|---|---|---|---|---|
| I14 | `durable_fs.rs:53-86` `rename_noreplace` (Linux) | `renameat2(RENAME_NOREPLACE)` | already **attempt-based**: returns the raw errno; no fs-type test | within run |
| I15 | `durable_fs.rs:88-105` (macOS) | `renamex_np(RENAME_EXCL)` | attempt-based | within run |
| I16 | `durable_fs.rs:107-118` | other OS | hard `Unsupported` | n/a |
| I17 | `residue.rs:89-101` | `rename_domain(managed) == rename_domain(private)` | typed `UnsupportedOperation`: *"managed and private parents are not in one atomic rename domain"* | **within run only** |
| I18 | `capability.rs:91` `PlatformCapability::RuntimeAdvisoryLock` | lock reliability | a *separate, already-minted* capability value — see §4.3 | within run |
| I19 | `docs/OperationModel.md:118-121` | advisory-lock reliability | documented policy, no code gate | — |
| I20 | `scripts/linux_identity_probe/provider.py:154-155`, `:240`, `:28-40` | the **R0-L release gate**: `filesystem != "ext4"` → unsupported; `provider != "FsIocGetFsUuid"` → reject; a 15-row negative table including real overlay and tmpfs mounts | CI evidence contract, both release architectures | release-blocking |

### 1.4 Where the refusal reaches the user

`capability.rs:66-71` `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY` (the one actionable
sentence; only `PersistentFilesystemIdentity` carries one — `capability.rs:98-103`,
pinned by `interface_tests/contracts.rs:150-165`) → `entry.rs:405-426`
`render_catalog_refusal` → `ErrorCode::UnsupportedOperation`. Mirrored in
`docs/OperationModel.md:139-166` and `dev-docs/GwzReleaseNotes-v0.13.0.md:18-24`.

> **Doc defect found in passing.** `docs/OperationModel.md:141-143` contradicts
> itself inside one sentence: *"the weaker legacy identity probe, which also
> admits btrfs, xfs and zfs — btrfs, xfs and zfs are refused"*. The second
> clause is pre-amendment text left standing. Fix regardless of which option is
> taken.
>
> **Code-comment defect found in passing.** `platform/linux.rs:351` says
> `FS_IOC_GETFSUUID` reports `ENOTTY` on "pre-6.8 kernels". Verified off by one:
> the ioctl is absent from `include/uapi/linux/fs.h` at v6.8 and present at v6.9
> (`fs.h:236-237`). The floor is **6.9**.

---

## 2. Consumers of durable identity — what each needs, and what breaks per tier

Tier definitions used throughout: **T3** = persistent handle + mount/volume id
(reboot-durable); **T2** = session identity, `dev`+`inode`(+`ctime`+`size`)
(stable across process exit, **not** across remount/reboot); **T1** = content
identity, `size`+`digest` (**leaves only** — directories have no digest).

| # | Consumer | Fields read | Recorded where | Horizon needed | At T2 | At T1 | User sees |
|---|---|---|---|---|---|---|---|
| C1 | `residue.rs:183-206` — recompute `name_digest` and match the persisted dirent (`goal_name`/`source_name`, minted `residue.rs:390-394`, `transition.rs:280-284`) | full `DurableObjectIdentity`, hashed | **the filename** in `.gwz/checked-artifacts` | **reboot** | **breaks** — digest changes ⇒ `foreign = true` ⇒ `classify_*` `Ambiguous` (`classification.rs:141-143`) and `"foreign family state prevents authority publication"` (`residue.rs:242-248`); the private area is orphaned | same, plus name collisions between equal-content objects | a hard refusal on resume, with residue nothing can reclaim |
| C2 | `classification.rs:155-174` — `authority.retained_parent_identity == parent_identity.durable` and `matches_request` | parent durable identity; `artifact_root_identity` (`authority.rs:110`); `family_key` = hash of root identity (`authority.rs:196-202`) | the on-disk `.authority` record and **its own filename** (`authority.rs:226-228`) | **reboot** | **breaks** — `authority_current == false` ⇒ `Ambiguous` (`:175-177`) ⇒ `"replacement evidence is ambiguous"` / `"removal evidence is ambiguous"` (`transition.rs:45-51`, `:121-127`); no crashed mutation is ever resumable | same, plus the authority file's *name* stops resolving | `gwz merge --resume` refuses; `--abort` may too |
| C3 | `classification.rs:178-186`; `residue.rs:213-219`; `transition.rs:220`, `:259`; `cleanup.rs:104` — retained source vs live source | file durable identity vs `RetainedSource::Existing` (`authority.rs:32`, encoded `:136-142`) | the `.authority` record | **reboot** | **breaks** — `Ambiguous`, or hard `"quarantined source identity changed"` (`transition.rs:220-226`) | **worse than breaking** — two equal-content files are indistinguishable, so a detach could retire the wrong object | refusal (T2) / silent wrong-object risk (T1, unacceptable) |
| C4 | `classification.rs:204-233` alias rows (`SameSourceAlias`/`SameGoalAlias`) | full `ObjectIdentity`, both live | — | within run | survives | **breaks** — `RecoverableDuplicate{Source,Goal}` (`:263-270`) unreachable ⇒ `_ => Ambiguous` (`:275`) | hardlink-duplicate resume becomes a refusal |
| C5 | `observation.rs:162-172` `parent_is_current` | full `ObjectIdentity` of a **directory** | — | within run | survives | **breaks — no content analogue** ⇒ `classify_exact` short-circuits `Ambiguous` (`classification.rs:137-139`) | every checked classification refuses |
| C6 | `platform.rs:211-218` sealed publication verifier | full `ObjectIdentity` through the retained rename handle | — | within run | survives | **breaks** — the TOCTOU cure the file claims (`:196-198`) is lost | a substituted equal-content object publishes undetected |
| C7 | `residue.rs:578-613` `rebarrier_exact` (`before.identity != after.identity`, `:606`) | full `ObjectIdentity` | — | within run | survives | breaks as C6 | as C6 |
| C8 | `residue.rs:89-101` rename-domain equality (I17) | `RenameDomainProof` (`stx_mnt_id` / `f_fsid` / volume GUID) | never persisted | **within run only** | **survives — this is exactly what T2 provides** | **breaks — no content analogue** | typed `UnsupportedOperation` |
| C9 | `platform/anchor.rs:316`, `:490-506` — anchor filename ↔ `name_digest` | `DurableObjectIdentity`, hashed | **the anchor filename**, permanent | **reboot** | **breaks** ⇒ `AnchorState::Invalid` ⇒ **hard refusal**, not `Ambiguous`: `"private durability anchor is missing or ambiguous while family state exists"` (`anchor.rs:239-246`) | same | hard refusal on every later barrier |
| C10 | **Catalog.** `protocol/catalog_bootstrap_record.rs:172-185` `matches_attempt`; `catalog/scratch.rs:19-77` (digest in the **filename**); `catalog/classifier.rs:96-104`, `:301-319`; `provider/digests.rs:110-134` | `DurableObjectIdentityV1` (external UUID / APFS volume UUID / NTFS volume GUID) + persistent handle, hashed into `DurableCatalogTargetDigestV1`; plus raw `retained_parent_identity` and `support_profile` | the bootstrap record **and** the scratch filename | **reboot and remount, by design** — this is the only place the *external* UUID is used, precisely for remount stability | **breaks** — `accepts` false ⇒ `merge_record` false (`classifier.rs:309-311`) ⇒ `Ambiguous` (`:274`) ⇒ `activate_workspace_catalog` refuses (`entry.rs:313-317` → `:405-426`); a half-built catalog is unreclaimable | same, plus `record_from_scratch:64` fails | `--no-ff` start refuses with the remedy sentence |
| C11 | `provider/namespace_mutation.rs:159-172`, `:219-250` | `DurableObjectIdentityV1` of an **action directory** | admitted-action record | within run (`execute_edge`) / **across runs** (`revalidate`) | `revalidate` breaks: `"retained action directory is no longer the named action directory"` | breaks (directory) | typed ambiguity |
| C12 | `workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs:166-187` — the forward publication observer | **none.** `symlink_metadata` type/mode + `Sha256` of contents | — | n/a | no change | no change — **already T1** | — |
| C13 | `workspace_ops/merge/abort/evidence.rs:343-361` `classify_file` | none directly; inherits C5 through `artifact_facts::observe` | — | n/a | no change | inherits C5 ⇒ `FileState::Other` ⇒ `V1EvidenceRollbackObservation::Ambiguous` ⇒ `"publication-evidence rollback is not at the exact before state"` (`:131-137`), `"…not at an exact rollback-representable state"` (`:44-50`) | `--abort` refuses |
| C14 | `abort/evidence.rs:307-311` `classify_remove` → `root/artifact_facts.rs:77-89` → `entry.rs:73-82` → the whole C1–C3 machinery; mapped by `transition_file` `:325-332` | as C1–C3 | as C1–C3 | reboot | as C1–C3 | as C1–C3 | as C13 |
| C15 | `root/artifact_facts.rs` (98 lines) | **none of its own** — `RegularFileFact` is `Missing \| Bytes \| Invalid` (`:9-13`), every function delegates to `entry` (`:28`, `:45`, `:60`, `:74`, `:83`) | — | n/a | pass-through | pass-through | — |

### 2.1 The four places that genuinely need reboot-durable identity

C1, C2/C3, C9, C10 — and all four for the *same structural reason*: **the
identity is committed to a persisted dirent or to a record that outlives the
process.** Everything else (C4–C8, C11) needs only within-run stability, which
T2 already provides. This is the load-bearing fact for the whole design: the
durable-identity requirement is not spread through the subsystem, it is
concentrated in four naming/record decisions, and each of them can be re-based
onto a gwz-minted name without weakening anything.

### 2.2 The failure mode is bimodal — say so in the design

- **Classification-layer** degradation yields `Ambiguous`, which *looks*
  recoverable and is terminal (`transition.rs:45-51`, `:121-127`).
- **Hard typed refusals** never reach a classification table at all: the anchor
  (`anchor.rs:239-246`), the rename-domain gate (`residue.rs:93-101`), and the
  catalog (`entry.rs:405-426`).

A graded design that only teaches the classifier about tiers will still hit the
second class. Both must be addressed (§4.4).

---

## 3. The work-arounds, ranked by size and risk

LOC estimates are production + test + tooling, in the house's < 500-LOC-per-step
idiom (`GwzM5-8DR1-Charter.md` §4 item 6).

### 3.0 (a0) — Delete the two gratuitous `require_ext4` calls. **~10 LOC. Zero guarantee change.**

**Changes:** remove `require_ext4(...)` at `platform/linux.rs:54` (`parent_mode`)
and `:70` (`rename_domain`). **Stays:** the call at `:126`, and `require_ext4`
itself.

**Guarantee lost: none.** `FS_IOC_GETFLAGS` refuses on its own (`:56-61`) and
`statx(MNT_ID)` refuses on its own (`:83-88`); `MNT_ID` is a VFS field, present
on every Linux filesystem. On a non-ext4 substrate the outcome is unchanged
because `identity` still refuses first — `catalog_lease/target.rs:142-143` calls
`dir_identity` **before** `parent_mode` (`:153`) and `rename_domain` (`:154`).

**Moves:** the tree digest
`check_checked_artifact_boundaries.py:265`
(`"checked_artifact/capability/pre_catalog.rs"`). Nothing else. Not
amendment-tier. It is a prerequisite hygiene step for (a), (b) and (c), and it
makes the *one* real gate visible instead of three.

### 3.1 (a) — Replace the magic number with attempt-based detection. **~350 LOC. Does not answer the ask.**

**Changes:** delete `require_ext4` (`linux.rs:136-151`) and its call at `:126`;
let `filesystem_uuid` (`:153-171`) be the probe; keep its zero/short rejection.
Rename `SupportedFilesystemProfile::LinuxExt4FsIocGetFsUuidV1`
(`capability.rs:25`) and `CheckedFilesystemProfile.linux_ext4_fs_ioc_getfsuuid_v1`
(`protocol/checked_artifact.taut.py:35`) to a substrate-neutral name **at the
same wire value 0**.

**What it admits — VERIFIED against the kernel:**

| Filesystem | `FS_IOC_GETFSUUID` | Evidence |
|---|---|---|
| ext4 | **yes** | `fs/ext4/super.c:5331` `super_set_uuid` |
| xfs | **yes** | `fs/xfs/xfs_mount.c:66` `super_set_uuid` ("Publish UUID in struct super_block") |
| f2fs | **yes** | `fs/f2fs/super.c:4502` `super_set_uuid` |
| **btrfs** | **NO — `ENOTTY`** | raw `memcpy(&sb->s_uuid, …)` without `super_set_uuid`, so `s_uuid_len == 0`: `fs/btrfs/disk-io.c:3364` (v6.9, v6.12), `:3553`/`:3555` (master) |
| tmpfs | **no** at v6.12; **YES on master** (`mm/shmem.c:5049` `super_set_uuid`) | a *new admission*, of a volatile substrate |
| zfs, overlayfs, NFS/SMB/FUSE | UNVERIFIED — out-of-tree or not checked; the generic rule is "answers iff the driver calls `super_set_uuid`" |

Mechanism, verified: `fs/ioctl.c:766-777` (v6.9) —
`ioctl_getfsuuid` reads `sb->s_uuid_len` and returns `-ENOTTY` when it is zero.
Introduced in **v6.9** (absent from `include/uapi/linux/fs.h` at v6.8, present at
v6.9 `:236-237`).

**Verdict.** (a) buys **xfs and f2fs only**. Fedora's btrfs stays refused, so
(a) does not answer the operator's ask. Worse, on a new-enough kernel it *newly
admits tmpfs* — which the R0-L release gate explicitly lists as a required
negative row (`scripts/linux_identity_probe/provider.py:40`) — so (a) cannot
land without an explicit answer to the volatility question (§6 Q4). Take (a0);
take (a) only as a component of (b).

**Moves:** `pre_catalog.rs` tree digest (`:265`); the R0-L probe contract
(`provider.py:28-40`, `:154-155`, `:240`) and its CI evidence schema — a
**release-blocking** gate; `capability.rs:66-71` remedy text and its flat digest
(`check_checked_artifact_boundaries.py:63`); `docs/OperationModel.md:139-166`;
`GwzReleaseNotes` for the next version. If the taut enum member is renamed:
regen + corpus + D0 round-trip + `protocol/check_log_additive.py`'s
`PRE_LOG_WIRE_SHA256`. **Amendment-tier?** No — it widens what the *checked*
probe admits and puts no capability-free operation on the probe, so
`GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §5.2 (`:909-917`) is untouched, and the
operator's ruling `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:10` is not
engaged. It does change the R0-L release evidence contract, which is its own
gate and needs the release lane's sign-off.

### 3.2 (b) — Admit what the legacy probe admits; bind the catalog to a gwz-minted instance id. **~600-750 LOC across 2-3 steps. RECOMMENDED as the relaxation.**

**Changes.**

1. **A new Linux durable identity kind** in `capability/durable_identity.rs:7-21`
   — `LinuxPersistentHandle { handle_type, persistent_handle }` — with **no**
   volume UUID in the identity value. The existing `LinuxExt4` variant is
   **retained**, not replaced: `decode_canonical` re-encodes and compares
   (`durable_identity.rs:160-162`), so old bytes must still round-trip.
2. **A gwz-minted catalog instance id.** A 32-byte nonce created once when the
   catalog is bootstrapped, stored in the bootstrap record
   (`protocol/catalog_bootstrap_record.rs`) and folded into
   `DurableCatalogTargetDigestV1` (`provider/digests.rs:110-134`) in place of the
   UUID's contribution. **The volume fact — `FS_IOC_GETFSUUID` where it answers,
   `f_fsid` otherwise, absent on the rest — is recorded alongside as
   corroboration and is never an admission condition** (the operator's and lane
   owner's formulation, confirmed viable here).
3. **`identity` on Linux** (`platform/linux.rs:122-134`) becomes exactly the
   legacy probe's shape: `name_to_handle_at` (I3) for the durable half, `dev`+`ino`
   for the invocation half, **no fs-type test** — i.e. the two regimes converge
   on one code path, which is the "one protocol, one code path" the lane owner
   asked for.
4. **A matching `SupportedFilesystemProfile` variant** — required, because
   `catalog_bootstrap_record.rs:236` cross-checks
   `retained_parent_identity.support_profile() != value.support_profile`, and the
   profile is folded into the order key (`catalog_lease/target.rs:443-461`) and
   the fresh digest (`digests.rs:69-72`, `:299-305`).

**What it admits:** ext4, xfs, btrfs, zfs, f2fs — everything with
`export_operations`, i.e. **everything the shipped abort path already accepts**
(`GwzM5-8R2E-CapabilityFreeAmendment.md:397-403`). Fedora and RHEL are answered.

**Guarantee lost, and where the user learns it.** One: cross-volume
disambiguation. Today the external UUID makes a byte-identical workspace copied
to a *different* ext4 volume produce a different durable digest. With the nonce,
the copy carries the nonce too — so the discriminator becomes the recorded
corroboration, and a **corroboration mismatch is a recorded tier downgrade plus
a typed refusal at the point of use, not a silent acceptance.** The user learns
it at the same door as today (`entry.rs:405-426`) with a sentence that names the
volume mismatch rather than the filesystem type. Two: on substrates where the
volume fact is absent entirely, the catalog is bound by nonce + handle alone;
that is *stronger* than the legacy probe ships today (which uses truncated
`f_fsid`), so no shipped guarantee is reduced.

**Migration for existing v0.13.0 ext4 catalogs.** Required and cheap:
`matches_attempt` (`catalog_bootstrap_record.rs:172-185`) must accept a record
whose identity is the old `LinuxExt4` kind when the *handle type and handle
bytes* agree, ignoring the UUID field, and re-issue in the new form at the next
publish. Without this, every existing v0.13.0 catalog on ext4 goes `Ambiguous`
and refuses. **This is the single highest-risk detail in (b)** and must have its
own step with a corpus fixture.

**Moves.** Protocol: `protocol/checked_artifact.taut.py:16-19`
(`CheckedDurableIdentityKind`), `:34-37` (`CheckedFilesystemProfile`), `:56-63`
(the identity message), `:103-111` (the bootstrap record) → regen
`protocol/generated.rs`, corpus vectors, `protocol/d0_roundtrip_check.py`,
`check_log_additive.py`'s wire pin. Pins:
`check_checked_artifact_boundaries.py:63` (`capability.rs`), `:265`
(`pre_catalog.rs` tree), `:263` (`catalog_lease.rs` tree), `:266`
(`catalog.rs` tree). R0-L probe contract as in (a). Docs + release notes as in (a).
**Records to name in the amendment:** `GwzM5-8I2CompatibilityContract.md:15-21`
(it freezes *protocol allocation*; a new identity kind and profile variant is an
allocation) — amended by the named-interface mechanism at `:7-13`;
`GwzM5-8R2E-RecordRootAmendment.md:202-217` (the **PLAIN-LEASE PROBE CLAUSE** —
this design *narrows* the probe rather than placing it on a new path, but the
clause's "enumerate lease-reachability BEFORE the build" obligation still binds
each implementing charter). **Not** amended: §5.2's capability-free list
(`GwzM5-8R2E-SemanticsAmendment-DRAFT.md:909-917`) — no capability-free
operation moves onto the probe; **not** the 165-key census
(`fault_expected_keys.rs:174`) — (b) adds no fault key; **not**
`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 — no writer converts, so neither the
`CAPABILITY_FREE_RAW_WRITER_INVENTORY` growth nor its shrinkage arm fires
(`GwzM5-8R2E-CapabilityFreeAmendment.md:207-210`).

**Tier:** amendment-tier, on the protocol allocation, with a Code+State dual.

### 3.3 (c) — The full graded-evidence design. **~2000-2600 LOC over 5 phases. The destination.**

Detailed in §4. Ranked third by *ship order*, first by *product answer*. It
subsumes (a), (b) and (d): the straight line stops refusing anywhere, including
FAT32 and NFS, and the guarantees move into the classifier where they can be
reasoned about and reported.

**Risk concentration:** the two hard-refusal sites that never reach a classifier
(§2.2) and the four naming decisions of §2.1. Both are addressed in §4 but both
are where a graded design silently over-promises if it is not careful.

### 3.4 (d) — A `--allow-weak-identity` flag / config key with a recorded downgrade. **~200-300 LOC. Not recommended.**

**Changes:** a flag on `gwz merge` (`gwz-cli/src/clirequest/merge.rs`), plumbed
to the catalog activation door (`entry.rs:313-317`), that converts the
`Unsupported(PersistentFilesystemIdentity, …)` refusal into an accepted
downgrade recorded in the bootstrap record.

**Why it ranks last as a product answer, despite being small.** It asks the user
to answer a question the system is better placed to answer; it is a *global
process* mode where the evidence needs a *per-object* fact (the classifier
cannot reason about "the user said it was fine"); and it either re-derives (c)'s
recording machinery anyway or degrades without a recordable tier, which §5
forbids. **Its only honest use is as a time-boxed escape hatch** if the operator
needs Fedora unblocked before (b) can be dualled — and even then (a0)+(b) is
smaller in risk terms because it removes a gate rather than adding a bypass.

### 3.5 Ranking summary

| Option | LOC | Admits on Linux | Answers the ask? | Amendment-tier | Risk |
|---|---|---|---|---|---|
| (a0) delete gratuitous gates | ~10 | unchanged | no (hygiene) | no | none |
| (a) attempt-based UUID | ~350 | + xfs, f2fs; **not btrfs**; **+ tmpfs on new kernels** | **no** | no (but moves the R0-L release gate) | volatility regression |
| **(b) instance-id + legacy parity** | **~600-750** | **+ btrfs, xfs, zfs, f2fs** | **yes** | yes (protocol allocation) | the v0.13.0 catalog migration |
| (d) override flag | ~250 | any, on the user's word | technically | no | a bypass with no per-object evidence |
| (c) graded evidence | ~2000-2600 | everything, graded | yes, and permanently | yes (protocol + possibly census) | the two hard-refusal sites; over-promising T1 |

---

## 4. The graded-evidence design (option (c))

### 4.1 The identity type

Replace the closed per-OS enum with an **evidence record carrying its own grade**,
and make the grade *per object class*:

```
IdentityEvidenceV1 {
  tier: IdentityTier,               // 3 | 2 | 1
  persistent: Option<PersistentHandleFact>,   // T3: handle_type + handle bytes
                                              //     (Linux) / persistent object id
                                              //     (macOS) / FileId128 (Windows)
  volume: Option<VolumeFact>,       // corroboration ONLY, never a gate:
                                    //   FS_IOC_GETFSUUID | f_fsid | APFS volume
                                    //   UUID | NTFS volume GUID | absent
  session: SessionFact,             // T2: dev + inode + ctime + size — ALWAYS present
  content: Option<ContentFact>,     // T1: size + digest — LEAVES ONLY
  class: ObjectClass,               // Directory | Leaf
}
```

Three rules the tree forces:

1. **`session` is unconditional.** `dev`+`ino` are already collected on every
   platform (`platform/linux.rs:130-133`, `macos.rs:78-81`,
   `identity.rs:269-277`) and are available on FAT32 and NFS. They are the floor,
   so **no probe ever returns "nothing"** — that is what makes "the straight line
   never refuses" true rather than aspirational.
2. **`class == Directory` ⇒ `content` is `None` and the tier floor is 2.** T1 is
   a leaf-only tier (§0 point 3; C5, C8, C11).
3. **`volume` never gates.** It downgrades or corroborates. This is the exact
   inversion of I1/I2 today.

Tier assignment is by **attempt**, in order, with no filesystem-type test
anywhere: try the persistent handle → if it answers and is well formed, T3; else
try content (leaves only) → T1 evidence is *added*, not substituted; the floor is
always T2. Note that T3 and T1 are **not exclusive** — recording both is strictly
better and costs one digest.

### 4.2 Where the tier is recorded

| Record | Change | Cite |
|---|---|---|
| Authority record | `retained_parent_identity` and `retained_source` become `IdentityEvidenceV1`; the record carries the **minting tier** | `authority.rs:32`, `:101-119`, `:136-142` |
| Goal / staged goal | the name is minted from a **gwz-minted artifact nonce**, not from `name_digest` | `residue.rs:390-394`, `transition.rs:280-284` |
| Anchor | same — nonce-named, evidence recorded inside | `platform/anchor.rs:316`, `:490-506` |
| Catalog bootstrap record | instance id (from (b)) + evidence + tier | `protocol/catalog_bootstrap_record.rs:172-185` |
| Protocol | new message + enum; `CheckedDurableIdentityKind` retained for decode | `protocol/checked_artifact.taut.py:16-19`, `:56-63` |

**The naming change is the heart of it.** C1, C9 and C10 need reboot-durable
identity *only because the identity is the filename*. Once the name is a
gwz-minted nonce recorded in the authority/bootstrap record, the reboot
requirement evaporates for the *naming* purpose and survives only for the
*verification* purpose — which is exactly where a tier can degrade the
conclusion instead of destroying the name. This is the single structural move
that makes (c) work.

### 4.3 The classifier's tier-aware rules

Guarantees degrade in one place, `classification.rs`, plus the two hard-refusal
sites:

- **T3, both sides:** unchanged. Exact automatic recovery — every existing arm
  (`classification.rs:155-174`, `:178-186`, `:204-233`) keeps today's meaning.
- **T2, both sides, same mount session** (proved by the recorded `dev` and the
  live `stx_mnt_id` agreeing): exact, automatic. Outside the session: not
  `Ambiguous` — a **new typed outcome**, `IdentityUnprovable { recorded_tier,
  live_tier, what_would_prove_it }`, so the user gets an actionable sentence
  instead of "evidence is ambiguous".
- **T1 (leaves):** content comparison decides `Baseline` / `Candidate` /
  `Other` — which is *already* what `abort/evidence.rs:343-361` and
  `publication/live.rs:166-187` do, so C12/C13 need no change at all. Where
  content is equal but identity is unproven, the correct answer is **an
  actionable typed refusal, never unattended recovery** — `RecoverableDuplicate*`
  (`classification.rs:263-270`) must not be reachable at T1 (C4), and a T1
  detach must not retire an object (C3's wrong-object hazard).
- **Directories:** T2 floor; `parent_is_current` (C5), the rename-domain gate
  (C8) and the action-directory checks (C11) run unchanged on the session fact.
- **The two hard-refusal sites** (§2.2) must be converted to the same typed
  vocabulary: the anchor (`anchor.rs:239-246`) becomes tier-aware; the
  rename-domain gate (`residue.rs:93-101`) stays a refusal, because it is a
  *correctness* precondition for atomic publication, not an identity question —
  **and it needs no tier at all**, since `statx(MNT_ID)` is VFS-generic.
- **Locality / lock reliability moves out of identity entirely.** macOS's
  `MNT_LOCAL` test (`macos.rs:166-171`) and Windows' NTFS name test
  (`windows.rs:135-140`) are not identity facts; they are the "is this substrate
  safe for concurrent mutation" question, which already has its own capability
  value (`capability.rs:91` `RuntimeAdvisoryLock`) and its own documented policy
  (`docs/OperationModel.md:118-121`). Give them that home. This is what keeps
  the design free of per-filesystem branches while still refusing network mounts
  **for the reason that is actually true**.

### 4.4 Interaction with the two open observer classes — they do not change

**R2, the detach/observation-dead window** (`GwzM5-8R2E-E4-Close.md:43`;
`finalization/execute.rs:48`, `:51`). The refusal is that a `Bytes → Bytes`
replacement detaches before publishing and the observers refuse to classify the
absence. That is a *window in time*, not an identity strength. **Tiers do not
change it.**

**R3, the directional residue** (`GwzM5-8R2E-E4-Close.md:44`;
`GwzM5-8R2E-E45B-Report.md:35`). `classify_remove` returns `Ambiguous` because
`classification.rs:175-177` sees an authority record whose `matches_request` was
minted for the FORWARD pair while the abort asks the REVERSE pair. The mismatch
is in the *pairing*, not in `DurableObjectIdentity` acquisition. **Tiers do not
change it either.** Its cure remains the 450-600 LOC abort-side observer
reconciliation on the capability-free plain lease (`E45B-Report.md:39`).

Two second-order interactions must nonetheless be stated in the design, because
they are real:

1. **Population.** Tiering admits more substrates to the checked door, so more
   users can reach R2 and R3. The cures become more urgent, not different.
   Sequencing note in §7.
2. **Threading.** If the tier is recorded in the authority record (§4.2), then
   `matches_request` (`authority.rs:101-119`) — the very predicate R3's mismatch
   lives in — must thread the tier. **Design rule: a tier *change* between
   minting and reading (a remount that drops T3 to T2) must degrade the
   classification, never make the record foreign.** Get this wrong and tiering
   *creates* a new R3-shaped bug on every remount.

### 4.5 Catalog re-binding and migration

As (b) §3.2, plus: the bootstrap record gains `tier` and `evidence`; the scratch
filename (`catalog/scratch.rs:19-46`) is minted from the instance id, so a
half-built catalog stays reclaimable across a tier change. Existing v0.13.0 ext4
catalogs are accepted through the handle-subset rule and re-issued at the next
publish — this needs a corpus fixture and its own step.

### 4.6 Fault-injection coverage — ext4 alone drives every tier

Per the operator's ruling, no FAT32 lab
(`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:19`). Inject "tier unavailable"
at the **primitive seam** — the four `DurableIdentityProvider` methods
(`capability.rs:169-189`) and the legacy `identity::platform` module — so that on
ext4 the suite can force `persistent = None` (→ T2) and
`persistent = None, class = Leaf` (→ T1), and can force a *tier transition*
between two observations (the §4.4 rule 2 case).

**Recommended seam: a test-only provider substitution at `HostPlatform`
(`capability/pre_catalog/provider.rs:156`, re-exported at
`capability/pre_catalog.rs:25`), not new fault keys.** Reason: the
fault census is frozen at `EXPECTED_KEY_COUNT = 165`
(`interface_tests/fault_expected_keys.rs:174`;
`GwzM5-8R2DSettledTuple.md:114`, `:349-357`), and moving it is amendment-tier on
its own (`GwzM5-8DR1-Charter.md:86`). A provider substitution gets the same
coverage without spending that dual. Operator question §6 Q6.

Coverage rows required: every §2 consumer × {T3, T2 in-session, T2 out-of-session,
T1 leaf}, plus directory-class × T1 must be *statically unreachable* (a
type-level guarantee, not a test).

### 4.7 The abort sentence and the remedy string, rewritten

`capability.rs:66-71` today names filesystems. Under (c) it names **claims**, not
admission — which is the operator's "refusal attaches to CLAIMS an operation
needs, not to admission":

> this operation needs *&lt;claim&gt;* — &lt;e.g. "unattended recovery of an interrupted
> merge across a reboot"&gt; — which needs identity this filesystem cannot prove
> (recorded tier &lt;n&gt;: &lt;what is available&gt;). &lt;What the user can do: re-run in the
> same session; complete the merge now; or run the workspace on a filesystem that
> exposes persistent file handles.&gt;

`docs/OperationModel.md:139-166` loses "**On Linux the admitted filesystem is
`ext4` and nothing else**" (`:140-141`) and the self-contradicting sentence at
`:142-143`, and gains a claims-and-tiers table. The **"What never refuses"**
paragraph (`:150-161`) grows rather than shrinks — under (c) the straight line
never refuses at all, and the abort's scoped clause
(`GwzM5-8R2E-CapabilityFreeAmendment.md:393-397`) collapses into the general
rule, which is the cure §6 of that amendment names.

Release notes: `GwzReleaseNotes-v0.13.0.md:18-24`'s "ext4 only for now" is
superseded; the next notes should state the tier table and, honestly, which
recoveries are automatic at which tier.

### 4.8 Sizing by phase

| Phase | Content | LOC |
|---|---|---|
| P0 | (a0): delete the two gratuitous gates; fix the two doc/comment defects | ~30 |
| P1 | `IdentityEvidenceV1` + protocol allocation + provider rework to attempt-based, all three OSes; **no consumer change** (T3 only, behaviour identical) | ~450 |
| P2 | Nonce-based naming for goal/source/anchor (C1, C9); the catalog instance id and the v0.13.0 migration (C10) | ~500 |
| P3 | Tier-aware classifier: the new typed outcome, the T2 in-session rule, the T1 leaf rules, the two hard-refusal conversions | ~450 |
| P4 | Locality/lock split out of identity into `RuntimeAdvisoryLock` (§4.3); macOS `MNT_LOCAL` and Windows NTFS tests re-homed | ~250 |
| P5 | Fault/substitution coverage matrix; R0-L probe contract rewrite; docs, remedy strings, release notes | ~450 |

Total ~2130 LOC. P1 and P2 are the ones that must not be merged into one step.

---

## 5. What NOT to do

1. **Do not put the identity probe on any capability-free operation.** Ordinary
   merge, `commit`, `repo create`, `init-from-sources`, abort and GC stay off the
   probe — `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:10` ("Do not amend the
   list to put those operations on the catalog probe") and
   `GwzM5-8R2E-SemanticsAmendment-DRAFT.md:909-917`. Nothing in §3 or §4 does
   this, and the design must say so explicitly in its own charter, because
   `GwzM5-8R2E-RecordRootAmendment.md:202-217` (the PLAIN-LEASE PROBE CLAUSE)
   requires a lease-reachability enumeration *before the build*.
2. **Do not weaken the ordinary path.** It is already capability-free; a graded
   design must leave it byte-identical. The `CAPABILITY_FREE_RAW_WRITER_INVENTORY`
   fires on **shrinkage** as well as growth
   (`GwzM5-8R2E-CapabilityFreeAmendment.md:207-210`) — a conversion that quietly
   improves a carved arm is a violation.
3. **Do not degrade silently.** Every tier below 3 is recorded *in the evidence*,
   and every decision that a lower tier changed is reported. A downgrade that
   only shows up as a different classification outcome is the failure mode this
   whole design exists to avoid.
4. **No per-filesystem branches — in either direction.** Not a magic-number
   whitelist (today's `require_ext4`), and **not a magic-number blacklist** of
   tmpfs/overlay/NFS to replace it. If a substrate must be refused, refuse it for
   the property that is actually missing, through the capability that names that
   property (§4.3).
5. **Do not promise T1 for directories.** No content analogue exists (C5, C8,
   C11). Make it a type error, not a runtime surprise.
6. **Do not let T1 drive unattended recovery.** Equal content is not equal
   identity; C3's detach path could retire the wrong object. T1 gets an
   actionable typed refusal.
7. **Do not adopt `f_fsid` as the durable volume fact.** Eight truncated bytes of
   a 16-byte value (`identity.rs:279-290`), no stability contract, and volatile
   on tmpfs (`mm/shmem.c:3550`). It is acceptable *corroboration*; it is not an
   identity.
8. **Do not build a FAT32 lab** (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:19`).
   Coverage comes from tier injection on ext4 (§4.6).
9. **Do not make the tier a global process mode.** Option (d)'s shape. The
   classifier needs a per-object fact.

---

## 6. Open questions for the operator — one line each

1. **Home:** is DR-1 R2-E phase E8 (the lane owner's default,
   `GwzM5-8DR1-Charter.md:6-10`) or a new lane?
2. **Scope of the relaxation:** ship (a0)+(b) now as the answer to Fedora/RHEL,
   or hold everything for (c)?
3. **Network mounts:** should NFS/SMB stay refused by policy once identity no
   longer refuses them — i.e. does the locality test survive as a separate
   `RuntimeAdvisoryLock`-class capability (§4.3)?
4. **tmpfs:** admit it (the artifacts die with the catalog, so the durability
   claim is vacuous) or keep the R0-L negative row
   (`scripts/linux_identity_probe/provider.py:40`)?
5. **Protocol allocation in a patch release:** (b) adds an identity kind and a
   profile variant — patch, or hold for a minor?
6. **Tier-injection seam:** test-only provider substitution (no census move), or
   fault keys (moves `EXPECTED_KEY_COUNT = 165`, amendment-tier dual)?
7. **`--allow-weak-identity`:** wanted at all, given (c) makes it unnecessary and
   §5 item 9 argues against its shape?
8. **R0-L release gate:** its ext4-only evidence contract is release-blocking —
   who owns rewriting it, this lane or the release lane?

---

## 7. Recommended sequencing

**Ship first — the smallest honest relaxation:**

1. **(a0)** — delete the two gratuitous `require_ext4` calls; fix the
   `OperationModel.md:142-143` contradiction and the `linux.rs:351` kernel-version
   comment. ~30 LOC, no dual beyond the ordinary landing gate. It makes the one
   real gate visible.
2. **(b)** — the Linux catalog identity re-based on the persistent handle plus a
   gwz-minted instance id, volume fact as corroboration, in two steps: the
   protocol allocation and provider rework first, the migration of existing
   v0.13.0 ext4 catalogs second. This is the step that answers the operator's ask
   — btrfs, xfs, zfs, f2fs, ext4 — and it does so by *removing* a gate rather
   than adding a bypass. Amendment-tier dual on the protocol allocation.

**Then, as the program:**

3. **(c) P1-P2** — `IdentityEvidenceV1` and nonce-based naming. P1 lands with no
   behaviour change (T3 only); P2 removes the reboot requirement from the four
   naming decisions of §2.1. This is where the design stops being a widening and
   becomes the architecture.
4. **The R3 cure (directional residue) before, or with, (c) P3.** Tiering widens
   the population that reaches R3 (§4.4 rule 1); shipping tier-aware
   classification while `--abort` can still strand on an interrupted marker would
   make the new substrates' first bad day worse than ext4's.
5. **(c) P3-P5** — the tier-aware classifier, the locality split, the coverage
   matrix and the doc/remedy rewrite.

**Not recommended at any point:** (a) on its own (it does not fix btrfs and
regresses the tmpfs refusal) and (d) (subsumed, wrong shape).

---

## Verification notes

- Kernel claims in §0 and §3.1 were verified by reading the kernel sources at the
  tags named (`fs/ioctl.c`, `include/uapi/linux/fs.h`, `fs/{ext4,btrfs,f2fs}/…`,
  `fs/xfs/xfs_mount.c`, `mm/shmem.c` at v6.8 / v6.9 / v6.12 / master), not from
  memory.
- **UNVERIFIED**, flagged as such: `FS_IOC_GETFSUUID` behaviour on zfs, overlayfs,
  NFS, SMB/CIFS and FUSE; whether any distro kernel backports
  `super_set_uuid` into btrfs; whether `name_to_handle_at` is reliably available
  on zfs across the versions gwz targets. Each is a one-command check on a live
  mount and should be measured before (b) lands, not argued.
- **UNVERIFIED**: the LOC estimates in §3 and §4.8 are structural estimates from
  the call sites cited, not from a spike.
- Not run: any test suite, `cargo check`, or `cargo doc` — this was a read-only
  investigation under a 75-minute box.
