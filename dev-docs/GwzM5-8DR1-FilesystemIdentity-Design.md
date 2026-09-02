# GwzM5-8 DR-1 — filesystem identity: inventory, work-arounds, and the graded-evidence design

Date: 2026-09-03. Author: the implementation lane. Status: **REVISION 2,
2026-09-03, folds the dual.** Round 1 was DRAFT (2026-09-03); the peer-blind
dual is filed at `dev-docs/GwzM5-8DR1-FilesystemIdentity-ReviewCode.md`
(CODE/MECHANISM axis, GO-WITH-CONDITIONS, conditions P1-1, P2-1, P2-2, P2-3) and
`dev-docs/GwzM5-8DR1-FilesystemIdentity-ReviewState.md` (STATE/GUARANTEE axis,
GO-WITH-CONDITIONS, conditions [P1-1]–[P1-3], [P2-1]–[P2-6]). **Every condition
from both axes is folded into this revision in place**; where a review refuted a
round-1 claim the claim is corrected here and marked *(fold: …)*.

Object under investigation: gwz-core `ffd4f95` (v0.13.0), workspace root
`97805d0`. Every claim cites `file:line` at that commit; kernel claims cite the
tag read. Read-only; no code, no commits.

Home: R2-E phase E8 per `GwzM5-8DR1-Charter.md:6-10` (lane owner's default; the
operator has not named it).

**Lane owner's adoption (2026-09-03):** the direction is ADOPTED — **(a0) now;
(b) after the folds; (c) the program; (a) and (d) declined.**

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
confirmed by the tree**, with one correction and three additions the tree and
the dual force:

1. **Correction — attempt-based `FS_IOC_GETFSUUID` does not rescue btrfs.**
   Verified at the source, and re-verified by the Code axis: the ioctl is generic
   VFS code (`fs/ioctl.c:766-777` at v6.9) answering from `sb->s_uuid`, returning
   `-ENOTTY` whenever `sb->s_uuid_len == 0`; `s_uuid_len` is zero unless
   `super_set_uuid` sets it (`include/linux/fs.h:2557-2562` v6.9; `alloc_super`
   is `kzalloc`, `fs/super.c:317` v6.9). ext4 (`fs/ext4/super.c:5344` v6.9), xfs
   (`fs/xfs/xfs_mount.c:65` v6.9) and f2fs (`fs/f2fs/super.c:4433` v6.9) call it;
   **btrfs does not** — raw `memcpy` into `sb->s_uuid` at
   `fs/btrfs/disk-io.c:3364` (v6.9, v6.12) and `:3553`/`:3555` (master), with no
   `super_set_uuid` anywhere in `fs/btrfs/`. Option (a) buys xfs and f2fs and
   **leaves Fedora refused**.
2. **Addition — the UUID requirement, not the magic number, is the real gate.**
   The catalog's durable tuple *requires* an external volume UUID
   (`capability/durable_identity.rs:8-12`, `:39-47`); the shipped **legacy**
   probe does not (`identity.rs:312-346`: `name_to_handle_at` + `f_fsid`, no
   filesystem-type test) and already admits btrfs/xfs/zfs on the abort path
   (`GwzM5-8R2E-CapabilityFreeAmendment.md:397-403`;
   `docs/OperationModel.md:141-142`). **Two identity regimes already ship and
   already disagree.**
3. **Addition — tier 1 is a leaf-only tier.** "Content identity, available
   everywhere" is true of files and false of directories: `parent_is_current`
   (`observation.rs:162-172`) and the action-directory checks
   (`provider/namespace_mutation.rs:167`, `:238-239`) compare *directories*,
   which have no digest. The lattice is per object class.
4. **Addition, from the dual — (b)'s admission set is wider than "what the
   legacy probe admits", and it opens volatile and network substrates.**
   *(fold: Code P2-1/P2-2, State [P1-3])* tmpfs publishes a random per-mount UUID
   from the ioctl's **first** kernel (`mm/shmem.c:4405` v6.9, `:4696` v6.12,
   `:5049` master) and has `shmem_export_ops`, so (b) admits tmpfs; NFS and most
   FUSE have `export_operations` but no `fileattr_get`, so they reach
   `parent_mode` and are refused by a capability that carries **no remedy**.
   §6 **Q3 (network) and Q4 (tmpfs) are therefore BLOCKING preconditions of (b)**,
   and the locality/volatility answer ships **with** (b), as its own capability,
   never as a filesystem-type branch.

Consequently: ship **(a0)** — the two gratuitous gate calls, plus the three
passing doc/comment defects — then **(b)** — the Linux catalog identity re-based
on the persistent handle plus a **distinct** gwz-minted instance id, with a
dual-tuple migration and a locality/volatility capability in front — as the
relaxation that answers the ask (btrfs, xfs, zfs, f2fs, ext4). **(c)**, graded
evidence, is the program that removes the last refusal. **(a)** alone is refuted
on its own terms; **(d)** is declined.

**One sequencing choice remains open for the operator** *(fold: State [P2-4])*:
(b)-first costs a **second** catalog migration and a **second** protocol dual
when (c) P1 lands; landing (c) P1's protocol shape first as (b)'s vehicle costs
Fedora one step of delay and buys one allocation, one migration, one dual. §6 Q2
puts the trade to the operator; §7 states the lane's recommendation.

---

## 1. Inventory — every filesystem-type or identity check in gwz-core

"Persistence horizon" is what the *consumer* needs, not what the probe returns.

### 1.1 The catalog's own provider tree (`capability/pre_catalog/provider/platform/`)

| # | Site | Admits / refuses | Evidence used | Guarantee bought | Consumed by | Horizon |
|---|---|---|---|---|---|---|
| I1 | `platform/linux.rs:136-151` `require_ext4` | **refuses everything but ext4**; `fstatfs.f_type != 0xEF53` → `Unsupported(PersistentFilesystemIdentity, "only local ext4 with FS_IOC_GETFSUUID is admitted")` | superblock **magic number** | *proxy* for "the UUID ioctl will answer and the substrate is not tmpfs/overlay/network". **It is the only thing refusing tmpfs today** *(fold: Code P2-1)* — I2 admits tmpfs on every kernel ≥ 6.9 | I2 (its only real dependant), and gratuitously I4/I5 | across reboot (via I2) |
| I2 | `platform/linux.rs:153-171` `filesystem_uuid` | refuses on ioctl failure, `len != 16`, or all-zero | `FS_IOC_GETFSUUID` (`:15`, `_IOR(0x15,0,17)` = `struct fsuuid2`, uapi `fs.h:75`/`:237` v6.9) | a remount-stable **external volume UUID** — **except on tmpfs**, where it is `uuid_gen`'d per mount | `DurableObjectIdentityV1::LinuxExt4` (I8) | across reboot **and remount** |
| I3 | `platform/linux.rs:173-203` `persistent_handle` | refuses on `name_to_handle_at` failure, `handle_type <= 0`, or length outside `1..=128` | `name_to_handle_at(fd, "", AT_EMPTY_PATH)` | an object identity that **survives rename and reboot** | I8 → C1/C2/C9/C10 | across reboot |
| I4 | `platform/linux.rs:52-67` `parent_mode` | calls `require_ext4` (`:54`), then `FS_IOC_GETFLAGS` for `FS_CASEFOLD_FL` | ioctl, answered only where `i_op->fileattr_get` exists (`vfs_fileattr_get` → `-ENOIOCTLCMD`, `fs/ioctl.c:526-534`, `:705-713` v6.9) | per-directory case-fold mode | `catalog_lease/target.rs:153`, `:221`; `provider/retained.rs:338-339`; `managed_mutation.rs:171`, `:191`, `:194`, `:291`; `association.rs:150`, `:167`; `interior.rs:72`; `alias.rs:21` | within run (re-probed each revalidate) |
| I5 | `platform/linux.rs:69-90` `rename_domain` | calls `require_ext4` (`:70`), then `statx(MNT_ID)`; refuses if the mask is absent | `statx` **`STATX_MNT_ID`** (VFS-generic, not fs-specific) | "a rename stays on one filesystem" | `catalog_lease/target.rs:154`, `:222` | **within run only** |
| I6 | `platform/macos.rs:118-173` `volume_attributes` | refuses unless `VOL_CAP_FMT_PERSISTENTOBJECTIDS` is valid and set (`:149-157`) **and** `statfs.f_flags & MNT_LOCAL != 0` (`:166-171`) | `fgetattrlist` + `fstatfs` | persistent object ids + **locality** | I8 | across reboot |
| I7 | `platform/windows.rs:100-140` `require_ntfs` | refuses unless `GetVolumeInformationByHandleW` reports exactly `"NTFS"` (`:130-135`) | **filesystem name string** | proxy for 128-bit file ids + volume GUID | I8 | across reboot |
| I8 | `capability/durable_identity.rs:34-85` | value contract: rejects zero UUID / non-positive handle type / handle length outside `1..=128` / empty volume GUID / zero file id | the values I2/I3/I6/I7 returned | *well-formedness*, distinct from substrate capability (`capability.rs:77-87`) | everything in §2 | n/a |
| I9 | `platform/unsupported.rs:27-52` | refuses every probe on any other OS | — | fail-closed | — | n/a |

