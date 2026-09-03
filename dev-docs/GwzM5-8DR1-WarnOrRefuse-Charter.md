# DR-1 ship (1) — warn-or-refuse: crash recovery as a capability, not a gate

**Status:** CHARTERED 2026-09-03 on the operator's instruction of the same date
(quoted in §0). Home: R2-E phase E8 ("E8 stands"). Ship (1) of two; ship (2)
is M5c and is chartered separately after ship (1) lands. Lane owner: Fable.
No tag of any kind is cut on this charter ("Cut no tag of any kind, including
0.13.1, without asking").

**What this is not** (verbatim parked list): the graded-evidence protocol;
the catalog nonce / dual-tuple migration / option (b) of
`GwzM5-8DR1-FilesystemIdentity-Design.md`; reconciliation S2+; recording the
flag in the catalog; marker / lock / boundary conversion; record-root S5;
`gc_archived`, O14 freeze amend, `:279` wording, tier-2 archive, residue sweep.
The (b) design is NOT amended by this charter; it is parked as adopted.

## 0. The operator's product statement (2026-09-03, controlling)

> The merge runs on every filesystem. Crash recovery is a capability, not a
> gate. At merge start, probe whether the volume can prove durable identity
> for crash recovery. Decide once, at this start:
> * Above the bar: activate the catalog as today. No warning.
> * Below the bar, default: print one warning and continue without activating
>   the catalog. Shape: `warning: crash recovery is unsupported on <fs> (no
>   durable filesystem identity). Merge will continue. Use --filesystem-strict
>   to refuse.` Parenthetical may be `no durable filesystem identity`, `remote
>   filesystem`, or `volatile filesystem`. Name the filesystem; use `unknown`
>   if you cannot. Network and tmpfs are warning reasons, not a hidden name
>   list. Same rule on Linux, macOS, Windows.
> * Below the bar, `--filesystem-strict`: refuse as the remaining identity
>   gate does today. No other flag, env, or config.
>
> Below the bar, crash recovery is absent, not degraded. Ctrl-C / abort /
> resume of a live process work as today; they never needed the catalog. A
> later continue or abort of that attempt uses what that start opened
> (catalog or not). They do not consult the flag again.
>
> Strict bar: identity-based, not an ext4 name test. Strict refuses a volume
> that cannot prove durable identity (btrfs, remote, volatile). It admits
> ext4, xfs, f2fs alike. Do not bring `require_ext4` back under the flag.
>
> `--no-ff` on a below-bar volume must start, warn, and not activate the
> catalog. Docs: rewrite R0-L / OperationModel so the default is not
> ext4-only.

Clarified later the same day: the taut request/response protocol MAY change
for new functionality ("you will need a new field at least and a response
indicating that it's proceeding anyway"); a protocol change is isolated in a
closed system today (gwz-cli, gwz-py); the CLI may need to throttle how often
it echoes the message — a design element of this charter (§3.5).

## 1. The mechanism as found (verified by reading, 2026-09-03)

- **Where the catalog is activated.** A `--no-ff` start runs
  `v1_lifecycle/start.rs::handle_start_durable_v1` → the creation lease
  `checked.rs::V1MutationLease::acquire_for_merge_start` →
  `entry.rs::bootstrap_merge_start_parents` (activates the catalog through
  `recover_or_create` TWICE, admission then execution, and installs the two
  managed parents `.gwz/merge` and the preservation-bundle parent) → the v1
  record is created by `store/rewrite.rs::create_open` →
  `entry.rs::create_merge_store_record`. Then `service::run(ResumeStart)`
  takes `V1MutationLease::acquire_activated` (`service.rs:119-123`: the two
  FORWARD arms activate; every reverse arm takes the plain, capability-free
  lease). So one start activates the catalog three times today, and a
  `--continue` once more.
- **The gate.** Activation refuses through
  `catalog_lease/target.rs::finish` → `platform.dir_identity` →
  `platform/linux.rs::identity`, whose first line is `require_ext4(fd)?` (the
  ONE remaining magic-number test after (a0)), then `filesystem_uuid`
  (`FS_IOC_GETFSUUID`, refused by btrfs with `ENOTTY`, absent on kernels
  < 6.9) and `persistent_handle` (`name_to_handle_at`). macOS:
  `volume_attributes` requires `VOL_CAP_FMT_PERSISTENTOBJECTIDS` and
  `MNT_LOCAL` — already capability-based. Windows: `require_ntfs` is a name
  test (`windows.rs:99-131`). The refusal renders through
  `entry.rs::render_catalog_refusal` as `ErrorCode::UnsupportedOperation` with
  `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY` (`capability.rs:66-71`, which still
  says "admits ext4 only").
- **The record boundary is a second, weaker gate.** `create_merge_store_record`
  and every `entry.rs` root/bundle door go through `CheckedArtifact::acquire`
  (`observation.rs:82`), whose `durable_identity` is the LEGACY probe
  (`identity.rs:312-346`: `name_to_handle_at` + `statx MNT_ID`, no
  filesystem test). It admits btrfs, xfs, zfs, tmpfs, ext4; it refuses a
  filesystem without persistent handles (overlayfs without `nfs_export`,
  sshfs and other FUSE mounts without export support). This charter does not
  touch it (§4).
- **Precedent for "probe, do not refuse".**
  `runtime/dispatch.rs::forward_lifecycle_viability_window` already treats a
  failed activation as an answer (`.ok()?` → the v0 lifecycle keeps the
  record) rather than an error. The decision point of §3.1 generalises it.
- **Channels.** Core has no stderr; it has the operation event stream
  (`push_event.rs::EventEmitter::emit(kind, severity, …, message, …)`) and the
  response. gwz-cli attaches `StderrProgressSink` in Human mode (prints
  member progress only, never `message`), `JsonlSink` in `--jsonl`,
  `NullSink` in Json/Porcelain (`globalargs/dispatch.rs:10-17`). gwz-py embeds
  core in-process (`bridge.py::NativeCoreBridge`, PyO3) and speaks the taut IR;
  in `--jsonl` it streams events, otherwise it renders the response only.
  gwz-py's `render_clone_event` (`cli_local.py:129-132`) is the precedent for
  printing `Severity.warn` messages as `warning: …` on stderr.
- **Schema.** `gwz-core/protocol/gwz.taut.py`: `MergeRequest` fields 1-7
  (`preserve` is 7); `MergeResponse` fields 1-10 (`record` is 10);
  `EventKind` values 0-7; `Severity` has `warn=2`. Regeneration:
  `gwz-core/protocol/regen.py` (Rust `src/protocol/generated.rs`), gwz-py
  `scripts/regen_protocol.py` (the packaged `gwz.ir.json`);
  `scripts/check_protocol_drift.py` pins `PRE_LOG_WIRE_FINGERPRINT` over every
  pre-log message, so an additive field moves that pin deliberately.
- **R0-L.** `scripts/linux_identity_probe/provider.py:154` (`filesystem !=
  "ext4"` → unsupported), `:240` (tuple `filesystem == "ext4"`), the 15-row
  negative table (`:28-40`, tmpfs/overlay/network rows), `README.md`, the
  contract tests, and `.github/workflows/linux-identity-probe.yml` (both
  release architectures; runs on paths under `scripts/linux_identity_probe/`).
- **Docs gate.** `scripts/checks/check_merge_docs.py` +
  `merge_docs_manifest.json` pin, among others, the `gwz merge` synopsis line
  in `gwz-cli/docs/commands/merge.md:7` (row `:304`) and the no-ff help text
  (`:397`); `docs/OperationModel.md:125-166` carries the admission paragraph.

## 2. The decision, stated once

```text
CrashRecoveryDecision
  = Supported            -- identity proved; the catalog is activated as today
  | Unsupported { filesystem: Option<String>, gap: NoDurableIdentity | RemoteFilesystem | VolatileFilesystem }
```

Made by one function, `crash_recovery_decision(root) -> ModelResult<…>`,
in `checked_artifact::entry` (the catalog's public door), by attempting the
catalog's own admission probe (`dir_identity` on the retained workspace target,
the same call `catalog_lease/target.rs::finish` makes) WITHOUT creating or
recovering anything. `Unsupported` arms come from `CheckedFsError::Unsupported`
and from a `CheckedFsError::Io` raised BY THE PROBE (a probe that cannot answer
is an absent identity); an `Io` from anything after the probe stays an error.
The gap is `VolatileFilesystem` when the volume description (§3.3) says
volatile, `RemoteFilesystem` when it says remote, otherwise
`NoDurableIdentity`. The filesystem name comes from the same description,
`None` → rendered `unknown`.

## 3. Ship (1) design

### 3.1 Where the decision runs, and what each door does with it

- **Start** (`handle_start_durable_v1`): decide BEFORE any lease is taken.
  `Supported` → `acquire_for_merge_start` exactly as today (activation +
  managed-parent bootstrap). `Unsupported` + `filesystem_strict` → refuse with
  the strict sentence (§3.6), no lease, no record, no Git work. `Unsupported` +
  default → emit the warning (§3.4), then take a NEW catalog-free creation
  lease `V1MutationLease::acquire_for_merge_start_uncatalogued` = the plain
  lease + the two parents prepared through `CheckedArtifact::prepare_parent`
  (the v0 store's own route, `entry.rs::prepare_merge_store_parents`, widened
  to prepare `.gwz/merge` as well) — never `create_dir_all`, never inside the
  managed-parent provider seam (`r2d_seam_freeze.rs:205` forbids it there).
  `create_open` is unchanged: the record is still published through the
  checked boundary (§1, second gate; §4).
- **Forward service loop** (`service.rs:119-123`, `ResumeStart` and
  `Continue`): the same decision function, silently. `Supported` →
  `acquire_activated` as today. `Unsupported` → the plain lease. The
  `ResumeStart` invocation inside a start is the same process as the start's
  decision, so it must NOT re-warn: the start passes its decision into
  `service::run` (a parameter, not a re-probe) — this is the in-process form of
  "decide once". A later `--continue` is a new process: it decides again
  (same volume → same answer) and, when `Unsupported`, warns ONCE for that
  invocation (§3.5). It never consults the flag: `filesystem_strict` is
  accepted only with `op = Start`; on any other op it is refused as
  `InvalidRequest` in the drivers (the same shape as `--preserve` outside
  abort).
- **Abort, preserve, status, gc:** untouched. Abort stays capability-free by
  path (`CapabilityFreeAmendment.md` §6).
- **Ordinary and `--ff-only` starts:** untouched in ship (1) — they write v0
  and never reach this door until M5c (ship (2)).
- **Nothing is recorded.** Not in the catalog (operator), not in the v1
  record (the record contract's optional-field precedent is an amendment with
  its own dual — `GwzM5-8OperatorEscapeAmendment.md`; sidecars are forbidden by
  `GwzM5-8I2RecordContract.md:104`). Consequence, disclosed as the one
  deviation from the literal "uses what that start opened": a workspace MOVED
  between volumes mid-attempt re-decides at `--continue`. Moved onto an
  above-bar volume: activation bootstraps a catalog mid-attempt (harmless; the
  catalog tracks only its own publications). Moved onto a below-bar volume:
  continue proceeds catalog-less with the warning, where today it refuses.
  The per-attempt record field is the alternative if the operator wants the
  literal rule (§8).

### 3.2 The Linux gate made identity-based

`platform/linux.rs::identity`: delete `require_ext4(fd)?` (and the function,
now unreferenced). Add, BEFORE the UUID ioctl, a volatility refusal:
`fstatfs.f_type ∈ {TMPFS_MAGIC 0x01021994, RAMFS_MAGIC 0x858458f6}` →
`Unsupported(PersistentFilesystemIdentity, "volatile filesystem: contents do
not survive power loss")`. Rationale: tmpfs publishes a random per-mount UUID
on every kernel ≥ 6.9 (design §3.1, verified), so identity alone would admit
it; the operator refuses it as volatile. Keeping the refusal INSIDE the
provider keeps R0-L's negative tmpfs row true. Detail strings that say "ext4
returned…" become "filesystem returned…". `DurableObjectIdentityV1::linux_ext4`
and `SupportedFilesystemProfile::LinuxExt4FsIocGetFsUuidV1` keep their names:
the profile is a persisted catalog value and renaming it is a catalog-format
change, which is parked. A doc comment on each says so, dated. Net admission
after this step: ext4, xfs, f2fs and any filesystem that answers
`FS_IOC_GETFSUUID` with a nonzero 16-byte UUID and `name_to_handle_at`;
btrfs (`ENOTTY`), pre-6.9 kernels, tmpfs/ramfs, and every network mount are
`Unsupported`. macOS: unchanged (already capability-based). Windows:
`require_ntfs` STAYS in ship (1) — its capability-flag replacement
(`GetVolumeInformationByHandleW` flags `& FILE_SUPPORTS_OPEN_BY_FILE_ID`,
which NTFS and ReFS set and FAT/exFAT do not) is named here as the follow-up
and is not built blind from a host that cannot run the Windows matrix.

### 3.3 Volume description — name, locality, volatility

One provider function per platform, `describe_volume(dir) -> VolumeDescription
{ name: Option<String>, remote: bool, volatile: bool }`, used ONLY to word the
warning and choose its gap. Never a decision input except `volatile` (§3.2).
- Linux: name from `/proc/self/mountinfo` (the row whose mount id equals
  `statx MNT_ID` of the directory; field 9 is the fstype, e.g. `fuse.sshfs`),
  falling back to a small `f_type` magic table (ext4, xfs, btrfs, f2fs, zfs,
  tmpfs, ramfs, nfs, cifs/smb2, fuse, 9p, overlay), else `None`. `remote`
  when the name or magic is nfs, cifs/smb, 9p, afs, ceph, or any `fuse.`
  variant whose name is sshfs/rclone/davfs (a labelled list, documented as a
  wording aid). `volatile` when tmpfs/ramfs.
- macOS: `statfs.f_fstypename`; `remote = f_flags & MNT_LOCAL == 0`;
  `volatile = false` (a RAM disk is indistinguishable from a local volume;
  documented).
- Windows: `GetVolumeInformationByHandleW` name; `remote` when
  `GetDriveTypeW` on the volume root is `DRIVE_REMOTE` or the final path is a
  `\\?\UNC\` path; `volatile = false`.
- `unsupported.rs` (other targets): `None`, `false`, `false`.

### 3.4 The warning — one sentence, one channel each side

Exact text (operator's shape), rendered by core into ONE string:

```text
crash recovery is unsupported on <fs> (<parenthetical>). Merge will continue. Use --filesystem-strict to refuse.
```

`<fs>` is the description's name or `unknown`; `<parenthetical>` is exactly
one of `no durable filesystem identity`, `remote filesystem`, `volatile
filesystem`. Drivers print it as `warning: ` + the string on stderr.

Channels:
1. **Event.** A new `EventKind.diagnostic = 8`, emitted with
   `Severity::Warn`, `message = <the string>`, no member. Emitted at the
   decision (start; a continue that decides `Unsupported`), once per process.
2. **Response.** `MergeResponse.crash_recovery` (§3.7) carries the decision
   on every op that decided (start, resume/continue); `None` otherwise. This
   is the machine truth: Json/Porcelain/gwz-py consumers never depend on
   stderr.

### 3.5 Echo discipline (the operator's throttle question)

- Core emits the diagnostic at most ONCE per process by construction: the
  start passes its decision into the service loop; a continue decides once.
- gwz-cli's human sink prints `Severity::Warn`/`Error` diagnostic messages
  to stderr and de-duplicates identical texts within one invocation (a small
  `HashSet<String>` in the sink), so a future second emitter cannot spam.
  Json/Porcelain print nothing (the response carries it); `--jsonl` streams the
  event as it does every event.
- gwz-py: `--jsonl` streams it; human mode prints `warning: …` from the
  response's `crash_recovery` after the call returns (its merge path does not
  stream events in human mode; live printing is a separate improvement).
- **No cross-invocation throttling in ship (1)**: a workspace on btrfs sees
  one line per start and per continue. Suppressing it across invocations needs
  persistent state or configuration, both outside "no other flag, env, or
  config"; named as an open decision (§8), not built.

### 3.6 The strict refusal

`--filesystem-strict` + `Unsupported` refuses before any lease, with
`ErrorCode::UnsupportedOperation` and a rewritten
`PERSISTENT_FILESYSTEM_IDENTITY_REMEDY`: it names persistent file handles and
a durable filesystem identity (not "ext4 only"), the filesystem and gap from
§3.3, and the two escapes: run without `--filesystem-strict` to proceed
without crash recovery; `gwz merge --abort` for a merge already open. The
contracts pin `only_the_substrate_identity_capability_carries_an_actionable_remedy`
moves with it (drop the `--no-ff` word, add `--filesystem-strict`). The same
sentence is what activation's OTHER refusals (an `Io` after the probe, an
`Ambiguous`) keep rendering — those are errors, not the bar.

### 3.7 Protocol additions (`gwz.taut.py`; closed-system change, no version bump)

```text
MergeRequest.filesystem_strict = F(8, BOOL, optional=True)   # start only
MergeCrashRecoveryGap = Enum(no_durable_identity=0, remote_filesystem=1, volatile_filesystem=2)
MergeCrashRecovery = Msg(supported=F(1, BOOL), filesystem=F(2, STR, optional=True),
                         gap=F(3, Ref.MergeCrashRecoveryGap, optional=True))
MergeResponse.crash_recovery = F(11, Ref.MergeCrashRecovery, optional=True)
EventKind.diagnostic = 8
```

Regenerate both sides (`protocol/regen.py`; gwz-py `regen_protocol.py`);
move `PRE_LOG_WIRE_FINGERPRINT` deliberately with a dated comment; gwz-py's
`api.py`/`gwz.ir.json` are committed together with the flag.

### 3.8 The test-only seam

`cfg(test)` thread-local in `pre_catalog/provider/platform.rs` (`HostPlatform`
is the production provider; `filesystem.rs` already injects fake providers for
provider-level tests, but the merge-level tests reach `HostPlatform` through
`entry.rs`): `inject_identity_unavailable(gap, name)` makes the next
`dir_identity` (and `describe_volume`) answer `Unsupported` with that gap and
name, on any host filesystem. It arms once and disarms on use, like
`fail_next_checked_artifact_at`. With it, the g23 rows in §5 W3 run on the
ext4/APFS CI hosts.

### 3.9 What does not change (pinned by the existing suites and gates)

The catalog's on-disk format and tuple; the v1 record contract; the checked
boundary and its protected-source digests
(`check_checked_artifact_boundaries.py`) except the digest re-pin any touched
protected file needs; reconciliation (S2+ parked); the marker/lock/boundary
arms; the abort path; ordinary/`--ff-only` starts (v0); `ACTIVE_WRITER_FLOOR`;
`bootstrap_ownership_token`; the R0-L negative table's rows.

## 4. Known limits, stated before the build

1. **The record boundary's handle requirement remains.** A filesystem without
   persistent handles (overlayfs without `nfs_export`, sshfs, some FUSE)
   refuses `--no-ff` at `create_merge_store_record` today and after ship (1),
   with the boundary's own `MergeRecoveryRequired` sentence, not the warning.
   Ship (1) measures which of the CI-reachable filesystems hit it (W3's
   matrix rows) and records the list in the close note. This is a
   PRECONDITION finding for M5c (ship (2)): raising the floor routes ordinary
   starts through this boundary, so M5c must either keep a raw record write
   below the legacy bar or accept the regression knowingly — the operator's
   call at ship (2)'s charter.
2. **A moved workspace re-decides** (§3.1, last bullet).
3. **Windows keeps its name test** (§3.2) until a Windows-verified step
   replaces it.
4. **Kernels < 6.9** have no `FS_IOC_GETFSUUID`: every Linux volume is below
   the bar there (as today). The warning names the filesystem correctly; the
   gap is `no durable filesystem identity`.

## 5. Steps (each a builder package; single-axis review by reading + cheap checks)

| Step | Scope | Files | LOC (aspirational) | Depends on |
|---|---|---|---|---|
| **W1** | Protocol additions §3.7; regen both sides; drift pin; round-trip tests | `protocol/gwz.taut.py`, `src/protocol/generated.rs`, `tests/protocol.rs`; gwz-py `protocol/generated/*`, `scripts/check_protocol_drift.py`, `test_protocol.py` | ~150 | — |
| **W2** | Linux gate §3.2 + `describe_volume` on Linux/macOS/Windows/unsupported §3.3 + provider unit tests (magic table, mountinfo parse, volatile refusal) | `platform/linux.rs`, `macos.rs`, `windows.rs`, `unsupported.rs`, `platform.rs`, `capability.rs` detail strings | ~300 | — (∥ W1) |
| **W3** | The decision function §2; start/forward plumbing §3.1; the uncatalogued creation lease; warning string + `diagnostic` emission §3.4; response population; strict refusal + remedy rewrite §3.6; the seam §3.8; g23 rows: default below-bar `--no-ff` start warns, creates no `catalog-final`, writes a v1 record and completes; the same with `--filesystem-strict` refuses before any record; continue of a catalog-less attempt proceeds silently-after-one-warning; abort of one clears it; above-bar start still activates (existing pins); `filesystem_strict` on a non-start op refused; contracts pin moved; driver pins moved + `check_pins.py` | `entry.rs`, `v1_lifecycle/start.rs`, `checked.rs`, `service.rs`, `capability.rs`, `platform.rs` (seam), `workspace_ops/tests/g23/*`, `interface_tests/contracts.rs`, `run_r4bg_aggregate_gates.py` | ~450 | W1, W2 |
| **W4** | Drivers: gwz-cli `--filesystem-strict` (merge only; `InvalidRequest` off-start), human sink prints + de-dups warn diagnostics §3.5, JSON/porcelain carry `crash_recovery`; gwz-py `--filesystem-strict`, response rendering, tests; CLI reference/help text | gwz-cli `globalargs/parser.rs`, `clirequest/merge.rs`, `progress_detail.rs`, `merge_render.rs`, tests; gwz-py `cli_merge.py`, `client.py`, `cli_render.py`, tests | ~250 | W1 (W3 for end-to-end rows) |
| **W5** | R0-L rewrite (`provider.py` positive checks admit the identity contract, not `ext4`; README; contract tests; the ext4 loop-mount row stays the reference; an xfs row if `xfsprogs` is on the runner); `docs/OperationModel.md` admission section; `gwz-cli/docs/commands/merge.md` synopsis + "Crash recovery and filesystems"; `MergeRecovery.md`, `CLI.md`; `merge_docs_manifest.json`; `GwzReleaseNotes-v0.14.0.md` DRAFT (no tag) | as named | ~300 | W3 (wording), W4 (help text) |

Order: W1 ∥ W2 → W3 → W4 ∥ W5. Every step: `cargo fmt` per crate before
delivery; lane gate over the range; digests recomputed for any protected file;
pins moved for any `#[test]` added or removed and `check_pins.py` green; the
Linux identity probe workflow and the platform matrix dispatched on W2's and
W3's branches (the Linux provider compiles only on Linux); the docs gate
(`check_merge_docs.py`) on W5. Reviews: verify mechanism and evaluation order
by reading; reproduce one measured claim; never re-run a partition the builder
ran at a byte-identical tree (`review-cost-discipline`).

## 6. Acceptance of ship (1)

On an ext4/APFS/NTFS host nothing observable changes except the new flag, the
new response field, and the rewritten remedy. Under the seam (and on the
matrix's btrfs/tmpfs/NFS rows where available): a `--no-ff` start warns once
with the exact sentence, writes a v1 record, creates no catalog, completes,
and its continue/abort behave as §3.1; with `--filesystem-strict` it refuses
before any write. R0-L green on both architectures. No tag.

## 7. Deviations from the operator's literal words, disclosed

1. "Uses what that start opened" is implemented as "decides the same way on
   the same volume" (§3.1); the literal rule needs a v1 record field.
2. Windows keeps `require_ntfs` (§3.2) for one more step.
3. Protocol additions per the operator's later clarification (§0, §3.7).

## 8. Open decisions (one line each; defaults stated, none blocking)

1. Per-attempt record of the decision (a v1 record optional field, amendment
   with dual, ~200 LOC) — default: not in ship (1).
2. Cross-invocation throttling of the warning — default: none.
3. Windows `FILE_SUPPORTS_OPEN_BY_FILE_ID` replacement of `require_ntfs` —
   default: the next Windows-verified step.
4. An xfs row in R0-L — default: yes if `xfsprogs` installs on the runner,
   else deferred with a note.
