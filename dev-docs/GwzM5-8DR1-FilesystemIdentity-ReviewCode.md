# GwzM5-8 DR-1 — filesystem identity design: peer-blind review, CODE/MECHANISM axis

Date: 2026-09-03. Reviewer: code/mechanism axis (peer-blind; the design/state
axis is a separate record). Subject: `dev-docs/GwzM5-8DR1-FilesystemIdentity-Design.md`
(DRAFT, 2026-09-03). Object: gwz-core `ffd4f95` (v0.13.0). Box: 30 minutes
wall clock, opened 00:40 AEST; filed 00:52. Method: reading the tree and the
public kernel source at the tags named (`raw.githubusercontent.com/torvalds/linux`
at v6.8, v6.9, v6.12, master, fetched 2026-09-03). No builds, no suites, no
probes — every item below settled by reading, so the two-probe allowance was
not spent. No code changes.

**Verdict: GO-WITH-CONDITIONS** — adopt the design as DR-1's answer and its
ranking ((a0)+(b) ship, (c) program, (a) and (d) not), on four conditions
(P1-1, P2-1, P2-2, P2-3 below). The inventory, the kernel mechanism for
btrfs, the leaf-only-tier claim, the gratuitous-gate claim and the observer
non-interaction are all confirmed by the tree. What is wrong is concentrated
in two places: the (b) migration mechanism as written would regress every
existing v0.13.0 ext4 catalog, and the tmpfs kernel fact is off by three
releases in the direction that makes (b) admit tmpfs today.

Tally: **37 claims verified, 6 refuted, 6 unverified** (§4).

---

## 1. Findings

### P1

**P1-1 — (b)'s v0.13.0 catalog migration mechanism is incomplete as written;
executed as described it makes every existing ext4 catalog `Ambiguous`.**
Design §3.2 "Migration": *"`matches_attempt` must accept a record whose
identity is the old `LinuxExt4` kind when the handle type and handle bytes
agree, ignoring the UUID field."* That relaxes one conjunct of five.
`CatalogBootstrapRecordV1::matches_attempt`
(`src/checked_artifact/protocol/catalog_bootstrap_record.rs:172-185`) is

```
self.root_kind == root_kind
    && self.support_profile == support_profile
    && self.durable_target_digest == durable_target_digest
    && &self.retained_parent_identity == retained_parent_identity
    && &self.retained_parent_path == retained_parent_path
```

Under (b) three of these change, not one: (i) `support_profile` — (b) item 4
adds a new `SupportedFilesystemProfile` variant, so old records carry
`LinuxExt4FsIocGetFsUuidV1` and the attempt carries the new one; (ii)
`durable_target_digest` — `target_digest`
(`capability/pre_catalog/provider/digests.rs:108-135`) is SHA-256 over
`profile_code(profile)` (`:120`, `:299-305`) and the `encode_canonical()` of
four directories (`:121-131`), each of which contains the UUID; ignoring a
field of `retained_parent_identity` does nothing to a hash; (iii)
`historical_collision_digest` takes the target digest as input
(`digests.rs:137-139`) and the scratch filename embeds both digests
(`catalog/scratch.rs:19-46`), so an interrupted v0.13.0 bootstrap's scratch
entry parses as `Other` under the new binding. `decode_canonical`'s
self-consistency cross-check (`:235-236`, identity kind vs profile) is fine —
an old record still decodes. The mechanism that works is a *dual-tuple*
acceptance: on ext4 the ioctl still answers, so the activation door can compute
the legacy digest alongside the new one and accept either, then re-issue. That
is feasible but it is not "cheap", it keeps the legacy digest computation
alive as a migration probe, and it needs its own corpus fixture for the record
*and* for the scratch name. Condition: rewrite §3.2 "Migration" to name all
three conjuncts, the collision digest and the scratch namespace, and size it.

### P2