`require_ext4` (I1) is called from exactly three places — `:54`, `:70`, `:126`
(tree-wide grep confirms no others; Code axis Q1). **Two of them buy nothing**:
`FS_IOC_GETFLAGS` refuses on its own (`:56-61`) and `statx(MNT_ID)` refuses on
its own (`:83-88`), and `MNT_ID` is a VFS field present on every Linux
filesystem. Only `identity`'s call at `:126` sits in front of something the magic
number proxies for — and even there `filesystem_uuid` refuses by itself
(`:158-169`). **`require_ext4`'s only load-bearing effect is the implicit refusal
of tmpfs/overlay/network**, which is a *different property* wearing an identity
probe's clothes (§3.2.4, §5).

### 1.2 The legacy probe (`checked_artifact/identity.rs`) — the second regime

| # | Site | Admits / refuses | Evidence | Guarantee | Horizon |
|---|---|---|---|---|---|
| I10 | `identity.rs:312-346` (Linux) | **no filesystem-type test**; refuses only if `name_to_handle_at` fails or the handle length is out of range | `name_to_handle_at` + `unix::mounted_filesystem_id` (`:279-290`, the whole 8-byte `__kernel_fsid_t`) | durable object identity with an 8-byte volume corroborator | across reboot |
| I11 | `identity.rs:356-367` `rename_domain` | refuses if `STATX_MNT_ID` mask is absent | `statx(MNT_ID)` | one rename domain | within run |
| I12 | `identity.rs:405-486` (macOS) | gated on `VOL_CAP_FMT_PERSISTENTOBJECTIDS` (`:468-472`) — **no `MNT_LOCAL` test**, unlike I6 | `fgetattrlist` | persistent object ids | across reboot |
| **I12w** | `identity.rs:551-575` (Windows) | `FILE_ID_INFO` + `GetFinalPathNameByHandleW(VOLUME_NAME_GUID)` — **no `require_ntfs`**, unlike I7 *(fold: Code P3-3(i))* | Win32 handle queries | 128-bit file id + volume GUID on **any** Windows filesystem answering them | across reboot |
| I13 | `identity.rs:60-66` `name_digest` | — | `SHA256(DurableObjectIdentity::encode())[..16]` | the durable identity is **committed to the directory namespace as a filename** | across reboot |
| I14 | `identity.rs:369-370` `case_sensitive` | never refuses — `Ok(true)` unconditionally | — | the legacy family has **no mode probe at all** | n/a |

**The second regime differs from the first on all three OSes**, not two: no
`require_ext4`, no `MNT_LOCAL`, no `require_ntfs`, and no `parent_mode`.

*(fold: Code P3-2)* Round 1 called I10's corroborator a "truncated `f_fsid`".
Corrected: `identity.rs:279-290` copies the **whole** 8-byte `__kernel_fsid_t`;
the 16→8 reduction is the kernel's own XOR fold `uuid_to_fsid`
(`include/linux/statfs.h:55-59` v6.9: the two `u64` halves of the UUID
XOR-combined), and on ext4 `f_fsid = uuid_to_fsid(es->s_uuid)`
(`fs/ext4/super.c:6829` v6.9) — a deterministic function of the same external
UUID, so it is stable on ext4, not arbitrary. The reason it is *corroboration and
not identity* is therefore **the fold's collision surface plus tmpfs's per-mount
volatility**, not "no stability contract" (btrfs/xfs derivation: UNVERIFIED).

### 1.3 Other filesystem-property gates

| # | Site | Property | Behaviour | Horizon |
|---|---|---|---|---|
| I15 | `durable_fs.rs:53-86` `rename_noreplace` (Linux) | `renameat2(RENAME_NOREPLACE)` | already **attempt-based**: returns the raw errno; no fs-type test | within run |
| I16 | `durable_fs.rs:88-105` (macOS) | `renamex_np(RENAME_EXCL)` | attempt-based | within run |
| I17 | `durable_fs.rs:107-118` | other OS | hard `Unsupported` | n/a |
| I18 | `residue.rs:89-101` | `rename_domain(managed) == rename_domain(private)` | typed `UnsupportedOperation`: *"managed and private parents are not in one atomic rename domain"* | **within run only** |
| I19 | `capability.rs:91` `PlatformCapability::RuntimeAdvisoryLock` | lock reliability | a *separate, already-minted* capability value — the home §3.2.4 and §4.3 use | within run |
| I20 | `docs/OperationModel.md:118-121` | advisory-lock reliability | documented policy, no code gate | — |
| I21 | `scripts/linux_identity_probe/provider.py:154-155`, `:240`, `:28-40` | the **R0-L release gate**: `filesystem != "ext4"` → unsupported; `provider != "FsIocGetFsUuid"` → reject; a 15-row negative table including real overlay and **tmpfs** mounts | CI evidence contract, both release architectures | release-blocking |

### 1.4 Where the refusal reaches the user

`capability.rs:66-71` `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY` (the one actionable
sentence; only `PersistentFilesystemIdentity` carries one — `capability.rs:98-103`,
pinned by `interface_tests/contracts.rs:150-165`) → `entry.rs:405-426`
`render_catalog_refusal` → `ErrorCode::UnsupportedOperation` for the
`Unsupported` arm, **`ErrorCode::IoError` for the `Ambiguous` arm** (`:421-423`)
— a distinction that matters in §3.2.3. Mirrored in
`docs/OperationModel.md:139-166` and `dev-docs/GwzReleaseNotes-v0.13.0.md:18-24`.

### 1.5 Doc and comment defects found in passing — fix at (a0)

1. `docs/OperationModel.md:140-143` contradicts itself inside one sentence:
   *"the weaker legacy identity probe, which also admits btrfs, xfs and zfs —
   btrfs, xfs and zfs are refused"*. The second clause is pre-amendment text left
   standing. Confirmed verbatim by both axes.
2. `platform/linux.rs:351` says `FS_IOC_GETFSUUID` reports `ENOTTY` on "pre-6.8
   kernels". The ioctl is **absent at v6.8 and present at v6.9** (uapi `fs.h:237`),
   so the floor is **6.9** and the comment is off by one.
3. *(fold: Code P3-3(ii))* `src/checked_artifact/platform.rs:184` names
   `require_ext4` as the closed support table in a doc comment; it goes stale the
   moment (a)/(b) lands and must be re-pointed in the same step.

---

## 2. Consumers of durable identity — what each needs, and what breaks per tier

Tiers: **T3** = persistent handle + mount/volume id (reboot-durable); **T2** =
session identity, `dev`+`inode`(+`ctime`+`size`) (stable across process exit,
**not** across remount/reboot); **T1** = content identity, `size`+`digest`
(**leaves only**).

