# R2-D Phase 4 Step 4.1 — legacy rename-edge conversion: single-axis Code review

Date: 2026-08-23. **Single-axis Code-lead** (`GwzM5-8R2DInterfaceFreeze.md` §9
:1345-1351: "Everything else is **single-axis with automatic escalation on any
P0/P1/P2**: … and Phase 4 steps 4.1-4.3 **including the Phase 4 settle**").
Round 1 of a two-round cap. Peer-blind: the implementer's report was read as
*claims to verify*, never as evidence; every count below was recounted and every
affordable gate re-run by this review in the live tree.

**Escalation is NOT triggered** — no P0, P1 or P2 against the object. Six P3
findings file and continue (§9: "P3 findings file and continue; they do not
become packages and do not enlarge R2-D"). One **P2-grade latent production
defect found *and closed* by this step** is recorded at §5; it is a grade of
pre-existing behavior, a credit to the step and not a finding against it, and it
therefore fires no §9 trigger.

Object: the uncommitted Step 4.1 edits over gwz-core base `3a45619` ("Land R2-D
Step 3.3: coordinator glue — Phase 3's step list closes (review GO round 2)"),
exactly eleven files — confirmed the whole dirty set by
`git -C gwz-core status --porcelain` (10 ` M` + 1 `??`):

| File | Disposition |
| --- | --- |
| `checked_artifact/tests/leaf_publication.rs` | **NEW** (282 lines) |
| `checked_artifact/platform.rs` | modified (+76) |
| `checked_artifact/transition.rs` | modified (+31/−5) |
| `checked_artifact/residue.rs` | modified (+29/−4) |
| `checked_artifact/fault.rs` | modified (+6) |
| `checked_artifact/tests.rs` | modified (+2) |
| `checked_artifact/interface_tests/capability_permit.rs` | modified (+54) |
| `checked_artifact/tests/recovery_protocol.rs` | modified (+4) |
| `checked_artifact/tests/removal_recovery.rs` | modified (+3) |
| `checked_artifact/tests/staging_recovery.rs` | modified (+3) |
| `scripts/checks/check_checked_artifact_boundaries.py` | modified (+17/−9) |

Baseline: gwz-core `3a45619`; gwz-dev repo HEAD `a220d5c`.

---

## 1. Verdict

**GO.**

The step converts §4.3 rows E18-E21 in situ through P1, exactly as the freeze
and the plan write it, and it does so **without** taking the route this
review's own brief specified. That divergence is not a deviation to be
tolerated — it is the correct reading of the authorities, and taking the brief's
route instead would have been a P1-class production regression. §2 establishes
that at the code level. Every other duty verifies: no successful path moves, no
error code moves on any supported target, all four expectations chain to
observed durable facts rather than caller restatements, the allowlist shrink is
exact by the checker's own counter, exactly one digest pin moves, the census is
untouched, and the `cleanup.*` zero-flip is the correct reading of the plan's
clause.

Findings, most severe first — all P3:

| # | Finding |
| --- | --- |
| [P3-1] | The twin seam duplicates ~25 lines of identity/byte verification with `publication.rs`'s; both halves are digest-pinned so drift is *reviewed*, not silent, but nothing pins their **equivalence**. Docket unification at R2-F legacy removal. |
| [P3-2] | The report's macOS narrowing claim ("non-persistent-id macOS volumes") is **false** — both arms enforce `VOL_CAP_FMT_PERSISTENTOBJECTIDS` identically. The true macOS narrowing is `MNT_LOCAL` (remote volumes). The conclusion is unaffected and the *liftable* §3.5 block does not carry the error; the report's prose does, and the settle reads the report. |
| [P3-3] | The drafted §3.5 block says "the row below is left as written"; house placement puts the annotation **after** its map row, so the referent is above. One-word fix by the lane owner at lift. |
| [P3-4] | `BeforeSealedLeafPublication` is one variant announced from four sites, so `fail_next_checked_artifact_at` can only ever address the **first** crossing (E21) in the three matrices. Unlike the V1 key vocabulary this enum is census-free, so four variants would have cost nothing. Per-edge coverage is delivered instead by `leaf_publication.rs`'s engineered resume states — sound, but the single-variant choice should be *recorded* as deliberate (as the freeze does for `staging_directory_flush`) or split at 4.2. |
| [P3-5] | "No error code moves" holds on all three supported targets but not universally: on `all(unix, not(linux), not(macos))` and `not(unix), not(windows)` the E21 refusal moves from `self.code` to `ErrorCode::UnsupportedOperation`. Both are refusals on targets where the edge was already unconditionally impossible. The claim needs its supported-target qualifier in the settle record. |
| [P3-6] | Gross added LOC is 507 against the aspirational <500 (net +489, under). Recorded per the brief, not a trim request — the 282-line test file *is* the behavior-unchanged evidence. |

Opinions requested by the brief are at §8 (digest pins: **pin now**) and §9
(cohesion: accept 1,093 now, **bind the split to 4.2 by name**).

---

## 2. The load-bearing call — P1's arms vs `publish_verified_no_replace`

This is the verdict-deciding question. The implementer routed E18-E21 through a
new `platform::publish_verified_leaf_no_replace` that composes P1's *arms*,
rather than calling `provider/publication.rs::publish_verified_no_replace`. I
verified all three legs of the justification from code, and found a fourth the
report does not state.

### 2.1 (a) The identity compare is `HostPlatform`-bound, and `HostPlatform` admits only the closed table — **TRUE**

`publication.rs:148` reads
`if encode_identity(&HostPlatform.file_identity(source_handle.file())?) != expected.expected_identity()`.
`HostPlatform` is a concrete unit type imported at `publication.rs:9`, not a
generic parameter — there is no seam at which a caller could supply a different
identity provider. Its three arms:

| Target | Gate | Where |
| --- | --- | --- |
| Linux | `require_ext4` refuses any `statfs.f_type != EXT4_SUPER_MAGIC` ("only local ext4 with `FS_IOC_GETFSUUID` is admitted"), then `filesystem_uuid` needs the `FS_IOC_GETFSUUID` ioctl | `provider/platform/linux.rs:126`, `:136-149`, `:153-158` |
| macOS | `ATTR_CMN_OBJPERMANENTID` + `VOL_CAP_FMT_PERSISTENTOBJECTIDS` **and** `MNT_LOCAL` ("remote macOS filesystems are not an admitted profile") | `provider/platform/macos.rs:85-157`, `:165-171` |
| Windows | `name != "NTFS"` → "only local NTFS is an admitted Windows profile", plus `FILE_ID_INFO` | `provider/platform/windows.rs:75-94`, `:130-133` |

### 2.2 (b) The legacy leaf writer is the LIVE production path — **TRUE**

`recover_or_create` still has **no production caller**: every reference outside
its own definition (`catalog/bootstrap.rs:213/:220`) and the `catalog.rs`
re-export is either a `tests*` file, a comment, or `interface_tests/`. Plan §5
item 2 holds.

The legacy writer, by contrast, is reached from live merge and stash code:
`workspace_ops/merge/root/artifact_facts.rs:39/:69` (`write_checked`,
`remove_exact`) called from `merge/root/abort.rs:387` and
`merge/abort/evidence.rs:152/:158/:164`;
`merge/preserve/checked_bundle.rs:93`; and
`git/gitbackend/preservation_root/files.rs:43`. All of these land on
`CheckedArtifact::replace_exact` / `remove_exact` — i.e. on E18-E21.

Its identity vocabulary is `checked_artifact/identity.rs`, and it is genuinely
broader than the closed table on **all three** supported targets:

| Target | Legacy vocabulary | Refused by `HostPlatform` but accepted by legacy |
| --- | --- | --- |
| Linux | `name_to_handle_at(AT_EMPTY_PATH)` + `statfs.f_fsid` (`identity.rs:255-290`) — no filesystem-type gate, no `FS_IOC_GETFSUUID` | btrfs, xfs, zfs, overlay — every exportable non-ext4 filesystem, and ext4 on kernels < 6.7 |
| macOS | `ATTR_CMN_OBJPERMANENTID` + `VOL_CAP_FMT_PERSISTENTOBJECTIDS`, **no `MNT_LOCAL` check** (`identity.rs:355-428`) | network-mounted workspaces (SMB/NFS/AFP) that still promise persistent object ids |
| Windows | `FILE_ID_INFO` + volume GUID path, **no NTFS name gate** (`identity.rs:500-560`) | ReFS and any non-NTFS volume exposing `FILE_ID_INFO` |

### 2.3 (c) Therefore the brief's route was a behavior break — **TRUE**

Routing E18-E21 through `publish_verified_no_replace` would have made live
merge/stash writes begin returning a typed `unsupported` refusal on every
non-ext4 Linux workspace, every remote macOS volume, and every non-NTFS Windows
volume. That is precisely what plan §4 Step 4.1's own "with identical external
behavior" clause forbids. Under any reading of the duty hierarchy the
implementer was right to stop.

### 2.4 A fourth leg the report does not state: the route was not even compilable

`publish_verified_no_replace` is declared `pub(super) fn` at
`publication.rs:132` — visible only inside
`checked_artifact/capability/pre_catalog/provider`. A call from
`checked_artifact/transition.rs` or `residue.rs` **cannot compile** without
widening the seal on the sealed primitive. The brief's route was therefore not
merely narrowing; it required breaking the seam the checker exists to protect.
Worth adding to the settle record as the shortest form of the argument.

### 2.5 Freeze conformance of the twin seam — **CONFORMANT**

The freeze texts say P1, not `publish_verified_no_replace`:

- §4.1's P1 row names its **arms** in the platform columns:
  "`platform::open_rename_source` `platform.rs:23-46` … then
  `platform::rename_open_source` `platform.rs:78-96` — retains the
  identity-checked handle across a relative no-replace rename".
- §4.3 rows E18-E21 read "P1 (replaces `platform::rename_relative`
  `platform.rs:276`)" and **all three platform cells name `platform.rs`**, not
  `publication.rs`. The frozen table therefore already places these edges'
  primitive in `platform.rs`.
- §5 decision 2 (:1169-1171) reads "Convert-in-place converts E18-E21 in situ
  **through P1**" — the family, whose arms are the platform pair.

The new composition is exactly `open_rename_source` → identity read-back through
the retained handle → bounded byte read-back through the same handle →
`rename_open_source(.., replace=false)` (`platform.rs:192-233`). It is P1's arms
with the legacy family's own durable identity vocabulary. **Conformant.**

The physical edge is confirmed unchanged in both directions:
`rename_open_source` on non-Windows *is* `rename_relative(.., replace)`
(`platform.rs:78-93`), and `rename_relative` on Windows *is*
`open_rename_source` + `rename_open_source` (`platform.rs:428-446`).

### 2.6 Is the twin seam UNSOUND? — **NO**, but see [P3-1]

The duplicated surface is ~25 lines: `try_reserve_exact(len+1)` →
`take(len+1).read_to_end` → byte compare, plus the identity compare. That is a
real duplication of `publication.rs:155-181`. Four things bound it:

1. The platform-critical half — the arms — is **not** duplicated; both seams call
   the same `open_rename_source`/`rename_open_source`.
2. Both halves are digest-pinned. `checked_artifact/platform.rs` is a flat
   `PROTECTED_SOURCE_DIGESTS` entry; `publication.rs` sits under the
   `checked_artifact/capability/pre_catalog.rs` **tree** pin (verified:
   `source_tree_digest` rglobs the whole `pre_catalog/` subtree,
   `check_checked_artifact_boundaries.py:777-793`). A change to either forces a
   reviewed pin refresh. Drift can happen, but not silently.
3. The new `capability_permit.rs` pin
   (`the_legacy_leaf_edges_use_one_source_associated_publication_seam`) asserts
   one definition of the composition, exactly 2 calls in each legacy file, and
   **zero** occurrences of any of the three raw tokens in either — which is what
   stops two seams becoming three. Verified green standalone, 1 passed / 0 failed.
4. Plan §5 item 3 schedules the legacy path for removal at R2-F, so the
   duplication has a defined end date.

**Ruling: SOUND.** [P3-1] is the residual: nothing machine-checks that the two
verification *bodies* stay equivalent. Remedy, for the docket rather than for
this step: at R2-F legacy removal the duplication disappears; if it must live
longer, parameterize `publish_verified_no_replace` over its identity provider
and let the legacy caller pass the `super::identity` vocabulary — one function,
two providers. Doing that inside 4.1 would have widened the sealed primitive's
visibility and its caller pin, which is scope creep the step was right to refuse.

### 2.7 The checker consequences the divergence removes — **VERIFIED**

The report claims no `CATALOG_PUBLICATION_CALL_COUNTS` entry and no 13→N permit
bump are owed. Confirmed structurally: `publication_callers` is computed only
over `production_rust_files(source / "checked_artifact/capability/pre_catalog/provider")`
(`check_checked_artifact_boundaries.py:848-856`), so a caller outside that
directory cannot be expressed in that map at all. `CATALOG_PUBLICATION_CALL_COUNTS`
is byte-untouched and still sums to 13 (1+5+2+1+2+2); `capability_permit.rs:139`'s
`assert_eq!(callers.matches("publish_verified_no_replace(").count(), 13)` is
untouched — the diff is a single additive hunk at `@@ -141,6 +141,60 @@`.

---

## 3. "Identical external behavior" — the four conversions against their old raw calls

Read each conversion against the raw call it replaces. **No successful path
moves.** **No error code moves on any supported target** ([P3-5] is the
unsupported-target caveat).

| Edge | Site | Expectation supplied | Where that expectation comes from | Delta |
| --- | --- | --- | --- | --- |
| E18 | `transition.rs:265-297` `detach_existing` | `{ identity: &source_identity, bytes: source_bytes }` | `observe_leaf_exact(dir, &self.leaf, ..)` at `:218`; its durable half already checked `!= *expected_identity` against the authority record's `RetainedSource::Existing` at `:255-259` | none on success |
| E19 | `transition.rs:378-402` `publish_goal` | `{ identity: &staged.identity, bytes: goal }` | `inspect_family`'s `staged_goal`, which is `Some` **only** when the observed fact equals `Bytes(goal)` *and* the entry's own name is `goal_name(family, action, identity.name_digest())` (`residue.rs:177-186`) | none on success |
| E20 | `residue.rs:385-408` `ensure_goal` | `{ identity: &identity, bytes: goal }` | `identity::file_identity(&file)` on the **write handle**, pre-existing at `:386` and already used to derive the destination name | none — nothing new is computed |
| E21 | `residue.rs:498-524` `publish_scratch` | `{ identity: &identity, bytes }` | **adds** `identity::file_identity(&file)` on the write handle before `drop(file)` | see below |

**E18's new arm is unreachable, verified.** The conversion adds
`let CheckedArtifactFact::Bytes(source_bytes) = &leaf.fact else { return Err(.., "exact existing source has no bytes") }`.
`observe_leaf_exact` sets `identity: Some(..)` **only** on the `Bytes` arm
(`observation.rs:278-282`; every other return carries `identity: None`), and the
immediately preceding guard at `transition.rs:255-259` already fails closed when
`leaf.identity` is `None` (`is_none_or` → true → `Err`). So `Bytes` ⟺ `Some`
identity, and the new arm cannot be reached. It replaces an implicit invariant
with a typed refusal rather than a panic — strictly the safer form. No behavior
moves.

**E21's added observation introduces no new failure mode.** Verified by walking
the drive: `publish_scratch` is called only from `ensure_authority:314`, and
`ensure_authority:315` immediately calls `rebarrier_exact`, whose first act is
`observe_leaf_exact(dir, name, ..)` on the record just published — which calls
`identity::file_identity` and maps failure through `observation.rs:340-345` to
**`ErrorCode::UnsupportedOperation`**. The composition's own identity failure
maps to the same code with a byte-identical message form ("checked {label}:
durable filesystem identity is unsupported: {cause}"). So on any filesystem
lacking durable identity, the old code failed one step later with the *same*
error code; the only delta is that the drive now fails **before** publishing an
orphan authority record. The report's claim is exactly right.

**Where behavior moves, precisely.** Only on the failure path, only under a
same-user substitution of the source inside the window the composition closes,
and only in the direction of refusing earlier. That is the freeze's own
drift-rejection property arriving at these four edges.

---

## 4. The four substitution rows and the new boundary

All five `leaf_publication` tests re-run green in the live tree (5 passed / 0
failed), and the seam pin 1/0. Each test drives a real flow front and
substitutes inside the closed window:

- **E21** — fresh drive; the authority publication is the first sealed leaf edge.
- **E20** — resumes behind a published authority (`AfterAuthorityParentBarrier`
  crash), so `ensure_authority` early-returns at `residue.rs:242-251` without
  calling `publish_scratch` and the goal staging is the first crossing.
- **E18** — resumes behind a staged goal (`AfterGoalParentBarrier`), so
  `ensure_goal` early-returns at `residue.rs:342-345` and the detach is first.
- **E19** — resumes after detach (`AfterDetach`), so the managed publication is
  first.

Each setup is precisely engineered so the intended edge is the drive's first
crossing of the single boundary. Verified against the early-return sites.

**Red-against-raw, substantiated.** I did not reproduce the scratch revert — my
brief permits only one file write — but each assertion is decisively red under
the raw rename by construction, and the analysis independently predicts the
implementer's reported 1-passed/4-failed split:

| Test | Assertion the raw rename violates |
| --- | --- |
| E18 | `assert_eq!(fs::read(&managed).unwrap(), b"source")` — the raw rename moves the substituted object into the private area, leaving the managed name absent; `unwrap()` panics |
| E19 | `assert!(!managed.exists())` — the raw rename delivers the substituted object onto the managed name |
| E20 | `family_entries(..).len() == 1` — the raw rename publishes a `.goal` alias whose name encodes the *original* digest, making it 2 (and `inspect_family` then marks the family foreign) |
| E21 | `.unwrap_err()` — the old code **succeeds outright**; see §5 |
| primitive | calls the composition directly, so it is unaffected by reverting the four call sites → the **1 that passes** |

**The new boundary is census-free.** `CheckedArtifactFault::BeforeSealedLeafPublication`
is added to the **legacy** `CheckedArtifactFault` enum (`fault.rs:41-46`), not to
`CheckedArtifactFaultKeyV1`. `fault()` compiles to `let _ = (boundary, code, label); Ok(())`
outside `cfg(test)` (`fault.rs:111-113`). `interface_tests/fault_expected_keys.rs`
is **byte-untouched** — it is absent from the eleven-file dirty set — so
`FAULT_INJECTION_SOURCES`, its completeness anchor, and `EXPECTED_KEY_COUNT`
(165) are all unchanged. **Census: 165, held. Zero keys minted, zero retired.**

**Driven on all three legacy matrices — and non-vacuously.** Added to
`recovery_protocol::every_non_cleanup_fault_restarts_to_one_exact_goal`,
`removal_recovery::every_removal_fault_restarts_to_exact_absence`, and
`staging_recovery::every_missing_source_fault_restarts_to_one_exact_goal`. Each
loop asserts `.is_err()` on the injected drive, so a boundary that were never
crossed would fail the row rather than pass vacuously. All three re-ran green.
[P3-4] is the coverage nuance: because `fail_next_checked_artifact_at` consumes
on the first match, these rows only ever drive the **E21** crossing.

---

## 5. The E21 finding — a latent production defect, found and closed by this step

Recorded here per the brief, explicitly and separately from the finding list.

**What it was.** Before this step, `residue::publish_scratch` staged the
authority record, `sync_all`-ed it, and then executed a raw
`rename_relative(dir, scratch, dir, name, false)`. A same-user substitution of
the scratch inside that window with a **byte-identical, different-inode** object
completed the whole replacement **successfully** — no post-hoc rejection
anywhere. The mechanism is exact and I verified every link:

1. The raw rename moves whatever object then bears `scratch` onto `name`.
2. `rebarrier_exact` reopens by name and fsyncs; it performs no identity compare.
3. `inspect_family` handles the authority entry in a branch that consults
   **only its bytes** — `if text == expected_authority_name { let CheckedArtifactFact::Bytes(bytes) = observed.fact else {..}; authority = decode(&bytes); continue; }`
   (`residue.rs:158-168`). The `let Some(identity) = observed.identity` guard is
   *after* that `continue`.
4. Byte-identical input decodes to the identical authority value, so
   `after.authority.as_ref() == Some(&authority)` holds and `ensure_authority`
   returns `Ok`.

The root cause is structural and worth recording on its own: **`authority_name`
is the only family name that does not embed an identity digest** —
`format!("ca1-{family}-{action}.authority")` (`authority.rs:226-228`), against
`goal_name`/`source_name` which both interpolate `hex(identity)`
(`:234-240`). Those two names are self-checking against substitution; the
authority name is not, which is exactly why E19 and E20 were caught post-hoc and
E21 was not caught at all.

**Grade, had it shipped to R2-E: P2.** Reasoning, both directions:

- *Toward severity:* it is a genuine violation of the freeze's own
  drift-rejection property, at the family's **authority anchor**, on the **first
  sealed edge of every drive**, and it was materially worse than E18-E20 because
  it produced a successful completion rather than a typed refusal. R2-E is the
  lane where this same legacy family becomes consumer-facing, so it would have
  been shipping into wider exposure.
- *Against severity:* the trigger is not benign — `scratch_name` is
  `getrandom`-suffixed (`authority.rs:242-247`), so two concurrent drives cannot
  collide; it requires an actor with write access inside
  `.gwz/checked-artifacts`, squarely within the amendment's accepted
  cooperating-same-user boundary. The substituted record is byte-identical, so
  recovery semantics do not change at the moment of substitution, and the one
  residual capability it confers — holding a live handle to rewrite the record
  later — is power that actor already has by virtue of directory write access.
  Any later byte change is caught by `decode` + `matches_request` → `foreign` →
  typed `MergeRecoveryRequired`, not silent corruption.

Not P1: there is no reachable path to silent data loss or to wrong managed
content, and no non-adversarial trigger. Not P3: a successful completion over an
object the drive did not write is a first-class property violation, not a
recording matter. **P2**, closed by this step at the moment it was created.

---

## 6. `cleanup.*` zero-flip — the reading is CORRECT

The eleven keys, checked one by one against code:

| Keys | Edge set | Verified at |
| --- | --- | --- |
| `worklist_scratch_create`, `worklist_scratch_write`, `worklist_scratch_flush`, `worklist_publish`, `worklist_reobserve` (5) | the `CleanupWorklistV1` record's own scratch/publish/reobserve lifecycle | `protocol/cleanup.rs:297` (`ProtocolRecordKindV1::CleanupWorklist`); `BaseActionSlotV1::CleanupWorklist` `protocol/slots.rs:112`; role mapping `namespace/roles.rs:23/:37` |
| `source_reobserve`, `destination_reobserve`, `alias_retire`, `retired_alias_reobserve`, `row_complete`, `completion_reobserve` (6) | retirement of the three `CleanupAliasV1` aliases into `RetiredSourceAlias`/`RetiredGoalAlias`/`RetiredAuthorityAlias` | `namespace/mod.rs:117-147` `cleanup_retirement`; `namespace/host.rs:622-633` `retire_exact` |

Both groups require an `AdmittedActionV1` and a scheduled action directory:
`cleanup_retirement` reads `self.admitted_action.reservation().schedule().cleanup_aliases().mask()`
and refuses `ScheduleErrorV1::OutOfBounds` without it, and `CleanupWorklist` is a
`BaseActionSlotV1`, i.e. an action-directory slot. That is consumer conversion,
which plan §5 items 1-2 place outside R2-D.

The legacy twin is `checked_artifact/cleanup.rs`, whose only physical edges are
three `remove_file` calls at `:87`, `:132`, `:167`. Confirmed by grep: the file
contains **no** rename token of any kind, which is also why it was never in
`RAW_RENAME_CALL_ALLOWLIST`. Removals, not renames — and the freeze's E4-retire
activation record (`§4.3` :955-961) already establishes that no admitted family
expresses a removal.

**Ruling.** Step 4.1's scope is the four rename sites; it converts zero
`cleanup.*` edges. The plan's clause "activate `cleanup.*` rows for the cleanup
edges this converts" has an **empty antecedent**, so RemPlan §10's duty never
attached rather than being deferred — the implementer's phrasing is exactly
right, and materially different from the Step-3.1 deferral record, which was a
real duty split across two steps of one package. Re-reserving all eleven for
R2-E is the map's own carried clause ("any key whose edge genuinely converts
later is explicitly re-reserved for R2-E/R2-F in the same update", freeze
:622-623) applied correctly.

**Is the drafted §3.5 block fit to lift? — YES, with one fix.**

- *House form:* matches the `managed_bootstrap.*` precedents — block quote,
  dated parenthetical naming the landing and what it discharges, bolded count
  sentence, a "**Counts.**" paragraph at the end. ✓
- *Claims:* every factual assertion in the block was checked above and is TRUE.
  It does **not** carry the macOS error of [P3-2]. ✓
- *Arithmetic:* 0 executed / 11 reserved (5+6=11 ✓); 165 total unchanged ✓;
  permit 13 ✓; `CATALOG_PUBLICATION_CALL_COUNTS` unchanged ✓; allowlist 5→6 ✓.
- *[P3-3]:* "the row below is left as written" — house placement puts the
  annotation **after** its map row (`cleanup.*` is at freeze :613, between the
  `managed_bootstrap.*` annotations and `barrier.*`), so the referent is the row
  **above**. One-word fix at lift.

---

## 7. Provenance — the twice-fired class

The composition's gate verifies identity and bytes **observed through the very
handle it renames**: `open_rename_source` returns `OpenedRenameSource`, and the
identity read (`file_identity(handle.file())`) and the bounded byte read
(`handle.file_mut().by_ref().take(len+1).read_to_end`) both go through that
retained handle before `rename_open_source(&handle, ..)` executes the edge. The
binding fact — which object moves — is observed, never restated.

Call-site expectation origins, audited individually:

| Edge | Origin | Caller restatement? |
| --- | --- | --- |
| E18 | `leaf.identity` / `leaf.fact` from `observe_leaf_exact`; the identity's durable half was checked against `authority.retained_source`'s `RetainedSource::Existing` before use | **No** — observed, then chained to the durable authority record |
| E19 | `staged.identity` / `staged.name` from `inspect_family`, which admits a `staged_goal` only when the observed bytes equal `goal` **and** the entry's own name equals `goal_name(.., identity.name_digest())` | **No** — the name itself binds the identity |
| E20 | `file_identity(&file)` on the write handle, plus `goal` which was just `write_all`-ed and `sync_all`-ed through that handle | **No** |
| E21 | `file_identity(&file)` on the write handle, plus the `bytes` just written through it | **No** |

E18's `bytes` deserve one explicit word, since they are the closest thing to a
restatement: they are `&leaf.fact`, the **observed** fact, which was separately
checked equal to the caller's `*expected` at `:255`. The value handed to the
gate is the observation, not the caller's argument. Clean.

No new gate accepts a caller-supplied restatement of a binding field.

---

## 8. Checker and companions — all verified independently

**Allowlist shrink — exact.** Recomputed with the checker's *own* masked
bare-identifier counter (`mask_non_code` + the `\btoken\b` scan excluding
`fn `-prefixed definitions, `:889-903`), driven from a fresh import of the
script:

```
checked_artifact/platform.rs      {open_rename_source: 6, rename_open_source: 6, rename_relative: 5}  == allowlist
provider/publication.rs           {open_rename_source: 1, rename_open_source: 1, rename_relative: 0}  == allowlist
checked_artifact/transition.rs    {0, 0, 0}   allowlist: {}  ✓
checked_artifact/residue.rs       {0, 0, 0}   allowlist: {}  ✓
checked_artifact/cleanup.rs       {0, 0, 0}   allowlist: {}  ✓ (never listed)
```

The subsystem's whole raw-rename surface is now two sealed files. The rewritten
allowlist comment records the retirement accurately.

**`APPROVED_RUST_PATH_EDGES`** gains `("checked_artifact/tests.rs", "tests/leaf_publication.rs")`
with a citing comment — required by the checker's `#[path]` inventory rule, and
the rule passes.

**Exactly ONE pin moves.** `checked_artifact/platform.rs` is a flat
`PROTECTED_SOURCE_DIGESTS` entry and is under **no** tree pin (the six tree pins
are `bootstrap/runtime/catalog_lease.rs`, `capability/path.rs`,
`capability/pre_catalog.rs`, `catalog.rs`, `merge/v1_lifecycle/authority/observe.rs`,
`merge/v1_lifecycle/mod.rs` — none covers it). None of the other ten touched
files is under any pin. The checker's single finding confirms it:

```
checked-artifact boundary: failed
- protected source allowlist changed: checked_artifact/platform.rs
```

Digests recomputed and **all three match the report byte for byte**:

```
ceccf3d83bb6cb0ca0943212522011e81c0563a642587dbeb6893a1a04061a9e  platform.rs
a68289ac7cad2d87f432cf4eafd88f6b3d6d83fc0e0686fcddf7c24f000234d3  transition.rs
07b90f925e7ea3f980b3fbefd655097309f1a2c44d36c37f328e52c34930b59d  residue.rs
```

**Opinion — should `transition.rs` and `residue.rs` gain flat digest pins NOW?
YES.** Three reasons:

1. The plan's Step 4.1 text asks for it *at this step* ("digest-pin or split
   what remains of the files"). Riding it to 4.2 is precisely how a clause gets
   silently dropped at a phase settle — the failure mode the freeze's whole
   annotation machinery exists to prevent.
2. Both files are now at their sealed resting state for the remainder of R2-D:
   4.2 rewrites `platform.rs`'s Windows-anchor half and touches neither, so
   pinning now costs 4.2 nothing (it still moves exactly one pin).
3. The existing protection is a **token** rule, not a **shape** rule. The
   allowlist's zero-entry for these files stops a raw rename returning; it does
   nothing to protect the four `LeafPublicationSourceV1` expectations, which are
   where this step's correctness actually lives. A digest pin is what makes that
   a reviewed invariant rather than a test-only one.

The two digests above are verified in this review and can be pasted as-is.

**Companions genuinely untouched.** `CATALOG_PUBLICATION_CALL_COUNTS` byte-identical
and still 13; `capability_permit.rs:139`'s 13-site assertion untouched (the
`capability_permit.rs` diff is one additive hunk); `fault_expected_keys.rs`
absent from the dirty set entirely.

---

## 9. Cohesion — `platform.rs` at 1,093 lines

`wc -l`: `platform.rs` 1,093, `transition.rs` 461, `residue.rs` 587. The
ConsumerCheckpoint §11 1,000-line cohesion trigger is tripped on `platform.rs`,
but it was **already** tripped at 1,018 before this step — 4.1 deepened it by 75
lines, it did not cause it. The placement argument in the report is correct and I
verified its counterfactual: a new sibling module would have moved
`checked_artifact/mod.rs`'s flat pin *and* created a new unpinned file holding
raw rename tokens (which the allowlist would then have had to grow to admit) —
strictly worse.

**Opinion for the settle:** accept 1,093 for 4.1, and **bind the split to Step
4.2 by name** rather than leaving it to the phase settle to notice. 4.2 rewrites
`platform.rs:372-505` and the anchor internals at `:551-672`, which is the
natural seam (legacy Windows durability anchor out, the P1 pair plus the two
compositions staying). If 4.2 does not split, the trigger arrives unaddressed at
Step 5.1's explicit "LOC/cohesion audit + ChangeBudget row", where it becomes a
settled-gate finding instead of a step deliverable.

---

## 10. Gates — re-run in the live tree

All under `CARGO_TARGET_DIR=…/scratchpad/p41-target`, foreground.

| Gate | Result | Status |
| --- | --- | --- |
| `cargo check --all-targets -p gwz-core` | `Finished dev … in 9.11s`, zero diagnostics | ✓ |
| `cargo test --lib -p gwz-core checked_artifact::` | **375 passed; 0 failed**; 1170 filtered | ✓ matches |
| `cargo test --lib -p gwz-core leaf_publication` | **5 passed; 0 failed**; 1540 filtered | ✓ |
| `… the_legacy_leaf_edges_use_one_source_associated_publication_seam` | **1 passed; 0 failed**; 1544 filtered | ✓ |
| Full lib 1544/0 (asserted, not re-run) | **corroborated**: 1540+5 = 1544+1 = **1545** total, of which 1 ignored → 1544 passed / 1 ignored, exactly as reported | ✓ |
| `workspace_ops::merge` 565/0, `git::tests::g15` 46/0 | not re-run — no cause found; both are subsumed by the corroborated full-lib total, and no assertion anywhere in `src/` references the new or old edge messages (grepped `"publication source identity changed"`, `"publication source bytes changed"`, `"atomic no-replace publication"` — hits only in the two production files) | ✓ |
| `cargo clean -p gwz-core` → `CLIPPY_CONF_DIR="$PWD" cargo clippy -p gwz-core --all-targets --all-features -- -D warnings` | `Removed 644 files, 1.2GiB`; `Finished dev … in 12.42s`, **zero diagnostics** from a genuine clean | ✓ |
| `rustfmt --edition 2024 --check` over all ten touched Rust files | clean | ✓ |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | **one finding**, the expected `platform.rs` pin move; every other rule passes | ✓ |
| **L2-04** (`retained-readers.yml` `harness` job, run locally, python3.13) | `validated 7 migration rules and 7 runtime bindings`; predicates unit suite `Ran 14 tests … OK`; `Ran 86 tests in 172.721s … OK`; `{"status": "valid", "tuple_count": 24}`; `{"status": "manifest-ready", "tuple_count": 24}` | ✓ reproduced |

**Is the L2-04 matrix-job scoping honest? YES — and better than the report
states.** The `matrix` job does download released reader binaries over the
network (`retained_reader_matrix.py … --allow-network`, five platform runners),
so it is genuinely unrunnable locally, and it is unaffected by a step that
changes no written byte. But the brief's question — "confirm CI will run it on
push or flag that it needs a dispatch" — resolves in CI's favour: the workflow
fires on `pull_request` and on `push: branches: [main]` filtered by paths that
include **`scripts/checks/**`**, and this step modifies
`scripts/checks/check_checked_artifact_boundaries.py`. The `matrix` job
(`needs: harness`) will therefore run automatically on the PR and on the push.
**No `workflow_dispatch` is needed.** The report should say so rather than
leaving the reader to assume a manual trigger.

**LOC budget — recorded.** `git diff --numstat` gives 225 added / 18 deleted
across the ten tracked files; the new file is 282 lines. Gross **+507**, net
**+489** — arithmetic confirmed exactly. Net is inside the aspirational <500;
gross is 1.4% over. Recorded per the brief as [P3-6], not a trim request: the
282-line test file is the step's behaviour-unchanged evidence and trimming it
would be trading proof for a number.

---

## 11. Escalation record

Freeze §9 (:1345-1351) makes Phase 4 steps 4.1-4.3 **single-axis with automatic
escalation on any P0/P1/P2**. This review raises **no P0, P1 or P2 against the
object**. The six findings are all P3 and file-and-continue.

**No escalation trigger fires.**

Two items the lane owner should nonetheless route deliberately, neither being a
finding:

1. **Ratify the scope divergence.** The implementer departed from the launching
   brief's instruction. Against the *authorities* the departure is correct (§2),
   and it was declared up front rather than discovered here — but the ratification
   is the lane owner's to record, not the implementer's to assume.
2. **The P2-grade defect at §5 is a historical grade**, not a live finding. It
   belongs in the settle record as evidence that the conversion paid for itself,
   and its structural root cause — `authority_name` alone carrying no identity
   digest — is worth carrying to R2-E as a design note, since any future
   authority-record edge inherits the same non-self-checking name.

---

## 12. Verification appendix — commands executed by this review

All from `gwz-dev/gwz-core/`, foreground, on this macOS host, with
`CARGO_TARGET_DIR` pinned to the mandated scratchpad target.

```
git -C gwz-core status --porcelain          10 ' M' + 1 '??' = 11, base 3a45619
git -C gwz-core diff / --numstat / --stat   225 + / 18 −, new file 282 → 507/489
cargo check --all-targets -p gwz-core                          9.11s, 0 warnings
cargo test --lib -p gwz-core checked_artifact::                375 passed; 0 failed
cargo test --lib -p gwz-core leaf_publication                    5 passed; 0 failed
cargo test --lib -p gwz-core the_legacy_leaf_edges_…_seam         1 passed; 0 failed
   filtered-count arithmetic 1540+5 == 1544+1 == 1545  → full lib 1544/1 confirmed
cargo clean -p gwz-core                              Removed 644 files, 1.2GiB
CLIPPY_CONF_DIR=$PWD cargo clippy … -- -D warnings          12.42s, 0 diagnostics
rustfmt --edition 2024 --check  (10 files)                                  clean
python3.13 scripts/checks/check_checked_artifact_boundaries.py   1 finding only
python3.13 -c "import checker; recount RAW_RENAME tokens via mask_non_code"
                       platform 6/6/5 · publication 1/1/0 · transition 0 · residue 0
python3.13 -c "list PROTECTED_SOURCE_DIGESTS / _TREE_DIGESTS keys"
                       platform.rs flat-pinned, under no tree pin; 10 others unpinned
shasum -a 256 platform.rs transition.rs residue.rs      all three match the report
L2-04 harness job, five steps, python3.13         86 tests OK, tuple_count 24 valid
grep publication_callers scope in checker      provider/ only → no CATALOG entry owed
grep "pub(super) fn publish_verified_no_replace"   publication.rs:132 → not callable
grep recover_or_create outside tests             definition + re-export + comments only
grep write_checked / entry:: consumers      merge root, abort, preserve, gitbackend
wc -l platform.rs transition.rs residue.rs                      1093 / 461 / 587
sed .github/workflows/retained-readers.yml   scripts/checks/** in push+PR filters
```

No git write operation was performed — every git call was
`status`/`log`/`diff`/`numstat`. `cargo clean -p gwz-core` and every build
artefact touched only the mandated scratchpad target directory. The live tree's
eleven-file dirty set is byte-identical to what the implementer left. The only
file this review has written is this document.