**P2-1 — REFUTED: tmpfs answers `FS_IOC_GETFSUUID` from the ioctl's first
kernel, not "on master".** Design §3.1 table: *"tmpfs — no at v6.12; YES on
master (`mm/shmem.c:5049`)"*. `shmem_fill_super` calls `super_set_uuid` at
**v6.9 `mm/shmem.c:4405`** and **v6.12 `mm/shmem.c:4696`** (`uuid_gen(&uuid);
super_set_uuid(sb, uuid.b, sizeof(uuid));`), as well as master `:5049`. So on
every kernel that has the ioctl at all, tmpfs returns a 16-byte random per-mount
UUID. Consequences the design must absorb: today the *only* thing refusing
tmpfs in the catalog is `require_ext4` (I1) — the UUID gate (I2) admits it;
(a) admits tmpfs on every kernel >= 6.9, not "new-enough" kernels; and (b)
admits tmpfs unconditionally (tmpfs has `shmem_export_ops`, so
`name_to_handle_at` answers), with a corroboration UUID that is volatile by
construction. The R0-L negative row `tmpfs: UnsupportedOperation`
(`scripts/linux_identity_probe/provider.py:40`) is therefore contradicted by
(b), not only by (a). Operator question Q4 is a precondition of (b), not a
side question. Condition: correct §3.1 and move Q4 ahead of (b).

**P2-2 — (b)'s parity claim is inexact: the catalog runs a gate the legacy
probe does not, and that gate has no remedy sentence.** Design §3.2: *"admits
exactly what the legacy probe admits ... everything with `export_operations`"*.
The catalog lease also calls `parent_mode` and `rename_domain` on every
retained directory (`catalog_lease/target.rs:153-156`, `provider/retained.rs:338-339`,
`managed_mutation.rs:171,191,194,291`, `association.rs:150,167`, `interior.rs:72`,
`alias.rs:21`); the legacy family's `case_sensitive` is `Ok(true)`
unconditionally (`identity.rs:369-370`) and it has no mode probe. Linux
`parent_mode` is `FS_IOC_GETFLAGS` (`linux.rs:56-61`), which the kernel
answers only where `i_op->fileattr_get` exists — `vfs_fileattr_get` returns
`-ENOIOCTLCMD` otherwise (`fs/ioctl.c:526-534` v6.9, via `ioctl_getflags`
`:705-713`). btrfs, xfs, f2fs, tmpfs and zfs have it (`btrfs_fileattr_get`
master `fs/btrfs/ioctl.c:273`; `xfs_fileattr_get` v6.9 `fs/xfs/xfs_iops.c:1121`;
`shmem_fileattr_get` v6.9 `mm/shmem.c:3595`), so the named targets pass. NFS
and most FUSE filesystems have `export_operations` but no `fileattr_get`, so
under (b) their first refusal becomes `Unsupported(PathEquivalence, "query ext4
directory flags")` — a capability with **no remedy** (`capability.rs:98-103`
maps only `PersistentFilesystemIdentity`), rendered by `entry.rs:414` as
`checked merge artifact catalog is unsupported: query ext4 directory flags`.
That is a worse sentence than today's, and it answers Q3 (network mounts) by
accident. Casefold semantics are not the problem — `FS_CASEFOLD_FL` is never
set on btrfs/xfs/zfs, so `Sensitive` is the correct answer there. Condition:
(b) must state `parent_mode`'s admission explicitly and either give
`PathEquivalence` a remedy or make the mode probe attempt-based with a stated
default; and the "ext4" in the error text at `linux.rs:59`, `:161`, `:167`,
`:199` becomes wrong the moment non-ext4 is admitted.