| # | Consumer | Fields read | Recorded where | Horizon | At T2 | At T1 | User sees |
|---|---|---|---|---|---|---|---|
| C1 | `residue.rs:183-206` — recompute `name_digest`, match the persisted dirent (`goal_name`/`source_name`, minted `residue.rs:390-394`, `transition.rs:280-284`) | full `DurableObjectIdentity`, hashed | **the filename** | **reboot** | **breaks by INVISIBILITY, not by `foreign`** *(fold: Code P2-3, State [P2-3])* — a root-identity change alters `family_key` (`authority.rs:196-202`), so `inspect_family`'s prefix filter (`residue.rs:116`, **`:127`**) skips the whole family before the `foreign` logic (`:160-207`) runs | same | see C2 |
| C2 | `classification.rs:155-174` — `authority.retained_parent_identity == parent_identity.durable` and `matches_request` | parent durable identity; `artifact_root_identity` (`authority.rs:110`); `family_key` (`authority.rs:196-202`); the authority filename (`:226-228`) | the `.authority` record **and its own name** | **reboot** | **the record is unfindable**, so classification takes the `authority == false` arms (`classification.rs:247-253`): `Before`/`After` where the managed leaf is intact — **a detached source or staged goal is silently orphaned in the private area** — and `Ambiguous` only via the `_ =>` fall-through (`:275`) where the managed leaf is `Missing`. **This is the silent degradation §5 rule 3 forbids** | same | either a wrong-but-plausible resume with orphaned residue, or `"replacement/removal evidence is ambiguous"` (`transition.rs:45-51`, `:121-127`) |
| C3 | `classification.rs:178-186`; `residue.rs:213-219`; `transition.rs:220`, `:259`; `cleanup.rs:104` | file durable identity vs `RetainedSource::Existing` (`authority.rs:32`, `:136-142`) | the `.authority` record | **reboot** | `Ambiguous`, or hard `"quarantined source identity changed"` (`transition.rs:220-226`) | **worse than breaking** — `RetainedSource::Existing(identity)` is the only thing distinguishing equal-content files at `classification.rs:178-186`; a detach could retire the wrong object | refusal (T2) / unacceptable at T1 |
| C4 | `classification.rs:204-233` alias rows | full `ObjectIdentity`, both live | — | within run | survives | **breaks** — `RecoverableDuplicate{Source,Goal}` (`:263-270`) unreachable ⇒ `_ => Ambiguous` (`:275`) | hardlink-duplicate resume becomes a refusal |
| C5 | `observation.rs:162-172` `parent_is_current` | full `ObjectIdentity` of a **directory** | — | within run | survives | **breaks — no content analogue** ⇒ `classify_exact` short-circuits `Ambiguous` (`classification.rs:137-139`) | every checked classification refuses |
| C6 | `platform.rs:211-218` sealed publication verifier | full `ObjectIdentity` through the retained rename handle | — | within run | survives | **breaks** — the TOCTOU cure the file claims (`:196-198`) is lost | a substituted equal-content object publishes undetected |
| C7 | `residue.rs:578-613` `rebarrier_exact` (`before.identity != after.identity`, `:606`) | full `ObjectIdentity` | — | within run | survives | breaks as C6 | as C6 |
| C8 | `residue.rs:89-101` rename-domain equality (I18) | `RenameDomainProof` (`stx_mnt_id` / `f_fsid` / volume GUID) — **mount ids, not identities** | never persisted | **within run only** | **survives — T2 is exactly what it needs** | **n/a — C8 is not an identity consumer** *(fold: Code P3-4)*; `statx(MNT_ID)` is VFS-generic (`linux.rs:71-89`), so it needs no tier (§4.3) | typed `UnsupportedOperation` only where `MNT_ID` is absent |
| C9 | `platform/anchor.rs:316`, `:490-506` — anchor filename ↔ `name_digest` | `DurableObjectIdentity`, hashed | **the anchor filename**, permanent | **reboot** | **breaks** ⇒ `AnchorState::Invalid` ⇒ **hard refusal**, not `Ambiguous`: `"private durability anchor is missing or ambiguous while family state exists"` (`anchor.rs:239-246`) | same | hard refusal on every later barrier |
| C10 | **Catalog.** `protocol/catalog_bootstrap_record.rs:172-185` `matches_attempt`; `catalog/scratch.rs:19-46`; `catalog/classifier.rs:96-104`, `:301-319`; `provider/digests.rs:108-135` | `DurableObjectIdentityV1` (external UUID) + handle, folded into `DurableCatalogTargetDigestV1`; plus raw `retained_parent_identity` and `support_profile` | the bootstrap record **and** the scratch filename | **reboot and remount, by design** | **breaks** — `accepts` false ⇒ `merge_record` false (`classifier.rs:309-311`) ⇒ `Ambiguous` (`:274`) ⇒ `activate_workspace_catalog` refuses | same, plus `record_from_scratch:64` fails | `--no-ff` start refuses with the remedy sentence |
| C11 | `provider/namespace_mutation.rs:167`, `:238-244` | `DurableObjectIdentityV1` of an **action directory** | admitted-action record | within run / **across runs** on `revalidate` | `revalidate` breaks: `"retained action directory is no longer the named action directory"` | breaks (directory) | typed ambiguity |
| C12 | `.../finalization/publication/live.rs:166-187` — the forward publication observer | **none.** `symlink_metadata` + `Sha256` of contents (`:184-186`) | — | n/a | no change | no change — **already T1** | — |
| C13 | `workspace_ops/merge/abort/evidence.rs:343-361` `classify_file` | none directly; inherits C5 through `artifact_facts::observe` | — | n/a | no change | inherits C5 ⇒ `FileState::Other` ⇒ `Ambiguous` ⇒ `"…not at the exact before state"` (`:131-137`), `"…not at an exact rollback-representable state"` (`:44-50`) | `--abort` refuses |
| C14 | `abort/evidence.rs:307-311` `classify_remove` → `root/artifact_facts.rs:77-89` → `entry.rs:73-82` → C1–C3; mapped by `transition_file` `:325-332` | as C1–C3 | as C1–C3 | reboot | as C1–C3 | as C1–C3 | as C13 |
| C15 | `root/artifact_facts.rs` | **none of its own** — `RegularFileFact` is `Missing \| Bytes \| Invalid` (`:9-13`); every function delegates to `entry` | — | n/a | pass-through | pass-through | — |

### 2.1 The four places that genuinely need reboot-durable identity

C1, C2/C3, C9, C10 — all for the same structural reason: **the identity is
committed to a persisted dirent or to a record that outlives the process.**
Everything else (C4–C8, C11) needs only within-run stability, which T2 provides.

### 2.2 The failure mode is trimodal — say so in the design

*(fold: Code P2-3)* Round 1 named two classes; there are three:

- **Invisibility** — the family prefix filter (`residue.rs:127`) skips the family
  and the `authority == false` arms run (`classification.rs:247-253`). Silent.
  **This is the dangerous one** and it is the class round 1 missed.
- **Classification-layer** degradation → `Ambiguous`, which *looks* recoverable
  and is terminal (`transition.rs:45-51`, `:121-127`).
- **Hard typed refusals** that never reach a classification table: the anchor
  (`anchor.rs:239-246`), the rename-domain gate (`residue.rs:93-101`), and the
  catalog (`entry.rs:405-426`).

A graded design must address all three.

---

## 3. The work-arounds, ranked by size and risk

### 3.0 (a0) — Delete the two gratuitous `require_ext4` calls. **~30 LOC. Zero guarantee change. UNCONDITIONALLY ADOPTED.**

**Changes:** remove `require_ext4(...)` at `platform/linux.rs:54` (`parent_mode`)
and `:70` (`rename_domain`); fix the three defects of §1.5. **Stays:** the call at
`:126`, and `require_ext4` itself.

**Guarantee lost: none — confirmed by both axes on the traced paths.**
`dir_identity` (which still calls `require_ext4` at `:126`) is taken on the same
handle *first* everywhere: `catalog_lease/target.rs:142-143` before `:153-156`;
`managed_mutation.rs:162` before `:171`, `:188` before `:191`/`:194`, `:288`
before `:291`; `retained.rs:337` before `:338-339`; `association.rs:148` before
`:150`/`:167`; `namespace_mutation.rs:166` before `:176`/`:179`. So on a non-ext4
substrate the outcome is unchanged. `interior.rs:72` and `alias.rs:21` take
already-retained handles (identity at `retain_opened`) — traced two levels,
**UNVERIFIED for every caller**.

**Moves:** the tree digest
`scripts/checks/check_checked_artifact_boundaries.py:265`
(`"checked_artifact/capability/pre_catalog.rs"`) — `source_tree_digest` (`:1264`)
walks the subtree, so `provider/platform/linux.rs` is inside that pin. Nothing
else; `linux.rs:386`'s `parent_mode_never_reports_ebadf_for_a_live_capability` is
unaffected. Not amendment-tier. **UNVERIFIED:** whether the R0-L
`mode_query_failure` negative row (`provider.py:33`) is pinned to the ext4
pre-test in `run_probe.py` (Code Q6).

### 3.1 (a) — Replace the magic number with attempt-based detection. **~350 LOC. DECLINED.**

**What it admits — VERIFIED at the kernel, corrected per Code P2-1:**

| Filesystem | `FS_IOC_GETFSUUID` | Evidence (tag stated per cite) |
|---|---|---|
| ext4 | **yes** | `fs/ext4/super.c:5344` (v6.9) `super_set_uuid` |
| xfs | **yes** | `fs/xfs/xfs_mount.c:65` (v6.9) `super_set_uuid` |
| f2fs | **yes** | `fs/f2fs/super.c:4433` (v6.9) `super_set_uuid` |
| **btrfs** | **NO — `ENOTTY`** | raw `memcpy` without `super_set_uuid`: `fs/btrfs/disk-io.c:3364` (v6.9, v6.12); `:3553`/`:3555` (master); no `super_set_uuid` anywhere in `fs/btrfs/` |
| **tmpfs** | **YES, from the ioctl's first kernel** *(fold: Code P2-1 — round 1 said "no at v6.12; yes on master", which was wrong by three releases)* | `shmem_fill_super`: `uuid_gen(&uuid); super_set_uuid(...)` at `mm/shmem.c:4405` (v6.9), `:4696` (v6.12), `:5049` (master) — a **random per-mount** UUID |
| zfs, overlayfs, NFS, FUSE | UNVERIFIED | the rule is "answers iff the driver calls `super_set_uuid`" |

