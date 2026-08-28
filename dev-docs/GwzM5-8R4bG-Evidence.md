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

**[J-1 ANNOTATION 2 of 2 — leg 4 restated and the call ruled, 2026-08-24.]**
The R4b-G dual ruled on it at round 1: **J-1 RATIFIED by both axes**
(Correctness §5 J-1 "the adjudication's ACCEPTED-WITH-RECORD stands";
Evidence §6 "not disqualifying"), **the before-A1 remedy not triggered**.
Leg 4 of the adjudication is **restated**, not struck: it read "the settled
dual's Code axis re-verified the M5b surfaces byte-identical", which the
Evidence axis's [P2-1] showed no settled-dual report contains (`M5b`,
`no_ff`, `3e60529`, `8c1624a`: zero hits in
`GwzM5-8R2DSettled-Review{Code,State}.md`). The verification is real but
belongs to **the D3 dual's round-2 re-verdicts**, on both axes —
`GwzM5-8D3Impl-ReviewCode.md:447-448` (`store/tests.rs` at exactly +2/−0,
all five M5b files byte-identical vs `0b059e8`) and
`GwzM5-8D3Impl-ReviewState.md:550-552` (M5b 37/0, all five files
`git status`-clean). **The fact was true; its citation was not.** What this
sub-section names as unblocked is therefore unchanged in substance — the
merge already happened and is accepted with record — but **the M5b-IMPL
settled review this dependency anticipates is still owed, pre-A1**, and is
carried as an owed-pre-A1 row in `GwzM5-8R2DSettledTuple.md` §11.1. Full
restatement and ruling at §9.3 J-1's two annotations.

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
**[Attribution corrected 2026-08-24 (Evidence axis [P3-3]): "the A1 brief" is
wrong — the "~343 s" figure came from the lane owner's session notes, an
out-of-band recollection, not from any brief and not from any corpus document.
See §8 item 3 addendum for the full restatement. The measurement claims in this
sentence are unaffected.]**

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
its own `clippy.toml` plus `#![forbid(clippy::disallowed_methods)]`
**[CORRECTION 2026-08-24 — R4b-G Correctness axis C-6: that clause reads
crate-wide and is not true crate-wide. gwz-core's **crate root carries
`#![allow(clippy::disallowed_methods, …)]`** (`gwz-core/src/lib.rs:4-7`, with
the reason "raw writers are isolated from the checked merge boundary by
module-level structural and compiler probes"). The seal is **8 checker-verified
forbid islands, not a crate-wide forbid**: the files of
`PROTECTED_COMPILER_MODULES` (`scripts/checks/check_checked_artifact_boundaries.py:26-35`),
each verified to contain `#![forbid(clippy::disallowed_methods)]` **through the
masked scan** — `mask_non_code()` is applied before the membership test, so a
commented-out or string-literal forbid cannot satisfy it
(`:902` defines the token, `:994-999` runs the per-module check). Of the 8,
only `workspace_ops/merge/v1_lifecycle/authority/observe.rs` is inside
`v1_lifecycle/`. The true topology is therefore: **crate-level allow + 8
masked-scan-verified forbid islands + the structural digests + the privacy
seals** — which is coherent by design (the raw v1 writer `store/rewrite.rs` is
*supposed* to write and is sealed by privacy, not by lint), but it is not the
standing crate-wide guarantee this sentence implies. This matters to J-3's
optics: v1 test code freely calls `std::fs::write` (e.g. the fault matrix's
`seed_recovery`,
`src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:174`)
while gwz-cli was flagged for the identical call under the imposed conf. No code
change is owed; the wording was the defect.]**. The CI
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
| 2.3b | Byte-equivalence — "every M4 scenario" | **FAIL** | Unchanged from the draft, re-verified at `b91bdeb`: **no document or suite enumerates the M4 scenario set by name and maps it to equivalence evidence.** `GwzMergeM4-RemPlan.md` and `-RemReview.md` mention scenarios only generically; `GwzM5-8Refactor.md` §15.3.3 (now at `:1456`) is "Archived record decoding and evidence-only projection", not an M4 table. The obligation text at `GwzM5-8Refactor.md:2265` is the only "every M4" in dev-docs outside the checklist's own self-references. This is W4. **[2026-08-24: the enumeration now exists — §12, 39 scenarios from `GwzM5-8R0Inventory.md` §4/§5.2, machine-checked by `check_m4_scenario_map.py`. The row's verdict does NOT flip to PASS: with the clause finally checkable, the answer is that it is **not met** — 13/39 scenarios carry a v1 equivalence binding, 22 are UNBOUND, 4 have NO FIXTURE, and the entire ten-shape archive family is unbound although O8 names archive output. Row stays **FAIL**, now with a measured denominator instead of an absent one. §12.6 R-1/R-2.]** |
| 2.4 | Unknown-field gate | **PASS** | `every_transition_effect_commits_its_exact_unknown_manifest` present at `v1_lifecycle/tests/store/matrix.rs:18`; `store/unknown.rs` and `store/rewrite.rs` present; `record_wire::unknown_fields` now **28** tests (was 23), `record_wire::archive` **29** (was 24), `record_wire::tests::decode` 18 — total `record_wire::` 75/0 green; drift-survivor test at `tests/store/drift.rs:16`; extension preservation at `tests/store.rs:142`. |
| 2.5a | Privacy — Rust visibility + boundary-checker probes | **PASS** | `PreparedV1Rewrite` still `pub(super)` at `v1_lifecycle/transition/mod.rs:20` (cite exact); authority/proof-token types `pub(super)` across `v1_lifecycle/authority.rs:28-158` (cite holds as a range); store commit path `pub(super)` at `store/rewrite.rs:15-23` (whole file is `pub(super)`-only). Boundary checker `ok`; its **65-test compiler-probe suite green (438.5 s)**, including `test_approved_outside_source_target_cannot_hide_an_observer_caller`, `test_compiler_rejects_nested_writer_in_authority_observer_tree`, `test_compiler_rejects_writer_in_v1_artifact_observer`, `test_merge_authority_backend_has_a_private_compiler_seal` (22 probe-bearing tests in all). CI green at `b91bdeb`. |
| 2.5b | Privacy — the dedicated `PreparedV1Rewrite` / proof-token / raw-writer probe | **FAIL → PASS 2026-08-24** | As executed 2026-08-23: zero `compile_fail`, zero `trybuild`, zero occurrences of `PreparedV1Rewrite` or any proof-token name in `gwz-core/scripts/`; the only "cannot construct" in `src/` a comment (`v1_lifecycle/authority.rs:716`). **Closed by `scripts/checks/test_v1_lifecycle_privacy_probe.py` (F-2): 5/5 green in 96.95 s, four negative probes plus a positive control, and demonstrated to FAIL under a widened `mod transition` / `pub(crate) struct PreparedV1Rewrite`. Three seals proven distinct — `PreparedV1Rewrite` and the proof tokens against everything outside `v1_lifecycle`, and `store::rewrite::commit` against the rest of the lifecycle too. §13.1.** |
| 2.6a | Call-graph — checked-artifact / writer boundary | **PASS** | Boundary checker `ok (15 visible entries, 5 classified modules)`; structural call-graph inventory intact (`source_tree_digest` now at `check_checked_artifact_boundaries.py:789`, `calls()` at `:808`, the `#![forbid(clippy::disallowed_methods)]` verification at `:845` — all moved +43, see §7). `clippy.toml` writer boundary present with the three gwz-core fall-through writers (`artifact::write_atomic` `:21`, `stash::write_bundle` `:22`, `sync_workspace_boundary` `:23`); clippy `-p gwz-core --all-targets --all-features -D warnings` from clean → zero diagnostics. Release-boundary suite 6/0. CI per-commit lane gate present (`checked-artifact-boundary.yml:23-25`) and green at `b91bdeb`. |
| 2.6b | Call-graph — "v1 lifecycle modules contain no raw v0 persistence call" | **FAIL → PASS 2026-08-24** | The property is **stronger than the draft claimed** — `grep -rn 'MergeStore\|FileMergeStore' src/workspace_ops/merge/v1_lifecycle/` returns **0 hits, including test code** (the draft said "outside test code"). The v0 finalizer remains outside the v1 tree at `merge/finalize_dispatch.rs:20`, `pub(super) fn finalize<B: GitBackend, S: MergeStore>`. **Closed by the masked-token scan added to `check_checked_artifact_boundaries.py` (F-3), which derives the seam from `merge/mod.rs`'s own `use store::{…}` re-exports and scans all 128 `v1_lifecycle/**/*.rs`. Masking is load-bearing: ten `"enter_finalizing"` occurrences in that tree are action-name string literals, so a naive grep reports ten false hits where the masked scan reports the true zero. Fail-closed tests in the checker's own suite, one of them a compiler probe proving the violation compiles before asserting the checker rejects it. §13.1.** |
| 2.7a | Settled-tree review gate, layer 1 — RemPlan-4 R6 checked-artifact settled-tree re-review pair | **RE-FRAMED** | The checklist calls this "a *precondition* of G, owned by the R2-R6 chain" (`:379-382`), citing `RemPlan-4:1140-1145`. **Superseded as an A1/R4b-G gate** by `GwzM5-8ThinA1Amendment.md:73-78` (§2.2) and `:79-82` (§2.3). **Thin-A1 equivalent, and it EXISTS:** the R2-D Phase 5.2 settled dual — "R2-D IS SETTLED — 2026-08-23, dual #3 of 3 consumed, GO/GO at round 2… The settled object is gwz-core `b91bdeb`" (`CurrentProgramCheckpoint.md:606-608`); reports `GwzM5-8R2DSettled-Review{Code,State}.md` (rounds 1+2 appended); round 2 "Code GO unconditional / State GO". Zero open P0/P1/P2. |
| 2.7b | Settled-tree review gate, layer 2 — the two final full-tree R4b reviews | **PENDING BY DESIGN** | `AgentProcessRules.md:2006`; tier dual, cross-model where available. Nothing to produce; **this document is that review's input.** Not satisfied by 2.7a: the R6/5.2 pair is the *checked-artifact* tree; R4b-G's own pair is the *full R4b* tree (~25k lines). The checklist states the distinction at `:126`; it survives the re-frame intact. |
| 2.8 | Shared router integration | **PASS** | `reverse_router` 3, `reverse_entry` 9, `c7_matrix` 7 (incl. `request_dispatch`) — all present by name and green inside the v1_lifecycle 255/0. |
| 2.9a | P3/P4 status/archive cross-driver integration — execution | **PASS** | `v1_lifecycle::status::tests` 12, `archive::archive_tests` 3, `archive::gc_tests` **7** (was 6), `archive_result` 1, `record_wire::archive` **29** (was 24). Python driver side re-executed: `gwz-py/src/tests/test_merge_cli_cross_driver.py` **14 passed** (the checklist says 4 — re-baselined, §7), `test_merge_cli_custom_message.py` 5 passed. |
| 2.9b | P3/P4 — the P2-P4 *acceptance* debt | **DEFERRED-BY-AMENDMENT** | The checklist says the debt is what "the settled-tree reviews absorb" (`:409-411`). Superseded: `GwzM5-8ThinA1Amendment.md:43-55` (§1) routes the v1_lifecycle P2/P3/P4 acceptance debt to the **A1 activation review**, which must either "(a) subsume a settled-tree acceptance of the `v1_lifecycle` tree with an operator-signed named-residual disposition for the D3/D4 classes, or (b) treat them as blocking per L1-19"; binding restated at `:242-244`. Record verified present — §8 item 1. |

