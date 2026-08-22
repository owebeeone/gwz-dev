# R2-D Step 2.1 review — LeafObserver production implementation (round 1)

- **Date:** 2026-08-22
- **Reviewer:** single-axis adversarial reviewer, **Code-axis lead** (adopted
  plan §4 Phase-2 gate note: single-axis, Code-first for 2.1, auto-escalation
  on any P0/P1/P2).
- **Object:** working tree of `gwz-core` (own repo, base `c13f773`):
  NEW `src/checked_artifact/capability/pre_catalog/provider/leaf_observation.rs`
  (539 lines, 402 code), NEW `provider/tests_leaf_observation.rs` (794/640),
  NEW `provider/tests_leaf_fault_matrix.rs` (497/379); the Step 2.1 lines of
  `provider.rs` (9 lines: the 5-line `#[allow(dead_code, reason = …Step 2.4
  converts its production caller")] mod leaf_observation;` block and the two
  2-line `#[cfg(test)] mod tests_leaf_*;` declarations — the brief said "+8",
  actual is 9, immaterial); and the `durable_leaf` region of
  `interface_tests/fault_expected_keys.rs` (family flip Reserved→Executed with
  its comment, the `leaf_observation.rs` `FAULT_INJECTION_SOURCES` entry, and
  `durable_leaf` in the executed-set literal). Four other lanes share the live
  tree (namespace 2.2 incl. `namespace_mutation.rs` and the `namespace` region
  of the same fixture file, admission remediation, platform/ARM,
  workspace_ops); nothing outside the object is judged here, and live-tree
  gate noise is attributed below.
- **Contract:** `GwzM5-8R2DInterfaceFreeze.md` §3.3 (frozen seam; `leaf.rs`
  byte-identical), §3.5 (11 `durable_leaf.*` keys, no minting), §4.3 rows
  E8-E11 (no publication-primitive call); `GwzM5-8R2D-Plan.md` §4 Step 2.1;
  ConsumerCheckpoint §8 :232-237.

## Verdict

**NO-GO** (one P1). Findings: **1 P1, 0 P2, 3 P3**. **The auto-escalation
rule is triggered** (a P0/P1/P2 exists): per the adopted plan §4 Phase-2 gate
note, the lane owner convenes the second axis (State) on this object together
with this report.

Everything else this step promises is delivered and verified — the seam is
untouched, the boundedness/one-handle/two-sided/no-conflation properties hold
under attack, the 11 injection sites are correctly placed with no key minted,
the matrix is substantive on both target variants, the documented deviation is
sound and correctly scoped, and the isolated-tree gates replicate exactly
(290/0; checker: only the pre_catalog tree finding). The one P1 is a
platform-arm defect: the E9 flush cannot succeed on Windows as coded.

## Findings

### [P1-1] E9 leaf flush is structurally un-executable on Windows: `sync_all` on a read-only handle

`leaf_observation.rs:169-170`:

```rust
    file.sync_all()
        .map_err(|source| CheckedFsError::io("flush the observed durable leaf", source))?;
```

against the handle opened at `leaf_observation.rs:339-340`:

```rust
    let mut options = OpenOptions::new();
    options.read(true).follow(FollowSymlinks::No);
```

`File::sync_all` is `FlushFileBuffers` on Windows, and `FlushFileBuffers` is
documented to require the handle to hold the **`GENERIC_WRITE`** access right;
on a `GENERIC_READ`-only handle it fails with `ERROR_ACCESS_DENIED` (os error
5). The observation handle is read-only by design (it observes; it must not
demand write access to the leaf). Therefore on Windows **every**
`observe_exact_durable` call errors at the flush edge and the exact durable
route can never produce a proof. macOS/Linux are unaffected (`fsync` is legal
on an `O_RDONLY` fd).

Aggravating facts, all verified on this tree:

- Every other production `sync_all` in `checked_artifact` is on a
  write-opened file or inside the Windows anchor write
  (`mutation.rs:163/:236`, `directory_mutation.rs:561`,
  `admission_mutation.rs:419`, `residue.rs:377/:477/:526`,
  `platform.rs:389/:453`) — this is the tree's **first** read-handle flush, so
  no existing Windows evidence covers it.