Mechanism verified: `fs/ioctl.c:766-777` (v6.9). Introduced in **v6.9** (absent
from uapi `fs.h` at v6.8; present at `:75`/`:237` v6.9).

**Verdict — DECLINED.** (a) buys **xfs and f2fs only**; Fedora stays refused. And
it admits tmpfs on **every** kernel ≥ 6.9, contradicting the R0-L negative row
(`provider.py:40`) far more broadly than round 1 stated. It is not a step on the
way to (b) either — (b) removes the UUID requirement outright.

### 3.2 (b) — Legacy-parity admission + a distinct gwz-minted instance id. **~950-1100 LOC, THREE steps. RECOMMENDED, after the folds below.**

*(fold: Code P3-5 — round 1's "~600-750 LOC across 2-3 steps" was optimistic once
the migration is priced honestly.)*

#### 3.2.1 What changes

1. **A new Linux durable identity kind** in `capability/durable_identity.rs:7-21`
   — `LinuxPersistentHandle { handle_type, persistent_handle }`, with **no**
   volume UUID in the identity value. `LinuxExt4` is **retained, not replaced**:
   `decode_canonical` re-encodes and compares (`durable_identity.rs:160-162`), so
   old bytes must still round-trip, and the migration (§3.2.2) needs the old
   constructor live.
2. **`identity` on Linux** (`platform/linux.rs:122-134`) becomes the legacy
   probe's shape: `name_to_handle_at` for the durable half, `dev`+`ino` for the
   invocation half, no fs-type test.
3. **A matching `SupportedFilesystemProfile` variant** — required, because
   `catalog_bootstrap_record.rs:236` cross-checks
   `retained_parent_identity.support_profile() != value.support_profile`, and the
   profile is folded into the order key (`catalog_lease/target.rs:443-461`), the
   target digest (`digests.rs:120`) and the fresh digest (`:69-72`, `:299-305`).
4. **A catalog instance id — a SECOND nonce, distinct from the shipped ownership
   token** (§3.2.3).
5. **A typed volume-corroborator field on the wire** (§3.2.3).
6. **A locality/volatility capability in front of the catalog** (§3.2.4).

#### 3.2.2 The migration of existing v0.13.0 ext4 catalogs — DUAL-TUPLE ACCEPTANCE

*(fold: Code P1-1, State [P1-1] — round 1's "relax `matches_attempt` to ignore
the UUID field" was refuted by both axes and would regress every existing ext4
catalog.)*

`CatalogBootstrapRecordV1::matches_attempt`
(`protocol/catalog_bootstrap_record.rs:172-185`) is a **five-way conjunction**:

```
self.root_kind == root_kind
    && self.support_profile == support_profile
    && self.durable_target_digest == durable_target_digest
    && &self.retained_parent_identity == retained_parent_identity
    && &self.retained_parent_path == retained_parent_path
```

Under (b) **three** of the five change, and one of them is a hash that cannot be
"relaxed":

- `support_profile` — (b) item 3 mints a new variant; old records carry
  `LinuxExt4FsIocGetFsUuidV1`.
- `durable_target_digest` — `target_digest`
  (`provider/digests.rs:108-135`) is SHA-256 over the framed tuple: the domain
  tag `"gwz-durable-catalog-target-v1\0"` (`:118`), `root_kind_code` (`:119`),
  **`profile_code`** (`:120`), then for **four** directories — root, repository,
  common directory, private parent (`:121-126`) — each one's
  `identity().durable().encode_canonical()` (`:129`, **UUID inside**) and
  `mode_code` (`:131`), then the durable path (`:133`). Ignoring a field of
  `retained_parent_identity` does nothing to a hash.
- `retained_parent_identity` — an `Eq` on the whole enum value.

And the blast radius extends past the record: `historical_collision_digest` takes
the target digest as input (`digests.rs:137-139`), and the **scratch filename**
embeds target digest, collision digest and ownership token
(`catalog/scratch.rs:25-39`), so an interrupted v0.13.0 bootstrap's scratch entry
parses as `Other` under a new binding.

**The mechanism that works — dual-tuple acceptance (option (i) of State
[P1-1], chosen explicitly).** On ext4 the ioctl still answers, so the activation
door can *recompute the legacy tuple alongside the new one*:

1. At catalog activation, compute the **new-form** target digest, collision
   digest and attempt binding as usual.
2. If no new-form record matches, and the substrate answers
   `FS_IOC_GETFSUUID`, compute the **legacy-form** identity, profile code,
   target digest and collision digest, and run `matches_attempt` against the
   resident record with the legacy tuple. A match is a **migration-eligible
   record**, and the same legacy tuple regenerates the legacy scratch name so an
   interrupted v0.13.0 bootstrap is still recognised.
3. Re-issue the record in the new form as an **admitted action** with its own
   fault rows (§3.2.6), not "at the next publish".

**Consequence to state plainly:** the UUID probe and the `LinuxExt4` kind stay
live as a *migration path* for as long as the migration window is supported, so
§3.2.1 item 2's "the two regimes converge" is true **for new catalogs from day
one and for existing ext4 catalogs only after re-issue**. That is the honest
statement; round 1's unqualified convergence claim is withdrawn.

