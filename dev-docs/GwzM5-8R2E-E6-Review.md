# R2-E Phase E6 — interior single-axis review

**Date:** 2026-08-28
**Axis:** interior single-axis, peer-blind (builder's report not seen, not sought)
**Tree under review:** gwz-core `a593dbd` (branch `e6-riders`)
**Base:** `8c59521`
**Train:** `d73abe8` (E6.1) → `e1e043f` (E6.2) → `a593dbd` (E6.2b), linear, no merges
**Reviewer worktree:** `.../scratchpad/e6-review-worktree`, detached. No tracked file mutated at close, nothing committed, no tag created or moved, `e6-riders` untouched (`git rev-parse e6-riders` → `a593dbd…` at open and at close). Two transient production edits were made for the negative probes required by axes C and E; both were reverted, byte-verified against pre-probe copies, and the seven protected-tree digests were recomputed after the reverts and match the pinned values.
**Host:** Darwin 26.5.2 / arm64; cargo 1.95.0, rustc 1.95.0; Python 3.13.12 (`/opt/homebrew/opt/python@3.13/bin/python3.13`).

**Verdict: GO.** No [P0], no [P1], no [P2]. Seven [P3], all record-only; none blocks the landing.

The train does what the charter asked and proves it rather than asserting it. O9's fault is a genuine `ENAMETOOLONG` at the upgrade's own `open(2)`, isolated from the v0 leg by a machine-held eight-byte window and paired with a control arm that migrates; O10's hygiene is verified by an independent negative build probe; the anchor guard is verified by an independent guard-removal probe; the protected-tree re-pin moves exactly one digest whose diff is exactly the two anchor edits, with the other six recomputed unchanged. Every moved pin is measured on its own tree, every unmoved pin was re-measured true by me at the tip, and all three commits gate green with the expected partition counts.

---

## Contents

- §1 Obligation ledger (axis A)
- §2 Axis A — scope completeness and non-creep
- §3 Axis B — O9 test validity
- §4 Axis C — O10 hygiene
- §5 Axis D — the guard tie
- §6 Axis E — anchor nit 2 correctness
- §7 Axis F — the protected-tree digest re-pin
- §8 Axis G — pins and the driver
- §9 Axis H — [P3-2]/[P3-3] fidelity
- §10 Axis I — per-commit green
- §11 Axis J — commit hygiene
- §12 Findings summary table
- §13 Verdict
- Appendix A — every command with its direct exit status
- Appendix B — what was not run, and why

---

## 1. Obligation ledger (axis A)

Derived independently from `GwzM5-8R2E-Plan.md` §1.1 rows O9 (`:54`), O10 (`:55`), O11 (`:56`), O12 (`:57`) and "Phase E6" (`:298-310`); `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §7.2 row (c) (`:898`) and the [P3-5] guard-tie routing (`:1555-1562`); `GwzM5-8R2DSettledTuple.md:659-662` (the two anchor nits' original terms); `GwzM5-8A1ActivationRecord.md` §14 (`:316-329`) and §15 [P3-R2-1] (`:333-336`); `GwzM5-8R2E-E5-Review.md` [P3-2] (`:701-709`) and [P3-3] (`:709-715`); `GwzM5-8R2E-E1-Review.md` F3 (`:537-546`).

All charter citations in the brief were checked against the documents and are accurate.

| # | Obligation | Source | Status | Evidence |
| --- | --- | --- | --- | --- |
| E6-1 | **O9** — one executed composed-path test: whitelisted `Finalizing` row, atomic upgrade `Err(_)`, v0 completes; filesystem-level fault; production keeps `AtomicUpgradeFault::None` | plan `:54`; A1 record §14 | **MET** | `a1_activation.rs:413` `an_eligible_row_completes_under_v0_when_its_atomic_upgrade_fails`; §3 |
| E6-2 | O9's **moved-pin coverage restoration** that L14 books as owed | A1 record `:325-329` | **MET** | L14's text names exactly this arm; the test's docstring cites `finalization::resumed_finalization_persists_each_phase_before_a_nested_mutation_fault` by name |
| E6-3 | **O10** — `AtomicUpgradeFault`'s injected variants unconstructible in production | plan `:55`; A1 record §15 | **MET, third shape — [P3-5]** | `atomic_upgrade.rs:36-45`; negative probe §4.2 |
| E6-4 | **O12 rider 4** — the abort rewrite-set bound guard tied at `g12:737-739` | plan `:57`; E0.2b `:1555-1558`; R1.2 (b) | **MET** | `g12.rs:727-750` doc + `:752`/`:761-767` structural tie; §5 |
| E6-5 | **Anchor nit 2** — the retired-ordinal parse made canonical | E0.2b §7.2 (c); settled tuple `:659-662`; lane-owner ruling 1 | **MET** | `anchor.rs:449-468`; guard-removal probe §6.3 |
| E6-6 | **Anchor nit 1** — NOT implemented, re-routed to the E7 dual queue | lane-owner ruling 2 | **MET (absent), ground verified correct** | `observation.rs` untouched by the whole train; §6.5 |
| E6-7 | **E5 review [P3-2]** — the G-VERIFYING structural claim executed | E5 review `:701-709` | **MET** | `compatibility_unbound_v0.rs:553-570`; §9.1 |
| E6-8 | **E5 review [P3-3]** — weaken-and-raise for the three closed sets, incl. a `finalizing` probe | E5 review `:709-715` | **MET** | `test_merge_compatibility_predicates.py:228-269`; §9.2 |
| E6-9 | **E1 review F3** — the `_fault_count` docstring's self-contradiction cured | E1 review `:537-546` | **MET, minor residual — [P3-4]** | `run_r4bg_aggregate_gates.py:71-78`; §8.4 |
| E6-10 | **E6.3 is VOID** — no E6.3 work exists | plan `:305-310`; E0.2b [P3-5] | **MET** | Nothing in the 10-file diff touches capability-refusal UX, runbooks, or notes; §2 |
| E6-11 | No scope creep: no registry case ids, corpus rows, scenario-map rows, dev-docs edits, census moves, schema/frozen-predicate changes; boundary visible surface unchanged | plan §4; standing rituals | **MET** | §2 |
| E6-12 | Per-commit green at all three commits | ritual 7 | **MET** | §10 |

---

## 2. Axis A — scope completeness and non-creep

`git diff --stat 8c59521 a593dbd` — **10 files, +573 / −16**, all modifications, zero additions or deletions of files.

```
 scripts/checks/check_checked_artifact_boundaries.py     |  10 +-
 scripts/checks/run_r4bg_aggregate_gates.py              |  67 +++++-
 scripts/checks/test_merge_compatibility_predicates.py   |  44 ++++
 src/checked_artifact/platform/anchor.rs                 |  21 +-
 src/checked_artifact/platform/anchor/tests.rs           |  72 +++++-
 src/git/tests/g12.rs                                    |  37 ++-
 src/workspace_ops/merge/store/atomic_upgrade.rs         |  30 +++
 src/workspace_ops/tests/g23/a1_activation.rs            | 252 +++++++++++++++++
 src/workspace_ops/tests/g23/compatibility_unbound_v0.rs |  23 ++
 src/workspace_ops/tests/g23/compatibility_v0.rs         |  33 +++
```

Per-commit budgets, all under the aspirational 500 LOC: d73abe8 `+293/−6`, e1e043f `+164/−3`, a593dbd `+116/−7`.

**2.1 Production source is exactly two files, both authorized.** `atomic_upgrade.rs` (O10, authorized by the plan's O10 row) and `anchor.rs` (nit 2, authorized by the lane owner 2026-08-28 for the third commit). Everything else under `src/` is inside `#[cfg(test)]` module trees (`g23/`, `git/tests/`, `platform/anchor/tests.rs`).

**2.2 No dev-docs, protocol, docs, or registry change.** `git diff --name-only 8c59521 a593dbd | grep -E "^dev-docs/|^protocol/|^docs/"` → **exit 1, no match.** The compatibility registry (`dev-docs/GwzM5-8I2CompatibilityPredicates.json`) is untouched, so no registry case id, no `fixture_corpus`/`valid_unlisted_corpus`/`archive_corpus` row, and no `descriptor_schema` or frozen-predicate change landed. The compat checker still reports **7 migration rules / 7 runtime bindings / 10 archive shapes** (exit 0), unchanged from base.

**2.3 No census move.** `src/checked_artifact/interface_tests/fault_expected_keys.rs` is not in the diff; `EXPECTED_KEY_COUNT: usize = 165` at `:174` stands, and the `checked_artifact::` partition — which executes the census assertion — passes at every commit.

**2.4 No scenario-map row, and none possible.** `check_m4_scenario_map.py:59` resolves `MAP_DOC = ROOT.parent / "dev-docs" / "GwzM5-8R4bG-Evidence.md"`, i.e. the gwz-dev workspace root, outside this checkout. Its enforcement is one-directional (`:139` `for test in sorted(tests - known)`), so adding lib tests without map rows is legal and the "43 named tests" pin is correctly unmoved.

**2.5 Boundary visible surface unchanged.** The diff to `check_checked_artifact_boundaries.py` is exactly one digest value plus a seven-line comment block; `ENTRY_REFERENCES` and `RAW_RENAME_CALL_ALLOWLIST` are untouched. The checker's ok-line reads **`checked-artifact boundary: ok (15 visible entries, 5 classified modules)`** at the tip (exit 0) — the same 15/5 the counts are derived from as script constants.

**2.6 Every removed line accounted for — sixteen, none a weakening.** `git diff 8c59521 a593dbd | grep -cE "^-[^-]"` → **16**. In full: one re-pinned digest; three docstring lines rewritten by the F3 cure; four pin/marker strings plus two of their comment lines; four `anchor.rs` survey lines rewritten to carry the guard; one `anchor/tests.rs` helper predicate line; three `g12.rs` lines rewritten with the `DIRTIED` const. The two apparent assertion removals are both **strengthenings**: `assert!(backend.merge_state(&repo).unwrap().is_some())` → `.unwrap().unwrap()` plus a conflict-set assertion, and `.is_some_and(|ordinal| ordinal.parse::<u32>().is_ok())` → the canonical-rendering predicate. Grepping the removed lines alone for `FAULT_|EXPECTED_KEY_COUNT|ACTIVE_WRITER_FLOOR|descriptor_schema|migration_whitelist|valid_unlisted_corpus|archive_corpus|case_id` → **exit 1, no match.**

**2.7 Exactly three Rust tests added**, matching the pin arithmetic exactly (+1 `checked_artifact::`, +2 remainder): `a_non_canonical_retired_ordinal_is_refused_not_adopted`, `an_eligible_row_completes_under_v0_when_its_atomic_upgrade_fails`, `the_overlong_staging_name_refuses_the_atomic_upgrade_at_the_filesystem`. Plus four Python tests in the compat suite (23 → 27), whose driver marker is the string `"OK"` and correctly does not move.

**2.8 O9's second test is justified.** The row asks for "one executed composed-path test". Two landed. The second (`the_overlong_staging_name_refuses_the_atomic_upgrade_at_the_filesystem`) is not a duplicate arm — it is the anti-vacuity guard that reads the refusal off `upgrade_open_v0` directly, so a future change that failed the row *earlier* (a typed compatibility refusal instead of the filesystem) could not keep the composed assertions green while covering something else. That is a real strengthening of the charter arm, disclosed and reasoned in both the docstring and the commit message. Extra is justified.

**2.9 E6.3 absent, as ruled.** Nothing in the diff resembles a capability-refusal message, a runbook line, or an O11 note.

---

## 3. Axis B — O9 test validity (the heart of the phase)

### 3.1 The mechanism, verified against the two stagers' code

The test's claim is that the eight bytes `.upgrade` in the staging name separate the two record stagers. I verified this against the sources rather than the comment.

- **Store leg** — `store/mod.rs:443-445`: `path.with_extension(format!("yaml.{}.{}.tmp", std::process::id(), sequence))`. Component length = `id + 5(".yaml") + 1 + pid + 1 + seq + 4(".tmp")` = **`id + pid + seq + 11`**.
- **Upgrade leg** — `store/atomic_upgrade.rs:176-181`: `target.with_extension(format!("yaml.{}.{}.upgrade.tmp", …))`. Component length = **`id + pid + seq + 19`**.

The delta is exactly 8, and it is exactly `.upgrade`. Both draw the same `TEMP_SEQUENCE` (`store/mod.rs:33`, imported at `atomic_upgrade.rs:10`), so the helper's arithmetic (`a1_activation.rs:305-325`) is correct as written.

At `id = 236 − pid_digits` the upgrade's name is `255 + seq_digits` bytes — over a 255-byte component cap for every sequence value — while the store's is `247 + seq_digits`, inside it for any sequence below 10⁸. The record itself is `241 − pid_digits` bytes and writes normally.

**The window is machine-held, not described** (`a1_activation.rs:415-424`):

```rust
const NAME_MAX: usize = 255;
assert!(overlong.len() + pid + 1 + 19 > NAME_MAX);   // shortest upgrade name already over
assert!(overlong.len() + pid + 8 + 11 <= NAME_MAX);  // longest reachable store name still under
assert!(overlong.len() + ".yaml".len() <= NAME_MAX); // the record itself is legal
```

I checked the arithmetic: with `overlong.len() + pid == 236`, the three evaluate to `256 > 255`, `255 ≤ 255`, and `241 − pid ≤ 255`. Correct, and pid-independent where it needs to be.

### 3.2 The fault reaches the atomic upgrade specifically — proven, not argued

`upgrade_open_v0` (`atomic_upgrade.rs:57-97`) runs `validate_merge_id` → `discover_open` → `require_regular_file` → `fs::read` → `decode_production_v0` → **`prepare_upgrade`** → `publish_prepared`. `prepare_upgrade` is what returns `PreparedOpenV0Upgrade::Eligible` — i.e. the whitelist match. `publish_prepared` (`:99-172`) reaches `create_unique_temporary` at `:117` before anything else touches the filesystem.

The isolated test drives `upgrade_open_v0` with `AtomicUpgradeFault::None` and asserts `error.code == ErrorCode::IoError` plus byte-identical record contents. **Production's own preparation therefore succeeded** — `Eligible` was returned — and the only remaining step is the staging `open(2)`. This is the strongest available evidence and the test obtains it.

The composed test independently proves the **v0 leg was not faulted**: it completes the operation, closes the record, and archives a `gwz.merge-operation/v0` envelope. A directory-level fault would have failed both legs; a name-level fault fails only the one with `.upgrade` in it. The test design proves the separation rather than asserting it.

### 3.3 The fault is a genuine filesystem refusal

`create_unique_temporary` loops only on `AlreadyExists` and otherwise returns `io_error(error)` (`atomic_upgrade.rs:174-192`); `io_error` is `ModelError::new(ErrorCode::IoError, error.to_string())` (`store/mod.rs:524-526`). The refusal is the OS's `ENAMETOOLONG`, not an injected variant. `AtomicUpgradeFault::None` is passed explicitly at `a1_activation.rs:544`, and production's hardcoded `None` at `runtime/dispatch.rs:411` is untouched by the whole train (confirmed after my probe revert: `git status --porcelain` empty).

The injected `FinalizationFault::AfterEnteringFinalizing` in the setup shapes the **durable record only** — it is the same window `characterization_v0.rs:25-34` registers under the rule id `finalizing-before-publication-record`. The resume runs through the production entry `handle_merge` (`dispatch.rs:157`) with `FileMergeStore` and `NullSink`; no injected store participates in the leg under test.

### 3.4 Eligibility is asserted, in both arms, before the resume

```rust
assert_eq!(i2_whitelist_matches(descriptor.value()), vec!["finalizing-before-publication-record"]);
let open = classify_open_record(temp.path()).unwrap().unwrap();
assert_eq!(open.version, RecordVersion::V0);
assert_eq!(open.adaptation, AdaptationPrecheck::MayAdapt);
```

Both conditions of `adapt_before_mutating` (`dispatch.rs:399-403`: op ∈ {Resume, Abort}; `adaptation == MayAdapt`) are therefore satisfied, so `upgrade_open_v0` **is** called, and `Err(_) => Ok(false)` (`:414`) is the mapping under test.

### 3.5 The control arm

Same durable shape, same fixture, ordinary merge id — and it lands `MergeRecordVersion::V1` with a `gwz.merge-operation/v1` envelope on disk. The two arms differ only in the merge id, so eligibility alone demonstrably does **not** produce the v0 completion. This is exactly the control the charter asked for.

### 3.6 Assertion strength

Not prose. Per arm: `ErrorCode::MergeRecoveryRequired` on the seeded fault; `record.merge_id`, `record.state == Finalizing`, `record.publication.is_none()`; the whitelist match; `RecordVersion::V0` + `AdaptationPrecheck::MayAdapt`; then `MergeOperationState::Completed`, `!completed.open`, `discover_open(...).is_none()`, `source_version` V0/V1 by arm, and — the durable outcome, not the projection — `envelope_on_disk` (`a1_activation.rs:27-37`, which reads `.gwz/merge/done/` when the record has been archived) asserting `("gwz.merge-operation/v0", 0)` vs `("gwz.merge-operation/v1", 1)`.

### 3.7 Vacuity analysis

I could not construct a vacuous pass.

- *Whitelist row not matched* — asserted equal to the exact one-element vector, in both arms, before the resume.
- *Fault firing before adaptation begins* — the isolated test proves `prepare_upgrade` returned `Eligible` first.
- *Filesystem allows >255-byte components* — then the row migrates and the `V0` assertion trips: loud failure, not silent pass.
- *Filesystem caps below 247 bytes* — then the store leg's own write fails and the setup fails: loud.
- *`TEMP_SEQUENCE` reaching 10⁸* — bounded by the second window assertion; unreachable in a test process.
- *`validate_merge_id` rejecting a 236-byte id* — checked: `store/mod.rs:313-325` imposes a charset rule and no length cap.

One modest strengthening is available and recorded below as an observation, not a finding: the isolated test's pair (`IoError` + bytes unchanged) does not by itself exclude an `IoError` raised at an earlier `fs::read`. In practice this is closed by the composed test's eligibility assertions, and a message assertion would be OS-locale-dependent. No cure required.

### 3.8 cfg gating

Both new tests, and both new helpers, are `#[cfg(unix)]` (`a1_activation.rs:325`, `:335`, `:412`, `:523`). The stated ground is that the fault needs a 236-byte path component that Windows `MAX_PATH` refuses. This is a defensible conservative gate: NTFS caps components at 255 like POSIX, so the upgrade name would likely be refused there too, but the *full* path (temp root + `.gwz/merge/` + a 241-byte record name) would breach `MAX_PATH` = 260 and break the setup and the control arm rather than the arm under test. Gating the helpers alongside the tests avoids dead-code warnings on Windows; `cargo check --all-targets` and clippy `-D warnings` are green at all three commits.

The consequence — the first R2-E delta that is **not** cfg-independent — is recorded in **both** places where the affected counts are pinned:

- `run_r4bg_aggregate_gates.py:184-188`: "a Windows pin — when this driver grows one — moves by zero, not by two."
- `:283-289` (the g23 byte-equivalence marker): "would read 122 on a Windows host; the `fault` battery's `_fault_count` refuses such a host first, which is why no split is added here."

I verified that second claim empirically rather than accepting it. `BATTERIES` is a module-level dict whose literal calls `_fault_count(...)`, so the refusal happens at **import**, before any battery runs. Importing the module with `sys.platform = "win32"` raises:

```
SystemExit: run_r4bg_aggregate_gates: no measured fault-battery count pin for host 'win32';
measure on this host and add it explicitly
```

The marker's protection is machine-true.

---

## 4. Axis C — O10 hygiene

### 4.1 The shape delivered

`atomic_upgrade.rs:36-45` puts `#[cfg(test)]` on each of the four injected variants; `:113`, `:130`, `:150`, `:160` gate the four injection sites; `:212` gates `injected_fault`; `:110-111` adds `#[cfg(not(test))] let _ = fault;` so the retained parameter raises no unused warning in production. The signature deliberately keeps the parameter rather than splitting on cfg, because a split would have to be carried through `upgrade_open_v0` into `dispatch.rs`'s call — the very site whose hardcoded `None` the finding is about, and the site O9's test reads.

### 4.2 Independent negative probe (required by the charter)

I made production name an injected variant and built the non-test lib:

```
$ sed -i '' 's|AtomicUpgradeFault::None,|AtomicUpgradeFault::BeforeStageWrite,|' \
      src/workspace_ops/merge/runtime/dispatch.rs      # line 411
$ cargo check -p gwz-core --lib
error[E0599]: no variant or associated item named `BeforeStageWrite` found for enum
              `AtomicUpgradeFault` in the current scope
error: could not compile `gwz-core` (lib) due to 1 previous error
exit = 101
```

Reverted with `git checkout --`, verified byte-identical against a pre-probe copy, `git status --porcelain` empty. **The commit's claim is exactly reproduced.** Because `cargo check --all-targets` includes the plain lib target, that command is indeed the standing machine check.

### 4.3 Leakage

- `cargo check --all-targets` → exit 0 at all three commits; clippy `--all-targets -D warnings` → exit 0 at all three.
- Every consumer of the four variants lives in `src/workspace_ops/tests/g23/atomic_upgrade_v0.rs:152-189`, inside a `#[cfg(test)]` module tree. Grep across `src/` and `tests/` finds no other reference.
- Injection sites are cfg-consistent: four variants, four gated `if` blocks, one gated constructor helper, one `cfg(not(test))` sink.
- **Public surface unchanged.** `AtomicUpgradeFault` is `pub(crate)` and re-exported only crate-internally (`store/mod.rs:24`, `merge/mod.rs:56`). Integration tests in `tests/` compile the lib without `cfg(test)` and are covered by `--all-targets`. `test_release_boundary.py` → 6 tests, OK, exit 0.
- Blast radius is the enum, its four sites and `injected_fault`, all in one file — the commit's claim, and the diff bears it out.

### 4.4 [P3-5] — the shape is a third option, not one of the two named

Plan row O10 and A1 record §15 [P3-R2-1] name "cfg-split signature **or** sealed constructor". The tree cfg-gates the **variants**, which is neither literally, and is stronger than both: a sealed constructor would leave the variants in the production binary and make `None` at the single call site a convention someone could later edit, whereas here `None` is the only value that exists. The commit argues this explicitly. Recorded so the ledger row closes against what actually landed rather than against the finding's wording.

---

## 5. Axis D — the guard tie

**The citation is exact.** At base, `g12.rs:737-739` is precisely the three lines the rider names: the `fs::write(repo.join("stable.txt"), …)`, the `abort_merge` call, and `assert_eq!(error.code, ErrorCode::DirtyMember)`. The E6.2 change lands on exactly those lines.

**The tie is structural, not commentary.** The new code (`g12.rs:752`, `:761-767`) is:

```rust
const DIRTIED: &str = "stable.txt";
…
let state = backend.merge_state(&repo).unwrap().unwrap();
assert!(
    !state.conflict_paths.iter().any(|path| path == DIRTIED),
    "the dirt must lie OUTSIDE the live conflict set {:?} or this guard \
     stops holding A′'s abort-site rewrite-set bound",
    state.conflict_paths
);
```

The named silent-drift mode — a fixture whose conflict set grows to cover the dirtied path, after which the `DirtyMember` refusal would still fire but for the wrong reason and the bound would no longer be under test — now fails the test rather than passing it quietly. That is a genuine tie, and the old `is_some()` assertion is preserved (strengthened to `.unwrap().unwrap()`).

**The ground it ties to is real.** `merge_recovery.rs:151-155` carries the "Rewrite-set bound:" comment whose soundness rests on `validate_abort_index_and_worktree` (`recovery_support.rs:216-235`) having pinned worktree changes to the conflict set:

```rust
let unexpected_worktree_change = status.files.iter()
    .any(|file| file.worktree_status != " " && !conflicts.contains(file.path.as_bytes()));
```

The doc block also names the precedent for the wrong edit — `ensure_clean_recovery_state`'s `status_dirty_outside_checked_artifact_private` carve-out — which is the concrete regression an analogous carve-out here would produce.

**Two honest limits, neither a defect.** First, the guard reads the **live** conflict set (`merge_recovery.rs:33`, from the live index) while production computes an **expected** set (`recovery_support.rs:199-214`, re-merged from `before × merge_head`); the comment says "live", and `validate_recovery_index` itself refuses when the two disagree outside the conflict paths, so the proxy is pinned to the thing it proxies. Second, a *narrow* carve-out targeting paths other than `stable.txt` would still leave this test green — but that is the half F-6 assigned to a note on `validate_abort_index_and_worktree` itself, which this step explicitly left to the lane owner as production scope, and discloses in the commit message. The rider as chartered ("the guard tied") is met.

---

## 6. Axis E — anchor nit 2 correctness

### 6.1 The change

`anchor.rs:449-468`. The parse arm keeps its existing `Invalid` return for an unparseable ordinal and adds:

```rust
if retired_name(ordinal) != *text {
    return Ok(AnchorState::Invalid);
}
```

`retired_name` is `format!("{RETIRED_PREFIX}{ordinal}")` (`:546-548`), so the rule is a full-name round trip: an ordinal is adopted only if `retired_name` would have produced this exact name. Against `retired_name`'s **real** rendering — unpadded, no sign, no width — this is correct, and it is the right implementation of the lane owner's ruling 1: a "canonical two-digit parse" would have refused `retired-0`, `retired-1`, `retired-10`, `retired-100`, i.e. most of what the protocol writes.

`assert_closed_grammar` (`anchor/tests.rs:116-124`) moves to the same predicate, so the helper and the production guard now state one rule, not two.

### 6.2 The rejected renderings, and that a refusal mutates nothing

`anchor/tests.rs:505-559` drives `"007"`, `"+7"`, `"00"`, `"0000000010"` — each first proven to parse (`.expect("the old guard admitted it")`) and proven **not** to be a name `retired_name` emits (`assert_ne!`), then written onto a prepared tree, then `prepare` is required to fail. `AnchorState::Invalid` maps to the typed refusal at `anchor.rs:239-246` ("private durability anchor is missing or ambiguous while family state exists"), returned before any mutation in that arm. Both the code and the message are asserted, and the foreign object is asserted still present.

The refusal cannot be the neighbouring `Missing { family_state: true }` arm sharing that message, because `survey` returns from inside the entry loop before `anchors.is_empty()` is ever consulted, and the first `prepare` published a resident anchor anyway.

### 6.3 The positive control, and an independent guard-removal probe

`anchor/tests.rs:546-559` writes `retired_name(0)`, `retired_name(1)`, `retired_name(10)` onto a fresh tree, requires `prepare` to **succeed**, and asserts `retired_ordinals == {0, 1, 10}` plus `assert_closed_grammar`. The guard does not over-refuse.

The first loop (`:507-515`) is the measured basis for the "unpadded, not two-digit" claim: `0, 1, 7, 9, 10, 99, 100, 1_000, u32::MAX` all round-trip through the predicate the survey now applies — spanning exactly the widths a fixed-width rule would have broken.

I verified the commit's "fails without the production change" claim myself rather than accepting it. Removing the three-line guard and running the single test:

```
thread '…::a_non_canonical_retired_ordinal_is_refused_not_adopted' panicked at
  src/checked_artifact/platform/anchor/tests.rs:532:61:
  called `Result::unwrap_err()` on an `Ok` value: ()
test result: FAILED. 0 passed; 1 failed;   exit = 101
```

`tests.rs:532` is `let error = prepare(&root.dir(), true, CODE, LABEL).unwrap_err();` for the first rejected rendering — exactly the failure the commit describes. Guard restored, tree verified clean, digests recomputed and matching.

### 6.4 Risk to the barrier family E2 landed

Low, and measured. `RETIRED_PREFIX` and `retired_name` have no consumer outside `anchor.rs` and its tests; the only other reference anywhere is a source-text probe (`interface_tests/capability_permit.rs:260`) that asserts the const declaration line, which is unchanged. The change is portable (no cfg), and the whole `checked_artifact::` partition — which carries E2's sixteen-key interruption/restart/convergence matrix and the five roaming rows — is green at 447 on darwin. Convergence is untouched in both directions: `smallest_free_ordinal` (`:533-544`) reads residency, never text.

### 6.5 Nit 1 is absent, and the re-routing ground is factually correct

I assessed the ground independently, as instructed.

- **Is the read shared?** Yes. `observation.rs:193` declares `observe_leaf_exact`; it has **twenty-one call sites across six modules** — `transition.rs` (11), `residue.rs` (5), `platform.rs` (2), `cleanup.rs` (1), `classification.rs` (1), `anchor.rs` (1, at `:551` inside `verify`) — several via the `observe_leaf_exact_current` wrapper at `observation.rs:137`. `anchor::verify` does not own the read; it rides it.
- **Is it unbounded?** Yes. `observation.rs:248-250`: `let mut bytes = Vec::new(); file.read_to_end(&mut bytes)…` — no `take`, no `try_reserve`. The size check `opened.len() != bytes.len()` happens *after* the read.
- **Is the cure a design decision?** Yes: bounding the shared reader changes every checked-artifact leaf observation, some of which legitimately read managed content of arbitrary size; the alternative is a new bounded observation entry that also returns identity, which is a new primitive.

The tree contains no attempt at it — `observation.rs` is not in the diff. Ruling 2 is correct on all three counts.

---

## 7. Axis F — the protected-tree digest re-pin (highest scrutiny)

### 7.1 Independent recomputation of all seven digests

I reimplemented `source_tree_digest` from the algorithm at `check_checked_artifact_boundaries.py:901-917` (descendant root = parent dir for `mod.rs`, else the suffix-stripped path; `{root} ∪ rglob("*")` filtered to files; sorted by posix relative path; sha256 over `len(rel)|rel|len(content)|content` with 8-byte big-endian lengths) and ran it against `src/` trees extracted with `git archive` at **four** revisions.

| tree | 8c59521 | d73abe8 | e1e043f | a593dbd |
| --- | --- | --- | --- | --- |
| `bootstrap/runtime/catalog_lease.rs` (8 files) | `1a13b93…` | same | same | same |
| `capability/path.rs` (2) | `23e46db…` | same | same | same |
| `capability/pre_catalog.rs` (39) | `5c65d5a…` | same | same | same |
| `catalog.rs` (6) | `cc845e2…` | same | same | same |
| **`platform.rs` (3)** | `2f938dd…` | same | same | **`7a73eee…`** |
| `v1_lifecycle/authority/observe.rs` (19) | `d16fa8b…` | same | same | same |
| `v1_lifecycle/mod.rs` (129) | `74a416a…` | same | same | same |

**Exactly one digest moved, at exactly one commit.** My independently computed base values match the base pins byte for byte, and my computed tip value `7a73eeedc40c2f028fd78b1cf5ad73f632ee2059c655c56e95d0695c4c035616` matches the re-pinned value byte for byte. The lowered guard was lowered to the true value of the tree it protects and to nothing else.

### 7.2 What actually changed inside the moved tree

The `platform.rs` protected tree is three files. `diff -u` on the root file `platform.rs` itself → **exit 0, identical**. `diff -ru` on the `platform/` subtree returns exactly two hunks and no others:

1. `platform/anchor.rs` — the `survey` retired-ordinal arm: the guard plus its comment.
2. `platform/anchor/tests.rs` — the strengthened `assert_closed_grammar` predicate and the new `a_non_canonical_retired_ordinal_is_refused_not_adopted`.

**Nothing else rode in.** No foreign change is possible under this re-pin: I recomputed from the git objects, not from the checker's own output, and the byte-level tree diff is exactly the two anchor edits. The other six digests being unchanged is itself the containment proof the comment claims it is, and I confirmed that claim rather than repeating it.

### 7.3 Precedent form

The E2 precedent (`check_checked_artifact_boundaries.py:155-166`) is a comment block that names the phase, cites the authorizing document and section, enumerates which entries move and why, and calls out any move that was *not* forecast. The E6.2b block (`:167-173`) is appended to that chain and follows the same form, and adds two things E2's did not: an explicit **date** and an explicit **authorization of record** ("authorized by the lane owner 2026-08-28"), plus the containment statement about the other six. Form followed and slightly exceeded.

### 7.4 No second inventory was owed

`RAW_RENAME_CALL_ALLOWLIST` (`:211-221`) is also keyed by `checked_artifact/platform.rs`; the anchor edit adds no rename reference and the checker passes at the tip with that entry unchanged. No other pin in the checker was owed.

---

## 8. Axis G — pins and the driver

### 8.1 Moved pins, provenance recorded

| pin | site | base → tip | recorded provenance | verified |
| --- | --- | --- | --- | --- |
| `checked_artifact::` | `:239` | 446/456 → **447/457** | E6.2b block `:188-204`: darwin MEASURED, linux DERIVED (+1, cfg-independent) FIRST-DISPATCH-EXPECTED | darwin **447** measured by me |
| lib remainder | `:243` | 935/936 → **937/938** | E6.1 block `:164-186`: darwin MEASURED, linux DERIVED (+2, **not** cfg-independent, both tests unix-gated) FIRST-DISPATCH-EXPECTED | darwin **937 + 1 ignored** measured by me |
| g23 marker | `:290` | 122 → **124** | comment `:283-289`: MEASURED, with the Windows divergence note | **124** measured by me |

Both dated blocks follow the E5 entries' style exactly: a prose header naming the phase, the date, which partition moves and by what, the named tests, an explicit statement that the other partitions were re-measured unchanged in the same run; then indented `darwin X -> Y: MEASURED …` / `linux X -> Y: DERIVED …, *not* measured, and therefore FIRST-DISPATCH-EXPECTED …. A measured number wins.` The E6.1 block is careful *not* to repeat "cfg-independent" for a delta that is not, and says so explicitly — that is a correctness improvement over a rote copy of the E5 form.

### 8.2 Unmoved pins re-measured true at the tip

| pin | expected | measured | exit |
| --- | --- | --- | --- |
| `v1_lifecycle::` minus `root_fault_matrix` | 256 passed | **256** | 0 |
| `record_wire::` | 75 passed | **75** | 0 |
| compat checker | 7 rules / 7 bindings / 10 archive shapes | verbatim match | 0 |
| compat suite | `OK` | 27 tests, OK | 0 |
| boundary checker | `checked-artifact boundary: ok` | ok (15 visible, 5 classified) | 0 |
| boundary suite | `OK` | Ran 69 tests, **OK**, 729.3 s | 0 |
| release boundary suite | `OK` | Ran 6 tests, OK | 0 |

Every count pin in the repo lives in `run_r4bg_aggregate_gates.py`; a repo-wide grep found no other file pinning these numbers. The pins the train left alone (`256 passed`, the release-profile `1 passed`, `75 passed`, the unknown-manifest `1 passed`, the M4 map's `43 named tests / 22 registry rows`) are all correctly unmoved given that the three new Rust tests land in `checked_artifact::` (1) and the remainder/g23 (2).

### 8.3 The g23 marker and the cfg divergence

Recorded in both pinned places (§3.8), and the "`_fault_count` refuses a Windows host first" claim is machine-true — verified by importing the driver with `sys.platform = "win32"` and observing `SystemExit` at import, before `BATTERIES` is consulted.

### 8.4 The F3 cure

E1 review F3's actual complaint was that the unqualified sentence *"Both values are EXECUTED, never derived from intent"* appeared to govern the whole function while the dated blocks below derive their linux counts from `+N` and mark them FIRST-DISPATCH-EXPECTED.

The cure (`run_r4bg_aggregate_gates.py:71-78`) scopes the claim to its own paragraph, states *why* it does not extend past it, points forward at the dated blocks, and cites the finding and date. **The contradiction with the derived blocks is cured** — that is the charter question, and the answer is yes.

A minor residual remains, recorded as [P3-4]: the newly scoped claim is still marginally broader than the paragraph it now governs, because that same paragraph derives its two remainder values arithmetically (`remainder = 1589 - (256+1+400) = 932`) and marks them FIRST-DISPATCH-EXPECTED three sentences later. The distinction being drawn — derived-from-measurement versus derived-from-intent — is defensible and is disclosed in place; the wording just does not quite say it.

---

## 9. Axis H — [P3-2]/[P3-3] fidelity

### 9.1 [P3-2] — the G-VERIFYING structural claim, executed

The E5 review's ask: the test measured `i2_whitelist_matches(descriptor) == []` for *one* descriptor while the disposition rests on the stronger claim that no rule *can* equal any `verifying_publication` descriptor; "one assertion over the whitelist would make the structural claim executed."

Delivered at `compatibility_unbound_v0.rs:553-570` — **two** assertions, and they bind the closed enum to the whitelist:

```rust
let steps = compatibility_v0::i2_normalization_enum("publication_step");
assert!(!steps.iter().any(|step| step == "verifying_publication"), …);
for (rule_id, step) in compatibility_v0::i2_whitelist_publication_steps() {
    assert!(steps.contains(&step), "{rule_id} carries a publication step outside the closed enum: {step}");
}
```

Both readers are new and read the registry itself, not a hand-copy (`compatibility_v0.rs:115-144`). I verified the registry shape independently: `normalization.enums.publication_step` = `[absent, validating_results, preparing_candidate, committing_evidence, publishing_candidate, complete]` — no `verifying_publication` — and all seven rules draw `descriptor.publication.step` from that set. The tie is genuinely structural: a rule minted with the token fails the first assertion (via the enum having to carry it), a rule minted outside the enum fails the second. Correctly, no new test was added — the claim belongs to the disposition it serves.

Cardinality vacuity (an empty `migration_whitelist` would make the loop assert nothing) is closed elsewhere in the same battery by the compat checker's pinned `"validated 7 migration rules, 7 runtime bindings, and 10 archive shapes"`.

### 9.2 [P3-3] — weaken-and-raise for the three closed sets

Four tests at `test_merge_compatibility_predicates.py:228-269`, in the suite's own mutate-then-expect-a-raise form (each test re-loads the registry in `setUp` and mutates the in-memory document):

| test | weakens | raises on |
| --- | --- | --- |
| `test_valid_unlisted_corpus_cannot_declare_finalizing` | row 0's `operation_state` → `"finalizing"` | `"exact valid-unlisted state"` |
| `test_valid_unlisted_state_registry_is_closed` | row 0's `operation_state` → `"invented"` | `"exact valid-unlisted state"` |
| `test_archive_disposition_registry_is_closed` | a `byte-preserved-v0-origin` row → `"byte-preserved-v1-origin"` | `"registered archive disposition"` |
| `test_tier_status_registry_is_closed` | an `owed` tier status → `"waived"` | `"registered tier status"` |

They do weaken and they do raise. Each expected message string is **unique** in the checker — `"exact valid-unlisted state"` only at `:577`, `"registered archive disposition"` only at `:460`, `"registered tier status"` only at `:413` — so `assertRaisesRegex` pins the raise to the closed-set check and not to an earlier validator. The finalizing probe is exactly the one [P3-3] singled out as missing ("nothing in the suite proves `finalizing` is rejected at `check_merge_compatibility_predicates.py:575-578`"), and `VALID_UNLISTED_STATES` (`:130-139`) is confirmed not to contain `finalizing`. The two archive sets each gain the test they lacked. Suite 23 → 27, green.

---

## 10. Axis I — per-commit green

Every commit gated in the review worktree, each command's exit read directly.

| | `d73abe8` | `e1e043f` | `a593dbd` |
| --- | --- | --- | --- |
| `cargo fmt --check` | 0 | 0 | 0 |
| `cargo check --all-targets` | 0 | 0 | 0 |
| `cargo clippy --all-targets -- -D warnings` | 0 | 0 | 0 |
| `checked_artifact::` | **446** (0) | **446** (0) | **447** (0) |
| lib remainder | **937 + 1 ignored** (0) | **937 + 1 ignored** (0) | **937 + 1 ignored** (0) |
| `v1_lifecycle::` − `root_fault_matrix` | **256** (0) | **256** (0) | **256** (0) |
| `g23::` | **124** (0) | **124** (0) | **124** (0) |
| `record_wire::` | — | — | **75** (0) |

**Every count matches the expected table exactly**, at every commit. Partition totals reconcile: 446 + 937 + 1 + 256 + 1 = 1641 at d73abe8/e1e043f, and 447 + 937 + 1 + 256 + 1 = 1642 at a593dbd, consistent with the `--skip` runs' "filtered out" figures (703 and 704 respectively).

One artefact worth recording so a future reader does not misread the logs: the remainder run emits **two** `test result` lines. The first (`1 passed; 1640 filtered out`) is a nested re-exec of the same harness by `src/operation/workspace_mutator_lock.rs`, which spawns `current_exe` for its cross-process lock test. Pre-existing, benign; the partition's own total is the last line.

At the tip additionally: boundary checker (exit 0), its 69-test compiler-probe suite (**Ran 69 tests, OK**, exit 0, 729.3 s), the compat checker (exit 0), its suite (27 tests, OK, exit 0), and the release-boundary suite (6 tests, OK, exit 0).

The boundary compiler-probe suite is expensive here and was the point at which an earlier run stalled: its eleven probes each copy the tree to a fresh temp dir and run `clippy --all-targets --all-features` against a shared `CARGO_TARGET_DIR` at `scratchpad/target/checked-boundary-probe`, and the host volume had under 1 GiB free. It was re-run under `timeout 780` with a disk watchdog set to abort below 250 MiB free; it completed on its own in 729.3 s, the watchdog never fired, and free space ended higher than it began (595 → 1034 MiB) as the probes' temp dirs were reclaimed. No kill or retry was needed.

---

## 11. Axis J — commit hygiene

- **No AI-attribution trailer anywhere.** `git log 8c59521..a593dbd --format=%B | grep -inE "co-authored|claude|anthropic|generated with|ai-assist|noreply@"` → **exit 1, no match.** `%(trailers)` is empty on all three commits. Author and committer are `Gianni Mariani <gianni@everbility.com>` throughout.
- **House style.** `R2-E E<step>: <summary>` matches the lane's existing subjects (`R2-E E5.2: …`, `R2-E E3: …`). Bodies carry the lane's conventions: a bolded item-by-item structure, an explicit "Pins moved" section with per-pin provenance, and a "Gates at this tree, direct exits" tail.
- **No stray or untracked artifacts committed.** Ten files, all modifications; no new file, no deletion, nothing under `target/`, no editor or scratch residue.
- **Diffs contain only what their messages claim.** Verified per commit: `d73abe8` = the two O9 tests plus their two pin moves and the F3 cure, nothing else; `e1e043f` = O10, the guard tie, [P3-2], [P3-3], and no pin move (correct — none was owed, and I re-measured all three partitions at that commit to confirm); `a593dbd` = the anchor guard, its test, the digest re-pin and the one pin move. No E6.2b content leaks into `e1e043f`, and no E6.1 content into either later commit.
- **Disclosure quality is high, including where it is inconvenient.** `e1e043f` records *why* both anchor nits are absent and that the deferral's own terms had drifted; `a593dbd` records that it supersedes that deferral under lane-owner authorization, and that nit 1 "is NOT here and is not coming". Every claim I spot-checked in the three messages — the staging-name arithmetic, the `Err(_) → Ok(false)` mapping, the negative build probe, the guard-removal failure, the six unchanged digests, the suite counts, the Windows-host refusal — reproduced.

---

## 12. Findings summary

| # | Grade | Title | Axis | Cure |
| --- | --- | --- | --- | --- |
| F-1 | [P3] | The module's "closed name grammar" table still names `.ca1-anchor-retired-v1`, which `survey` now hard-refuses | E | One table row: `.ca1-anchor-retired-<ordinal>` |
| F-2 | [P3] | "A refusal mutates nothing" is asserted over the foreign object only | E | Assert the resident name set is unchanged too |
| F-3 | [P3] | The cure trades a tolerated degraded state for a fail-closed family refusal; not stated where "convergence unaffected" is quoted | E | One sentence in the landing record or the `survey` comment |
| F-4 | [P3] | The cured F3 sentence's scoped claim is still marginally broader than its own paragraph | G | One qualifying clause naming the two remainder values |
| F-5 | [P3] | O10 delivered a third shape, not either of the two the finding named | C | Landing-record wording |
| F-6 | [P3] | Nit 1's re-routing ground is correct; the in-tree template for its cure should travel with it to E7 | E | Hand `platform.rs:219-234` to the E7 queue |
| F-7 | [P3] | Phase E6's milestone ("the review-debt ledger empty") is not met at E6 | A | Reconcile the plan heading at the landing |

### F-1 [P3] — the module doc's closed-grammar table names a fifth name the code refuses

**Evidence.** `src/checked_artifact/platform/anchor.rs:51` — the "closed name grammar" table's fourth row reads:

```
| `.ca1-anchor-retired-v1` | the retirement destination that replaces the removal |
```

but `retired_name` renders `.ca1-anchor-retired-{ordinal}` (`:546-548`), and after E6.2b `".ca1-anchor-retired-v1".strip_prefix(RETIRED_PREFIX)` = `"v1"`, which fails `parse::<u32>()` and returns `AnchorState::Invalid` at `:462-464`. The doc names a name the survey hard-refuses.

**Why it matters.** This table is the module's own authoritative statement of the closed grammar and the reference the E7.2 frozen-surface reconciliation will read. E6.2b is the commit that makes that grammar canonical, and it moved this exact protected tree, so the correction would have ridden free under the re-pin already performed.

**Pre-existing** — wrong at `8c59521` too, and the ordinal design at `:155-168` supersedes it. Record-only.

**Cure.** One row edit plus the digest recomputation the same commit already does.

### F-2 [P3] — the "refusal mutates nothing" assertion is narrower than its message

**Evidence.** `anchor/tests.rs:539-542`:

```rust
assert!(root.0.join(&foreign).is_file(), "{rendering}: a refusal mutates nothing");
```

Only the foreign object is re-read. The resident anchor, the scratch name and the retired set are not checked after the refusal, so the assertion's message claims more than the assertion tests.

**Why it matters.** The charter question is whether a refusal mutates nothing; the test answers it for the object an attacker planted, not for the protocol's own state. `survey` returns before any mutation in that arm, so the property holds — but it is asserted only in part.

**Cure.** Add `assert_eq!(retired_ordinals(&root.0), BTreeSet::new())` (or capture `names(root)` before and after) inside the loop.

### F-3 [P3] — the cure widens a fail-closed class on a durable shape; the trade is not stated

**Evidence, machine-backed.** Before the guard, `.ca1-anchor-retired-007` was adopted as ordinal 7 and the family kept working — my guard-removal probe (§6.3) demonstrated exactly this, since `prepare` returned `Ok(())` where the new test expects an error. After the guard, that same on-disk name returns `AnchorState::Invalid`, which `anchor.rs:239-246` turns into a hard refusal ("missing or ambiguous while family state exists") for all seven `AnchoredPrivateArea` callers on that tree until the foreign name is removed by hand.

The settled tuple's terms called the nit convergence-neutral hygiene (`GwzM5-8R2DSettledTuple.md:659-662`, "convergence unaffected"), and the code comment repeats that. Convergence *is* unaffected; **availability on that shape is not**.

**Why it does not rise higher.** The identical refusal already existed at base for any *unparseable* retired name, so the poisoning vector is unchanged in kind — the guard widens the poisonable subset, it does not create the class. The nits are explicitly scoped "inside the accepted same-user boundary". And the retirement design's own docstring (`:155-168`) is built to avoid *permanent* wedges, which this is not: removing the foreign name restores the family.

**Cure.** One sentence, in the landing record or beside the guard, saying the cure trades slot-wastage for a fail-closed refusal on the foreign shape. Escalate to [P2] only if the lane owner reads a fail-closed widening on a durable shape as a semantics move needing E0.2b's blessing rather than the step's own authorization.

### F-4 [P3] — the cured F3 sentence remains marginally over-scoped

**Evidence.** `run_r4bg_aggregate_gates.py:71-78` now reads "Both values in THIS paragraph … are EXECUTED, never derived from intent", while the same paragraph (`:79-85`) computes `remainder = 1589 - (256+1+400) = 932` and `933 = its lib total 1600 minus 256+1+410`, then says "Both remainder values are marked FIRST-DISPATCH-EXPECTED until their next battery execution confirms them."

**Why it matters — and why it is small.** E1 F3's actual complaint is cured: the sentence no longer appears to govern the dated blocks. The residual is intra-paragraph and the distinction intended (derived-from-measurement vs derived-from-intent) is sound and disclosed in place; the wording just does not draw it.

**Cure.** "Both **partition** values in this paragraph are EXECUTED; the two remainder values are those same runs' totals less the other partitions, and are marked FIRST-DISPATCH-EXPECTED below."

### F-5 [P3] — O10's delivered shape is neither of the two the finding named

**Evidence.** Plan `:55` and A1 record `:333-336` name "cfg-split signature **or** sealed constructor". `atomic_upgrade.rs:36-45` cfg-gates the **variants** instead.

**Why it matters.** Only for the ledger: the row should close against what landed. The delivered shape is strictly stronger than sealing (no constructor exists in a production build at all — verified by probe) and has a smaller blast radius than the signature split (which would have propagated into `dispatch.rs`, the very site the finding is about and the site O9's test reads). The commit argues both points explicitly.

**Cure.** Landing-record wording.

### F-6 [P3] — nit 1's ground verified; hand E7 the in-tree template with it

**Evidence.** Ground verified correct in all three respects (§6.5): the read at `observation.rs:249` is inside the shared `observe_leaf_exact` (`:193`) with twenty-one call sites across six modules, it is genuinely unbounded, and bounding it is a cross-cutting design decision.

**What E7 should also be handed:** the tree already contains the exact cure shape the deferral named. `platform.rs:219-234` bounds a verification read with `try_reserve_exact(expected.bytes.len() + 1)` followed by `by_ref().take(expected.bytes.len() as u64 + 1).read_to_end(...)`. That is the "new bounded observation entry" option, already written and already reviewed, in the same subsystem. It narrows E7's decision from "invent a bound" to "reuse this shape or change the shared reader".

**Why it matters.** Not a defect in this train — nit 1 is correctly absent. It is an input that shortens the E7 dual's work.

### F-7 [P3] — the E6 milestone is not met at E6

**Evidence.** Plan `:298` states Phase E6's milestone as "the review-debt ledger empty". Two rows spill: nit 1 (re-routed to the E7 dual queue by lane-owner ruling 2) and E6.3 (VOID, closing with a dated no-work record at E7.2 per E0.2b [P3-5]).

**Why it matters.** Both are lane-owner rulings, correctly reflected in the tree, but the plan heading now overstates what E6 closes. The landing record should carry the two open rows explicitly so the E7.2 acceptance is not reading an "empty ledger" claim it cannot honour.

**Cure.** Reconcile the Phase E6 heading (or add a dated note beside it) at the landing.

---

## 13. Verdict

**GO.**

No [P0], no [P1], no [P2]; nothing on the list must be fixed before landing, and no lane-owner adjudication is required to land. The seven [P3] items are record-only and may ride; F-1, F-2 and F-4 are one-line edits that could be folded into the landing commit if the lane owner prefers the tree over the record.

Substantively: the phase's charter items are all dispositioned; the O9 arm is executed on a genuine filesystem refusal, isolated from the v0 leg and controlled against a migrating twin; the two hygiene riders are verified by independent negative probes rather than by their own claims; the fail-closed guard that was deliberately lowered was lowered to exactly the true value of exactly the tree that legitimately moved; every pin the train moved is measured on its own tree with its provenance dated and stated, and every pin it did not move I re-measured true myself; and all three commits gate green with the expected counts.

Three claims in the commit messages that a reviewer would ordinarily have to take on trust were independently reproduced: the O10 non-test build failure (`E0599`, exit 101), the anchor test's dependence on the production guard (`FAILED` at `tests.rs:532`, exit 101), and the driver's Windows-host refusal at import. All three held.

---

## Appendix A — commands run, with direct exit status

All in `/private/tmp/…/scratchpad/e6-review-worktree` unless noted. Exit codes read directly; where output was piped, `${pipestatus[1]}` was read.

**At the tip `a593dbd`**

| # | Command | Exit | Result |
| --- | --- | --- | --- |
| 1 | `cargo fmt --check` | **0** | — |
| 2 | `cargo check --all-targets` | **0** | Finished dev profile |
| 3 | `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets -- -D warnings` | **0** | — |
| 4 | `cargo test --lib -p gwz-core checked_artifact::` | **0** | 447 passed; 0 failed; 0 ignored |
| 5 | `cargo test --lib -p gwz-core -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` | **0** | 937 passed; 0 failed; 1 ignored |
| 6 | `cargo test --lib -p gwz-core workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix` | **0** | 256 passed |
| 7 | `cargo test --lib -p gwz-core workspace_ops::tests::g23::` | **0** | 124 passed |
| 8 | `cargo test --lib -p gwz-core workspace_ops::merge::record_wire::` | **0** | 75 passed |
| 9 | `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | **0** | `ok (15 visible entries, 5 classified modules)` |
| 10 | `timeout 780 python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py` (disk watchdog armed at 250 MiB) | **0** | Ran 69 tests in 729.308 s — OK; watchdog never fired; free 595 → 1034 MiB |
| 11 | `python3.13 scripts/checks/check_merge_compatibility_predicates.py dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .` | **0** | `validated 7 migration rules, 7 runtime bindings, and 10 archive shapes` |
| 12 | `python3.13 -m unittest scripts/checks/test_merge_compatibility_predicates.py` | **0** | Ran 27 tests — OK |
| 13 | `python3.13 -m unittest scripts/checks/test_release_boundary.py` | **0** | Ran 6 tests — OK |

**Per-commit gates (`scratchpad/gate_train.sh`, exit 0)**

| # | Command | `d73abe8` | `e1e043f` |
| --- | --- | --- | --- |
| 14 | `cargo fmt --check` | **0** | **0** |
| 15 | `cargo check --all-targets` | **0** | **0** |
| 16 | `CLIPPY_CONF_DIR=… cargo clippy --all-targets -- -D warnings` | **0** | **0** |
| 17 | `cargo test --lib -p gwz-core checked_artifact::` | **0** — 446 | **0** — 446 |
| 18 | `cargo test --lib … --skip checked_artifact:: --skip …v1_lifecycle::` | **0** — 937 + 1 ign. | **0** — 937 + 1 ign. |
| 19 | `cargo test --lib … v1_lifecycle:: -- --skip root_fault_matrix` | **0** — 256 | **0** — 256 |
| 20 | `cargo test --lib … workspace_ops::tests::g23::` | **0** — 124 | **0** — 124 |
| 21 | `git checkout --detach a593dbd` (return) | **0** | HEAD `a593dbd`, `git status --porcelain` empty |

**Independent probes and derivations**

| # | Command | Exit | Result |
| --- | --- | --- | --- |
| 22 | `git archive {8c59521,d73abe8,e1e043f,a593dbd} src | tar -x -C …` | **0** | four `src/` trees extracted |
| 23 | `python3.13 scratchpad/dig/recompute.py …` (own reimplementation of `source_tree_digest`) | **0** | 7 digests × 4 revisions; exactly one moved, at `a593dbd`, to the re-pinned value |
| 24 | `diff -u …/base/src/checked_artifact/platform.rs …/tip/…` | **0** | identical |
| 25 | `diff -ru …/base/src/checked_artifact/platform …/tip/…` | **1** | exactly two hunks: the `survey` guard and the new anchor test |
| 26 | Import driver with `sys.platform = "win32"` | **0** | `SystemExit: … no measured fault-battery count pin for host 'win32' …` |
| 27 | **O10 negative probe**: point `dispatch.rs:411` at `AtomicUpgradeFault::BeforeStageWrite`, then `cargo check -p gwz-core --lib` | **101** | `error[E0599]: no variant or associated item named 'BeforeStageWrite'` |
| 28 | `git checkout -- src/workspace_ops/merge/runtime/dispatch.rs`; `diff -q` vs pre-probe copy | **0** | byte-identical; `git status --porcelain` empty |
| 29 | **Anchor-guard negative probe**: delete the three-line guard, `cargo test --lib -p gwz-core platform::anchor::tests::a_non_canonical_retired_ordinal_is_refused_not_adopted` | **101** | FAILED at `anchor/tests.rs:532` — `unwrap_err()` on an `Ok` value |
| 30 | `git checkout -- src/checked_artifact/platform/anchor.rs`; re-run #23 on the live tree | **0** | all seven digests match the pins; `git status --porcelain` empty |
| 31 | `git diff --name-only 8c59521 a593dbd | grep -E "^dev-docs/|^protocol/|^docs/"` | **1** | no match |
| 32 | `git diff 8c59521 a593dbd | grep -E "^-[^-]" | grep -E "FAULT_|EXPECTED_KEY_COUNT|ACTIVE_WRITER_FLOOR|descriptor_schema|migration_whitelist|valid_unlisted_corpus|archive_corpus|case_id"` | **1** | no forbidden surface among removed lines |
| 33 | `git log 8c59521..a593dbd --format=%B | grep -inE "co-authored|claude|anthropic|generated with|ai-assist|noreply@"` | **1** | no match |
| 34 | `git status --porcelain`; `git rev-parse e6-riders`; `git tag --points-at a593dbd` | **0** | clean; `a593dbd`; zero tags |

---

## Appendix B — what was not run, and why

| Gate | Reason |
| --- | --- |
| `check_m4_scenario_map.py` | Workspace-relative: `:59` resolves `MAP_DOC = ROOT.parent / "dev-docs" / "GwzM5-8R4bG-Evidence.md"`, which does not exist relative to a detached scratchpad worktree. Per the brief; not attempted. Unaffected by the train (§2.4). |
| `check_merge_docs.py`, `test_check_merge_docs.py` | Workspace-relative: `DEFAULT_WORKSPACE_ROOT = parents[3]` and `WORKSPACE_ROOT = CHECKS_DIR.parents[2]`. Per the brief; not attempted. Neither file is in the diff. |
| `cargo test --release --lib -p gwz-core root_fault_matrix` (`"1 passed"`) | Requires a full release build; the host volume had under 1 GiB free during the review. The pin is unmoved by the train, sits outside all three partitions under review, and is unreachable from any changed code. Recorded as not re-measured. |
| `test_v1_lifecycle_privacy_probe.py` | Not in the charter checklist; compiler-probe based; `v1_lifecycle/` is untouched by the train (its protected-tree digest is unchanged, independently recomputed). |

**Caveat carried forward.** The three darwin partition counts are measured here; the linux counts (457 / 938) are DERIVED and marked FIRST-DISPATCH-EXPECTED in the driver, and the g23 marker's 124 is a darwin/linux number with no Windows split. All three are the lane owner's three-platform dispatch to confirm; the driver refuses a Windows host at import rather than inheriting a foreign count, which I verified.
