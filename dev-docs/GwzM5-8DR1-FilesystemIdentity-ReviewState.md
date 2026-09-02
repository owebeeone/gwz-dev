# GwzM5-8 DR-1 — filesystem identity design: STATE / GUARANTEE axis review

Date: 2026-09-03. Reviewer: peer-blind, State/Guarantee axis (the Code/mechanism
axis runs separately; kernel facts, inventory completeness and LOC are NOT
re-verified here). Object: `GwzM5-8DR1-FilesystemIdentity-Design.md` (DRAFT,
2026-09-03) at gwz-core main `ffd4f95` (v0.13.0). Method: reading only — the
design, the binding records, and the tree at the cites; no build, no suite, no
change. Time-box: 30 minutes wall clock; items not reached are marked
UNVERIFIED.

**VERDICT: GO-WITH-CONDITIONS** — adopt the design's DIRECTION as DR-1's answer
((a0) now; (b) as the relaxation; (c) as the program; (a) and (d) declined), but
(b) is NOT charterable as written: three P1s below sit on (b)'s own guarantee
statements (the migration mechanism, the instance id vs the shipped ownership
token, the admission set (b) actually opens). Sequencing (a0)+(b) first is
acceptable for the operator's ask only with the P1 folds and with §6 Q3/Q4 made
BLOCKING on (b), not deferred to (c); the operator should also be offered the
reorder in [P2-4] (protocol vehicle first), because (b)-then-(c) as drafted is
two catalog migrations and two protocol-allocation duals.

---

## 1. The guarantee ledger

Legend: KEPT / WEAKENED (to what; where the user learns it) / LOST. "Design
says" = whether the draft states the same; "silent" where it does not.

