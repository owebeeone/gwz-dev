# R2-C2 owner interface state/filesystem re-review (round 3, focused)

Date: 2026-08-15

Reviewer designation: State-3

Filing note: this is the State-axis round-3 report on the R2-C2 publication-seam remediation; in `dev-docs/CurrentProgramCheckpoint.md` item 3 this same event is called the "round-4 focused dual re-review" (program-round numbering counts the Code axis's extra round). Per the review independence rule (L1-18) I did not read the paired current-round report, and I also did not open `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewCode-3.md`; the paired finding P2-1 and the Code-3 P3 list were taken from the review brief and from `dev-docs/CurrentProgramCheckpoint.md`, and every closure claim below is verified from source and executed evidence, not from the peer report.

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 1 |

Per `dev-docs/AgentProcessRules.md` L1-19 a gate is NO-GO while any P0/P1/P2 is open. This review finds none. The one new finding is a bounded robustness gap in the new boundary-checker inventory (an adversarial evasion shape), with the production seam itself verified correct; it routes to the checker-hardening/R2-D lane rather than a new remediation round.

## Exact reviewed tuple and worktree state

| Repository | Verified HEAD | Status |
| --- | --- | --- |
| gwz-dev (root) | `f7ba3235703b11eb1c9a6a72e2179afa2b7b5837` | clean |
| gwz-core | `0d8382e1cbf6500e499bf2dc5ed23d6256f6e642` | clean |
| gwz-cli | `3cca145c0b32410f250f640730ed7ca18f1da59f` | clean |
| gwz-py | `929efb0d440206772155cadda89748e9431f9ca8` | clean |
| taut | `f00841966663bb0d500974ede446d30e9bfeeac4` | clean |

All five taken mechanically from `git rev-parse HEAD` with empty `git status --porcelain`, identically at review start and final recheck; they match the requested tuple exactly. No source, design, plan, lock, or marker file was modified. The only writes were `target/` build artifacts, self-cleaning test temp dirs, and a checker evasion probe built entirely in the session scratchpad on a copied tree.

Reviewed delta: gwz-core `c436180..0d8382e` — two commits (`b923109` "Seal directory-interior publication window", `0d8382e` "Refresh platform source pin"), 8 files, +271/−26, read in full: `provider/publication.rs`, `provider/directory_mutation.rs` (+ tests), `provider/mutation.rs`, `provider/interior.rs` (one-line visibility), `platform.rs`, `scripts/checks/check_checked_artifact_boundaries.py` (+ tests). Root delta `d2b4c772..f7ba3235`: checkpoint update, the §4.1 erratum in `dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md`, the two filed round-3 reports (peer report not read), lock/journal. `fault_v1.rs`, `interface_tests/`, the classifier, the aggregate, the owner loop, and the lease set are byte-unchanged in this range; my round-2 verification of them carries.

## Prior/paired-finding closure

**[Paired Code-3 P2-1 — foreign interior drift injected between the caller's interior re-check and the primitive's acquisition was published into the reserved final destination, wedging the catalog] — CLOSED on this tuple, verified independently from source and execution.** The window is now closed inside the sealed primitive itself. For the staging→final edge, `publish_verified_no_replace` (`publication.rs:130-153`) opens a second no-follow `Dir` capability from the retained parent (`:134`), proves its encoded identity equal to the fresh-aggregate staging identity (`:137` — hence equal to the rename-source handle proved at `:98`, i.e. same object), re-observes the complete bounded interior through that capability (`:143`), and requires `interior::staging_plan(..) == Complete` (`:144-152`) — all after source-capability acquisition (`:91-97`) and immediately before the rename (`:172-180`). For the active→retired edge, `DestinationRecheckV1::PreRetirementFinal` (`:155-170`) re-observes the destination interior through the retained, caller-identity-proved final handle and requires `completed_record` present plus retired row absent. Drift at the exact P2-1 injection point (the new post-caller-recheck hooks, `directory_mutation.rs:238` and `:333`) is therefore observed by the primitive before publication. Executed: both inverted-probe regression tests pass, both fault matrices converge (below), and the base §4.1 sentence Code-3 falsified ("Drift present before source-capability acquisition, including … directory interior, destination … still rejects before publication") is now true by construction, with the residual correctly narrowed to post-acquisition (analysis in the next section).

**Code-3 P3 corrections, as visible in the delta and checkpoint:**

- **P3-1 (checker blind to raw renames outside two files).** Implemented: `RAW_RENAME_CALL_ALLOWLIST` plus a subsystem-wide masked-source scan of every production `checked_artifact` file for call-shaped `open_rename_source`/`rename_open_source`/`rename_relative` tokens, exact per-file counts (publication.rs 1+1; platform.rs 2+2+5, verified against the actual call sites including the in-file Windows test module; residue.rs 2; transition.rs 2), and two adversarial unit tests (provider probe, legacy-interior probe) — all 63 checker unit tests pass here. My new [P3-1] below records one bounded evasion shape the counter does not catch.
- **P3-2 (platform.rs comment overclaim).** Closed: `platform.rs:121-130` now states that the destination handle does NOT prevent same-user renames of the destination directory or ancestors (`FILE_SHARE_DELETE`), assigns the residual to the amendment boundary, and cites the post-publish verification and the R2-F native window test. This is exactly the correction my round-2 [P3-2] required.
- **P3-3 (digest discipline).** Adopted, with one lineage blemish: `b923109` refreshed the `capability/pre_catalog.rs` tree digest same-commit but missed the `platform.rs` flat digest, so the intermediate commit is checker-red; `0d8382e` is the one-line refresh. The reviewed tip is green; the checkpoint's "same-commit digest refresh" sentence describes the adopted rule, executed here in two commits for platform.rs. Observation only; the settled tuple is what this gate reviews.
- **P3-4 (allocation parity).** Closed: the primitive's byte-verification buffer now uses `try_reserve_exact(len + 1)` with a typed `unsupported` rejection (`publication.rs:108-116`), matching the established provider pattern (`interior.rs:381`, `directory_mutation.rs:678`); the reservation exactly covers the `take(len + 1)` read, so no hidden growth path exists in the changed code. Allocation failure rejects read-only before the rename.
- **P3-5 (stale tuple table).** The checkpoint now pins "this coordinated commit" rows to `gwz.conf/gwz.lock.yml` in the same commit, with literal restatement deferred to the next docs-only commit.

**[State-2 P3-1 — Git-parent AlreadyExists arm and adopted-parent recovery never re-establish the Git-directory dirent barrier] — still open, correctly tracked, not silently affected.** `create_git_private_parent` (`mutation.rs:66-95`) is byte-identical in the changed range except the file's import line and the `DestinationRecheckV1::None` argument at `publish_active_record`; the `AlreadyExists => Ok(())` arm (`:89`) still skips `finish_private_parent_edge`. The checkpoint records the routing ("the Git-parent dirent-barrier gap to the next matrix package/§6 errata"); no §6 erratum was filed in this delta (the amendment's 20 added lines are all §4.1), so the finding remains open as routed, and my round-2 analysis (bounded, all-or-nothing at the `gwz` dirent, no misclassification, no production consumer of `Complete`) still holds.