**Table 1 tally (14 rows): PASS 8 · FAIL 3 · RE-FRAMED 1 · DEFERRED-BY-AMENDMENT 1 · PENDING BY DESIGN 1.**
**Re-tallied 2026-08-24 after the FAIL remediation: PASS 10 · FAIL 1 (2.3b, now
with a measured denominator) · RE-FRAMED 1 · DEFERRED-BY-AMENDMENT 1 · PENDING
BY DESIGN 1.**

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
| O8 | byte-equivalent lock/candidate/root/archive output and identical restart actions for every M4 **and** all seven adapted-v0 scenarios | **FAIL** | Split. Adapted-v0 half: PASS (row 2.3a). "Every M4" half: **FAIL** (row 2.3b / W4) — the enumeration does not exist, so the clause is not checkable. Restart-action identity for adapted cases: `assert_v1_restart` (`g23/atomic_upgrade_v0.rs:341`); for v0 scenarios `abort_recovery` 7, `root_recovery` 3, `continue_merge` 6 — all green, but they are not the missing enumeration. **[2026-08-24: enumeration filed at §12 and machine-checked. The clause is checkable and the "every M4" half is measured NOT MET at 13/39. O8 stays **FAIL**; what changed is that the failure is now quantified and dispositioned per scenario rather than unknowable. The 7 ADAPTED rows are exactly row 2.3a's seven — the adapted-v0 half and the bound half of the every-M4 half are the same seven cases, which is itself the finding: no M4 scenario outside the seven registry-whitelisted ones has a v1 byte-equivalence assertion at all.]** |
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
| W1 | privacy compile probe (`PreparedV1Rewrite` / proof tokens / raw v1 writer) | **FAIL → LANDED 2026-08-24** | Was absent; zero `compile_fail`/`trybuild` anywhere, and none of the 22 sibling compiler probes named `PreparedV1Rewrite`, a proof token, or `store/rewrite.rs`. **`scripts/checks/test_v1_lifecycle_privacy_probe.py`, 138 lines, 5/5 green.** J-4's reading is adopted: R3 is post-A1, so the "sequence after R3" hedge was spent, not deferred. |
| W2 | v1→v0 call-graph check | **FAIL → LANDED 2026-08-24** | Was absent. **Masked-token scan in `check_checked_artifact_boundaries.py`**, seam derived from `merge/mod.rs`'s re-exports, floor-checked so a restructured re-export fails the derivation rather than silently scanning nothing. Pins the stronger 0-hits-including-tests property. Explicitly NOT subsumed by the `PROTECTED_SOURCE_TREE_DIGESTS` pin on `v1_lifecycle/mod.rs`: that digest says "this tree changed", states no property, and is refreshed whenever the tree legitimately moves. |
| W3 | rollback-prefix privacy/call-graph gate (`RollbackPrefixIssuer`) | **DEFERRED-BY-AMENDMENT** | `grep -rn RollbackPrefixIssuer src/` → **0**. Owned by RemPlan-4 R3 step 6 (`:1087-1101`, exit criterion 12). R3 is superseded off the A1 path by `GwzM5-8ThinA1Amendment.md:73-78`/`:79-82`/`:113-119`. The current seam remains `pub(super) fn issue_verified_rollback_prefix` (`…/observe/reverse/rollback_prefix.rs:240`, cite exact) — visibility-private, unprobed. **R4b-G cannot verify a gate its own amendment moved past A1; it can only record that the seam is private today.** |
| W4 | "every M4 scenario" enumeration | **FAIL → LANDED 2026-08-24** | Was absent. **§12 of this document + `scripts/checks/check_m4_scenario_map.py` (144 lines *[corrected in place 2026-08-24 from "133 lines" per Evidence axis [P3-4](i); `wc -l` over `git show 78badbc:scripts/checks/check_m4_scenario_map.py`, and the `b91bdeb..78badbc` diff, both give 144 at the review object, matching §13.1's table. Counts are pinned to `78badbc`; the sibling tooling lane's post-review work moves the working tree]*).** The work item is discharged; its *result* is a new open finding (13/39 bound), which is the honest outcome of making an unmeasurable clause measurable. |
| W5 | aggregate gate wiring | **FAIL (half-discharged) → LANDED 2026-08-24** | As executed 2026-08-23 there was no driver and no gate manifest. The **recorded-checklist** half was discharged by §3-§6 of this document. The **mechanised-driver** half is now `scripts/checks/run_r4bg_aggregate_gates.py` (150 lines *[corrected in place 2026-08-24 from "139 lines" per Evidence axis [P3-4](i); `wc -l` over `git show 78badbc:scripts/checks/run_r4bg_aggregate_gates.py`, and the `b91bdeb..78badbc` diff, both give 150 at the review object, matching §13.1's table. Counts are pinned to `78badbc`; the sibling tooling lane's post-review work moves the working tree]*), driving the seven gates of `GwzM5-8Refactor.md:2243-2244` with a required output marker per command; the seventh reports REVIEW and is never counted green. §13.2-§13.3. |

**Table 3 tally (5 rows): FAIL 4 · DEFERRED-BY-AMENDMENT 1.**
**Re-tallied 2026-08-24: LANDED 4 · DEFERRED-BY-AMENDMENT 1 (W3, unchanged —
`RollbackPrefixIssuer` still does not exist and R3 is still post-A1).**

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
| 3 — privacy probes | **FAIL → PASS 2026-08-24** | W1 absent at execution; W3 deferred. **Now: `python -m unittest scripts/checks/test_v1_lifecycle_privacy_probe.py` → 5/5, 96.95 s.** W3 remains deferred and unrunnable by design (J-5). |
| 4 — full test battery | **PASS** | 1569/0/1 lib in four partitions + 50/0 integration. Counts re-baselined (§7.3). |
| 5 — format/lint | **FAIL as written → PASS as corrected 2026-08-24** | `cargo fmt --all -- --check` clean. The clippy command **as the runbook stated it** exits 101 (§3.3). The runbook step is now two lines — `-p gwz-core` with `CLIPPY_CONF_DIR`, plus workspace-wide plain clippy — with the reason recorded in place at `GwzM5-8R4bG-EvidenceInventory.md` §5 step 5. Both lines re-executed green at §13.3. |
| 6 — protocol + parity | **PASS** | `regen.py --check` OK (the tuple's deferred gate, now executed); `d0_roundtrip_check.py` all passed; gwz-py 330/0 with `regen_protocol.py --check` OK; gwz-cli 139/0 (§6 below is the detail). |
| 7 — CI platform evidence | **FAIL** | Boundary + retained-readers (incl. the 5-platform matrix) green **at `b91bdeb`**. **No Windows-matrix and no Platform-matrix run exists at `d45458d` or `b91bdeb`** — last is run 21 at `514f8e6`. `linux-identity-probe` last ran at `f36d20d`. The R5 exact-tree three-platform release jobs are DEFERRED-BY-AMENDMENT off the A1 gate (`ThinA1:236-239`), but the program's own Windows/platform matrix at the settled tree is **not** an R5 job and is missing. |
| 8 — ledger/LOC audit | **PASS** | ~~R4b-G row `ChangeBudget.md:515` = ≤150 test/tool, **0 production**, ≤3 unique production-bearing files, ≤12.~~ **[CORRECTION 2026-08-24 — this row's own reading of the budget was wrong, and it propagated into W5 and F-4. `ChangeBudget.md:507` gives the column order: *Net production-bearing Rust · Moved production Rust · Test/tool/doc LOC · Production files · Test/tool/doc files*. So R4b-G's row `≤150 · 0 · ≤1,500 · ≤3 · ≤12` reads **≤150 net production Rust, 0 moved, ≤1,500 test/tool/doc LOC, ≤3 production files, ≤12 test/tool/doc files** — not "≤150 test/tool". The two columns were transposed. The correct constraint is 10× looser for exactly the material W5 needs.]** As executed 2026-08-23 this document spent 0 of all five. **After the 2026-08-24 FAIL remediation (§13.1): 0 net production Rust, 0 moved, 1,177 net test/tool/doc LOC (582 tool + 595 doc, by `git diff --numstat` plus the three new files), 0 production files, 7 test/tool/doc files — inside every column on the corrected reading, at 78% of the test/tool/doc ceiling, and the driver was held to 150 lines anyway. On the *transposed* reading the same work would be 8× over a budget that does not exist.** |
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

#### Item 3 addendum — the D3 perf pricing record, DRAFTED 2026-08-24 (F-7)

Branch (c) taken: the record did not exist, so it is written here. **This is a
draft for the lane owner.** The RULING line is deliberately unsigned; drafting
a record is not adjudicating it.

**[Status correction 2026-08-24 (R4b-G Evidence axis [P3-3]; Correctness axis
C-7.3) — this paragraph is superseded and is retained only as the drafting
frame. The ruling below IS SIGNED (gwz-dev `d378db9`, 2026-08-24 10:24:43) and
its register row IS INSERTED at `GwzM5-8R2DSettledTuple.md` §11.1. Both axes
found the document contradicting itself on this point across §8, §9.2, §13.1
and §13.4; the signed state is the real one, and it is now said once, here,
with each surviving "unsigned"/"not inserted" sentence annotated in place.]**

**Object.** `workspace_ops::merge::v1_lifecycle::reverse::preservation::tests::
root_fault_matrix::every_root_physical_and_successor_boundary_recovers_without_
repeating_mutation`, the single most expensive test in the tree.

**Figures on record — both, because they are both real and they disagree.**

| Figure | Where | Tree | Build | How obtained |
| --- | --- | --- | --- | --- |
| **576.03 s** | `CurrentProgramCheckpoint.md:642-643` ("the 576.03s on record"); narrative at `GwzM5-8D3Impl-ReviewState.md:553` and `GwzM5-8D3Impl-ReviewCode.md:435,445-446` | the D3 implementation review's round-2 tree | **release**, explicit | implementer's partition B against the prebuilt release binary, `find -newer` attribution; State axis did not re-run it and said so |
| **318.71 s** | `GwzM5-8R4bG-Evidence.md` §3.1 | gwz-core **`b91bdeb`** (the R2-D settled object) | **release**, explicit | executed for this document, Darwin 25.5.0 / arm64, scratch `CARGO_TARGET_DIR`, green, 1 passed / 1569 filtered out |

Neither figure is wrong and the gap is not a regression: they are different
trees on different hosts, and the D3 figure was never re-measured after the
round-2 delta landed. **The "~343 s release" figure exists nowhere in this
workspace or its history** (§8 item 3 body), and **its provenance is now
stated rather than guessed: it came from the lane owner's own session notes.
It is not from the A1 brief, not from any other brief, and not from any
document in the corpus — it is an out-of-band recollection, and no A1 input
should cite it.** **[Attribution corrected 2026-08-24 per the R4b-G Evidence
axis [P3-3], which found "the A1 brief" to be the one unsourced authority
reference in §8 and required the brief be named or the figure marked
out-of-band. It is marked out-of-band: lane-owner session notes. The exhaustive
negative search recorded in the §8 item 3 body is unchanged and stands.]**

**Pricing frame: PRICED, NOT OPTIMIZED.** What is being bought is the D3
amendment's §8 acceptance suite — `GwzM5-8DurableCursorAmendment.md:568-642`,
"Test obligations" — and specifically its crash matrix: the restart-with and
restart-without-cursor equivalence obligation (§8 item 2) drives the same
fixture to every cursor position and injects a crash between each live pass
and its marker write. That matrix is the reason the runtime is what it is. The
cost is therefore **accepted as evidence weight, not carried as a defect**:
the seconds buy per-position restart legality that no cheaper shape produces,
and the amendment's own §8 is the thing that would have to shrink for the
number to shrink. No optimization work is proposed, requested, or implied by
this record.

**Routing.** This is an **A1-activation-review input**, not an R4b-G
acceptance item. `GwzM5-8ThinA1Amendment.md:43-55` (§1) routes the
`v1_lifecycle` P2/P3/P4 acceptance debt — of which the D3 named residual is
part — to the A1 activation review, which must either "(a) subsume a
settled-tree acceptance of the `v1_lifecycle` tree with an operator-signed
named-residual disposition for the D3/D4 classes, or (b) treat them as
blocking per L1-19"; binding restated at `:242-244`. L1-19
(`AgentProcessRules.md:392-401`) is what makes branch (b) exact: "A gate is
`NO-GO` while any P0, P1, or P2 is open."

**RULING — SIGNED by the lane owner, 2026-08-24** *[signature date corrected in
place 2026-08-24 per the R4b-G Evidence axis [P3-3]: it read "2026-08-23", a
date on which neither the ruling nor its lifted tuple row existed — both landed
in gwz-dev `d378db9` (2026-08-24 10:24:43), and the ruling cites the driver's
327.6 s corroboration, itself measured 2026-08-24. Substance unchanged; the axis
independently corroborated it at 352.48 s release on `78badbc`.]*. The record's citable price is **318.71 s (release, `b91bdeb`)** — the settled tree's own measurement, corroborated by the driver's 327.6 s wall (`fault:2`) — with 576.03 s retained as the historical figure at the D3 review tree. "Priced, not optimized" is **ACCEPTED** as the disposition the A1 activation review consumes under branch (a): the cost is the crash matrix's evidence weight, and no optimization is owed pre-A1. The "~343 s" figure the A1 brief expected never existed in the corpus and is corrected by this ruling *[attribution corrected 2026-08-24 per Evidence axis [P3-3]: not "the A1 brief" — **the figure came from the lane owner's session notes**, an out-of-band recollection, not from any brief and not from any corpus document. The ruling's substance is unchanged; only the source of the figure it corrects is restated.]*. Original unsigned frame follows: Which figure is the record's
citable price (576.03 s at the D3 tree, or 318.71 s at the settled tree), and
whether "priced, not optimized" is ACCEPTED as the disposition the A1
activation review consumes under branch (a). This document supplies the
measurement and the frame; it does not sign the ruling.

**Drafted for the lane owner to lift into `GwzM5-8R2DSettledTuple.md` §11.1**
(the "Added at the settled-dual round-2 remediation" block, whose row form
this matches). The row is drafted, not inserted — the tuple document is the
lane owner's:

> | `root_fault_matrix` perf pricing — 576.03 s (D3 review tree) vs 318.71 s (release, `b91bdeb`) | PRICED-NOT-OPTIMIZED; the cost is `GwzM5-8DurableCursorAmendment.md` §8's crash matrix, accepted as evidence weight. A1-activation-review input per `GwzM5-8ThinA1Amendment.md:43-55` / L1-19. Record drafted at `GwzM5-8R4bG-Evidence.md` §8 item 3 addendum; RULING unsigned. The "~343 s" figure the A1 brief expects does not exist anywhere in the corpus. |

**[Lift executed 2026-08-24 — the block quote above is the historical draft and
is left unedited. The row IS INSERTED in `GwzM5-8R2DSettledTuple.md` §11.1
(landed `d378db9`), and the inserted row is corrected this pass on two points
the R4b-G dual found: its date reads **2026-08-24**, not 2026-08-23 (Evidence
[P3-3] — the block it sits in is headed "Added at the settled-dual round-2
remediation (2026-08-23)", which the inserted row postdates), and the "~343 s"
clause names the figure's real provenance, **the lane owner's session notes**,
not "the A1 brief" and no corpus document.]**

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

**[Re-counted 2026-08-24 — Evidence axis [P3-3] found the three figures above
stale the moment F-7's row was lifted, and the R4b-G round-2 remediation has
since added three more rows. The paragraph above is left as the
2026-08-24-morning reading; the current reading is:** `GwzM5-8R2DSettledTuple.md`
§11 now runs `:634-793` (still to EOF) — §11.1 base table **6 rows**
(`:640-645`); the settled-dual round-2 additions block **6 bullets**
(`:649-693`, header verbatim "**Added at the settled-dual round-2 remediation
(2026-08-23):**"); a **new** block, **3 bullets** (`:696-751`, header "**Added
at the R4b-G dual's round-2 remediation (2026-08-24):**" — C-1, C-2, and the
owed-pre-A1 M5b-IMPL settled review); §11.2 **6 rows** (`:756-761`); §11.3
**8 numbered caveats** (`:765-793`). **29 items.** Commits that touch the file
after `5f090eb`: **`d378db9`** (the F-7 perf-pricing row lifted in — which is
why "5 bullets … 25 items … no later commit touches the file" all went stale
at once) and this round-2 records commit. The §11.3(7) double-count caveat in
the note below still applies to a naive tally.**]**

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

| Verdict | Count (as executed 2026-08-23) | After the FAIL remediation (2026-08-24) |
| --- | --- | --- |
| **PASS** | **22** | **27** |
| **FAIL** | **7** | **1** — F-1's underlying row 2.3b/O8, now measured rather than unknowable. F-6 is closed by the lane, not by this pass: gwz-dev `48ee7fe`, "run 22 green (1530/0/1) at the settled object `b91bdeb` — F-6 closed", recorded in `GwzWindowsMatrix-Classification.md`. Its §9.2 row is left as executed; updating it is the matrices lane's, and `CurrentProgramCheckpoint.md:641` still reads "dispatched". |
| **RE-FRAMED** | **6** | 6 |
| **DEFERRED-BY-AMENDMENT** | **2** | 2 |
| **PENDING BY DESIGN** | **1** | 1 |

**Remediation pass, 2026-08-24 (§13).** F-2, F-3, F-4, F-5 closed with
executed evidence; F-7 drafted with its RULING left unsigned for the lane
owner **[superseded 2026-08-24 — the ruling was SIGNED and its register row
INSERTED later the same day at gwz-dev `d378db9` (10:24:43); the signature date
is corrected to 2026-08-24 in §8 item 3 per Evidence [P3-3] / Correctness
C-7.3]**; F-1's work item (W4) discharged, and in discharging it the row it
serves stays FAIL with a **measured** denominator — 13 of 39 M4 scenarios
bound. Two residual findings are opened by that measurement, **R-1** and
**R-2** (§12.6), and they are new inputs to this review, not closures.

Every RE-FRAMED row has its thin-A1 equivalent named, and **five of the six
have that equivalent in evidence** (F1/2.7a → the R2-D 5.2 settled dual,
GO/GO at `b91bdeb`; F2 → the amendment's own checkpoint rewrite; F3 → R2-D
settled ✓ and M5b bound proofs ✓; F4 → the R2-D churn leg realized and
measured; F6 → this document's tree identity). **F4 is the one RE-FRAME whose
equivalent is only half-evidenced** — see F-8.

### 9.2 The seven FAILs, unsoftened

| ID | FAIL | Root | Owner under thin A1 |
| --- | --- | --- | --- |
| **F-1** | Row 2.3b / O8 — "every M4 scenario" byte-equivalence clause is **not checkable**: no enumeration of the M4 scenario set mapped to equivalence evidence exists. | W4 | R4b-G. Doc-only, unblocked, cheap. An R4b obligation quoted verbatim from `GwzM5-8Refactor.md:2265` cannot be confirmed today. **[CLOSED 2026-08-24 — §12 enumerates all 39 M4 scenarios from `GwzM5-8R0Inventory.md` §4/§5.2 and maps each to its tests; `check_m4_scenario_map.py` keeps the map honest. The clause is now checkable, and it **is not met**: 13/39 bound, 22 UNBOUND, 4 NO FIXTURE. F-1 is discharged; residual findings **R-1 (P2 candidate)** and **R-2 (P3 candidate)** at §12.6 are what it exposed, and they are the review's to rule on — not papered.]** |
| **F-2** | Row 2.5b / W1 — no privacy compile probe. Visibility gives the property; **nothing proves it fails closed** the way 22 sibling probes do for the checked-artifact boundary. Battery source `TransitionDesign:1478-1479` is unmet. | W1 | R4b-G. Fits the ≤1,500 test/tool line budget. **[CLOSED 2026-08-24 — `scripts/checks/test_v1_lifecycle_privacy_probe.py`, 5/5 green, and demonstrated to FAIL when the visibility is widened. §13.1.]** |
| **F-3** | Row 2.6b / W2 — no mechanical assertion that v1 lifecycle modules contain no raw v0 persistence call. Battery source `TransitionDesign:1480-1481` is unmet. | W2 | R4b-G. Property is 0-hit clean today, so the pin is trivial to write and trivial to break silently without it. **[CLOSED 2026-08-24 — masked-token scan in `check_checked_artifact_boundaries.py`, seam derived from `merge/mod.rs`'s own re-exports, covering test code too; inherits the checker's per-commit CI wiring. Load-bearing for J-1. §13.1.]** |
| **F-4** | W5 — no aggregate gate driver. The **record** half is discharged by this document; the **mechanised** half (≤150 lines, `ChangeBudget.md:515`) is unspent. | W5 | R4b-G. This is literally the only material the R4b-G row's budget is for. **[CLOSED 2026-08-24 — `scripts/checks/run_r4bg_aggregate_gates.py`, 150 lines *[count corrected in place 2026-08-24 from "139": at the review object `78badbc` both `git show` and the `b91bdeb..78badbc` diff give 150; §13.1's table was the correct one all along — Evidence axis [P3-4](i)]*, seven batteries, executed. §13.2-§13.3. Note the "≤150 lines" reading is wrong — see §7.5 step 8's correction.]** |
| **F-5** | Runbook step 5 — `CLIPPY_CONF_DIR=$PWD cargo clippy --workspace --all-targets --all-features -- -D warnings` **exits 101** with 7 `disallowed_methods` errors in gwz-cli: 2 production (`gwz-cli/src/pager.rs:127,131`, `io::Write::write_all`) and 5 test (`src/tests/g01/commands.rs:460`; `src/tests/m2c.rs:51,55,85,99`). | runbook command scope | R4b-G. **Not a gwz-core defect.** Either correct the runbook to `-p gwz-core` (matching CI) or make an explicit decision to extend the writer perimeter to gwz-cli. Do not silently drop the step. **[CLOSED 2026-08-24 — runbook step 5 is now two lines (`-p gwz-core` WITH the CONF_DIR; workspace-wide plain clippy as its own line), with the reason recorded in place: the perimeter is gwz-core's by design and the 7 gwz-cli hits are out-of-perimeter, verified not silently suppressed because the plain workspace line still compiles every one of those sites under `-D warnings`. The step was corrected, not dropped. J-3 is answered by choosing `-p gwz-core` (match CI); if the review prefers extending the perimeter to gwz-cli that is a scope decision it still owns.]** |
| **F-6** | Runbook step 7 — **no Windows-matrix and no Platform-matrix run exists at the settled tuple.** Last is run 21 at `514f8e6`, three commits back (`d45458d`, `b91bdeb` uncovered). `linux-identity-probe` last ran at `f36d20d`. | dispatch not issued | R4b-G to decide: dispatch, or record the deferral with the risk named. The tuple already owes this as a round-2 ledger duty (`GwzM5-8R2DSettledTuple.md:663-665`) — scoped there to "the ten multi-component writer rows"; the CI record shows the gap is **the whole settled tree**, not ten rows. |
| **F-7** | §8 item 3 — the **D3 perf pricing record does not exist.** No "~343 s release", no "priced not optimized", anywhere in the repo or its history. | record never written | The A1 activation review is expected to consume a figure that is not filed. This document supplies **318.71 s release at `b91bdeb`**; the nearest filed figure is **576 s release** (`GwzM5-8D3Impl-ReviewState.md:553`). **[DRAFTED 2026-08-24 — §8 item 3 addendum carries both figures (576.03 s / 318.71 s), the priced-not-optimized frame with the D3 amendment §8 crash matrix as the cost, and the L1-19 routing; plus a one-row register addition drafted for `GwzM5-8R2DSettledTuple.md` §11.1. The **RULING line is unsigned** and the tuple row is **not inserted** — both are the lane owner's. F-7 moves from "record never written" to "record drafted, ruling open".]** **[SUPERSEDED / CLOSED 2026-08-24 — per the R4b-G Evidence axis [P3-3] and Correctness axis C-7.3, this row's "unsigned"/"not inserted" reading was left standing after the state changed. The **RULING IS SIGNED** and the **tuple row IS INSERTED** (`GwzM5-8R2DSettledTuple.md` §11.1), both at gwz-dev `d378db9`, 2026-08-24 10:24:43; the signature date is corrected from 2026-08-23 to **2026-08-24** at §8 item 3. F-7 is **record written, ruling signed**. Substance corroborated independently by the Evidence axis at 352.48 s release on `78badbc` — inside the 319-353 band, decisively not 576. The "~343 s" figure it corrects is **the lane owner's session notes**, out-of-band, not the A1 brief and not any corpus document.]** |

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
  **[2026-08-24: reason (c) is now addressed. The gate exists — F-3's masked-
  token scan in `check_checked_artifact_boundaries.py`, wired into the
  per-commit CI lane. It lands as a **standing guard going forward**, not as a
  retroactive claim about the merge that already happened; that is the exact
  framing the lane owner's J-1 adjudication uses
  (`CurrentProgramCheckpoint.md:653-656`). The guard's own comment says it is
  load-bearing for J-1 and must not be weakened without the lane owner's
  ruling. The ordering deviation itself is still the dual's to accept.]**
- **The review must rule:** ratify the ordering as superseded (and have the
  lane strike `CurrentProgramCheckpoint.md:1092` and annotate the design's
  `:976-986`), or record it as a deviation under L1-16. Either way the
  reason-(c) dependency needs an answer.
- **[J-1 ANNOTATION 1 of 2 — leg 4 restated, 2026-08-24, round-2 merged
  remediation.]** The lane-owner adjudication's fourth leg read "the settled
  dual's Code axis re-verified the M5b surfaces byte-identical". **That
  attribution was wrong**: the strings `M5b`, `no_ff`/`no-ff`, `3e60529` and
  `8c1624a` appear zero times in `GwzM5-8R2DSettled-Review{Code,State}.md`
  (rounds 1+2, whole files) — the R4b-G **Evidence axis's [P2-1]**, the one
  P2 of its round-1 NO-GO. **The true source is the D3 dual**, whose round-2
  re-verdicts verified the five M5b surfaces byte-identical on **both** axes:
  `GwzM5-8D3Impl-ReviewCode.md:447-448` ("all five M5b files byte-identical
  vs `0b059e8`; `store/tests.rs` exactly **+2/−0**", inside the round-2
  "Gates (re-executed by me, release, round-2 binary)" section) and
  `GwzM5-8D3Impl-ReviewState.md:550-552` ("M5b four-filter **37/0** with all
  five M5b files `git status`-clean (byte-identical)", inside the ROUND-2
  FOCUSED RE-VERDICT). **The fact was true; its citation was not.** The leg
  is restated at its source, `CurrentProgramCheckpoint.md`'s J-1 record, with
  the same dated bracket.
- **[J-1 RULING, 2026-08-24 — RATIFIED BY BOTH AXES.]** Correctness §5 J-1:
  "**RATIFIED**: the adjudication's ACCEPTED-WITH-RECORD stands", on facts
  re-verified independently (`3e60529`/`8c1624a` ancestors of `78badbc`; prod
  net **0** for both at `GwzM5-8R2DSettledTuple.md:375-376` rows 3-4; leg 3
  proved fail-closed by that axis's own guard probes). Evidence §6: deviation
  REAL and fairly stated, legs 1-3 verified on the record, **not
  disqualifying**. **The named before-A1 remedy is NOT triggered** by either
  axis. What survives is not the merge but the review: **the M5b-IMPL settled
  review remains owed pre-A1** on its frozen tier (mandated-dual by default),
  recorded at `CurrentProgramCheckpoint.md`'s M5b-IF freeze record and in the
  settled tuple's §11.1 register. The two bookkeeping annotations both axes
  make the price of ratification are landed:
  `GwzM5-8M5bNoFfDesign.md:976-986` and the checkpoint's live restatement.

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
**[2026-08-24: the cheap answer is taken and recorded, not assumed. Runbook
step 5 is now two lines — `-p gwz-core` with `CLIPPY_CONF_DIR` (matching CI's
`checked-artifact-boundary.yml:30-35`), plus workspace-wide plain clippy as
its own line — with a written reason in place. The step is corrected, not
dropped, and the 7 gwz-cli hits are verified not silently suppressed: the
workspace-wide line still compiles every one of those call sites under
`-D warnings`. If the review prefers extending the writer perimeter to
gwz-cli, that remains its call; nothing here forecloses it.]**

**J-4 — W1's sequencing note is now moot.** The checklist says "Sequence
after R3 (see churn note)" (`:661-662`). R3 is post-A1. **W1 no longer has a
reason to wait**, and the R3 churn it was hedging against will not arrive
before A1. If the review accepts that, W1 becomes plainly buildable now, and
F-2 loses its only excuse. **[2026-08-24: W1 was built on that reading. If the
review rejects J-4 and holds that W1 should have waited for R3, the probe is
still correct today and its three seals are stated independently, so an R3
reshuffle of the rollback-prefix seam changes W3's future gate, not this
one.]**

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

**Second pass, 2026-08-24 — the FAIL remediation.** §§12-13 were added by a
later session on the same host and the same tree (gwz-core `b91bdeb`, clean at
start). That pass is **not** an execution of the inventory; it closes the FAILs
§9.2 recorded, and it does write files: five under `gwz-core/scripts/checks/`
and this document plus `GwzM5-8R4bG-EvidenceInventory.md`. **Zero production
lines.** No git write operation was performed by it either, and no digest pin
was moved — pins are the lane owner's. Its own provenance, budget, and gate
tails are §13. Everything in §§1-11 above is the 2026-08-23 record and is left
as executed, annotated in place where a later fact bears on it.

---

## 12. The M4 scenario enumeration (F-1 / W4), added 2026-08-24

**Status of this section: INVENTORY bound to an executed run (L1-16).** The
tables below are a name-to-name map. They are not themselves an executed
matrix. The run that executes them is recorded at §12.5 and is the g23 suite
at `b91bdeb`; every test path named here is verified to exist by
`scripts/checks/check_m4_scenario_map.py`, which is battery 3 of the §13
driver.

### 12.1 Which document defines the M4 scenario set

`GwzM5-8Refactor.md:51` puts M4's characterization in R0: "R0-R1 characterize
M4 and centralize current semantics without changing the …". The document that
did it is **`GwzM5-8R0Inventory.md`** — "GWZ M5-M8 R0 Baseline and Lifecycle
Inventory". Its §4 ("Closed v0 progress inventory mapped to §15.3.2",
`:179-205`) enumerates the durable M4 progress shapes by name in rows A-M, and
its §5.2 ("Observed terminal archive shapes", `:226-246`) enumerates the
archive shapes by name. Between them they are the only by-name enumeration of
the M4 durable scenario set in this corpus, and they are exactly the four
output families O8 names — lock, candidate, root, archive.

Two companion documents are consulted and neither supplies a competing
enumeration: the **M4 acceptance record** (`GwzMergeM4-RemPlan.md` §8-§9,
whose exit criteria are the M4 *feature* gate — prediction, `--ff-only`, the
selection axis — carried at §12.4 below) and `GwzM5-8Refactor.md` §15.3.3
(`:1456`, the archive projection design that §5.2's rows target).

**29 progress shapes + 10 archive shapes = 39 named M4 scenarios.**

### 12.2 How each row is dispositioned

The v1 equivalence binding is the frozen predicate registry,
`gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json`, which is
`include_str!`-bound at `g23/compatibility_v0.rs:10` and machine-validated by
`check_merge_compatibility_predicates.py`. It carries **7 `fixture_corpus`**
rows and **6 `valid_unlisted_corpus`** rows, each naming an exact test and
subcase.

- **ADAPTED** — a `fixture_corpus` row. The scenario is adapted to v1 and
  driven through `assert_i2_compatibility_fixture`
  (`g23/compatibility_v0.rs:375`), which asserts the classification tuple, the
  accepted-workspace byte identity `accepted.lock.exact_yaml ==
  candidate.lock_yaml` (`:451`), and then `assert_upgrade_fixture`
  (`g23/atomic_upgrade_v0.rs:21`) — per-case fault matrix, unknown-field
  survival plus verifier, accepted-lock extension, and `assert_v1_restart`
  (`:341`). **This is the full O8 property.**
- **UNLISTED** — a `valid_unlisted_corpus` row. v1 refuses to adapt the
  scenario for a recorded reason, so there is no v1 output to be equivalent
  to; the obligation discharged is that the v0 bytes stay exact, driven
  through `assert_i2_valid_unlisted_fixture` (`g23/compatibility_v0.rs:473`).
- **UNBOUND** — the scenario has v0 pinning and/or scenario coverage but **no
  registry row at all**: neither adapted nor recorded as deliberately
  unlisted. **Residual finding.** §12.5 counts these.
- **NO FIXTURE** — no independently injected durable fixture exists for the
  shape at all. **Hard residual finding.**

Two further dispositions were added on 2026-08-24 by the C-1 closure (§12.9)
for shapes the registry's two corpora **cannot** express, because both corpora
model non-`Finalizing` refusal (`assert_i2_valid_unlisted_fixture` asserts
`record.state != Finalizing`, and every corpus row's own `operation_state` value
is terminal, recovery, preserving, or rolling-back — never `finalizing`) while
these shapes are `Finalizing`:

- **DISPOSITIONED-UNLISTED** — a well-formed descriptor that matches zero
  whitelist rules. `adapt_open` returns `ValidUnlisted` and the atomic upgrade
  leaves the v0 bytes exact with nothing staged, driven through the real
  decode/adapt/upgrade path and recorded by contract clause in §12.9 rather
  than by a registry row.
- **DISPOSITIONED-REFUSED** — the row refuses before staging with an exact
  `ErrorCode` and the registry's own `rejection_reasons` sentence; v0 bytes
  exact, nothing staged. Also driven through the real path and recorded in
  §12.9.

Both are *stronger* than UNBOUND (an executed per-scenario answer, not a class
cite) and *weaker* than ADAPTED (no v1 output exists to be byte-equivalent to).

<!-- m4-map:begin -->

### 12.3 Table A — the 29 durable progress shapes (`GwzM5-8R0Inventory.md` §4)

Test paths are relative to `workspace_ops::tests::g23::` unless fully
qualified.

| R0 row | M4 shape | v0 pin / scenario evidence | Registry row | Disposition |
| --- | --- | --- | --- | --- |
| A | `A-EXECUTING` | `preserve::review_remediation::preserve_abort_accepts_exact_interrupted_executing_state`, `start::preflight_checks_every_member_before_mutating_an_earlier_member`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `A-EXECUTING`) | `open/executing` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| A | `A-AWAITING` | `open_operation_gate::open_awaiting_resolution_blocks_dry_run_and_real_starts_from_an_explicit_root`, `continue_merge::conflict_continues_to_later_member_and_status_recovers_with_baseline_lock`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `A-AWAITING`) | `open/awaiting-resolution` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| A | `A-HALTED` | `open_operation_gate::open_halted_blocks_dry_run_and_real_starts_from_an_explicit_root`, `continue_merge::failed_and_unattempted_rows_retry_only_after_whole_operation_preflight`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `A-HALTED`) | `open/halted` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| A | `A-PRE-PRESERVE` | `preserve::preserving_abort_gate::plain_abort_rejects_interrupted_preservation_before_rollback`, `preserve::preserve_abort_failure_windows_never_begin_rollback_and_retry_converges`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `A-PRE-PRESERVE`) | `preserving/pre-acceptance` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| A | `A-PRE-ROLLBACK` | `abort_recovery::abort_accepts_every_pre_candidate_finalization_fault_point`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `A-PRE-ROLLBACK`) | `rollback/pre-acceptance` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| B | `B-NO-PUBLICATION-ROW` | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` (window `finalizing_before_publication_record`, fault `AfterEnteringFinalizing`) | `changed/finalizing-before-publication-record` | **ADAPTED** |
| B | `B-NOT-STARTED` | none — R0 §4 row B records "durable `NotStarted` post-rename window is not independently injected"; still true at `b91bdeb` | — | **NO FIXTURE** |
| B | `B-VALIDATING` | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` (window `validating_before_candidate`, fault `BeforeCandidateCreation`) | `changed/validating-before-candidate` | **ADAPTED** |
| B | `B-PREPARING-EMPTY` | none — no window injects `PreparingCandidate` with the candidate still absent; the nearest window (`candidate_persisted_before_evidence`) already carries the candidate | — | **NO FIXTURE** |
| C | `C-CANDIDATE-PERSISTED` | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` (window `candidate_persisted_before_evidence`, fault `AfterCandidatePersistence`) | `changed/candidate-persisted` | **ADAPTED** |
| D | `D-EVIDENCE-CREATED-UNRECORDED` | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` (window `evidence_created_before_recording`, fault `AfterEvidenceCommit`) | `changed/evidence-unrecorded` | **ADAPTED** |
| E | `E-EVIDENCE-RECORDED` | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` (window `evidence_recorded_before_publication`, fault `AfterEvidencePersistence`) | `changed/evidence-recorded` | **ADAPTED** |
| F | `F-BASELINE` | `characterization_publication_prefix_v0::v0_candidate_publication_prefixes_are_restartable_at_every_mutation`; adaptation disposition `compatibility_residue_v0::v0_mid_publication_prefixes_are_not_a1_migration_rules` (subcase `F-BASELINE`) | — (no registry vocabulary; §12.9) | **DISPOSITIONED-UNLISTED** |
| F | `F-MARKER` | `characterization_publication_prefix_v0::v0_candidate_publication_prefixes_are_restartable_at_every_mutation`, `drift::status_detects_marker_and_boundary_drift_without_a_prior_recovery_mutation`; adaptation disposition `compatibility_residue_v0::v0_mid_publication_prefixes_are_not_a1_migration_rules` (subcase `F-MARKER`) | — (typed refusal, not a corpus row; §12.9) | **DISPOSITIONED-REFUSED** |
| F | `F-LOCK` | `characterization_publication_prefix_v0::v0_candidate_publication_prefixes_are_restartable_at_every_mutation`; adaptation disposition `compatibility_residue_v0::v0_mid_publication_prefixes_are_not_a1_migration_rules` (subcase `F-LOCK`) | — (typed refusal, not a corpus row; §12.9) | **DISPOSITIONED-REFUSED** |
| F | `F-BOUNDARY` | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` (window `candidate_published_before_recording`, fault `AfterLockPublication`) | `changed/prefix-boundary` | **ADAPTED** |
| G | `G-VERIFYING` | `characterization_publication_v0::v0_verifying_publication_is_durable_for_born_and_unborn_roots`; adaptation disposition by clause, executed inside `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (the `G-VERIFYING` arm, plain fn `g_verifying_is_dispositioned_by_clause`) | — (no corpus vocabulary; §12.9(c), ground corrected at §12.9(e)) | **DISPOSITIONED-UNLISTED** *(E5.1, 2026-08-28)* |
| G | `G-COMPLETE-PRE-ARCHIVE` | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` (window `completed_before_archive`, fault `BeforeArchive`), `characterization_v0::v0_terminal_completed_before_archive_is_read_only_and_closes_byte_exactly` | `terminal/completed` | **UNLISTED** ("terminal v0 archives byte-exact") |
| H | `H-PRESERVING-PRE` | `characterization_preservation_v0::v0_preservation_restart_rebuilds_missing_stash_bundle_from_recorded_evidence` | `preserving/stash` | **UNLISTED** ("v0 has no lossless write-ahead reverse owner") |
| H | `H-PRESERVING-CANDIDATE` | `preserve::preserve_abort_handles_post_composition_root_work_with_root_bundle_identity`, `preserve::review_remediation::member_only_merge_preserves_post_composition_root_work`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `H-PRESERVING-CANDIDATE`) | `preserving/candidate` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| H | `H-PRESERVING-PREFIX` | `characterization_preservation_v0::v0_preserving_overlay_round_trips_every_recorded_root_prefix`, `preserve::review_remediation::preserve_retry_repairs_interrupted_root_publication_normalization`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `H-PRESERVING-PREFIX`) | `preserving/root-prefix` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| I | `I-PARTICIPANT-ROLLBACK` | `characterization_preservation_v0::v0_participant_rollback_has_restartable_durable_reverse_prefixes` | `rollback/participant` | **UNLISTED** ("v0 has no lossless write-ahead reverse owner") |
| I | `I-EVIDENCE-ROLLBACK` | `characterization_preservation_v0::v0_evidence_rollback_records_each_reverse_artifact_prefix`, `abort_recovery::born_root_evidence_abort_recovers_both_record_persistence_windows`, `abort_recovery::unborn_root_evidence_abort_recovers_both_record_persistence_windows`; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `I-EVIDENCE-ROLLBACK`) | `rollback/evidence` | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| J | `J-NO-PUBLICATION-BORN` | `characterization_v0::v0_no_publication_complete_before_terminal_write_is_read_only` (fault `AfterNoPublicationComplete`, the eighth variant, driven separately from the seven-window array) | `unchanged/no-publication-finalizing` | **ADAPTED** *[twin-label correction 2026-08-24, §12.9: the corpus fixture named here builds an **unborn** root, and `GwzM5-8I2CompatibilityContract.md:123` excludes "born root" by name, so the adapted case is the UNBORN twin. The born twin at this same window is valid-unlisted, pinned by `compatibility_residue_v0::v0_no_publication_finalizing_twins_split_on_the_born_root_exclusion`. The count is unaffected — one J twin is adapted either way — but the label is on the wrong row.]* |
| J | `J-NO-PUBLICATION-UNBORN` | `characterization_v0::v0_no_publication_completion_preserves_born_and_unborn_root_inputs`, `characterization_publication_v0::v0_publication_windows_have_born_and_unborn_root_twins`; adaptation disposition `compatibility_residue_v0::v0_no_publication_finalizing_twins_split_on_the_born_root_exclusion` and `compatibility_residue_v0::v0_no_publication_terminal_twins_stay_v0_for_born_and_unborn_roots` | `unchanged/no-publication-finalizing` (finalizing twin; §12.9) | **ADAPTED** (finalizing twin) + **DISPOSITIONED-UNLISTED** (R0 terminal shape) |
| K | `K-COMPLETED-CANDIDATE-OPEN` | `characterization_v0::v0_terminal_completed_before_archive_is_read_only_and_closes_byte_exactly` | `terminal/completed` (shared with `G-COMPLETE-PRE-ARCHIVE` — one durable object, two R0 rows) | **UNLISTED** |
| K | `K-COMPLETED-NOPUB-OPEN` | `characterization_publication_v0::v0_no_publication_completed_open_record_closes_byte_exactly` — **this closes R0 §4 row K's "no-publication terminal-open byte-exact fixture missing" gap**; adaptation drive `compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open` (arm `K-COMPLETED-NOPUB-OPEN`) | `terminal/completed-no-publication` (its durable object also carries `J-NO-PUBLICATION-UNBORN`'s R0-terminal DISPOSITIONED-UNLISTED record — one durable object, two R0 rows, the terminal/completed pattern; §12.9(e)) | **UNLISTED** *(E5.1 registry row, 2026-08-28)* |
| L | `L-RECOVERY-OVERLAY` | `characterization_v0::v0_recovery_required_overlays_preserve_candidate_and_no_publication_evidence` (subcases `candidate`, `no_publication`), `characterization_publication_v0::v0_recovery_required_overlays_keep_each_constructible_publication_row_byte_exact` | `recovery/candidate` + `recovery/no-publication` (two rows, one shape) | **UNLISTED** ("recovery origin is not uniquely owned") |
| M | `M-ABORTED-OPEN` | `characterization_v0::v0_terminal_aborted_before_archive_is_read_only_and_closes_byte_exactly`; candidate-era variant `characterization_publication_v0::v0_candidate_aborted_open_record_closes_byte_exactly` — **closes half of R0 §4 row M's "candidate/evidence-era … terminal-open variants missing" gap; the preservation-bearing variant remains open** | `terminal/aborted` | **UNLISTED** |

