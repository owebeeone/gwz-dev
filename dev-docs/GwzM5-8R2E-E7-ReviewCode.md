# GwzM5-8 R2-E E7.1 — settled-gate review, CODE axis (dual #2, peer-blind)

- **Axis:** CODE (one of two peer-blind axes; the State axis files separately —
  nothing here was coordinated with it or read from it).
- **Date:** 2026-08-29.
- **Object:** gwz-core `main` at **`be693bd`** (tag `v0.11.1`,
  `chore(release): gwz-core 0.11.1` — Cargo.toml/Cargo.lock only over
  `cc7c625`).
- **Worktree:** detached at
  `/private/tmp/claude-501/-Users-owebeeone-limbo/3a4f2794-661b-40df-b586-1f52221017f3/scratchpad/e7-code-wt`;
  `git rev-parse HEAD` = `be693bdebbecd8208ffc61f3343f8185c06f7184` — **matches
  the object commit**. All probe edits were made in this worktree only and every
  one was reverted; `git status --porcelain` is empty at filing. No commit, push
  or tag was made anywhere.
- **Charter honoured:** Phase E4's absence (the §10 consumer conversion,
  O1/O2/O3/O13-substantive) is EXPECTED state per the operator's ruling verbatim
  "a" (2026-08-27) — it is re-scheduled after R2-F's relocation and is **not**
  graded here. Tier-2 archive equivalence is deliberately unexecuted (its
  comparable sub-surface deliberately unminted; see Q-section note). The
  conf-integrity lane is separately reviewed (GwzConfIntegrity-Review.md /
  -Review-2.md) and is not re-reviewed; its seams into the surfaces below were
  checked where touched (it adds nothing inside `src/checked_artifact/` and
  moves no census/inventory pin — its +42 tests sit inside the lib-remainder
  partition, which I re-measured).

## VERDICT: **GO-WITH-CONDITIONS**

The tree is sound as landed: all 38 re-reserved keys are bound with the
semantics the freeze and the controlling amendment pair record, the three
matrices execute on both target variants, the O6 witness closes the
caller-restatement class on both the mint and the read side, the T1 widening is
confined to exactly its three gates with the E3 [P1] cure structurally intact,
the E6 riders are as recorded, and all five machine-enforced inventories match
the tree and **fail closed under my own perturbation probes**. No P0, no P1, no
P2. Four P3 findings, all record-class.

The two conditions are dispositions the E7.2 acceptance record must carry —
both docs-only, no code owed on this tree:

- **(C1)** The ledger's O12 anchor-nit-1 row closes **re-owned with a named
  carrier** carrying this review's Q1 recommendation (bound the shared reader —
  the concrete shape is in Q1 below). The Phase-E6 plan annotation promised the
  review-debt ledger empties at E7; this row cannot close silently or stay
  unshaped.
