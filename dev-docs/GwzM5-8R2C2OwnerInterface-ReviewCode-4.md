# R2-C2 physical catalog-owner code re-review, round 4 (focused)

Date: 2026-08-15

Reviewer designation: Code-4 (independent code/architecture axis; focused re-review of the round-3 remediation `c436180..0d8382e`; no current-round peer report was read — `GwzM5-8R2C2OwnerInterface-ReviewState-2.md`, filed in the same root delta, was not opened).

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 2 |

Round-3's blocking P2-1 is closed by exactly its option (a): the sealed primitive now re-verifies the directory-source interior through its own identity-proved capability inside the acquisition window, and the retirement edge re-verifies the destination interior through the retained final handle, immediately before the rename. The round-3 counterexample shape is a committed passing inversion on both edges. The exit-criterion-violating P3-1 is closed by the specified subsystem-wide raw-rename inventory; my round-3 probe B now fails closed on two independent gate axes. P3-2, P3-4, and P3-5 are closed. **P3-3 remains open**: the remediation itself recurred its exact failure shape (red boundary gate at intermediate commit `b923109`, verified by execution), which the prose "lane rule" did not prevent. Two new P3s are filed (identifier-rebinding evasion of the new gate; the checkpoint misrecording the remediation as same-commit-refreshed). No open P3 violates an exit criterion under the readings justified below; the checkpoint gate requires zero P0-P2, which is met.

## Exact reviewed tuple and worktree state

- workspace root `f7ba3235703b11eb1c9a6a72e2179afa2b7b5837` — clean at start and end
- `gwz-core` `0d8382e1cbf6500e499bf2dc5ed23d6256f6e642` — clean
- `gwz-cli` `3cca145c0b32410f250f640730ed7ca18f1da59f` — clean (unchanged since round 2)
- `gwz-py` `929efb0d440206772155cadda89748e9431f9ca8` — clean
- `taut` `f00841966663bb0d500974ede446d30e9bfeeac4` — clean
- `gwz status --json`: 7 members, all `lock_match: Matches`; `gwz.conf/gwz.lock.yml` pins gwz-core `0d8382e`.

No workspace file was written or staged by this review; all probes ran in disposable `git archive` exports under the session scratchpad; the only pristine-tree writes were `target/` build artifacts (permitted).

Scope: the full remediation delta `git -C gwz-core diff c436180..0d8382e` (2 commits, 8 files, +271/−26) read line-by-line, plus full reads of `publication.rs`, `platform.rs`, both `directory_mutation` edges and their tests, `interior.rs` (`observe`/`staging_plan`/`completed_record`/`row`/`exact_slot`), `mutation.rs` (`publish_active_record`, `read_bounded`), the checker's `check()` and both new unit tests, `capability_permit.rs`, the §4.1 erratum, amendment §8.13, RemPlan §"R2-C2 second-review publication correction" and §9, `CurrentProgramCheckpoint.md`, and the round-3 report in full. The root delta `d2b4c77..f7ba323` (docs + lock only) was reviewed for the erratum and checkpoint items.

## Prior-finding closure

### [P2-1] Directory-interior publication window — **CLOSED**

Correction is round-3's option (a), implemented exactly:

- `PublicationSourceV1::Directory` now carries `DirectoryInteriorRecheckV1 { durable_identity, expected }` (`publication.rs:23-37`). Inside the primitive, after `open_rename_source` and the first identity proof, the Directory arm reopens the source no-follow through the retained parent as a second `Dir` capability (`:134-136`), re-proves its identity against the owner-observed identity (`:137-142`), observes the complete bounded interior through it, and requires `interior::staging_plan == Complete` (`:143-152`), all before the rename (`:172`). A foreign-named entry rejects even earlier, inside `interior::observe` → `exact_slot` ("catalog directory contains an unowned child") — still inside the primitive, still pre-rename; a grammar-shaped byte drift rejects via the new "publication source interior changed inside the acquisition window" ambiguity. Either way: typed rejection before publication.
- Retire edge: `DestinationRecheckV1::PreRetirementFinal` re-observes the destination through the caller's retained, identity-proved final handle (`:155-171`) and requires `completed_record` present **and** retired row absent — the exact condition pair the caller checks (`directory_mutation.rs:288-304`, `:323-331`; the extra `row(.., CatalogBootstrapRetired)` guard is necessary because `completed_record` legally tolerates a retired row post-retirement).
- **Second-handle same-object argument:** both the rename handle and the interior-bearing capability are independently proved equal to the owner-observed identity; divergence between them requires a same-user swap plus object-identity reuse strictly after acquisition — inside the accepted §4.1 same-user boundary, and on Windows the renamed object is exactly the first handle. The retire destination is handle-bound (no path re-resolution on Unix), so no destination identity recheck is needed there.
- **Residual is genuinely post-acquisition:** everything up to `interior::observe` (`:143`) is caught, and observe runs strictly after source acquisition (`:91`); only drift in [observe, rename] escapes, which is the accepted boundary. The `DirectoryInteriorRecheckV1` doc comment states exactly this.
- **`DestinationRecheckV1::None` on the other five edges is correct:** scratch→active, staging→final, and the three anchor moves have a name-absence-only destination precondition, enforced atomically by NOREPLACE; no destination-interior fact exists to protect there (final-publish additionally has the caller's fresh-aggregate final-absent check).
- Hooks `FinalPublishAfterInteriorRecheck` (`directory_mutation.rs:237-238`) and `ActiveRetireAfterInteriorRecheck` (`:332-333`) sit after the caller's last recheck, immediately before the primitive; the enum, thread-local, `run_next_at`, and `run_fault` are all `#[cfg(test)]` — production-inert.
- **Regression tests are the round-3 probe inverted and are sound:** foreign bytes present proves the hook fired; run error plus final absent (resp. active present plus retired-name absent) proves the rename never executed, so rejection occurred at/inside the primitive after the drift. Both ran green in the 243. (They pin behavior, not the error detail — acceptable; noted under residuals.)
- Interior verification transitively inherits `observe`'s discipline: ten-entry bound, name budget, `try_reserve_exact` rows — no new unbounded or infallible allocation inside the primitive.
- The extra directory capability drops at match-arm end, before the rename — no self-held handle across the Windows source-directory rename.

### [P3-1] Structural gate blind to raw provider renames — **CLOSED** (a narrower, new-root-cause residual is filed as new P3-1 below)

- `RAW_RENAME_CALL_ALLOWLIST` + subsystem-wide scan landed exactly as round-3's required correction specified: publication.rs 1+1, platform.rs 2/2/5 (including its in-file windows test module), transition.rs 2 and residue.rs 2 (the four legacy edges until R2-D), zero everywhere else, exact equality both directions over `production_rust_files(checked_artifact)` (which includes `*_tests.rs` siblings).
- **Independent recount:** my own grep over the subsystem found exactly the allowlisted call sites and nothing else; counts match to the token; the only non-call occurrences are the platform.rs `use` line, one masked comment, and string literals in the excluded `interface_tests`.
- **Round-3 probe B re-run (disposable export):** a direct `platform::rename_relative` caller appended to `provider/retained.rs` now fails the checker with two independent findings — the tree digest **and** "raw rename caller outside the sealed publication seam: …retained.rs (rename_relative)", exit 1. The raw-rename finding is digest-independent, so the round-3 self-service-refresh laundering no longer clears it: with the digest refreshed, the raw-rename finding still fires.
- The two specified adversarial unit tests exist (`retained.rs` and `cleanup.rs` probes), are genuine subprocess executions of the gate, and pass (suite 61→63, OK).
- Together with the digest pins (any provider-tree edit must visibly touch the checker file), the described channel is closed for call-shaped reintroduction. The identifier-rebinding variant that survives is a distinct root cause — new finding [P3-1] below, non-blocking under the reading justified there.

### [P3-2] Windows destination comment overclaim — **CLOSED**

- `platform.rs:121-130` now states the true residual: destination is an absolute path derived from the retained handle immediately before the rename; the handle does **not** prevent a same-user rename of the destination directory or an ancestor (directory opens share `FILE_SHARE_DELETE`); residual assigned to the cooperating-same-user boundary; mandatory post-publish verification through the retained destination handle detects a redirect read-only; native window test at R2-F. This is byte-for-byte the round-3 required correction. The edit is comment-only (confirmed by diff and by the digest forensics below).
- **§4.1 erratum verified against the implementation (amendment-class doc change assigned to this round):** clarification 1 (in-primitive interior recheck for directory sources through "its own acquired capability"; retained-final-handle destination recheck for retirement; drift up to source-capability acquisition rejects before publication; only post-acquisition drift falls to the same-user boundary) matches `publication.rs` exactly and is conservatively true — the implementation catches drift up to observe-time, which is later than acquisition. Clarification 2 (Windows destination re-binding residual, same-user assignment, post-publish retained-handle detection, R2-F native test) matches `platform.rs` and holds on all three destination-bearing edges: post-publish verification runs through the retained parent handle (`mutation.rs:234-239`, `directory_mutation.rs:258-270`) and the retained final handle (`:350-355`), so a redirected publication is detected read-only. One wording note: the erratum self-describes as "verified by the round-4 re-review", written before this review ran — true as of this report, but pre-declaring verification in a controlling document is practice to avoid (no finding; no present consequence).

### [P3-3] Protected-tree digest discipline — **OPEN (recurred inside the remediation itself)**

The disclosed two-commit shape does **not** satisfy the same-commit-refresh intent, and I verified the disclosure independently:

- `platform.rs` at `b923109` hashes `b51ad4e7…` while the checker pin at `b923109` remained `7745b7a1…` — **the mandatory boundary gate was red at that commit** (executed: checker on a `b923109` export fails with exactly one finding, "protected source allowlist changed: checked_artifact/platform.rs", exit 1). The `pre_catalog` tree pin **was** refreshed same-commit (`3a1a72b0…` at `b923109`); only the individual platform.rs file pin was missed after the comment-only edit — matching the disclosure. `0d8382e` changes exactly that one pin line, and at `0d8382e` pin == file.
- Nothing was smuggled: the `b923109..0d8382e` protected-source delta is the comment block alone, read line-by-line.
- The disclosed cause (a piped exit code masked the red gate on the lane) is consistent with the evidence but not independently verifiable from artifacts; it exposes a second weakness — the lane's gate invocation is not fail-safe.
- Round-3's required correction was taken as prose only ("adopted as a lane rule", checkpoint item 3) and failed on its first exercise; no enforcement mechanism exists in-tree (no git hooks, no per-commit lane CI; the push workflow checks branch heads only, so a multi-commit push again hides the red commit; bisect on the gate now breaks at `b923109` exactly as it did at `95d292f`).
- **Strengthened required correction:** a mechanism, not prose — per-commit checker execution on lane pushes (CI over the pushed range, or a pre-push hook) plus unmasked gate invocation (direct call or `pipefail`). The deviation itself must be recorded (new finding [P3-2] below).

Severity stays P3 (final tuple green; no correctness defect; no smuggling), not exit-criterion-violating.

### [P3-4] Fallible allocation in the primitive — **CLOSED**

`publication.rs:108-116`: `try_reserve_exact(expected_bytes.len() + 1)` with `CheckedFsError::unsupported(PlatformCapability::PrivateNamespaceCollisionScan, …)` — the exact `read_bounded` pattern (`mutation.rs:322-337`), same typed variant; it reserves the same `+1` it reads, marginally stronger than `read_bounded`'s `expected_len` reserve. Parity achieved as specified.

### [P3-5] Stale checkpoint tuple table — **CLOSED; the lock-authoritative wording is acceptable**

The two moving rows no longer carry a literal SHA that can contradict the lock; they defer to `gwz.conf/gwz.lock.yml` **in the same commit** (verified: lock pins gwz-core `0d8382e`; `gwz status` all Matches), and the literal-restatement obligation for the next docs-only commit is recorded in the document itself. Self-consistent by construction; the cold-start resolution chain (checkpoint → same-commit lock → exact SHAs) satisfies L1-31. The defect — an authority table naming a wrong SHA — cannot recur in this form. The deferred restatement must actually ride the next docs commit as promised (same commit as the corrections in new finding [P3-2]).

## Findings (new only)

### [P3-1] The raw-rename inventory matches identifier-call shape, not name binding; a one-line rebinding (use-alias, fn-pointer, or empty turbofish) reopens the laundered-rename channel with every gate green

- **Where:** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:854-870` (scan regex `\b<token>\s*\(`), `RAW_RENAME_CALL_ALLOWLIST` at `:158-176`.
- **Violated invariant:** L2-06 — structural gates must survive alias/re-export probes. On the binding sentences (RemPlan:987-988; amendment §8.13 "direct raw provider renames are structurally rejected"): under the syntactic reading of "direct" — which matches round-3's own required-correction specification (a token caller-inventory) and the program's demonstrated habit of naming "unlisted writer alias, function pointer …, crate-local wrapper" explicitly when alias-resistance is intended (RemPlan exit gate item 8, merge writer seam) — the exit criterion is satisfied and this does not block. Under a functional reading of "direct" (any raw rename not routed through the primitive) it would violate the exit criterion and block. I classify non-blocking on the first reading and flag the interpretation for the lead to record explicitly.
- **Reproduction (deterministic, disposable exact-SHA exports of `0d8382e`; executed):** (a) append to `cleanup.rs`: `use crate::checked_artifact::platform::rename_relative as relocate_entry;` plus a function calling `relocate_entry(…)` → checker prints "checked-artifact boundary: ok", exit 0, **with no checker edit at all** (cleanup.rs carries no digest pin — zero checker-visible trace); (b) the same alias inside `provider/retained.rs` plus the routine self-service `pre_catalog` tree-digest refresh → "ok", exit 0. Same class, not executed: `rename_relative::<>(…)` (empty turbofish defeats `\s*\(`) and `let f = …::rename_relative; f(…)` (fn-pointer binding).
- **Impact:** a future writer — most realistically an agent iterating against a red gate — can reintroduce the round-2 check-then-raw-rename defect with all committed gates green; in the un-pinned legacy interior the evasion is silent.
- **Required correction:** count bare identifiers (`\b(open_rename_source|rename_open_source|rename_relative)\b`, definitions excluded) per file instead of call shapes. A rebinding must name the item at least once, so alias, fn-pointer, and turbofish all fail closed; calls through re-exports already fail (leaf name stays call-shaped); `#[path]`/`include!` routes are already inventoried and the crate has no proc-macro identifier synthesis.
- **Regression test:** an alias-probe checker unit test beside the two committed direct-probe tests.

### [P3-2] The current-state authority misrecords the remediation as same-commit digest refresh; the landed history is a red intermediate gate plus a follow-up pin fix, and the deviation is unrecorded

- **Where:** `dev-docs/CurrentProgramCheckpoint.md`, "Next ordered actions" item 3: "… same-commit digest refresh (P3-3 discipline adopted as a lane rule)" — committed in root `dea0953`, the coordinated commit whose gwz-core member `b923109` was red on the mandatory boundary gate (forensics and execution above). Root `f7ba323`/gwz-core `0d8382e` fix the pin but no artifact records the deviation.
- **Violated invariant:** L1-16 (record evidence, not conclusions) and the checkpoint's own charter as the single current-state authority: the claim asserted adherence to the just-adopted rule in the very coordinated commit that broke it.
- **Reproduction:** compare the checkpoint text at `dea0953` with the digest forensics at `b923109` (executed checker: exit 1) and the one-line content of `0d8382e`.
- **Impact:** a cold-start reader concludes the P3-3 discipline held for this remediation; the authority and the actual history disagree, eroding trust in the checkpoint's self-certified gate claims (the exact drift class L1-16 exists to prevent). The honest disclosure reached this review only through the review-lane prompt, not the paper trail.
- **Required correction:** in the next docs commit (already owed for the literal tuple restatement), record the `b923109` red-gate deviation, its disclosed cause (piped exit status masking the checker), and the enforcement follow-up; checkpoint gate claims should cite executed evidence.
- **Regression test:** process-level; covered by the P3-3 mechanism correction (per-commit lane gate + unmasked invocation).

## Commands and exact results

All commands ran on the pristine tuple unless a disposable export is stated.

```text
git rev-parse HEAD; git -C {gwz-core,gwz-cli,gwz-py,taut} rev-parse HEAD; all status --porcelain
  PASS: exact requested tuple, all clean (start and end)
/Users/owebeeone/.cargo/bin/gwz status --json
  PASS: 7 members, all lock_match=Matches; lock pins gwz-core 0d8382e

git -C gwz-core diff c436180..0d8382e            2 commits, 8 files, +271/-26 (read line-by-line)
git -C gwz-core show 0d8382e                      exactly one changed line: platform.rs pin 7745b7a1->b51ad4e7

python3.13 scripts/checks/check_checked_artifact_boundaries.py
  PASS: checked-artifact boundary: ok (15 visible entries, 5 classified modules), exit 0
python3.13 -m unittest scripts.checks.test_check_checked_artifact_boundaries
  PASS: Ran 63 tests in 458.999s — OK (61 -> 63; both new raw-rename adversarial tests ok)
cargo test --lib checked_artifact::               (gwz-core, workspace target)
  PASS: 243 passed; 0 failed (241 -> 243; both new inverted-probe tests listed ok)

Digest forensics (git show <sha>:path | shasum -a 256 vs checker pins):
  c436180  platform.rs 7745b7a1... == pin                      green
  b923109  platform.rs b51ad4e7... != pin 7745b7a1...          GATE RED AT THIS COMMIT
  b923109  pre_catalog tree pin 3a1a72b0... (refreshed same-commit; only the file pin missed)
  0d8382e  platform.rs pin b51ad4e7... == file                 green

Disposable export, b923109:
  checker -> "failed / protected source allowlist changed: checked_artifact/platform.rs", exit 1
Disposable export, 0d8382e + direct raw rename call in provider/retained.rs (round-3 probe B):
  checker -> tree-digest finding + "raw rename caller outside the sealed publication seam:
  ...retained.rs (rename_relative)", exit 1  (raw-rename finding fires independently of digests)
Disposable export, 0d8382e + use-alias raw call in cleanup.rs (no checker edit):
  checker -> "ok", exit 0   UNDETECTED (new P3-1)
Disposable export, 0d8382e + use-alias in provider/retained.rs + recomputed pre_catalog tree digest
(byte-identical algorithm, accepted by the checker — algorithm parity proven):
  checker -> "ok", exit 0   UNDETECTED (new P3-1)

Independent token recount (grep over src/checked_artifact, definitions excluded):
  publication.rs 1+1; platform.rs 2/2/5 (incl. windows test module); transition.rs 2; residue.rs 2;
  zero elsewhere — allowlist exact against this tree
```

## Residual risks and unexecuted evidence

- **Windows evidence gap unchanged from round 3:** no full-crate Windows compile/run for any commit after `a02d7d6`. This remediation adds no new Windows compile risk — the platform.rs change is comment-only and publication.rs's additions are cfg-agnostic — but the object-binding and destination-window tests remain unexecuted until R2-F.
- The same-user post-acquisition boundary now also absorbs second-handle divergence (swap plus object-identity reuse between the primitive's two source opens) — bounded, documented in the new type's comment, same actor class as the accepted §4.1 residual.
- The two inverted-probe tests pin the invariant (typed rejection before publication, foreign fact untouched, destination absent) but not the specific error detail; a refactor could change which typed rejection fires without test signal.
- The alias/fn-pointer/turbofish gate shape (new [P3-1]) until the bare-identifier correction lands; the legacy interior (cleanup.rs, coordinator/, namespace/) remains un-pinned pending R2-D, so evasion there is checker-silent.
- The §4.1 erratum's "verified by the round-4 re-review" self-description predates this review; it is true as of this report.
- The disclosed piped-exit-code cause for the b923109 miss is consistent with all evidence but not independently verifiable from artifacts.
- gwz-cli, gwz-py, and taut are byte-identical to round 3; nothing in this remediation touches a cross-repo surface (gwz-core internals plus its checker only).

## Final tuple recheck and next action

Rechecked after writing this report: root `f7ba323` clean; `gwz-core 0d8382e`, `gwz-cli 3cca145`, `gwz-py 929efb0`, `taut f008419`, all clean; scratchpad-only probe writes; no workspace file, lock, or marker modified.

**Next action:** with zero open P0-P2 on this axis, the checkpoint's re-review gate is satisfied from the Code side pending the state-axis round-4 counterpart. File the two new P3s and the P3-3 reopen into the remediation ledger; land the bare-identifier gate correction plus alias regression test, and the P3-3 enforcement mechanism (per-commit lane checker, unmasked invocation), with the next package; the next docs-only commit must both restate the literal tuple (P3-5 promise) and record the `b923109` red-gate deviation (new [P3-2]); the lead should record an explicit reading of "direct raw provider renames" in §8.13 (syntactic vs functional) so the new [P3-1]'s blocking status is settled by authority rather than interpretation. Then proceed per checkpoint order toward R2-D/R2-F.