### 12.4 Table B — the 10 archive shapes (`GwzM5-8R0Inventory.md` §5.2)

*(E5.2, 2026-08-28: the ten shapes are recorded in the standalone archive
corpus of GwzM5-8I2CompatibilityPredicates.json — not the migration registry,
which has no vocabulary for them. Tier 1 = byte-SHA256 across the production
archival act, executed on the eight fixtured rows over five distinct durable
bases; tier 2 is owed on all ten, carrier E4.4, R2-F-encumbered. §12.9(e).)*

| Archive id | Fixture shape | Evidence | Registry row | Disposition |
| --- | --- | --- | --- | --- |
| `AC-CANDIDATE` | `av0_b` / `CompletedCandidate` | `characterization_archive_v0::archived_v0_b_through_g_status_uses_only_archive_bytes`; tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `av0_b`) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |
| `AC-NOPUB-BORN` | `av0_c` / `CompletedNoPublication` | `characterization_archive_v0::archived_v0_b_through_g_status_uses_only_archive_bytes`, `characterization_archive_v0::id_qualified_gc_returns_the_validated_archived_record_projection`; tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `av0_c`) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |
| `AC-NOPUB-UNBORN` | none — no archive-only projection with the live root later born or missing | `characterization_v0::v0_no_publication_completion_preserves_born_and_unborn_root_inputs` pins the live-side unborn twin only | — | **NO FIXTURE** *(PENDING-FIXTURE both tiers in the archive corpus, machine-enforced as exactly this pair; carrier R2-F fixtures/native-evidence lane — E5.2, 2026-08-28)* |
| `AA-PREACCEPTANCE` | `av0_e` / `AbortedPreAcceptance` | `characterization_archive_v0::archived_v0_b_through_g_status_uses_only_archive_bytes`; tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `av0_e`) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |
| `AA-CANDIDATE-COMPLETE` | `av0_f` / `AbortedCompleteCandidate` | `characterization_archive_v0::archived_v0_b_through_g_status_uses_only_archive_bytes`; tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `av0_f`) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |
| `AA-CANDIDATE-PARTIAL` | `av0_g` / `AbortedPartialCandidate` | `characterization_archive_v0::archived_v0_b_through_g_status_uses_only_archive_bytes`; tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `av0_g`) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |
| `AP-PRESERVED` | none — no stash-only / ref-only / combined **archive** fixture; the evidence is live-workspace GC | `characterization_preservation_v0::v0_gc_restarts_after_one_of_two_recorded_backup_refs_was_deleted`, `gc::explicit_gc_checked_deletes_only_backup_refs_and_archive_record` | — | **NO FIXTURE** *(PENDING-FIXTURE both tiers in the archive corpus, machine-enforced as exactly this pair; carrier R2-F fixtures/native-evidence lane — E5.2, 2026-08-28)* |
| `AL-OPTIONAL-MISSING` | `av0_d` / `CompletedEvidenceGap` | `characterization_archive_v0::archived_v0_optional_evidence_gaps_remain_readable_and_untouched` (the four named gaps `exact_lock_bytes`, `complete_member_audit`, `accepted_root_input`, `publication_evidence`); tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `av0_d` — the tier-1 base is the pre-overlay archival act; the named evidence-gap overlay is byte-pinned by the characterization test) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |
| `AL-UNKNOWN` | `unknown_retention` / `CompletedCandidate` | `characterization_archive_v0::archived_v0_unknown_fields_and_raw_bytes_survive_status_and_retention` — **closes R0 §5.2's "open-record unknown round-trip unit test only" gap**; tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `unknown_retention` — the tier-1 base is the pre-overlay archival act; unknown-field survival is byte-pinned by the characterization test) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |
| `AR-C` | contradiction cases over `CompletedNoPublication` | `characterization_archive_v0::archived_v0_missing_optional_evidence_is_not_an_unreadable_contradiction`; tier-1 archival byte-preservation `archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records` (subcase `ar_c` — the tier-1 base is the pre-overlay archival act; the contradiction overlays are byte-pinned by the characterization test) | — | **TIER-1 BYTE-PRESERVED** *(v0-origin; tier 2 owed — E5.2, 2026-08-28)* |