| # | Guarantee the checked boundary gives today (v0.13.0, ext4) | Mechanism today | Under (b) — nonce + handle, legacy-parity admission | Under (c) — tiers 3/2/1 | Design says |
|---|---|---|---|---|---|
| G1 | Exact classification after a **crash** (same boot) | handle+UUID hashed into the goal/anchor/authority names (`residue.rs:183-206`, `anchor.rs:316`, `authority.rs:196-202`) | **KEPT** — the persistent handle is unchanged by (b); the UUID contributed only to the hash | **KEPT at T3; KEPT at T2 in-session** (dev+ino agree within a boot); **WEAKENED at T1** to content equality with a typed refusal on unattended recovery (§4.3) | §4.3 says so for (c); §3.2 silent for (b) (implied by "no shipped guarantee is reduced") |
| G2 | Exact classification after a **clean exit and a later resume across reboot/remount** | as G1; the external UUID makes it remount-stable (§2 C10) | **KEPT** on ext4/xfs/f2fs/btrfs (handles are reboot-durable); **UNVERIFIED on zfs** (the design's own verification note); on tmpfs **VACUOUS** (the artifacts die with the boot — and (b) ADMITS tmpfs, see [P1-3]) | T3: KEPT. T2 out-of-session: **WEAKENED** to the typed `IdentityUnprovable` (§4.3) — the user learns it at the resume/abort door. T1 leaves: as G1 | (c): stated. (b): the zfs caveat lives only in the verification notes, not in §3.2's guarantee paragraph — **silent where it matters** |
| G3 | The **within-run interference guard** (a substituted equal-content object is caught, C6/C7) | invocation `dev`+`ino` (`linux.rs:130-133`) + the retained handle | **KEPT** (invocation half untouched) | **KEPT at T3/T2** (`session` is unconditional, §4.1 rule 1); **LOST at T1** — the design says so (C6 "a substituted equal-content object publishes undetected") and answers with "T1 gets an actionable typed refusal" (§5 item 6) — that is a refusal of the operation, not a preserved guard; correct, but the ledger should say LOST-at-T1-therefore-refuse | stated for (c) |
| G4 | The catalog is **bound to one filesystem instance** (a copy on another volume does not pass as the original) | `DurableCatalogTargetDigestV1` folds the encoded `LinuxExt4` identity incl. the external UUID for root/repo/common/parent (`digests.rs:110-134`) + `retained_parent_identity` equality (`catalog_bootstrap_record.rs:172-185`) | **WEAKENED**: the binding is now handle + nonce; the volume fact "corroborates". A byte-copy to another volume is caught by the HANDLE (fresh inodes) in every ordinary copy (`cp`/`rsync`/`tar`); the case only the UUID caught today is the same-handle cross-volume image (`dd`/block restore to a different volume) — under (b) that case is caught only if the corroboration mismatch REFUSES, which makes the volume fact an admission condition at re-binding, contradicting §3.2 item 2 "never an admission condition" — see [P1-2]. Where the user learns it: the design promises a volume-mismatch sentence at `entry.rs:405-426`, but the mismatch arrives as `Ambiguous` (`classifier.rs:309-311` → `:274`), whose door is the `CheckedFsError::Ambiguous` arm (`entry.rs:419-422`, `IoError`), not the `Unsupported` arm that carries a remedy | as (b), plus the tier and the evidence are in the bootstrap record (§4.5) | **contradictory** for (b) (see [P1-2]); (c) stated |
| G5 | The **rename-domain proof** (managed and private parents in one atomic rename domain, `residue.rs:89-101`) | `statx(STATX_MNT_ID)` (VFS-generic) | **KEPT** — (a0) removes only the gratuitous `require_ext4` in front of it (`linux.rs:70`); the proof itself is untouched | **KEPT** (§4.3: "needs no tier at all") | stated |
| G6 | The abort's **path-scoped capability freedom** (amendment §6; `capability.rs:57-63`) — an abort touching no checked artifact needs no filesystem; the re-verifying doors take the legacy probe, STRICTLY WEAKER than the catalog's | two probes (`identity.rs:312-367` vs `linux.rs:122-134`) | **KEPT in extension** (the same substrates pass); **the "strictly weaker" clause becomes FALSE by obsolescence** — catalog and abort admission coincide once §3.2 item 3 converges the two regimes; `capability.rs:57-63`'s "the string's 'ext4 only' is the CATALOG's admission list, not the abort's" and amendment §6 :397-403 need dated brackets — the design's §3.2 "Moves" says only "Docs + release notes as in (a)" | **WEAKENED at T2 post-reboot**: the re-verifying abort doors reach `IdentityUnprovable` and REFUSE — so "the straight line never refuses" is true of the PROBE and false of the OPERATION; §4.7's "the abort's scoped clause collapses into the general rule" over-claims — see [P2-1] | (b): silent; (c): over-claimed |
| G7 | Every downgrade is **recorded in the evidence** (design §5 item 3) | n/a today (one tier) | **NOT AVAILABLE in (b) as scoped**: (b) adds a nonce and a "recorded corroboration" but no tier field and no named wire enum for WHICH corroborator was recorded (UUID / `f_fsid` / absent) — see [P2-2] | provided by `IdentityEvidenceV1.tier` + the bootstrap/authority records (§4.2) | (b): silent on the wire field; (c) stated |

---

## 2. Findings

### P1

**[P1-1] The v0.13.0 ext4 catalog migration in (b) cannot work as described, and its crash-safety and one-way-ness are unstated.** §3.2 says `matches_attempt` "must accept a record whose identity is the old `LinuxExt4` kind when the handle type and handle bytes agree, ignoring the UUID field". But `matches_attempt` (`catalog_bootstrap_record.rs:172-185`) compares FIVE things, three of which bind the old form irreversibly: `support_profile` (the old `LinuxExt4FsIocGetFsUuidV1` variant, and `decode_canonical` at `:236` rejects a record whose `retained_parent_identity.support_profile()` disagrees with its `support_profile`), `durable_target_digest` (a SHA-256 over the encoded identities of root/repo/common/parent — `digests.rs:110-134` — in which the UUID is folded and cannot be "ignored"), and `retained_parent_identity` (an `Eq` on the whole enum value). The scratch name embeds the same digest (`scratch.rs:29-33`). So either (i) the migration re-probes `FS_IOC_GETFSUUID` on ext4 to RECONSTRUCT the old-form digest and identity for comparison — which keeps the UUID probe and the `LinuxExt4` kind alive as a live code path through the migration window, i.e. the "two regimes converge" claim (§3.2 item 3) is false until every catalog is re-issued; or (ii) the migration accepts on a handle subset and skips the digest and profile checks — which binds a v0.13.0 catalog by LESS than either regime during the window. The design chooses neither. Crash-safety: the re-issue "at the next publish" means an accepted-old record and a new-form record can coexist after a crash; `merge_record` (`classifier.rs:301-319`) requires every `Exact` record to be EQUAL (`expected == value`), so old+new → `false` → `Ambiguous` → refusal — the migration must be designed as an admitted action with its own fault rows, not "cheap". One-way: a v0.13.0 binary reading a re-issued record meets an unknown identity kind / profile code → `decode_identity` fails → `Other` → `Ambiguous` (UNVERIFIED that the closed decode fails rather than defaults; either way the older binary refuses), so the migration is one-way and the release notes must say so. **Fold:** rewrite §3.2's migration paragraph against the five-way `matches_attempt`; pick (i) or (ii) explicitly; state crash-safety via the classifier's equal-records rule; state one-way; make it its own step with corpus fixture (the design already says the last).

**[P1-2] The "gwz-minted instance id" already exists, and the design neither names it nor re-litigates its declared boundary; and the volume-fact corroboration is a gate in disguise.** The bootstrap record already carries a 32-byte cryptographically random `bootstrap_ownership_token` (`catalog_bootstrap_record.rs:19-38`, `:49`), persisted (`:284`) and folded into the scratch name (`scratch.rs:34-38`). Its adopted boundary is explicit: "That read-back is self-consistency … it is NOT an adoption or exclusion proof … A later step that uses a resident record to decide adoption of state this action did not create makes determinism load-bearing for exclusion, and must re-litigate the token then" (`bootstrap/managed/provider.rs:51-62`, carried from the Step-3.1 review §9). Binding the catalog's filesystem identity to a nonce is exactly "using a resident record to decide adoption". The design must say whether the instance id IS this token (then re-litigate its boundary, in the design) or a SECOND nonce beside it (then say why two, and which the scratch name is minted from — §4.5 says "the scratch filename is minted from the instance id", which it already partly is). Separately, §3.2's guarantee paragraph says a corroboration mismatch is "a typed refusal at the point of use" AND that the volume fact "is never an admission condition". Both cannot hold: a mismatch that refuses is a comparative admission condition at re-binding. Resolve: either the volume fact gates at RE-BINDING only (state it: "never gates ADMISSION of a fresh catalog; gates RE-BINDING of an existing one"), or it never gates and the same-handle cross-volume image is accepted (state the residual). See G4.

**[P1-3] (b) admits more than the design's ranking table says — including volatile and network substrates — and the design defers the only two questions that bound it (Q3, Q4) to (c).** §3.2 states the admission set as "ext4, xfs, btrfs, zfs, f2fs — everything with `export_operations`". "Everything with `export_operations`" includes tmpfs (`shmem_export_ops`) and NFS (`nfs_export_ops`) — UNVERIFIED here against kernel source (not read in this review; the Code axis or a one-command probe should confirm), but if true then (b) puts the CATALOG on tmpfs and NFS with no locality/lock capability in front of it, while the R0-L release gate lists tmpfs and overlay as required negative rows (`provider.py:40`, the design's own I20) and `OperationModel.md:118-121` calls network mounts unsupported for concurrent mutation. The design flags this hazard for (a) ("newly admits tmpfs … cannot land without an explicit answer to §6 Q4") and then recommends (b), which admits the same and more, with Q3 (locality) and Q4 (tmpfs) phrased as (c)-era questions and the locality split sequenced at (c) P4. **Fold:** state (b)'s admission set honestly in §3.2 and §3.5; make Q3 and Q4 blocking on (b); either ship the locality/lock capability (§4.3's `RuntimeAdvisoryLock` home) WITH (b) or record NFS/tmpfs admission as a dated residual with what the user sees. This is also where "no per-filesystem branches" bites: the answer must be a property capability, not a magic-number blacklist (§5 item 4) — the design already knows this; it must apply it to (b).

