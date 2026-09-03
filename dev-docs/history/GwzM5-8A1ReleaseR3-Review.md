# GwzM5-8 A1 Release R3 — interior single-axis review (release-truth surfaces, G3/G4/G6)

Date: 2026-08-25. Axis: interior single-axis, adversarial. Reviewer: the R3
review lane. Object: the R3 builder's package, uncommitted in
`/private/tmp/claude-501/-Users-owebeeone-limbo/3a4f2794-661b-40df-b586-1f52221017f3/scratchpad/r3-release/`
at gwz-core `8e40fa8` (6 files, +106/−4), PLUS the already-committed companion
gwz-cli `cf0d16d` (`docs/commands/merge.md`).

## VERDICT — **NO-GO as filed.**

| Gate | Ruling |
| --- | --- |
| **G3** — the A1 decode generation | **GO WITH CONDITIONS** — the entry is well-formed, states shipped behaviour, and cannot be confused with the frozen contract row. Two P2 under-coverage findings on what it omits. |
| **G4** — release-lane battery wiring | **GO** — the topology audit is correct command by command, the PARTIAL path cannot mask a failure, the call-graph exclusion holds, Linux-only is right. P3s only. |
| **G6 — versions** | **GO** — the convention finding is correct, independently re-derived from history; the member-repo proposal is accurate and is *more* accurate than the repos' own `RELEASE.md` files. P3s only. |
| **G6 — notes / the `cf0d16d` companion** | **NO-GO** — **[P1-1]** the committed fix breaks a green release-lane gate and the finding it was filed under is false; **[P2-3]** the older-reader claim the notes must make is wrong for two of the four releases it names. |

**The evidence-digest re-pin is RULED SOUND — not a rubber stamp.** Full
argument at §2; the builder's reasoning is correct and I verified it from the
harness source and by execution in both directions. One narrowing condition on
the *rule* (not this application) is recorded as [P3-7].

**Escalation:** this review carries 1 P1 and 3 P2, so it auto-escalates per the
plan §10 rule for R3 packages.

---

## 1. G3 — the A1 decode generation

### 1.1 Well-formedness against the schema — PASSES

`scripts/retained_readers/manifest.json` gains one `decode_generations` entry:

- `id`: `v0-v1-dual-decode-v0-writer-floor` — `$defs/identifier`,
  `{"type":"string","minLength":1}` (`manifest.schema.json:40`, referenced at
  `:52`). Non-empty, unique. Uniqueness is separately enforced by `_unique_ids`
  (`retained_reader_harness.py:76-83`).
- `release`: `v0.11.0` — matches `$defs/release`
  `^v[0-9]+\.[0-9]+\.[0-9]+$` (`manifest.schema.json:41`) and the harness's own
  `RELEASE_RE` (`retained_reader_harness.py:34`, enforced at `:145`).
- `description`: non-empty (`manifest.schema.json:54`;
  `retained_reader_harness.py:146`).
- `additionalProperties:false` with exactly `{id, release, description}`
  (`manifest.schema.json:49-55`) — the entry carries exactly those three.

Executed (L2-04, my run, worktree, verbatim):

```
=== L2-04 validate ===
{"status": "valid", "tuple_count": 24}
exit=0
=== L2-04 gate-ready ===
{"status": "manifest-ready", "tuple_count": 24}
exit=0
```

### 1.2 SHIPPED behaviour vs the frozen contract row — PASSES, and well

Frozen contract `GwzM5-8I2CompatibilityContract.md` §2 creation matrix,
A1 row: `| A1 | v1 | v1 | unsupported | unsupported | unsupported |` —
**ordinary/custom = v1**.

The generation description says the opposite, explicitly, with the mechanism,
the owner and the discipline:

> "The A1 generation, described as shipped: the reader decodes both v0 and v1
> merge records, and --no-ff starts create v1 records, but ordinary and
> custom-message starts still create v0 because the active writer floor ships
> at ACTIVE_WRITER_FLOOR = V0 — the named residual owned by milestone M5c,
> which raises the floor with the production v1 ordinary-start owner as one
> reviewed change."

Cross-checked against the tree, not the prose:
`ACTIVE_WRITER_FLOOR = RecordVersion::V0`
(`src/workspace_ops/merge/model/version.rs:39`);
`RequestedSemantics::Ordinary => Ok(RecordVersion::V0)` and
`Self::NoFf => Ok(RecordVersion::V1)` (`version.rs:71-77`);
`InstalledMergeRecordVersions::PRODUCTION = Self { v0: true, v1: true }`
(`src/workspace_ops/merge/record_wire/header.rs:30`). All four clauses of the
description are true of the tree.

It self-labels *"described as shipped"*, names `ACTIVE_WRITER_FLOOR = V0`, and
names M5c and the "one reviewed change" discipline. It **cannot** be mistaken
for the contract row. Safety §R2.2 binding condition (c) is **MET**.

### 1.3 The tuple-count-unmoved reasoning — VERIFIED FROM SOURCE

The builder's in-comment reasoning
(`test_retained_reader_harness.py:87-90`) is that generations without artifacts
add no reader rows and `gate_readiness_errors` walks readers only. I did not
take the comment; I read the code.

- `iter_tuple_ids` (`retained_reader_harness.py:348-351`) iterates
  `manifest["readers"]` → `reader["artifacts"]` and yields
  `f"{reader['surface']}:{reader['release']}:{artifact['platform']}"`. It never
  touches `decode_generations`.
- `gate_readiness_errors` (`:370-381`) iterates `manifest["readers"]` only.
- `main()` computes `tuple_count = sum(1 for _ in iter_tuple_ids(manifest))`
  (`:466`) — the same reader-only walk.
- Repo-wide, `decode_generations` is consumed **only** by
  `_validate_manifest` (`:139-147`), the schema, and the two test files.
  `decode_generation` (singular) additionally only at `:190-195` (the reader →
  generation cross-reference and the release-equality check) and
  `test_retained_reader_matrix.py:243`.