<!-- m4-map:end -->

### 12.5 The M4 feature axis (the M4 acceptance record's own gate)

`GwzMergeM4-RemPlan.md:412-418` exit criterion 6 — "member-only, root-only,
and mixed prediction and `--ff-only` matrices pass" — is the M4 *feature*
scenario axis, and it is orthogonal to the durable-shape axis above. It is
IMPLEMENTED+TESTED and needs no new enumeration: `m4_matrix` carries the three
mixed rows by name (`m4_matrix::mixed_member_root_dry_run_reports_clean_and_conflicted_predictions_without_mutation`,
`m4_matrix::mixed_ff_only_rejects_later_root_true_merge_before_member_fast_forward`,
`m4_matrix::mixed_ff_only_accepts_member_fast_forward_and_up_to_date_root`),
with the member-only rows in `start::` (`ff_only_fast_forward_persists_its_durable_mode`,
`ff_only_rejects_mixed_batch_before_an_earlier_fast_forward`,
`dry_run_predicts_conflicts_without_changing_git_or_gwz_state`) and the
root-only rows in `root_start::` (`explicit_root_ff_only_rejects_true_merge_before_mutation`,
`explicit_root_dry_run_predicts_conflict_without_mutation`,
`explicit_root_dry_run_is_visible_and_does_not_mutate`). This axis produces no
durable v1 record of its own, so O8's byte-equivalence clause does not reach
it; row 2.9/O9 already carries it.