### P2

**[P2-1] §4.7 over-claims for (c): "the straight line never refuses at all" is true of the probe, false of the operation.** At T2 after a reboot the re-verifying abort doors (amendment §6's three paths) reach `IdentityUnprovable` and refuse; a `--resume` at T2 post-reboot refuses; a T1 leaf recovery refuses by design (§5 item 6). The abort's scoped clause does not "collapse into the general rule"; it becomes TIERED: "aborts that must re-verify checked artifacts recover exactly at tier 3, or at tier 2 within the minting session; otherwise they refuse with `IdentityUnprovable`, and the merge is cleared by completing it or by …". Write the sentence in that form for §4.7, for `capability.rs:57-63`, `OperationModel.md:150-161`, and the release notes. Also G6's (b) consequence: amendment §6 :397-403 and `capability.rs:60-62` ("STRICTLY WEAKER") need a dated bracket the moment (b) converges the regimes; §3.2 "Moves" must name them.

**[P2-2] (b) has no wire home for "which corroborator was recorded", so a reader cannot tell a UUID-corroborated catalog from an `f_fsid`-corroborated one from an uncorroborated one.** §3.2 item 2 says the volume fact is "recorded alongside as corroboration" but the protocol list (`:319-326`) names no field or enum for it. Without a typed `VolumeFact { Uuid | FsId | Absent }` in the bootstrap record, the reader classifies at the tier it wishes — the exact failure §5 item 3 forbids. Name the field; make the decoder reject an inconsistent pair (the I8 value-contract pattern, `durable_identity.rs:34-85`).

**[P2-3] The (c) naming change omits the authority record's own name; a tier change then makes the record foreign, which is the §4.4 rule-2 hazard the design warns against.** §4.2 lists goal/staged goal and anchor as nonce-named and says the authority record "carries the minting tier" — but the authority FILENAME is `ca1-{family}-{action}.authority` with `family_key = SHA256(root_identity.durable.encode() …)` (`authority.rs:196-202`, `:226-228`), and `matches_request` compares `artifact_root_identity == artifact.root_identity.durable` (`authority.rs:107-119`). At a T3→T2 transition the root's durable encoding changes, the family key changes, the record is unfindable, and the residue reads foreign. C2 in the design's own table names this ("the authority file's name stops resolving"). Add the authority name and `family_key` to §4.2's naming list; define `matches_request` over the nonce, not the encoded identity.

**[P2-4] Sequencing cost is unstated: (b) then (c) is two catalog migrations and two protocol-allocation duals.** (b) mints `LinuxPersistentHandle` + a profile variant + a nonce field; (c) P1 then replaces the closed enum with `IdentityEvidenceV1` and (c) P2 adds tier/evidence to the bootstrap record and re-names goal/anchor/authority. Every (b)-form catalog then migrates AGAIN (§4.5 "As (b) §3.2, plus …" hides this). Offer the operator a third answer to Q2: land (c) P1 FIRST as the protocol vehicle (T3 only, no behaviour change, ~450 LOC), then (b)'s admission change and nonce ride that allocation — one allocation, one migration, one dual, Fedora unblocked one step later. If the operator wants Fedora before that, (b) must state the second migration as its cost.

**[P2-5] The wrong frozen clause is named for the protocol allocation.** §3.2 cites `GwzM5-8I2CompatibilityContract.md:15-21` and its "named-interface mechanism at :7-13". That contract's "protocol allocation" is the merge-operation ENVELOPE registry (`:23-35`, `gwz.merge-operation/v0..v4`); its `:7-13` is the R4b amendment note, not a mechanism. The checked-artifact CBOR protocol (`checked_artifact.taut.py`, `protocol/generated.rs`) is frozen by `GwzM5-8R2DInterfaceFreeze.md` §6 "Wire and records rule — zero new durable records … Any discovered need for a new or changed durable record, slot, purpose, or phase stops the step for a Track-W freeze plus dual review before any implementation" (`:1711-1722`), inherited by the settled tuple (`GwzM5-8R2DSettledTuple.md` §3 census, `:114`; §4.5 sixth-key rule `:286`). Name that clause; the amendment is a Track-W freeze, and the 165-key census is untouched by (b) only if the migration ([P1-1]) is built without fault rows — which [P1-1] argues it should not be.

**[P2-6] The ledger of §3.2 is incomplete for (b): it names one lost guarantee and says "no shipped guarantee is reduced" for the rest.** Per §1 above, (b) keeps G1/G3/G5, keeps G2 with a zfs UNVERIFIED and a tmpfs-vacuous caveat, weakens G4, obsoletes G6's "strictly weaker" clause, and has no G7 home. Put the table in the design.

### P3

**[P3-1]** `IdentityEvidenceV1.tier` is a stored field beside the facts it summarises (§4.1). Make the tier DERIVED from the present facts (or reject an inconsistent pair at decode) so no reader path can hold `persistent: Some` with `tier: 2`.

**[P3-2]** The test-only provider substitution at `HostPlatform` (§4.6) is the right call over fault keys (the census stays at 165; agreed). It needs the house's absence pin — how a substituted provider is kept out of production builds (`cfg(test)` seam vs trait object) and pinned absent, in the `catalog_activation_pin.rs` idiom. Say so. The coverage matrix as stated (every §2 consumer × {T3, T2 in, T2 out, T1 leaf}) does exercise every §4.3 arm; the two hard-refusal sites (§2.2) need their own rows, which §4.6 does not list.

**[P3-3]** §6 questions: Q1–Q8 are one-line answerable. Add Q9 "is the instance id the shipped `bootstrap_ownership_token`, or a second nonce?" ([P1-2]); re-phrase Q3/Q4 as blocking on (b) ([P1-3]); Q2 gains the [P2-4] option. Q5 (patch vs minor) is answered by the freeze §6 and the one-way migration: minor.

**[P3-4]** The two doc defects found in passing (`OperationModel.md:141-143`; `linux.rs:351` "pre-6.8" → 6.9) are real at the cites (the first re-verified by reading `:139-143` at `ffd4f95`; the second UNVERIFIED here, kernel not read). Fix at (a0) as proposed.

---

## 3. The operator's four constraints

| Constraint | Honoured? | Cite |
|---|---|---|
| FAT32 out of product and lab | YES — §5 item 8, §4.6 (tier injection on ext4), no FAT32 row anywhere | design §4.6, §5.8; ruling `:19` |
| No FAT32 volume drive | YES | as above |
| "Do not weaken the ordinary path" | YES by construction — no capability-free operation moves onto the probe (§5 item 1); the inventory's shrinkage arm does not fire (§3.2 "Not amended"). One watch item: §3.2 item 3 converges `identity.rs` onto the new provider — that is a code path the ABORT's checked doors ride on the plain lease; RR §4's enumeration must be in the implementing charter (the design says the clause binds — correct) | amendment §3 :207-210; RR §4 :202-217 |
| No per-filesystem branches | (b): YES (no fs-type test). But (b) then needs the locality answer WITHOUT a blacklist — [P1-3]. (c): YES (§4.3, §5 item 4) | design §5.4 |

## 4. The abort sentence and E4-Close §2's residual grounds

- **Amendment §6 / `capability.rs:57-63` / `OperationModel.md:150-161`:** under (b) the sentence's operative half ("need persistent file handles and a mount identity") stays TRUE; the "STRICTLY WEAKER filesystem than the merge" clause and "the string's 'ext4 only' is the CATALOG's admission list" become false by obsolescence → dated brackets, and the remedy string's "ext4 only" clause is rewritten (pinned by `interface_tests/contracts.rs:150-165`). Under (c) the sentence becomes TIERED ([P2-1]); the design's "collapses into the general rule" is withdrawn.
- **E4-Close §2's four residual grounds** (record-root detach window; observation-dead window `:48/:51`; directional residue `:45`; the twenty carved writers): **none changes under (b) or (c)** — all are windows in time or the standing list, not identity strength. The design says exactly this (§4.4) and is right. Its two second-order notes (population; threading of the tier through `matches_request`) are real and correctly stated — [P2-3] is the concrete instance of the second.

## 5. Record of the round — what must be amended, and by whom, before any build

| Record | Amendment | Owner |
|---|---|---|
| `GwzM5-8R2DInterfaceFreeze.md` §6 (wire rule) + `GwzM5-8R2DSettledTuple.md` §3/§4.5 | Track-W freeze for the new identity kind / profile / nonce (and, if [P1-1] takes fault rows, the census) — dual | lane owner (design round proposes; dual adopts) |
| `GwzM5-8I2CompatibilityContract.md` | NOT the home ([P2-5]); no edit unless the envelope registry moves (it does not) | — |
| E0.2 §5.2 capability-free list (`SemanticsAmendment-DRAFT.md:909-917`) | NOT amended (correct). The `:917` reading bracket gains one dated sentence: the catalog's Linux admission now equals the legacy probe's | lane owner |
| `GwzM5-8R2E-CapabilityFreeAmendment.md` §6 :397-403 | dated bracket: "strictly weaker" obsolete at (b) | lane owner |
| `GwzM5-8R2E-RecordRootAmendment.md` §4 (PLAIN-LEASE PROBE CLAUSE) | no text change; each implementing charter carries the enumeration (the design says so) | implementing charters |
| `bootstrap/managed/provider.rs:51-62` (ownership-token boundary) | re-litigated in the design ([P1-2]) | design author |
| `capability.rs:57-63` remedy + `interface_tests/contracts.rs:150-165` pins; `OperationModel.md:139-166`; `GwzReleaseNotes` next version | rewritten per §4 above; the release notes carry the one-way migration and the honest admission set | implementing step |
| `scripts/linux_identity_probe/provider.py` (R0-L release gate) | rewritten — release lane sign-off (Q8) | release lane |
| `check_checked_artifact_boundaries.py` digests `:63`, `:263`, `:265`, `:266`; protocol regen/corpus/D0/`PRE_LOG_WIRE_SHA256` | moved by the implementing steps | implementing steps |
| `GwzM5-8DR1-Charter.md` | the design's own charter must carry the lease-reachability enumeration and the record-root analysis before (b)'s build (charter §4 item 6) | lane owner |

## 6. UNVERIFIED (time-box)

- `name_to_handle_at` / `export_operations` on tmpfs, NFS, overlayfs (default), zfs — from kernel knowledge, not read at the cites in this review; the [P1-3] conclusion holds if any one of tmpfs/NFS answers.
- Whether a v0.13.0 binary's `decode_identity` on an unknown kind fails closed (assumed; [P1-1]).
- `linux.rs:351`'s kernel-version comment; all LOC figures; the design's kernel cites (Code axis).
- `catalog_lease/target.rs` lives at `bootstrap/runtime/catalog_lease/target.rs`, not `capability/pre_catalog/catalog_lease/` — the design's short cite resolves; content at `:138-158` and `:443-461` verified as described.

---

**Verdict, one line:** GO-WITH-CONDITIONS — adopt the direction and (a0); (b) not charterable until [P1-1]–[P1-3] fold and Q3/Q4 are answered as blocking; sequencing (a0)+(b)→(c) acceptable only with [P2-4]'s cost stated or its reorder taken.

---

## Round 2 — confirmation of the folds (2026-09-03, State/Guarantee axis)

Object: REVISION 2 of `GwzM5-8DR1-FilesystemIdentity-Design.md` (944 lines,
"REVISION 2, 2026-09-03, folds the dual"). Method: text diff of each round-1
finding against the revision's fold-marked text; no new tree reads (no folded
claim contradicted a cite verified in round 1). Time-box: 15 minutes.

