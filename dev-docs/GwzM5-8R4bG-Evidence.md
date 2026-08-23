# R4b-G evidence — the inventory executed against the R2-D settled tuple

Date: 2026-08-23

Status: **EXECUTED — the compilation pass, not the gate.** This document
executes every item of `GwzM5-8R4bG-EvidenceInventory.md` against the current
tuple and records the result with its evidence. It accepts nothing, rescopes
nothing, and adds no production line. The R4b-G review (dual, Fable) runs on
this document next; §9 is written for that review and lists every FAIL,
every un-evidenced RE-FRAME, and every judgment call.

**Discipline.** Every gate tail below was produced on this host, at this
tuple, for this document. Where a claim is a *citation* rather than an
execution it says so. Where the checklist's own text is wrong or stale, this
document says so rather than reproducing it (§7). No git write operation was
performed; the only file written is this one.

**Controlling frame.** `GwzM5-8ThinA1Amendment.md` (ACCEPTED 2026-08-22,
GO/GO round 2) is the gate-chain authority. R4b-G on the A1 path is gated by
the **R2-D settled gate alone**; R2-E/R2-F and R3-R6 are post-A1 or parallel.
The checklist was drafted against the pre-thin-A1 frame and carries the §5
re-frame banner (`GwzM5-8R4bG-EvidenceInventory.md:3-9`); every item whose
frame assumed the R2-R6-settled tree is re-read here against the R2-D settled
tuple and marked RE-FRAMED or DEFERRED-BY-AMENDMENT with its superseding
clause named.

---

## 1. The acceptance object

### 1.1 What R4b-G accepts

R4b-G is the **aggregate acceptance gate for the whole R4b reverse merge
lifecycle** — the ~25k-line R4b tree (`GwzM5-8ProgressReviewF5.md:426-428`),
built across checkpoints R4b-TI/TR/S/A/F/X/P. Three documents define its
content, and they compose rather than compete:

1. `GwzM5-8Refactor.md:2243-2244` — checkpoint 8 of the R4b package list:
   > 8. **R4b-G** runs aggregate fault, compatibility, byte-equivalence,
   >    unknown-field, privacy, call-graph, and settled-tree review gates.

2. `AgentProcessRules.md:2000-2006` (§A.5, operator sequence instruction) —
   adds two named items beyond checkpoint 8 and fixes the closing review
   form:
   > 4 Once P1 and P2 are accepted, complete R4b-G:shared router integration;
   > aggregate restart/fault matrices; v0 compatibility and byte-equivalence;
   > unknown-field, privacy, and call-graph gates; P3/P4 status/archive
   > cross-driver integration.
   >
   > 5 Run the two final full-tree R4b reviews.

3. `GwzM5-8ChangeBudget.md:515` — the shape of the package:
   `R4b-G — aggregate gates and wiring only | ≤150 | 0 | ≤1,500 | ≤3 | ≤12`.
   **Zero production lines.** And `:553-556`: "R4b-G includes two independent
   settled-tree reviews with no open P0/P1/P2 finding. A failed review or a
   requirement for an unowned mutation stops the package; it is not covered
   by the ledger's ordinary 20% numeric tolerance."

The property R4b-G's acceptance asserts is the R4b exit condition,
`GwzM5-8R4bTransitionDesign.md:1522-1527` (the inventory cites this as
`:1341-1348`; the document has since grown — see §7):

> R4b is complete only when every v1 durable rewrite reachable in the test
> harness is produced by the typed kernel, committed by the checked store,
> and covered by the predecessor, footprint, unknown-field, restart, and
> fault matrices. All finalization and reverse paths must consume the
> persisted accepted workspace and exact lock bytes. The production v1 writer
> and R3 migration dispatch must still be unreachable.

Followed immediately by `:1529-1530`: "Passing R4b does not activate v1. A1
remains a separate, small, reviewed runtime-dispatch change after R4b and M5b
meet their gates." **R4b-G accepts a tree, not a behaviour change.**

### 1.2 What R4b-G unblocks — including the IMPL merge the checkpoint names

Three consumers, in the order the record names them.

**(a) M5b-IMPL merge and settled review — this is the checkpoint's "IMPL
merge" reference.** `CurrentProgramCheckpoint.md:1090-1093`:

> M5b-IMPL review tier recorded at freeze: mandated-dual by default,
> single-axis only for test/fixture-confined diffs. **IMPL merge waits for
> R4b-G per the frozen dependency statement.**

The frozen dependency statement is `GwzM5-8M5bNoFfDesign.md:976-989`, whose
heading is "**Dependent on R4b-G settling — must wait:**" and whose first
bullet is "Merging and settled-review of M5b-IMPL", with three named reasons:
(a) the frozen resume order is R4b-G → M5b → A1
(`GwzMergeCheckpoint-v0.10.5.md:128-130`); (b) "M5b-IMPL edits the same
`v1_lifecycle` tree whose P2/P3/P4 lanes (~3.5k production lines) are
implemented but **not yet independently accepted** … landing M5b there first
would give R4b-G's settled-tree/call-graph/byte-equivalence gates a moving
target and alias finding ownership"; (c) "M5b's own unreachability argument
leans on R4b-G's call-graph gate (§6 item 4)."

**This dependency has already been overtaken by events, and the R4b-G review
must rule on it.** See §9 judgment call J-1: M5b-IMPL is merged, in
`b91bdeb`'s ancestry, and pushed.

**(b) The operator-escape implementation package(s).**
`GwzM5-8OperatorEscapeDesign.md:751-753`: "implementation lands as its own
reviewed package after R4b-G, before or at A1". Restated at
`CurrentProgramCheckpoint.md:989-991`: "implementation lands as its own
reviewed package(s) after R4b-G, at or before A1 (Q3)."

**(c) The A1 activation review.** Under thin A1 the checkpoint's own R2-D
SETTLED record (`CurrentProgramCheckpoint.md:629-631`) states the position
exactly: "THIN-A1 STATUS: R2-D settled ✓ and M5b bound proofs green on the
settled tree ✓ — the A1 activation's two preconditions stand. **Remaining:
R4b-G, then A1 activation.**" R4b-G is the last gate standing in front of the
A1 activation review.

### 1.3 What R4b-G does *not* accept

- **The R2-F/R5 native-evidence RELEASE gates.** Unwaived and unmoved
  (`GwzM5-8ThinA1Amendment.md:236-239`): "they remain the native-platform
  barrier between the A1-enabled v1 writer and shipped users — thin A1 moves
  them off the A1 gate, not off the release gate."
- **The v1_lifecycle P2/P3/P4 acceptance debt and the D3/D4 named-residual
  disposition.** Routed to the A1 activation review by
  `GwzM5-8ThinA1Amendment.md:43-55` and `:242-244`. §8 verifies those records
  exist; this document does not execute them.
- **Production v1 activation.** `TransitionDesign:1529`.

---

## 2. Tuple verification and member-state drift

| Repository | Tuple-table pin | Actual HEAD | Verdict |
| --- | --- | --- | --- |
| gwz-dev (root) | pause-checkpoint docs commit | `42aadce` "Checkpoint: R2-D IS SETTLED — dual 3 of 3 GO/GO at round 2" | advanced by the program (expected) |
| gwz-core | `d32b2c9` local/unpushed (table at `CurrentProgramCheckpoint.md:794`) | **`b91bdeb`**, `origin/main` = `b91bdeb`, 0 ahead | **table row is STALE**; the live pin is the RESUMED record's `b91bdeb` (`:606-608`) |
| gwz-cli | `3cca145` | `3cca145` "Close R4b P1/P2 remediation gate" | **MATCH — no drift** |
| gwz-py | `929efb0` | `929efb0` "Implement R4b reverse merge lifecycle" | **MATCH — no drift** |
| taut | `f008419` | `f008419` "Emit fallible from_cbor in rust corpus/roundtrip dispatchers" | **MATCH — no drift** |

All four member trees are clean. The gwz-core row of the §"Exact tuple" table
(`CurrentProgramCheckpoint.md:789-804`) is the **pause-record baseline of
2026-08-16** and has not been restated since; the R2-D SETTLED record at
`:606-608` names `b91bdeb` as "the settled object". The two are not in
conflict — the table is simply un-refreshed — but the table is the document's
own literal-restatement mechanism (ReviewCode-3 P3-5), so this is a
bookkeeping drift the R4b-G review should have the lane close (§9, J-6).

The settled tuple document (`GwzM5-8R2DSettledTuple.md:6-8`) names `d45458d`;
`b91bdeb` is one commit further — the settled-dual round-2 remediation. All
gates below were re-run at `b91bdeb`.

---

## 3. The executed gate train at `b91bdeb`

Every command below was run from `/Users/owebeeone/limbo/gwz-dev/gwz-core`
with `CARGO_TARGET_DIR` pointed at a scratch directory. Python is 3.13.7
(`/opt/homebrew/bin/python3.13`). Host: macOS/Darwin 25.5.0, arm64.