**Crash-safety.** Re-issue must be atomic with respect to the classifier, because
`merge_record` (`catalog/classifier.rs:301-319`) accepts a second `Exact` record
only when `expected == value` (`:313`). An accepted-old record coexisting with a
new-form record therefore yields `false` → `Ambiguous` → refusal. The window is
closed by making the re-issue a **publish-then-retire admitted action under the
catalog lease** — the new record is published, the old retired, and any crash
leaves exactly one of the two resident; the migration probe recognises either.
Rows: crash before publish (old only, re-migrates), crash between publish and
retire (**both resident — must be classified by the migration arm, not by
`merge_record`'s equality rule**), crash after retire (new only).

**One-way-ness.** A v0.13.0 binary meeting a re-issued record decodes an unknown
identity kind and profile code and fails closed → `Other` → `Ambiguous` →
refusal. **The migration is one-way and the release notes must say so.**
*(UNVERIFIED: that the closed decoder fails rather than defaults — assumed
fail-closed from `decode_canonical`'s round-trip check
(`durable_identity.rs:160-162`) and `catalog_bootstrap_record.rs:236`; must be
proven by a corpus row before the step lands.)*

**Test rows the migration owns** (its own step, ~250 LOC):
(1) legacy record + legacy scratch absent → migrate, re-issue, resume clean;
(2) legacy record + **legacy scratch present** (interrupted v0.13.0 bootstrap) →
recognise both under the legacy tuple;
(3) crash between publish and retire → both records resident → migration arm
classifies, `merge_record`'s equality rule does not fire;
(4) new-form record on a substrate whose UUID ioctl fails (btrfs) → no legacy
probe attempted, no spurious migration;
(5) corpus fixture for the **record** and corpus fixture for the **scratch name**;
(6) a v0.13.0-shaped decoder meeting a new record → fails closed (the one-way pin).

#### 3.2.3 The instance id, the ownership token, and what corroboration does

*(fold: State [P1-2], §6 Q9.)*

**The instance id is a SECOND nonce, distinct from the shipped ownership token.**
`CatalogBootstrapOwnershipTokenV1` already exists
(`protocol/catalog_bootstrap_record.rs:19-38`): 32 cryptographically random
bytes, zero reserved, carried in the record (`:52`) and folded into the scratch
name (`catalog/scratch.rs:38`). **Its adopted boundary stands and is not
re-litigated here**: `bootstrap/managed/provider.rs:51-62` records that a
resumed drive's read-back of the token is *self-consistency* — "this chain is the
one this admitted action's plan describes" — and is **not an adoption or
exclusion proof**, and that "a later step that uses a resident record to decide
adoption of state this action did not create … must re-litigate the token then."

The instance id must therefore be a distinct value with a distinct role:

| | `bootstrap_ownership_token` | `catalog_instance_id` (new) |
|---|---|---|
| Role | self-consistency of one bootstrap chain | **volume-instance binding** of the catalog across runs |
| Boundary | not an adoption/exclusion proof (`provider.rs:51-62`) — **unchanged** | is not an exclusion proof either; it answers "is this the same catalog instance", not "did anyone else write here" |
| Lifetime | one bootstrap drive | the catalog's life |
| Where | record `:52`, scratch name `scratch.rs:38` | record (new field); **not** in the scratch name |

Round 1's §4.5 said "the scratch filename is minted from the instance id" —
**withdrawn**: the scratch name already carries the ownership token, and adding a
second nonce to it would change the name length and the parse
(`scratch.rs:25-39`, `:48-50`) for no gain. The scratch name is unchanged by (b)
except through the digests.

**What a corroborator mismatch does — the round-1 contradiction resolved.**
Round 1 said both "a typed refusal at the point of use" and "never an admission
condition". Both cannot hold. **Resolution:**

> The volume fact **never gates ADMISSION of a fresh catalog** — a substrate that
> cannot answer any volume query still gets a catalog. It **does gate RE-BINDING
> of an existing one**: when a resident record's recorded corroborator and the
> live one are both present and disagree, the catalog refuses, through the
> `Unsupported` arm with a remedy naming the volume mismatch — **not** through
> `Ambiguous`, whose door is `ErrorCode::IoError` and carries no remedy
> (`entry.rs:419-423`). Where either side is absent, the mismatch cannot be
> evaluated and the outcome is acceptance with the *lower* recorded corroborator
> class, recorded on the wire.

**Residual to state in the release notes:** with corroboration absent on both
sides, a same-handle cross-volume image (`dd`/block-level restore to a different
volume) is accepted, where v0.13.0's UUID caught it. Every *ordinary* copy
(`cp`/`rsync`/`tar`) is still caught by the handle, because inodes are fresh.

**The wire field** *(fold: State [P2-2])*. Add a typed corroborator to the
bootstrap record so a reader classifies at the **writer's** class, never at the
class it wishes:

```
VolumeFactV1 = Uuid(bytes[16]) | FsId(bytes[8]) | Absent
```

Decoding rejects an inconsistent pair the way I8 does
(`durable_identity.rs:34-85`): `Uuid` with wrong length, `FsId` with wrong
length, or a corroborator whose class contradicts the identity kind.

*(fold: State [P3-1])* And the general rule this establishes, carried into (c):
**a tier or class is DERIVED from the facts present, never stored as a free
label.** Where a summary field is unavoidable on the wire, the decoder rejects
any pair in which the summary contradicts the facts — so no reader path can hold
`persistent: Some` with `tier: 2`.

#### 3.2.4 What (b) actually admits — and the locality/volatility capability

*(fold: Code P2-2, State [P1-3].)* Round 1 said (b) "admits exactly what the
legacy probe admits … everything with `export_operations`". **Restated exactly:**

> (b) admits every filesystem that answers **both** `name_to_handle_at`
> (`export_operations`) **and** `FS_IOC_GETFLAGS` (`i_op->fileattr_get`), because
> the catalog lease calls `parent_mode` on every retained directory
> (`catalog_lease/target.rs:153-156` and the eight further sites in I4) whereas
> the legacy family has **no mode probe at all** (`identity.rs:369-370`).

Consequences:

- **The named targets pass.** btrfs, xfs, f2fs, tmpfs and zfs have `fileattr_get`
  (`btrfs_fileattr_get` master `fs/btrfs/ioctl.c:273`; `xfs_fileattr_get` v6.9
  `fs/xfs/xfs_iops.c:1121`; `shmem_fileattr_get` v6.9 `mm/shmem.c:3595`).
  Casefold semantics are fine: `FS_CASEFOLD_FL` is set only where a driver sets
  it (ext4, f2fs), so `Sensitive` is the correct answer elsewhere.
- **tmpfs is admitted** — volatile UUID, volatile everything.
- **NFS and most FUSE are refused by accident, with a bad sentence.** They have
  `export_operations` but no `fileattr_get`, so `vfs_fileattr_get` returns
  `-ENOIOCTLCMD` (`fs/ioctl.c:526-534`, `:705-713` v6.9) and the first refusal
  becomes `Unsupported(PathEquivalence, "query ext4 directory flags")` — a
  capability with **no remedy** (`capability.rs:98-103`), rendered by
  `entry.rs:414` as *"checked merge artifact catalog is unsupported: query ext4
  directory flags"*. That is a **worse** sentence than today's and it answers Q3
  by accident.

**Therefore §6 Q3 and Q4 are BLOCKING preconditions of (b)**, and (b) ships a
**locality/volatility capability** in front of the catalog — the
`RuntimeAdvisoryLock` class already minted at `capability.rs:91`, with its own
remedy sentence — expressed as *property attempts*, never as a magic-number
allow- or deny-list (§5 item 4). The properties it asserts, each probed by
attempt on the retained root:

1. **Reliable advisory locking** — the property `OperationModel.md:118-121`
   already names as the reason network mounts are unsupported for concurrent
   mutation. This is the honest ground for refusing NFS/SMB.
2. **Non-volatility across reboot** — probed by asking whether the substrate's
   own recorded evidence is expected to survive; on tmpfs the answer is no.
   **Note the vacuity argument**, which the operator may prefer (Q4): on a
   volatile filesystem the *artifacts die with the catalog*, so a durability
   claim about them is vacuously safe, and admitting tmpfs may be correct.
3. **A per-directory lookup mode** — `parent_mode` becomes attempt-based with a
   stated default (`Sensitive` where `FS_IOC_GETFLAGS` is unavailable), **or**
   `PathEquivalence` gains a remedy. Either way the NFS/FUSE sentence stops being
   an accident. *(Code P2-2 condition.)*

Also in scope of the same step, because they become wrong the moment non-ext4 is
admitted: the ext4-specific error strings at `linux.rs:59` ("query ext4
directory flags"), `:161` ("query ext4 external filesystem UUID"), `:167` ("ext4
returned an absent or malformed external UUID"), `:199` ("ext4 returned an
unsupported persistent handle").

#### 3.2.5 The guarantee ledger for (b) and (c)

*(fold: State [P2-6] — the State axis's §1 table, adopted verbatim in substance.)*

| # | Guarantee today (v0.13.0, ext4) | Mechanism today | Under (b) | Under (c) |
|---|---|---|---|---|
| G1 | Exact classification after a **crash** (same boot) | handle+UUID hashed into goal/anchor/authority names (`residue.rs:183-206`, `anchor.rs:316`, `authority.rs:196-202`) | **KEPT** — the handle is unchanged; the UUID contributed only to the hash | KEPT at T3; KEPT at T2 in-session; **WEAKENED at T1** to content equality with a typed refusal on unattended recovery |
| G2 | Exact classification after **clean exit and later resume across reboot/remount** | as G1; the external UUID makes it remount-stable (C10) | **KEPT** on ext4/xfs/f2fs/btrfs; **UNVERIFIED on zfs**; on tmpfs **VACUOUS** (artifacts die with the boot) — and (b) admits tmpfs, §3.2.4 | T3 KEPT; T2 out-of-session **WEAKENED** to `IdentityUnprovable`; T1 leaves as G1 |
| G3 | **Within-run interference guard** (a substituted equal-content object is caught, C6/C7) | invocation `dev`+`ino` (`linux.rs:130-133`) + the retained handle | **KEPT** (invocation half untouched) | KEPT at T3/T2 (`session` is unconditional); **LOST at T1 — therefore refuse**, not "degrade" |
| G4 | The catalog is **bound to one filesystem instance** | `DurableCatalogTargetDigestV1` folds the encoded identity incl. UUID for root/repo/common/parent (`digests.rs:108-135`) + `retained_parent_identity` equality | **WEAKENED**, precisely: handle+nonce bind; the volume fact gates **re-binding only** (§3.2.3). Ordinary copies still caught by the handle; the same-handle cross-volume image is the stated residual | as (b), plus tier + evidence in the record |
| G5 | **Rename-domain proof** (`residue.rs:89-101`) | `statx(STATX_MNT_ID)` (VFS-generic) | **KEPT** — (a0) removes only the gratuitous gate in front of it | **KEPT** — needs no tier |
| G6 | The abort's **path-scoped capability freedom** (amendment §6; `capability.rs:53-65`) — the re-verifying doors take the legacy probe, **STRICTLY WEAKER** than the catalog's | two probes (`identity.rs:312-367` vs `linux.rs:122-134`) | **KEPT in extension**; the **"strictly weaker" clause becomes FALSE BY OBSOLESCENCE** once the regimes coincide — dated brackets required (§3.2.7) | **WEAKENED at T2 post-reboot**: the re-verifying doors reach `IdentityUnprovable` and refuse (§4.7) |
| G7 | Every downgrade **recorded in the evidence** (§5 item 3) | n/a (one tier) | provided by `VolumeFactV1` (§3.2.3) — round 1 had no wire home for it | provided by the derived tier + the records (§4.2) |

#### 3.2.6 The census and the fault rows

*(fold: State [P2-5] second clause.)* Round 1 asserted the 165-key fault census
(`interface_tests/fault_expected_keys.rs:174`) is untouched by (b). That holds
**only if** the migration is built without fault rows — and §3.2.2 argues it must
have them (crash-between-publish-and-retire is a real window). **So (b) is
expected to move the census**, and the Track-W freeze (§3.2.7) covers both the
wire and the census in one dual rather than two.

#### 3.2.7 Moves, and the records to amend

*(fold: State [P2-5] first clause — round 1 named the wrong frozen clause.)*

**The frozen clause is `GwzM5-8R2DInterfaceFreeze.md` §6 "Wire and records rule
— zero new durable records" (`:1711-1722`):** *"Any discovered need for a new or
changed durable record, slot, purpose, or phase stops the step for a Track-W
freeze plus dual review before any implementation."* Inherited by
`GwzM5-8R2DSettledTuple.md` §3 (census `:114`) and §4.5 (`:286`).
**`GwzM5-8I2CompatibilityContract.md` is NOT the home** — its "protocol
allocation" is the merge-operation **envelope** registry (`:15-21` naming it,
`:23-35` the `gwz.merge-operation/v0..v4` table), and its `:7-13` is the R4b
amendment note, not a mechanism. Round 1's cite of that contract is withdrawn.

| What moves | Cite |
|---|---|
| Protocol allocation (new identity kind, profile variant, nonce field, `VolumeFactV1`) | `protocol/checked_artifact.taut.py:16-19`, `:34-37`, `:56-63`, `:103-115` → regen `protocol/generated.rs`, corpus, `protocol/d0_roundtrip_check.py`, `protocol/check_log_additive.py:17` `PRE_LOG_WIRE_SHA256` |
| Boundary pins | `scripts/checks/check_checked_artifact_boundaries.py:63` (`capability.rs` flat), `:71` (`entry.rs` flat), `:263` (`catalog_lease.rs` tree), `:265` (`pre_catalog.rs` tree), `:266` (`catalog.rs` tree) |
| Fault census, if the migration takes fault rows | `interface_tests/fault_expected_keys.rs:174` |
| Remedy string + its pin | `capability.rs:66-71`; `interface_tests/contracts.rs:150-165` |
| **The abort clause's dated brackets** *(fold: State [P2-1], G6)* | `capability.rs:53-65` — specifically `:59-62` ("Those doors take the LEGACY probe … which admits btrfs/xfs/zfs where the catalog's `require_ext4` refuses, so the string's 'ext4 only' is the CATALOG's admission list, not the abort's"), and `GwzM5-8R2E-CapabilityFreeAmendment.md` §6 `:397-403` |
| Docs | `docs/OperationModel.md:139-166`; `platform.rs:184`; `GwzReleaseNotes` for the next version (admission set, one-way migration, the G4 residual) |
| R0-L release gate | `scripts/linux_identity_probe/provider.py:28-40`, `:154-155`, `:240` + `run_probe.py`, `test_probe_contract.py` — **release-lane sign-off**, §6 Q8 |
| Capability-free list — **NOT amended** | `GwzM5-8R2E-SemanticsAmendment-DRAFT.md:909-917`; its `:917` reading bracket gains one dated sentence (the catalog's Linux admission now equals the legacy probe's) |
| Record-root amendment — **no text change** | `GwzM5-8R2E-RecordRootAmendment.md` §4 `:202-217`; each implementing charter carries the lease-reachability enumeration |
| Ownership-token boundary — **stands, not re-litigated** | `bootstrap/managed/provider.rs:51-62` (§3.2.3) |

#### 3.2.8 Sizing

| Component | LOC |
|---|---|
| Identity variant + codec | ~60 |
| taut regen + corpus + D0 + wire pin | ~100 |
| `linux.rs` rework + error-string de-ext4-ing | ~40 |
| Profile plumbing (`capability.rs`, `target.rs:443-461`, `digests.rs:299-305`, codec) | ~60 |
| Instance-id + `VolumeFactV1` record fields | ~80 |
| Locality/volatility capability + remedy + attempt-based `parent_mode` | ~150 |
| **Dual-tuple migration + fixtures + fault rows** | ~250 |
| R0-L probe contract rewrite | ~150 |
| Pins, docs, release notes | ~50 |
| Tests | ~200 |
| **Total** | **~950-1100, three steps** |

UNVERIFIED (no spike).

### 3.3 (c) — The full graded-evidence design. **~2200-2800 LOC over 6 phases. The destination.**

Detailed in §4.

### 3.4 (d) — `--allow-weak-identity`. **~250 LOC. DECLINED.**

A global process mode answering a per-object question; either it re-derives (c)'s
recording machinery or it degrades without a recordable class, which §5 forbids.
Declined by the lane owner and by both review axes.

### 3.5 Ranking summary

| Option | LOC | Admits on Linux | Answers the ask? | Tier | Risk |
|---|---|---|---|---|---|
| (a0) delete gratuitous gates + 3 doc fixes | ~30 | unchanged | no (hygiene) | no | none |
| (a) attempt-based UUID | ~350 | + xfs, f2fs; **not btrfs**; **+ tmpfs on every kernel ≥ 6.9** | **no** | moves the R0-L release gate | volatility regression |
| **(b) instance-id + legacy parity + locality capability** | **~950-1100** | **+ btrfs, xfs, zfs, f2fs (+ tmpfs unless Q4 refuses it); NFS/FUSE refused by the locality capability, with a remedy** | **yes** | Track-W freeze + dual | the dual-tuple migration; blocked on Q3/Q4 |
| (d) override flag | ~250 | any, on the user's word | technically | no | a bypass with no per-object evidence |
| (c) graded evidence | ~2200-2800 | everything, graded | yes, permanently | Track-W freeze + dual | the invisibility class (§2.2); over-promising T1 |

---

## 4. The graded-evidence design (option (c))

### 4.1 The identity type

```
IdentityEvidenceV1 {
  persistent: Option<PersistentHandleFact>,  // T3: handle_type + bytes (Linux) /
                                             //     ATTR_CMN_OBJPERMANENTID (macOS) /
                                             //     FileId128 (Windows)
  volume:     VolumeFactV1,                  // Uuid | FsId | Absent — corroboration
  session:    SessionFact,                   // T2: dev + inode + ctime + size — ALWAYS
  content:    Option<ContentFact>,           // T1: size + digest — LEAVES ONLY
  class:      ObjectClass,                   // Directory | Leaf
}
```

Four rules the tree forces:

1. **`session` is unconditional.** `dev`+`ino` are already collected on every
   platform (`platform/linux.rs:130-133`, `macos.rs:78-81`, `identity.rs:269-277`)
   and exist on FAT32 and NFS. They are the floor, which is what makes "the probe
   never refuses" true rather than aspirational.
2. **`class == Directory` ⇒ `content` is `None` and the floor is T2.**
3. **`volume` never gates admission**; it gates re-binding only (§3.2.3).
4. *(fold: State [P3-1])* **The tier is DERIVED, never stored.**
   `tier() = if persistent.is_some() { 3 } else if content.is_some() { 1‑capable }
   else { 2 }` — computed from the facts present. Where a summary must appear on
   the wire for a reader's benefit, the decoder **rejects any record whose
   summary contradicts its facts**, the I8 pattern (`durable_identity.rs:34-85`).
   This is the same rule §3.2.3 establishes for `VolumeFactV1`.

T3 and T1 are **not exclusive** — recording both is strictly better and costs one
digest.

### 4.2 Where the evidence is recorded — and the complete naming rebase

*(fold: Code P2-3, State [P2-3] — round 1's naming list was incomplete, and the
omission was the structural hole in (c).)*

| Record / name | Change | Cite |
|---|---|---|
| **`family_key`** | **minted from the artifact nonce, not `SHA256(root_identity.durable.encode() ‖ path)`** | `authority.rs:196-202` |
| **`family_prefix` / `authority_name`** | derived from the nonce-based family key; the residue prefix filter therefore survives any tier change | `authority.rs:226-232`; `residue.rs:116`, **`:127`** |
| **`matches_request`** | defined over the **nonce**, not the encoded identity, so `artifact_root_identity` stops being a comparison key | `authority.rs:101-119`, esp. `:110` |
| `goal_name` / `source_name` | minted from the nonce + action, not `name_digest` | `authority.rs:234-240`; `residue.rs:390-394`; `transition.rs:280-284` |
| Anchor name | nonce-based; evidence recorded inside | `platform/anchor.rs:316`, `:490-506` |
| Authority record body | `retained_parent_identity` / `retained_source` become `IdentityEvidenceV1` | `authority.rs:32`, `:136-142` |
| Catalog bootstrap record | instance id (from (b)) + evidence + `VolumeFactV1` | `protocol/catalog_bootstrap_record.rs:172-185` |
| Protocol | new message + enum; old kinds retained for decode | `protocol/checked_artifact.taut.py:16-19`, `:56-63` |

**Why the naming rebase is the heart of it.** C1, C2, C9 and C10 need
reboot-durable identity *only because the identity is the name*. Once names come
from a gwz-minted nonce recorded in the record, the reboot requirement survives
only for the *verification* purpose — where a tier can degrade the conclusion
instead of destroying the name. **And critically it closes the invisibility class
of §2.2:** a family whose prefix is nonce-derived is still enumerated by
`residue.rs:127` after any tier change, so the `foreign`/degrade logic actually
runs.

**Migration of the family prefix.** Existing private areas carry
identity-derived `ca1-{family}-` names. The (c) P2 step must either enumerate
both prefixes during a migration window, or migrate the family prefix under the
same admitted-action discipline as §3.2.2. This is a named sub-step, not a
detail.

### 4.3 The classifier's tier-aware rules

- **T3, both sides:** unchanged; every existing arm keeps its meaning.
- **T2, both sides, same mount session** (recorded `dev` and live `stx_mnt_id`
  agree): exact, automatic. Outside the session: a **new typed outcome**,
  `IdentityUnprovable { recorded_tier, live_tier, what_would_prove_it }` — an
  actionable sentence, not `"evidence is ambiguous"`.
- **T1 (leaves):** content decides `Baseline`/`Candidate`/`Other` — already what
  `abort/evidence.rs:343-361` and `publication/live.rs:166-187` do, so C12/C13
  need no change. `RecoverableDuplicate{Source,Goal}` (`classification.rs:263-270`)
  must be **unreachable at T1** (C4), and a T1 detach must not retire an object
  (C3). G3 is **LOST at T1 and the operation therefore refuses** — it is not
  preserved by a weaker check.
- **Directories:** T2 floor; C5 and C11 run on the session fact.
- **C8 needs no tier at all** *(fold: Code P3-4, now consistent with §2's C8
  row)*: `statx(MNT_ID)` is VFS-generic (`linux.rs:71-89`), and the rename-domain
  gate compares **mount ids, not identities**. It stays a refusal because it is a
  correctness precondition for atomic publication.
- **The two hard-refusal sites** (§2.2) join the typed vocabulary: the anchor
  (`anchor.rs:239-246`) becomes tier-aware; the rename-domain gate does not
  change.
- **Locality/lock leaves identity entirely** — as (b) already ships it (§3.2.4).
  macOS's `MNT_LOCAL` (`macos.rs:166-171`) and Windows' NTFS name test
  (`windows.rs:130-135`) re-home to the same capability.

### 4.4 Interaction with the two open observer classes — they do not change

**R2, the detach/observation-dead window** (`GwzM5-8R2E-E4-Close.md:43`;
`workspace_ops/merge/v1_lifecycle/finalization/execute.rs:48`, `:51` — cited via
E4-Close, not read here). A window in time, not an identity strength. **Tiers do
not change it.**

**R3, the directional residue** (`GwzM5-8R2E-E4-Close.md:44`;
`GwzM5-8R2E-E45B-Report.md:35`). `classify_remove` returns `Ambiguous` because
`classification.rs:175-177` sees an authority record whose `matches_request` was
minted for the FORWARD pair while the abort asks the REVERSE pair
(`authority.rs:101-119` pairs `family_key`, `action_key`,
`canonical_path_identity`, `artifact_root_identity`, `expected`, `goal`,
`operation`; no tier participates). **Tiers do not change it.** Its cure remains
the 450-600 LOC abort-side observer reconciliation on the capability-free plain
lease (`E45B-Report.md:39`).

Two second-order interactions, both real:

1. **Population.** Tiering admits more substrates, so more users reach R2 and R3.
   The cures become more urgent, not different (§7).
2. **Threading — restated against the right site** *(fold: Code P2-3, Code Q7,
   State [P2-3])*. Round 1 pointed this rule at `authority.rs:101-119`. **That is
   too late in the path.** Because `artifact_root_identity` both feeds
   `family_key` and is compared at `:110`, a tier change bites at the **prefix
   filter, `residue.rs:127`**, before `matches_request` is ever reached: the
   family is not enumerated, the record is not `foreign`, it is **invisible**, and
   the `authority == false` arms (`classification.rs:247-253`) silently orphan a
   detached source or staged goal. **Design rule, corrected:**

   > A tier change between minting and reading must leave the family
   > **enumerable** — which §4.2's nonce-derived `family_prefix` guarantees — and
   > must then **degrade the classification**, never make the record foreign and
   > never make it invisible.

### 4.5 Catalog re-binding and migration

As (b) §3.2.2, plus tier and evidence in the record. **The second migration is
real and is (c)'s cost if (b) ships first** — see §6 Q2 and §7. The scratch name
is unchanged except through the digests (§3.2.3).

### 4.6 Fault-injection coverage — ext4 alone drives every tier

No FAT32 lab (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:19`). Inject "tier
unavailable" at the **primitive seam** — the four `DurableIdentityProvider`
methods (`capability.rs:169-189`) and the legacy `identity::platform` module — so
that on ext4 the suite can force `persistent = None` (→ T2), `persistent = None,
class = Leaf` (→ T1), and a **tier transition between two observations**.

**Seam: a test-only provider substitution at `HostPlatform`
(`capability/pre_catalog/provider.rs:156`, re-exported at
`capability/pre_catalog.rs:25`), not new fault keys** — the (c) census stays at
165 where (b)'s migration has not already moved it (§3.2.6). *(fold: State
[P3-2])* **It carries the house's absence pin**: the substitution seam is
`cfg(test)`-gated and pinned absent from production builds in the
`interface_tests/catalog_activation_pin.rs` idiom (`:81-84`, which already pins
`#[cfg(test)] mod` shapes and rejects off-convention test files).

**Coverage matrix.** Every §2 consumer × {T3, T2 in-session, T2 out-of-session,
T1 leaf}; `class == Directory` × T1 must be **statically unreachable** (a type
error, not a test). Plus, added per State [P3-2], **rows for the two
hard-refusal sites of §2.2**: the anchor at each tier
(`anchor.rs:239-246`), and the rename-domain gate proving it is tier-independent
(`residue.rs:93-101`). Plus a row for the §4.4 rule-2 case: a family remains
enumerable at `residue.rs:127` across a forced T3→T2 transition.

### 4.7 The abort sentence, rewritten — TIERED, not collapsed

*(fold: State [P2-1] — round 1's "the straight line never refuses at all" and
"the abort's scoped clause collapses into the general rule" are both withdrawn.)*

**Stated precisely: "never refuses" is true of the PROBE and false of the
OPERATION.** The probe always returns evidence (§4.1 rule 1). The operation still
refuses where the claim it needs exceeds the evidence: a `--resume` at T2 after a
reboot refuses; the re-verifying abort doors at T2 after a reboot refuse; a T1
leaf recovery refuses by design (§5 item 6).

So the abort clause does not collapse — **it becomes tiered.** The sentence for
`capability.rs:53-65`, `docs/OperationModel.md:150-161` and the release notes:

> An abort that touches no checked artifact needs no filesystem capability at
> all. Aborts that must re-verify checked artifacts — a preservation bundle, a
> selected root's manifest and lock, or the merge's published evidence — recover
> **exactly at tier 3, and at tier 2 within the session that minted the
> evidence**. Outside that, they refuse with `IdentityUnprovable`, naming what
> would prove it; the merge is then cleared by completing it, or by re-running
> the abort in the minting session.

The remedy string (`capability.rs:66-71`) stops naming filesystems and names
**claims**:

> this operation needs *&lt;claim&gt;* — &lt;e.g. "unattended recovery of an interrupted
> merge across a reboot"&gt; — which needs identity this filesystem cannot prove
> (recorded evidence: &lt;what is available&gt;). &lt;What the user can do.&gt;

`docs/OperationModel.md:139-166` loses "**On Linux the admitted filesystem is
`ext4` and nothing else**" (`:140-141`) and the self-contradicting sentence at
`:142-143`, and gains a claims-and-tiers table.

### 4.8 Sizing by phase

| Phase | Content | LOC |
|---|---|---|
| P0 | (a0): the two gratuitous gates; the three doc/comment defects | ~30 |
| P1 | `IdentityEvidenceV1` + protocol allocation + attempt-based providers, all three OSes; **no consumer change** (T3 only, behaviour identical) | ~450 |
| P2a | Nonce-based `family_key`/`family_prefix`/`authority_name`/`matches_request` (C1, C2) | ~350 |
| P2b | Nonce-based goal/source/anchor names + **the family-prefix migration** (C9) | ~300 |
| P2c | Catalog instance id, evidence, tier + the catalog migration (C10) | ~350 |
| P3 | Tier-aware classifier: `IdentityUnprovable`, T2 in-session, T1 leaf rules, the anchor conversion | ~450 |
| P4 | Locality/lock split (if not already shipped with (b)); macOS/Windows re-homing | ~250 |
| P5 | Substitution seam + absence pin + coverage matrix; R0-L rewrite; docs, remedy strings, release notes | ~500 |

Total ~2680 LOC. P2 is three steps, not one — round 1 under-counted it by
omitting the naming rebase's true surface.

---

## 5. What NOT to do

1. **Do not put the identity probe on any capability-free operation.** Ordinary
   merge, `commit`, `repo create`, `init-from-sources`, abort and GC stay off the
   probe — `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:10` and
   `GwzM5-8R2E-SemanticsAmendment-DRAFT.md:909-917`. Each implementing charter
   carries the lease-reachability enumeration required by
   `GwzM5-8R2E-RecordRootAmendment.md` §4 `:202-217` **before the build**. Watch
   item: §3.2.1 item 2 converges `identity.rs` onto the new provider, and that is
   a path the abort's checked doors ride on the plain lease.
2. **Do not weaken the ordinary path.** The
   `CAPABILITY_FREE_RAW_WRITER_INVENTORY` fires on **shrinkage** as well as growth
   (`GwzM5-8R2E-CapabilityFreeAmendment.md:207-210`).
3. **Do not degrade silently.** Every class below the best is recorded *in the
   evidence* on the wire (§3.2.3), and every decision a lower class changed is
   reported. **The invisibility class of §2.2 is the concrete instance of this
   rule being violated by accident** — it is why §4.2's naming rebase is
   mandatory rather than cosmetic.
4. **No per-filesystem branches — in either direction.** Not a magic-number
   allowlist (today's `require_ext4`), and **not a magic-number denylist** of
   tmpfs/overlay/NFS to replace it. Refuse for the property that is missing,
   through the capability that names it (§3.2.4).
5. **Do not promise T1 for directories.** Make it a type error.
6. **Do not let T1 drive unattended recovery.** G3 is lost at T1; the operation
   refuses.
7. **Do not adopt `f_fsid` as the durable volume fact.** It is the kernel's XOR
   fold of the UUID's two halves (`include/linux/statfs.h:55-59` v6.9) — stable
   on ext4 (`fs/ext4/super.c:6829`) but a lossy 8-byte reduction, and volatile on
   tmpfs. Acceptable as `VolumeFactV1::FsId` corroboration; never an identity.
8. **Do not build a FAT32 lab.** Coverage comes from tier injection on ext4 (§4.6).
9. **Do not make the tier a global process mode** — (d)'s shape.
10. **Do not store a tier or class as a free label.** Derive it; reject
    contradictory pairs at decode (§4.1 rule 4).
11. **Do not re-open the ownership token's boundary.** `bootstrap/managed/provider.rs:51-62`
    stands; the instance id is a distinct nonce (§3.2.3).

---

## 6. Open questions for the operator — one line each

1. **Home:** is DR-1 R2-E phase E8 (the lane owner's default,
   `GwzM5-8DR1-Charter.md:6-10`) or a new lane?
2. **Sequencing — now a three-way choice** *(fold: State [P2-4])*: **(i)**
   (a0)+(b) now, then (c) — Fedora soonest, at the cost of a **second** catalog
   migration and a **second** Track-W dual when (c) P1 lands; **(ii)** (c) P1's
   protocol shape (`IdentityEvidenceV1`) first as (b)'s vehicle — one allocation,
   one migration, one dual, Fedora one step later; **(iii)** hold everything for
   (c).
3. **BLOCKING ON (b) — network mounts:** are NFS/SMB/FUSE to be refused by
   policy through the locality/lock capability with its own remedy, rather than
   by today's accident of a remedy-less `PathEquivalence` failure?
4. **BLOCKING ON (b) — tmpfs:** admit it (artifacts die with the catalog, so the
   durability claim is vacuous) or keep the R0-L negative row
   (`scripts/linux_identity_probe/provider.py:40`) and refuse it through the
   volatility property?
5. **Patch or minor:** the lane reads the freeze §6 (`:1711-1722`) and the
   one-way migration as answering this — **minor**. Confirm?
6. **Tier-injection seam:** test-only provider substitution with the
   `catalog_activation_pin.rs` absence pin (no (c)-era census move), confirmed?
7. **`--allow-weak-identity`:** wanted at all, given (c) makes it unnecessary and
   §5 item 9 argues against its shape?
8. **R0-L release gate:** its ext4-only evidence contract is release-blocking —
   does the release lane own the rewrite, or this lane?
9. **The instance id** *(fold: State [P1-2], [P3-3])*: confirm it is a **second**
   nonce beside `bootstrap_ownership_token`, leaving that token's declared
   boundary (`bootstrap/managed/provider.rs:51-62`) untouched — rather than
   re-using the token and re-litigating its boundary now?

---

## 7. Recommended sequencing

**Ship first:**

1. **(a0)** — the two gratuitous `require_ext4` calls, plus the three defects of
   §1.5. ~30 LOC, no dual beyond the ordinary landing gate. Both axes confirm it
   behaviour-preserving.

**Then, on the answers to Q2/Q3/Q4:**

2. **(b)**, in three steps — (i) protocol allocation + provider rework +
   locality/volatility capability; (ii) the dual-tuple migration with its
   fixtures and fault rows; (iii) the R0-L probe contract and the docs. ~950-1100
   LOC, Track-W freeze plus dual. **Not charterable until Q3 and Q4 are
   answered.**

**Lane owner's recommendation on the Q2 trade.** Option (ii) — landing (c) P1's
`IdentityEvidenceV1` first as (b)'s vehicle — is the better engineering answer:
it pays one protocol allocation, one catalog migration and one Track-W dual
instead of two of each, and the migration in §3.2.2 is the single highest-risk
piece of work in the whole program, so doing it **once** is worth more than doing
it soon. Option (i) is the better *product* answer only if Fedora/RHEL users are
being turned away now and one step of delay is unacceptable. **The lane
recommends (ii) unless the operator judges the Fedora block urgent**; the cost of
(i) — a second migration over the same records, and a second dual — must be
written into the charter if (i) is taken.

**Then, as the program:**

3. **(c) P1-P2** — the evidence type and the **complete** naming rebase
   (`family_key` included). P2 is where the invisibility class of §2.2 is closed;
   it is three steps.
4. **The R3 cure (directional residue) before, or with, (c) P3.** Tiering widens
   the population that reaches R3 (§4.4 note 1); shipping tier-aware
   classification while `--abort` can still strand would make the new substrates'
   first bad day worse than ext4's.
5. **(c) P3-P5** — the tier-aware classifier, the locality split (where not
   already shipped with (b)), the coverage matrix and the doc/remedy rewrite.

**Not recommended at any point:** (a) alone and (d).

---

## Verification notes

- Kernel claims cite the **tag read** per line, at
  `raw.githubusercontent.com/torvalds/linux` (v6.8, v6.9, v6.12, master), fetched
  2026-09-03. Round 1's ext4/f2fs/xfs line numbers were from a later tag; §3.1
  now states v6.9 numbers (`:5344`, `:4433`, `:65`) per Code P3-1.
- Tree claims re-verified for this revision at `ffd4f95`:
  `catalog_bootstrap_record.rs:19-38`/`:41-54`/`:172-185`; `digests.rs:108-135`;
  `scratch.rs:19-46`; `classifier.rs:301-319`; `residue.rs:106-130`;
  `authority.rs:194-240`; `classification.rs:245-278`;
  `bootstrap/managed/provider.rs:51-62`; `platform.rs:178-190`;
  `capability.rs:53-71`; `identity.rs:551-580`;
  `GwzM5-8R2DInterfaceFreeze.md:1711-1725`; `GwzM5-8I2CompatibilityContract.md:5-35`;
  `interface_tests/catalog_activation_pin.rs:81-84`.
- **UNVERIFIED**, carried from round 1 and the dual: `FS_IOC_GETFSUUID` and
  `name_to_handle_at` on zfs, overlayfs, NFS and FUSE; whether any distro kernel
  backports `super_set_uuid` into btrfs; btrfs/xfs `f_fsid` derivation; that a
  v0.13.0 decoder fails closed on an unknown identity kind (§3.2.2); that every
  caller of `interior.rs:72`/`alias.rs:21` is retained-first; whether the R0-L
  `mode_query_failure` row depends on the ext4 pre-test; all LOC figures (no
  spike); `finalization/execute.rs:48`/`:51` (cited via E4-Close, not read).
- Not run: any test suite, `cargo check`, or `cargo doc`.