- **(C2)** The Q2 disposition lands as a dated annotation: the freeze §3.5
  `barrier.*` activation record gains the named-exception sentence for the P5
  roaming recovery rename (the E2 review's [R2-P3-2] second branch — see Q2),
  since the first branch (a seventeenth key) is refused here on substance.

---

## 1. Findings

### [P3 F1] — the barrier family's announcement claims are true only on the weak reading (Q2's surface)

Two in-code claims sit in tension with one durable edge:

- `barrier_mutation.rs:19` (module header): *"every durable barrier edge is
  announced from here"*.
- `converge_target_anchor_alias`'s doc (`barrier_mutation.rs:494-498`): *"No
  `barrier.*` boundary is announced here, and none is owed. The return rename is
  inside primitive family P5's own recovery surface, **exactly like** the
  resident protocol's `AnchorState::NeedsReturn` return…"*.

The converge return rename (`platform.rs:506-521`, row three of
`prepare_roaming_target`'s table) is a real durable no-replace rename on the
barrier drive path, announced by no key and physically performed in
`platform.rs`, not in the file the header names. The cited precedent is
imperfect in exactly the way the E2 round-2 review recorded: `NeedsReturn`'s
return runs *inside* `private_barrier`, under key #8's umbrella ("key #8 names
'`private_barrier` has returned'"); the converge runs *before* the barrier and,
on the `Stranded` path, on a drive where `private_barrier` is never called at
all. The header's sentence is true as "all `barrier.*` announcements live in
this file" and false as "every durable edge reachable on a barrier drive has an
announcement". No behaviour is wrong; all sixteen announced keys perform what
their rows record (re-verified — §2.4 below). Cure is C2's dated sentence plus,
optionally, one qualifying clause at `barrier_mutation.rs:19`. Full
determination at Q2.

### [P3 F2] — [R2-P3-3]'s landing-optional wording is still on the tree

`platform.rs:489` (as of be693bd) still reads *"the survey is two
`symlink_metadata` calls"*; `leaf_is_resident` (`platform.rs:529-534`) is a full
bounded-shape leaf observation through `observe_leaf_exact` (stat, no-follow
open, identity compare, full read, re-stat). The E2 round-2 disposition was
"fold it into whichever commit next touches `platform.rs`" — no later commit
touched that file (E6's two platform-**tree** digest moves were
`platform/anchor.rs` and `platform/anchor/tests.rs` edits), so the trigger never
fired. Not a defect; re-recorded so the one-word fix ("two bounded leaf
observations") does not evaporate. Fold into the same train as C1/Q1, which
edits the function this sentence mis-describes the cost of.

### [P3 F3] — residue's family-byte gate charges after the read it means to bound

`residue.rs:19` pins `MAX_FAMILY_BYTES = 1 MiB`, but the gate at
`residue.rs:145-150` runs `observe_leaf_exact(...)` first and charges
`bytes.len()` of the **already-read** allocation; a large foreign object under a
`ca1-` family name is fully read (or dies in allocation) before the 1 MiB
intent can classify it foreign. This is the survey-side consequence of the Q1
unbounded reader and is cured to allocation-bounded-by-file-size by the Q1
recommendation; a further hardening — moving the family gate to stat level
(sum `entry.metadata().len()` before any read) — is a natural rider for
whichever E4-era train reworks `residue.rs`. Recorded here so Q1's cure and
this gate's reordering are seen as one class.

### [P3 F4] — "converged ⇏ flushed" and "settled ⇏ barriered" are sibling consumer-contract clauses; E4 must carry both (Q4's disposition)

The terminal converged-restart path (`completed.rs:247-279`) returns `Ok(())`
on `retired_resident` without key #8's retired-root flush or key #9's
catalog-root barrier — fully disclosed at the site with both corpus precedents,
the reachable-state enumeration, and the both-parents refusal. The barrier
family's record already carries the matching clause ([R2-P3-1]: a settled
ordinal does not prove its target parent's dirents were ordered). My Q4 ruling
(below) accepts the idiom as landed; the residue is that **E4's first consumers
must not assume either implication**, and only the barrier half is recorded as
an E4 scope clause today. Disposition: when E4 resumes, its gate note (plan §3
Phase E4 preamble, already the home of E2.2-ordering and the seven
preconditions) gains the terminal sibling clause in one sentence. No code owed.

---

## 2. Mandate 1 — the settled-tree verification record

All lines below were read in the worktree at `be693bd`; all counts were
measured there (commands and outputs in the appendix).

### 2.1 Census and activation vocabulary

- `EXPECTED_KEY_COUNT = 165` (`fault_expected_keys.rs:174`), stable-key list
  ends `:172`; the 38 R2-E keys present in the frozen order — `barrier.*` 16
  (`:105-120`), `cleanup.*` 11 (`:151-161`), `terminal.*` 11 (`:162-172`).
- Activation rows: `cleanup` → `Executed("R2-E phase E1 …")` (`:409`),
  `barrier` → `Executed("R2-E phase E2 …")` (`:429`), `terminal` →
  `PartiallyExecuted(…, TERMINAL_EXECUTED_KEYS)` (`:454-458`) with
  `TERMINAL_EXECUTED_KEYS.len() == 10` asserted (`:957`) and the T-D
  determination doc (`:446-451`, `:466-476`).
- **The `Reserved` arm is unconstructed** and annotated exactly as the
  checkpoint records: `#[allow(dead_code, reason = "the frozen activation
  vocabulary's reserved arm; unconstructed since the R2-E E1-E3 landings,
  retained for the map's carried re-reservation clause")]`
  (`fault_expected_keys.rs:219-223`), with the retained-vocabulary ground in
  the doc above it (`:210-218`).

### 2.2 Key bindings vs the freeze's semantics

- **cleanup.*** — exactly 11 `CheckedArtifactFaultKeyV1::Cleanup*` references
  in `namespace_mutation.rs`, 11 distinct variants, zero elsewhere (grep
  appendix). Spot-checked #10 `row_complete` against the frozen classifier:
  `bound.classify(index, &source, &destination) != Some(CleanupResolutionV1::Complete)`
  refuses (`namespace_mutation.rs:946`). The E1 interior review's row-by-row
  (GO, six P3) stands; nothing on the settled tree contradicts it.
- **barrier.*** — exactly 16 distinct `Barrier*` variants, all in
  `barrier_mutation.rs`, zero elsewhere. The E2 review's sixteen-row table
  verified against the addendum §1.5 rows stands; I re-walked keys #6/#8/#16 on
  the settled tree (fresh-copy `create_new` alias with P2 family writes at
  `barrier_mutation.rs:531-545`; `DirentBarrierClass::RoamingAnchoredTarget`
  dispatching `anchor::round_trip_supplied` at `platform.rs:704-711`;
  completion's two-sided reobservation) — sound.
- **terminal.*** — 10 sites split by capability per T-C′: 5 in
  `namespace_mutation.rs` (#1-#5, `observe_terminal_preconditions`,
  `:349-454`), 5 in `admission_mutation.rs` (#6-#10);
  `TerminalAuthorityRelease` names **zero** sites tree-wide (T-D held). The one
  T-C′ forward is `RetainedCompletedCatalogV1::retired_root`
  (`completed.rs:226-228`) — no visibility modifier, private to its file, the
  `&Dir` crossing only as a call argument, with the corrected two-half contract
  citation from the E3 round-2 [P3 F4] cure in place (`completed.rs:209-225`).

### 2.3 Injection machinery

- Matrices: `CLEANUP_MATRIX: [Fault; 11]` (+8 repeated / 3 single-crossing),
  `BARRIER_MATRIX: [Fault; 16]` (+6/10), `TERMINAL_MATRIX: [Fault; 10]`
  (+3 repeated / 4 single-crossing, `REPEATED_CRASH_ROUNDS = 12`), each with
  `reconcile_executed_keys()` set-equality against the vocabulary and both
  target-variant drivers
  (`…matrix_on_a_workspace_target` / `…on_a_git_directory_target`), all green
  inside the 447 partition.
- The E3 F6 cure is live on the tree: `Fixture::admit` returns the real
  `AdmittedActionV1` from `resume_or_admit`; the test-only issuer appears in
  `tests_terminal_fault_matrix.rs` only inside prose.

### 2.4 The O6 witness (mint side + read side)

- `RoamingAnchorHomeWitnessV1` (`barrier_mutation.rs:184-244`): fields private,
  **no** `Clone`/`Default`/`From`/serde; `owner_mint`
  `pub(in …capability::pre_catalog)` with the home *name* derived from
  `InfrastructureSlotV1::RoamingAnchorHome.name()`, not a parameter; exactly
  **one** production call site tree-wide (`completed.rs:180`, inside
  `observe_roaming_anchor_home`); `test_only` is `#[cfg(test)]`.
- Read side: `read_and_bind_barrier_intent(…, home: &RoamingAnchorHomeWitnessV1)`
  (`protocol/barrier.rs:297`) refuses typed on any of the three identity facts
  disagreeing (`:311-317`, "barrier intent does not match the observed roaming
  anchor home"), **after** the five existing checks; `issue` (`:66-70`) and
  `test_issue` (`:236-239`) both take the witness — no caller-supplied identity
  route exists. The refusal-arm mint (`CompletedCatalogPermitV1::
  observe_roaming_anchor_home` = `revalidate()?` then mint) matches §5.4.

### 2.5 The T1 widening — exactly three gates

Tree-wide `EmptyDirectory` census (appendix): declaration (`provider.rs:240`),
producer (`interior.rs:501`), gate 2 (`completed.rs:455`,
`retain_directory`), gate 3 (`completed.rs:541`,
`require_named_directory_identity`), `staging_plan`'s preserved-by-name arm
(`interior.rs:238`), helpers (`interior.rs:965/:987`), framing
(`snapshot.rs:210`) — no fourth gate. Gate 1 is `completed_record` via
`retired_root_identity` (`interior.rs:377` → `:961-973`,
`unaccepted_rows == 0 && retired_action_dirs <= MAX_RETIRED_ACTION_DIRS`).

### 2.6 E6 riders as recorded

- **O9**: the two `#[cfg(unix)]` tests
  (`a1_activation.rs:411-514` composed + control arms; `:522-555` isolation
  guard pinning `ErrorCode::IoError`, "nothing was published"). The 8-byte
  window is **machine-held**, not described: three asserts at `:422-424` pin
  upgrade-shortest > 255 ≥ store-longest and the record name inside the cap; the
  id is supplied at the start through the `cfg(test)` `IdProvider`
  (`invoke_with_store_and_merge_id`, `:335-375`) with the GWZ-Merge-ID-trailer
  ground documented; `AtomicUpgradeFault::None` untouched (`:546`). I re-derived
  the window arithmetic independently (store `id+pid+seq+11`, upgrade `+19`, at
  `id = 236 − pid`): it holds for 1-8 seq digits, and the asserts encode exactly
  those endpoints.
- **O10**: the four injected variants are `#[cfg(test)]`
  (`store/atomic_upgrade.rs:36-46`); the injection checks themselves are
  `#[cfg(test)]` blocks (`:113-151`); production names only
  `AtomicUpgradeFault::None`, at exactly one site
  (`runtime/dispatch.rs:411`). Production compiles no injected constructor —
  the negative-build probe (E0599) is structurally implied and was reproduced
  twice upstream (builder + E6 review); not re-run here.
- **Anchor nit 2**: `survey`'s retired arm re-renders —
  `retired_name(ordinal) != *text → AnchorState::Invalid`
  (`platform/anchor.rs:467-472`) — with the F-3 trade sentence in the comment
  (`:458-466`) and the F-1 grammar-table row corrected to
  `.ca1-anchor-retired-<ordinal>` (`:51`). The F-2 whole-listing-unchanged
  assertion shape is present in `anchor/tests.rs` (`:142-144`).

### 2.7 The five machine-enforced inventories — matched and fail-closed

All five were verified in both directions: green on the pristine tree, RED
under a perturbation probe (probe transcripts in the appendix).

1. **`PROTECTED_SOURCE_TREE_DIGESTS`** (7 trees, incl. the `catalog.rs` tree,
   checker `:185-193`): checker recomputes and passes on pristine → every pin
   matches the tree at `be693bd`. Probe A (one appended comment byte in
   `platform.rs`) → RED "protected source tree changed". **Re-pin archaeology
   executed** across `8597d32..be693bd` (appendix): every R2-E commit's digest
   movement enumerated; the two flagged deliberate lowerings are E2's landing
   (`c11c5ef` — `platform.rs` first moves, with `pre_catalog.rs`/`catalog.rs`
   re-executed on the union tree) and E6 (`a593dbd` and the `afbc25d`
   reconcile — **each moved exactly one digest, both `platform.rs`**), exactly
   as the checkpoint records. Conf-integrity commits touched no digest.
2. **`CATALOG_PUBLICATION_CALL_COUNTS`** (checker `:773-…`): dict sums to
   **14** across six files (1+5+3+1+2+2) with the fifth dated extension
   (terminal retire, 2→3 at `admission_mutation.rs`) and E2's banked negative.
   Probe C (dict value 1→2) → RED "catalog publication seam changed".
3. **The `capability_permit` Rust twin**: `assert_eq!(…count(), 14)`
   (`interface_tests/capability_permit.rs:158`) — green in the measured
   partition; equals the Python sum.
4. **`V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`** (checker `:288-294`, the O13
   pin, three files): probe B (new `v1_lifecycle/probe_writer.rs` naming
   `durable_fs`) → RED "v1 lifecycle gained a raw durable_fs writer outside the
   O13 accepted residual" (the v1_lifecycle tree digest fired too — redundant
   coverage). The accepted-residual set is unchanged (shrink-only, awaiting
   E4.2/E4.3 per the ruling).
5. **`FAULT_INJECTION_SOURCES` len()==10 + completeness rescan**
   (`fault_expected_keys.rs:854-880`): ten declared files on the tree (the
   single R2-E move at E2 with `barrier_mutation.rs`, matching freeze
   `:525-538`'s in-place-edited addendum and the `:1043-1052` E0 annotation).
   Probe D (a `cfg(test)` fault-key reference appended to
   `capability/collision.rs`, an undeclared production source) →
   `the_declared_injection_sources_are_every_production_source_holding_sites`
   **FAILED** with the drift message. Reverted; suite green again.

### 2.8 Gates, wire, and pins

- `cargo fmt --all -- --check` → exit 0. `cargo check --all-targets -p
  gwz-core` → exit 0. `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets -- -D
  warnings` → exit 0.
- Partitions, direct exits: `checked_artifact::` **447/0** ·
  remainder (`--skip checked_artifact:: --skip …v1_lifecycle::`) **979/0 + 1
  ignored** · `v1_lifecycle::` (skip root_fault_matrix) **256/0** — all three
  equal to the driver's darwin pins (`run_r4bg_aggregate_gates.py` `:260-264`,
  `:241`). The root-fault-matrix release leg was not re-run here; it is
  EXECUTED on linux at this very commit by the release verify
  (run 33196574973→33196576270: 457/980/256/1), which also discharges the
  derived linux markers — E7.2 should cite that run per the checkpoint.
- Boundary checker: `ok (15 visible entries, 5 classified modules)`, exit 0.
  Compatibility checker: `validated 7 migration rules, 7 runtime bindings, and
  10 archive shapes`, exit 0; its unittest suite **27/27 OK** (the E5 [P3-3]
  weaken-and-raise rows included).
- Wire: `git diff 8597d32..be693bd -- …protocol/generated.rs` is **empty** —
  the plan §4 zero-diff non-goal held across the whole R2-E span.
- E5 registry surfaces (code side): `archive_corpus` = 10 rows = **8
  tier-1 `byte_preserved` + exactly {AC-NOPUB-UNBORN, AP-PRESERVED}
  PENDING-FIXTURE** — the E0.2b §8 denominators as machine-checked;
  `valid_unlisted_corpus` = 15 rows total (the 9 E5.1 additions ride the
  parametric `adapt_open` refusal test; row-fidelity vs the Evidence doc is the
  State axis's surface).

---

## 3. Mandate 2 — second-axis rulings on the interior escalations

**(a) E3 [P1 F1] — the observe_slot↔observe re-entry. CURE RATIFIED,
structurally and by my own boundary probe.** On the settled tree `observe(` is
called **nowhere** except its own definition (`interior.rs:68`); `observe_slot`
(`:457`) reaches the retired root only through `read_retired_root` (`:499`),
which is file-private with that single call site, performs **one**
`entries()` pass, charges the name budget, and refuses typed at
`entry_count() > MAX_RETIRED_ACTION_DIRS` **before** any classification
(`:590-596`), then duplicate-refuses on sorted digests (`:605-611`). No
recursion survives anywhere in the interior observers (`observe_action_interior`
and `observe_managed_component_interior` are flat loops). The driven rows are
on the tree (nested chain at depth 1024 on both variants; 65-past-the-bound
refusal). The round-2 reviewer's PROBE B (the at-the-bound side) was
reviewer-local, so I re-executed it independently: **my probe planted 64
action rows → recovery `Ok`; the 65th → typed refusal** (appendix; reverted).
No off-by-one in either direction. One deliberate note: the same
abort-instead-of-refusal *class* (by allocation rather than stack) is what Q1
addresses in the legacy shared reader — the two should be read as one
discipline.

**(b) E3 [P2 F2] — the forced terminal source-interior arm. SHAPE CONCURRED.**
Existence re-verified as forced from the type system: `PublicationSourceV1::
Directory` has no "no recheck" form, and none of the three prior
`DirectoryInteriorExpectationV1` variants can accept a lived-in action
directory (`CatalogStaging` runs `staging_plan`; `ManagedStaging` compares a
marker; `AdmissionStaging` requires `extra_children == 0`). The landed shape
(`TerminalActionDirectory`, `publication.rs:61-110`, dispatching
`is_reservation_exact` at `:273-276`) is the admission arm's own bounded reader
(`observe_action_interior`, `MAX_ACTION_SLOTS`-capped, flat) minus exactly the
clause false by construction. My second-axis judgment: the weakening is sound —
the extra children travel with the atomic rename into the retired root, where
the widened reading classifies only the top-level action row, so no downstream
predicate weakens; the recheck's in-window purpose (substitution detection) is
fully served by the identity compare plus reservation exactness; the narrower
conceivable arm (extra children == the completed row set) is correctly named in
the doc as not taken. The freeze §4.4 landing annotation (`:1543-1555`) is
present, dated, quotes the row verbatim, and the cross-reference is two-way as
promised. **Ratified.**

**(c) E3 [P2 F3] — rows #2/#3 fingerprint binding, code half. VERIFIED.**
Row #3: `require_completed_cleanup_worklist` supplies two observed
`CleanupPhysicalFactV1` per row and requires
`worklist.classify(index, &source, &destination) == Some(CleanupResolutionV1::Complete)`
(`namespace_mutation.rs:557-560`) — the frozen `protocol/cleanup.rs` rule,
uncopied; facts come through `HostPlatform.file_identity` with the stream
length-bounded by the **bound worklist row's own recorded field** (never the
object's), overrun and underrun both classifying to refusal. Row #2: the dated
DETERMINATION block sits at the site (`:377-394`) and its exact replacement
text is carried in freeze §3.5's terminal record (`:975-986`), with row #3's
strengthened form recorded beside it — landing condition R2.9(b) discharged.
The code and the record say the same sentence. **Concur.**

**(d) E2 [P2-1] — the Windows roundtrip-orphan converge arm. ARGUMENT VERIFIED
AGAINST THE CODE.** `prepare_roaming_target` (`platform.rs:493-524`) derives
the outbound name itself and owns both names; the match over
`(alias_resident, outbound_resident)` is three arms covering the `(bool,bool)`
domain — **compiler-exhaustive**, no wildcard hiding a state; row three
converges by verify → `publish_verified_leaf_no_replace` → verify (a rename,
never a removal — zero `remove_file`/`remove_dir`/`unlink` on the roaming
production path; the only `remove_dir_all` hits in `platform.rs` are test
fixtures). The entry decision is the drive's only question
(`converge_target_anchor_alias`, `barrier_mutation.rs:499-519`, preceding
`create_target_anchor_alias` in the only creating branch). The wedge argument's
load-bearing ground I re-derived independently: `ActionSlotV1::parse` returns
`Valid` only after `strip_suffix("-v1")` succeeds (`protocol/slots.rs:245-247`),
so a `.roundtrip` name classifies `RecognizedInvalid(UnsupportedVersion)` —
**never `Valid`** — and `require_reserved_target_leaf` (`namespace/host.rs:1073`)
admits only `Valid`, so no ordinal of any action can ever reserve a colliding
leaf. Row four's toleration is therefore bounded to pre-remediation trees,
grammatically unreachable as a slot, and census-pinned by the driven rows.
**The cure and the wedge argument are sound.**

**(e) E5 [P1-1]** — a records pin (the forward map's 43-named-tests value);
State axis's surface. Code-relevant halves checked here: the compatibility
checker and its suite are green on this tree (7/7/10, 27/27), and the archive
corpus's machine-enforced denominators hold (2.8 above). Nothing further from
this axis.

---

## 4. The E7-dual queue — determinations Q1-Q4

### Q1 — anchor nit 1: bound the observation read. **RECOMMENDATION: bound the SHARED READER itself; do not add a bounded entry.**

**The surface, measured on this tree.** The unbounded read is
`observation.rs:248-250` (`Vec::new()` + `read_to_end`), inside
`observe_leaf_exact`. Call census at `be693bd`: **18 call expressions** —
transition.rs ×7 (`:22/:31/:218/:313/:316/:357/:414`), residue.rs ×4
(`:145/:421/:571/:597`), platform.rs ×2 (`:531/:545` —
`leaf_is_resident`/`verify_leaf_bytes`), classification.rs ×1 (`:144`),
platform/anchor.rs ×1 (`:556`, `verify`), observation.rs ×3 internal
(`observe` `:134`, `observe_leaf_exact_current` `:152`, the `observe_leaf`
wrapper `:185`) — plus wrapper-mediated consumers (`cleanup.rs:78` via
`observe_leaf_exact_current`; every `CheckedArtifact::observe` caller). That is
the E6 review's "~21 across six modules", made exact.

**Why no constant can cap it — the fact that decides the design.** The reader
serves two irreconcilable caller classes: (i) fixed-expected verifiers (anchor
bytes, the 22-byte roaming alias, protocol records bounded by
`authority.rs:8`'s `MAX_RECORD_BYTES = 16 KiB`), and (ii) **user-artifact
content reads** — `transition.rs`'s detach observes the workspace artifact
itself and stages its full bytes (`transition.rs:269-300`), which are
legitimately arbitrary-sized. A family constant refuses legitimate user files;
a per-call cap parameter (the "bounded observation entry" option) forces all 18
sites to re-derive a bound, and every class-(ii) site can never do better than
"the file's own stat" — the option degenerates into the shared-reader cure with
18 copies of the plumbing, plus signature churn across exactly the files E4's
deferred consumer conversion will rework.

**The recommended shape** (platform.rs `:220-235`'s template — `try_reserve_exact`
+ `take(len+1)` — transplanted with the cap source changed from caller-truth to
the reader's own already-consistency-checked fstat):

- **Signature: unchanged.** `observe_leaf_exact(dir, leaf, code, label) ->
  ModelResult<LeafObservation>`. Zero call-site churn.
- **Cap source:** `opened.len()` — the post-open metadata already taken at
  `observation.rs:239-241` and already identity-checked against the pre-open
  stat at `:242`.
- **Mechanics at `:248-250`:** checked `usize` conversion of
  `opened.len() + 1` (failure ⇒ return `fact: Invalid` — the function's
  existing fact-level refusal vocabulary; a leaf larger than address space is
  not a canonical artifact); `bytes.try_reserve_exact(cap)` (failure ⇒ typed
  `ModelError` through the existing `io_op_error` shape, e.g. "read artifact
  bytes: allocation refused" — the template's refusal type, never an abort);
  `file.by_ref().take(cap as u64).read_to_end(&mut bytes)`.
- **No new refusal arm for overrun:** a file that grew past its stat reads
  `cap` bytes and fails the existing five-way check
  (`opened.len() != bytes.len()`, `:273`) ⇒ `Invalid` — today's arm, kept.
- **What it cures:** the OOM-abort class (infallible geometric `read_to_end`
  growth — the same abort-not-typed-refusal class as E3's [P1 F1], by
  allocation instead of stack) and unbounded reads of objects growing during
  the read. It also makes residue's 1 MiB intent (F3 above) allocation-bounded.
- **Accepted residual, stated:** a *stable* multi-GB foreign object still
  reserves its stat size fallibly and is refused typed only if reservation
  fails; verify-class callers accept exactly this residual at the publication
  template today. If the lane later wants cheap refusal of huge foreign files
  at fixed-expected sites, a stat-only survey entry on `LeafObservation` can
  ride the E4-era residue rework — out of this cure's scope.
- **Blast radius:** one function body in `observation.rs` (~10 lines), no
  signature drift, no wire/census/inventory movement, one protected-tree digest
  does **not** move (observation.rs is not a pinned tree); owed companions: the
  house not-inherited-bound doc sentence (the F1-cure precedent), and one or
  two rows extending `tests_leaf_observation.rs`'s changed-under-read coverage
  to pin the take-cap path.
- **Carrier (per condition C1):** the next production train touching
  `checked_artifact/` — E7.2 names it in the O12 row's close; this review
  supplies the shape so the carrier implements rather than re-decides. F2's
  one-word wording fix rides the same train.

### Q2 — [R2-P3-2], the unannounced return rename / 17th-key question. **DETERMINATION: no 17th key in substance; the record owes the named-exception sentence (condition C2).**

Is there a seventeenth `barrier.*` key in substance? **No.** The converge
return (row three) is one atomic no-replace rename, idempotent and
self-converging: a crash before it re-presents row three; a crash after it
presents row two; the seeded-state row
(`assert_mid_round_trip_residue_converges`) already asserts census-exactly
everything an interruption row could assert, on both variants. A key would move
the frozen sixteen-key §3.5 record and the 165 census to buy **zero** new
distinguishable crash-window evidence — ceremony against a frozen surface. The
matrix-completeness machinery (`reconcile_executed_keys`, the per-key siteless
scan) is not weakened by its absence, because the edge is not a family
boundary: it is P5-recovery convergence *before* the family's drive proper.

Does any code surface announce a semantic the tree does not perform?
**Functionally no** — all sixteen announced keys perform their recorded rows —
but two doc claims are imprecise, per finding F1: the header's universal
quantifier (true only as "announcements live here") and the converge doc's
"exactly like `NeedsReturn`" (which elides that `NeedsReturn` runs under key
#8's umbrella inside `private_barrier`, while the converge runs before/without
it — the E2 reviewer's distinction, confirmed against `platform.rs:704-711` and
the `Stranded` path). Hence C2: the §3.5 barrier record takes the E2 review's
**second branch** — a dated sentence naming the P5 roaming recovery rename as
a deliberate, announced-by-no-key exception to "every durable barrier edge is
announced", with the idempotence/atomicity ground — and optionally one
qualifying clause at `barrier_mutation.rs:19`. Docs only; no census movement;
closes [R2-P3-2].

### Q3 — [R2-P3-1], settled-does-not-imply-barriered. **CONFIRMED: nothing on the settled tree relies on the implication.**

The entire barrier drive/completion surface is `pub(in crate::checked_artifact)`
(`namespace/host.rs:348/:383/:422`; `barrier_mutation.rs` owner fns); a
crate-wide grep for its entry points outside `src/checked_artifact/` returns
**zero** hits; inside the subsystem the only non-test consumer is
`namespace/provider_compile.rs`, which is a compile-only shape under
`#![allow(dead_code, reason = "R1 compile-only platform provider shape")]` —
it proves signatures, executes nothing. No predicate anywhere consumes
`observe_barrier_completion` to derive a physical-barrier fact. The false
implication therefore has no reliant on this tree; it remains exactly what the
record says it is — an E4 scope clause for the family's first consumer (now
paired with F4's terminal sibling clause). Expected frozen-dead state under the
operator's ruling; nothing owed now.

### Q4 — E3 F7, the convergence idiom. **SECOND OPINION: acceptable as landed; record the consumer clause at E4 (finding F4); no code owed.**

Verified at `completed.rs:247-279`: the converged path returns `Ok(())` on
`retired_resident` without re-crossing #8/#9, refuses both-parents-resident
typed, and discloses the full reachable-state enumeration with both corpus
precedents. My grounds for concurring rather than tightening: (1) every
post-crash state converges or refuses — a discarded rename resurrects the
active row, which is a legitimate re-enterable state, not corruption, and the
occupancy/credit arithmetic is re-observed per drive
(`observed_admission_occupancy` at `:306`), so accounting follows the durable
truth, never the earlier report; (2) the exposure — a converging process
reporting completion on another process's unflushed rename — is the
corpus-wide semantic of converged-by-observation (`installed_resident`, the
namespace resident-row return); making terminal alone re-flush would fork the
family contract while leaving the first process's own rename→flush window
untouched; (3) the alternative (running #8's flush and #9's barrier
idempotently on the converged path) is cheap and available (both handles are in
scope) but is a **corpus-idiom decision**, not a terminal-local one — if the
program ever wants converged-by-observation to imply flushed, it should decide
it once, corpus-wide, in a hardening lane (R2-F class), not at a settled gate.
Until then the contract clause of F4 is the honest closure.

*(Queue note carried for completeness: the tier-2 archive-equivalence
sub-surface question — E5 adjudication G(iii) — is an amendment-with-dual
question for E4.4's future carrier and is confirmed here as deliberately
unminted on this tree; nothing in the code pretends otherwise. The K/J(ii)
same-object determination is the State axis's.)*

---

## 5. Probes-executed appendix

All in the detached worktree at `be693bd`,
`CARGO_TARGET_DIR=…/scratchpad/e7-code-target`. Every probe edit reverted;
`git status --porcelain` empty at filing.

**Gates (direct exits):**

```
cargo fmt --all -- --check                                  → exit 0
cargo check --all-targets -p gwz-core                       → exit 0 (Finished dev)
CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets -- -D warnings → exit 0
cargo test --lib -p gwz-core checked_artifact::             → ok. 447 passed; 0 failed; exit 0
cargo test --lib -p gwz-core -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::
                                                            → ok. 979 passed; 0 failed; 1 ignored; exit 0
cargo test --lib -p gwz-core workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix
                                                            → ok. 256 passed; 0 failed; exit 0
python3.13 scripts/checks/check_checked_artifact_boundaries.py
                                                            → checked-artifact boundary: ok (15 visible entries, 5 classified modules); exit 0
python3.13 scripts/checks/check_merge_compatibility_predicates.py dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .
                                                            → validated 7 migration rules, 7 runtime bindings, and 10 archive shapes; exit 0
python3.13 -m unittest scripts.checks.test_merge_compatibility_predicates
                                                            → Ran 27 tests … OK; exit 0
```

(The 69-test checker self-suite and the root-fault-matrix release leg were not
re-run by this axis: the former ran at every R2-E landing and the latter is
EXECUTED on ubuntu at this exact commit by release-verify run
33196574973→33196576270 — 457/980/256/1 — which E7.2 cites for the linux
markers.)

**Probe A — protected-tree digest fail-closed.** Appended `// probe` to
`src/checked_artifact/platform.rs` → checker: `failed / - protected source
tree changed: checked_artifact/platform.rs`, exit 1. Reverted → ok, exit 0.

**Probe B — O13 raw-writer growth.** Created
`src/workspace_ops/merge/v1_lifecycle/probe_writer.rs` containing
`use crate::durable_fs;` → checker: `failed / - protected source tree changed:
workspace_ops/merge/v1_lifecycle/mod.rs / - v1 lifecycle gained a raw
durable_fs writer outside the O13 accepted residual:
workspace_ops/merge/v1_lifecycle/probe_writer.rs`, exit 1 (both guards fired —
redundant coverage). Removed → ok, exit 0.

**Probe C — publication caller-count drift.** Edited the checker's dict value
for `provider/mutation.rs` 1→2 → checker: `failed / - catalog publication seam
changed: all six physical moves must use the single source-associated
publication primitive`, exit 1. Reverted → ok, exit 0.

**Probe D — injection-source rescan fail-closed.** Appended to
`src/checked_artifact/capability/collision.rs` a `#[cfg(test)]` fn naming
`CheckedArtifactFaultKeyV1::CleanupRowComplete` →
`cargo test --lib -p gwz-core interface_tests::fault_expected_keys`:
`the_declared_injection_sources_are_every_production_source_holding_sites …
FAILED` — "the production injection-site inventory drifted: every production
source that names CheckedArtifactFaultKeyV1 must be declared in
FAULT_INJECTION_SOURCES" (5 passed / 1 failed). Reverted → 6/6 green.

**Probe E — retired-root bound, both directions (my own re-execution of the
round-2 reviewer's at-the-bound probe).** Appended a temporary test to
`tests_retired_root.rs`: plant 64 distinct `ActiveAction` rows →
`recover_or_create` succeeds; plant a 65th → typed refusal (matched
`Unsupported`/"frozen retired-action bound" class). Result:
`e7_probe_retired_root_at_and_past_the_bound … ok. 1 passed; 0 failed`.
Reverted.

**Digest re-pin archaeology (read-only).** `git show <c> --
scripts/checks/check_checked_artifact_boundaries.py` for every R2-E commit
touching the checker: `4a0b01a` pre_catalog only · `6848109` pre_catalog ·
`d36c725` pre_catalog + catalog (the E3 third pin, review F8) · `06616f8`
pre_catalog · `88829c7` pre_catalog · `1d50e59` pre_catalog · `c11c5ef`
pre_catalog + catalog + platform (re-executed on the squashed union tree; the
E2 precedent) · `a593dbd` **platform only** · `afbc25d` **platform only** ·
conf-integrity commits: none. Current pins verified against tree bytes by the
pristine checker run.

**Static censuses (read-only greps, appendix-summarized in §2):** 11/16/10
distinct fault-key variants in exactly the declared owner files;
`TerminalAuthorityRelease` zero sites; `EmptyDirectory` ten hits with no fourth
gate; `observe(` never called outside its definition; `owner_mint` exactly one
call site; zero barrier-surface consumers outside `src/checked_artifact/`;
`git diff 8597d32..be693bd` empty on `protocol/generated.rs`; 18
`observe_leaf_exact`/`observe_leaf` call expressions (list in Q1).

---

*Filed 2026-08-29 by the E7.1 CODE axis (Fable tier), peer-blind. Object:
gwz-core `be693bd` (v0.11.1). Worktree left in place at
`…/scratchpad/e7-code-wt` for the lane owner to clean up. Round 1 is intended
complete under the two-round cap.*