### 12.6 Verdict, and the residual findings this enumeration exposes

**O8's "every M4" clause is now checkable. The answer is that it is not
met — 13 of 39 scenarios carry a v1 equivalence binding.**

| Disposition | Progress (§12.3) | Archive (§12.4) | Total |
| --- | ---: | ---: | ---: |
| **ADAPTED** — full O8 property proven | 7 | 0 | **7** |
| **UNLISTED** — v1 declines to adapt, v0 bytes pinned | 6 | 0 | **6** |
| **UNBOUND** — no registry row at all | 14 | 8 | **22** |
| **NO FIXTURE** — no durable fixture either | 2 | 2 | **4** |
| **Total named scenarios** | **29** | **10** | **39** |

*[C-1 closure movement, 2026-08-24 — this table is left exactly as the two
R4b-G axes measured it, per §12.7's rule. §12.9 moves four of the 14 progress
UNBOUND rows: `F-BASELINE` to DISPOSITIONED-UNLISTED, `F-MARKER` and `F-LOCK`
to DISPOSITIONED-REFUSED, and `J-NO-PUBLICATION-UNBORN` to ADAPTED at its
finalizing twin plus DISPOSITIONED-UNLISTED at its R0 terminal shape. Read
forward: **18 UNBOUND** (10 progress + 8 archive), and the four rows now carry
an executed per-scenario answer. The 13-registry-row arithmetic below is
unchanged — none of the four gained a registry row, and §12.9(c) records why
none can.]*

*[E5 closure movement, 2026-08-28 — read forward from the C-1 note above: the
18 UNBOUND rows are now 0. The 10 progress rows landed as 9 new
`valid_unlisted_corpus` registry rows plus G-VERIFYING's clause-cited
disposition on §12.9(c)'s own ground (§12.9(e) — the "10 registry rows"
denominator was a 9+1 partition); the 8 fixtured archive rows carry executed
tier-1 byte-preservation in the standalone archive corpus with tier 2 owed on
all ten (carrier E4.4, ratified with its R2-F encumbrance named). The
registry-row arithmetic below becomes: 22 rows bind 22 shape labels; the
durable-object count is lower — `terminal/completed` is one object for two R0
rows, and `terminal/completed-no-publication`'s object also carries
J-NO-PUBLICATION-UNBORN's R0-terminal clause record (§12.9(e), the [P2-1]
determination). §12.8's byte-equivalence verdict stays **PARTIAL** — tier 2 is
executed nowhere.]*