**[State-2 P3-2 — Windows destination binding path-witnessed; comment overclaimed] — the tracking and documentation corrections demanded are now in place; the residual itself remains, by design, an R2-F obligation.** All three parts of my required correction landed: the comment fix (above); the controlling-document record — the §4.1 erratum explicitly describes the absolute-path derivation, names the same-user rename of the destination directory **or a path ancestor** inside the syscall window (the exact ancestor route my round-2 finding said the base text did not literally cover) and assigns it to the cooperating-same-user boundary; and the native Windows destination-window test is committed in the amendment's own text to the R2-F platform gate ("A native Windows destination-window test executes at the R2-F platform gate", reinforced at amendment line 667). State-model accuracy of the erratum is verified in the next section.

## Findings (new only)

### [P3-1] An alias-bound reference to a raw rename primitive evades the new subsystem-wide caller inventory

- **Where:** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py`, the new scan in `check()` (regex `\b<token>\s*\(` over masked production sources, with a `"fn "`-prefix exclusion), against `RAW_RENAME_CALL_ALLOWLIST`.
- **Violated invariant:** L2-06 — structural boundary gates must survive alias/re-export/callback probes, "never rely only on a denylist regex." The new gate's stated purpose (checkpoint: "subsystem-wide raw-rename caller inventory … Any other caller anywhere in the subsystem … fails closed here") is delivered only for call-shaped spellings.
- **Reproduction (executed on a scratchpad copy of this exact tree):** appending to `checked_artifact/cleanup.rs` a function that binds the primitive first — `let publish = crate::checked_artifact::platform::rename_relative; publish(dir, …)` — passes the checker: `checked-artifact boundary: ok`, exit 0 (baseline copy also 0). The direct-call form of the same probe in the same file fails the checker (proven by the suite's own `test_raw_rename_caller_in_legacy_interior_is_rejected`, green here). `use … rename_relative as r;` aliasing evades identically. The compiler does not backstop this: `rename_relative` is `pub(super)` in `checked_artifact/platform.rs`, visible to every descendant of `checked_artifact`.
- **Impact:** bounded. I enumerated the exposed surface mechanically: 55 production `checked_artifact` files sit outside both digest maps (`cleanup.rs`, `transition.rs`, `residue.rs`, `leaf.rs`, `namespace/*`, `protocol/*`, …) where an aliased caller would be caught by neither the counter, the digests, nor visibility. Inside the digest-pinned trees (the whole `capability/pre_catalog` provider, `catalog.rs` tree, `platform.rs` itself) any byte change still forces a deliberate digest update, and ordinary regressions — direct calls, the realistic accidental shape — are caught everywhere, including exact-count drift within the four allowlisted legacy sites. No production defect exists on this tuple (checker green, allowlist counts verified by hand against source). This is gate robustness, not seam correctness.
- **Required correction:** extend the scan to bare-token references (count `\b<token>\b` excluding the defining `fn` lines and the allowlist), or digest-pin the remaining legacy interior files until R2-D retires their allowlisted call sites; either lands in the checker-hardening/R2-D lane.
- **Regression test:** alias-binding and `use … as` variants of the two existing adversarial checker unit tests (provider tree and legacy interior), asserting checker failure.

## State-machine verification of the changed seam

Reconstructed from source, independently of the tests; everything outside the changed range carries from my round-2 verification of the identical bytes.

**Staging→final, full sequence** (`directory_mutation.rs:174-276` → `publication.rs`): fresh-aggregate preconditions (final absent, staging present and `Complete`) → caller opens staging identity-proved against the aggregate → staging fsync → `StagingFlush` fault key → `FinalPublishBeforeRename` hook → named re-verify → caller's own fresh interior observe must be `Complete` → `FinalPublishAfterInteriorRecheck` hook (the P2-1 injection point) → primitive: acquire rename-source handle (no-follow, maybe-dir) → handle identity vs aggregate → **second no-follow Dir capability, identity vs the same aggregate bytes, fresh bounded interior observe, `staging_plan == Complete`** → `renameat NOREPLACE` (Unix) / `SetFileInformationByHandle` on the exact handle (Windows) → `FinalPublish` key → reopen final, identity vs staging identity, parent fsync.

**Active→retired, full sequence** (`directory_mutation.rs:278-362`): fresh-aggregate preconditions (final present with `completed_record` and retired absent; active present with exact canonical bytes) → caller opens final identity-proved (the retained final handle) → `ActiveRetireBeforeRename` hook → caller's fresh interior observe through that handle (completed present, retired absent) → `ActiveRetireAfterInteriorRecheck` hook → primitive: acquire active source handle, identity + exactly-bounded byte compare → **destination interior re-observed through the same retained final handle: `completed_record` present and retired row absent** → rename into `final/…retired` (destination anchored to the final handle on Unix; path-witnessed on Windows) → `ActiveRetire` key → post-publish `verify_named_file` through the retained final handle → final fsync, parent fsync.

**Window algebra — the re-check closes both edges of the reported window.** Let T_acq be source-capability acquisition, T_obs the primitive's interior observation, T_ren the rename. The code order forces T_acq < T_obs < T_ren with only pure checks between. Interior drift at any t < T_obs — which strictly contains everything up to T_acq, i.e. the entire P2-1 window between the caller's re-check and acquisition — is observed and rejects as typed ambiguity before publication. The remaining residue is (T_obs, T_ren): genuinely post-acquisition. That is exactly where amendment §4.1's base text anchors the same-user boundary ("a non-cooperating process owned by the same OS user must not mutate GWZ's private catalog namespace **after the primitive has acquired its source capability**"), so the erratum's claim — pre-acquisition interior drift rejects; only post-acquisition drift falls to the boundary — is accurate, and in fact conservatively understated (rejection extends past acquisition to T_obs).

**Same-object proof of the second capability.** Both the rename-source handle and the second Dir capability are proved equal to the same fresh-aggregate identity bytes. `encode_identity` is one encoding (length-prefixed canonical durable identity + dev/ino invocation identity, `retained.rs:417-427`), and `dir_identity`/`file_identity` funnel into a single `identity(fd, metadata)` per platform (verified in `platform/macos.rs`: volume UUID + `ATTR_CMN_OBJPERMANENTID` + dev/ino; Linux analogous per the round-2-verified provider). Two live handles carrying the same durable+invocation identity are the same object; the interior observation therefore binds to the object the Windows rename moves, and to the object the Unix rename moves provided the name still binds it at T_ren — which is the pre-existing, explicitly documented §4.1 non-atomicity (the erratum makes no compare-and-rename claim, and `rename_open_source` on Unix is transparently `rename_relative` by name, `platform.rs:79-96`). Kind swaps and symlinks at either open reject (no-follow, `open_dir_nofollow`, identity mismatch). On Windows the second capability is dropped at match-arm end, before the rename, so no sharing-mode interaction exists.

**Re-check grammar cannot drift from the classifier's.** The primitive calls the very functions the production aggregate classifies with (`interior::observe`, `staging_plan`, `completed_record`, `row`) — not copies. `observe` is read-only and bounded (10-slot cap, name budget, closed slot grammar with noncanonical-alias rejection, bounded `try_reserve_exact` reads); a foreign name rejects at `observe` itself ("catalog directory contains an unowned child"), content/kind/prefix drift rejects at `Complete`/`completed_record`, and the descriptor/format bytes durably pin the directory's and members' durable identities (`owner_issue_for_catalog`), so byte-identical substitution also rejects. Both rejection routes are typed ambiguity, pre-rename, mutation-free.

**No false ambiguity under legitimate concurrent recovery.** The destination re-check (completed present, retired absent) can only newly fail on genuine in-window interior change. Cooperating GWZ processes are excluded by the catalog lease, which is held across the edge with witness revalidation before permit issuance and before every physical edge (amendment §4, unchanged code, round-2-verified); the edge itself is single-threaded straight-line code; and the retired row is created by nothing but this rename — a previous crashed run that completed retirement yields a fresh-aggregate `Complete` classification and this edge is never entered. A transient IO failure inside the re-check rejects read-only and the next preflight converges (`CheckedFsError::ambiguous` is an error return, not a durable ambiguity row). So under the stated boundary the re-check is exact: it fires only on out-of-boundary drift, and then read-only.

**Rejection paths are strictly read-only.** Every failure exit of the primitive precedes `rename_open_source`; the operations executed before it are opens (no-follow), fstat-class identity queries, directory enumeration, bounded reads, pure grammar evaluation, and one fallible reservation. Executed confirmation: both inverted-probe tests assert the foreign fact byte-preserved and no publication (final absent; retired absent with active still in place).

**Coverage completeness of the two re-checks.** The two guarded edges are exactly the two whose rename launders an interior across the durable grammar boundary (staging interior becomes the reserved final; active enters the final's interior). The other four seam edges move byte-pinned regular files into destinations protected by NOREPLACE, where in-window destination drift either fails EEXIST or lands in staging — a directory whose foreign-drift wedge is identical with or without the anchor move and is already executed as read-only ambiguity (`unowned_staging_and_final_directories_are_read_only_ambiguity`). No edge is missing a needed re-check.

**Matrix integrity and crash-window invariance.** The re-checks add no durable mutation and no namespace edge; the `fault_v1` interruption-key set is untouched (`fault_v1.rs` and `interface_tests/` empty in the range; `EXPECTED_KEY_COUNT` still 163; `catalog_bootstrap.*` family still 23); the classifier and decision table are untouched. The two new hook points are `cfg(test)` single-shot callback slots between existing keys, not interruption keys, so every interruption key still observes the identical durable state it did at `c436180`, every repairable prefix and ambiguity row is unchanged, and the only semantic delta is a strictly enlarged in-edge rejection set that can never fire on a state the protocol itself produces — demonstrated by both matrices executing green on this tuple: 21 keys interrupted/restarted/converged on the workspace target and all 23 on the Git-directory target, each with asserted non-vacuity, fresh lease reacquisition, convergence to `Complete`, and retained-catalog revalidation, plus the 23-key set-equality assertion in both variants.

**Regression-test judgment.** Both new tests drive the real production loop (workspace runtime lease → `recover_or_create`) with injection at the post-caller-recheck hook — i.e. pre-acquisition drift, the exact probe-proven P2-1 class — and assert typed failure, foreign fact untouched, and no publication. They test what they claim, and their names correctly scope "after … recheck" to the caller's re-check. The primitive-internal residue (drift between the primitive's own observation and the rename) is deliberately not hook-injectable inside the sealed primitive; it is correctly characterized as post-acquisition boundary territory by the erratum and the `DirectoryInteriorRecheckV1` doc comment, and the durable state such drift would produce is not evidence-free: its classification (read-only ambiguity, foreign bytes preserved) is executed by the constructed-state unowned-final/staging test. No scenario is mischaracterized. One nit, not a finding: that doc comment cites "amendment §4.1 ¶2" for the rejection claim, which lives in §4.1's third paragraph and the erratum; worth a pointer fix in the next docs pass.

**§4.1 erratum state-model accuracy (verification assigned to this round).** Paragraph 1 matches the implementation exactly as analyzed above (second identity-proved no-follow capability; complete bounded interior; retained final handle for the retirement destination; "immediately before the rename" is literal — nothing but the destination-recheck match and, on Windows, the in-function path derivation intervenes). Paragraph 2 is accurate as a state model: the destination path is derived from the retained handle inside `rename_open_source` itself; re-binding by a same-user rename of the destination directory or an ancestor inside that window is possible (`FILE_SHARE_DELETE`), is now explicitly assigned to the boundary (closing the ancestor-route text gap my round-2 P3-2 identified); the post-publish verification on every edge goes through the retained destination handle, so a redirect is detected as a typed error with no further mutation — and the erratum correctly claims detection only, not convergence: the post-redirect durable state (e.g. pre-retirement final with active vanished) is not a legal protocol tuple and classifies as read-only ambiguity, never misrecovery. The "rejects a non-null RootDirectory" clause and the window test are platform claims executable only at R2-F, where the amendment now pins them.

## Commands and exact results

All from this exact tuple; gwz-core commands from `gwz-core/`.

```text
git rev-parse HEAD (root)                  → f7ba3235703b11eb1c9a6a72e2179afa2b7b5837
git -C gwz-core rev-parse HEAD             → 0d8382e1cbf6500e499bf2dc5ed23d6256f6e642
git -C gwz-cli rev-parse HEAD              → 3cca145c0b32410f250f640730ed7ca18f1da59f
git -C gwz-py rev-parse HEAD               → 929efb0d440206772155cadda89748e9431f9ca8
git -C taut rev-parse HEAD                 → f00841966663bb0d500974ede446d30e9bfeeac4
git status --porcelain (all five)          → empty, at start and at final recheck
git -C gwz-core diff --stat c436180..0d8382e
                                           → 8 files, +271/−26 (complete delta read)
git -C gwz-core diff c436180..0d8382e -- src/checked_artifact/fault_v1.rs
    src/checked_artifact/interface_tests   → empty (163-key pin and family untouched)
cargo test --lib checked_artifact::        → ok. 243 passed; 0 failed (8.84s, macOS host)
cargo test --lib checked_artifact::catalog::bootstrap::tests -- --nocapture
                                           → ok. 12 passed; 0 failed (4.28s; both matrices:
                                             21-key workspace and 23-key Git-directory,
                                             every key interrupted, restarted, converged,
                                             retained catalog revalidated, non-vacuity per row)
cargo test --lib checked_artifact::capability::pre_catalog::provider::directory_mutation_tests
    -- --nocapture                         → ok. 9 passed; 0 failed (includes the two new
                                             inverted-probe tests, drift preserved byte-exact,
                                             no publication)
python3.13 scripts/checks/check_checked_artifact_boundaries.py
                                           → checked-artifact boundary: ok
                                             (15 visible entries, 5 classified modules)
python3.13 scripts/checks/test_check_checked_artifact_boundaries.py
                                           → Ran 63 tests in 464.441s — OK (includes both new
                                             adversarial raw-rename caller probes)
cargo fmt --all -- --check                 → PASS
[P3-1 probe, scratchpad copy of this tree]
  baseline checker on copy                 → ok, exit 0
  alias-bound rename_relative caller appended to checked_artifact/cleanup.rs
  checker on mutated copy                  → ok, exit 0   (evasion; direct-call form fails
                                             per the suite's own legacy-interior probe test)
  digest-coverage enumeration              → 55 production checked_artifact files outside
                                             both digest maps (the evasion surface)
grep recover_or_create (production)        → definition + sealed re-export only; no
                                             production caller (fail-closed status unchanged)
git -C gwz-core show 0d8382e --stat        → 1 file, +1/−1: platform.rs digest refresh
                                             (intermediate b923109 was checker-red on it)
```

## Residual risks

- **Native Linux and Windows execution: none on this tuple** (unchanged position; R2-F gate). Everything green here is macOS/APFS. The Windows arm of the changed seam — exact-handle rename, `RootDirectory` rejection claim, the destination-window residual and its detection — remains source-only from this seat; the erratum correctly pins the native window test to R2-F.
- **The primitive-internal residue window (T_obs → T_ren) has no direct injection test** and cannot get one without a hook inside the sealed primitive; evidence for its downstream behavior is the constructed-state ambiguity test plus analysis. This is the same class of documented, boundary-assigned residue round 2 accepted for pathname substitution.
- **Matrix granularity unchanged:** each key interrupts at first occurrence per run; repeating keys' later occurrences are compensated by the exactly-10-restarts walk and constructed partial states, not separately interrupted.
- **[P3-1] evasion surface** stands at 55 non-digest-pinned files until the checker counts bare-token references or R2-D retires the legacy call sites; direct-call regressions are caught today.
- **State-2 P3-1 (Git-parent dirent barrier)** remains open and routed; nothing in this delta touches it.
- **Lineage note:** intermediate commit `b923109` fails the boundary checker (stale platform.rs digest, healed one commit later); reviews and CI gates on this line should continue to target settled tips.

## Final tuple recheck and next action

Final recheck (post-review): all five repositories at the exact requested tuple — root `f7ba3235…`, gwz-core `0d8382e1…`, gwz-cli `3cca145c…`, gwz-py `929efb0d…`, taut `f0084196…` — all worktrees clean, identical to the start-of-review values. This review made no workspace changes; the only expected root status entry after filing is this report at `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewState-3.md`.

Verdict **GO** from the State-3 axis: the paired P2-1 is closed with the window sealed inside the primitive on both edges, verified by independent source reconstruction and executed probes; the two fault matrices interrupt, restart, and converge on every key with no crash-window classification changed; both round-2 P3 routings are intact and the §4.1 erratum is state-model accurate, including its post-acquisition residual characterization and the ancestor-route boundary assignment. The one new finding is P3 (checker alias-shape evasion) and routes to the checker-hardening/R2-D lane. Next action per `CurrentProgramCheckpoint.md`: reconcile with the paired round-4 Code-axis verdict; if it also reports no open P0/P1/P2, record R2-C2 acceptance at this checkpoint and proceed down the ordered list (Windows compile-correction port and matrix classification before any catalog activation, then the docs package, then R4b-G → M5b → A1), carrying [P3-1], State-2's [P3-1]/[P3-2], and the audit P3-2/P3-3 packages as tracked, non-blocking obligations.