| # | Gate | Result | Command |
| --- | --- | --- | --- |
| 1 | `cargo fmt --all -- --check` | **clean** (exit 0) | workspace-wide |
| 2 | Full `cargo test --lib -p gwz-core`, four partitions | **1569 / 0**, 1 ignored | §3.1 |
| 3 | Integration binaries ×4 | **50 / 0** | §3.2 |
| 4 | Protocol currentness by name | **3 / 0** | §3.2 |
| 5 | Boundary checker | `ok (15 visible entries, 5 classified modules)` | exit 0 |
| 6 | Boundary-checker unit suite (compiler probes) | **65 / 0** (438.5 s) | exit 0 |
| 7 | Release-boundary suite | **6 / 0** | exit 0 |
| 8 | Merge-doc gate + its suite | `ok (11 sources, 147 assertions)` / **3 / 0** | exit 0 |
| 9 | Merge-compatibility gate + its suite | `validated 7 migration rules and 7 runtime bindings` / **14 / 0** | exit 0 |
| 10 | L2-04 retained-reader harness | **86 / 0** (152.0 s); `validate` and `gate-ready` both `tuple_count: 24` | exit 0 |
| 11 | Clippy `-p gwz-core`, all-targets/all-features, from `cargo clean -p` | **zero diagnostics**, exit 0 | §3.3 |
| 12 | **Clippy `--workspace`, all-targets/all-features, `CLIPPY_CONF_DIR=$PWD`** | **RED — exit 101, 7 errors** | §3.3, **FAIL** |
| 13 | `protocol/regen.py --check` | `OK -- committed protocol artifacts are current.` exit 0 | §3.4 |
| 14 | `protocol/d0_roundtrip_check.py` | `RESULT: all diff D0 round-trips passed`, exit 0 | §3.4 |

