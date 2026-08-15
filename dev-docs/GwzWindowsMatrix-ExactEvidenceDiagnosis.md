# Windows exact-evidence diagnosis — CRLF smudge & durability-anchor contamination

Date: 2026-08-15. Evidence: matrix run 6 (`31883655778` on `a506a93`) and
stability replay 6b (`31885262853` on `117117e`), windows-2022,
rustc 1.95.0. Produced by a commissioned read-only diagnosis agent under the
implementation lane; filed verbatim by the lane owner. Verdicts route the
two production-side finding classes from
`GwzWindowsMatrix-Classification.md` (run-6 tail, item b). Confirmation
instrumentation dispatched on probe branch `probe/exact-evidence-diagnosis`
(label-only), per the report's §4.

---

## 0. Scope correction on counts

The log contains **four** distinct tests panicking with
`RecoveryEvidenceMismatch` / "live rollback state is neither the exact
before nor after state" (log lines 2937, 2942, 2964, 2972), not two; and
exactly **two** with `PreservationEvidenceMismatch` / "root preservation
mutation failed exact post-verification" (lines 2856, 2878). Both messages
have exactly one production emit site each, so the grouping is unambiguous.

## 1. Failing tests and panic sites

**Class A — RecoveryEvidenceMismatch ("live rollback state is neither the
exact before nor after state"), 4 tests**, all in
`src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/`:

| Test | Panic site (run-6 tree) |
|---|---|
| `…reverse::preservation::tests::root_durability::bundle_consumer_rebarriers_visible_goals_before_advancing` | `reverse_preservation/root_durability.rs:283:13` |
| `…root_ambiguity_matrix::every_root_phase_rejects_fresh_ambiguous_work_without_physical_execution` | `root_ambiguity_matrix.rs:115:22` |
| `…root_successor_matrix::every_legal_root_handoff_form_exhausts_the_complete_phase_graph` | `root_successor_matrix.rs:165:21` |
| `…root_successor_matrix::every_root_phase_durable_successor_restarts_without_repeating_the_phase` | `root_successor_matrix.rs:91:18` |

The `bundle_consumer` and `every_legal_root_handoff` dumps both include the
decisive context: `rollback=Some(Participant { member_id: "mem_a", action:
ResetIntegrated, terminal_state: RolledBack }); recovery=Some(
RecoveryContextV1 { origin_state: RollingBack })` — the member was
**already successfully rolled back**, and the failure occurs when recovery
**re-verifies** that state.

**Class B — PreservationEvidenceMismatch ("root preservation mutation
failed exact post-verification"), 2 tests**, both in
`src/git/tests/g15/root_preservation/`:

| Test | Panic site |
|---|---|
| `git::tests::g15::root_preservation::mutation::every_physical_phase_row_is_before_after_and_restart_safe` | `mutation.rs:114:10` — the `.unwrap()` on `execute(CreateStash)` at mutation.rs:107-114 |
| `git::tests::g15::root_preservation::stash::checked_stash_rechecks_preimage_and_preserves_ignored_work` | `stash.rs:182:14` — the `.unwrap()` on `execute_root_preservation_step_checked(CreateStash)` at stash.rs:178-184 |

Both class-B failures are at the **CreateStash** step. Critically, in
stash.rs the pre-observe assert at stash.rs:171-177 (`observe == Before`)
**passed**, and in mutation.rs the execute's own internal pre-observe
returned `Before` (otherwise it would have errored "root preservation step
has ambiguous physical evidence" at `preservation_root.rs:105-109`
*without mutating*). So in both, the state was **exactly Before**, the
native stash **succeeded**, and the post-observation was **not After**.

## 2. Production code paths

**Class A emit site**:
`src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rolling_back_recovery.rs:70-75`,
in `verify_recovery_origin`. For `PendingRollbackActionV1::Participant` it
requires `observe_v1_participant_rollback(...) != Ambiguous` (lines 35-52).
That observer (`src/workspace_ops/merge/abort/participants.rs:95-133`)
computes for `ResetIntegrated`:
- `after` = `clean_checkout(backend, path, target_branch, before_commit, exclusions)` (line 106-113)
- `before` = `clean_checkout(..., resulting_commit, ...)` (line 124-126)
- `(false,false) → Ambiguous` (lines 128-132).

`clean_checkout` (participants.rs:235-252) = repository state Clean +
attached head/ref string equality +
**`backend.checkout_matches_commit_except(path, commit, [])`**. That lands
in `src/git/gitbackend/preservation_image.rs:149-197`: it flattens the
commit tree (blob **content bytes** from the ODB, lines 172-190, 442-449)
and compares it structurally (`live == expected`, line 196) against
`live_entries` — where a tracked file's worktree image is the **raw
on-disk bytes** read with `std::fs::read` (line 506-509). The comparison
is exact-byte, **filter-blind**.

The mutation being verified is `set_branch_target_checked`
(`src/git/gitbackend/merge_recovery.rs:165-216`), which before re-pointing
the ref performs `repo.checkout_tree(&target_object, safe)` (lines
202-206). libgit2's checkout writes files **through the CRLF smudge
filter**. Its own post-verification `verify_merge_result`
(`src/git/gitbackend/repository_support.rs:55-74`) checks only
head/branch/`status.is_dirty`/native state — and git **status is
filter-aware**, so a CRLF-smudged worktree still reads clean. That
asymmetry (mutation verified with the filter-aware model, recovery
verified with the raw-byte model) is the heart of class A.

**Class B emit site**: `src/git/gitbackend/preservation_root.rs:145-149`
in `execute_root_preservation_step_checked` (observe-Before → mutate →
re-observe must be exactly After, lines 79-150). For CreateStash the
mutation is `stash_for_merge_preservation(backend, root, merge_id, true)`
(line 118-121 → `preservation.rs:166-244` → libgit2 `stash_save` with
untracked included), and the post-observation is `observe_create_stash`
(preservation_root.rs:238-275), whose `After` branch requires the single
decoded stash to satisfy `stash.image.preimage_sha256 == *sha256` — the
guard sha captured by `prepare_root_preservation_stash` →
`capture_normalized` (preservation_root.rs:26-32).

**The exact-state model, precisely**: the `ImageEntry` model
(preservation_image.rs:10-29) hashes/compares (a) index facts `(stage,
mode, semantic-flags, oid)`, and (b) worktree content as **raw bytes**
(full bytes for equality in checkout-match; `Sha256(bytes)` inside the
encoded image, line 546), plus symlink targets and gitlink oids. It does
**not** use mtimes, file-index/volume-serial, or directory-entry order
(`BTreeMap` over raw path bytes). File identity `(dev, ino)` is used only
in the checked-artifact layer (preservation_root/files.rs:88-90, parent.rs
`is_current`), not in these two comparisons. So of the Windows-semantics
candidates, mtime/nFileIndex/enumeration-order are structurally irrelevant
here; what actually varies on Windows is **(1) worktree byte
representation after any libgit2 checkout (CRLF smudge)** and **(2) the
content set of the private `.gwz/checked-artifacts` area (Windows-only
durability anchor file)**.

## 3. Root causes

### Class A: CRLF smudge after production checkouts breaks the raw-byte "exact after state" (high confidence)

Chain of evidence:

1. Fixtures inherit host git config. `create_repo` is a plain
   `git2::Repository::init_opts` with no config overrides
   (`src/git/gitbackend/repository.rs:63-70`); `commit_file` writes LF
   content via `fs::write` then git2 `index.add_path`
   (`src/workspace_ops/tests/g02.rs:797-830`) — blobs are LF, worktree
   stays LF, **no checkout during fixture setup**. Grep proves only the
   `diff` test family defends against autocrlf
   (`src/diff/tests/workspace_fixture.rs:145`, `src/diff/tests/fixture.rs:50`,
   `src/diff/render/tests.rs:54`, `tests/diff_render_spike.rs:59`) — the
   merge/g15/reverse families never set `core.autocrlf=false`. GitHub's
   windows-2022 image ships Git for Windows with machine-level
   `core.autocrlf=true`, and libgit2 discovers and honors that system
   config.
2. The narrow tests fail exactly at the checkout edge.
   `integrated_participant_classifies_exact_before_after_and_ambiguous`
   (`tests/reverse_rollback/phases.rs:9-53`): observe `Before` **passes**
   (worktree bytes are still the test's own `fs::write` LF output);
   `set_branch_target_checked` **succeeds** (its `verify_merge_result` is
   filter-aware); the `After` assert at phases.rs:28 gets `Ambiguous` —
   log: `left: Ambiguous, right: After`. Identically `faults.rs:27`.
   `entry.rs:44-65` (`no_mutation_participants_require_the_exact_clean_before_checkout`)
   runs `set_branch_target_checked` at entry.rs:52-55 and then requires
   the byte-exact `verify_v1_no_mutation_participant` to pass at
   entry.rs:58-65 — it fails with "no-mutation rollback participant does
   not exactly match its recorded checkout" (participants.rs:158-162),
   same mechanism.
3. Independent smoking gun with no checked-artifact involvement:
   `g15::merge_preservation_stash_is_verified_idempotent_and_excludes_ignored_files`
   fails at `src/git/tests/g15.rs:170` on
   `assert!(backend.checkout_matches_commit(...))` right after a native
   stash — `git stash` resets dirty tracked files to HEAD via checkout →
   smudge → CRLF on disk → raw-byte compare false. This proves smudging is
   happening in this exact environment through this exact backend.
4. The four class-A tests are the same failure one layer up: mem_a
   (fixture `tests/reverse_preservation/mod.rs:68-99`: `members/a`,
   README.md "before\n"/"result\n" via `commit_file`) is rolled back by
   the service (`ResetIntegrated` → `set_branch_target_checked` → README
   smudged to `before\r\n`), reaches `terminal_state: RolledBack`
   (mutation-time verification is filter-aware), and then a later recovery
   boundary (WriteBundle BeforeDurability injection, ambiguity probes,
   successor restarts) calls `verify_recovery_origin` →
   `observe_v1_participant_rollback` → `after=false` (CRLF ≠ blob LF),
   `before=false` (head moved) → `Ambiguous` → the exact logged error with
   the exact logged context.

Why Linux/macOS pass: no CRLF filter → checkout writes blob bytes verbatim
→ raw-byte equality holds.

What contradicts the alternative hypotheses:
FlushFileBuffers/rename/handle-sharing issues produce `IoError` os-error
panics (the other ~20 tail failures do exactly that), not clean
`Ambiguous` classifications; mtime/file-index are not inputs to this
comparison; 8.3 names can't matter because paths compared are relative raw
index paths, not filesystem spellings.

Blast-radius note (same root, adjacent messages): the
`Stopped(RecoveryRequired) != Terminal(Aborted)` family
(reverse_preservation `faults.rs:58`, `phases.rs:35`, `real_git.rs:40`,
`real_git.rs:159`, `root_successor_matrix.rs:209`; reverse_rollback
`service_fault_matrix.rs:114`, `service_durability.rs:82` "live rollback
aggregate prefix has drifted") is the same two roots surfacing through the
service loop — per-test attribution between root A and root B should fall
out of the same instrumentation run.

### Class B: the Windows-only durability anchor file contaminates the exact stash image (high confidence)

The mechanism is entirely internal — the model's own Windows durability
machinery violates its own exactness invariant:

1. On Windows, every checked-artifact operation creates a **persistent
   anchor file** `.gwz/checked-artifacts/.ca1-durability-anchor-<32hex>`
   (content `GWZ-CHECKED-ARTIFACT-DURABILITY-ANCHOR-V1\n`):
   `prepare_private` at `src/checked_artifact/platform.rs:369-445`; the
   rename-round-trip barrier that *needs* it is `private_barrier` at
   platform.rs:448-483. `finish()` (cleanup.rs:26-165) removes
   goal/source/authority files but **never the anchor** — correctly, since
   it is the durability anchor. On Unix `prepare_private` is a no-op
   (platform.rs:250-258), so **no such file exists on Linux/macOS**. The
   g15 normalize steps (MarkerWorktree/LockWorktree via
   `files::replace_relative` → `checked_artifact::entry` →
   `ensure_authority` → `open_private(true)`) create it in every failing
   test's repo.
2. The anchor is a normal **untracked** worktree file (fixture excludes
   only `ignored/`, g15 support.rs:12, 76). The exclusion constant
   `CHECKED_ARTIFACT_PRIVATE_PATH = ".gwz/checked-artifacts"` is applied
   **only** in `capture_normalized` and the checkout-overlay paths
   (`raw_excluded_paths`, preservation_image.rs:221-232, applied at lines
   40, 192-195) — **not** in `capture`/`capture_inner` (line 50 passes
   `&[]`), **not** in `decode_stash` (lines 320-382), and nothing keeps
   libgit2's `stash_save --include-untracked` from sweeping it into the
   stash's untracked commit.
3. That asymmetry reproduces every class-B signature exactly:
   - `checked_stash_preserves_all_eligible_work_and_excludes_late_ignored_work`
     (stash.rs:397-449, failed at 410:5): compares un-excluded `capture`
     against the excluded `normalized_image` → the anchor is one extra
     `ImageEntry` on the left → **different `preimage_sha256`, identical
     dirty flags** (`untracked` already true from `untracked.txt`) —
     precisely the logged `left/right` pair.
   - The two post-verification panics: pre-observe is anchor-blind
     (normalized guard) → `Before`; `stash_save` sweeps the anchor into
     the untracked stash commit and deletes it from the worktree;
     `decode_stash` therefore yields an image with one extra entry →
     `stash.image.preimage_sha256 != guard` (preservation_root.rs:256-262)
     → not After; stash list non-empty → not Before → `Ambiguous` →
     `evidence_error` at preservation_root.rs:145-149.
4. Consistency checks that clinch it: the **checked** non-root stash path
   (`stash_for_merge_preservation_checked`, preservation.rs:246-363)
   compares un-excluded `capture` against an expected preimage recorded
   from the same un-excluded `capture` — anchor present on **both** sides
   — which is why the reverse_preservation *service* flows complete their
   root stash phases (see `real_git.rs:159`'s dump showing the full
   stash/reset phase lists done) while the g15 API tests with the
   **normalized** guard fail. Only untracked work is dirty in the two
   failing g15 tests (`user.txt`), so the stash rewrites no tracked files
   and CRLF cannot explain them — the anchor does, byte for byte. Related
   corroboration: `fault_boundaries.rs:61` fails with `GitCommandFailed
   "path too long: '...\repo\.gwz\checked-artifacts\ca1-<64hex>-<64hex>-<32hex>.source'"`
   — private-area residue is visible to git machinery on Windows and its
   ~170-char names also breach MAX_PATH; same contamination surface,
   different symptom.

## 4. Ranked hypotheses, evidence, and discriminating instrumentation

Class A: **H1 (autocrlf smudge at production checkout edges)** — supported
by items 2-3 above; contradicted by nothing in the log. H2 (index
stat/mode divergence) — contradicted: modes/oids are tree-derived on both
sides; would fail on Linux too. H3 (handle/sharing interference) —
contradicted: produces IoError, not clean Ambiguous. Confidence: high, but
one precondition (system `core.autocrlf=true` reaching libgit2) is
environmental.

Class B: **H1 (durability-anchor contamination)** — every observed byte of
evidence fits; no counter-evidence. H2 (CRLF in stash blobs) —
contradicted: all fixture bytes are LF end-to-end, and only untracked
files were dirty (no tracked rewrite). H3 (delete-pending residue) —
contradicted: would surface as IoError/`empty preservation image entry`,
not a clean hash mismatch.

**Minimal instrumentation (label-only, io_op_error idiom per
`src/checked_artifact/observation.rs:331`) to settle both in one CI run:**
1. `rolling_back_recovery.rs:70-75`: extend the RecoveryEvidenceMismatch
   message with the observation triple — e.g. `"...neither the exact
   before nor after state (before={}, after={}, head_ok={}, ref_ok={},
   state_clean={}, checkout_bytes_ok={})"` — by returning those booleans
   from `clean_checkout`'s sub-checks into the message only.
   `checkout_bytes_ok=false` with everything else true proves H1-A in one
   run.
2. `preservation_root.rs:145-149`: append `observed={:?}` plus, for
   CreateStash, the three candidate discriminators from
   `observe_create_stash`: guard sha, decoded stash image sha, and the
   first path present in the stash image but absent from the guard image
   (that path will literally print
   `.gwz/checked-artifacts/.ca1-durability-anchor-…`, naming H1-B in the
   log).
3. Zero-code environmental confirmation: one diagnostic workflow step
   `git config --show-origin --system core.autocrlf` in
   windows-matrix.yml.

## 5. Verdicts and fix direction

**Class A (RecoveryEvidenceMismatch): (b) production model too strict for
Windows semantics — needs a reviewed semantic amendment (State-axis review
required).** The model currently holds two inconsistent definitions of
"exact checkout": mutation-time (`verify_merge_result`, filter-aware) and
recovery-time (`checkout_matches_commit_*`, raw-byte). On Windows with
active CRLF filters these disagree by design of git itself. The amendment
must pick one semantics for both edges. Recommendation: keep raw-byte
exactness as the model and make production checkouts byte-faithful —
libgit2 checkout at the two rollback edges (`merge_recovery.rs:202-206`
and `abort_merge`'s checkout at 149-155) should run with filters disabled
or the comparison should compare **filtered** (clean-side) content; either
choice changes production semantics on Windows and therefore triggers the
program's State-axis review. (A test-only "set autocrlf=false in fixtures"
would go green but merely hides the same divergence from real Windows
users, and the campaign's rules correctly treat this as a production
finding.) Not (a) — no state was actually lost; the after-state is
git-equivalent. Not (c) — injection only chose *when* re-verification ran;
g15.rs:170 fails with no injection at all.

**Class B (PreservationEvidenceMismatch): (a) production bug on Windows —
the exactness invariant is genuinely violated by the product's own
artifact, via an asymmetric exclusion.** The `.gwz/checked-artifacts`
private area is excluded from the guard image but not from `capture`, not
from `decode_stash`, and not from the native stash sweep; the Windows-only
anchor makes the asymmetry observable (and any crash-retained `ca1-*`
residue would equally poison it, on any OS). Fix direction: make the
private area invisible to the preservation image model everywhere — apply
`CHECKED_ARTIFACT_PRIVATE_PATH` exclusion in `capture_inner` and
`decode_stash` (and assert the stash untracked tree contains nothing under
it, or pre-exclude it from the stash via the handoff boundary);
alternatively relocate the private area under `.git/` (same volume → same
rename domain, satisfying `open_private`'s domain check at
residue.rs:89-101), which also retires the MAX_PATH exposure. Either way
this changes what the recorded preimage covers — **production semantics
change → State-axis review under the program rules.** Not (c): no
injection in either failing test's mutation path; the injected-fault
sibling tests fail for the same two roots at their recovery boundaries.

Both classes are deterministic (run 6b re-run was bit-identical per
`GwzWindowsMatrix-Classification.md`, run-6b section), consistent with
environment-structural causes rather than races. If the program wants
belt-and-braces confirmation before the semantic amendments, the two
label-only message extensions above discriminate all hypotheses in a
single dispatch.