- Crucially, `_validate_manifest` imposes reader → generation, **not**
  generation → reader: a generation with no readers is legal by construction.

Empirical confirmation, both sides: the pre-change manifest
(`git show HEAD:scripts/retained_readers/manifest.json`) validates to
`tuple_count: 24`; the changed manifest validates to `tuple_count: 24`.
**Unmoved. The reasoning is correct and the comment is accurate.**

### 1.4 [P2-1] The description omits the ordinary → v1 **migration** path

`precheck` (`src/workspace_ops/merge/store/mod.rs:254-262`):

```rust
fn precheck(record: &MergeOperationRecord) -> AdaptationPrecheck {
    if record.state == super::OperationState::Finalizing
        && record.mode == super::MergeExecutionMode::Normal
    {
        AdaptationPrecheck::MayAdapt
```

`MergeExecutionMode::Normal` is the **ordinary** mode. So an ordinary start's
v0 record in `Finalizing` is whitelist-eligible and migrates to v1 through
`adapt_before_mutating` (`runtime/dispatch.rs:394-403`) on Resume/Abort.

The description says ordinary starts "still create v0". True at creation —
and materially incomplete for a register whose entire purpose is *what an
older retained reader will meet on disk*. A user who never typed `--no-ff`,
whose merge was interrupted in `Finalizing` and resumed, has a **v1 record**
in their store; a retained v0.10.2 reader then refuses it.

**Cure (small, in-package):** extend the description with one clause — e.g.
"…still create v0; an eligible ordinary v0 record may still migrate to v1 on
resume through the seven-shape whitelist, so a v1 record can appear in a
workspace where no `--no-ff` start was ever run." This is also a release-notes
claim (§5).

### 1.5 [P2-2] The register now under-covers: v0.10.3/v0.10.4/v0.10.5 have no generation, and v0.10.4/v0.10.5 differ materially from v0.10.2

Registered generations after this change:

| id | release |
| --- | --- |
| `pre-v0-record-reader` | v0.9.2 |
| `v0-mode-known-recovery-dormant` | v0.10.2 |
| `v0-v1-dual-decode-v0-writer-floor` | **v0.11.0** |

Nothing for v0.10.3, v0.10.4, v0.10.5 — and v0.10.4/v0.10.5 are **not** the
v0.10.2 generation. Verified at the tags in the r3 worktree:

- v0.10.2, v0.10.3: **no `record_wire` module at all**
  (`git ls-tree -r --name-only v0.10.3 | grep -c record_wire` → `0`;
  `b0781b4` "Install strict merge record envelope decoding" is **not** an
  ancestor of v0.10.3).
- v0.10.4 (41 `record_wire` files) and v0.10.5 (9):
  `InstalledMergeRecordVersions::PRODUCTION_R3 = Self { v0: true, v1: false }`
  (`v0.10.5:src/workspace_ops/merge/record_wire/header.rs:25-28`), and a v1
  envelope lands in
  `HeaderClassificationError::Unsupported { header, required_wave }`
  (`:47-54`) — a **typed unsupported-record-version** outcome carrying the
  required wave, *not* `record_unreadable`.

The manifest schema itself treats these as different facts:
`"classification": { "enum": ["unsupported_record_version", "record_unreadable"] }`
(`manifest.schema.json:185`). The v0.10.2 readers are pinned as
`record_unreadable` for every v1..v4 and `future@99` envelope.

Two readings, both problematic:

- **Old meaning** (a keying table for the four `FROZEN_R0_READERS`,
  `retained_reader_harness.py:36-41`): the hole is fine, but then adding a
  generation with **no readers** is a new pattern and the entry is describing
  a release the register does not otherwise model.