Registry-row arithmetic, since the map is not 1:1 in either direction: the 7
`fixture_corpus` rows bind 7 shapes one-for-one; the 6 `valid_unlisted_corpus`
rows bind 6 shapes, but `terminal/completed` binds two R0 rows
(`G-COMPLETE-PRE-ARCHIVE` and `K-COMPLETED-CANDIDATE-OPEN` are one durable
object seen from §4's progress axis and its terminal axis) while
`L-RECOVERY-OVERLAY` consumes two rows (`recovery/candidate`,
`recovery/no-publication`). 13 registry rows, 13 bound shapes.

**Residual finding R-1 (P2 candidate).** 22 named M4 scenarios have **no
registry row** — they are neither adapted-and-proven-equivalent nor recorded
as deliberately unlisted. The registry is the only mechanism the tree has for
stating either answer, and `check_merge_compatibility_predicates.py` only
validates the rows that exist; it cannot notice a scenario nobody filed. The
whole archive family (§12.4) is unbound: **not one of the ten M4 archive
shapes has a v1 equivalence binding**, although O8 names archive output
explicitly.

**Residual finding R-2 (P3 candidate).** 4 scenarios have no durable fixture
at all: `B-NOT-STARTED` and `B-PREPARING-EMPTY` (R0 §4 row B's own recorded
gap, unchanged since R0), `AC-NOPUB-UNBORN`, and `AP-PRESERVED`
(archive-side; the live-workspace evidence is not the archive projection R0
§5.2 asks for). These cannot be dispositioned either way until a fixture
exists.

**What did close since R0**, recorded so the review does not re-open them:
`K-COMPLETED-NOPUB-OPEN`, `AL-UNKNOWN`, and the candidate-era half of
`M-ABORTED-OPEN` now have the fixtures R0 listed as missing.

**Executed run binding this inventory (L1-16).** Tree gwz-core `b91bdeb`,
clean; Darwin 25.5.0 / arm64; `cargo test --lib -p gwz-core
workspace_ops::tests::g23::` -> **111 passed; 0 failed**, and
`scripts/checks/check_m4_scenario_map.py` -> ok. Tails at §13.3. Unexecuted
remainder: none for the named tests; the 22 *[in-place arithmetic fix
2026-08-24: this read "21"; the §12.6 disposition table and both R4b-G axes
give **22** UNBOUND — 14 progress + 8 archive]* UNBOUND and 4 NO FIXTURE rows
are inventory gaps, not unexecuted tests.

### 12.7 R-1/R-2 reconciled — the C-1 and C-2 record (added 2026-08-24, R4b-G dual round 2)

Both R4b-G axes examined R-1 and R-2 independently and converged. This
sub-section is the reconciled record; it supersedes the *grading* of §12.6's
"R-1 (P2 candidate)" / "R-2 (P3 candidate)" labels and leaves every measured
number in §12.3-§12.6 exactly as recorded.

**C-1 (= R-1) — GRADED P2, OPEN, BLOCKING-FOR-A1.** The 22 UNBOUND rows are
**not** an unmeasured behavioural space. `GwzM5-8I2CompatibilityContract.md`
disposes of them **categorically, in frozen accepted text**: `:117-125` — "A1
deliberately whitelists only seven one-member-workspace, `Finalizing`,
normal-mode shapes … **Marker/lock-only prefixes, multi-member workspaces,
selected root, born root, drift, pending actions, recovery, preservation,
rollback, and terminal rows are not A1 migration rules**" — and `:159-165` —
"**Zero whitelist matches is not an error.** Open read-only status leaves bytes
unchanged … Completed and aborted v0 records remain v0 and use byte-preserving
archival. **Archived v0 uses only the archive decoder/projection**". Every one
of the 22 falls inside a named exclusion class of that clause (A-* pre-
acceptance; `F-BASELINE`/`F-MARKER`/`F-LOCK` as marker/lock-only prefixes;
`G-VERIFYING` outside the seven; H-* preservation; `I-EVIDENCE-ROLLBACK`
rollback; K/M terminal; the ten archive shapes under the archive-projection
clause), and 18 of the 22 additionally carry the v0 byte/restart pins §12.3
names, executed green inside g23 111/0. **So the substance of the unbound space
is: contractually dispositioned fail-closed by class, and byte-pinned per
shape. What is missing is the per-scenario *record*** — a registry row or an O8
acceptance note citing the clause per row — so the disposition is
machine-visible where `check_merge_compatibility_predicates.py` looks.

**The class cite does not cover the whole residue. Four rows are live and are
named here:**

| Row | Why the class cite is not enough | Found by |
| --- | --- | --- |
| `J-NO-PUBLICATION-UNBORN` | the only UNBOUND row whose class membership is **ambiguous**: whether the whitelisted no-publication descriptor matches the unborn-root twin is untested per-case | Evidence axis [P3-1] |
| `F-BASELINE`, `F-MARKER`, `F-LOCK` | all three are **`Finalizing`-state, i.e. inside the only state the whitelist adapts**, and no test drives `adapt_open` on them (`characterization_publication_prefix_v0.rs` contains no adapt/upgrade/`ValidUnlisted` reference). The adapter's descriptor *should* make a mid-prefix observation differ from `changed/prefix-boundary`'s and fall through to `ValidUnlisted` (`open_v0/descriptor.rs`: `publication_step`, candidate/composition/hash relations, `normalized_i2_root_observation`) — **but nothing pins that.** If a mid-prefix observation ever coincided with the boundary descriptor, v1 would adapt with a boundary-tuned `next_action` and the publication restart would re-drive the prefix from the wrong position. **Adaptation disposition unstated and untested.** | Correctness axis C-1 |

**Grade and routing.** **P2** — a concrete compatibility/correctness defect
under L1-19 (`AgentProcessRules.md:392-401`), not a documentation nit, because
the four named rows are finalizing-era or ambiguous and the registry is the
tree's only machine-checked adapt-vs-refuse statement, `include_str!`-bound
into both the production adapter and the tests. **Pre-A1 exposure is ZERO**,
verified in code by the Correctness axis (§6): three independent `cfg(test)`
gates stand between production and the v1 writer/migration dispatch —
`#[cfg(test)] mod v1_lifecycle;` (`merge/mod.rs:23-24`), the **entire**
`open_v0` adapter module `#[cfg(test)]` (`record_wire/mod.rs:13-14`), and every
r3-test re-export `#[cfg(test)]` (`merge/mod.rs:44-50`), with
`recover_or_create` carrying no production caller. The unbound dispositions
therefore have **no reachable consumer at this tuple**; their load-bearing
moment is **A1 activation**. C-1 is accordingly **bound to the A1 activation
review's input register as BLOCKING-FOR-A1 per L1-19**, under the routing
`GwzM5-8ThinA1Amendment.md:43-55` already defines for this debt class (branch
(a)/(b)), restated at `:242-244`. Row 2.3b / O8 **stays FAIL**; the fix is
disposition-recording plus the four named rows, by clause, never silently.
Named cheap closures, recorded so A1 inherits them rather than re-deriving
them: (i) registry rows — adapted or declared-refusal — for the 14 unbound
progress shapes, where `assert_i2_valid_unlisted_fixture`'s own closure
assertion (every rule is open+finalizing) forces the refusal answer for most;
(ii) **one parametric test driving `adapt_open` over every non-whitelisted
fixture asserting `ValidUnlisted` + byte preservation**, which alone pins the
F-row residue above; (iii) an archive-equivalence **mechanism decision** for
the ten archive shapes — the registry's two corpora model open-record
adaptation only and there is **no registry vocabulary in which an archive
shape could be bound**, so what O8's archive clause still owes is different and
real: proof that operations *finishing under v1* produce archives
byte-equivalent to v0's for the same scenario. No such comparison exists; that
is A1-shaped work.

**C-2 (= R-2) — GRADED P3, named, owner = the A1 activation review's input
register.** Four scenarios have no durable fixture and cannot be dispositioned
either way: **`B-NOT-STARTED`** and **`B-PREPARING-EMPTY`** (R0 §4 row B's own
recorded gap, re-verified still true — no `NotStarted` injection exists in g23
outside `fixtures.rs`), **`AC-NOPUB-UNBORN`**, and **`AP-PRESERVED`**
(live-workspace GC evidence verified present at
`characterization_preservation_v0.rs:442` and `gc.rs:46`, but no
archive-projection fixture, exactly as R0 §5.2 records). Both axes concur on
P3. Fixture them or route them **named, with the acceptance debt**; the owner
is the A1 activation review's register, alongside C-1.

### 12.8 GATE CONDITION — the byte-equivalence battery is recorded PARTIAL, not green

**Required by the Correctness axis as an explicit condition of its GO**
(round-1 verdict §1 and §9: R4b-G "may close as GO only if its acceptance
record (a) states the byte-equivalence battery as PARTIAL … and (b) binds
C-1/C-2 onto the A1-activation input register"). Stated here, in the acceptance
record, in the form the axis requires:

> **The O8 / `GwzM5-8R4bTransitionDesign.md:1476-1477` byte-equivalence
> battery — "byte-equivalent accepted lock, candidate, root output, and
> archive results **across all M4** and all seven R3 adapted-v0 scenarios",
> restated at `GwzM5-8Refactor.md:2264-2265` — is `PARTIAL`, not met, and is
> recorded as `PARTIAL` in this acceptance record.**
>
> **7 proven + 6 refusal-pinned of 39.** 7 ADAPTED cases carry the full O8
> property (descriptor equality + canonical-JSON SHA256 against the registry,
> I2-vs-v1 restart-action identity, accepted-lock byte identity
> `accepted.lock.exact_yaml == candidate.lock_yaml` + SHA256, the four-point
> upgrade fault matrix, unknown-field survival at six depths, and before/after
> head+status observation proving the upgrade mutates no repository); 6
> UNLISTED cases are **refusal-pinned** — refusal asserted plus byte-exact v0
> preservation plus the whitelist's own closure assertion. **22 UNBOUND** and
> **4 NO FIXTURE** carry no v1 equivalence binding at all. The battery's
> substance is strong **where bound**; the denominator is C-1.
>
> R4b-G accepts the tree with this battery PARTIAL and with **C-1 [P2]** and
> **C-2 [P3]** bound to the A1-activation input register, where **L1-19 makes
> C-1 blocking for A1**. A green byte-equivalence battery is **not** claimed by
> this gate and must not be cited from it.

### 12.9 The C-1 dispositions record — the four live residue rows, stated and tested (added 2026-08-24)

§12.7 grades C-1 P2/BLOCKING-FOR-A1 and names **four rows the class cite does
not settle**: `F-BASELINE`, `F-MARKER`, `F-LOCK` (all `Finalizing`, i.e. inside
the only operation state the whitelist adapts, with **no test driving
`adapt_open` on them**) and `J-NO-PUBLICATION-UNBORN` (class membership
ambiguous per case). This sub-section states each row's adaptation disposition,
**derived from the frozen `GwzM5-8I2CompatibilityContract.md` and its machine
half `GwzM5-8I2CompatibilityPredicates.json`, never invented**, and binds each
to an executed test that drives the shape's real durable form through the real
`decode -> adapt_open -> atomic upgrade` path.

**The dispositions.**

| Shape | Disposition | Contract clause it is derived from | Test that pins it |
| --- | --- | --- | --- |
| `F-BASELINE` — `Finalizing`, step `PublishingCandidate`, candidate + evidence durable, live root still at the pre-publication baseline prefix | **DISPOSITIONED-UNLISTED.** A well-formed descriptor is built; its root observation normalizes to `recorded_evidence` at step `publishing_candidate`, a pair no whitelist rule carries; zero matches; `adapt_open` -> `ValidUnlisted`; atomic upgrade -> `ValidUnlisted` with the v0 bytes exact and nothing staged. | `:117-121` whitelists seven shapes whose only publication-era member is the "exact boundary/index publication prefix" — a baseline prefix is not it, and it is not "evidence recorded" either, which the registry pins at step `committing_evidence`. `:159-160` "Zero whitelist matches is not an error. Open read-only status leaves bytes unchanged". | `compatibility_residue_v0::v0_mid_publication_prefixes_are_not_a1_migration_rules`, subcase `F-BASELINE` |
| `F-MARKER` — same durable record; live root carries the candidate marker only, lock and boundary still baseline, index unpublished | **DISPOSITIONED-REFUSED**, typed `PublicationPrefixMismatch`, before staging. No descriptor is built at all: the observation is not one exact index-aligned publication prefix. v0 bytes exact, nothing staged. The message carries the registry's own frozen sentence, "filesystem or index does not match one legal recorded publication prefix". | `:123-125` "**Marker**/lock-only prefixes … are not A1 migration rules", read with `:121-123` "every selected result, … participant HEAD/ref/index/worktree, and root observation **is exact**". `:167-169` "an unreadable/contradictory row … rejects before staging". Refusal text: registry `rejection_reasons.PublicationPrefixMismatch`. | `compatibility_residue_v0::v0_mid_publication_prefixes_are_not_a1_migration_rules`, subcase `F-MARKER` |
| `F-LOCK` — same durable record; live root carries the candidate marker and lock, boundary still baseline, index unpublished | **DISPOSITIONED-REFUSED**, typed `PublicationPrefixMismatch`, identically. | `:123-125` "Marker/**lock-only** prefixes … are not A1 migration rules", plus the same `:121-123` exactness clause and `:167-169`. | `compatibility_residue_v0::v0_mid_publication_prefixes_are_not_a1_migration_rules`, subcase `F-LOCK` |
| `J-NO-PUBLICATION-UNBORN` | **Two-part, and both parts are now executed.** (i) At the pre-terminal `Finalizing` window the **unborn** twin *is* the whitelisted rule `no-publication-complete-before-terminal`: descriptor equality against the registry, exactly one match, `adapt_open` -> `Eligible` with next action `complete_no_publication` — so the row is **ADAPTED**, per case, not by class. (ii) In R0 §4 row J's own terminal shape (`Completed`, publication `Complete`, no candidate) it is **DISPOSITIONED-UNLISTED**: `adapt_open` -> `ValidUnlisted`, upgrade -> `ValidUnlisted`, bytes exact — for the unborn and born roots alike. | (i) `:117-121`, seventh whitelisted shape, "no-publication complete before the terminal state write"; every rule's `baseline.root_checkout` is `unborn_attached` with `root_commit_hash: absent`, which is the same fact stated in the registry. (ii) `:162-163` "Completed and aborted v0 records remain v0 and use byte-preserving archival", with `:125` "terminal rows are not A1 migration rules". | (i) `compatibility_residue_v0::v0_no_publication_finalizing_twins_split_on_the_born_root_exclusion`; (ii) `compatibility_residue_v0::v0_no_publication_terminal_twins_stay_v0_for_born_and_unborn_roots` |

**(a) Why the three F rows split two ways, which is the whole of C-1's
scenario.** C-1's sharpest live scenario is a workspace interrupted
mid-candidate-publication, and its worry is exact: "if a mid-prefix observation
ever coincided with the boundary descriptor, v1 would adapt with a
boundary-tuned `next_action` and the publication restart would re-drive the
prefix from the wrong position." The measurement is that **all four F shapes
share one durable record** — the record is persisted once when the step
advances to `PublishingCandidate` and not again until the whole prefix is
published, so `F-BASELINE`, `F-MARKER`, `F-LOCK` and `F-BOUNDARY` differ in the
**live observation only**. The descriptor's `publication` block is therefore
byte-identical across all four, and the test asserts that identity against the
registry's `candidate-published-before-recording` rule directly, so the
coincidence C-1 fears is measured rather than assumed. What separates them is
one field: `observation.root`, which is `prefix_boundary` for the published
prefix and `recorded_evidence` for the baseline one. The marker-only and
lock-only prefixes never reach that field, because the live index has not been
published either — the producer stages the lock and marker last — so the
observation is not one exact recorded prefix and the row refuses typed. The
unstaged **boundary** prefix behaves the same way and is driven with them; the
whitelisted `F-BOUNDARY` case is the fully staged one, and it rides in the same
test as the positive control, so the discrimination is non-vacuous in both
directions.

**(b) The J twins, and a label correction the record needs.** [P3-1] asks
"whether the whitelisted no-publication descriptor matches the unborn-root
twin", untested per case. It matches it exactly — and the reason is that the
whitelist has **no born-root rule at all**: `:124` names "born root" in the
excluded list, every rule's baseline descriptor is `unborn_attached` /
`root_commit_hash: absent`, and the adapter classifies a record with a present
`baseline.root_head` straight to `ValidUnlisted`. Consequently §12.3's J rows
carry their labels the wrong way round: the corpus fixture bound there
(`characterization_v0::v0_no_publication_complete_before_terminal_write_is_read_only`)
initialises a workspace whose root is **unborn**, so the ADAPTED case is the
UNBORN twin, and the BORN twin at the same window is valid-unlisted. This
changes **no count** — one J twin is adapted either way, and the 7-ADAPTED /
13-bound arithmetic in §12.6 stands — but the A1 review must not read §12.3's
J-BORN row as a statement that a born-root v0 record can be migrated. It cannot.
Both facts are now executed in one test.

**(c) Why none of the four gained a registry row, and why that is the correct
mechanism.** §12.7's named cheap closure (i) proposes registry rows for the
unbound progress shapes. It does not reach these four: the
`valid_unlisted_corpus` **cannot express a `Finalizing` shape**. Its runtime
binding asserts the whitelist's own closure property (every rule is
open+finalizing) and then `assert_ne!(record.state, OperationState::Finalizing)`
on the fixture; no corpus row's own `operation_state` value is `finalizing`, and
the runtime mapping panics on one. That closure assertion is load-bearing — it
is what makes "not whitelisted" mean something for the corpus — so widening it
to admit these rows would weaken the registry, not extend it. The disposition
is therefore recorded **here, by clause, with an executed test**, which is the
`§12.7` branch "an O8 acceptance note citing the clause per row". §12.2 defines
the two new disposition tokens; the map cites the tests in the ordinary way, so
`check_m4_scenario_map.py` verifies they exist and the driver pins the count.

**(d) What C-1 still owes after this, unchanged.** This closes the four named
rows and nothing else. **18 UNBOUND rows remain** (10 progress + 8 archive),
still disposed of categorically by `:117-125` / `:159-165` and byte-pinned per
shape, still lacking a per-scenario record; the **archive-equivalence mechanism
decision** for the ten archive shapes is untouched and remains A1-shaped work
(there is no registry vocabulary in which an archive shape could be bound, and
what O8's archive clause owes is a comparison of archives produced by
operations *finishing under v1* against v0's, which does not exist); **C-2
[P3]** is untouched. §12.8's byte-equivalence verdict therefore stays
**PARTIAL** and must still not be cited as green: the four rows gain an
executed adaptation disposition, which is not the O8 byte-equivalence property.

**(e) E5 landing corrections and the K/J(ii) record (added 2026-08-28 at the
R2-E E5 landing; interior review adjudication A and findings
[P1-1]/[P2-1]/[P2-4]; lane-owner determination).**

- **(d)'s "10 progress" is a 9+1 partition.** Nine of the ten UNBOUND
  progress rows gained `valid_unlisted_corpus` registry rows at E5.1
  (`open/executing`, `open/awaiting-resolution`, `open/halted`,
  `preserving/pre-acceptance`, `preserving/candidate`,
  `preserving/root-prefix`, `rollback/pre-acceptance`, `rollback/evidence`,
  `terminal/completed-no-publication`), each driven through the real
  `decode -> adapt_open -> atomic upgrade` path by the one parametric test
  the L6 ruling required. The tenth, `G-VERIFYING`, is (c)'s own case
  arriving one row later: it is a `Finalizing` shape (R0 §4 row G, the
  pairing `VerifyingPublication`↔`Finalizing`), so the corpus cannot hold it
  without deleting the load-bearing closure assertion, and its disposition is
  DISPOSITIONED-UNLISTED, recorded by clause with an executed arm of the same
  test — §12.7's second branch, the F-BASELINE mechanism. E0.2b §8 [P2-2]'s
  "10 registry rows" is therefore right as a shape count and off by one as a
  registry-row count, for (c)'s own reason.
- **(c)'s ground, corrected.** Its Finalizing-inexpressibility argument
  reaches the three F rows and now `G-VERIFYING` — four `Finalizing` rows.
  It never grounded `J-NO-PUBLICATION-UNBORN`: part (i) is ADAPTED at its
  `Finalizing` window through the whitelist (Finalizing-ness is what makes it
  adaptable, not what excluded it), and part (ii) is a terminal `Completed`
  shape — a state the corpus could already express on the day (c) was
  written (`terminal/completed`, `terminal/aborted` existed). J(ii)'s
  DISPOSITIONED-UNLISTED record stands on its executed test and its contract
  clauses (`:162-163`, `:125`), not on inexpressibility.
- **The K/J(ii) same-object determination ([P2-1]).** E5.1's
  `terminal/completed-no-publication` row binds `K-COMPLETED-NOPUB-OPEN`,
  and its durable object is the object of J(ii)'s R0-terminal shape: the
  corpus fixture is the identical construction to `compatibility_residue_v0`'s
  unborn no-publication twin (measured by the E5 review, byte-for-byte, and
  by construction the unborn one). One durable object, two R0 rows — the
  same sanctioned pattern §12.6 records for `terminal/completed`. Both
  records stand, because they answer different ledger questions: J(ii)'s
  clause record answers R0 row J's per-case adaptation disposition
  (2026-08-24); the registry row answers Table A row K's binding obligation
  (E5.1, 2026-08-28). Registry-row arithmetic in §12.6's form: 22 rows bind
  22 shape labels; the durable-object count is lower — `terminal/completed`
  is one object for two R0 rows, and `terminal/completed-no-publication`'s
  object also carries J(ii)'s clause record. This determination rides the E7
  dual for second-axis scrutiny.

**Executed run binding.** Tree gwz-core `1bd885f` plus this package, clean
otherwise; Darwin 25.5.0 / arm64. `cargo test --lib -p gwz-core
workspace_ops::tests::g23::` -> **114 passed; 0 failed** (111 + the three tests
above), and `scripts/checks/check_m4_scenario_map.py` -> ok at the new counts
(39 scenario rows, 41 named tests, 13 registry rows all claimed), with the
aggregate driver's pinned markers moved to match in the same package.

---

## 13. R4b-G FAIL remediation — executed 2026-08-24

Same tree, same host, later session: gwz-core **`b91bdeb`**, clean at start;
Darwin 25.5.0 / arm64; Python **3.13.12** (`/opt/homebrew/bin/python3.13` —
the host has moved on from §11's 3.13.7); every cargo invocation under a
scratch `CARGO_TARGET_DIR` outside every repository. **No git write
operation. No production line. No digest pin moved.**

### 13.1 What landed, by FAIL

| FAIL | Landed | Files | LOC |
| --- | --- | --- | ---: |
| **F-1** (W4) | §12 above — the 39-scenario M4 enumeration from `GwzM5-8R0Inventory.md` §4/§5.2, each scenario mapped to its tests and its registry binding — plus the script that keeps the map honest: every test it names must exist, every registry row must be claimed by exactly one scenario, every cited case id must exist. | `scripts/checks/check_m4_scenario_map.py` (new); §12 of this document | 144 |
| **F-2** (W1) | Temp-copy compile probes for the sealed lifecycle perimeter, in the `run_compiler_probe` idiom but inverted: there the compiler accepts and the checker must reject, so the probe asserts returncode 0 first; here the compiler **is** the enforcement, so the negatives assert it rejects and a **positive control** compiles byte-identical probe text from inside the seal. Without the control a renamed or deleted item would make every negative pass for the wrong reason. Three seals proven separately, and the raw writer proven sealed **against the rest of the lifecycle** too, not merely against the outside. | `scripts/checks/test_v1_lifecycle_privacy_probe.py` (new) | 138 |
| **F-3** (W2) | Masked-token scan over all **128** `v1_lifecycle/**/*.rs` (including the ten `#[path]`-mounted test files §7.3 warns about — they resolve inside the tree, so `rglob` reaches them), seam **derived** from `workspace_ops/merge/mod.rs`'s own `use store::{…}` re-exports — **9 names today**: `MergeStore`, `FileMergeStore`, `archive_merge_record`, `enter_finalizing`, `persist_merge_record`, `persist_operation_transition`, `AtomicUpgradeFault`, `AtomicUpgradeOutcome`, `upgrade_open_v0_for_r3_tests` — so a newly exported v0 persistence item is covered the day it is added, with a floor check that fails the derivation closed if that re-export is restructured away. Measured: `enter_finalizing` is **10 raw grep hits / 0 masked hits** in that tree. Four fail-closed tests, one of them a compiler probe proving the violation compiles before asserting the checker rejects it. Comment marks it **load-bearing for J-1**. | `scripts/checks/check_checked_artifact_boundaries.py` (+76), `scripts/checks/test_check_checked_artifact_boundaries.py` (+74) | 150 |
| **F-4** (W5) | The seven-battery driver, each command carrying the output marker its green result must print. | `scripts/checks/run_r4bg_aggregate_gates.py` (new) | 150 |
| **F-5** | Runbook step 5 corrected in place to two lines with the reason recorded. | `GwzM5-8R4bG-EvidenceInventory.md` §5 step 5 | doc |
| **F-7** | Pricing record drafted at §8 item 3 addendum; register row drafted, **not inserted**, for `GwzM5-8R2DSettledTuple.md` §11.1; **RULING unsigned**. **[SUPERSEDED 2026-08-24 (Evidence [P3-3] / Correctness C-7.3): the ruling was SIGNED and the register row INSERTED at gwz-dev `d378db9`, 2026-08-24 10:24:43. Signature date corrected to 2026-08-24 at §8 item 3; the inserted tuple row is corrected to the same date this pass. F-7 = record written, ruling signed.]** | §8 of this document | doc |
| corrections | `clippy.toml` 19-not-21 and `FinalizationFault` eight-not-seven, as bracketed dated corrections in place (not silent rewrites); plus the `ChangeBudget.md:515` column transposition this document itself carried (§7.5 step 8). | `GwzM5-8R4bG-EvidenceInventory.md`, §7.5 here | doc |

**Ledger.** 0 net production-bearing Rust, 0 moved, **1,177 net test/tool/doc
LOC** (582 tool + 595 doc), **0 production files**, **7 test/tool/doc files**.
Against `ChangeBudget.md:515` read correctly (§7.5 step 8): inside every
column, at **78% of the ≤1,500 test/tool/doc ceiling** — the tightest column,
and the reason the driver was held to 150 lines even though nothing required
it. Doc lines dominate (595 of 1,177) because §12's enumeration and this
section are the deliverable, not overhead.

**[Charging convention, stated 2026-08-24 (Evidence axis [P3-4](ii), which
found the recorded 1,177 to be "neither convention" and unreproducible without
guessing).** The figure uses **per-pass net charging**: each pass charges the
`git diff --numstat` net of exactly the files *it* wrote — here 582 tool LOC in
gwz-core `78badbc` (five `scripts/checks/` files, +582/−0, zero `src/` lines)
plus 595 dev-docs lines at gwz-dev `d378db9` (evidence +552/−25, inventory
+73/−5; net 595, verified by the axis). **Applied consistently, that convention
also charges pass 1**, whose own text declared "this document spent 0 of all
five" for an 851-line evidence document — that declaration is **wrong under
this convention** and is corrected here, and the two-pass cumulative
test/tool/doc column is therefore **2,028 (582 + 595 + 851), over the ≤1,500
ceiling**, not 78% of it. The only reading under which the package fits is the
program's **review-artifact precedent** — review reports and the settled tuple
are never charged — which yields **582** and no doc column at all. This record
does not pick between them, because `GwzM5-8ChangeBudget.md` is the operator's
row: **the R4b-G ledger row is filed as OPEN on the convention question**, with
both reproducible figures (**2,028 uniform / 582 review-artifact**) and the
recorded 1,177 marked superseded. Every underlying number here is diff-verified;
what was missing was the rule, and the rule is now named.]

**F-6 is not this pass's** — the Windows/Platform matrices were dispatched
separately (`CurrentProgramCheckpoint.md:640-641`, run 22 at `b91bdeb`).

### 13.2 The driver, and why its end-to-end run is partitioned

`run_r4bg_aggregate_gates.py` drives the seven gates
`GwzM5-8Refactor.md:2243-2244` names. Two properties matter to the review:

1. **A command that exits 0 while printing the wrong count still fails.**
   Each command declares the marker its green result must contain — `254
   passed`, `ok (11 sources, 147 assertions)`, `validated 7 migration rules
   and 7 runtime bindings`. This is W5's "expected count on the settled tree",
   mechanised rather than recited.
2. **The seventh gate can never make it green.** `settled-tree-review` has no
   commands; it prints REVIEW, and the exit code says only that *the
   mechanical gates in the selection* pass.

**On "executed end-to-end once":** the seven batteries need ~25 minutes of
wall clock, of which the `fault` battery alone is **683 s** of pure test
execution (355.9 s + 327.6 s) — more than this session's 600 s per-command
budget, the same budget that forced §3.1's four-way partition of the lib
suite. The driver therefore carries a `battery:index` selector, and a
partitioned run prints **PARTIAL** and withholds the aggregate pass line so
the reconciliation stays explicit. All seven batteries were executed, green,
across **four invocations**, disjoint and exhaustive:

| Invocation | Selection | Result |
| --- | --- | --- |
| 1 | `compatibility byte-equivalence unknown-field privacy` | 9 commands, all ok |
| 2 | `fault:1` | ok, PARTIAL |
| 3 | `fault:2` | ok, PARTIAL |
| 4 | `call-graph settled-tree-review` | 3 commands ok + REVIEW |

14 commands across 6 mechanical batteries + 1 review gate = the full set.

### 13.3 Gate tails, verbatim

```
$ cargo check --all-targets                       # workspace
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 24.54s
exit=0

$ cargo fmt --all -- --check                      # gwz-core
exit=0

$ python3.13 -m unittest scripts/checks/test_v1_lifecycle_privacy_probe.py -v
test_prepared_v1_rewrite_is_unnameable_outside_the_perimeter ... ok
test_proof_tokens_are_unnameable_outside_the_perimeter ... ok
test_raw_v1_writer_is_unnameable_from_the_rest_of_the_lifecycle ... ok
test_raw_v1_writer_is_unnameable_outside_the_perimeter ... ok
test_sealed_names_exist_and_compile_inside_the_perimeter ... ok
Ran 5 tests in 96.953s
OK

  # and the same suite against a copy of the tree with the seal widened to
  # `pub(crate) mod transition;` + `pub(crate) struct PreparedV1Rewrite`:
  FAIL: test_prepared_v1_rewrite_is_unnameable_outside_the_perimeter
  AssertionError: 0 == 0
  Ran 5 tests in 99.443s
  FAILED (failures=1)
  # exactly one probe fails, the widened one; the positive control stays green.

$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
exit=0                                                     (7.14s)

$ python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py
Ran 69 tests in 703.295s
OK
exit=0
  # 65 on 2026-08-23 (§3, gate 6) + F-3's four: the compiled-and-rejected
  # persistence probe, the test-code probe, the seam-underivable probe, and
  # the string-literal negative control.

$ python3.13 -m unittest scripts/checks/test_release_boundary.py
Ran 6 tests in 0.047s
OK
exit=0

$ python3.13 scripts/checks/check_m4_scenario_map.py
M4 scenario map: ok (39 scenario rows, 38 named tests, 13 registry rows all claimed)
exit=0

$ cargo test --lib -p gwz-core checked_artifact::
test result: ok. 400 passed; 0 failed; 0 ignored; 0 measured; 1170 filtered out; finished in 38.68s

$ cargo clean -p gwz-core
$ CLIPPY_CONF_DIR="$PWD" cargo clippy -p gwz-core --all-targets --all-features -- -D warnings
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 13.72s
exit=0        (0 lines matching '^error')

$ cargo clippy --workspace --all-targets --all-features -- -D warnings
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 13.32s
exit=0        (0 lines matching '^error')
```

**F-5's two verifications, executed rather than argued.** The runbook's OLD
single line still reproduces exactly the recorded failure, and the hits are
not suppressed:

```
$ CLIPPY_CONF_DIR="$PWD" cargo clippy --workspace --all-targets --all-features -- -D warnings
exit=101      7 lines matching '^error: use of a disallowed method'
--> gwz-cli/src/pager.rs:127:23
--> gwz-cli/src/pager.rs:131:21
--> gwz-cli/src/tests/g01/commands.rs:460:9
--> gwz-cli/src/tests/m2c.rs:51:5
--> gwz-cli/src/tests/m2c.rs:55:5
--> gwz-cli/src/tests/m2c.rs:85:5
--> gwz-cli/src/tests/m2c.rs:99:5

$ grep -rn 'allow(clippy::disallowed_methods)\|expect(clippy::disallowed_methods)' gwz-cli/src/
(no matches)
$ ls gwz-cli/clippy.toml /Users/owebeeone/limbo/gwz-dev/clippy.toml
(neither exists)
```

Same 7 sites, same order, as §3.3 recorded on 2026-08-23; **no `allow` or
`expect` anywhere in gwz-cli**, and no `clippy.toml` in gwz-cli or at the
workspace root. The perimeter is gwz-core's because gwz-core is where it is
declared, and the corrected step's second line still compiles all 7 sites
under `-D warnings`.

**Driver invocations, verbatim:**

```
=== compatibility -- v0 compatibility gate (evidence row 2.2)
    ok    frozen predicate registry (0.0s, 'validated 7 migration rules and 7 runtime bindings')
    ok    registry checker suite (0.1s, 'OK')
    ok    merge-doc assertions (0.2s, 'ok (11 sources, 147 assertions)')
    ok    merge-doc checker suite (0.5s, 'OK')

=== byte-equivalence -- byte-equivalence gate, both halves of O8 (rows 2.3a/2.3b, §12)
    ok    M4 scenario map (0.3s, 'M4 scenario map: ok')
    ok    g23 adapted-v0, characterization and upgrade suites (26.4s, '111 passed')

=== unknown-field -- unknown-field gate (evidence row 2.4)
    ok    record wire unknown/archive/decode (0.1s, '75 passed')
    ok    exact unknown manifest per transition effect (1.2s, '1 passed')

=== privacy -- privacy gate (row 2.5b / W1, TransitionDesign:1478-1479)
    ok    sealed v1 lifecycle compile probes (77.9s, 'OK')

=== R4b-G aggregate gate summary
    ok      compatibility
    ok      byte-equivalence
    ok      unknown-field
    ok      privacy
AGGREGATE: this selection's mechanical gates pass; the settled-tree review is not.

=== fault:1 -- aggregate fault/restart matrices (TransitionDesign:1469-1475)
    ok    v1 lifecycle fault and restart matrices (355.9s, '254 passed')
AGGREGATE: PARTIAL -- fault:1 ran one command only;
reconcile the remaining commands across invocations before claiming a pass.

=== fault:2 -- aggregate fault/restart matrices (TransitionDesign:1469-1475)
    ok    root physical/successor boundary matrix (release profile) (327.6s, '1 passed')
AGGREGATE: PARTIAL -- fault:2 ran one command only;
reconcile the remaining commands across invocations before claiming a pass.

=== call-graph -- call-graph gate, both halves (rows 2.6a/2.6b, TransitionDesign:1480-1481)
    ok    structural boundary and v1->v0 persistence guard (5.7s, 'checked-artifact boundary: ok')
    ok    boundary checker suite and compiler probes (516.9s, 'OK')
    ok    release boundary suite (0.2s, 'OK')

=== settled-tree-review -- two independent full-tree R4b reviews (AgentProcessRules.md:2006)
    REVIEW -- not mechanisable; this driver's record is its input

=== R4b-G aggregate gate summary
    ok      call-graph
    REVIEW  settled-tree-review
AGGREGATE: this selection's mechanical gates pass; the settled-tree review is not.
```

**A fourth measurement of the D3 perf figure**, incidental but worth filing
against §8 item 3: `fault:2` reports **327.6 s** wall for the release
`root_fault_matrix` including cargo overhead, against §3.1's **318.71 s** of
libtest time on the same tree. Consistent; neither is anywhere near 576 s.

### 13.4 What this pass deliberately did not do

- **Move no pin.** `PROTECTED_SOURCE_DIGESTS`, `PROTECTED_SOURCE_TREE_DIGESTS`
  and `PROTECTED_COMPILER_ROOT_DIGESTS` are untouched; the checker still
  prints its 2026-08-23 string, `ok (15 visible entries, 5 classified
  modules)`. Pins are the lane owner's.
- **Sign no ruling.** F-7's RULING line is unsigned and its
  `GwzM5-8R2DSettledTuple.md` register row is drafted in a block quote, not
  inserted. J-1, J-2, J-3, J-5, J-6, J-7 are untouched as judgments; J-1 and
  J-3 gained facts, not verdicts.
  **[SUPERSEDED 2026-08-24 — Evidence axis [P3-3] and Correctness axis C-7.3
  found this bullet still describing a state that had already changed within
  the same commit. It was true of the drafting pass and is **false of the
  record as it stands**: the F-7 RULING **is signed** and its register row
  **is inserted** in `GwzM5-8R2DSettledTuple.md` §11.1, both at gwz-dev
  `d378db9` (2026-08-24 10:24:43). The J-* sentence still holds for this pass,
  but the judgments have since been ruled by the R4b-G dual itself: **J-1
  RATIFIED by both axes** (round 1), with J-2/J-4/J-5 concurred and J-3
  concurred-with-C-6's-sharpening by the Correctness axis, and J-7 ruled
  acceptable-with-the-debt-registered. The pin statement above is unchanged and
  still true.]**
- **Paper over nothing.** F-1's enumeration made O8's clause checkable and the
  answer came back **negative** — 13 of 39. Row 2.3b and O8 stay **FAIL**, and
  §12.6 opens two new residual findings, R-1 and R-2, as inputs to the R4b-G
  dual rather than as closures.
- **Touch no member repo.** gwz-cli and gwz-py are unmodified; F-5 was a
  runbook-scope defect, not a gwz-cli defect, and nothing in this pass needed
  a change outside gwz-core's `scripts/checks/` and these two dev-docs.
- **Wire nothing into CI.** Workflow files are untouched. What that means, per
  gate, stated so the review does not have to derive it:
  - **F-3 runs in CI today.** It lives inside
    `check_checked_artifact_boundaries.py`, which
    `checked-artifact-boundary.yml:30-35` already runs per commit, along with
    its unit suite — so F-3's four fail-closed tests are in the push lane too.
    That was the reason for choosing that home over a standalone script.
  - **F-2 and F-4 do not.** Neither `test_v1_lifecycle_privacy_probe.py` nor
    `run_r4bg_aggregate_gates.py` is named by any workflow. Adding a CI step
    is a workflow change and belongs to the lane owner.
  - **F-1's checker cannot run in CI as written** — a **J-7-class** limitation
    and the sharpest of the three. The M4 map lives in the gwz-dev workspace's
    `dev-docs/`, one level above the gwz-core checkout, so
    `check_m4_scenario_map.py` resolves it exactly as `check_merge_docs.py`
    resolves its workspace root, and inherits exactly that gate's weakness —
    the one the tuple's §11.3(7) calls "a gate that only passes on a
    developer's machine is not a gate". It is fail-closed rather than
    vacuous (an absent map exits 1, never 0), and the map's natural CI-able
    home would be `gwz-core/dev-docs/`, which this pass had no mandate to
    create. **Flagged for the review, not worked around.**
