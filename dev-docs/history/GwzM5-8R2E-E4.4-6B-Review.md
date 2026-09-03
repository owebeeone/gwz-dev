# GwzM5-8 R2-E E4.4-6-B — single-axis peer-blind review (Opus, interior tier)

Candidate: ONE commit `b99bfb72e14f4b369e8b2cf94f4971b3af45c193` on
`e4/e4-4-6b-capfree-pins`, base `60072a7` (E4.3-B's delivery tip). Reviewed AS
DELIVERED on its base. Main has since moved to `0dae0d5`; the rebase is the lane
owner's and its collisions are enumerated in §10.

Authority: `GwzM5-8R2E-CapabilityFreeAmendment.md` (ADOPTED 2026-09-02) §1/§3/§4/§5/§6/§7;
`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`; `GwzM5-8R2E-E4.3B-Review.md`
([P3-3] and its round-2 disposition); E4.3-B's landed shape at `60072a7`.

Worktree `scratchpad/e4-pins-wt` (probed, reverted, left byte-clean —
`git status --short` empty at exit). Own target `scratchpad/e4-pins-target`.
Checkers via `/opt/homebrew/bin/python3.13` under a symlink farm
(`farm/gwz-core` a real directory of symlinks with `scripts/checks/` copied so
`Path(__file__).resolve().parents[3]` lands on the farm root; `dev-docs` and
`gwz-cli` symlinked from the real workspace).

---

## VERDICT: **NO-GO**

1 P1, 2 P2, 10 P3. **Escalation fires.** Round 1 of the two-round cap.

The package's arithmetic is clean — I recounted all nineteen rows with an
independent masker and an independent (strictly stricter) `#[cfg(test)]` drop
and got **zero mismatches**; all six digests recompute; every fail-closed
direction I drove fires with an amendment-naming message; the three partition
counts are MEASURED on my own target and match. What blocks the landing is not
arithmetic:

- **[P1-1]** The SHARED MASKER — §7's NAMED OBLIGATION, the one item this
  package exists to discharge for E4.3-B — **is met in letter and defeated in
  fact**. It cures the one shape it was chartered for (probe **M-g** fires) and
  introduces or leaves **four quiet-failure shapes, three of them LIVE in the
  tree today**. The blind region it creates is **178 blinded lines across 3
  production files** where the string-unaware strip it replaced blinded **5**,
  and it is UNBOUNDED where the old one was line-local. Driven: probe
  **M-live** declares the record-root door in a production source and the
  re-pointed E4.3-B tripwire reports `2 passed`. No anti-vacuity belt in P-2
  can see it — the byte floors are length-invariant under blanking.
- **[P2-1]** A **twentieth carved row is missing**:
  `workspace_ops/merge/store/archived.rs::archive` is a LIVE `:275` raw durable
  writer (the `DONE_DIR` "when missing" bootstrap plus `rename_durable`,
  `remove_file` and two `sync_dir`) reached from ordinary v0 merge finalization
  and from BOTH abort forms. It is in no inventory, in no digest, in no negative
  scan and carries no dated annotation. Converting it — the exact act the ruling
  forbids — fires nothing.
- **[P2-2]** §6's release-train obligation ("the pins package's delivery
  restates it verbatim for the train") is **undischarged**: the corrected abort
  sentence appears nowhere in the delivery, so the E4.1 release-notes over-claim
  has neither an on-disk home nor a carrier.

All three are small, local and bounded. Everything else in the package is sound.

---

## 1. THE MASKER, adversarially — [P1-1]

### 1.1 The subject

`src/workspace_ops/merge/v1_lifecycle/tests/mod.rs`, `masked_code`. Its doc
comment claims:

> `check_checked_artifact_boundaries.py::mask_non_code`'s idiom in Rust — only
> `"` toggles, which covers raw strings (`r#"…"#`), and `//` cannot occur inside
> a char literal.

Both halves of the parenthetical are wrong in the direction that matters.
`quoted` is declared OUTSIDE the per-line loop, so ANY unbalanced `"` — from a
char literal, a byte-char literal, a raw string with an interior quote, a raw
string ending in a backslash, or a block comment — desynchronises the toggle for
**the rest of the file, or until the next stray `"`**, and every character in
that span is blanked. Blanked code cannot contain a door. For a pin whose
property is ABSENCE, that is the quiet direction.

The claim "`//` cannot occur inside a char literal" is true and irrelevant: the
hazard in a char literal is `'"'`, not `'/'`.

### 1.2 Probe transcripts

All probes appended to a PURE carved file (`src/workspace_ops/merge/store/gc.rs`)
unless stated, then reverted. `T` = `cargo test --lib capability_free`.

**Probe M-0 — the control.** Bare door, no desync:

```rust
#[allow(dead_code, unused_imports)]
mod probe_control { use crate::checked_artifact; }
```
```
test ...the_pure_carved_capability_free_files_name_no_checked_boundary_door ... FAILED
panicked at capability_free_exception.rs:119:13:
`workspace_ops/merge/store/gc.rs` names the checked boundary door vocabulary `checked_artifact` …
```
FIRES. Good.

**Probe M-a — CHAR LITERAL CONTAINING A DOUBLE QUOTE. THE EVASION SUCCEEDS.**

```rust
#[allow(dead_code, unused_imports)]
mod probe_a {
    pub(crate) fn quoting(character: char) -> bool { character != '"' }
    use crate::checked_artifact;
}
```
```
test ...the_mixed_files_carved_arms_name_no_checked_boundary_door ... ok
test ...the_pure_carved_capability_free_files_name_no_checked_boundary_door ... ok
test result: ok. 2 passed; 0 failed; …
```
The door is literally present in a carved production source and the tripwire is
green. **`character != '"'` is not a contrived shape — it is the exact idiom
already live at `git/gitbackend/merge_support.rs:177` and `model/mod.rs:555`.**

**Probe M-c — RAW STRING WITH AN INTERIOR QUOTE. THE EVASION SUCCEEDS.**

```rust
#[allow(dead_code, unused_imports)]
mod probe_c {
    pub(crate) const SAMPLE: &str = r#"a"b"#;
    use crate::checked_artifact;
}
```
```
test result: ok. 2 passed; 0 failed; …
```
Directly contradicts the doc comment's "only `"` toggles, which covers raw
strings (`r#"…"#`)". It covers raw strings whose contents contain no `"` — which
is the case raw strings exist to avoid.

**Probe M-g — THE REVIEWER'S ORIGINAL EVASION (E4.3-B [P3-3] probe E5). FIRES.**

```rust
#[allow(dead_code, unused_imports)]
mod probe_g {
    pub(crate) fn evade() -> &'static str { "see https://gwz.invalid/x" }
    use crate::checked_artifact;
}
```
```
test ...the_pure_carved_capability_free_files_name_no_checked_boundary_door ... FAILED
panicked at capability_free_exception.rs:119:13: … names the checked boundary door vocabulary `checked_artifact` …
```
**The chartered shape IS cured.** The obligation's letter is discharged.

**Probes M-b / M-d / M-e / M-f / M-j — the safe half.** Screened with a
byte-faithful Python emulation of `masked_code` (validated against M-a, M-c, M-g
and against the three live tree desyncs it predicted exactly):

| shape | source | result |
| --- | --- | --- |
| (b) byte string `b"a/b"` | `pub const S: &[u8] = b"a/b";` | **FIRES** (safe) |
| (d) escaped quote | `pub const S: &str = "\"";` | **FIRES** (safe — escapes consumed correctly) |
| (e) `//` inside a multi-line raw string | `r#"\n// not a comment\n"#` | **FIRES** (safe — the `//` is correctly NOT stripped, because `quoted` persists across lines) |
| (f) door inside `/* … */` | `/* legacy: use crate::checked_artifact; */` | **FIRES** — the LOUD direction, acceptable and unchanged from the house precedent |
| (j) nested block comment | `/* outer /* inner */ */` | **FIRES** (safe) |

**Probes M-b2 / M-h / M-i / M-k — three further QUIET shapes:**

| shape | source | result |
| --- | --- | --- |
| (b2) byte-char literal | `pub const C: u8 = b'"';` | **QUIET** |
| (h) raw string ending in a backslash | `pub const P: &str = r"C:\";` | **QUIET** — the closing `"` is eaten by the escape branch, which raw strings do not have |
| (i) block comment with an odd quote | `/* don't use " here */` | **QUIET** — block comments are not handled at all |
| (k) char comparison | `pub fn f<'a>(c: char) -> bool { c == '"' }` | **QUIET** — the live tree shape |

### 1.3 The blind region is LIVE, and 35× larger than the one it replaced

I ran the emulated masker over all 411 files in the record-root tripwire's walk
and diffed it against the checker's own `mask_non_code` (the strong masker), and
did the same for the pre-package `code()`:

```
production files in the walk: 411
  NEW masked_code: 178 blinded lines across 3 files
       ('git/gitbackend/merge_support.rs', 144 whole + 5 partial)
       ('workspace_ops/workspace_bootstrap/claude_settings.rs', 21 whole + 4 partial)
       ('model/mod.rs', 3 whole + 1 partial)
  OLD code():        5 blinded lines across 3 files
       ('operation/operation_runtime.rs', 3 partial)
       ('status/workspace_path.rs', 1 partial)
       ('git/git_host.rs', 1 partial)
```

Exact ranges, measured:

```
git/gitbackend/merge_support.rs (393 lines): 144 lines blinded —
  :178-:180, :182-:183, :186, :188-:190, :237-:238, :243-:246, :248-:262,
  :264-:267, :271-:274, :276, :278-:294, :296-:301, :303-:307, :309-:317,
  :319-:329, :331-:340, :342-:344, :346-:354, :356-:369, :371-:376,
  :378-:386, :388-:393
model/mod.rs (559 lines): :556, :558-:559
workspace_ops/workspace_bootstrap/claude_settings.rs (512 lines):
  :262-:263, :278, :280-:283, :285-:287, :289-:290, :293-:296, :298-:299, :510-:512
```

Desync triggers, all pre-existing production idiom:

```
git/gitbackend/merge_support.rs:177  .all(|character| !character.is_control() && character != '"' && …)
git/gitbackend/merge_support.rs:236  output.push('"');
model/mod.rs:555                     matches!(ch, ',' | ':' | ';' | '<' | '>' | '"' | '\\' | '\'')
workspace_bootstrap/claude_settings.rs:261/:277  out.push('"');
workspace_bootstrap/claude_settings.rs:508       emitted.contains(r#""s": "a\"b\\c\nd\u0007""#)
```

The old blind was **line-local** — the tail of one line after a `//` that sat
inside a string. The new blind is **unbounded**: from the stray quote to the
next stray quote, or to EOF.

### 1.4 Driven on the live tree — probe M-live

The whole point of the re-point is that E4.3-B's tripwire now sees what it could
not. It sees less. Appended to `src/git/gitbackend/merge_support.rs`, whose
desync at `:236` runs to EOF:

```rust
/// PROBE: live-tree blind region (desync begins at `:236`).
#[allow(dead_code)]
pub(crate) fn rewrite_merge_store_record() {}
```
```
test ...record_root_exception::the_record_root_rewrite_publishes_by_atomic_rename_and_creates_no_parent ... ok
test ...record_root_exception::the_checked_rewrite_door_is_absent_from_production_sources ... ok
test result: ok. 2 passed; 0 failed; …
```

The rejected E4.3 door is DECLARED in a production source and the tripwire whose
stated property is its absence is green — the same failure E4.3-B's reviewer
drove at probe E5, reproduced through the cure that was supposed to close it.

### 1.5 No belt sees it

- `SOURCE_FLOOR` (400 bytes) and `carved_arm.len() >= SOURCE_FLOOR` are measured
  on the MASKED string. Blanking writes `' '` in place of every masked character
  and preserves every newline, so **the length is invariant under the desync**.
  The floors cannot detect it, by construction. They detect a truncated read,
  which is a different failure.
- `PRODUCTION_FILE_FLOOR` counts files, not content.
- The positive controls (`LANDED_DOOR` in exactly two files; `checked_artifact`
  present in `artifact_facts.rs`; a door somewhere in each mixed file) are all
  satisfied in files with no desync, so they stay green through the blinding.

### 1.6 Why this is P1 and not P3

E4.3-B's reviewer rated the ORIGINAL string-unaware strip P3 on an explicit
ground: three independent belts (P-1's exact counts, the `v1_lifecycle/mod.rs`
tree digest, `entry.rs`'s byte pin) hold the same property, so the tripwire's
marginal contribution was only the STATED property. That reasoning does not
transfer here, for three reasons:

1. **It is a regression, not an inherited residual.** This commit CREATES the
   178-line unbounded blind region. Before it, the blind was 5 line-tails.
2. **The residual is now recorded as CURED.** The commit message says the strip
   "hid a door on any line whose earlier `//` sat inside a literal" — past
   tense — and main's `code()` comment (which the rebase deletes) is the only
   place the residual is named. After this lands, a reader has no notice that a
   quiet blind region exists at all, and it is larger.
3. **P-2's own rows have no belts.** The seventeen PURE files and the two mixed
   arms are pinned by NOTHING except these two tests — sixteen of the nineteen
   carved files are unpinned by any digest, as the checker's own comment states.
   For those rows the negative scan IS the property. None of the nineteen
   desyncs today (verified), so P-2 is sound as delivered — but it is one
   `'"'` away from silence, in files that already write `.git/info/exclude`
   candidates and lock/manifest artifacts.

**Remedy (small).** Port `mask_non_code` faithfully: `//` to EOL, `/* … */`
nested, `r#*"…"#*` by hash matching, `"…"` with escapes, `'c'`/`'\c'` with the
lifetime disambiguation the Python already has. It is ~40 lines and it is the
component E4.3-B's reviewer said belonged here. **At minimum**, and cheaply:
make the failure LOUD — `panic!` if `quoted` is still true at end of input (that
alone catches M-a, M-b2, M-c, M-h, M-i, M-k and all three live tree files) —
and handle char literals, which is the only shape live in the tree. A quiet
masker under an absence pin is the one trade this program has repeatedly refused.

---

## 2. THE `#[cfg(test)]`-MODULE DROP — sound, fail-loud where imprecise

```python
TEST_MODULE = re.compile(
    r"(?m)^#\[cfg\(test\)\]\s*\n(?:#\[[^\n]*\]\s*\n)*(?:pub(?:\([^)]*\))?\s+)?mod\s+\w+\s*\{"
)
```

Read: column-zero `#[cfg(test)]`, then any number of further attribute lines,
then an optional `pub`/`pub(…)`, then `mod NAME {`. The brace walk starts at
depth 1 after the `{` and runs on ALREADY-MASKED text, so no brace inside a
string, char literal or comment can unbalance it (`mask_non_code`'s `'` branch
masks `'{'`; its raw-string branch masks `r#"{"#`). `if match.start() < index:
continue` correctly skips matches inside an already-dropped span.

**A production item can be dropped only if it is preceded by `#[cfg(test)]`,
which makes it test-only by definition.** There is no path by which production
code is silently removed. Probed:

```
2c  #[cfg(test)] mod probe { … create_dir_all … }         -> DROPPED   (checker green)
2a  #[cfg(not(test))] mod probe { … create_dir_all … }    -> COUNTED   (fires)
      "capability-free raw writer inventory moved, workspace_ops/merge/store/gc.rs
       (:275 the LIVE GC deletion writer, WorkspaceMutatorLock):
       expected={'sync_dir': 1, 'remove_file': 1}
       actual={'sync_dir': 1, 'create_dir_all': 1, 'remove_file': 1}.
       A NEW RAW WRITER IS NOT BLESSED: …"
2b  #[cfg(all(test, unix))] mod probe { … }               -> COUNTED   (fires)
2d  mod outer { #[cfg(test)] mod inner { … } }            -> COUNTED   (fires)
```

**2a is the mandated case and it is correct**: `#[cfg(not(test))]` does not
match `^#\[cfg\(test\)\]` and is counted.

2b and 2d are the LOUD direction: an over-count, which the exact-count pin
raises immediately. They make the commit message's "the pin measures the
PRODUCTION surface" approximate rather than exact. I verified that **no carved
file today carries an `#[cfg(all(test, …))]` or an indented `#[cfg(test)] mod`
containing a vocabulary token** — my independent recount (§3) used a STRICTER
drop that handles both, and agreed with the checker on all nineteen rows.
**[P3-3]**, documentation only.

---

## 3. THE 19 ROWS — recounted independently, ZERO mismatches; one row MISSING

### 3.1 The recount

I did not use the checker's code. I wrote my own Rust masker (`//`, nested
`/* */`, `r#*"…"#*`, `b"…"`, `"…"` with escapes, `'c'`/`b'c'` with lifetime
disambiguation), my own test-module drop (stricter: also drops
`#[cfg(all(test, …))]` and indented modules), and my own `\btoken\b` counter,
then applied the same "skip O13's classes on O13's files" rule:

```
ok  stash/mod.rs                                  {'write_atomic': 2, 'write_bundle': 1}
ok  workspace_ops/handle_branch.rs                 {'write_lock': 1, 'sync_workspace_boundary': 1}
ok  workspace_ops/handle_commit.rs                 {'create_dir_all': 1, 'write_marker': 1, 'write_lock': 1, 'sync_workspace_boundary': 2}
ok  workspace_ops/handle_create_repo.rs            {'write_manifest_and_lock': 4, 'sync_workspace_boundary': 4}
ok  workspace_ops/handle_init_from_sources.rs      {'write_manifest_and_lock': 1, 'sync_workspace_boundary': 1}
ok  workspace_ops/handle_materialize.rs            {'write_lock': 3, 'sync_workspace_boundary': 3}
ok  workspace_ops/handle_repo_lifecycle.rs         {'write_manifest_and_lock': 3, 'sync_workspace_boundary': 3}
ok  workspace_ops/handle_stage.rs                  {'ensure_workspace_exclude': 1}
ok  workspace_ops/handle_stash/commands.rs         {'remove_file': 1, 'write_bundle': 6}
ok  workspace_ops/merge/abort/evidence.rs          {'remove_file': 1, 'write_atomic': 1, 'publish_workspace_exclude_candidate': 1}
ok  workspace_ops/merge/abort/preflight.rs         {'write_atomic': 2}
ok  workspace_ops/merge/finalize.rs                {'write_atomic': 2, 'publish_workspace_exclude_candidate': 2}
ok  workspace_ops/merge/preserve/artifacts.rs      {'remove_file': 1, 'write_atomic': 3, 'write_bundle': 1, 'publish_workspace_exclude_candidate': 1}
ok  workspace_ops/merge/store/gc.rs                {'sync_dir': 1, 'remove_file': 1}
ok  workspace_ops/merge/store/retention.rs         {'sync_dir': 1, 'remove_file': 1}
ok  workspace_ops/merge/v1_lifecycle/archive.rs    {'remove_file': 1}
ok  workspace_ops/merge/v1_lifecycle/store/archive.rs {'create_dir_all': 1, 'remove_file': 2}
ok  workspace_ops/pull_head_member_preflight.rs    {'write_lock': 3, 'sync_workspace_boundary': 2}
ok  workspace_ops/sync_workspace_boundary.rs       {'write_atomic': 2, 'publish_workspace_exclude_candidate': 1, 'sync_workspace_boundary': 1, 'ensure_workspace_exclude': 2}
mismatches: 0
```

**19/19 correct.** The amendment's post-adoption correction (1) is confirmed:
`store/archive.rs` has THREE raw `std::fs` mutations
(`create_dir_all` 1 + `remove_file` 2), not four.

### 3.2 §1's cited sites, covered

Every writer cited in amendment §1's row table maps to exactly one inventory row
or to O13, with no double-count and no orphan: `:275` → `store/archive.rs`,
`store/gc.rs`, `store/retention.rs`, `v1_lifecycle/archive.rs`; `:276` →
`preserve/artifacts.rs`, `stash/mod.rs` (+ the builder's `handle_stash/commands.rs`);
`:277` → `handle_commit.rs`, `finalize.rs`, `abort/evidence.rs`,
`preserve/artifacts.rs`; `:278` → `finalize.rs`, `handle_commit.rs`,
`abort/evidence.rs`, `abort/preflight.rs`; `:279` → `sync_workspace_boundary.rs`,
`handle_create_repo.rs`, `handle_init_from_sources.rs`, `finalize.rs`,
`abort/evidence.rs`, `preserve/artifacts.rs`, `handle_commit.rs`,
`handle_materialize.rs`, `handle_stage.rs`, `handle_branch.rs`,
`handle_repo_lifecycle.rs`, `pull_head_member_preflight.rs`. The three ALREADY-
CONVERTED writers §1 names (`reverse/execute/preservation.rs:74`,
`root/abort.rs:380`, `finalization/execute.rs:45/:48/:51`) are correctly ABSENT.
`store/rewrite.rs` is correctly absent (RR's carve, O13's row).

### 3.3 The builder's three additions — all real, all correctly guarded

- **`workspace_ops/handle_stash/commands.rs`**: six `stash::write_bundle`
  (`:68,:86,:93,:122,:293,:302`) + one `fs::remove_file` (`:300`), all reached
  under `guarded_workspace_root(OpenMergeCommand::StashMutate)`
  (`handle_stash.rs:43`, `:58`). No `acquire_activated`, no `CheckedArtifact`
  reference. **Real listed-operation writer** (`stash`, the mutation guard's
  broad reading). ✓
- **The third `write_lock` in the pull preflight**: at
  `pull_head_member_preflight.rs:458` (NOT `:457` as the amendment's bracket (3)
  says — **[P3-7]**), in `rewrite_conflicted_root_lock…`, under the file's
  `guarded_workspace_root` at `:45`. The row's `{'write_lock': 3,
  'sync_workspace_boundary': 2}` is exact: `write_lock` at `:186,:458,:492`,
  `sync_workspace_boundary` at `:187,:493` (`:458`'s path stages explicitly
  instead, as its own comment records). ✓
- **The five `handle_*.rs` manifest/lock writers**: `handle_create_repo.rs`
  (bare `WorkspaceMutatorLock::acquire` at `:35` for workspace-create;
  `guarded_workspace_root(RepoMutate)` at `:91,:245,:459`),
  `handle_materialize.rs` (`Capture` at `:103`, `Materialize` at `:157`),
  `handle_repo_lifecycle.rs` (`RepoMutate` at `:49,:193,:269`),
  `handle_branch.rs` (`BranchMutate` at `:33`), `handle_init_from_sources.rs`
  (bare `WorkspaceMutatorLock::acquire` at `:91`). **None is an
  activated-lease arm** — `acquire_activated` appears in none of the nineteen,
  which P-2's own scan re-asserts as a property. ✓

### 3.4 [P2-1] — THE MISSING TWENTIETH ROW

I swept every production file in the crate for the thirteen vocabulary tokens
(own masker, own test-drop) and classified the thirteen files that name one and
are neither an inventory row nor an O13 row. Twelve are correctly out of scope:
the primitive layer (`durable_fs.rs`, `artifact/mod.rs`), the boundary's own
implementation (`checked_artifact/{cleanup,transition}.rs`,
`capability/pre_catalog/provider/admission_mutation.rs`), the v0 RECORD store
(row `:274`, RR's family: `merge/store/mod.rs`, `merge/store/atomic_upgrade.rs`),
the three convertible forward arms (`v1_lifecycle/finalization/execute.rs`),
non-`:275`-`:279` artifacts (`artifact/conf_integrity.rs`,
`workspace_bootstrap/claude_settings.rs`, `git/gitbackend/preservation_root/parent.rs`
— which uses its own local `sync_dir` on `Dir` handles, not `durable_fs`'s), and
`workspace_ops/mod.rs` (a `mod` + re-export, no writer).

**The thirteenth is not.** `src/workspace_ops/merge/store/archived.rs`:

```rust
pub(super) fn archive(root: &Path, merge_id: &str) -> ModelResult<()> {
    …
    let (source_raw, record) = read_record(&source, RecordLocation::Open)?;
    ensure_terminal_for_archive(&record)?;
    fs::create_dir_all(root.join(DONE_DIR)).map_err(io_error)?;     // :39
    …
        fs::remove_file(&source).map_err(io_error)?;                // :47
    } else {
        rename_durable(&source, &destination, false).map_err(io_error)?;  // :49
    }
    sync_dir(&root.join(MERGE_DIR)).map_err(io_error)?;             // :51
    sync_dir(&root.join(DONE_DIR)).map_err(io_error)?;              // :52
```

This is **row `:275`'s "`MergeArchive` when missing" bootstrap on the v0 path**,
and it satisfies cell 3's frozen ordering the same way `store/archive.rs:38-45`
does (terminal/source validation BEFORE the bootstrap; early return for
both-absent and destination-only). Its production reach, traced:

```
archived::archive                       (store/archived.rs:23)
  ← FileMergeStore::archive             (store/mod.rs:152-153)
  ← persistence::archive_merge_record   (store/persistence.rs:32-42)
  ← finalize_dispatch.rs:34             ORDINARY v0 MERGE finalization
  ← finalize_support.rs:99              ORDINARY v0 MERGE
  ← abort/mod.rs:111, :189, :218        v0 ABORT, both forms
```

Ordinary merge and abort are the two operations E0.2 §5.2 names first. This
writer is squarely inside the amendment's stated carve — "every §10 row `:275`-
`:279` durable writer that executes from … an ordinary merge … merge abort
(either form, either record version)" — and it is enforced by **nothing**:

- not in `CAPABILITY_FREE_RAW_WRITER_INVENTORY` (19 rows);
- not in O13 — it lives outside `v1_lifecycle/`, exactly as the amendment's own
  correction notes for its two siblings `store/gc.rs` and `store/retention.rs`,
  which WERE listed;
- not in `PROTECTED_SOURCE_DIGESTS` and covered by no tree root (measured: the
  seven tree roots are all under `checked_artifact/` or `v1_lifecycle/`);
- not in P-2's `PURE_CARVED_FILES`;
- carries no `CAPABILITY-FREE EXCEPTION` annotation.

It appears NOWHERE in the amendment, in `GwzM5-8R2E-E4.4-CharterPrep.md`, or in
either axis of the amendment's own dual (grepped: zero hits for `archived.rs` /
`store/archived`). So the gap is inherited from §1 — but this package's charter
was to enumerate "in the tree, not from the amendment's tables", and it found
three §1 omissions by doing exactly that. This is the fourth.

**Consequence:** a future step converting `archived.rs::archive` onto the
boundary would place ordinary v0 merge AND both abort forms on the
durable-identity probe — the precise act the ruling forbids — and this package
would report green. That is the failure mode P-1 exists to prevent, at the two
operations the ruling names first. **[P2-1]** — add the twentieth row
(`{'create_dir_all': 1, 'remove_file': 1, 'rename_durable': 1, 'sync_dir': 2}`,
measured), its `:275` annotation, its `PURE_CARVED_FILES` entry, the new key-set
digest, and one sentence in the amendment's post-adoption bracket.

*(Related, not blocking: **[P3-5]** — the inventory counts references to the
`artifact::write_atomic` FAMILY'S NAMES at their call sites. Converting the
family's own implementation in `artifact/mod.rs` would convert every carved
`:277`/`:278`/`:279` caller at a stroke while moving no count in any of the
nineteen rows and naming no door in any scanned file: nothing fires.
`artifact/mod.rs` is neither flat- nor tree-pinned. This is a chosen scope limit
of §3's mechanism, not a defect — but it is stated nowhere, and it is the
cheapest possible way for a future step to defeat the entire pin. One sentence
in the checker comment, or a flat digest on `artifact/mod.rs`, closes it.)*

---

## 4. THE TWO-MAP DISJOINTNESS AND THE KEY-SET DIGEST — all directions drive

**Probe 4a — declare an O13 class on an O13 row.** Added `"sync_dir": 2` to the
`v1_lifecycle/archive.rs` inventory row:

```
Traceback (most recent call last):
  File ".../check_checked_artifact_boundaries.py", line 483, in <module>
    assert not any(
AssertionError: a carved site would be measured twice: O13 already counts that file's durable_fs class
```
FIRES. Note it is a bare `AssertionError` traceback rather than a named finding —
see [P3-2].

Belt-and-braces confirmed by reading: even if the module-scope assert were
stripped, the measurement loop's `if not (o13 and token in
V1_LIFECYCLE_RAW_DURABLE_WRITERS)` guard means the declared `sync_dir` could
never be produced, so the row would mismatch and the CHECK would fire. Two
independent belts on the same property.

**Probe 4b — the key-set digest, all three directions:**

```
DELETE  (drop the store/retention.rs row) -> AssertionError: the capability-free carved SET moved --
        a row added, DELETED or swapped. It is the amendment's, not a checker edit:
        revise GwzM5-8R2E-CapabilityFreeAmendment.md §3
ADD     (insert artifact/conf_integrity.rs)            -> same, FIRES
SWAP    (retention.rs -> archived.rs, counts kept)      -> same, FIRES
```

**The dropped namer closure.** The checker's parenthetical:

> (A crate-wide namer closure over the leaf-publication spellings would also
> catch a new raw writer in a file NOT listed here: drafted, dropped for the line
> budget; allowlist `artifact/mod.rs`, `finalization/execute.rs`, `mod.rs`.)

The "row deleted" duty is **fully covered** by the key-set digest (probe 4b-i)
plus the "carved file is GONE" branch (probe P1-b). The comment does NOT claim
the digest is strictly stronger — it says the closure "would ALSO catch" the
unlisted-file case, which is honest and correct: neither the digest nor the
per-file counts see a new raw writer in an unlisted file. **Nothing mandated was
lost**: §3 mandates per-carved-file counts fail-closed both ways plus a per-file
negative scan; the crate-wide closure was above mandate. **[P3-4]**: the
parenthetical is not reconstructable as written — with the stated vocabulary and
that three-entry allowlist the closure would be RED on day one at
`artifact/conf_integrity.rs` (`write_atomic` 1) and
`workspace_bootstrap/claude_settings.rs` (`write_atomic` 1). Either the
vocabulary was narrower than "leaf-publication spellings" or the allowlist was
longer; a reader cannot tell.

*[P2-1] above is exactly the gap the closure would have covered had it been
delivered — noted for the record, though `archived.rs` needs a ROW, not a
closure.*

---

## 5. THE NEGATIVE SCANS — sound, region scoping and anti-vacuity both drive

Baseline: both rows green on the pristine tree.

**Region boundaries, measured.** `rollback_evidence` signature at
`abort/evidence.rs:668` (annotation `:664-:667`), first column-zero `}` at
`:720`; `restore_baseline` at `abort/preflight.rs:350` (annotation `:346-:349`),
close at `:415` = the file's last line. The latter is why `masked_code` restores
the trailing newline — `item_body` needs `"\n}\n"` and `restore_baseline` is its
file's last item. Good, deliberate, and stated in the doc comment.

**Probe 5a — a door INSIDE `restore_baseline` (the second mixed arm):**

```rust
pub(super) fn restore_baseline(root: &Path, record: &MergeOperationRecord) -> ModelResult<()> {
    let _probe = artifact_facts::RegularFileFact::Missing;
```
```
test ...the_mixed_files_carved_arms_name_no_checked_boundary_door ... FAILED
panicked at capability_free_exception.rs:151:13:
the carved arm `pub(super) fn restore_baseline(` in `workspace_ops/merge/abort/preflight.rs`
names the boundary door vocabulary `artifact_facts`. …
```
FIRES.

**Probe 5c — a door INSIDE `rollback_evidence`:** FIRES, same message shape. And
the region scoping is real: the same `use` placed just ABOVE the signature does
NOT fire, which is the whole point of the region scope (the file's twenty-six
converted-arm door references are the control).

**Probe 5b — the git seam.** A second `_checked`-suffixed git-shaped name added
to `finalize.rs`:

```rust
#[allow(dead_code)]
fn probe_git_seam_checked(_x: u8) -> bool { true }
```
```
test result: ok. 2 passed; …
```
Does NOT fire. Correct — the needles are `checked_artifact` and `artifact_facts`,
not `_checked(`, and row 1 states that exclusion and pins it live
(`finalize.rs` must contain `commit_gwz_paths_checked` AND neither needle may be
a substring of it). The amendment's post-adoption correction (5) is confirmed:
`_checked(` has 40+ production hits and is unusable.

**Probe 5d — anti-vacuity, region lookup blinded.** Renamed `restore_baseline`
throughout:

```
panicked at v1_lifecycle/tests/mod.rs:65:28:
`workspace_ops/merge/abort/preflight.rs` no longer declares `pub(super) fn restore_baseline(`
```
Panics BY NAME, and `item_body`'s new `subject` parameter carries the file path
(E4.3-B's `body()` hard-coded `store/rewrite.rs`). A genuine improvement.

**Positive controls.** For `evidence.rs` the control is the file's own converted
arms (26 door references at `:22,:82,:83,:85,:88,…`). For `preflight.rs` it is
`:24` (`use super::super::root::artifact_facts`) and `:111-112` (the converted
`artifact_facts::observe`) — both outside the region, both real converted code.
Critically, the new `:348` annotation names `artifact_facts` inside a `///`
comment, which `masked_code` strips, so the control is NOT satisfied vacuously
by the package's own prose. Verified.

**The one hole is [P1-1]'s:** the byte floors cannot see a masker desync (§1.5).

---

## 6. THE ABORT SENTENCE — every home true by path; one obligation missed

§6's mandated sentence, verbatim:

> *"an abort that touches no checked artifact needs no such filesystem; aborts
> that must re-verify checked artifacts — preservation bundles, a selected
> root's manifest and lock, or the merge's published evidence, re-verified
> through the checked boundary — need persistent file handles and a mount
> identity."*

| home | delivered | verdict |
| --- | --- | --- |
| `capability.rs:50` ("`--abort` never does") | clause REMOVED from the sentence; a "**The `--abort` clause, SCOPED BY PATH**" paragraph carries §6's sentence **verbatim**, plus the dated-residual, the legacy-probe reconciliation, [P3-C1]'s pin and [P3-8]'s close | ✓ |
| `capability.rs:54-55` (remedy string) | "on Linux **the checked catalog admits** ext4 only"; "`gwz merge --abort`, which needs no such filesystem **unless it must re-verify checked artifacts — a preservation bundle, a selected root's manifest and lock, or the merge's published evidence**" | ✓ both §7 reconciliations |
| `OperationModel.md:131-132` | "An abort **asks only when it must re-verify a checked artifact, and then of a weaker probe**" | ✓ |
| `OperationModel.md:139` | "that is the CATALOG's admission list; an abort's checked doors go through the weaker legacy identity probe, which also admits btrfs, xfs and zfs" | ✓ (State axis round-2 note) |
| `OperationModel.md:147-149` | three paths named; "of the weaker legacy probe above" | ✓ |
| `OperationModel.md:158` | workaround scoped | ✓ |
| `v1_lifecycle/checked.rs:129-133` | "the LEASE is capability-free unconditionally, but an abort that must re-verify … still takes the legacy identity probe ON this lease" | ✓ — the right distinction |
| `runtime/dispatch.rs:437-438` | "an abort that re-verifies a checked artifact still takes the legacy identity probe there" | ✓ |
| `runtime/dispatch.rs:447-448` | "this abort is capability-free BY PATH (§6): it touches no checked artifact" | ✓ — correct, R4's race exit is a pre-publication abort |
| `v1_lifecycle/service.rs:112-115` | reverse-arm scoping | ✓ |

Six in-tree homes + four `OperationModel.md` edits. All ten present, all
by-path-true, all dated, all citing §6.

**The legacy probe IS strictly weaker — verified in the tree.**
`checked_artifact/identity.rs` (`name_to_handle_at` at `:312-367`, `statx`
`MNT_ID` in `rename_domain`) performs **no** filesystem-magic test.
`capability/pre_catalog/provider/platform/linux.rs:136-151` `require_ext4`
tests `f_type != EXT4_SUPER_MAGIC` (`:12`, `:144`) and refuses with "only local
ext4 with FS_IOC_GETFSUUID is admitted" (`:147`). So the abort's doors admit
btrfs/xfs/zfs where the catalog refuses. The remedy's reconciliation is true.

**Path (i) spot-verified**: `abort/evidence.rs:148-165` is
`artifact_facts::write_checked` / `remove_exact` — a checked door on the plain
lease, as §6 says.

**E4.1 carriers.** `contracts.rs:158`/`:181` are the remedy's content pins
(`:157-160` the named-substrings list; `:181` the renderer row's
`["persistent file handles", "--abort", "vfat lacks handles"]`). Both GREEN
against the rewritten string:

```
test checked_artifact::interface_tests::contracts::only_the_substrate_identity_capability_carries_an_actionable_remedy ... ok
test checked_artifact::interface_tests::contracts::the_catalog_refusal_renderer_carries_the_remedy_into_every_arm ... ok
test workspace_ops::tests::g23::a1_activation::a_v1_resume_refuses_without_mutation_and_abort_still_clears_the_record ... ok
+ 12 capability_permit rows      -> test result: ok. 15 passed; 0 failed
```

`[P3-C1]`'s named pin is at
`src/workspace_ops/tests/g23/a1_activation.rs:668` exactly (`#[test]` at `:667`).
**[P3-10]**: `capability.rs` cites it as "`tests/g23/a1_activation.rs`" — there
is no such path; it is `src/workspace_ops/tests/g23/a1_activation.rs` (and
`src/git/tests/g23/` does not exist). One-word fix.

**Flag 4 — CONFIRMED, and the disposition is right.** `60072a7` (E4.3-B) touches
six files: the checker, the aggregate gates, `bootstrap/managed.rs`,
`store/rewrite.rs`, `tests/store.rs`, `tests/store/record_root_exception.rs`.
There is no `--abort` content pin among them. §7's "E4.3-B +1-line `--abort`
content pin" does not exist; the amendment's post-adoption correction (8) records
that and names `contracts.rs:158,:181` instead, both verified green above.

**[P2-2] — §6's release-train obligation is UNDISCHARGED.** §6 ends:

> and the E4.1 release-notes line, which has NO on-disk home (E4.1 [P3-1]: it
> lives in a commit message; carrier the release train) — the corrected line is
> recorded below and **the pins package's delivery restates it verbatim for the
> train**.

Grepped the commit message for "touches no checked artifact" and "re-verify
checked artifacts": **zero hits**. The delivery restates nothing. The E4.1
release-notes line ("`gwz merge --abort`, which needs no such filesystem") is a
user-facing over-claim that now has neither an on-disk home NOR a carrier — the
one gap §6 explicitly designed the delivery to fill. Cure: one paragraph in the
commit message quoting the corrected sentence and naming the release train as its
carrier.

**[P3-9]**: [P3-8]'s close is recorded only in `capability.rs`'s doc comment. Its
own carrier sites (`entry_service.rs:28/37/56`, `entry_service_drift.rs:21/40/54`
and the shared `collect_files` exclusion list) say nothing, so a reader at the
residual does not learn it is closed. §7 authorises the wording ("[P3-8] closes
(nothing converts, no snapshot exclusion grows)"); the site compresses it to
"this package converts nothing" and drops the second half.

---

## 7. THE EIGHT JSON CARRIERS — exactly eight, accurate, nothing else moved

`dev-docs/GwzM5-8I2CompatibilityPredicates.json`: 8 insertions, 8 deletions —
**exactly the eight `byte-preserved-v0-origin` rows** (`AC-CANDIDATE`,
`AC-NOPUB-BORN`, `AA-PREACCEPTANCE`, `AA-CANDIDATE-COMPLETE`,
`AA-CANDIDATE-PARTIAL`, `AL-OPTIONAL-MISSING`, `AL-UNKNOWN`, `AR-C`), each
`tier2.carrier` only. The two `pending-fixture` rows (`AC-NOPUB-UNBORN`,
`AP-PRESERVED`) correctly untouched — they point at the R2-F fixtures lane, not
at E4.4. `tier1`, `clause`, `disposition`, `fixture` and every other key
unchanged in all ten rows.

New text vs §5, by clause: "DR-1, the phase-end design round" ✓ · "RE-POINTED
2026-09-02 from R2-E E4.4, which does not start" ✓ (§7) · "§5 explicitly DEFERS
the tier-2 comparable sub-surface for DR-1 to mint **by amendment with its own
dual**, naming its execution carrier there" ✓ verbatim to §5's own construction ·
"§3 carves the terminal archive as permanently capability-free" ✓ · the E5-landing
ratification retained ✓ · the residual fact retained ✓. The dropped clause ("E4
is gated on R2-F's relocation … transitively R2-F-dependent") is correctly
dropped: §5 records that half as satisfied by the landed relocation.

Checkers, all under the farm:

```
check_merge_compatibility_predicates.py  validated 7 migration rules, 7 runtime bindings, and 10 archive shapes
check_merge_docs.py                      merge document consistency: ok (12 sources, 155 assertions)
check_m4_scenario_map.py                 M4 scenario map: ok (39 scenario rows, 43 named tests, 22 registry rows all claimed)
check_checked_artifact_boundaries.py     checked-artifact boundary: ok (18 visible entries, 8 classified modules)

python3.13 -m unittest test_merge_compatibility_predicates      Ran 27 tests   OK
python3.13 -m unittest test_check_checked_artifact_boundaries   Ran 69 tests   OK   (871s)
python3.13 -m unittest test_check_merge_docs                    Ran  3 tests   OK
python3.13 -m unittest test_release_boundary                                   OK
python3.13 -m unittest test_v1_lifecycle_privacy_probe          Ran  8 tests   OK
```

Every claimed figure (7/7/10, 27, 12/155, 39/43/22, 69) reproduced.

---

## 8. THE ALLOWANCE ANNOTATIONS — complete against §4's list; flag 7 confirmed

Nineteen dated `CAPABILITY-FREE EXCEPTION` sentences, one at each carved site
(grepped and read: 19/19, each naming its `:275`-`:279` row(s), the reached
operation, "permanently", the date and the amendment). Two are `///` doc
comments on the mixed arms (`evidence.rs:664`, `preflight.rs:346`) scoping the
exception to THAT ARM and naming the region scan — correct and load-bearing.

The `E4.2-E4.6` / "awaiting R2-E consumer conversion" class, against §4's homes:

| §4 home | annotated | members named |
| --- | --- | --- |
| `catalog.rs:19-22,27-29,34-36,46` | `catalog.rs:6` | "the three `dead_code` allows on `bootstrap`, `classifier` and `enumeration`, and the `unused_imports` allow on the re-export below" — measured: exactly 4 `E4.2-E4.6` strings in the file (`:23,:30,:37,:47`); the file's other two allows carry different reasons ✓ |
| `coordinator/mod.rs:16` | `coordinator/mod.rs:14` | ✓ |
| `entry.rs:299-300` | inline at `:299` | ✓ |
| `bootstrap/managed.rs:45-46` | `managed.rs:43` | ✓ (but see [P3-6]) |
| `checked_artifact/mod.rs:12,18,29-31,37,44,50,59` | `mod.rs:10` | names 7 modules for the file's 7 `#[allow(` at `:11,17,28,36,43,49,58` ✓ |
| checker `:358` | `:496` in the candidate | verified: at `7f28907` (the amendment's tree) `:358` IS "A SECOND caller is an E4.2-E4.6 conversion…", and the candidate annotates exactly that comment ✓ |

Each annotation says the range is STALE, cites §7, names the remaining movers
(E4.5/6-B's three `finalization/execute.rs` arms), and says **E4.7 EXPIRES or
RE-REASONS** — not expired here. ✓ Correct scoping.

`archive.rs:108-111` — RE-REASONED to DR-1: "the terminal archive is CARVED …
so this checked family has no consumer to arrive: the route RE-OWNS to DR-1,
conditional on (C), and E4.7 deletes the family or re-reasons this allowance as
permanent-pending-DR-1." Verbatim to §5's `gc_archived` disposition. ✓

`catalog_names.rs:44` — "still live until E4.7" → "still live until DR-1 decides
its retirement — E4.7's legacy in-place-writer clause RE-OWNS there (2026-09-02,
§4)". Verbatim to §4's re-own. ✓

`workspace_mutator_lock.rs:44-47` (E4.1 [P3-5]'s stale allows) is NOT annotated —
correct: §7 names only the `E4.2-E4.6` class plus checker `:358` as this
package's duty; the [P3-5] class is E4.7's.

**Flag 7 — CONFIRMED.** An independent grep for the class strings across
`src/`, `scripts/` and `docs/` returns exactly two unannotated members outside
§4's list: `checked_artifact/interface_tests/catalog_activation_pin.rs:43`
and `:136` ("E4.2-E4.6"), and `src/git/tests/g12.rs:834` ("still live until
E4.7"). Both correctly left, both recorded in the amendment's bracket (6), both
routed to E4.7's STRING sweep. **[P3-8]**: after this package `g12.rs:834` and
`catalog_names.rs:44` carry contradictory sentences about the same retirement
(E4.7 vs DR-1) — harmless in a test comment, but the sweep should be told the
pair exists.

---

## 9. COMPANIONS — digests, counts, provenance, tripwire

**Six digests, recomputed with an independent implementation of the checker's
own algorithm** (flat = SHA-256 of the file's bytes; tree = SHA-256 over
length-prefixed relative-path + content for the root file plus, for a `mod.rs`
root, every file under its parent, sorted by POSIX relative path):

```
OK   flat  checked_artifact/capability.rs                da1d946305abda14…
OK   flat  checked_artifact/entry.rs                     2873107fc155d7f3…
OK   flat  checked_artifact/mod.rs                       c4125e60ad18ee48…
OK   flat  workspace_ops/merge/preserve/artifacts.rs     c2f97f284c6e9ad2…
OK   tree  checked_artifact/catalog.rs                   e5ff1f3fd014ca52…
OK   tree  workspace_ops/merge/v1_lifecycle/mod.rs       ce432c216a2862f0…
```

6/6. The digest ALGORITHM is untouched by the commit (diffed: no change to
`source_tree_digest` or the flat path).

**Counts, MEASURED on my own target, no sharing:**

```
cargo test --lib -p gwz-core workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix
    ok. 264 passed; 0 failed; 0 ignored; 1569 filtered out          <- pin 262 -> 264 ✓
cargo test --lib -p gwz-core checked_artifact::
    ok. 457 passed; 0 failed; 0 ignored; 1376 filtered out          <- 457 UNMOVED ✓
cargo test --lib -p gwz-core -- --skip 'checked_artifact::' --skip 'workspace_ops::merge::v1_lifecycle::'
    ok. 1110 passed; 0 failed; 1 ignored; 722 filtered out          <- 1110 + 1 UNMOVED ✓
cargo test --lib catalog_activation_pin      ok. 1 passed           <- PRODUCTION_CALLER_COUNT 1 ✓
cargo fmt --check                            clean
cargo clippy --all-targets                   clean
```

+2 is exactly the two new rows in `capability_free_exception.rs`; nothing else
moved, as the block claims.

**Provenance block form.** The new paragraph in `run_r4bg_aggregate_gates.py`
mirrors its predecessors exactly: what moved and why, the partition named, the
darwin value MEASURED with its date, the linux value DERIVED with its
cfg-independence argument ("Both new rows read source text checked out everywhere
and CRLF-normalize it under no `cfg` gate … FIRST-DISPATCH-EXPECTED at the
landing dispatch"), and the two unmoved partitions re-MEASURED in the same
session. The cfg-independence claim is TRUE — I read both rows; neither carries a
`cfg`, and both read from `CARGO_MANIFEST_DIR`. ✓

**P-1's red probes, driven by me (three directions):**

```
SHRINK  (write_atomic -> write_atomicX in finalize.rs)
  "capability-free raw writer inventory moved, workspace_ops/merge/finalize.rs (:277/:278/:279 …):
   expected={'write_atomic': 2, …} actual={'write_atomic': 1, …}. a PARTIAL CONVERSION of a carved
   arm may not land without revising GwzM5-8R2E-CapabilityFreeAmendment.md §3 -- these writers are
   a DATED EXCEPTION, not unfinished work, and E4.7 does not retire them"
GONE    (hide store/retention.rs)
  "capability-free carved file is GONE: workspace_ops/merge/store/retention.rs (:275 GC retention
   enforcement, the same lock); the capability-free exception, dev-docs/GwzM5-8R2E-
   CapabilityFreeAmendment.md §3 names it and must be revised first"
GROWTH  (write_marker added to handle_stage.rs — the write_atomic family in a non-durable_fs file)
  "… expected={'ensure_workspace_exclude': 1} actual={'write_marker': 1, 'ensure_workspace_exclude': 1}.
   A NEW RAW WRITER IS NOT BLESSED: the exception covers the arms it enumerates and nothing else,
   and converting this one instead would put a capability-free operation on the durable-identity probe"
```

Every message names the amendment and the row. Good.

---

## 10. CAP, REDUCTION TRAIL, TRAILERS, REBASE

### 10.1 Cap

My own `--numstat` sum: **+499 / −61** across 36 files. Nothing in the change set
is generated: `GwzM5-8I2CompatibilityPredicates.json` is a hand-maintained
registry (validated by `check_merge_compatibility_predicates.py`, produced by no
generator in `scripts/`), so its 8 added lines are handwritten and count.
**499/500 confirmed** — one line of headroom.

### 10.2 Reduction trail — nothing MANDATED was dropped

The ~37-line crate-wide namer closure was above §3's mandate (§3 mandates
"per carved file … fail-closed in BOTH directions" plus a per-carved-file
negative scan; it mandates no crate-wide sweep). Its stated duty is covered as
analysed in §4. Everything §3 and §7 mandate IS delivered — with the one
exception of §6's release-train restatement, [P2-2], which is a delivery
omission rather than a reduction. The masker is delivered but weak, [P1-1].

### 10.3 Trailers

`%(trailers)` on `b99bfb7`: **empty**. Author and committer both
`Gianni Mariani <gianni@everbility.com>`. No AI attribution. ✓

### 10.4 THE REBASE onto `0dae0d5` — collision list for the lane owner

`60072a7..0dae0d5` is two commits (`2b0eed9` E4.3-B round-1 fold, `0dae0d5`
landing nit) touching three files, ALL of which this candidate also touches.

**A. `src/workspace_ops/merge/v1_lifecycle/tests/store/record_root_exception.rs`
— the hardest.**

1. Main inserts a **7-line NAMED RESIDUAL comment INSIDE `code()`'s body** — the
   function the candidate DELETES. Do not simply drop it: given [P1-1], the
   residual is **not cured**; it must be rewritten and moved onto `masked_code`
   in `tests/mod.rs`, describing the NEW blind shape (char literals, raw strings
   with interior quotes, block comments) until the masker is fixed. If [P1-1] is
   cured properly, delete it and say so at the site.
2. Main rewrites `PRODUCTION_FILE_FLOOR`'s doc comment (the "up to 61 files may
   vanish unseen; the EXACT positive control is what catches the blinding that
   matters" text). The candidate leaves the base text. **Keep main's.**
3. Main widens two assertion messages — the `bypass` denylist ("…and nothing else
   in this denylist (the `v1_lifecycle/mod.rs` tree digest sees every byte)") and
   the `create_dir_all` one ("DECLINED as a refusal at E4.3-B because it is
   structurally undrivable…"). The candidate keeps the base text on both while
   changing only the `body(` → `item_body(` call. **Keep main's message text AND
   the candidate's `item_body` call.**
4. `production_rust_files` cite. base `:942-949` (already stale — actual `:974`);
   main `:982-989` (correct at main); candidate keeps `:942-949` while its own
   insertions move the function to **`:1084`**. See [P3-1]: the merged value must
   be **RE-MEASURED after the last checker edit** (derived estimate `:1092-1099`,
   but do not ship a derived number — this cite has now been wrong three times).

**B. `scripts/checks/check_checked_artifact_boundaries.py` — five hunks.**

1. `PROTECTED_SOURCE_TREE_DIGESTS["workspace_ops/merge/v1_lifecycle/mod.rs"]`:
   base `c99ae92…`, main `6122fce…`, candidate `ce432c2…` — **three different
   values, and none of them is right after the merge.** The tree root covers all
   of `v1_lifecycle/`, which both sides edit. **MUST BE RE-MEASURED post-merge.**
   (The other five digests are candidate-only edits in files main does not touch
   and carry through unchanged — verified per file.)
2. The O13 comment `:360-366`. Main rewrites the "one data row each" sentence to
   "…one data row each AT THE DATA LAYER … widening the SCAN SET to the non-v1
   carved files is **the pins package's** (amendment §3 (i)-(iii))". The
   candidate appends its "R2-E E4.4-6-B (2026-09-02) makes the two ARCHIVE rows
   permanent" paragraph immediately after `:366`. Keep BOTH, and re-tense main's
   forward reference now that the pins package has landed.
3. `V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS`'s comment: main appends "…within
   this v1_lifecycle scan; the wider scan set is that package's"; the candidate
   adds two dict entries. Keep both.
4. **The SystemExit guard.** Main replaces
   `assert V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS.keys() <= …` with a
   `for _key in sorted(…): raise SystemExit(…)` loop (E4.3-B [P3-1]'s fold), and
   adds a two-line "Unreachable while the module-scope guard above stands"
   comment on the `set(raw_writer_files) - expected_files` branch. The candidate
   keeps the `assert` **and adds two more module-scope `assert`s** in the retired
   idiom. Take main's guard AND convert the candidate's key-set digest and
   overlap asserts to the same `raise SystemExit` form — see [P3-2].
5. Two **direct string conflicts**, same lines, same intent, different wording:
   - `carved_finding`: main "(each row's amendment names its own re-decision
     point)" vs candidate "(the record root's at O14's fork; the capability-free
     carve-outs at DR-1)". **Take main's** — it is the folded review outcome and
     it survives the next carve-out.
   - the GREW message: main "the carved path's EXISTING publication primitive
     only" vs candidate "the carved arm's existing publication primitive only".
     **Take main's.**

   The candidate's new blocks (`CAPABILITY_FREE_*`, `TEST_MODULE`,
   `without_test_modules`, the new check loop, the ENTRY_REFERENCES annotation)
   do not overlap any main hunk.

**C. `src/checked_artifact/bootstrap/managed.rs`.** Main rewrites the
`unused_imports` reason at `:43-48` to the ruling-true text ("no E4.4-E4.6 as
chartered, activated-lease forward arms only …"). The candidate inserts an
annotation line immediately ABOVE that attribute whose last clause reads **"The
reason below still names E4.4 as the next mover"** — which becomes **FALSE** on
the rebase, because main's folded reason no longer does. Adjacent-line conflict
plus a truth defect. **[P3-6]**: drop that clause at the rebase (the rest of the
annotation, the STALE-range sentence, stays).

**D. No collision** in `run_r4bg_aggregate_gates.py`: main's two commits add and
remove no test, so `v1_lifecycle::` is still 262 at main and 262 → 264 stands.
Confirmed by reading both diffs.

---

## 11. FLAGS — disposition

The builder's numbered flag list is not on disk; I reconstructed the mapping from
the amendment's post-adoption bracket, which the mandate's "flag 4" and "flag 7"
both fit exactly under the ordering below. Where the mapping is my reconstruction
I say so.

| # | flag | disposition |
| --- | --- | --- |
| 1 | `store/archive.rs` has THREE raw `std::fs` mutations, not four | **CONFIRMED.** `create_dir_all` 1 + `remove_file` 2 = 3, independently recounted (§3.1). The amendment's §1 cell is corrected by bracket (1). |
| 2 | `handle_stash/commands.rs` is a second carved `:276` home the tables missed | **CONFIRMED.** 6 `write_bundle` + 1 `remove_file` under `guarded_workspace_root(StashMutate)` (§3.3). Correctly the 19th row. |
| 3 | Further carved sites cited by neither cell: a third pull-preflight `write_lock`, and the five `handle_*.rs` manifest/lock writers | **CONFIRMED**, all real listed-operation writers under guards or the bare lock, none an activated-lease arm (§3.3). **Cite nit [P3-7]**: the third `write_lock` is at `:458`, not `:457`. |
| 4 | E4.3-B's "+1-line `--abort` content pin" (§7) does not exist | **CONFIRMED.** `60072a7` touches six files, none a content pin. The real pins are `contracts.rs:158,:181`, both green (§6). Bracket (8) records it correctly. |
| 5 | Digest coverage MEASURED: 3 of 19 pinned, 16 unpinned | **CONFIRMED** by my own tree-root arithmetic: `preserve/artifacts.rs` flat; both v1 archive files by the `v1_lifecycle/mod.rs` tree root (which also covers `store/rewrite.rs`, so RR §3's backstop is true); the other 16 unpinned. This is *why* [P1-1] matters for P-2. |
| 6 | `_checked(` unusable as a needle (40+ production hits) | **CONFIRMED**, and the substitute is pinned live: probe 5b shows the git seam does not fire, and row 1 asserts `finalize.rs` still contains `commit_gwz_paths_checked` while neither needle is a substring of it. |
| 7 | Two allowance-class members §4 omits: `catalog_activation_pin.rs:43,136`; `g12.rs:834` | **CONFIRMED** by independent grep — exactly those two and no others. Correctly left for E4.7's STRING sweep per bracket (6). **[P3-8]**: tell the sweep that `g12.rs:834` and `catalog_names.rs:44` now disagree. |
| 8 | Pin references counted with `#[cfg(test)]` modules dropped first — a stated departure from O13 | **CONFIRMED and sound** (§2). Probes 2a/2c correct; 2b/2d over-count in the LOUD direction only; no such module exists in any carved file today. **[P3-3]** on the "PRODUCTION surface" wording. |
| 9 | *(reconstructed)* The crate-wide namer closure drafted and dropped for the line budget | **ACCEPTED as a reduction** — above mandate, its "row deleted" duty fully covered by the key-set digest (probes 4b). **[P3-4]**: the parenthetical is not reconstructable — with the stated allowlist it would be RED today at two files. |
| 10 | *(reconstructed)* Two maps, "measured exactly once", O13 kept as the SCAN | **ACCEPTED and correct.** O13 must stay: it is fail-closed against an UNSEEN `durable_fs` file under `v1_lifecycle/`, which a list cannot be. Disjointness has two independent belts (probe 4a). `store/rewrite.rs` correctly stays RR's alone. |

---

## FINDINGS

### [P1-1] — the shared masker errs QUIET in four shapes, three live in the tree; the cure is a 35× regression on the property it was written to strengthen. DRIVEN.

§1. Probes M-a (`character != '"'`), M-c (`r#"a"b"#`), M-b2 (`b'"'`), M-h
(`r"C:\"`), M-i (`/* … " … */`), M-k (`c == '"'`) all leave the door VISIBLE in
production source and the tripwire GREEN. Probe M-live declares
`rewrite_merge_store_record` in `git/gitbackend/merge_support.rs` and the
re-pointed E4.3-B tripwire reports `2 passed`. Live blind surface: 178 lines
across 3 production files (was 5 across 3), and unbounded where the old was
line-local. No belt in P-2 can see it: the byte floors are length-invariant under
blanking. §7 made this a NAMED OBLIGATION precisely so the promise would not live
only in prose; delivered as written it is weaker than the prose it replaced, and
the residual is now recorded as cured. **Must cure.** Port `mask_non_code`
faithfully, or at minimum handle char literals and `panic!` when the mask ends
inside a string.

### [P2-1] — the twentieth carved row: `workspace_ops/merge/store/archived.rs::archive`, a live `:275` writer reached from ordinary merge and both aborts, pinned by nothing.

§3.4. `create_dir_all(DONE_DIR)` (`:39`), `remove_file` (`:47`),
`rename_durable` (`:49`), `sync_dir` ×2 (`:51,:52`); reached
`finalize_dispatch.rs:34` / `finalize_support.rs:99` / `abort/mod.rs:111,:189,:218`
via `archive_merge_record` → `FileMergeStore::archive`. Absent from the
inventory, from O13, from all seven tree roots and the flat table, from
`PURE_CARVED_FILES`, and from every dated annotation — while its two
same-directory `:275` siblings (`store/gc.rs`, `store/retention.rs`) have all of
them. Appears nowhere in the amendment, the charter prep or either axis of the
amendment's dual. **Must cure**: add the row + annotation + `PURE_CARVED_FILES`
entry + new key-set digest, and one sentence in the amendment's bracket.

### [P2-2] — §6's release-train restatement is undischarged; the E4.1 abort over-claim now has neither an on-disk home nor a carrier.

§6. Grepped the delivery for the corrected sentence: zero hits. **Must cure**:
restate §6's sentence verbatim in the delivery and name the release train.

### P3

- **[P3-1]** `production_rust_files` cite stale by 142 lines as delivered
  (`:942-949`; actual `:1084-1091`), and re-staled by the rebase. Third
  occurrence of E4.3-B [P3-5]/[P3-5R]. Measure it after the last checker edit.
- **[P3-2]** Two new module-scope `assert`s reintroduce the idiom main's E4.3-B
  fold retired for [P3-1]; stripped under `python -O`/`PYTHONOPTIMIZE`, and they
  emit a traceback rather than a named finding. House-consistent on the delivered
  base; convert at the rebase.
- **[P3-3]** `#[cfg(all(test, …))]` and indented `#[cfg(test)] mod` are not
  dropped (probes 2b/2d). Loud direction only; none exists today. "The pin
  measures the PRODUCTION surface" is approximate.
- **[P3-4]** The dropped closure's parenthetical is not reconstructable: with the
  stated vocabulary and its three-entry allowlist it would be RED today at
  `artifact/conf_integrity.rs` and `workspace_bootstrap/claude_settings.rs`.
- **[P3-5]** Unstated scope limit: converting `artifact::write_atomic`'s own
  implementation would convert every carved `:277`/`:278`/`:279` caller while
  moving no count and naming no door — nothing fires, and `artifact/mod.rs` is
  unpinned.
- **[P3-6]** `bootstrap/managed.rs`'s annotation clause "The reason below still
  names E4.4 as the next mover" becomes FALSE on the rebase onto `0dae0d5`.
- **[P3-7]** Amendment bracket (3) cites `pull_head_member_preflight.rs:457`; the
  writer is at `:458`.
- **[P3-8]** `g12.rs:834` ("still live until E4.7") and the re-reasoned
  `catalog_names.rs:44` ("until DR-1") now disagree in-tree; name the pair for
  E4.7's string sweep.
- **[P3-9]** [P3-8]'s close lives only in `capability.rs`, not at its own carrier
  sites, and drops §7's "no snapshot exclusion grows" half.
- **[P3-10]** `capability.rs` cites the [P3-C1] pin as
  "`tests/g23/a1_activation.rs`"; the path is
  `src/workspace_ops/tests/g23/a1_activation.rs`.

---

## What is GOOD, for the record

The generalization is faithful to §3's three named changes and the mechanism is
better than O13's in every direction I could drive: growth, shrinkage, file-gone
and set-moved all fire with messages that name the amendment and the row, and the
two-map split is defended by two independent belts. The nineteen rows are exactly
right — I recounted every primitive in every one with independent code and found
zero errors, including the three sites the builder added over the amendment's own
tables. The region-scoped mixed-file scans are correctly bounded, correctly
controlled, correctly anti-vacuous, and `item_body`'s new `subject` parameter is a
real improvement on E4.3-B's hard-coded panic. The abort sentence is by-path-true
at all ten homes, the "ext4 only" reconciliation is verified against
`identity.rs` and `linux.rs`, and the JSON re-point is exactly the eight rows §5
names, with the two `pending-fixture` rows correctly untouched. Six digests, three
partition counts, five checker suites and the aggregate-gate provenance block all
reproduce. The package converts nothing, adds no probe, and touches no production
write path — as it says.

Three defects stand between it and the tree, and all three are small.

---

## 12. Lane-owner escalation ruling (Fable tier, 2026-09-02, under the operator's E4 standing order: round-1 NO-GO → the lane owner rules and remediates within the two-round cap)

**Mechanism verified before ruling, per the program's lesson (verify the
mechanism, positive or negative, before folding a reviewer claim):**

- **[P1-1] CONFIRMED.** `masked_code` (`v1_lifecycle/tests/mod.rs:38-57` at
  `b99bfb7`) declares `quoted` OUTSIDE the per-line loop; the char literal
  `'"'` at `git/gitbackend/merge_support.rs:177` (`character != '"'`) and
  `:236` (`output.push('"')`) flips it, and every character to the next stray
  `"` or end of file is blanked. The doc comment's two claims are false in the
  direction that matters. **Ruling: MUST CURE — faithful Rust port of the
  checker's `mask_non_code` (line comments, nested block comments, hash-matched
  raw strings, escaped strings, char literals with lifetime disambiguation),
  PLUS a fail-loud belt (panic when the mask ends inside a literal or comment)
  PLUS a table-driven masker self-test over the reviewer's probe shapes and the
  lifetime shapes, PLUS the M-live re-drive (door declared in
  `merge_support.rs` → tripwire RED) as an ablation transcript in the delivery.**
  Main's NAMED-RESIDUAL comment (E4.3-B [P3-3]) is deleted only WITH a dated
  sentence at the masker saying the residual is cured by the port and which
  shapes the self-test proves.
- **[P2-1] CONFIRMED.** `store/archived.rs::archive` carries `create_dir_all`
  (`:39`), `remove_file` (`:47`), `rename_durable` (`:49`), `sync_dir` ×2
  (`:51-52`); `archive_merge_record` is called from `finalize_dispatch.rs:34`,
  `finalize_support.rs:99`, `abort/mod.rs:111,:189,:218`; the file is named by
  no inventory, digest, scan or annotation. **Ruling: MUST CURE — the twentieth
  row (counts measured), the `:275` site annotation, the `PURE_CARVED_FILES`
  entry, the new key-set digest (gwz-core, the builder); the amendment's §7
  bracket sentence (root, the lane owner, at landing).** The gap is inherited
  from the amendment's §1 (which, like the charter prep and both axes of its
  dual, never names `archived.rs`) — the package's charter was to enumerate in
  the tree, and this is the fourth §1 omission it should have found.
- **[P2-2] CONFIRMED.** `git show -s b99bfb7` contains neither "touches no
  checked artifact" nor "re-verify checked artifacts" (48-line message, zero
  hits). **Ruling: MUST CURE — the fold commit's message restates §6's sentence
  verbatim and names the release train as carrier; the lane owner gives the
  corrected line an on-disk home in the plan's release-notes rider at landing.**

**P3 dispositions.** [P3-1] re-measure the cite after the LAST checker edit;
[P3-2] convert the two new module-scope asserts to main's `raise SystemExit`
form at the rebase; [P3-3] soften the wording to what the drop does; [P3-4] make
the parenthetical true or delete it; [P3-5] the scope-limit sentence in the
checker AND a flat digest on `artifact/mod.rs` — ruled in, because the reviewer
is right that it is the cheapest defeat of the whole pin and a one-line pin
closes it; [P3-6] drop the false clause at the rebase; [P3-7] root-side, lane
owner (`:458`); [P3-8] root-side, the pair goes into E4.7's string-sweep brief;
[P3-9] the authorised wording at the carrier sites; [P3-10] the path.

**Sequencing.** The fold is delivered ON TOP OF the rebase onto main `0dae0d5`
(resolutions per §10.4 A–D verbatim; digest and cite re-measured post-merge), as
two commits — the rebased package and the fold — so round 2 reviews the tree
that lands. The standalone GC fix (`gc/v1-archive-decode`, in its own round 2)
lands FIRST; the pins package's final rebase over it (the `store/gc.rs` hunk:
GC's seam read and comment above the pins' exception comment; the
`v1_lifecycle/mod.rs` digest recomputed) is the lane owner's at landing — a
rehearsal of that exact rebase (`scratchpad/pins-rehearsal`, checker green)
already exists.

**Cap.** The fold ≤ +90 net whole lines over the delivered 499 (both commits
≤ 589 MEASURED); an overrun for the faithful port is to be delivered and
disclosed, not avoided — a quiet masker under an absence pin is the trade this
program refuses; line count is not. Stop-trigger: a port needing > ~120 lines
or a change outside `tests/` returns to the lane owner before it is written.

**Round 2** (the same reviewer, peer-blind to this ruling's execution) asks only
on the three cures, the P3 folds, the rebase resolutions and the re-measured
companions.

---

## 13. Round 2 (2026-09-02) — the same reviewer, on the three cures, the P3 folds, the §10.4 resolutions and the re-measured companions

Candidate: `e4/e4-4-6b-capfree-pins`, two commits on main `0dae0d5` — `086f7c0`
(the round-1 package rebased, carrying §10.4's A–D resolutions) and `b31a229`
(the fold). Reviewed AS DELIVERED against `0dae0d5`; the final rebase over
`3c632ec` (the landed GC fix) is the lane owner's and is not reviewed or
reported here. Worktree `e4-pins-wt` byte-clean at `b31a229` on exit; the
assigned target `e4-pins-target` used throughout, no extra target created;
partitions run from one `--list`-verified snapshot binary
(`debug/deps/gwz_core-dba41e97d4500b7d`, 63 162 520 bytes).

Round 1's §§1–11 verified ground is not re-reviewed.

### 13.1 [P1-1] THE MASKER — **CURED**, structurally and empirically

The replacement (`v1_lifecycle/tests/mod.rs:29-177`) is a five-arm SCANNER —
helpers `at` (`:29-36`), `raw_string_hashes` (`:38-45`), `blank` (`:47-55`), and
`masked_code` (`:78-177`) — that skips each construct WHOLE. There is no
`quoted` flag to desynchronise: that is the structural cure, and it is why the
round-1 shapes cannot recur rather than merely being enumerated away.

**Arm-for-arm against the reference.** I read the port beside
`check_checked_artifact_boundaries.py::mask_non_code` (`:1155-1225`). The arms,
their order (`//`, then `/* */`, then raw, then string, then char) and every
bound match, including: nested block-comment depth counting; raw-string close by
matching hash count (so `r"`, `r#"…"#`, `r##"…"##` and `br#"…"#` all close
correctly); `\\` consuming two positions inside a normal string; and the char
arm's lifetime disambiguation (`index + 2 < len`, `+3` on a backslash, and the
"a lifetime carries no closing quote" test at `:155`, which is exactly the
Python's `if end < length and text[end] == "'"`). Bounds are safe: `at`'s
`text.len() - index` is only ever reached with `index < text.len()`, `blank`
min-clamps, and an `index` that overshoots exits the loop.

**One deliberate divergence, disclosed and benign.** `:147-153` adds a
`'\u{…}'` sub-branch the Python does not have. The doc names it ("plus
`'\u{…}'`, which the Python skips rather than masks"). It makes the mask
STRICTER — see 13.1.3.

#### 13.1.1 Every round-1 shape re-driven, individually, on the real tree

Each probe appended to a PURE carved file (`workspace_ops/merge/store/gc.rs`),
`cargo test --lib capability_free`, then reverted. All six shapes that were
QUIET in round 1 now FIRE:

```
M-a   pub(crate) fn q(c: char) -> bool { c != '"' }        -> FAILED (1 passed; 1 failed)
M-b2  pub(crate) const C: u8 = b'"';                        -> FAILED (1 passed; 1 failed)
M-c   pub(crate) const S: &str = r#"a"b"#;                  -> FAILED (1 passed; 1 failed)
M-h   pub(crate) const P: &str = r"C:\";                    -> FAILED (1 passed; 1 failed)
M-i   /* don't use " here */                                -> FAILED (1 passed; 1 failed)
M-k   fn k<'a>(c: char, s: &'a str) -> bool { c == '"' … }  -> FAILED (1 passed; 1 failed)
M-g   fn e() -> &'static str { "see https://gwz.invalid/x" } -> FAILED (1 passed; 1 failed)
```

with the row's own message, e.g. for M-a:

```
`workspace_ops/merge/store/gc.rs` names the checked boundary door vocabulary
`checked_artifact`, but GwzM5-8R2E-CapabilityFreeAmendment.md §3 carves ALL of its
writers out PERMANENTLY … Revise the amendment, at DR-1, before this may exist
```

**R2-P1 — the combined probe, the strongest single form.** Sixteen tricky
constructs in one module, with ONE door as the last line, so a FIRE proves that
no construct before it desynchronised the mask: an odd `"` in a block comment, a
nested block comment, `c != '"'`, `<'a>` + `c == '"'`, `b'"'`, `r#"a"b"#`,
`br##"a"#b"##`, `r"C:\"`, `b"a/b"`, `"\""`, `'\''`, `'\u{2014}'`, a multi-line
`r#"…"#` containing `//`, `"see https://…"`, and an `'outer:` label. Result:

```
test …the_pure_carved_capability_free_files_name_no_checked_boundary_door ... FAILED
test result: FAILED. 1 passed; 1 failed; 0 ignored; 0 measured; 1833 filtered out
```

**Twenty-six-shape table** (byte-faithful emulation of the delivered port,
validated against the real Rust by the self-test's agreement and by the 411-file
run in 13.1.3): M-a, M-c, M-b2, M-h, M-i, M-k, M-g, M-b, M-d, M-e, M-j, plus
lifetimes `'a` / `'static` / `<'a>` before a same-line string / `'outer:` label /
`<'_>`, chars `'\''` / `'\u{2014}'` / `'_'`, raw forms `br#"a"b"#` / `r"a/b"` /
`r##"a"#b"##`, a string containing `/*` and `//`, a block comment containing
`//`, and a lifetime declaration immediately before a string — **25 FIRE, 1
quiet: M-f only** (the disclosed change, 13.1.4).

#### 13.1.2 M-live re-driven — the round-1 evasion is RED

Appended to `src/git/gitbackend/merge_support.rs`, past the `:236` desync that
blinded 144 lines under the round-1 masker:

```rust
/// PROBE: the region blinded by the round-1 masker (desync began at `:236`).
#[allow(dead_code)]
pub(crate) fn rewrite_merge_store_record() {}
```
```
test …record_root_exception::the_checked_rewrite_door_is_absent_from_production_sources ... FAILED
production sources name the checked rewrite door `rewrite_merge_store_record` at
{"git/gitbackend/merge_support.rs"} — the rejected E4.3 conversion, whose detach-then-
publish window no shipped reconciler closes. GwzM5-8R2E-RecordRootAmendment.md §2 must
be revised, at O14's fork, before it may exist
```

Round 1's `2 passed` is now a named RED that reports the offending file.

#### 13.1.3 The 411-file differential — **zero blinded lines**

I re-ran round 1's measurement, this time diffing a byte-faithful emulation of
the DELIVERED port against the checker's own `mask_non_code` over every file in
the record-root tripwire's walk:

```
production files in the walk: 411
RESULT: blinded-by-port 6 lines / 2 files ; port-keeps-more 0 lines / 0 files ; belt panics 0
```

and the six are, in full, the `'\u{…}'` char literals the port masks and the
Python leaves visible:

```
git/gitbackend/merge_support.rs:194
   src :  '\u{0007}' => output.push_str("\\a"),
   py  :  '\u{0007}' => output.push_str(     ),
   rust:             => output.push_str(     ),
  (same shape at :195, :198, :199; workspace_bootstrap/claude_settings.rs:269, :270)
```

What the port hides that the reference does not is **char-literal content**,
which cannot spell a door. **Real blind lines: 0** — against 178 across three
files at `b99bfb7` and 5 at `60072a7`. The regression is not merely repaired; the
property is now strictly better than the state the package inherited.

#### 13.1.4 The belt — fires on a real read, false-positives on nothing

The `#[should_panic(expected = "ends inside a string literal")]` row passes. It
is also driven on a REAL FILE READ, which the unit row cannot reach: a walked
but uncompiled source (`src/zz_probe_belt.rs`, declared by no `mod`, so `rustc`
ignores it while `production_sources()` reads it):

```
/* PROBE: this block comment is never closed
panicked at v1_lifecycle/tests/mod.rs:166:9:
`zz_probe_belt.rs` ends inside a block comment: the mask ran to end of input, so every
character after the opener is BLANK and an absence asserted over this file would be an
artefact of the mask, not a property of the source

const S: &str = "never closed
panicked at …: `zz_probe_belt.rs` ends inside a string literal: …
```

Both forms panic and BOTH NAME THE FILE — the fold's `masked_code(relative, …)`
threading (`record_root_exception.rs:67`, `capability_free_exception.rs`'s
`carved`) is what makes that possible. **No false positive**: 0 panics over the
411 in the emulation, and `the_checked_rewrite_door_is_absent_from_production_sources`
— which masks all 411 through the real port — passes.

The belt is unreachable on a tree `rustc` accepts (every unterminated construct
is a lex error), which is the right shape for a belt; it protects a truncated,
mis-encoded or non-Rust read, and it is driven by both rows above.

#### 13.1.5 The doc, and the cured-residual sentence — TRUE

Both round-1 false claims are GONE (`only "` toggles…`" and `//` cannot occur
inside a char literal") — grepped, zero hits. The replacement (`:57-77`) claims
the port is faithful, names the one divergence, and records E4.3-B [P3-3] and
round-1 [P1-1] as CURED, naming twelve shapes the self-test proves. I checked
all twelve against the fifteen table rows (`:185-201`): `'"'`→row 1, `c == '"'`→2,
`b'"'`→3, `r#"a"b"#`→4, `r"C:\"`→5, odd `"` in a block comment→6,
`"https://…"`→7, `b"…"`→8, nested block comment→9, `//` in a raw string→10,
lifetimes→2/7/11, `'\''`→12, `'\u{…}'`→13 (plus 14 line comment, 15 escaped
quotes). **Every named shape is present; the sentence is true.** §10.4 A1's
condition — main's NAMED RESIDUAL deleted only with a true cured-residual
sentence — is met.

#### 13.1.6 The disclosed behaviour change (M-f): ACCEPTABLE

A door named inside `/* … */` no longer fires. This is correct on three grounds:
(i) a door in a comment is not a call, and stable Rust has no way to reach an
item without spelling its name contiguously in code (E4.3-B [P3-3] probe E7);
(ii) `//` comments were ALREADY invisible under both prior maskers, so block
comments were an inconsistency, not a feature — E4.3-B's own probe E6 called the
block-comment hit "a false positive"; (iii) the P-1 half has always used
`mask_non_code`, which masks block comments, so the two halves are now
consistent. **No pin relied on comment visibility** — proven, not argued: every
positive control in the suite (`LANDED_DOOR` in exactly two files;
`checked_artifact` in `artifact_facts.rs`; `commit_gwz_paths_checked` in
`finalize.rs`; a door somewhere in each mixed file; `create_dir_all` count 1 in
`rewrite.rs`) is satisfied by CODE, and all six rows are green under the new
mask. The only thing lost is that a conversion staged as commented-out code no
longer trips the scan — which is right, because commented-out code is not a
conversion.

### 13.2 [P2-1] THE TWENTIETH ROW — **CURED**

**Recounted independently** — my own masker, my own (stricter) `#[cfg(test)]`
drop, my own counter, no checker code:

```
ok  workspace_ops/merge/store/archived.rs  {'rename_durable': 1, 'sync_dir': 2, 'create_dir_all': 1, 'remove_file': 1}
… (all twenty rows) …
rows: 20  mismatches: 0
```

**Key-set digest recomputed independently**: `sha256` over the sorted keys =
`867c580f625d7efe0cf72dcc8e0ad01e36268d1478829a469eb0f57953dbd385` — matches the
pinned constant exactly.

**All four directions driven on the new row:**

```
GROWTH   (a second remove_file in archived.rs)
  capability-free raw writer inventory moved, workspace_ops/merge/store/archived.rs
  (:275 the v0 terminal archive -- ordinary merge finalization and BOTH abort forms):
  expected={…'remove_file': 1} actual={…'remove_file': 2}. A NEW RAW WRITER IS NOT BLESSED …
SHRINK   (sync_dir -> sync_dirX at :52)
  … expected={…'sync_dir': 2 …} actual={…'sync_dir': 1 …}. a PARTIAL CONVERSION of a carved
  arm may not land without revising GwzM5-8R2E-CapabilityFreeAmendment.md §3 …
GONE     (the file hidden)
  capability-free carved file is GONE: workspace_ops/merge/store/archived.rs (:275 …);
  the capability-free exception … names it and must be revised first
DOOR     (use crate::checked_artifact; in archived.rs)
  `workspace_ops/merge/store/archived.rs` names the checked boundary door vocabulary
  `checked_artifact` … test result: FAILED. 1 passed; 1 failed
SET      (a row deleted)  raise SystemExit: the capability-free carved SET moved …
```

**Companions of the row**: the `:275` annotation is at `archived.rs:39` in its
siblings' exact form and names both reached operations; `src/` now carries
**20** `CAPABILITY-FREE EXCEPTION` sites; `PURE_CARVED_FILES` is `[&str; 18]`
with the file added and the module doc's "Seventeen" → "Eighteen"; the checker's
counts of the inventory ("nineteen"→"twenty", "Sixteen"→"Seventeen" unpinned,
"thirteen"→"fourteen" without `durable_fs`, "Two corrections"→"THREE") are all
updated. The root-side bracket (9) is landed in the amendment (`:518-535`) and
states the provenance correctly.

### 13.3 [P2-2] THE SENTENCE — **CURED, byte-identical**

Extracted §6's mandated sentence from the amendment and the quoted block from
`git show -s b31a229`, whitespace-normalised (commit-message wrapping is the only
possible difference):

```
AMENDMENT §6 : an abort that touches no checked artifact needs no such filesystem; aborts
that must re-verify checked artifacts — preservation bundles, a selected root's manifest
and lock, or the merge's published evidence, re-verified through the checked boundary —
need persistent file handles and a mount identity.
FOLD MESSAGE : (identical)
BYTE-IDENTICAL (whitespace-normalised): True
```

The carrier is named ("THE RELEASE TRAIN IS ITS CARRIER"), and the message goes
further than the obligation by stating what the corrected line SUPERSEDES and
why (the legacy probe at `identity.rs:312-367`, "strictly weaker than the
catalog's `require_ext4` but not nothing"). The on-disk home in the plan's
release-notes rider remains the lane owner's at landing, per §12.

### 13.4 The P3 folds

| finding | delivered | verified |
| --- | --- | --- |
| [P3-1] | `record_root_exception.rs:47` cites `:1120-1127` | **TRUE** — `def production_rust_files` is at `:1120`, its body closes at `:1127`, measured after the last checker edit |
| [P3-2] | both new module-scope asserts → `raise SystemExit` | **DONE** — zero module-scope `assert`s remain; three `raise SystemExit` guards at `:408` (main's), `:511` (key-set digest), `:520` (overlap). Driven: the delete probe now emits the named message, not a traceback |
| [P3-3] | drop wording narrowed | **TRUE** — "TOP-LEVEL `#[cfg(test)] mod` blocks are dropped first … `cfg(all(test, …))` and an INDENTED `#[cfg(test)] mod` are NOT dropped, so they OVER-count, which an exact-count pin raises immediately … No carved file carries either shape today." Matches my round-1 probes 2b/2d and my recount (which used the stricter drop and still agreed on all twenty) |
| [P3-4] | parenthetical made honest | **TRUE** — "it was drafted and dropped for the line budget, and its allowlist is NOT reconstructable from this text" |
| [P3-5] | scope limit stated AND closed | **DONE and DRIVEN** — the six-line SCOPE LIMIT paragraph before the inventory, plus the flat row `"artifact/mod.rs": "22bce818…"`. Probe: a new item appended to `artifact/mod.rs` → `- protected source allowlist changed: artifact/mod.rs`. The cheapest defeat of the pin now moves a pin |
| [P3-6] | the false managed.rs clause dropped | **DONE** — `managed.rs:43` keeps only the STALE-range sentence, which is true against main's folded reason |
| [P3-9] | §7's authorised wording at the carriers | **DONE** — at `entry_service.rs`'s shared `collect_files` exclusion list ("[P3-8] closes (nothing converts, no snapshot exclusion grows) … so this list does not grow for it"), at `entry_service_drift.rs`, and the `capability.rs` site upgraded from "this package converts nothing" to the full authorised form |
| [P3-10] | the path | **DONE** — `src/workspace_ops/tests/g23/a1_activation.rs` |

### 13.5 The §10.4 rebase resolutions

| | resolution | verified against `git diff 0dae0d5 b31a229` |
| --- | --- | --- |
| A1 | main's NAMED-RESIDUAL deleted with `code()`, cured-residual sentence at the masker | **DONE, and the condition I attached is met** — the sentence is true (13.1.5) |
| A2 | main's `PRODUCTION_FILE_FLOOR` doc kept | **KEPT** verbatim ("up to 61 files may vanish unseen by it, so the EXACT positive control … is what catches the blinding that matters") |
| A3 | main's two widened assertion messages kept, with `item_body` calls | **KEPT** — the denylist message at `:99` and the "DECLINED as a refusal at E4.3-B … structurally undrivable" text, both alongside `item_body(&rewrite, "store/rewrite.rs", …)` |
| A4 | cite re-measured | **DONE** (`:1120-1127`, correct) |
| B1 | `v1_lifecycle/mod.rs` tree digest re-measured post-merge | **DONE** — `29168296…`, recomputed by me |
| B2 | main's "AT THE DATA LAYER" sentence + the candidate's paragraph, re-tensed | **DONE and TRUE** — "widening the SCAN SET to the non-v1 carved files is the pins package's, **LANDED BELOW as `CAPABILITY_FREE_RAW_WRITER_INVENTORY`**" |
| B3 | exceptions comment, both sides, re-tensed | **DONE and TRUE** — "the wider scan set **landed as** `CAPABILITY_FREE_RAW_WRITER_INVENTORY` below" |
| B4 | main's SystemExit guard + the two conversions | **DONE**; main's "Unreachable while the module-scope guard above stands" comment kept at `:1413` |
| B5 | main's wording on both strings | **DONE** — `:1407` "(each row's amendment names its own re-decision point)" and `:1439` "the carved path's EXISTING publication primitive only" |
| C | `managed.rs` | **DONE** ([P3-6]) |
| D | no collision in the gate driver | correct — main's two commits add and remove no test |

### 13.6 Companions — all re-measured by me

Partitions run from ONE `--list`-verified snapshot binary. The `--list` counts
partition exactly: 266 + 457 + 1111 + 1 (`root_fault_matrix`) = **1835 = the
binary's total**.

```
--list  v1_lifecycle:: minus root_fault_matrix   266
--list  checked_artifact::                        457
--list  remainder                                1111   (1110 + 1 ignored)
--list  total                                    1835

run  v1_lifecycle::      ok. 266 passed; 0 failed; 0 ignored; 1569 filtered out   (359.30s)
run  checked_artifact::  ok. 457 passed; 0 failed; 0 ignored; 1378 filtered out   ( 54.59s)
run  lib remainder       ok. 1110 passed; 0 failed; 1 ignored; 724 filtered out   ( 61.27s)
run  root_fault_matrix (release)  ok. 1 passed; 0 failed                          (368.36s)
```

262 → 266 is exactly the four rows the driver's block now names: P-2's two, plus
the masker self-test and its `#[should_panic]` belt. The dated block is TRUE at
every clause I could check — the partition it moves, the four rows by name, the
cfg-independence argument ("All four rows read source text checked out
everywhere (two of them only in-memory literals) under no `cfg` gate"), and the
base ("at the `0dae0d5` base — the standalone GC decode fix has NOT landed and
its +4 to the remainder is deliberately NOT pre-counted here"), which my
remainder of 1110 confirms. The battery expectation is `"266 passed"`.

**Digests** — I recomputed **every** row of both tables with my own
implementation of `source_tree_digest`'s algorithm: 18 flat + 7 tree, **0
mismatches**, including the three the fold names (`artifact/mod.rs`
`22bce818…`, `checked_artifact/capability.rs` `dcb8d2b0…`,
`v1_lifecycle/mod.rs` tree `29168296…`) and the five unchanged.

**Checkers, under a symlink farm** (`farm/gwz-core` a real directory of
symlinks with `scripts/checks/` copied, so `Path(__file__).resolve().parents[3]`
lands on the farm root):

```
checked-artifact boundary: ok (18 visible entries, 8 classified modules)
validated 7 migration rules, 7 runtime bindings, and 10 archive shapes
merge document consistency: ok (12 sources, 155 assertions)
M4 scenario map: ok (39 scenario rows, 43 named tests, 22 registry rows all claimed)
unittest test_check_checked_artifact_boundaries:  Ran 69 tests in 778.160s  OK
```

**Tripwires and pins**: `catalog_activation_pin` (PRODUCTION_CALLER_COUNT 1) 1
passed; `contracts.rs:158`/`:181` 2 passed against the rewritten remedy string;
E4.1's `a_v1_resume_refuses_without_mutation_and_abort_still_clears_the_record`
1 passed. `cargo fmt --check` clean; `cargo clippy --all-targets` zero
warnings. **Trailers: none on either commit**; author and committer
`Gianni Mariani <gianni@everbility.com>` on both.

### 13.7 The port audit (the cap's reviewable half)

Measured: the masker block is `:29-177` = **149 lines** (helpers 27 incl. doc,
the `masked_code` doc comment 21, the scanner body 100); the self-test and belt
are `:179-228` = **50**. Net over `0dae0d5`: **+713 / −68 over 39 files**
(`086f7c0` +506/−68, `b31a229` +264/−57). The re-ruled cap is the lane owner's
and is not re-litigated here.

**Does the port carry anything a faithful port does not need?** No. I read it
arm by arm against the reference: no duplicated scanning, no shape handled
twice, no dead arm, no unreachable branch. The scanner is longer than the
Python's ~72 lines for three accountable reasons — Rust `else if` arms with
braces, the `unterminated` bookkeeping the ruling mandated (~10 lines), and the
`'\u{…}'` sub-branch (~7 lines). The 21-line doc comment is the cured-residual
record §12 required, not code.

**Does the size hide a defect?** No. The 411-file differential (13.1.3) is the
answer: the port and the reference agree on every line of every production file
except six, and those six are the `'\u{…}'` divergence, benign in the strict
direction. Two notes for the record, neither a finding: (a) the `'\u{` branch's
only bad case — an opener whose `}` and following `'` lie far away — is
unreachable on a tree `rustc` accepts and is not flagged by the belt; (b) the
self-test's fifteen rows omit `br#"…"#`, an `'outer:` label and `<'_>`, all
three of which I drove externally and all three of which pass.

### 13.8 Round-2 findings (three, all P3, all foldable)

**[R2-P3-1] A NEW wrong cite, introduced by the fold three lines above the one
it fixed.** `v1_lifecycle/tests/mod.rs:61` cites `mask_non_code` at
`check_checked_artifact_boundaries.py:1013-1076`. Measured: `def mask_non_code`
is at **`:1155`** and returns at **`:1225`**. `:1013` and `:1076` are an
unrelated allowlist entry (`…catalog_tests/grammar.rs`) and an unrelated
comment. The value matches NO revision — not base (`:984`), not main (`:992`),
not the pre-fold rebase (`:1134`). The fold's own message says the sibling cite
was "re-measured after the last checker edit"; this one was not. Fourth
occurrence of the E4.3-B [P3-5]/[P3-5R]/[P3-1] class in this file family. A doc
comment; nothing depends on it. **It must be re-measured at the rebase over
`3c632ec` in any case, since that rebase moves `mask_non_code` again.**

**[R2-P3-2] The checker's own provenance sentence is INVERTED.**
`check_checked_artifact_boundaries.py:462-463` reads:

> `store/archived.rs::archive` … **is named by the amendment, the charter prep
> and neither axis of its dual** (round 1 [P2-1]).

which asserts the opposite of the fact. The fold's commit message states it
correctly ("It is named by neither the amendment, the charter prep nor either
axis"), and so does the amendment's landed bracket (9) ("that §1, the E4.4
charter prep and both axes of this amendment's dual all missed"). A dropped "by
neither … nor". It inverts a load-bearing provenance fact at the site a future
reader consults first. One-line fold.

**[R2-P3-3] The quiet-shape count is stated three ways, and the origin is
mine.** The masker doc (`:68`) says "the four QUIET shapes"; the fold's commit
message says "all five formerly quiet live shapes fire"; the number actually
driven, in round 1 and again here, is **six** — M-a, M-c, M-b2, M-h, M-i, M-k.
My round-1 report is the source of the drift: its verdict line said "four
quiet-failure shapes" while its own table listed six, and a sub-head said "three
further QUIET shapes" over four rows. **The correct number is six.** The commit
message is immutable; the doc comment at `:68` is the home for the correction.

### 13.9 Verdict

Everything §12 ruled MUST CURE is cured, and the two cures I could attack hardest
survive the attack. [P1-1] is not patched but replaced: a scanner has no state to
desynchronise, the 411-file differential now shows **zero** real blinded lines
against the reference (from 178 at `b99bfb7` and 5 at the inherited `60072a7`),
the belt fires by name on a real read and false-positives on nothing, and
round 1's live evasion is RED. [P2-1]'s twentieth row recounts clean and drives
in all four directions. [P2-2] is byte-identical with its carrier named. Every P3
fold and every §10.4 resolution landed as authorised, with B2/B3's re-tensed
forward references true. All companions re-measured by me and reproduced.

Three P3s remain, all one-line edits, one of which the lane owner must touch
anyway during the rebase over `3c632ec`.

**GO-WITH-CONDITIONS.** Conditions, foldable by the lane owner at landing, no
third round:

1. **[R2-P3-1]** Re-measure the `mask_non_code` cite at
   `v1_lifecycle/tests/mod.rs:61` on the final rebased tree — it is wrong today
   (`:1013-1076` vs the actual `:1155-1225`) and the GC-fix rebase moves the
   function again.
2. **[R2-P3-2]** Repair the inverted provenance sentence at
   `check_checked_artifact_boundaries.py:462-463` to "is named by NEITHER the
   amendment, the charter prep NOR either axis of its dual".
3. **[R2-P3-3]** Correct "four QUIET shapes" to six at
   `v1_lifecycle/tests/mod.rs:68`.

Reviewer note for the record: no P0/P1/P2 stands, so no escalation fires and
nothing returns to the operator on this axis.