- **New meaning** (a decode-behaviour history — which is what "described as
  shipped, ahead of its artifacts" makes it): the register now reads as if
  v0.10.2's behaviour held unchanged until v0.11.0. **That is false**, and
  falsely on the conservative side (it under-reports how much better v0.10.4/
  v0.10.5 behave).

**Cure options (lane owner's choice):** (a) constrain the new entry's meaning
in-description ("registers the v0.11.0 binary's own decode behaviour ahead of
its retained artifacts; the register keys retained readers only"), or (b) add
the missing v0.10.4/v0.10.5 strict-envelope generation. (a) is ~1 line and
sufficient; (b) is more truthful and still adds no reader rows.

No gate breaks either way — validate/gate-ready are green and the tuple count
is unmoved. This is a *truth* finding, which is exactly what G3 is for.

---

## 2. THE EVIDENCE DIGEST RE-PIN — **RULED SOUND**

The one edit that could have been a rubber stamp:
`scripts/retained_readers/evidence-macos-aarch64.json` `inputs.manifest_sha256`
hand-updated `9b1af0e3…` → `dacdb187…` while keeping all 314 recorded
macOS-arm64 execution results. I checked it eight ways.

**(1) Arithmetic, both directions.**

```
manifest.json actual sha256:
dacdb187d0ad154a8c7ef59335f1608ea109e218c496451d8dee8a2d7eab6000  manifest.json
--- recorded in evidence ---
{ "cases_sha256": "facb07f0…", "manifest_sha256": "dacdb187d0ad154a8c7ef59335f1608ea109e218c496451d8dee8a2d7eab6000" }
```

and, for the *old* value, against `HEAD:scripts/retained_readers/manifest.json`:

```
9b1af0e3e85e006baced4a32d523c55a3df16c0f5205eae7f28bd3c3feb2dac2  .../prev/manifest.json
```

The old pin was correct for the old manifest; the new pin is correct for the
new one. No transcription error, no stale second digest (`cases_sha256`
unchanged and `cases.json` unchanged).

**(2) The digest check is a file digest, re-derived at validation time.**
`validate_evidence_document` (`retained_reader_evidence.py:242-262`) builds
`expected_inputs = {"manifest_sha256": _sha256(manifest_path), …}` (`:252-254`)
and rejects on mismatch (`:255-256`). So this field cannot drift silently.

**(3) The result set is re-validated *after* the digest check.** `:257`
`validate_result_set(manifest, cases, platform, document.get("results"))`.
The builder's central claim, and it is literally in the code.

**(4) `expected_result_keys` is reader-keyed.** `:179-199` — iterates
`manifest["readers"]`, selects that reader's artifact for the platform, and
keys on `(reader["id"], case_id)`. No generation term anywhere.

**(5) The one indirect path also does not depend on generations.**
`expected_result_keys:183-186` calls
`retained_reader_matrix.validate_cases(cases, manifest)`, which calls
`harness.validate_manifest(manifest)` (`retained_reader_matrix.py:137`) — so
the manifest must be **valid** (it is; §1.1) — then derives the case set from
`{reader["id"] for reader in manifest["readers"]}` (`:139`) and
`_validate_envelope_cases` (`:53-76`), which reads only
`reader["record_envelopes"]` and `reader["projections"]`. **No generation
dependency.**

**(6) The subtle one: provenance is NOT keyed on `manifest.json`.**
`validate_source_provenance` (`:224-231`) compares against
`source_digests(source_root)` over `EVIDENCE_SOURCE_NAMES` (`:21-36`) —
`manifest.schema.json` is in that set, **`manifest.json` is not**. So a
manifest *content* change leaves `sources` / `source_set_sha256` /
`evaluator_sha256` untouched. This is the trap the re-pin could have sprung,
and it does not.

**(7) Fixture provenance is filesystem-derived** (`:260-262`), independent of
the manifest.

**(8) Executed.** Harness suite green (§6); validate/gate-ready green (§1.1).

**Ruling: the recorded macOS-arm64 execution results remain valid under the new
digest. The re-pin is a correct, load-bearing edit, not a rubber stamp.**
The builder flagged it for review itself, which is the right instinct.

### [P3-7] The soundness *rule* as stated is broader than the code supports

"Readers did not change, so the result set is unaffected" is true here but is
**not** the general rule. `validate_evidence_document` never re-derives
`document["artifacts"]` or `document["runtime_artifacts"]` from the manifest —
compare `build_evidence:319-337`, which *does* build them from
`reader["artifacts"]` (name, sha256, support, reason) and
`runtime.get("artifacts", [])`. A future digest re-pin justified by
"readers didn't change" would silently keep a **stale embedded artifacts
block** if reader artifact *metadata* changed.

The rule to record for the register: *a manifest-digest re-pin without
re-execution is sound only when the manifest delta touches nothing the evidence
document embeds and nothing `expected_result_keys` reads — i.e. readers, their
artifacts, their `record_envelopes`/`projections`, runtimes' artifacts, and
platforms.* Here the delta is +5 lines confined to `decode_generations`, so it
qualifies. Worth one sentence in the retained-readers `README.md`.

---

## 3. G4 — the `release.yml` R4b-G battery step

### 3.1 (a) Topology safety, command by command — **CONFIRMED**

The wired selection is `fault byte-equivalence:2 unknown-field privacy`
(`.github/workflows/release.yml:160-161`).

**The load-bearing claim confirmed first:** `byte-equivalence:1` **IS**
`check_m4_scenario_map.py`.

```python
"byte-equivalence": ("byte-equivalence gate, both halves of O8 (rows 2.3a/2.3b, §12)", [
    ("M4 scenario map",
     check("check_m4_scenario_map.py"), …),          # run_r4bg_aggregate_gates.py:86-89  → index 1
    ("g23 adapted-v0, characterization and upgrade suites",
     lib("workspace_ops::tests::g23::"), "119 passed"),  # :90-91                          → index 2
```

with `commands = [all_commands[int(index) - 1]] if index else all_commands`
(`:115`). **Wiring `byte-equivalence` whole would have reproduced run
`32749441874` exactly** — the checker's `MAP_DOC = ROOT.parent / "dev-docs" /…`
resolves one level above a single-repo checkout. Selecting `:2` is correct and
the reasoning in the comment (`release.yml:113-119`) is accurate.

Per remaining command:

- `fault` (4 cmds), `unknown-field` (2), `byte-equivalence:2` (1) — all
  `cargo test` over this repo's own lib (`run_r4bg_aggregate_gates.py:60-61`,
  `:65-76`, `:90-97`). `subprocess.run(argv, cwd=ROOT)` with
  `ROOT = parents[2]` = the repo (`:47`, `:123`). No path leaves the checkout.
  ✔
- `privacy` — one command, `python -m unittest
  scripts/checks/test_v1_lifecycle_privacy_probe.py` (`:56-57`, `:98-100`).
  Topology-safe, but see **[P3-1]**: the audit line cites the wrong file.

`compatibility` correctly not selected — its 3rd/4th commands are
`check_merge_docs.py` / `test_check_merge_docs.py`, both workspace-root
readers (`:82-83`; `check_merge_docs.py:20`
`DEFAULT_WORKSPACE_ROOT = Path(__file__).resolve().parents[3]`). The comment
at `release.yml:172-177` states this correctly.

### 3.2 (b) PARTIAL cannot mask a real failure — **CONFIRMED**

`run_r4bg_aggregate_gates.py:151-166`:

```python
results = {selector: run_battery(selector) for selector in selected}
failed  = sorted(name for name, ok in results.items() if ok is False)   # :152
partial = sorted(name for name in results if ":" in name)               # :153
…
if failed:
    print(f"AGGREGATE: FAILED -- {', '.join(failed)}")
    return 1                                                            # :158-160
if partial:
    print(f"AGGREGATE: PARTIAL -- …")
    return 0                                                            # :161-164
```

`failed` is evaluated and returns **1** before `partial` returns 0. A failure
inside `byte-equivalence:2` itself sets `passed = False` (`:130`) → `False` →
`failed` → exit 1. Count-pin discipline is real: a command that exits 0 while
printing the wrong marker sets `missing` (`:125`) → `passed = False`
(`:127-132`). Fail-closed on a bad index (`IndexError` at `:115`, uncaught).
GitHub Actions runs `bash` steps as `-eo pipefail`, so a non-zero driver exit
fails the step. **The partitioned-selector path cannot mask a real failure.**

Observed live in my own run (verbatim tail):

```
=== byte-equivalence:2 -- byte-equivalence gate, both halves of O8 (rows 2.3a/2.3b, §12)
    ok    g23 adapted-v0, characterization and upgrade suites (97.7s, '119 passed')

=== unknown-field -- unknown-field gate (evidence row 2.4)
    ok    record wire unknown/archive/decode (0.2s, '75 passed')
    ok    exact unknown manifest per transition effect (1.9s, '1 passed')

=== R4b-G aggregate gate summary
    ok      byte-equivalence:2  (PARTIAL)
    ok      unknown-field
AGGREGATE: PARTIAL -- byte-equivalence:2 ran one command only;
reconcile the remaining commands across invocations before claiming a pass.

[exited with code 0]
```

The aggregate pass line **is** withheld and the gap **is** legible in the log,
exactly as `release.yml:117-119` claims. Three of the seven pinned counts
(`119 passed`, `75 passed`, `1 passed`) also reproduce on this macOS arm64 host.

### 3.3 (c) The call-graph exclusion on redundancy — **HOLDS**

The three call-graph commands (`run_r4bg_aggregate_gates.py:101-107`) versus
`release.yml:85-87`:

| driver | existing release.yml boundary step |
| --- | --- |
| `python scripts/checks/check_checked_artifact_boundaries.py` | identical (`:85`) |
| `python -m unittest scripts/checks/test_check_checked_artifact_boundaries.py` | `python scripts/checks/test_check_checked_artifact_boundaries.py` (`:86`) |
| `python -m unittest scripts/checks/test_release_boundary.py` | `python scripts/checks/test_release_boundary.py` (`:87`) |

**No coverage delta.** Both suites end `if __name__ == "__main__": unittest.main()`
(`test_check_checked_artifact_boundaries.py:1254`, `test_release_boundary.py:148`);
neither defines `load_tests`; neither manipulates `sys.path`; no `TestCase` is
conditionally defined. `unittest.main()` on `__main__` and
`python -m unittest <path>` therefore load tests from the same module and
collect the same set. The markers really are exit-code-equivalent
(`"checked-artifact boundary: ok"` / `"OK"` / `"OK"`), so the driver adds no
count discipline the plain step lacks — unlike the cargo batteries, whose
`"926 passed"`-class markers are strictly more than `cargo test --locked`'s
exit code. **The distinguishing principle is coherent and correctly applied.**

The measured cost (call-graph 1240.2s vs privacy 334.0s) is consistent with the
driver's own docstring (`run_r4bg_aggregate_gates.py:30-31`: "~27 min,
dominated by the call-graph compiler probes"). The re-add trigger is recorded
(`release.yml:132-134`).

One gap the comment does not state — see **[P3-4]**: the two guards are
independent and could diverge.

### 3.4 (d) Linux-only placement — **CONFIRMED, and an unmeasured Windows pin would be wrong to ship**

Both cited gates exist at exactly the cited lines:

```
573:#[cfg(all(test, windows))]        →  mod windows_tests
863:#[cfg(all(test, target_os = "linux"))]  →  mod linux_tests
```
(`src/checked_artifact/platform.rs`)

So the Windows leg both **adds** tests (`windows_tests`) and **loses** others
(`linux_tests`) relative to the settled-tree counts. The `#[cfg(unix)]` half of
the claim is also real — 42 occurrences under `src/checked_artifact/`,
including genuine test functions, e.g.:

```rust
#[cfg(unix)]
#[test]
fn symlink_parent_is_invalid_and_cannot_escape_the_root() {
```
(`src/checked_artifact/tests.rs:163-165`) — inside the `checked_artifact::`
partition the driver pins at `"400 passed"`. See **[P3-3b]** for the imprecise
cite.

**Ruling:** the count a Windows runner produces *is* a different number, it has
never been measured, and pinning it unmeasured is §17's lesson in a new dress —
the gate would fail on a count delta, not a regression. `cargo test --locked`
(`release.yml:89-90`, no `if:`) stays on the full matrix and remains the
Windows behavioural gate. **Linux-only is correct and correctly argued.**

### 3.5 (e) The `-f` tag-presence guard vs enumerating historical tags

The guard (`release.yml:156-159`) is the **right generalization**, and strictly
better than the enumeration pattern already in the file. Verified by
`git ls-tree` at each tag:

| tag | boundary scripts present | `run_r4bg_aggregate_gates.py` present |
| --- | --- | --- |
| v0.10.2 | no | no |
| v0.10.3 | no | no |
| v0.10.4 | **yes** | no |
| v0.10.5 | no | no |

So on a re-verify of v0.10.4 the boundary step runs (correctly) and the new
step skips (correctly); on v0.10.5 both skip. **No gap, and no case where the
new step's guard is wrong.** An enumeration would have needed two entries
(v0.10.4 and v0.10.5) that behave differently.

Two observations, both recorded below: the guard is **fail-open** ([P3-2]), and
the file's *existing* `v0.10.5` enumeration (`:82`) is already broken for tags
≤ v0.10.3 (pre-existing, not R3's).

---

## 4. G6 — versions

### 4.1 The convention, re-derived from history by me — **CONFIRMED**

`git show <tag>:Cargo.toml` in the r3 worktree:

```
v0.9.1  -> version = "0.9.1"   (4e2ba51)
v0.9.2  -> version = "0.9.2"   (ba87eea)
v0.10.0 -> version = "0.10.0"  (4d180c8)
v0.10.1 -> version = "0.10.1"  (40f1a21)
v0.10.2 -> version = "0.10.2"  (a638f8f)
v0.10.3 -> version = "0.10.3"  (277d235)
v0.10.4 -> version = "0.10.4"  (f6beb26)
v0.10.5 -> version = "0.10.5"  (4bcf13c)
```

**crate version == tag minus `v`, without exception.** `0.10.4 → 0.11.0` in
`Cargo.toml` + the matching `Cargo.lock` `gwz-core` entry conforms.

### 4.2 The v0.10.5 anomaly — **CONFIRMED, and it is a hotfix branch**

```
git merge-base --is-ancestor v0.10.4 v0.10.5   → NO
git merge-base --is-ancestor v0.10.5 HEAD      → NO
git merge-base --is-ancestor v0.10.4 HEAD      → YES
merge-base(v0.10.4, v0.10.5)                   → b0781b4
git branch -a --contains v0.10.5               → remotes/origin/hotfix/v0.10.5
```

v0.10.5 was cut from `origin/hotfix/v0.10.5` off `b0781b4`, never merged back —
which is precisely why main's `Cargo.toml` legitimately still said `0.10.4`.
The builder's "narrow-branch lineage, by design" is correct.

**One fact the lane owner needs that the package does not state:** the hotfix
carries **no code** main lacks. Only two commits are on v0.10.5 and not on
HEAD (`4bcf13c` version bump, `45f1107` lockfile refresh), and
`git diff --stat b0781b4 v0.10.5` is `Cargo.lock | 116 ++…` + `Cargo.toml | 2 +-`
and nothing else. So **v0.11.0 cut from main strictly supersedes v0.10.5** —
worth one sentence in the notes, because `0.11.0 > 0.10.5` alone does not prove
it. No collision, no version regression.

### 4.3 The member-repo proposal — **ACCURATE, and more accurate than the repos' own RELEASE.md**

All three claims verified against the v0.10.5 member release commits:

- **Release-branch operation at the tag.** gwz-cli `fbb04ca`
  ("chore(release): gwz-cli 0.10.5 (pins gwz-core v0.10.5)") is a **merge
  commit** and is **not** an ancestor of gwz-cli HEAD; same shape for gwz-py
  `0dc00ea`. `gwz-cli/RELEASE.md` states the mechanism directly: "`main` (dev)
  … **Do not cut release tags here.**" / "**Release tags are cut off
  `release`.**" ✔
- **path → git-tag re-point.** gwz-cli HEAD `Cargo.toml:20`
  `gwz-core = { path = "../gwz-core" }` → at `v0.10.5`
  `gwz-core = { git = "https://github.com/owebeeone/gwz-core", tag = "v0.10.5" }`.
  Identical for gwz-py (`Cargo.toml:15`). ✔
- **`0.2.0-dev` / `0.0.0` are dev placeholders, not separate version lines.**
  gwz-cli HEAD `version = "0.2.0-dev"` → at `v0.10.5` `version = "0.10.5"`;
  gwz-py HEAD `version = "0.0.0"` → at `v0.10.5` `version = "0.10.5"`. Both
  members are in **lockstep** with gwz-core at the tag. ✔
- **gwz-py needs no `pyproject.toml` edit** — corroborated more strongly than
  the builder stated: `pyproject.toml:3` `dynamic = ["version"]`, `:45`
  `tag_regex = "^v(?P<version>\\d+\\.\\d+\\.\\d+)$"`, `:46`
  `fallback_version = "0.0.0"`. The wheel version is **derived from the git
  tag**; `0.0.0` is literally the declared fallback. ✔

**[P3-5] — three stale claims in the `RELEASE.md` pair the R5 operator will
read.** The builder's proposal is right; the repos' docs are not:

1. `gwz-core/RELEASE.md:5` — "gwz-core … has **no release branch** … **Release
   tags are cut directly off `main`.**" The most recent tag, v0.10.5, was cut
   off `hotfix/v0.10.5`.
2. `gwz-core/RELEASE.md:42-43` — "(same gwz-core tag; **gwz-cli version is
   independent** — see gwz-cli/RELEASE.md)". It is not independent in practice;
   gwz-cli's tag carries `version = "0.10.5"`.
3. `gwz-cli/RELEASE.md` step 5 — "tag **off `release`**: `git tag
   gwz-cli-vA.B.C`". The actual gwz-cli tags are `v0.10.0 … v0.10.5`, with no
   `gwz-cli-` prefix anywhere.

Cure is one small edit to `gwz-core/RELEASE.md` (in R3's own repo) plus a
gwz-cli doc note; neither blocks the tag.

### 4.4 [P3-6] The bump lands in a content commit, dropping the other half of the convention the builder established

Every prior tag points at a standalone release commit:

```
v0.10.0: 4d180c8 chore(release): gwz-core 0.10.0
v0.10.1: 40f1a21 chore(release): gwz-core 0.10.1
v0.10.2: a638f8f chore(release): gwz-core 0.10.2
v0.10.3: 277d235 chore(release): gwz-core 0.10.3
v0.10.4: f6beb26 chore(release): gwz-core 0.10.4
v0.10.5: 4bcf13c chore(release): gwz-core 0.10.5
v0.9.2:  ba87eea chore(release): gwz-core 0.9.2
```

Pre-bumping in R3 means v0.11.0 will be the **first tag with no
`chore(release)` marker commit**, and main will self-report `0.11.0` for the
whole R1→R5 window while no v0.11.0 exists. This follows the plan (§R3.3 orders
the bump in R3), so it is not the builder's error — but it is half of the
convention the builder itself identified.

**Mechanically safe — I verified the R5 path will not trip:**

- `bump_cargo_version` (`scripts/release.py:242-260`) detects the no-op:
  `re.sub` changes nothing, `f'version = "0.11.0"' in text` is true →
  `log("Cargo.toml already at version 0.11.0"); return False`.
- `main` then takes the else branch (`:534-538`): `current == version` →
  `log("main already at version 0.11.0; no new commit needed")`, and tags the
  current HEAD. Documented behaviour (`release.py:14-16`).
- `assert_lock_current` (`:263-282`) will pass: I ran
  `cargo metadata --format-version 1 --locked` in the worktree with the
  hand-edited `Cargo.lock` → **exit 0**.

**Recommendation:** the lane owner records in the release record that v0.11.0
carries no `chore(release)` commit and why, so the next release's convention
scan does not read it as a defect.

> **Out-of-band, not an R3 finding but it fires at R5:**
> `scripts/release.py:527-530` hard-codes a
> `Co-Authored-By: Claude Opus 4.8 <noreply@anthropic.com>` trailer into the
> release commit message. Because of the pre-bump, v0.11.0's path never
> constructs that commit — but any future release that does will violate the
> standing no-AI-attribution rule at the source. Worth a separate fix.

---

## 5. G6 — the release-notes claims, checked against the tree

The lane owner holds the text; I judged each claim the notes **must** make.

| Claim | Ruling | Evidence |
| --- | --- | --- |
| Reads v0 **and** v1 | **SUPPORTED** | `InstalledMergeRecordVersions::PRODUCTION = { v0: true, v1: true }` (`record_wire/header.rs:30`) |
| Writes v1 **only** for `--no-ff` | **SUPPORTED, with the §1.4 caveat** | `version.rs:39` floor `V0`; `:71-77` `Ordinary→V0`, `NoFf→V1`. But an ordinary record **can migrate** to v1 (`store/mod.rs:254-262`) — the notes must say so or they mislead. |
| Floor `V0` owned by M5c | **SUPPORTED** | `version.rs:39` + activation record §2 (M5c minted, floor raise lands WITH it as one reviewed change) |
| Foreign-filter refusal semantics | **SUPPORTED, with [P3-8]** | `refuse_foreign_filtered_rewrites` (`src/git/gitbackend/recovery_support.rs:46-95`): allowlist `lfs` (`:75-77`); configured non-passthrough via `filter.<n>.clean`/`.process` (`:103-115`); deletions skipped (`:57-59`); non-UTF-8 name fails closed (`:70-72`); refusal message "refusing before any ref or worktree mutation" (`:117-123`). Called at `merge_recovery.rs:164`. **New since v0.10.4** — `git show v0.10.4:…/recovery_support.rs \| grep -c refuse_foreign_filtered_rewrites` → `0`; landed `9939b02` (2026-08-16), on main. |
| CRLF birth pins | **SUPPORTED, with a scope caveat** | `pin_creation_time_filter_neutralization` (`src/git/gitbackend/repository_support.rs:28-33`) sets `core.autocrlf=false` + `core.eol=lf`; `transport.rs:31` `checkout.disable_filters(true)`. **Caveat, from the code's own doc comment (`:19-22`):** "Attribute-driven smudge (`eol=crlf`, `ident`, foreign `filter=`) is deliberately untouched: it stays the frozen fail-closed residual on every OS." The notes must not imply CRLF exactness is universal. |
| Adopted-worktree residual | **SUPPORTED** | Same doc comment: pins are **creation-time only** (`:24-28`, the run-9 lesson). Adopted/legacy worktrees are unserved until §R6's renormalize. Per the checkpoint, R2 measured the `stash_save` filtered-reset smudge **on macOS** — so the notes must **not** frame this as Windows-only. |
| Older-reader behaviour, "v0.10.2–v0.10.5 decode v0 + typed record-unreadable for v1" | **[P2-3] WRONG** | See below. |

### [P2-3] The older-reader claim is wrong for v0.10.4/v0.10.5 and unevidenced for v0.10.3

- **Harness evidence exists for exactly two releases**: `FROZEN_R0_READERS =
  {rust-cli-v0.9.2, gwz-py-v0.9.2, rust-cli-v0.10.2, gwz-py-v0.10.2}`
  (`retained_reader_harness.py:36-41`), enforced at `:264`. There are **no
  retained readers** for v0.10.3, v0.10.4 or v0.10.5.
- **`record_unreadable` is v0.10.2's pinned classification only** — the
  rust-cli-v0.10.2 / gwz-py-v0.10.2 rows pin
  `{"schema":"gwz.merge-operation/v1","record_schema_version":1,"classification":"record_unreadable"}`.
- **v0.10.4/v0.10.5 do something materially different and better**:
  `PRODUCTION_R3 = { v0: true, v1: false }` with
  `HeaderClassificationError::Unsupported { header, required_wave }`
  (`v0.10.5:…/record_wire/header.rs:25-28`, `:47-54`) — the
  *unsupported_record_version* class the manifest schema names separately
  (`manifest.schema.json:185`).

**Cure:** either narrow the claim to what is pinned ("the retained v0.9.2 and
v0.10.2 readers, harness-pinned"), or state the range with its two distinct
behaviours. Do not flatten them — flattening under-sells v0.10.4/v0.10.5 and
asserts un-evidenced behaviour for v0.10.3.

### [P3-8] The new refusal is not a new error *code*

`foreign_filter_refusal` → `recovery_dirty` →
`ModelError::new(ErrorCode::DirtyMember, message)`
(`recovery_support.rs:14-16`, `:117-123`). The *behaviour* is new
(pre-mutation, names filter and path); the **typed code is the existing
`DirtyMember`**. Machine consumers keying on error codes see nothing new. The
notes should say "a new pre-mutation refusal, reported as `DirtyMember` with a
filter+path message", not "a new typed error class".

---

## 6. The `cf0d16d` companion — **[P1-1] the fix breaks a green gate, and its stated finding is false**

The prose fix itself is **correct and welcome**. `docs/commands/merge.md:374-380`
now says:

> `--no-ff` always creates a merge commit, even where a fast-forward is
> possible. It is the counterpart to `--ff-only`; supplying both together is
> rejected. A `--no-ff` start writes a v1 coordinated merge record and
> publishes a two-parent integration commit. Ordinary and custom-message starts
> continue to write v0 records.

Every clause is true of the tree (§5), the synopsis at `:7` gains `[--no-ff]`,
and `docs/CLI.md:1349` already carried `--no-ff` from `3000916`.

**But the commit message says:**

> "The docs checker's 147 assertions do not cover this prose - recorded as a
> gap for the R3 review."

**That is false.** `scripts/checks/merge_docs_manifest.json`, source
`merge_command` → `gwz-cli/docs/commands/merge.md`, carries **four required
assertions, and the fix deletes or changes all four**:

```
merge_command_ff_only_and_message_current   'gwz merge <source> [--dry-run] [--ff-only] [-m <message>]'
merge_command_deferred_heading              '## Features not yet available'
merge_command_no_ff_deferred                '`--no-ff` is not yet available.'
merge_command_no_ff_unsupported_is_typed    'It remains hidden and returns a typed unsupported error if submitted directly.'
```

Executed in the live workspace, verbatim:

```
$ python3 scripts/checks/check_merge_docs.py
merge document consistency: failed (4 finding(s))
gwz-cli/docs/commands/merge.md: [merge_command_deferred_heading] required statement is absent
gwz-cli/docs/commands/merge.md: [merge_command_ff_only_and_message_current] required statement is absent
gwz-cli/docs/commands/merge.md: [merge_command_no_ff_deferred] required statement is absent
gwz-cli/docs/commands/merge.md: [merge_command_no_ff_unsupported_is_typed] required statement is absent
EXIT=1
```

And it was **green immediately before**. Substituting gwz-cli `3000916`'s
`merge.md` through the checker's own `source_overrides` seam:

```
PRE-FIX merge.md (gwz-cli 3000916) substituted -> ok=True findings=0 sources=11 assertions=147
```

(147 = 70 per-source `required`+`forbidden`, plus 7 `global_forbidden` × 11
sources — the exact marker the driver pins.)

**Why this is P1, not cosmetic:**

1. It is a **release-lane gate**, and the R3 package's own new comment says so:
   `release.yml:172-181` — "`check_merge_docs.py` / `test_check_merge_docs.py`
   (L2-05) … Until it lands both remain local-gate, **executed in every landing
   gate set and at the release lane by hand**."
2. It breaks the **pinned marker** of the `compatibility` battery's 3rd command:
   `("merge-doc assertions", check("check_merge_docs.py"), "ok (11 sources, 147 assertions)")`
   (`run_r4bg_aggregate_gates.py:82`). The marker can no longer be produced.
3. It falsifies standing evidence: Safety §R2.3's `[P2-2]` ruling rested in part
   on "(iii) I executed `check_merge_docs.py` locally: *merge document
   consistency: ok (11 sources, 147 assertions)*".
4. The false coverage claim is **already propagated** into
   `CurrentProgramCheckpoint.md:1103-1109` ("uncovered by the docs checker's
   147 assertions (gap recorded)"). It needs correcting there too.

**Root cause, stated fairly:** the four assertions are themselves **stale
relative to A1** — they pin the *M5a* truth and should have been updated when
`3000916` unhid `--no-ff`. The A1 landing train missed them because the checker
is not CI-wired (that is L2-05's whole blocker). So this is the L2-05 gap
biting exactly as predicted — but the builder reported the opposite of what
happened.

**Cure — small, and in R3's own repo:** update the four `merge_command`
assertions in `gwz-core/scripts/checks/merge_docs_manifest.json` to the post-A1
truth, e.g.

- synopsis → `gwz merge <source> [--dry-run] [--ff-only] [--no-ff] [-m <message>]`
- heading → `## \`--no-ff\``
- replace the two "not yet available" requireds with requireds asserting the
  shipped semantics (`--no-ff always creates a merge commit…`, `writes a v1
  coordinated merge record`), and consider adding a **forbidden** for
  `is not yet available` so the class cannot regress.

Then re-run for the marker. Also correct the checkpoint sentence.

### [P3-9] `## --no-ff` now owns two paragraphs that are not about `--no-ff`

`merge.md:382-388` — "Merge also rejects unrelated operation policies supplied
explicitly: `--sync`, `--remote`, …" and the `gwz branch --merge` deprecation
note — are the residue of the deleted "Features not yet available" section and
were left under the new heading. The word "**also**" in "Merge **also** rejects"
chained off the old "`--no-ff` … returns a typed unsupported error"; with that
sentence gone it now reads as if those rejections are `--no-ff`-specific. Move
both paragraphs out from under the `--no-ff` heading (own section, or back to
the end of "Machine output").

### Rest of `merge.md` read for other stale merge-behaviour claims — CLEAN

I read all 388 lines. The remaining behaviour claims check out against the tree:
the participant-state list (`:127-131`), the start sequence (`:140-154`), the
merge-commit template (`:156-163`), "Requiring fast-forwards everywhere"
(`:168-180`, `--ff-only` only — not stale), abort/preserve/GC (`:252-353`), and
the machine-output `record_context` note (`:370-372`). No other "not yet
available"/"hidden"/"unsupported" claim about merge behaviour survives in the
file.

---

## 7. Gate executions in the worktree (item 6)

All run in the `r3-release` worktree with my own
`CARGO_TARGET_DIR=…/scratchpad/rev-target`. Verbatim tails.

**Checked-artifact boundary checker:**

```
$ python3 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

**Retained-reader harness suite** (`python3 -m unittest discover -s . -p 'test_retained_reader*.py'`):

```
----------------------------------------------------------------------
Ran 86 tests in 161.260s

OK

[exited with code 0]
```

**L2-04 validate / gate-ready:**

```
{"status": "valid", "tuple_count": 24}
{"status": "manifest-ready", "tuple_count": 24}
```

**Wired-selection spot check** (`byte-equivalence:2 unknown-field`) — full tail
quoted at §3.2; three of the seven pinned counts reproduce here
(`119 passed`, `75 passed`, `1 passed`), exit 0, PARTIAL printed, aggregate
pass line withheld.

**Cargo lock consistency** (the R5 `assert_lock_current` gate):
`cargo metadata --format-version 1 --locked` → exit 0.

**L2-05 merge-docs checker:** **RED** — see §6.

---

## 8. Findings register

| # | Sev | Finding | Where | Cure |
| --- | --- | --- | --- | --- |
| P1-1 | **P1** | `cf0d16d` breaks 4 required assertions of the L2-05 docs gate (green → 4 findings, exit 1); the "checker does not cover this prose" finding is false and is already propagated to the checkpoint | `merge_docs_manifest.json` `merge_command`; `release.yml:172-181`; `run_r4bg_aggregate_gates.py:82`; `CurrentProgramCheckpoint.md:1103-1109` | Update the 4 assertions to post-A1 truth (+ a `forbidden` for the regressed class); correct the checkpoint sentence |
| P2-1 | P2 | Generation description omits the ordinary → v1 whitelist migration; a v1 record can appear with no `--no-ff` start | `manifest.json` new entry; `store/mod.rs:254-262` | One clause in the description; same clause in the notes |
| P2-2 | P2 | Register under-covers: no generation for v0.10.3/4/5, and v0.10.4/5 differ materially from v0.10.2 | `manifest.json` `decode_generations`; `v0.10.5:record_wire/header.rs:25-28,47-54` | Constrain the entry's stated meaning, or add the missing generation |
| P2-3 | P2 | Notes' older-reader claim wrong for v0.10.4/v0.10.5, unevidenced for v0.10.3 | `retained_reader_harness.py:36-41`; `manifest.schema.json:185` | Narrow to the harness-pinned pair, or split the two behaviours |
| P3-1 | P3 | Privacy topology-audit line cites the driver's `ROOT` and an irrelevant `REGISTRY`; the probe has its own `ROOT` and builds `ROOT.parent/"target"/…` | `release.yml:110-112`; `test_v1_lifecycle_privacy_probe.py:61,112-114`; `run_r4bg_aggregate_gates.py:49,79` | Conclusion is right (a write into an existing dir, not a read of a missing one); cite boundary run `32754064482` at `8e40fa8`, which executed privacy for real on the single-repo runner, instead of "§17 already verified" |
| P3-2 | P3 | The `-f` guard is fail-open: remove/rename the driver and the release gate silently self-disables at exit 0 | `release.yml:156-159` | Keep the guard (it is the right generalization); add a `test_release_boundary.py` assertion pinning the selection string — that suite currently asserts nothing about `.github/workflows/*` |
| P3-3b | P3 | The "many `#[cfg(unix)]` test fns" cite reads as if they are in `platform.rs`; `grep -c 'cfg(unix)' src/checked_artifact/platform.rs` = **0** (they are elsewhere under `src/checked_artifact/`, e.g. `tests.rs:163`) | `release.yml:140-142` | One-word cite fix; the inference itself holds |
| P3-4 | P3 | The call-graph redundancy is void if the boundary step's `v0.10.5` skip ever fires while the driver is present; the two guards are independent. Also "all three suites carry `unittest.main()`" — the first is a checker, not a suite | `release.yml:82,120-134` | Note the coupling beside the re-add trigger |
| P3-5 | P3 | Three stale claims in the `RELEASE.md` pair the R5 operator will read (no-release-branch; "gwz-cli version is independent"; `gwz-cli-vA.B.C` tag naming) | `gwz-core/RELEASE.md:5,42-43`; `gwz-cli/RELEASE.md` step 5 | Small doc edits; not tag-blocking |
| P3-6 | P3 | v0.11.0 will be the first tag with no standalone `chore(release)` commit; main self-reports 0.11.0 for the whole pre-tag window | `Cargo.toml`; `release.py:242-260,534-538` | Mechanically safe (verified); record it in the release record |
| P3-7 | P3 | The digest re-pin *rule* as stated is broader than the code supports — `artifacts`/`runtime_artifacts` are embedded but never re-derived at validation | `retained_reader_evidence.py:242-262` vs `:319-337` | Record the narrower rule in the retained-readers `README.md` |
| P3-8 | P3 | The foreign-filter refusal is not a new error **code** — it is `ErrorCode::DirtyMember` | `recovery_support.rs:14-16,117-123` | Notes wording |
| P3-9 | P3 | `## --no-ff` now owns two paragraphs unrelated to `--no-ff`; "Merge **also** rejects" lost its antecedent | `merge.md:382-388` | Move both paragraphs out |

Pre-existing, **not** R3's, recorded in passing: `release.yml:82`'s `v0.10.5`
tag enumeration makes the boundary step **fail** on any tag ≤ v0.10.3, where
the three scripts do not exist; and `release.py:527-530` hard-codes an AI
co-author trailer into the release commit message.

---

## 9. What the package got right (recorded, because the adversarial frame hides it)

- The `byte-equivalence:2` discovery is the whole ballgame for G4. Wiring the
  battery whole would have reproduced `32749441874` on the release lane, on
  both OS legs, at the tag. The builder found it by reading the driver's
  command list rather than trusting the battery name.
- The evidence-digest re-pin was flagged **by the builder** as the one possible
  rubber stamp. It is sound, and the instinct to flag it was correct.
- The tuple-count reasoning is right *and* the in-comment justification is the
  right artifact — it survives the next reader who wonders why a generation was
  added without moving a pin.
- The Linux-only argument is evidence-based rather than house-style, and
  correctly identifies that an unmeasured count pin is §17's lesson wearing a
  new dress.
- The member-repo proposal corrects the plan **and** both repos' `RELEASE.md`
  files, from the actual v0.10.5 release commits.
- The `merge.md` prose the builder wrote is accurate to the tree in every
  clause. The defect is in the *coverage claim* and the untouched manifest, not
  the prose.

---

## 10. Conditions for GO

1. **[P1-1]** Update the four `merge_command` assertions in
   `gwz-core/scripts/checks/merge_docs_manifest.json`; re-run
   `check_merge_docs.py` to `ok (11 sources, 147 assertions)`; correct the
   checkpoint's "uncovered by the docs checker" sentence.
2. **[P2-1]** Add the migration clause to the generation description.
3. **[P2-2]** Constrain the register entry's stated meaning (or add the
   v0.10.4/v0.10.5 generation).
4. **[P2-3]** Narrow or split the notes' older-reader claim.

P3s are lane-owner discretion and none block the tag. With 1–4 closed, **G3,
G4 and G6 are satisfied for v0.11.0.**