**P2-3 — (c)'s naming rebase omits `family_key`; a root-identity change makes
prior residue *invisible*, not `foreign`, so the C1/C2 "at T2" cells name the
wrong mechanism.** `inspect_family` (`residue.rs:106-129`) enumerates the
private directory and **skips** every entry not starting with
`family_prefix(family)` = `ca1-{family}-` (`:116`, `:127`), where
`family_key = SHA256(root_identity.durable.encode() ‖ canonical_path)`
(`authority.rs:196-202`); the authority filename (`:226-228`), goal and source
names (`:234-240`) all carry that prefix. Any change to the *root's* durable
encoding — a tier drop where T2's `dev` is renumbered across reboot, or a new
identity kind under (c) P1 — filters the whole family out at `:127` before the
`foreign` logic at `:160-207` ever runs. The classification then takes the
`authority == false` arms (`classification.rs:247-253`): `Before`/`After` where
the managed leaf is intact (the detached source or staged goal is silently
orphaned in the private area), `Ambiguous` only where the managed leaf is
`Missing` (the `_ => Ambiguous` fall-through at `:275`). That is the silent
degradation §5 rule 3 forbids, and §4.4 rule 2 ("must degrade the
classification, never make the record foreign") does not cover it because
the record is neither. §4.2's table lists goal/staged-goal, anchor and the
bootstrap record but not `family_key`/`authority_name`/`family_prefix`.
(b) does not hit this — it changes the pre-catalog provider, not the legacy
`identity.rs` encoding. Condition: add `family_key` to §4.2's nonce rebase and
correct C1/C2's T2 cells.

### P3

**P3-1 — Kernel line citations drift; claims stand.** At v6.9 the
`super_set_uuid` calls are `fs/ext4/super.c:5344` (design: 5331),
`fs/xfs/xfs_mount.c:65` (design: 66), `fs/f2fs/super.c:4433` (design: 4502);
uapi `fs.h` has `struct fsuuid2` at `:75` and the `FS_IOC_GETFSUUID` define at
`:237` (design: 236-237). The design's Verification Notes say v6.8/v6.9/v6.12/master
were read; the ext4/f2fs numbers look like a later tag. State the tag per cite.

**P3-2 — "truncated `f_fsid`" mischaracterises the legacy corroborator.**
`identity.rs:279-290` copies the whole 8-byte `__kernel_fsid_t`; the 16-to-8
reduction is the kernel's XOR fold `uuid_to_fsid`
(`include/linux/statfs.h:55-59` v6.9), and on ext4 `f_fsid = uuid_to_fsid(es->s_uuid)`
(`fs/ext4/super.c:6829` v6.9) — a deterministic function of the same external
UUID, so "no kernel stability contract" is overstated for ext4 (btrfs/xfs
derivation UNVERIFIED). §5 item 7's conclusion (corroboration, not identity)
still holds; the reason given should be the fold and the tmpfs volatility.

**P3-3 — Inventory omissions.** (i) The legacy *Windows* probe
(`identity.rs:551-575`, `FileIdInfo` + `GetFinalPathNameByHandleW`, **no
`require_ntfs`**) is missing from §1.2 — the second regime differs from the
first on all three OSes, not two. (ii) `src/checked_artifact/platform.rs:184`
doc comment names `require_ext4` as the closed table and goes stale under (a)/(b);
add it to the doc-defect list with `linux.rs:351`. (iii) The boundary checker is
at `scripts/checks/check_checked_artifact_boundaries.py` (design cites it
without the `scripts/checks/` prefix); lines `:63`, `:263`, `:265`, `:266`
and the `entry.rs` byte pin at `:71` are confirmed.

**P3-4 — Internal inconsistency at C8.** §2 row C8 says "At T1: breaks — no
content analogue" while §4.3 says the rename-domain gate "needs no tier at all"
(true — `statx(MNT_ID)` is VFS-generic, `linux.rs:71-89`). The honest cell is
"n/a"; C8 is not an identity consumer.

**P3-5 — (b) LOC is optimistic once P1-1 is priced.** Structural count from
the sites named: identity variant + codec ~60, taut regen + corpus + D0 + wire
pin ~100, `linux.rs` rework ~40, profile plumbing (`capability.rs`,
`target.rs:443-461`, `digests.rs:299-305`, codec `path_matches_profile`) ~60,
bootstrap nonce field ~80, dual-tuple migration + fixtures ~250, R0-L probe
contract rewrite (`provider.py` is a 15-row negative table plus
`run_probe.py`/`test_probe_contract.py`) ~150, pins/docs ~50, tests ~200 —
roughly 950-1100, i.e. three steps not "2-3". UNVERIFIED (no spike), as the
design itself flags.

---

## 2. Item-by-item verification (the eight questions)

**Q1 — Inventory.** All nine I-rows and I10-I13 exist as described:
`require_ext4` at `linux.rs:136-151` with call sites exactly `:54`, `:70`,
`:126` (tree-wide grep confirms no others); `filesystem_uuid` `:153-171` with
`_IOR(0x15,0,17)` at `:15`; `persistent_handle` `:173-203`; macOS
`VOL_CAP_FMT_PERSISTENTOBJECTIDS` `macos.rs:149-152` and `MNT_LOCAL`
`:166-171`; Windows `require_ntfs` `windows.rs:100-137` (name test at
`:130-135`); `durable_identity.rs:34-85` value contract; legacy
`identity.rs:312-346` with no fs-type test and `f_fsid` at `:279-290`;
legacy `rename_domain` `:356-367`; legacy macOS with no `MNT_LOCAL`
(grep: no `MNT_` in `identity.rs`); `name_digest` `:60-66`. Catalog UUID
binding: stored in `retained_parent_identity` of the bootstrap record
(`catalog_bootstrap_record.rs:46`) and folded into `durable_target_digest`
(`digests.rs:121-131`); compared in `matches_attempt` (`:180-184`) via
`classifier.rs:96-104` and `merge_record` (`:301-319`). Missing from the
inventory: P3-3 (i)-(ii). Tree-wide grep for `statfs|fstatfs|f_type|MNT_|
GetVolumeInformation|FileIdInfo|getattrlist|f_fstypename` surfaces nothing
else in production code; `scripts/linux_identity_probe/run_probe.py:22,:88`
(R0-L) is covered by I20.

**Q2 — The two gratuitous calls.** Confirmed. After `require_ext4`,
`parent_mode` runs `FS_IOC_GETFLAGS` and refuses on its own (`:56-61`);
`rename_domain` runs `statx(MNT_ID)` and refuses on its own (`:83-88`).
`FS_IOC_GETFLAGS` exists on btrfs/xfs/f2fs/tmpfs/zfs (P2-2 cites), and
`FS_CASEFOLD_FL` is meaningful only where a driver sets it (ext4, f2fs), so on
the others the answer is `Sensitive`, which is correct. Removing the ext4 test
changes no guarantee *on the traced paths* because `dir_identity` — which
still calls `require_ext4` at `:126` — is taken on the same handle first:
`target.rs:142-143` before `:153-156`; `managed_mutation.rs:162` before
`:171`, `:188` before `:191`/`:194`, `:288` before `:291`; `retained.rs:337`
before `:338-339`; `association.rs:148` before `:150`/`:167`;
`namespace_mutation.rs:166` before `:176`/`:179`. `interior.rs:72` and
`alias.rs:21` take handles that are already retained (identity taken at
`retain_opened`) — traced two levels, marked UNVERIFIED for every caller.

**Q3 — Kernel claims.** Confirmed at the source. `ioctl_getfsuuid`
(`fs/ioctl.c:766-777` v6.9): `if (!sb->s_uuid_len) return -ENOTTY;`.
`s_uuid_len` is zero unless set: `alloc_super` is `kzalloc` (`fs/super.c:317`
v6.9) and the only setter is `super_set_uuid` (`include/linux/fs.h:2557-2562`
v6.9: `sb->s_uuid_len = len; memcpy(&sb->s_uuid, uuid, len);`). ext4, xfs,
f2fs call it (P3-1 lines). btrfs does not: `memcpy(&sb->s_uuid,
fs_info->fs_devices->fsid, BTRFS_FSID_SIZE)` at `fs/btrfs/disk-io.c:3364`
(v6.9 and v6.12) and `:3553`/`:3555` (master), and a grep for `s_uuid_len` or
`super_set_uuid` across master `fs/btrfs/{disk-io,super,volumes,fs,ioctl}.c`
finds nothing — so `FS_IOC_GETFSUUID` is `ENOTTY` on btrfs on every kernel
read. The uapi ioctl is absent at v6.8 and present at v6.9 (`fs.h:75`,
`:237`). tmpfs: REFUTED as stated — see P2-1.

**Q4 — Consumer table.** C1-C15 sites and quoted strings confirmed
(`classification.rs:137-139`, `:141-143`, `:155-177`, `:178-186`, `:204-233`,
`:263-270`, `:275`; `transition.rs:45-51`, `:121-127`, `:220-226`, `:280-284`;
`residue.rs:89-101`, `:183-206`, `:242-248`, `:390-394`, `:606`;
`anchor.rs:239-246`, `:316`, `:490-506`; `namespace_mutation.rs:167-171`,
`:238-244`; `live.rs:166-187`; `abort/evidence.rs:44-50`, `:131-137`,
`:307-311`, `:343-361`; `artifact_facts.rs:9-13`, `:77-89`;
`platform.rs:211-218`). **Tier 1 is leaf-only as the code is written:**
`parent_is_current` compares the full `ObjectIdentity` of a re-traversed
*directory* (`observation.rs:162-172`); `namespace_mutation.rs:167`, `:238-239`
compare a directory's `durable()`; the rename-domain gate compares mount ids,
not identities. None has a content analogue. Where the table is wrong: the T2
mechanism for C1/C2 (P2-3) and the C8 T1 cell (P3-4). C3's "worse than
breaking" at T1 is right — `RetainedSource::Existing(identity)` is the only
thing distinguishing equal-content files at `classification.rs:178-186`.

**Q5 — Option (b).** Moves confirmed: `PROTECTED_SOURCE_DIGESTS` flat pin on
`capability.rs` (`scripts/checks/check_checked_artifact_boundaries.py:63`),
`PROTECTED_SOURCE_TREE_DIGESTS` on `catalog_lease.rs`/`pre_catalog.rs`/`catalog.rs`
(`:263`, `:265`, `:266`; `source_tree_digest` at `:1264` walks the subtree so
`provider/platform/linux.rs` is inside the `pre_catalog.rs` pin), `entry.rs`
byte pin (`:71`), taut allocations `protocol/checked_artifact.taut.py:16-19`,
`:34-37`, `:56-63`, `:103-115`, and `PRE_LOG_WIRE_SHA256`
(`protocol/check_log_additive.py:17`). The cross-check at
`catalog_bootstrap_record.rs:236` and the order key at `target.rs:443-461` do
force a matching profile variant, as the design says. Migration risk: real;
mechanism: incomplete (P1-1). LOC: optimistic (P3-5).

**Q6 — Option (a0).** Behaviour-preserving on ext4 (the test passes there).
Newly admits nothing on the traced paths (Q2). Moves the `pre_catalog.rs` tree
digest only; the `linux.rs:386` test `parent_mode_never_reports_ebadf_for_a_live_capability`
is unaffected. UNVERIFIED: whether the R0-L `mode_query_failure` negative row
(`provider.py:33`) is pinned to the ext4 pre-test in `run_probe.py`.

**Q7 — Observer classes.** Confirmed. `matches_request` (`authority.rs:101-119`)
pairs `family_key`, `action_key(expected, goal)`, `canonical_path_identity`,
`artifact_root_identity`, `expected`, `goal`, `operation`; the R3 residue is a
forward-pair `action_key` read by a reverse-pair request, exactly as
`GwzM5-8R2E-E45B-Report.md:35` records, and no tier field participates. The
detach window (`GwzM5-8R2E-E4-Close.md:43`) is a window in time; the forward
observer is already content-only (`live.rs:184-186`). One correction to the
design's own second-order note: because `artifact_root_identity` is compared
at `:110` *and* feeds `family_key`, a tier change bites at the prefix filter
(P2-3) before `matches_request` is reached — §4.4 rule 2 must be stated
against `residue.rs:127`, not `authority.rs:101-119`. Path nit: the
`execute.rs` cited via E4-Close is
`src/workspace_ops/merge/v1_lifecycle/finalization/execute.rs`; its `:48`/`:51`
were not read (design axis).

**Q8 — Passing defects.** Both confirmed verbatim. `docs/OperationModel.md:140-143`:
*"the weaker legacy identity probe, which also admits btrfs, xfs and zfs —
btrfs, xfs and zfs are refused"*. `linux.rs:351`: *"on pre-6.8 kernels reports
`ENOTTY`"* — the ioctl is absent at v6.8 and lands at v6.9, so the floor is 6.9
and the comment is off by one. Add `platform.rs:184` (P3-3).

---

## 3. Option ranking — reviewer's position

Agree with the design's order: **(a0) → (b) → (c) as the program; (a) alone
and (d) not.** (a) is refuted on its own terms by the btrfs mechanism (Q3) and
its tmpfs cost is larger than stated (P2-1). (d) is correctly rejected as a
process-global answer to a per-object question. (b) is the right relaxation
*provided* P1-1 is re-mechanised and P2-1/P2-2 are answered before the
protocol allocation is dualled — with those, its risk stays where the design
put it (the migration) rather than spreading to a new admission surface. (c)
is the destination and P2-3 is the one structural hole in it.

## 4. Claim tally

- **Verified: 37** — I1-I13 sites and call-site count (13); the six kernel
  mechanism claims for the ioctl, `s_uuid_len`, ext4/xfs/f2fs, btrfs at three
  tags, and uapi presence (6); C1-C15 sites and strings (9, grouped); the
  leaf-only-tier claim; (a0) preservation on traced paths; the pin/allocation
  list for (b) (3); the `matches_request` pairing and both observer
  non-interactions (2); both passing defects (2).
- **Refuted: 6** — tmpfs at v6.9/v6.12 (P2-1); the (b) migration mechanism
  (P1-1); "(b) admits exactly what the legacy probe admits" (P2-2); the C1/C2
  T2 mechanism (P2-3); "truncated `f_fsid`" (P3-2); the C8 T1 cell (P3-4).
- **Unverified: 6** — (b)/(c) LOC (P3-5); `FS_IOC_GETFSUUID`/`name_to_handle_at`
  on zfs, overlayfs, NFS, FUSE (design flags the same); every caller of
  `interior.rs:72`/`alias.rs:21` being retained-first; the R0-L
  `mode_query_failure` row's dependence on the ext4 pre-test; btrfs/xfs
  `f_fsid` derivation; `finalization/execute.rs:48/:51` (design axis).

## 5. Conditions for GO

1. **P1-1** — §3.2 "Migration" rewritten to the dual-tuple acceptance covering
   profile, target digest, collision digest and scratch name, with its own
   step and two fixtures (record + scratch).
2. **P2-1** — §3.1 tmpfs row corrected to v6.9; Q4 answered before (b).
3. **P2-2** — (b) states `parent_mode`'s admission and gives `PathEquivalence`
   a remedy (or an attempt-based default); ext4-specific error text at
   `linux.rs:59,161,167,199` listed among the moves.
4. **P2-3** — §4.2 adds `family_key`/`authority_name`/`family_prefix` to the
   nonce rebase; C1/C2 T2 cells and §4.4 rule 2 re-pointed at `residue.rs:127`.

P3 items are editorial and may land with (a0)'s doc fixes.