Gates 13 and 14 are **new relative to the settled tuple**, which recorded
both as un-executable on its host (`GwzM5-8R2DSettledTuple.md:579-584`:
"`protocol/regen.py --check` needs a PyPI `taut-proto` venv and is
`scripts/release.py`'s"). The venv (`protocol/.regen-venv`, taut-proto 0.8.1)
exists on this host and the gate is green; `d0_roundtrip_check.py` needs
`taut` on `PYTHONPATH` (the sibling checkout) or the regen venv's
interpreter, and is green under both. This **closes** one of the tuple's two
honestly-scoped not-run gates.

### 3.1 Lib-suite partitions (the 600 s per-command budget forced a split)

```
$ cargo test --lib -p gwz-core -- --list | grep -c ': test$'
1570

$ cargo test --lib -p gwz-core checked_artifact::
test result: ok. 400 passed; 0 failed; 0 ignored; 0 measured; 1170 filtered out; finished in 37.40s

$ cargo test --lib -p gwz-core -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::
test result: ok. 914 passed; 0 failed; 1 ignored; 0 measured; 655 filtered out; finished in 53.63s

$ cargo test --lib -p gwz-core workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix
test result: ok. 254 passed; 0 failed; 0 ignored; 0 measured; 1316 filtered out; finished in 344.53s

$ cargo test --release --lib -p gwz-core root_fault_matrix
test workspace_ops::merge::v1_lifecycle::reverse::preservation::tests::root_fault_matrix::every_root_physical_and_successor_boundary_recovers_without_repeating_mutation ... ok
test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 1569 filtered out; finished in 318.71s
```

Reconciliation: 400 + 914 + 254 + 1 = **1569 passed, 0 failed, 1 ignored** —
byte-for-byte the settled tuple's full-suite figure
(`GwzM5-8R2DSettledTuple.md:521-522`), reached by partition rather than one
700 s run. The partition is a harness accommodation, not a scope reduction:
the four filters are disjoint and exhaustive over the 1570 listed tests.

**New citable perf figure.** `root_fault_matrix::every_root_physical_and_
successor_boundary_recovers_without_repeating_mutation` — **318.71 s,
release, at `b91bdeb`, green.** See §8 item 3: the "~343 s release" figure
the A1 brief expects does not exist in the repository; this measurement, and
the 576 s release figure at `GwzM5-8D3Impl-ReviewState.md:553`, are what does.

### 3.2 Integration binaries and protocol currentness

```
$ cargo test --test diff_render_spike -p gwz-core   → ok. 10 passed; 0 failed
$ cargo test --test protocol -p gwz-core            → ok. 29 passed; 0 failed
$ cargo test --test publish_workflow -p gwz-core    → ok.  9 passed; 0 failed
$ cargo test --test rename -p gwz-core              → ok.  2 passed; 0 failed

$ cargo test --test protocol -p gwz-core generated_
test taut_command_forces_utf8_for_generated_source_files ... ok
test generated_checked_artifact_protocol_is_current ... ok
test generated_protocol_is_current ... ok
test result: ok. 3 passed; 0 failed; 0 ignored; 0 measured; 26 filtered out; finished in 0.14s
```

### 3.3 Clippy — and the runbook command that is red

```
$ cargo clean -p gwz-core && CLIPPY_CONF_DIR="$PWD" cargo clippy -p gwz-core --all-targets --all-features -- -D warnings
     Removed 3226 files, 2.2GiB total
    Finished `dev` profile
(zero diagnostics)   exit=0

$ CLIPPY_CONF_DIR="$PWD" cargo clippy --workspace --all-targets --all-features -- -D warnings
error: use of a disallowed method `std::io::Write::write_all`  --> gwz-cli/src/pager.rs:127:23
error: use of a disallowed method `std::io::Write::write_all`  --> gwz-cli/src/pager.rs:131:21
error: use of a disallowed method `std::fs::create_dir_all`    --> gwz-cli/src/tests/g01/commands.rs:460:9
error: use of a disallowed method `std::fs::create_dir_all`    --> gwz-cli/src/tests/m2c.rs:51:5
error: use of a disallowed method `std::fs::write`             --> gwz-cli/src/tests/m2c.rs:55:5
error: use of a disallowed method `std::fs::write`             --> gwz-cli/src/tests/m2c.rs:85:5
error: use of a disallowed method `std::fs::write`             --> gwz-cli/src/tests/m2c.rs:99:5
error: could not compile `gwz` (lib) due to 2 previous errors
error: could not compile `gwz` (lib test) due to 7 previous errors
exit=101

$ cargo clippy --workspace --all-targets --all-features -- -D warnings     # no CLIPPY_CONF_DIR override
    Finished `dev` profile   exit=0
```

**Diagnosis, stated so the review need not re-derive it.** The runbook's §5
step 5 command (`GwzM5-8R4bG-EvidenceInventory.md:577-579`) is
`CLIPPY_CONF_DIR=$PWD cargo clippy --workspace …`. `CLIPPY_CONF_DIR` forces
gwz-core's `clippy.toml` onto **every** workspace member, and gwz-cli is not
inside the checked-artifact writer perimeter — it has no `clippy.toml` of its
own, and neither does the workspace root. So the failure is a **scoping
artifact of the command as written**, not a gwz-core defect and not a gwz-cli
regression: without the override, `--workspace` is clean, and gwz-core keeps
its own `clippy.toml` plus `#![forbid(clippy::disallowed_methods)]`. The CI
form (`checked-artifact-boundary.yml:30-35`, run inside the gwz-core
checkout) is gwz-core-scoped and green. **The runbook step needs correcting
to `-p gwz-core` or the perimeter needs extending — a real, if small, R4b-G
decision (§9, J-3).**

### 3.4 Protocol and parity

```
$ python3.13 protocol/regen.py --check
regen: using taut-proto 0.8.1  (venv: …/gwz-core/protocol/.regen-venv)
regen: $ tautc gen protocol/gwz.taut.py … -l rust --api-only --with-runtime
regen: $ tautc gen protocol/checked_artifact.taut.py … -l rust --api-only
regen: $ tautc corpus protocol/gwz.taut.py -o protocol/corpus -l rust --check
regen: $ tautc corpus protocol/checked_artifact.taut.py -o protocol/checked_artifact-corpus -l rust --check
regen: OK -- committed protocol artifacts are current.

$ PYTHONPATH=…/taut/src python3.13 protocol/d0_roundtrip_check.py
RESULT: all diff D0 round-trips passed
```

### 3.5 CI evidence at the settled tuple (queried, not executed)

| Workflow | Latest run | SHA | Conclusion |
| --- | --- | --- | --- |
| Checked-artifact boundary | `32620718426` | **`b91bdeb`** | **success** (job: Structural boundary) |
| Retained merge readers | `32620718427` | **`b91bdeb`** | **success** — 7 jobs: harness ×2 (ubuntu-24.04, windows-2022) **+ the full 5-platform matrix** (linux-x86_64, linux-aarch64, macos-x86_64, macos-aarch64, windows-x86_64), all green |
| Checked-artifact boundary | `32619207902` | `d45458d` | success |
| Retained merge readers | `32619207899` | `d45458d` | success |
| **Windows matrix** | `32616954369` | `514f8e6` (run 21) | success — **no run at `d45458d` or `b91bdeb`** |
| **Platform matrix** | `32616955672` | `514f8e6` (run 21) | success — **no run at `d45458d` or `b91bdeb`** |
| **Linux identity probe** | `31901707157` | `f36d20d` | success — **21 gwz-core commits stale** |

The retained-readers 5-platform matrix result is a **strengthening** over the
settled tuple, which recorded the network matrix job as "CI's" and not run
(`GwzM5-8R2DSettledTuple.md:579-584`). It is green at the exact settled
object. The Windows/Platform matrix gap is a **FAIL** — see §9, F-7; it is
the tuple's own round-2 open item ("The ten multi-component writer rows
(5.1's delta) have no native Windows/ARM execution record — runs 19-21
predate `d45458d`", `GwzM5-8R2DSettledTuple.md:663-665`), now confirmed
against the CI record and extended: *nothing* on the settled tree has native
execution, not merely the ten new rows.

Boundary and retained-readers were **red** at `514f8e6` and `51a9cba` — the
companion-assertion drift Step 5.1 diagnosed and repaired
(`GwzM5-8R2DSettledTuple.md:76-103`). They are green from `d45458d` onward,
which corroborates that repair against CI rather than the lane's own host.

---

## 4. Named-gate inventory, executed

Verdicts: **PASS** (evidence produced) · **FAIL** (named artifact absent) ·
**RE-FRAMED** (item's frame assumed the R2-R6-settled tree; the thin-A1
equivalent is stated) · **DEFERRED-BY-AMENDMENT** (now post-A1; superseding
clause cited) · **PENDING BY DESIGN** (nothing to produce; the gate *is* the
review).

| # | Checklist item | Verdict | Evidence / what is missing |
| --- | --- | --- | --- |
| 2.1 | Aggregate fault gate — "aggregate restart/fault matrices" | **PASS** | Every suite named at `:150-175` re-executed green on the settled tree in one recorded pass (this document, §3.1): lib 1569/0/1 across four disjoint partitions; `checked_artifact::` 400/0 carries the 165-key fault census (`GwzM5-8R2DSettledTuple.md` §4.1-4.8, 107 keys with per-key site+row tables); `v1_lifecycle::` 255 incl. `reverse::preservation` 36 (`root_fault_matrix` green at 318.71 s release) and `reverse::rollback` 29; g23 fault/recovery suites inside the 111. **Counts re-baselined — see §7.** The *wiring* half is W5. |
| 2.2 | Compatibility gate — "v0 compatibility" | **PASS** | `check_merge_compatibility_predicates.py … --core .` → `validated 7 migration rules and 7 runtime bindings`; its suite 14/0. `check_merge_docs.py` → `ok (11 sources, 147 assertions)`; its suite 3/0. Registry confirmed 7 `migration_whitelist` / 7 `fixture_corpus` / 6 `valid_unlisted_corpus`; `include_str!`-bound at `src/workspace_ops/tests/g23/compatibility_v0.rs:10`; production consumption at `src/workspace_ops/merge/record_wire/open_v0/adapter.rs:115-117` (binding at `:22`). g23 111/0, `record_wire::` 75/0. CI: retained-readers 5-platform matrix green at `b91bdeb`. |
| 2.3a | Byte-equivalence — the seven adapted-v0 scenarios | **PASS** | All seven registry cases confirmed present and named exactly as the checklist lists them (`changed/finalizing-before-publication-record`, `changed/validating-before-candidate`, `changed/candidate-persisted`, `changed/evidence-unrecorded`, `changed/evidence-recorded`, `changed/prefix-boundary`, `unchanged/no-publication-finalizing`). Driven through `assert_upgrade_fixture` (`g23/atomic_upgrade_v0.rs:21-57`, cite exact) with `assert_v1_restart` (`:341`), `assert_unknown_fields_and_verifier` (`:52`); accepted-workspace byte assertion `accepted.lock.exact_yaml == candidate.lock_yaml` at `g23/compatibility_v0.rs:451` (within the cited `:444-458`). Executed inside the green g23 111/0. |
| 2.3b | Byte-equivalence — "every M4 scenario" | **FAIL** | Unchanged from the draft, re-verified at `b91bdeb`: **no document or suite enumerates the M4 scenario set by name and maps it to equivalence evidence.** `GwzMergeM4-RemPlan.md` and `-RemReview.md` mention scenarios only generically; `GwzM5-8Refactor.md` §15.3.3 (now at `:1456`) is "Archived record decoding and evidence-only projection", not an M4 table. The obligation text at `GwzM5-8Refactor.md:2265` is the only "every M4" in dev-docs outside the checklist's own self-references. This is W4. |
| 2.4 | Unknown-field gate | **PASS** | `every_transition_effect_commits_its_exact_unknown_manifest` present at `v1_lifecycle/tests/store/matrix.rs:18`; `store/unknown.rs` and `store/rewrite.rs` present; `record_wire::unknown_fields` now **28** tests (was 23), `record_wire::archive` **29** (was 24), `record_wire::tests::decode` 18 — total `record_wire::` 75/0 green; drift-survivor test at `tests/store/drift.rs:16`; extension preservation at `tests/store.rs:142`. |
| 2.5a | Privacy — Rust visibility + boundary-checker probes | **PASS** | `PreparedV1Rewrite` still `pub(super)` at `v1_lifecycle/transition/mod.rs:20` (cite exact); authority/proof-token types `pub(super)` across `v1_lifecycle/authority.rs:28-158` (cite holds as a range); store commit path `pub(super)` at `store/rewrite.rs:15-23` (whole file is `pub(super)`-only). Boundary checker `ok`; its **65-test compiler-probe suite green (438.5 s)**, including `test_approved_outside_source_target_cannot_hide_an_observer_caller`, `test_compiler_rejects_nested_writer_in_authority_observer_tree`, `test_compiler_rejects_writer_in_v1_artifact_observer`, `test_merge_authority_backend_has_a_private_compiler_seal` (22 probe-bearing tests in all). CI green at `b91bdeb`. |
| 2.5b | Privacy — the dedicated `PreparedV1Rewrite` / proof-token / raw-writer probe | **FAIL** | Re-verified at `b91bdeb`: zero `compile_fail`, zero `trybuild`, zero occurrences of `PreparedV1Rewrite` or any proof-token name in `gwz-core/scripts/`. The only "cannot construct" in `src/` is still a comment (`v1_lifecycle/authority.rs:716` — moved from the cited `:710`). Visibility gives the property; **nothing proves it fails closed.** This is W1. |
| 2.6a | Call-graph — checked-artifact / writer boundary | **PASS** | Boundary checker `ok (15 visible entries, 5 classified modules)`; structural call-graph inventory intact (`source_tree_digest` now at `check_checked_artifact_boundaries.py:789`, `calls()` at `:808`, the `#![forbid(clippy::disallowed_methods)]` verification at `:845` — all moved +43, see §7). `clippy.toml` writer boundary present with the three gwz-core fall-through writers (`artifact::write_atomic` `:21`, `stash::write_bundle` `:22`, `sync_workspace_boundary` `:23`); clippy `-p gwz-core --all-targets --all-features -D warnings` from clean → zero diagnostics. Release-boundary suite 6/0. CI per-commit lane gate present (`checked-artifact-boundary.yml:23-25`) and green at `b91bdeb`. |
| 2.6b | Call-graph — "v1 lifecycle modules contain no raw v0 persistence call" | **FAIL** | The property is **stronger than the draft claimed** — `grep -rn 'MergeStore\|FileMergeStore' src/workspace_ops/merge/v1_lifecycle/` returns **0 hits, including test code** (the draft said "outside test code"). The v0 finalizer remains outside the v1 tree at `merge/finalize_dispatch.rs:20`, `pub(super) fn finalize<B: GitBackend, S: MergeStore>`. But **no check asserts it**; nothing fails closed if it changes. This is W2. |
| 2.7a | Settled-tree review gate, layer 1 — RemPlan-4 R6 checked-artifact settled-tree re-review pair | **RE-FRAMED** | The checklist calls this "a *precondition* of G, owned by the R2-R6 chain" (`:379-382`), citing `RemPlan-4:1140-1145`. **Superseded as an A1/R4b-G gate** by `GwzM5-8ThinA1Amendment.md:73-78` (§2.2) and `:79-82` (§2.3). **Thin-A1 equivalent, and it EXISTS:** the R2-D Phase 5.2 settled dual — "R2-D IS SETTLED — 2026-08-23, dual #3 of 3 consumed, GO/GO at round 2… The settled object is gwz-core `b91bdeb`" (`CurrentProgramCheckpoint.md:606-608`); reports `GwzM5-8R2DSettled-Review{Code,State}.md` (rounds 1+2 appended); round 2 "Code GO unconditional / State GO". Zero open P0/P1/P2. |
| 2.7b | Settled-tree review gate, layer 2 — the two final full-tree R4b reviews | **PENDING BY DESIGN** | `AgentProcessRules.md:2006`; tier dual, cross-model where available. Nothing to produce; **this document is that review's input.** Not satisfied by 2.7a: the R6/5.2 pair is the *checked-artifact* tree; R4b-G's own pair is the *full R4b* tree (~25k lines). The checklist states the distinction at `:126`; it survives the re-frame intact. |
| 2.8 | Shared router integration | **PASS** | `reverse_router` 3, `reverse_entry` 9, `c7_matrix` 7 (incl. `request_dispatch`) — all present by name and green inside the v1_lifecycle 255/0. |
| 2.9a | P3/P4 status/archive cross-driver integration — execution | **PASS** | `v1_lifecycle::status::tests` 12, `archive::archive_tests` 3, `archive::gc_tests` **7** (was 6), `archive_result` 1, `record_wire::archive` **29** (was 24). Python driver side re-executed: `gwz-py/src/tests/test_merge_cli_cross_driver.py` **14 passed** (the checklist says 4 — re-baselined, §7), `test_merge_cli_custom_message.py` 5 passed. |
| 2.9b | P3/P4 — the P2-P4 *acceptance* debt | **DEFERRED-BY-AMENDMENT** | The checklist says the debt is what "the settled-tree reviews absorb" (`:409-411`). Superseded: `GwzM5-8ThinA1Amendment.md:43-55` (§1) routes the v1_lifecycle P2/P3/P4 acceptance debt to the **A1 activation review**, which must either "(a) subsume a settled-tree acceptance of the `v1_lifecycle` tree with an operator-signed named-residual disposition for the D3/D4 classes, or (b) treat them as blocking per L1-19"; binding restated at `:242-244`. Record verified present — §8 item 1. |

**Table 1 tally (14 rows): PASS 8 · FAIL 3 · RE-FRAMED 1 · DEFERRED-BY-AMENDMENT 1 · PENDING BY DESIGN 1.**

---

## 5. Obligation-by-obligation inventory (O1-O9)

All symbol cites in checklist §3 were re-resolved at `b91bdeb`. **Every cited
symbol still exists; four line numbers moved (§7); no cite is GONE.**

| # | Obligation (`GwzM5-8Refactor.md:2248-2268`) | Verdict | Evidence |
| --- | --- | --- | --- |
| O1 | member audit entries separating integration ref, final checkout, exact lock membership/row | **PASS** | `MemberAcceptanceV1` at `merge/model/v1/acceptance.rs:46`, `Selected { integration, final_checkout, lock_member }` `:47-51`, `UnselectedPresent` `:52-54`, `Absent` `:55` — cite `:46-56` exact. Suites: `validate::acceptance_tests` incl. `unselected_present_and_manifest_only_absent_rows_are_distinct` (`model/v1/validate/acceptance_tests.rs:334`); `finalization::input_tests` 6; `tests/acceptance.rs` 4 incl. `builder_accounts_for_unselected_present_and_intentionally_absent_members` (`:38`); `first_acceptance_write_preserves_derived_lock_member_extensions` (`tests/store.rs:142`). All green. |
| O2 | born and attached-unborn accepted roots without sentinels, M4 checked first-commit/rollback retained | **PASS** | `AcceptedRootBaseV1` at `acceptance.rs:100` — closed enum `BornAttached` `:101-104`, `BornDetached` `:105-107`, `UnbornAttached` `:108-110`; no sentinel. Suites: g23 `root_start` 9, `root_abort` 6, `root_recovery` 3, `characterization_v0.rs:397`; v1 `finalization::root_tests` 6, `reverse::rollback` 29. Green. |
| O3 | persist the complete immutable accepted workspace before classifying publication need; consume only its exact lock YAML after restart | **PASS** | `AcceptedWorkspaceV1` `:10-16`, `AcceptedLockV1 { exact_yaml: :39, sha256: :40 }` `:38-41`; `Serialize` is `#[cfg_attr(test, derive(serde::Serialize))]` throughout — the cfg(test)-gating claim **holds**. `concrete_finalizer_freezes_acceptance_and_publishes_exact_candidate` (`v1_lifecycle/tests/finalization.rs:15`) asserting published lock == `accepted.lock.exact_yaml` at `:44-47`; `restart_reconciles_every_owned_publication_mutation_prefix` `:157`; adapted assertions `g23/compatibility_v0.rs:444/:451`. Green. |
| O4 | deterministic no-publication as complete publication progress, no candidate or output evidence | **PASS** | `model/v1/validate/publication.rs` + `publication_tests`; `no_change_finalization_freezes_acceptance_without_physical_publication` (`tests/finalization.rs:94`); `characterization_v0.rs:397,457`; registry cases `unchanged/no-publication-finalizing` (fixture corpus, confirmed present) and `recovery/no-publication` (valid-unlisted corpus). Green inside `model::v1::` **69** and g23 111. |
| O5 | typed owner before every mutation; result accepted only through the matching opaque exact-observation proof | **PASS** | `tests/forward.rs` **22** (was 16), `effect` 8 / `effect_retry` 2, `tests/authority.rs` 7 incl. `bound_payload_rejects_stale_record_and_value_tampering` (`:34`) and `every_authority_binding_rejects_an_identical_record_from_another_root` (`:58`), `authority::observe` 5 (`…/finalization/publication.rs:684`, `…/publication/live.rs:190`), the four dispatcher suites 18, service 5 + sequence 4, `contention_and_wrong_root_are_rejected_before_mutation` (`tests/store.rs:212`), `reverse::preservation` 36, `reverse::rollback` 29. Green. |
| O6 | exact phase edges rather than ordinal publication progress or generic old/new validation | **PASS** | `predecessor_matrix` 3, all three by name: `every_transition_variant_executes_its_declared_footprint` (`:60`), `every_operation_transition_accepts_only_its_listed_predecessor_states` (`:105`), `publication_physical_action_phase_matrix_is_closed` (`:131`). `model::v1::validate::{lifecycle,journal,canonical}_tests` + vocabulary-closure suites, green inside `model::v1::` 69. |
| O7 | keep v0/v1 mutation services separate; `v0_common_view` removed; borrowed non-serializable inputs | **PASS** | `grep -rn v0_common_view src/` → **0**. Replacement seam confirmed: `build_v1_acceptance` at `merge/acceptance/v1.rs:52` over `RecordView<'a>` (`:248-253`) with `&'a` fields (`:20-21`). Separation: `finalize_dispatch.rs:20` generic over `MergeStore` lives outside `v1_lifecycle/`, which has **0** `MergeStore`/`FileMergeStore` references at all. The *mechanical guard* is still absent — W2 / row 2.6b. |
| O8 | byte-equivalent lock/candidate/root/archive output and identical restart actions for every M4 **and** all seven adapted-v0 scenarios | **FAIL** | Split. Adapted-v0 half: PASS (row 2.3a). "Every M4" half: **FAIL** (row 2.3b / W4) — the enumeration does not exist, so the clause is not checkable. Restart-action identity for adapted cases: `assert_v1_restart` (`g23/atomic_upgrade_v0.rs:341`); for v0 scenarios `abort_recovery` 7, `root_recovery` 3, `continue_merge` 6 — all green, but they are not the missing enumeration. |
| O9 | no skip behavior; selected and unselected M4 handling remains exact | **PASS** | `m4_matrix.rs:51,119,155` — three mixed selected/unselected tests, all cites exact, green. Model rows `UnselectedPresent`/`Absent`; `builder_accounts_for_unselected_present_and_intentionally_absent_members`; archive retention `unavailable_projection_retains_unselected_rows_and_still_rejects_their_mutation` (`record_wire/archive/tests/v0.rs:130`); phase-skip rejection under O6. |

**Table 2 tally (9 rows): PASS 8 · FAIL 1** (O8, same root defect as row 2.3b
— counted once in §9's headline).

**Bonus evidence for the R4b exit condition** (`TransitionDesign:1526-1527`,
"The production v1 writer and R3 migration dispatch must still be
unreachable"): mechanically proven, not argued —
`src/workspace_ops/merge/mod.rs:23-24` is `#[cfg(test)] mod v1_lifecycle;`.
The entire v1 lifecycle tree is compiled out of production builds. The same
line discharges the thin-A1 unwaived item "v1 remains `cfg(test)` until the
A1 activation review" (`GwzM5-8ThinA1Amendment.md:231-232`). Companion:
`recover_or_create` has **no production caller** — the only callers are
`checked_artifact/bootstrap/managed/tests_provider.rs:168,180` and
`checked_artifact/catalog/bootstrap/tests.rs:147,194`, with the definition at
`catalog/bootstrap.rs:213` and a documented unactivated-owner allow at
`catalog.rs:12`. The A1 coexistence gate is fail-closed in code, as the tuple
claims (`GwzM5-8R2DSettledTuple.md:683-686`).

---

## 6. Pre-G work items W1-W5, re-verified

| # | Work item | Verdict | Status at `b91bdeb` |
| --- | --- | --- | --- |
| W1 | privacy compile probe (`PreparedV1Rewrite` / proof tokens / raw v1 writer) | **FAIL** | Absent, unchanged. Zero `compile_fail`/`trybuild` anywhere; 22 compiler probes exist in `scripts/checks/test_check_checked_artifact_boundaries.py` and **none** names `PreparedV1Rewrite`, a proof token, or `store/rewrite.rs`. The churn note "sequence after R3" is now moot — R3 is post-A1, so **W1 no longer has a reason to wait** (see §9, J-4). |
| W2 | v1→v0 call-graph check | **FAIL** | Absent, unchanged. Underlying property now provably *stronger* (0 hits including tests) and therefore cheaper to pin than when drafted. |
| W3 | rollback-prefix privacy/call-graph gate (`RollbackPrefixIssuer`) | **DEFERRED-BY-AMENDMENT** | `grep -rn RollbackPrefixIssuer src/` → **0**. Owned by RemPlan-4 R3 step 6 (`:1087-1101`, exit criterion 12). R3 is superseded off the A1 path by `GwzM5-8ThinA1Amendment.md:73-78`/`:79-82`/`:113-119`. The current seam remains `pub(super) fn issue_verified_rollback_prefix` (`…/observe/reverse/rollback_prefix.rs:240`, cite exact) — visibility-private, unprobed. **R4b-G cannot verify a gate its own amendment moved past A1; it can only record that the seam is private today.** |
| W4 | "every M4 scenario" enumeration | **FAIL** | Absent, unchanged. Doc-only work; nothing blocks it. |
| W5 | aggregate gate wiring | **FAIL (half-discharged)** | No driver, no gate manifest: `gwz-core/scripts/checks/` holds exactly the four checkers, their four suites, `check_lane_commits.sh`, and `merge_docs_manifest.json` — no meta-gate artifact; no workspace-root `scripts/`. The **recorded-checklist** half of W5 is discharged by §3-§6 of this document (every gate named, its command given, its result recorded on the settled tree, counts re-baselined). The **mechanised-driver** half — the ≤150 test/tool lines the ChangeBudget row reserves — remains unspent. |

**Table 3 tally (5 rows): FAIL 4 · DEFERRED-BY-AMENDMENT 1.**

---

## 7. Where the checklist itself is wrong or stale

The R4b-G review should not carry these forward.

### 7.1 Two claims that were wrong at drafting time — not drift

1. **`clippy.toml` "21 entries"** (`GwzM5-8R4bG-EvidenceInventory.md:347`) —
   **there are 19.** `grep -c 'path ='` → 19 at `b91bdeb` *and* at the
   draft's own tree `43c37bc`; `git diff 43c37bc b91bdeb -- clippy.toml` is
   empty. The document's own inline enumeration also sums to 19
   (5 `std::fs` + `File::create` + `OpenOptions::open` + 2 `io::Write` +
   5 `cap_std` + 2 `git2` + 3 gwz-core).
2. **"the seven-member `FinalizationFault` enumeration at
   `src/workspace_ops/tests/g23/finalization.rs:4-13`"** (`:161-162`) — the
   **enum has eight variants** (`g23/fixtures.rs:15-24`:
   `AfterEnteringFinalizing, BeforeCandidateCreation,
   AfterCandidatePersistence, AfterEvidenceCommit, AfterEvidencePersistence,
   AfterLockPublication, AfterNoPublicationComplete, BeforeArchive`). The
   cited line range resolves, but it holds a **seven-element array literal**
   that omits `AfterNoPublicationComplete`; that eighth variant is driven
   separately at `characterization_v0.rs:467`. Also true at `43c37bc`.

### 7.2 Line-number drift (symbol intact, cite moved)

| Cite as drafted | Current | Δ |
| --- | --- | --- |
| `check_checked_artifact_boundaries.py:746` (`source_tree_digest`) | `:789` | +43 |
| `check_checked_artifact_boundaries.py:765` (`calls()`) | `:808` | +43 |
| `check_checked_artifact_boundaries.py:802` (forbid verification) | `:845` | +43 |
| `v1_lifecycle/authority.rs:710` ("cannot construct" comment) | `:716` | +6 |
| `GwzM5-8R4bTransitionDesign.md:1170` (packages table) | `:1335` | +165 |
| `…TransitionDesign.md:1303` (unknown survivor battery) | `:1468` | +165 |
| `…TransitionDesign.md:1304-1307` (fault battery) | `:1469-1472` | +165 |
| `…TransitionDesign.md:1311-1312` (byte-equivalence battery) | `:1476-1477` | +165 |
| `…TransitionDesign.md:1313-1314` (privacy battery) | `:1478-1479` | +165 |
| `…TransitionDesign.md:1315-1316` (call-graph battery) | `:1480-1481` | +165 |
| `…TransitionDesign.md:1341-1348` (R4b exit condition) | `:1522-1527` | +181 |
| `GwzM5-8ChangeBudget.md:553-556` | `:553-556` | 0 (verified exact) |

Also: the workflow paths in §2.2/§2.5/§2.6 are **gwz-core-relative** —
`/Users/owebeeone/limbo/gwz-dev/.github` does not exist; all workflows live
in `gwz-core/.github/workflows/`.

### 7.3 Count re-baseline (the draft's figures were taken at 1350 lib tests)

| Group | Draft | At `b91bdeb` |
| --- | --- | --- |
| lib total (listed) | 1350 | **1570** (1569 pass + 1 ignored) |
| `checked_artifact::` | 244 | **400** |
| `workspace_ops::merge::v1_lifecycle::` | 225 | **255** |
| `workspace_ops::tests::g23::` | 108 | **111** |
| `record_wire::` | 65 | **75** (archive 24→29, unknown_fields 23→28, decode 18) |
| `model::v1::` | 56 | **69** |
| `forward` | 16 | **22** |
| `prefixed_preservation` | 3 | **7** |
| `reverse::preservation` | 32 | **36** |
| `archive::gc_tests` | 6 | **7** |
| gwz-py `test_merge_cli_cross_driver.py` | 4 | **14** |

Nine v1_lifecycle suites exist that the draft does not name: `archive_result`
(1), `checked` (1), `journal_vocabulary` (2), `no_ff_determinism` (4),
`no_ff_wire` (6), `reducer` (4), `retirement` (1), `reverse_no_ff` (5),
`vocabulary` (1). Six of these are M5b's.

**One structural trap for whoever writes W5's driver.** Ten v1_lifecycle test
files are mounted **outside** the `v1_lifecycle::tests` module via `#[path]`
on their production modules — `finalization::{tests,input_tests,root_tests}`,
`archive::{archive_tests,gc_tests}`, `service::{tests,sequence_tests}`,
`status::tests`, `reverse::preservation::tests`,
`reverse::rollback::tests`. A filter of `v1_lifecycle::tests` silently misses
all of them (roughly 105 tests). The partitions in §3.1 use
`v1_lifecycle::` — the module prefix, not `::tests` — and are therefore
complete; the arithmetic in §3.1 proves it.

### 7.4 Frame statements re-read under thin A1

| # | Checklist text | Verdict | Superseding clause / thin-A1 equivalent |
| --- | --- | --- | --- |
| F1 | §1.3 bullet quoting `RemPlan-4:1140-1145` and `:1238` — "R4b-G resumes only when both report no open P0/P1/P2" / "Only then does step 4 … begin" (`:121-126`) | **RE-FRAMED** | `GwzM5-8ThinA1Amendment.md:73-78` (§2.2), `:79-82` (§2.3). The R6 pair remains the hardening chain's own acceptance gate; R4b-G no longer waits on it. The banner pass moved the RemPlan-4 targets to `:12-14`, `:1149-1150`, `:1245`. |
| F2 | §1.3 bullet restating the SCOPE CORRECTION 2026-08-16 — "the gate to R4b-G is **RemPlan-4's R2-R6 chain**" (`:128-130`) | **RE-FRAMED** | `GwzM5-8ThinA1Amendment.md:83-91` (§2.4) — replaced in the amendment's own checkpoint commit. The checklist's banner (`:3-9`) already names `:121` as superseded; the text at `:128-130` is the post-banner location of that restatement. |
| F3 | §5 preconditions — "RemPlan-4 exit criteria 1-14 closed", "The R5 settled gate … will already have run once" (`:550-556`) | **RE-FRAMED** | `GwzM5-8ThinA1Amendment.md:29-34` (§1) and `:236-239` (§4). **Thin-A1 preconditions, both SATISFIED:** R2-D settled at `b91bdeb` (GO/GO, `CurrentProgramCheckpoint.md:606-631`) and M5b's bound proofs green on the settled tree (T-6 + clean-tree re-cut, `:110-118`). Exit criteria 1-14 and the R5 settled gate are **not** R4b-G preconditions on the A1 path. |
| F4 | §6 churn risk — per-lane exposure for R2-D / R3 / R4 / R5 / R6 (`:605-652`) | **RE-FRAMED** | The R2-D leg **realized** (counts re-baselined, §7.3; `checked_artifact::` 244→400 exactly as forecast). R3/R4/R5/R6 churn is post-A1 per `GwzM5-8ThinA1Amendment.md:29-34`. Consequence the review must hold: **the counts recorded here are stable for the A1 path but are NOT final for the hardening chain** — R4's executable-C7 re-key will delete `c7_matrix`/dispatcher-matrix rows (`RemPlan-4:1111-1112`), and R5 regenerates all platform evidence. |
| F5 | §2.5 tail — the R3-owned reverse-prefix privacy gate (`RollbackPrefixIssuer`) (`:320-329`) | **DEFERRED-BY-AMENDMENT** | Same as W3; counted once. |
| F6 | Header — "Verified tree: gwz-core HEAD `43c37bc` … 1350 lib tests … six uncommitted in-flight modifications … boundary checker is locally RED" (`:21-40`) | **RE-FRAMED** | Wholly superseded. Tree is `b91bdeb`, **clean**, boundary checker `ok`, 1570 listed tests. The draft's dirty-tree caveat and its RED-checker explanation are historical and must not be read as a current finding. |

**Table 5 tally (6 rows, F5 counted with W3): RE-FRAMED 5 · DEFERRED 1 (dup).**

### 7.5 The §5 runbook, step by step

| Step | Verdict | Result |
| --- | --- | --- |
| 1 — mechanical doc/compat gates | **PASS** | `check_merge_docs.py` ok (11 sources, 147 assertions); `check_merge_compatibility_predicates.py` 7/7. Both suites green. |
| 2 — boundary + call-graph gates | **PASS** | Checker `ok`; 65/0 unit suite (438.5 s); `test_release_boundary.py` 6/0; clippy `-p gwz-core -D warnings` from clean, zero diagnostics. The W2 check does not exist (row 2.6b). |
| 3 — privacy probes | **FAIL** | W1 absent; W3 deferred. Nothing to run. |
| 4 — full test battery | **PASS** | 1569/0/1 lib in four partitions + 50/0 integration. Counts re-baselined (§7.3). |
| 5 — format/lint | **FAIL as written** | `cargo fmt --all -- --check` clean. The clippy command **as the runbook states it** exits 101 (§3.3). Gwz-core-scoped form green. |
| 6 — protocol + parity | **PASS** | `regen.py --check` OK (the tuple's deferred gate, now executed); `d0_roundtrip_check.py` all passed; gwz-py 330/0 with `regen_protocol.py --check` OK; gwz-cli 139/0 (§6 below is the detail). |
| 7 — CI platform evidence | **FAIL** | Boundary + retained-readers (incl. the 5-platform matrix) green **at `b91bdeb`**. **No Windows-matrix and no Platform-matrix run exists at `d45458d` or `b91bdeb`** — last is run 21 at `514f8e6`. `linux-identity-probe` last ran at `f36d20d`. The R5 exact-tree three-platform release jobs are DEFERRED-BY-AMENDMENT off the A1 gate (`ThinA1:236-239`), but the program's own Windows/platform matrix at the settled tree is **not** an R5 job and is missing. |
| 8 — ledger/LOC audit | **PASS** | R4b-G row `ChangeBudget.md:515` = ≤150 test/tool, **0 production**, ≤3 unique production-bearing files, ≤12. This document spends **0** of all four: it is a dev-doc, no `src/` or `scripts/` change. The wiring budget (W5) is entirely unspent. |
| 9 — aggregate gate record | **PASS** | This document. |
| 10 — two independent full-tree R4b reviews | **PENDING BY DESIGN** | Same as row 2.7b; counted once. |

**Table 4 tally (10 rows, steps 3 and 10 deduped against W1/W3 and 2.7b):
PASS 6 · FAIL 2 (steps 5, 7).**

---

## 8. A1-activation inputs — record verification (not execution)

The amendment routes four things off R4b-G and onto the A1 activation
review. This section verifies each has a record for that review to consume.
**None of these was executed here.**

### Item 1 — the v1_lifecycle P2/P3/P4 acceptance debt (~3.5k lines)

**PRESENT** (checkpoint side PARTIAL).

- Routing clause: `GwzM5-8ThinA1Amendment.md:43-55` (§1, "Also part of the
  named residual (ReviewSafety P1-1): the R4b lifecycle acceptance debt"),
  with the (a)/(b) branch at `:51-55`. Binding restatement at `:242-244`.
- The figure's source: `GwzM5-8ProgressReviewF5.md:56-59` — "P2 (rollback,
  ~1,941 lines), P3 (status/protocol, ~1,096), P4 (archive/GC, ~508) are
  implemented but **not yet independently accepted** — ~3.5k production lines
  of review debt ahead of R4b-G's settled-tree double review." (1,941 + 1,096
  + 508 = 3,545; the arithmetic checks.) Also `:426-428`.
- The rule it routes under: `AgentProcessRules.md:392-401` (L1-19, "Use one
  severity and verdict contract") — "**A gate is `NO-GO` while any P0, P1, or
  P2 is open.**" This makes branch (b) exact.
- Corroboration: `GwzM5-8ThinA1Amendment-ReviewSafety.md:120-182` (the P1-1
  finding) and `:382-398` (round-2 RESOLVED).
- **Gap, minor:** `CurrentProgramCheckpoint.md` carries **no standalone
  standing-input entry**. The only forward-facing pointer is `:90-91`
  (M5b's stopped abandonment item "routed to the R4b acceptance-debt surface
  at the A1 activation review"), which presumes the surface exists without
  defining it; `:756-758` is the historical finding, not a register. The
  durable record is the amendment; the checkpoint is a pointer.
- **Also worth the A1 review's attention:** §4's compressed phrase "explicit
  inputs, per L1-19" is not literally what L1-19 says (L1-19 is the
  severity/verdict contract, not an inputs rule). §1's (a)/(b) branch is what
  makes the sentence coherent.

### Item 2 — the D3/D4 named-residual disposition

**PRESENT as an obligation; PARTIAL on independent restatement.**

Nomenclature, since two different "D3"s are live in this corpus:
- **R4b D3/D4** — RemPlan-4's consolidated interface blockers
  (`GwzM5-8R4bP1P2-RemPlan-4.md:64-65`). **These are the item's subjects.**
- **A1 decision-packet D3** — the durable preservation cursor
  (`GwzM5-8A1DecisionPacket.md:36`), **already landed and accepted** at
  gwz-core `8b83a2c`, dual GO/GO (`CurrentProgramCheckpoint.md:194-220`).
  Different object, already closed.

Records:
1. `GwzM5-8ThinA1Amendment.md:47-55` — the controlling clause naming
   "dirty-boolean recovery predicates; `preservation/cursor.rs:277-345`".
2. `GwzM5-8ThinA1Amendment-ReviewSafety.md:133-155` — **the only place the
   mechanism is traced site-by-site**; failure scenario at `:157-164`;
   round-2 disposition at `:382-398`.
3. Scheduling source: `RemPlan-4:1100` (R3 item 3, "remove global dirty
   booleans as recovery authority") and `:1111` (R4 item 1).

**Live-code check.** The predicates are still live at
`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/
preservation/cursor.rs`: `stash_complete` (`:283-308`) and `reset_complete`
(`:310-335`) now open with D3 durable-marker fast paths and then fall through
to the surviving `…dirty == GitPreservationDirtySummary::default()` checks at
`:305-306` and `:332-333`; companions at `…/preservation/phase.rs:187,198`
and `…/observe/finalization.rs:217`. A third dirty-equality site now exists
at `cursor.rs:498`, outside the cited range. **The amendment's `277-345`
range still brackets the predicates; the Safety review's per-line cites
(`:277/:280/:302/:345`) predate the D3 landing and no longer resolve
one-to-one.**

### Item 3 — the D3 perf pricing (root_fault_matrix, "~343 s release, priced not optimized")

**MISSING as a record — but now measured here.**

An exhaustive search of `dev-docs/`, `gwz-core/dev-docs/`, `gwz-core/src`,
`gwz-cli`, `gwz-py`, `dev-docs/history/`, plus `git log -S` over full history
and `--grep=343`, finds **no 343-second figure and no occurrence of "priced
not optimized"** anywhere in this workspace. Every `343` in the corpus is a
line-number citation, a hex fragment of the ext4 UUID
`718c918e3cc343c9ae9e27f5cecc8a17`, or a commit-SHA fragment.

What *is* on record:

| Figure | Location | Build |
| --- | --- | --- |
| **576 s** | `GwzM5-8D3Impl-ReviewState.md:552-555` — "the 576s `root_fault_matrix` (implementer's partition B ran it against the prebuilt **release** binary…)" | release, explicit |
| **576 s** | `GwzM5-8D3Impl-ReviewCode.md:435,445-446` (§ heading "Gates (re-executed by me, **release**, round-2 binary)") | release |
| **>585 s** | `CurrentProgramCheckpoint.md:739-743`; `GwzM5-8R2DInterfaceFreeze.md:1715-1718`; `GwzM5-8M5bImpl-ReviewCode.md:278-281` | not stated |

**Measured for this document: 318.71 s, release, at `b91bdeb`, green** (§3.1
tail). This is now the citable figure. The R4b-G review should decide whether
the pricing record the A1 review is meant to consume is (a) the 576 s D3
figure, (b) this 318.71 s re-measurement, or (c) a record that must still be
written — and note that the discrepancy is large enough (576 → 319) that
"priced, not optimized" would be quoting a number nobody re-checked.

### Item 4 — the settled tuple's open-item register

**PRESENT, complete and current at the round-2 remediation.**

`GwzM5-8R2DSettledTuple.md:634-711` (§11, running to EOF): §11.1 base table
6 rows (`:636-645`) + round-2 additions block 5 bullets (`:647-668`, header
verbatim "**Added at the settled-dual round-2 remediation (2026-08-23):**");
§11.2 six rows (`:670-679`); §11.3 eight numbered caveats (`:681-711`).
**25 items.** The additions landed at `5f090eb` ("Settled-dual round-2
remediation: the frozen map is made true"); no later commit touches the file.
This matches `CurrentProgramCheckpoint.md:617-621` ("the register
completed").

The `runtime.*` adjudication — "RULED RESTATE with drafted text" — landed in
the freeze document: `GwzM5-8R2DInterfaceFreeze.md:426-479`, §3.5, with the
row at `:433` and the restatement block-quote at `:434-479`, whose governing
sentence is "**The family's 'executed' status is a family-level claim on a
disjoint mechanism, and it claims no per-key evidence.**"

**Two staleness notes for the register's consumer.** (i) The tuple's §4.6
item 3 (`:324-329`) and §11.3(3) (`:692-694`) still carry the
pre-adjudication framing ("Open question for the 5.2 dual"; "The dual should
rule") with **no back-pointer** to the ruling — a reader landing there alone
would think the item is still open. It is closed at
`GwzM5-8R2DInterfaceFreeze.md:434-479`. (ii) §11.3(7) (the CI-unwired L2-05
gates) is cross-filed as a §11.1 round-2 addition (`:666-668`) and
double-counts on a naive tally.

**Independently confirmed here:** the L2-05 merge-doc gates really are wired
into **no workflow.** `grep -rn 'check_merge_docs\|test_check_merge_docs'
gwz-core/.github/workflows/` returns nothing; only
`check_merge_compatibility_predicates.py` appears, at
`retained-readers.yml:53`. The tuple's §11.3(7) verdict — "A gate that only
passes on a developer's machine is not a gate" — holds at `b91bdeb`.

---

## 9. What the R4b-G review must judge

### 9.1 Headline

**38 distinct inventory rows executed** (44 raw rows across the five tables,
6 exact duplicates deduped: O8≡2.3b, W1≡2.5b, W2≡2.6b, F5≡W3, S3≡W1+W3,
S10≡2.7b).

| Verdict | Count |
| --- | --- |
| **PASS** | **22** |
| **FAIL** | **7** |
| **RE-FRAMED** | **6** |
| **DEFERRED-BY-AMENDMENT** | **2** |
| **PENDING BY DESIGN** | **1** |

Every RE-FRAMED row has its thin-A1 equivalent named, and **five of the six
have that equivalent in evidence** (F1/2.7a → the R2-D 5.2 settled dual,
GO/GO at `b91bdeb`; F2 → the amendment's own checkpoint rewrite; F3 → R2-D
settled ✓ and M5b bound proofs ✓; F4 → the R2-D churn leg realized and
measured; F6 → this document's tree identity). **F4 is the one RE-FRAME whose
equivalent is only half-evidenced** — see F-8.

### 9.2 The seven FAILs, unsoftened

| ID | FAIL | Root | Owner under thin A1 |
| --- | --- | --- | --- |
| **F-1** | Row 2.3b / O8 — "every M4 scenario" byte-equivalence clause is **not checkable**: no enumeration of the M4 scenario set mapped to equivalence evidence exists. | W4 | R4b-G. Doc-only, unblocked, cheap. An R4b obligation quoted verbatim from `GwzM5-8Refactor.md:2265` cannot be confirmed today. |
| **F-2** | Row 2.5b / W1 — no privacy compile probe. Visibility gives the property; **nothing proves it fails closed** the way 22 sibling probes do for the checked-artifact boundary. Battery source `TransitionDesign:1478-1479` is unmet. | W1 | R4b-G. Fits the ≤1,500 test/tool line budget. |
| **F-3** | Row 2.6b / W2 — no mechanical assertion that v1 lifecycle modules contain no raw v0 persistence call. Battery source `TransitionDesign:1480-1481` is unmet. | W2 | R4b-G. Property is 0-hit clean today, so the pin is trivial to write and trivial to break silently without it. |
| **F-4** | W5 — no aggregate gate driver. The **record** half is discharged by this document; the **mechanised** half (≤150 lines, `ChangeBudget.md:515`) is unspent. | W5 | R4b-G. This is literally the only material the R4b-G row's budget is for. |
| **F-5** | Runbook step 5 — `CLIPPY_CONF_DIR=$PWD cargo clippy --workspace --all-targets --all-features -- -D warnings` **exits 101** with 7 `disallowed_methods` errors in gwz-cli: 2 production (`gwz-cli/src/pager.rs:127,131`, `io::Write::write_all`) and 5 test (`src/tests/g01/commands.rs:460`; `src/tests/m2c.rs:51,55,85,99`). | runbook command scope | R4b-G. **Not a gwz-core defect.** Either correct the runbook to `-p gwz-core` (matching CI) or make an explicit decision to extend the writer perimeter to gwz-cli. Do not silently drop the step. |
| **F-6** | Runbook step 7 — **no Windows-matrix and no Platform-matrix run exists at the settled tuple.** Last is run 21 at `514f8e6`, three commits back (`d45458d`, `b91bdeb` uncovered). `linux-identity-probe` last ran at `f36d20d`. | dispatch not issued | R4b-G to decide: dispatch, or record the deferral with the risk named. The tuple already owes this as a round-2 ledger duty (`GwzM5-8R2DSettledTuple.md:663-665`) — scoped there to "the ten multi-component writer rows"; the CI record shows the gap is **the whole settled tree**, not ten rows. |
| **F-7** | §8 item 3 — the **D3 perf pricing record does not exist.** No "~343 s release", no "priced not optimized", anywhere in the repo or its history. | record never written | The A1 activation review is expected to consume a figure that is not filed. This document supplies **318.71 s release at `b91bdeb`**; the nearest filed figure is **576 s release** (`GwzM5-8D3Impl-ReviewState.md:553`). |

### 9.3 Judgment calls

**J-1 — M5b-IMPL merged ahead of R4b-G. The single largest call.**
The frozen dependency statement (`GwzM5-8M5bNoFfDesign.md:976-986`) puts
"Merging and settled-review of M5b-IMPL" under "**Dependent on R4b-G
settling — must wait**", and `CurrentProgramCheckpoint.md:1092` restates it
as live current-state text. **The facts:** `3e60529` ("Park the M5b no-ff
semantics package pre-review") and `8c1624a` ("Accept the M5b package
(GO/GO)") are both ancestors of `b91bdeb` and both on `origin/main` — merged
and pushed 2026-08-22, ahead of both R4b-G and the R2-D settle (2026-08-23).

- **Scope of the amendment's supersession.** §2.10 supersedes the design's
  `:987-989` *dependency-chain* sentence ("R4b-G is behind the R2-R6
  chain"), and §2.9 re-sequences "R4b-G, M5b, and A1 now follow R2-D settle"
  — a parallel reading, not an ordering of M5b behind R4b-G. But **neither
  clause names the `:976-986` must-wait bullet**, and `:1092` is still live
  checkpoint text.
- **What mitigates it.** M5b landed **0 production lines** (measured
  independently in the tuple's own LOC audit,
  `GwzM5-8R2DSettledTuple.md:375-376`, rows 3-4: prod net 0 for both
  commits), under a ratified zero-production-line ceiling, with a round-1
  clean dual GO/GO (`CurrentProgramCheckpoint.md:95-118`). Design reason (b)
  — "would give R4b-G's settled-tree/call-graph/byte-equivalence gates a
  moving target" — is therefore **materially defused for the production
  surface**: those three gates read production code, and none moved. The test
  surface did move (six new M5b suites, §7.3), which is what the count
  re-baseline in this document exists to absorb.
- **What is not mitigated.** Reason (c) stands unaddressed: "M5b's own
  unreachability argument leans on R4b-G's call-graph gate (§6 item 4)" —
  and the call-graph gate's v1→v0 half (W2/F-3) **does not exist**. M5b's
  unreachability argument is currently leaning on an absent gate.
- **The review must rule:** ratify the ordering as superseded (and have the
  lane strike `CurrentProgramCheckpoint.md:1092` and annotate the design's
  `:976-986`), or record it as a deviation under L1-16. Either way the
  reason-(c) dependency needs an answer.

**J-2 — Does the R2-D 5.2 settled dual discharge row 2.7a?** The checklist
treats RemPlan-4 R6 as G's precondition. Thin A1 substitutes the R2-D Phase
5.2 settled dual, which is GO/GO at `b91bdeb` with zero open P0/P1/P2 — but
it is a **checked-artifact** settled-tree review, exactly as R6 was, and
R4b-G's own pair is the **full R4b tree**. The review must confirm that the
substitution is like-for-like at the checked-artifact layer and that nothing
in the R4b tree outside `checked_artifact/` inherited an unreviewed
precondition from R6's cancellation.

**J-3 — The clippy perimeter (F-5).** `-p gwz-core` (match CI, correct the
runbook) or extend the writer perimeter to gwz-cli? Five of the seven hits
are test fixtures; the two production hits are `pager.rs:127,131` writing
paged output to a stdout handle — not durable-store mutations, and exactly
the class `clippy.toml`'s `io::Write::write*` entries exist to catch *inside
gwz-core's merge tree*. gwz-cli is a CLI driver, not a durable-store owner.
The cheap answer is almost certainly the right one, but it is a scope
decision, not a typo fix, and the step must not be silently dropped.

**J-4 — W1's sequencing note is now moot.** The checklist says "Sequence
after R3 (see churn note)" (`:661-662`). R3 is post-A1. **W1 no longer has a
reason to wait**, and the R3 churn it was hedging against will not arrive
before A1. If the review accepts that, W1 becomes plainly buildable now, and
F-2 loses its only excuse.

**J-5 — What R4b-G can even say about W3.** `RollbackPrefixIssuer` does not
exist; R3 owns it; R3 is post-A1. R4b-G "verifies it" per the checklist
(`:668-671`), but there is nothing to verify. The honest disposition is a
recorded DEFERRED with the current seam's privacy stated
(`pub(super) fn issue_verified_rollback_prefix`,
`…/observe/reverse/rollback_prefix.rs:240`) — which is what this document
does. The review should confirm that is the intended reading rather than an
omission.

**J-6 — Bookkeeping the review should have closed.** (i) The checkpoint's
§"Exact tuple" table (`:789-804`) still names gwz-core `d32b2c9`; the live
pin is `b91bdeb`. (ii) The checklist's two factual errors (§7.1) and eleven
drifted cites (§7.2). (iii) The tuple's §4.6/§11.3(3) `runtime.*` framing
with no back-pointer to its ruling (§8 item 4). (iv) The acceptance-debt
surface has no standing entry in the checkpoint, only a pointer (§8 item 1).
None of these is a correctness defect; all four will mislead the A1
activation review if left.

**J-7 — Two gates that are green only because this host is special.** The
L2-05 merge-doc gates pass here because the sibling `gwz-cli` checkout
exists; they are wired into no workflow (verified, §8 item 4). Likewise
`protocol/regen.py --check` and `d0_roundtrip_check.py` (§3.4) pass here
because `protocol/.regen-venv` and the `taut` checkout are present — both
are green in this document, and neither runs in CI. The review should decide
whether a gate that only a developer machine can run counts as executed
evidence for an aggregate acceptance gate.

### 9.4 What is unambiguously in evidence

For the review's convenience, the load-bearing positives, all executed at
`b91bdeb` on this host today: the twelve-gate settled-tuple train re-runs
green and reconciles to the tuple's own figures exactly (1569/0/1, 400/0,
50/0, 65/0, 86/0, 6/0, 3/0, 14/0, checker `ok`, clippy clean, fmt clean);
two gates the tuple could not run are now executed and green (`regen.py
--check`, `d0_roundtrip_check.py`); CI is green at the exact settled object
for both the boundary workflow and the retained-readers 5-platform matrix;
all three sibling member repos are at their tuple pins with clean trees and
green suites; every symbol cite in the checklist's §2/§3 still resolves; the
production v1 writer is mechanically unreachable
(`merge/mod.rs:23-24`, `#[cfg(test)] mod v1_lifecycle;`) and
`recover_or_create` still has no production caller.

---

## 10. Cross-repo verification

### 10.1 gwz-cli at `3cca145` — no drift

```
$ cargo fmt --all -- --check           → clean, exit 0
$ cargo test -p gwz --all-targets      → exit 0
  unittests src/lib.rs              ok.  80 passed; 0 failed
  unittests src/main.rs             ok.   0 passed; 0 failed
  tests/diff_workflows.rs           ok.  26 passed; 0 failed
  tests/local_workflows.rs          ok.  25 passed; 0 failed
  tests/publish_workflow.rs         ok.   4 passed; 0 failed
  tests/release_script.rs           ok.   2 passed; 0 failed
  tests/rename.rs                   ok.   2 passed; 0 failed
  examples/generate_cli_docs.rs     ok.   0 passed; 0 failed
```

**139 passed, 0 failed** across 8 targets. gwz-cli is a member of the shared
cargo workspace rooted at `/Users/owebeeone/limbo/gwz-dev/Cargo.toml` and
builds against gwz-core `b91bdeb` by path dependency, so this is a genuine
cross-repo integration result at the settled tuple, not an isolated run.

The only gwz-cli negative is F-5, and it is an artifact of the runbook's
clippy invocation, not of gwz-cli's own gates.

### 10.2 gwz-py at `929efb0` — no drift

```
$ python run_tests.py     (gwz-py's own harness, its own .venv, Python 3.12.12)
+ python -m maturin develop
    Finished `dev` profile in 22.26s
  📦 Built wheel for abi3 Python ≥ 3.10 … 🛠 Installed gwz-0.0.0
+ python scripts/regen_protocol.py --check
  regen_protocol: OK
+ python -m pytest src/tests -q
  330 passed in 43.30s
exit=0
```

**330 passed, 0 failed**, across 30 test files. Two cross-repo properties are
proven by this run, not merely by the count:

1. **Native parity against the settled gwz-core.** `maturin develop` rebuilds
   the native extension from gwz-py's `native/` against gwz-core `b91bdeb`
   before pytest runs — the harness comment says why ("Never let a stale
   editable native extension satisfy the Python parity gate").
2. **Protocol currentness across the repo boundary.**
   `scripts/regen_protocol.py --check` regenerates from
   `gwz-core/protocol/gwz.taut.py` using `taut` at `f008419` (via
   `PYTHONPATH`) and byte-compares. Green. This is the one gate that
   exercises three of the four member repos at once.

The §2.9 cross-driver evidence, re-run individually:
`test_merge_cli_cross_driver.py` **14 passed** (the checklist says 4 —
re-baselined), `test_merge_cli_custom_message.py` 5 passed, and the CLI merge
family `test_cli_merge.py`/`test_cli_merge_m3.py`/`test_merge_m2c.py`
22 passed.

**Disclosure (L1-16).** `maturin develop` rewrote `gwz-py/Cargo.lock` as a
side effect — gwz-py's committed lockfile pins `gwz-core 0.10.3`, while the
tuple's gwz-core is `0.10.4` with two new dependencies (`libc`,
`unicode-normalization`). The file was **restored to `HEAD` immediately**;
`gwz-py` is clean. Worth recording as a cross-repo observation for the A1
review: **gwz-py's committed `Cargo.lock` is stale relative to the settled
gwz-core tuple, and building gwz-py against that tuple necessarily rewrites
it.** No gwz-py test depends on the pinned version, and all 330 pass.

### 10.3 taut at `f008419` — no drift

Participates through gwz-py's protocol regeneration (§10.2) and gwz-core's
`protocol/d0_roundtrip_check.py` (§3.4), both green. taut has no separate
gate in the R4b-G inventory.

---

## 11. Provenance

Executed 2026-08-23 on Darwin 25.5.0 / arm64, Python 3.13.7 for gwz-core's
gates and 3.12.12 for gwz-py's, cargo/rustc from the repo toolchain pins.
All cargo invocations used a scratch `CARGO_TARGET_DIR` outside every
repository. The lib suite ran in four disjoint, exhaustive filter partitions
because this session's per-command budget is 600 s and the single-run figure
is ~700 s; the arithmetic reconciliation is in §3.1. No git write operation
was performed. `gwz-py/Cargo.lock` was transiently modified by the project's
own harness and restored (§10.2). This document is the only file written.