| Finding | Where folded | Status |
|---|---|---|
| [P1-1] migration cannot ignore a UUID folded into a SHA-256; crash-safety; one-way | §3.2.2 "DUAL-TUPLE ACCEPTANCE": the five-way `matches_attempt` conjunction quoted; the digest's framed tuple and the scratch-name blast radius stated; option (i) of my report chosen explicitly (recompute the legacy tuple where `FS_IOC_GETFSUUID` answers; legacy scratch name regenerated); the "two regimes converge" claim qualified (new catalogs from day one; existing ext4 only after re-issue); re-issue as a publish-then-retire ADMITTED ACTION under the catalog lease with fault rows (§3.2.6) and the three crash rows named, the both-resident row routed to the migration arm and not `merge_record`'s equality rule; one-way-ness stated with the release-notes obligation and the fail-closed decode kept UNVERIFIED pending a corpus row; six test rows; own step | **FOLDED AS STATED** |
| [P1-2] the shipped `bootstrap_ownership_token`; its boundary; corroboration "gates" vs "never gates" | §3.2.3: the instance id is a SECOND nonce; the token's boundary (`provider.rs:51-62`) STANDS, not re-litigated (also §5 item 11, §6 Q9); role/boundary/lifetime/where table; round-1 "scratch name minted from the instance id" withdrawn; the contradiction resolved as "never gates ADMISSION of a fresh catalog; gates RE-BINDING of an existing one", refusing through the `Unsupported` arm with a remedy (not `Ambiguous`/`IoError`); the same-handle cross-volume image named as the release-notes residual; G4 row matches | **FOLDED AS STATED** — the "second nonce" branch of my either/or was taken, which is the branch that needs no re-litigation; acceptable. One-line note for the implementing charter: the re-binding rule speaks of "both present and disagree" and of "either side absent"; the class-differs case (record `Uuid`, live `FsId`, or the reverse) should be assigned explicitly — evaluate by class-fold where derivable, else treat as the lower class. Not a condition |
| [P1-3] (b) admits tmpfs/NFS; Q3/Q4 deferred to (c) | §0 point 4; §3.2.4 restated admission set (both `name_to_handle_at` AND `FS_IOC_GETFLAGS`; tmpfs admitted; NFS/FUSE refused by accident with a remedy-less sentence); §6 Q3/Q4 marked "BLOCKING ON (b)"; §7 "Not charterable until Q3 and Q4 are answered"; the locality/volatility capability ships WITH (b) in the `RuntimeAdvisoryLock` class as property attempts, never a magic-number list (§5 item 4); §3.5 table implied by §3.2.4 | **FOLDED AS STATED** (and sharpened by the Code axis's `parent_mode` finding, which I did not verify and do not need to — it only narrows (b)'s set further) |
| [P2-1] "never refuses" over-claim; tiered abort sentence; amendment §6 / `capability.rs` brackets | §4.7 "TIERED, not collapsed": probe vs operation distinguished; the tiered sentence written for `capability.rs:53-65`, `OperationModel.md:150-161`, release notes; §3.2.7 table row "The abort clause's dated brackets" naming `capability.rs:59-62` and amendment §6 `:397-403`; G6 row | **FOLDED AS STATED** |
| [P2-2] no wire home for which corroborator was recorded | §3.2.3 `VolumeFactV1 = Uuid(bytes[16]) \| FsId(bytes[8]) \| Absent`, decoder rejects inconsistent pairs in the I8 pattern; §3.2.7 allocation row; G7 row | **FOLDED AS STATED** |
| [P2-3] the authority record's own name / `family_key` omitted from the naming rebase | §4.2 table: `family_key`, `family_prefix`/`authority_name`, `matches_request` (over the nonce; `artifact_root_identity` no longer a comparison key), goal/source, anchor; §4.4 rule 2 restated at the prefix filter `residue.rs:127` (enumerable, then degrade, never foreign, never invisible); §4.6 row for enumerability across a forced T3→T2; §4.8 P2a | **FOLDED AS STATED** — folded at a sharper site than I cited (the Code axis's `residue.rs:127`), which subsumes mine; acceptable |
| [P2-4] the second migration / second dual unstated; three-way Q2 | §0 sequencing note; §4.5 "the second migration is real and is (c)'s cost if (b) ships first"; §6 Q2 three-way with the trade; §7 lane recommendation (ii) with (i)'s cost to be written into the charter if taken | **FOLDED AS STATED** |
| [P2-5] wrong frozen clause | §3.2.7: `GwzM5-8R2DInterfaceFreeze.md` §6 Track-W (`:1711-1722`) quoted; settled tuple §3/§4.5 inherited; the I2 CompatibilityContract cite withdrawn with the envelope-registry reason; §3.2.6 the census expected to move, one Track-W dual covering wire + census | **FOLDED AS STATED** |
| [P2-6] the G1–G7 ledger | §3.2.5 adopted "verbatim in substance"; checked row by row against my §1 — G1–G7 match, including the zfs UNVERIFIED and tmpfs-vacuous caveats on G2 and "LOST at T1 — therefore refuse" on G3 | **FOLDED AS STATED** |
| [P3-1] tier derived, not stored | §4.1 rule 4 (`tier()` from facts; decoder rejects a contradicting summary); §5 item 10; §3.2.3 general rule | **FOLDED AS STATED** |
| [P3-2] substitution seam absence pin; hard-refusal rows | §4.6: `cfg(test)`-gated, pinned absent in the `catalog_activation_pin.rs:81-84` idiom; rows for the anchor at each tier and the rename-domain gate's tier-independence; plus the enumerability row | **FOLDED AS STATED** |
| [P3-3] Q3/Q4 blocking; Q9 | §6 Q3/Q4 "BLOCKING ON (b)"; Q9 the instance id; Q2 three-way; Q5 answered "minor — confirm?" | **FOLDED AS STATED** |
| [P3-4] the two passing doc/comment defects at (a0) | §1.5 (now three defects, the third from the Code axis); §3.0, §4.8 P0, §7 item 1 | **FOLDED AS STATED** |

Operator constraints re-checked on the revision: FAT32 out (§4.6, §5 item 8);
no FAT32 drive; ordinary path untouched (§5 items 1-2; §3.2.7 "Capability-free
list — NOT amended"; RR §4 enumeration per implementing charter); no
per-filesystem branches (§3.2.4's capability is by property attempt; §5 item 4).
All four honoured.

NOT FOLDED items: none.

**CONFIRMED (adopt)** — every State-axis condition is folded as stated or at a
sharper site; the verdict stands as GO-WITH-CONDITIONS with the conditions now
discharged in the text. What remains is the operator's: Q1, Q2 (the three-way),
Q3/Q4 (blocking on (b)), Q5–Q9.