- Neither the object nor any contract doc names the constraint
  (grep for `FlushFileBuffers`/`GENERIC_WRITE`/read-only-flush across the
  freeze, plan, checkpoint, and platform amendment: no hits). The freeze's
  §4.3 E9 win cell ("`sync_all`") froze an arm that is unimplementable as
  written on a read-only observation handle — the implementation follows the
  table faithfully and inherits the defect.
- The durable-route tests in `tests_leaf_observation.rs` and all of
  `tests_leaf_fault_matrix.rs` are **not** platform-gated (only the
  symlink/exec-bit cases are `cfg(unix)`), so as parked this object turns the
  **next full Windows matrix run red** — a standing program gate
  (checkpoint ledger; the freeze §4.2 tracked-acceptance pattern) — rather
  than merely deferring discovery to the R2-F native-fault gate.

Failure mode is fail-closed (typed io error; no wrong proof, no mutation), so
this is not a soundness hole; it is a required-gate breaker on a declared
target platform.

**Minimal remediation:** route E9 through a platform helper with a documented
Windows arm, exactly on the adopted P2-family precedent — `sync_parent` is a
documented no-op on Windows because "Windows durability is the write-through
open plus the anchor barrier in P5" (freeze §4.1 P2 row;
`platform.rs:372-377`). The leaf's writer wrote it through `durable_write_options`
(write-through on Windows) and E10's scheduled barrier is the P5 anchor
round-trip, so a `cfg(windows)` no-op handle-flush arm with that citation
preserves the durability argument without demanding write access. Annotate the
freeze §4.3 E9 win cell in the settling update, and keep the durable-route
tests un-gated so the Windows matrix exercises the corrected arm. (Rejected
alternative: opening the leaf with write access on Windows — it widens the
observation's access demand and fails on genuinely read-only artifacts.)

### [P3-1] Second subsumed check, undocumented: the post-barrier `revalidated != first` comparison

`leaf_observation.rs:223-225`:

```rust
    if revalidated != first {
        return Ok(DurableLeafProof::Other(LeafOther::ContentMismatch));
    }
```

`compare_payload` returns `Exact` only after enforcing
`fingerprint.length == content.len()` (`:501`) and
`fingerprint.sha256 == content.sha256()` (`:504`), so for any `Content` whose
`len()`/`sha256()` are stable across the two calls, `first` and `revalidated`
are both pinned to `(content.len(), content.sha256())` and the comparison is
statically unreachable. Probe (replicating the implementer's own method for
the length check): with **both** the `:212-214` length refusal and this
refusal neutered (hits and reads retained), the full leaf suite still passes
19/0 — both are non-detecting alone. The length check carries the corrected
comment (`:200-205`, accurate: constant-work refusal + syscall diversity —
right to keep); this one carries none. **Remediation:** one comment in the
same shape (defensive against a `Content` whose declarations drift between
calls; subsumed by `compare_payload`'s trailing checks otherwise), or delete
the two lines.

### [P3-2] Post-barrier reobservation flattens typed kind/mode facts to `NameChanged`

`leaf_observation.rs:184-186`:

```rust
    let OpenedLeafV1::Exact(named) = named else {
        return Ok(DurableLeafProof::Other(LeafOther::NameChanged));
    };
```

`open_leaf` returns typed `Other(WrongKind | Executable | Substituted)` facts,
and the pre-barrier route reports them precisely (tested on both routes), but
across the barrier a kind/mode/symlink swap collapses to `NameChanged`.
Refusal-correct, precision-lossy, and slightly misleading for the E11 evidence
story ("the post-barrier reobservation must name the fact that moved").
**Remediation:** a three-arm match passing `Other(other)` through and
reserving `NameChanged` for `Absent`.

### [P3-3] Checker's protected pre_catalog tree pin is left failing; re-pin owed at the settle

On the isolated tree (base `c13f773` + only this object) the boundary checker
reports exactly one finding: `protected source tree changed:
checked_artifact/capability/pre_catalog.rs` — and on the **pristine** base it
reports none, so the finding is caused entirely by the object's new files in
the protected subtree (`source_tree_digest` hashes the root file plus every
descendant; new files fail closed by design,
`check_checked_artifact_boundaries.py:148-151/:753-767`). This matches the
implementer's disclosure. A 2.1-local re-pin would be wrong: Step 2.2's
`namespace_mutation.rs` lives in the same protected subtree, so only the
multi-lane settling package can compute the final digest. **Remediation:**
none in this object; the settling package must update
`PROTECTED_SOURCE_TREE_DIGESTS["checked_artifact/capability/pre_catalog.rs"]`
and return the checker to green — recording it here so the debt is tracked,
not discovered.

## Attack results (all other axes clean)

1. **Boundedness.** Both routes are caller-budgeted: `observe_bounded` does a
   fallible `try_reserve_exact(max_bytes)` (`:81`) then
   `file.take(budget).read_to_end` with `budget = max_bytes + 1` via
   `checked_add` (`:513-520`, overflow is a typed refusal), refusing
   `> max_bytes` as a length fact (`:455-457`); the durable route materialises
   nothing — two fixed 8 KiB stack windows (`:481-482`) against
   `take(content.len() + 1)`, with the expectation reader consumed at most
   budget + one window. No other `read_to_end`, no `read_to_string`, no
   `fs::read`, no `with_capacity`, no unsafe (grep-verified); the test file
   pins the shape by source scrape (`tests_leaf_observation.rs:360-373`).
   §8.10 posture: `RetainedLeafV1`/`OpenedLeafV1` are module-private, no
   handle, `Dir`, or path string escapes the module; the test-only capability
   issuer is `namespace::test_support`, `#[cfg(test)]` on the live tree.
   The allocation-failure capability tag follows the established provider
   idiom exactly (`interior.rs:148-153/:613-618`,
   `admission_mutation.rs:457-462`).
2. **One-handle discipline.** One `File` is opened before the proof
   (`open_retained_leaf`) and owned across proof → flush → barrier →
   reobservation; the post-barrier open at `:181` is the **name** cross-check
   only (its handle is dropped; only its identity is compared), while length
   (`:206-209`) and the second streamed comparison (`:219`) read the retained
   handle — live `fstat`/`metadata`/stream re-derivations, not caches. The
   expectation's two `open()`s are the content source (a caller-supplied
   reader, distinct from the leaf handle by construction); `opens() == 2` plus
   `crossed() == 1` is asserted, and the substitution/rewrite tests prove the
   post-barrier facts really come from the retained handle and fresh name
   resolution respectively. The encoded-pair compare (`:192-193`) refuses
   same-name substitution even under durable-id reuse.
3. **Two-sided absence.** Absent (first open, `NotFound`) → scheduled barrier
   → parent identity recheck → absent again through the same retained parent
   handle (`openat` semantics pin the directory *object*; the
   `dir_identity` compare additionally binds the capability's issued identity,
   with real discriminating power — the foreign-parent test). A leaf
   created-and-removed strictly between the two sides is not excluded — but no
   two-point protocol excludes it, the frozen contract asks exactly for the
   two-sided proof (`leaf.rs:38-46`, checkpoint §8 :234-235), and the appears-
   across-the-barrier case is tested to refuse. Contradicted expectations are
   typed (`NameChanged`) before any durable edge, asserted by
   `namespace.crossed() == 0`.
4. **Injection sites.** 11 keys across 10 textual sites (the `ContentPhaseV1`
   two-keys-one-announce at `:437` is the tenth), every site placed **after**
   its edge per the admission idiom — verified line-by-line
   (`:172, :183, :195, :211, :256, :280, :290, :315, :317-319, :437`). The
   phase cannot silently mix up keys: a wrong phase would make the
   `content_revalidate` matrix row's "fault point was not reached" assertion
   fire. Key names map 1:1 onto the pre-existing `fault_v1.rs:121-131`
   variants and the pre-existing key list (`fault_expected_keys.rs:94-104`);
   `EXPECTED_KEY_COUNT` stays 165 — **no key minted**. E8-E11 stand on
   P2/P3/P5 (plus a name-probe negative arm cheaper than E11's allotted P4 —
   using less than the frozen allotment, no new primitive): zero
   `publish_verified_no_replace` references in the object, zero direct
   `private_barrier`/`sync_parent` calls (the barrier is only
   `namespace.barrier(...)`), and the checker's pinned publication caller set
   holds on the isolated tree.
5. **Matrix substance.** 28 executed interruption rows (14 boundaries × both
   target variants: 10 exact-arm + `missing_revalidate` and the three shared
   boundaries on the absence arm — exactly the fixture comment's claim) plus
   2 keys × 12 rounds × 2 variants repeated crashes, against a real admitted
   action directory reached through the sealed catalog owner and the Phase 1
   admission owner. Each row asserts the interruption genuinely fired, the
   restart re-derives the **identical** proof (`Eq` over identity, length,
   sha256), the action-root **and** catalog-root censuses are unchanged, and a
   third settle still changes nothing; `reconcile_executed_keys` fails any
   family key lacking a row. The transcript
   (`matrix-evidence.txt`, 28 + 4 lines) matches the printed forms exactly,
   and I re-executed all four matrix tests green on the isolated tree and the
   live tree. **The documented deviation is sound and correctly scoped:**
   per-attempt fresh recovery without re-admission
   (`tests_leaf_fault_matrix.rs:210-215`) because the §7 exactness predicate
   (`interior.rs:497-500`) refuses a mid-life action, with the retained-parent
   capability re-minted through the `#[cfg(test)]` issuer — the admission
   handoff's directory identity is proven against the live directory once at
   `prepare` (`:308-312`), the object under test (the observer) re-derives
   everything per attempt, and the parts not exercised are exactly the parts
   the plan assigns downstream: the production caller to Step 2.4 (the
   `provider.rs` `dead_code` reason), execution wiring to Step 3.3 ("wires
   machinery; it does not convert consumers"), and mid-life resume to R2-E.
   It does not hollow the restart evidence for what Step 2.1 must prove.
6. **The confession.** Verified by my own probe (above, P3-1): the
   metadata-length check is indeed non-detecting alone and its corrected
   comment is accurate; keeping it is right. One further subsumed-and-
   undocumented check found → P3-1. The trailing `:501/:504` checks in
   `compare_payload` are **not** redundant (they bind the returned fingerprint
   to the expectation's declarations) and were left alone.
7. **Gates.** Frozen seam: `leaf.rs` byte-identical to `c13f773`
   (SHA-256 `4193c06f…` both sides). Isolated tree (base `c13f773` via
   `git archive` — no git write ops — plus exactly the object; the 2.2 hunks
   of `provider.rs`/`fault_expected_keys.rs` reverted):
   `cargo test --lib 'checked_artifact::'` → **290 passed; 0 failed**
   (replicates the implementer's number); checker → **only** the pre_catalog
   tree finding (P3-3; pristine base is green, so nothing else in the object
   trips it). Live tree: the object's 19 leaf tests pass; the checker's
   additional live findings (`platform.rs`, `workspace_ops/*`, `catalog.rs`,
   `v1_lifecycle/*`, publication seam) all name other lanes' files, none in
   this object.

## Verification appendix (commands, this host, 2026-08-22)

```
git -C gwz-core show c13f773:src/checked_artifact/leaf.rs | shasum -a 256   # == live file
# isolated tree: git -C gwz-core archive c13f773 | tar -x -C <scratch>/iso/gwz-core
#   + the three new provider files, + 2.1-only provider.rs hunks,
#   + fault_expected_keys.rs with the 2.2 hunks reverted,
#   + workspace wrapper replicating the taut-shape path patch
cargo test --offline -p gwz-core --lib 'checked_artifact::'                 # iso: ok. 290 passed; 0 failed (19.11s)
python3.13 scripts/checks/check_checked_artifact_boundaries.py --source src # iso: 1 finding (pre_catalog tree)
python3.13 …check_checked_artifact_boundaries.py --source src               # pristine base: ok, 0 findings
python3.13 …check_checked_artifact_boundaries.py --source src               # live: findings in other lanes' files only
cargo test --offline -p gwz-core --lib '…provider::tests_leaf'              # live: ok. 19 passed; 0 failed
# probe: neuter :212-214 and :223-225 refusals (hits retained) on the iso copy
cargo test --offline -p gwz-core --lib '…provider::tests_leaf'              # probe: ok. 19 passed (both checks non-detecting)
```

---

# Focused re-verdict (round 2) — remediation against the round-1 findings

- **Date:** 2026-08-22 (same day, after remediation).
- **Reviewer:** same single-axis Code-lead; scope per the coordinator's
  request: **my round-1 findings only**.
- **Object delta:** base moved to `ae4e143` ("Land the ARM64 EBADF substrate
  package", after the Phase 1 settle remediation `bf438ed`); the Step 2.1
  object is still working-tree. Since round 1, only `leaf_observation.rs`
  (539→663 lines) and `tests_leaf_observation.rs` (794→844) changed —
  verified structurally, not by trusting the claim:
  `tests_leaf_fault_matrix.rs` is 497 lines with its round-1 anchors intact,
  the `provider.rs` 2.1 hunks and the `fault_expected_keys.rs` `durable_leaf`
  region are textually identical to the round-1 quotes, and the test-file
  delta is exactly one inserted block (every pre-insertion test sits at its
  round-1 line number; everything after is shifted +50). **Evidence-hygiene
  note:** my round-1 scratchpad `iso/` snapshots were found matching the
  *current* files (the tree was evidently reused for the implementer's
  overlay work), so they were discarded as baselines; all round-2
  verification is re-derived from the live tree and a fresh `git archive` at
  `ae4e143`.

## Per-finding disposition

- **[P1-1] — RESOLVED.** `leaf_observation.rs:172` now calls
  `flush_observed_leaf(&file)?` (the `DurableLeafFileFlush` hit stays after
  the edge, both platforms). `#[cfg(not(windows))]` (`:291-300`) performs the
  real `sync_all` on the read-only handle, with the correct note that this
  regular-file descriptor sits outside the Linux `O_PATH`/`EBADF` substrate
  `ae4e143` repairs; `#[cfg(windows)]` (`:302-322`) is a documented no-op in
  exactly the shape my round-1 remediation prescribed (the `sync_parent`
  P2-family precedent, `platform.rs:393-397`), and the rejected alternative —
  widening the open to write access — is stated with my round-1 reason
  (`:287-290`: genuinely read-only artifacts must remain observable). The new
  `platform_tests` module (`:601-663`) pins both arms on a handle opened
  exactly as `open_leaf` opens it: the unix test asserts the flush executes;
  the windows test asserts the arm's `Ok` **and** that a raw `sync_all` on the
  same handle fails with `raw_os_error == Some(5)` — so if `FlushFileBuffers`
  ever accepts read-only handles the justification cannot silently go stale.
  I replicated the implementer's overlay probe on a fresh isolated tree:
  flipping the four cfg predicates compiles cleanly (the Windows bodies
  type-check) and fails exactly one test at exactly `:654`'s `expect_err` on
  macOS (where `fsync` succeeds on a read-only fd) — the canary is
  non-vacuous. The round-1 gate-breaking mechanism is gone: the durable route
  no longer raw-syncs anywhere, so the un-gated durable-route tests no longer
  have a structural Windows failure, and the one windows-gated test is the
  canary itself.

- **[P3-1] — RESOLVED.** The post-barrier fingerprint comparison now carries
  the confession-shaped comment (`:232-240`), matching my round-1 static
  analysis (subsumed for a `Content` with stable declarations) and adding the
  correct sharper reason to keep it: a `Content` whose `len()`/`sha256()`
  drift between its two `open()` calls passes each stream's own pin while
  describing different payloads, and only the fingerprint comparison catches
  that.

- **[P3-2] — RESOLVED.** The `:184-186` let-else is now a three-arm match
  (`:191-195`): typed kind/mode/link facts pass through, `NameChanged` is
  reserved for a name that stopped resolving, symmetric with the absence arm
  (`:275-279`). Pinned by the new test
  `a_kind_or_mode_swap_across_the_barrier_keeps_its_typed_fact`
  (`tests_leaf_observation.rs:516-559`), which injects directory/symlink/
  exec-bit swaps at the barrier boundary and asserts
  `WrongKind`/`Substituted`/`Executable` — a test the round-1 code would have
  failed.

- **[P3-3] — UNRESOLVED BY DESIGN (tracked, not owed by this object).** The
  round-2 isolated checker still reports exactly the one finding
  (`protected source tree changed: …pre_catalog.rs`); the re-pin remains the
  multi-lane settling package's, as round 1 recorded. The freeze §4.3 E9 win
  cell still reads `sync_all` — its annotation rides the same settling
  update, per the round-1 remediation text ("annotate … in the settling
  update"); noted here so both residues travel together.

## Is the Windows-arm property statement code-true?

Yes. Each factual clause of `:302-322` is verified against committed code at
`ae4e143`, not taken from the comment: the leaf's writers open through
`durable_write_options`, whose `cfg(windows)` block really sets
`FILE_FLAG_WRITE_THROUGH` (`directory_mutation.rs:711-716`), so owner-written
bytes are through the cache before an observation begins; the ordering the
observer still owes is E10's scheduled barrier, which the observer only ever
crosses through the protocol (`cross_namespace_barrier`), and on Windows that
is the P5 anchor round-trip the freeze §4.1 row and `platform.rs` anchor
internals define; and the constraint that forces the no-op — read-only handle,
`FlushFileBuffers` wants `GENERIC_WRITE` — is *pinned by an executable canary*
rather than asserted prose. The statement is also honestly scoped: it
conditions on "the leaf's **writer**" being the P2 write-through path, which
is precisely the conditional the adopted P2 row already states for
`sync_parent` ("Windows durability is the write-through open plus the anchor
barrier in P5"). The residual — an out-of-model writer's unflushed bytes would
not be ordered by the no-op arm, where the unix arm's real `fsync` would order
them — is the P2 family's accepted platform posture, not a new weakening
introduced here, and such writers sit outside the sealed-catalog ownership
model this observer serves. One asymmetry is worth naming for the record: on
unix E9 is enforced by a syscall, on Windows by an argument plus a canary;
that is the strongest posture available to a read-only observer on Windows,
and the canary converts the argument's load-bearing premise into a test.

## Round-2 verification appendix

```
git -C gwz-core log --oneline -3            # ae4e143 (ARM64 landed), bf438ed (Phase 1 settle), c13f773
# fresh isolated tree: git archive ae4e143 + the three object files
#   + 2.1-only provider.rs hunks + fault_expected_keys.rs with 2.2 hunks reverted
cargo test --offline -p gwz-core --lib 'checked_artifact::'   # iso2: ok. 295 passed; 0 failed (15.73s)
python3.13 …check_checked_artifact_boundaries.py --source src # iso2: 1 finding (pre_catalog tree) — P3-3 posture
# flip probe on the iso2 copy: swap the 4 cfg predicates
cargo test --offline … 'leaf_observation::platform_tests'     # compiles; FAILED 1: expect_err at :654 (non-vacuity shown)
# faithful copy restored
cargo test --offline … 'leaf_observation::platform_tests'     # iso2: ok. 1 passed
cargo test --offline -p gwz-core --lib 'provider::tests_leaf' # live: ok. 20 passed; 0 failed
cargo test --offline … 'leaf_observation::platform_tests'     # live: ok. 1 passed
```

## Round-2 verdict

**GO.** All three findings owed by this object are resolved ([P1-1], [P3-1],
[P3-2]); [P3-3] remains open by design as the settling package's tracked duty,
together with the freeze §4.3 E9 win-cell annotation. Zero P0/P1/P2 remain —
no escalation from this round; the round-1 escalation stands satisfied by this
focused re-review of the remediation.
