# R2-E Phase E4 Step E4.3-B — single-axis peer-blind review (Opus, interior tier)

**Step.** E4.3-B — the record-root carve-out's PINS package (gwz-core half),
chartered by `GwzM5-8R2E-RecordRootAmendment.md` §7 on the amendment's dual GO:
P-1 (the O13 pin row made PERMANENT-DOCUMENTED), P-2 (the negative tripwire),
and four companions. No converted code.

**Candidate.** ONE commit `60072a760dfff3d018951105b5e62f6431487263`
("R2-E E4.3-B: pin the record-root exception (gwz-core half)") on branch
`e4/e4-3b-record-root`.
**Base.** gwz-core main `7f28907` (E4.2's landing).
**Worktree.** `…/scratchpad/e4-e43b-wt`. `git status --porcelain` empty on
entry and on exit at `60072a7` (verified both times). Every probe was applied
to the worktree and reverted with `git checkout --` or an inverse `mv`; the
tree was re-verified pristine after each block and once more at the end.
**Environment.** darwin 25.6.0 / arm64. Boundary checker driven with
`/opt/homebrew/bin/python3.13`. Direct exit codes throughout.
`CARGO_TARGET_DIR` = the shared `…/scratchpad/e4-e43-target` — **the shared-tree
hazard was handled per mandate**: `touch src/lib.rs`, rebuild, copy the produced
lib test binary to `.review-snap/gwz_core_snap` inside the worktree, verify it
with `--list` (**1831 tests**, both `record_root_exception` rows present), run
every partition from the snapshot, delete the snapshot at exit. The checker
suite's own `…/scratchpad/target/checked-boundary-probe` was created, used, and
**deleted** (`ls` confirmed absent immediately after). No other target directory
was created by this review. **Note for the operator:** at 06:45, minutes after
my deletion, that path was re-created by a *different* concurrent run — at exit
a live `cargo-clippy 1.95.0` on `/var/folders/…/tmp2zf36a4k/gwz-core` is
writing into it, and it is none of my processes (all three of mine had
completed). I left it alone rather than break another lane's in-flight suite.
Free disk at exit: 12 GiB.
**Date.** 2026-09-02. **Round 1 of 2.**

---

## VERDICT

# GO-WITH-CONDITIONS

The package does what it was chartered to do, and it does it fail-closed. I
re-ran **all eight** claimed red probes and fired **eleven more of my own**;
every one of the eight fires, and every message names the amendment and the
direction. P-1's generalization is **real at the data layer** — a second
exception row is literally one line and is accepted cleanly, with both
directions reporting to that row's *own* amendment (probes B1/B2/B3). P-2's
anti-vacuity is not decoration: the file-count floor, the exact positive
control, the by-name region panic and the `create_dir_all` containment
assertion each fire under a targeted probe. Item 5's declination is **correct**
— I verified the structural ground in the tree — and the positive bound the
builder substituted **does** discharge amendment §2's "Acknowledged latent …
P-2 bounds it" clause, driven in both of its halves. Every companion measured
green on my own runs, from a verified snapshot binary. The cap is 249 against
250. No rider. No trailers. `cargo fmt --check` clean.

**One [P1] and two [P2] fire, so escalation fires.**

The [P1] is the sharpest thing I found and it is **not a defect in this
commit**. The sentence the lane owner instructed the builder to delete — the
"digest backstop" for `store/rewrite.rs` — is **true**, and the "CORRECTED
2026-09-02" bracket now folded into the **ADOPTED** RR §3 saying it is false is
**driven false**. `store/rewrite.rs` *is* digest-pinned at main, transitively,
through `PROTECTED_SOURCE_TREE_DIGESTS["workspace_ops/merge/v1_lifecycle/mod.rs"]`,
because `source_tree_digest` on a `mod.rs` root digests the whole parent
subtree. I reproduced main's pinned value byte-for-byte from main's 129-file
`v1_lifecycle` tree, showed the value moves when `store/rewrite.rs` alone is
perturbed, and — decisively — every one of my three P-1 source-side probes
emitted `protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs`
alongside the carved finding. The instruction should be **withdrawn**, not
executed; executing it would put a false statement into the checker.

**Conditions:**

* **C1** — [P2-1], the builder's: fold `managed.rs:44-47` to what is true under
  the 2026-09-02 ruling. The delivered text ("shrink as E4.4-E4.6 convert …
  E4.4 is the next mover") is stale on arrival against the ruling that says
  E4.4-E4.6 do **not** start as chartered and that the archive arms are carved
  out permanently. De-staling that reason was this site's entire object.
* **C2** — [P2-2], the builder's: the "one data row each" generality claim is
  refuted by my own probe. Two record-root-specific clauses leak verbatim into
  every future carved row's message, and 11 of the 13 carve-outs CapFree §3
  foresees are outside this checker's scan root entirely. Either parameterize
  the two clauses (two string edits, zero net lines) or soften the claim in the
  checker comment and the commit message.
* **C3** — [P1-1], the lane owner's: withdraw the delivery instruction, and
  correct RR §3's "CORRECTED 2026-09-02" bracket and CapFree §3's parenthetical.
  The checker comment at `:366` may stay as written; sharpening it to name the
  mechanism ("via the `v1_lifecycle/mod.rs` TREE digest, which covers the whole
  subtree") would prevent the same confusion recurring.

None of the three is a code defect. The mechanism ships correct.

---

## FINDINGS

### [P1-1] The adopted "`store/rewrite.rs` is NOT digest-pinned at main" correction is driven false; the sentence it condemns is true

**The claim under review.** RR §3 P-1 now reads, on disk, with this bracket
folded in:

> *[CORRECTED 2026-09-02 by `GwzM5-8R2E-CapabilityFreeAmendment.md` §3's dual:
> `store/rewrite.rs` is NOT digest-pinned at main — in neither
> `PROTECTED_SOURCE_DIGESTS` nor `PROTECTED_SOURCE_TREE_DIGESTS`; P-1's counts
> and P-2's tripwire are its only pins, and the std::fs class is bounded by
> P-2's `create_dir_all` clause, not by a digest]*

CapFree §3 sources it: *"the 'refreshable tree digest backstop' sentence of RR
§3 P-1 is likewise false at main for `store/rewrite.rs`: its only pins are the
O13 counts and the `create_merge_store_record` entry-reference row."*

**The mechanism.** `check_checked_artifact_boundaries.py:1057-1073`:

```python
def source_tree_digest(source: Path, root_relative: str) -> str:
    root_file = source / root_relative
    descendant_root = (
        root_file.parent if root_file.name == "mod.rs" else root_file.with_suffix("")
    )
    paths = {root_file}
    if descendant_root.is_dir():
        paths.update(path for path in descendant_root.rglob("*") if path.is_file())
```

For a root whose file name is `mod.rs`, `descendant_root` is **the parent
directory**. `workspace_ops/merge/v1_lifecycle/mod.rs` therefore digests the
entire `v1_lifecycle/` subtree — `store/rewrite.rs` included, and
`tests/store/record_root_exception.rs` included.

**Measurement 1 — main's pinned value reproduces from a tree containing
`store/rewrite.rs`.** Reconstructed from git objects at `7f28907`, no working
tree involved:

```
7f28907 1b36abfea784ee1b6cfaa35d601d9bcee442c1240183ecfd61c9db8a6461676d files= 129 rewrite.rs in tree: True
60072a7 c99ae927dd0c33033f4e81c870d7ccc097eb9a90ecde66cb02b4907783d29cc7 files= 130 rewrite.rs in tree: True
```

`1b36abfe…` is exactly the value pinned at main. It is a function of
`store/rewrite.rs`'s bytes.

**Measurement 2 — perturbing `store/rewrite.rs` alone at main moves it.**

```
main tree digest recomputed : 1b36abfea784ee1b6cfaa35d601d9bcee442c1240183ecfd61c9db8a6461676d
main PINNED value           : 1b36abfea784ee1b6cfaa35d601d9bcee442c1240183ecfd61c9db8a6461676d
MATCH                       : True

with a std::fs::rename swap INSIDE store/rewrite.rs at main:
  perturbed digest          : 7c505e4333409f82e2f4ecb882126ca54d5556a85c67d309c2a54130a3cad809
  differs from the main pin : True
```

**Measurement 3 — the live checker says so, unprompted.** Every one of my three
P-1 source-side probes (D1/D2/D3, transcripts below) produced **two** findings,
the first being:

```
- protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs
```

That is the digest backstop firing on a `store/rewrite.rs` edit, end to end.

**What is true.** `store/rewrite.rs` is not in `PROTECTED_SOURCE_DIGESTS` (the
per-file table — I enumerated all 17 keys; it is absent). It **is** covered by
`PROTECTED_SOURCE_TREE_DIGESTS`, via the `v1_lifecycle/mod.rs` tree root. The
correction conflates the two tables. RR §3's original sentence — "a std::fs-direct
writer on the carved path is backstopped by the refreshable tree digest
(`PROTECTED_SOURCE_TREE_DIGESTS`, property-free — every edit is noticed, not
classified)" — was **correct as written**, and names the right table.

**Consequence if left standing.** A future builder reading the adopted
amendment will believe the carved path has no byte-level pin and will either
build the "optional hardening" property pin the amendment says is not required,
or — worse — will believe a `std::fs`-direct publication on `commit` could land
unnoticed. It cannot: the tree digest refuses every byte.

**Scope note, on the record.** This is a defect in the authority, not in the
commit. `check_checked_artifact_boundaries.py:366` states the true thing. I
cannot see the builder's two lane-owner messages, so I record only the outcome
— the citation is present and true — and make no finding about whether the
conflict was surfaced.

**Cure (lane owner's).** Withdraw the delivery instruction. Replace RR §3's
bracket with a dated note that the backstop is the `v1_lifecycle/mod.rs` TREE
digest (not a per-file entry), and correct CapFree §3's parenthetical, which is
otherwise right about the other twelve carved files.

---

### [P2-1] `managed.rs:44-47` is stale on arrival against the 2026-09-02 ruling, and cites neither it nor the amendment that carries it

**Delivered** (`src/checked_artifact/bootstrap/managed.rs:43-49`):

```rust
#[allow(
    unused_imports,
    reason = "Step 3.3 wired the coordinator caller; REACHED from the entry point at R2-E E4.2 \
              (2026-09-01) — the unconsumed imports shrink as E4.4-E4.6 convert (2026-09-02, \
              R2-E E4.3-B: E4.3 converts nothing, its rewrite path being permanently carved out \
              by GwzM5-8R2E-RecordRootAmendment.md §2, so E4.4 is the next mover)"
)]
```

**Claim-by-claim against `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`:**

| Claim | Status |
| --- | --- |
| "E4.3 converts nothing" | **TRUE** (RR §2/§7; no production code moves in this commit) |
| "its rewrite path being permanently carved out by … §2" | **TRUE** |
| "the unconsumed imports shrink as E4.4-E4.6 convert" | **FALSE as stated.** The ruling: *"Do not start E4.4–E4.6 as originally chartered. After the amendment GO: pins package + any remaining activated-lease forward arms only."* |
| "so E4.4 is the next mover" | **FALSE as stated.** CapFree §3 carves `store/archive.rs` and `archive.rs::remove_archive` out permanently; its `:275` row reads "nothing — no forward arm". There is no E4.4 conversion candidate — the lane's own reading of the ruling records that "nothing is parked beyond that report". |

The whole object of touching this site was to *de-stale* the reason ("shrink as
E4.3-E4.6 convert" → E4.4-E4.6, RR §7). The package delivers a differently
stale one, of exactly the shape CapFree §7 warned about: *"its builder is told
the `managed.rs:44-47` range wording … so its landing is not stale on arrival."*

**Why P2 and not P3.** Same class as the E4.2 round-2 fix (`3717249`, "revert
the provider.rs allow reason to its true 7214010 text"): a committed
`reason = ` string that asserts something the record contradicts. It is also
the only companion whose *sole purpose* was accuracy at that string.

**Cure (≤3 lines, inside cap).** Name the ruling and scope the shrink to what
the ruling actually leaves convertible — the remaining **activated-lease
forward arms** — and drop "E4.4 is the next mover".

---

### [P2-2] "One data row each" is refuted by probe: two record-root clauses leak into every future row, and 11 of 13 foreseen carve-outs are out of scan scope

**The claim**, in two places. Commit message: *"general by design, so the
further `:275`-`:279` carve-outs the 2026-09-02 capability-free ruling foresees
are one data row each."* Checker comment `:361-364`: *"general by design: the
2026-09-02 capability-free ruling foresees further `:275`-`:279` carve-outs,
one data row each naming its own amendment."*

**Probe B1 — a second exception row is accepted cleanly.** Added
`store/archive.rs` with its own authority string; nothing else touched:

```
--- B1 accept ---
checked-artifact boundary: ok (18 visible entries, 8 classified modules)
EXIT=0
```

**Probe B2 — SHRANK on the second row.** Second exception row + its pin moved
to `sync_dir: 8` (source has 7):

```
- permanent writer exception (the capability-free archive carve-out, dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md §3),
  workspace_ops/merge/v1_lifecycle/store/archive.rs: the raw-writer count SHRANK
  (expected={'rename_noreplace': 2, 'sync_dir': 8} actual={'rename_noreplace': 2, 'sync_dir': 7})
  -- a PARTIAL conversion may not land. That path is PERMANENT-DOCUMENTED: revise its amendment
  first -- the conversion is re-decided there (for the record root, at O14's fork), not by a commit
  that moves this pin
EXIT=1
```

**Probe B3 — GREW on the second row.** Pin moved to `sync_dir: 6`:

```
- permanent writer exception (the capability-free archive carve-out, dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md §3),
  workspace_ops/merge/v1_lifecycle/store/archive.rs: the raw-writer count GREW
  (expected={'rename_noreplace': 2, 'sync_dir': 6} actual={'rename_noreplace': 2, 'sync_dir': 7})
  -- the exception blesses `commit`'s existing publication primitive only, not a new raw writer.
  That path is PERMANENT-DOCUMENTED: revise its amendment first -- the conversion is re-decided
  there (for the record root, at O14's fork), not by a commit that moves this pin
EXIT=1
```

**So: the answer to the generalization question is YES at the data layer and NO
at the prose layer.** The row carries its own authority, both directions fire,
the operational instruction ("revise its amendment first") is correct for any
row. But two clauses are hard-coded to the record root and are emitted verbatim
for a row that has nothing to do with it:

1. **"the exception blesses `commit`'s existing publication primitive only"** —
   `store/archive.rs` has no `commit`. The message names the wrong function.
2. **"(for the record root, at O14's fork)"** — the archive carve-out's
   re-decision point is DR-1, not O14's fork.

**And the scan-scope half.** `V1_LIFECYCLE_TREE = "workspace_ops/merge/v1_lifecycle"`
(`:313`) is the scan root, gated by `re.search(r"\bdurable_fs\b", text)`
(`:1223`). CapFree §3 enumerates 13 carved files and states three named
mechanism changes are needed — "(i) an EXPLICIT carved-file list under `src/`
replaces the `v1_lifecycle/` scan root; (ii) the `\bdurable_fs\b` population
gate is dropped — most carved files never name it; (iii) the token vocabulary
widens to all three primitive classes". Only `store/archive.rs` and `archive.rs`
are inside this scan root. **"One data row each" is therefore true for 2 of the
13 carve-outs and false for the other 11.**

**Cure.** Either parameterize the two clauses (fold the re-decision point into
the per-row value, or reword generically — "the exception blesses the carved
path's EXISTING publication primitive only" and "each row's amendment names its
own re-decision point"), which is two string edits at zero net lines; or narrow
the claim in both the checker comment and the commit message to the two v1
archive rows.

---

### [P3-1] Loop-1's carved branch is unreachable under the module-scope guard

`:1240-1246` reads:

```python
    for relative in sorted(set(raw_writer_files) - expected_files):
        findings.append(
            carved_finding(relative, "its pin row was DELETED while the file still names durable_fs")
            if relative in carved
            else …
```

`relative in carved` and `relative not in expected_files` is precisely the
state the module-scope assert at `:382` forbids. **Probe C1** (pin row deleted,
exception row kept) confirms the assert preempts it:

```
AssertionError: a permanent writer exception names no pinned row
EXIT=1
```

The branch is defence-in-depth against someone deleting the assert too, which
is defensible — but a maintainer reading it will believe it is a live path.
One comment clause ("unreachable while the `:382` guard stands; kept as the
second belt") closes it.

### [P3-2] The module-scope `assert` is a corpus novelty, names neither the key nor the amendment, and is `-O`-strippable

It is the **only** `^assert ` in the whole of `scripts/checks/` (measured). Its
message — "a permanent writer exception names no pinned row" — does not name
the offending key (a reader must diff) and does not name the amendment, unlike
every other message this package adds. It is also stripped under `python -O` /
`PYTHONOPTIMIZE`; the gate runner uses `PY = sys.executable` with no `-O`, so
this is latent, not live. A `findings.append(...)` in `check()` or a
`raise SystemExit` would be house-consistent and would carry the key.

### [P3-3] P-2's negative half errs QUIET, not loud — the inverse of the house pin's trade — and the direction is undocumented. Driven.

The mandate asked for one evasion the builder did not try. Two, plus the answer
to the macro question.

`code()` strips `//` to end of line with no string awareness:

```rust
.map(|line| line.split_once("//").map_or(line, |(kept, _)| kept))
```

**Probe E5 — THE EVASION SUCCEEDS.** Appended to `src/checked_artifact/entry.rs`:

```rust
pub(crate) fn evade() { let _hint = "see https://gwz.invalid/x"; let _ = rewrite_merge_store_record; }
```

```
test result: ok. 2 passed; 0 failed; 0 ignored; 0 measured; 1829 filtered out
```

The door is literally present in a production source and the tripwire is
green. The `//` inside `"https://…"` truncates the line before the needle.

**Probe E6 — the loud direction, for contrast.** `/* legacy: pub(crate) fn
rewrite_merge_store_record() {} */` appended to the same file **fires**
(block comments are not stripped) — a false positive, which is the safe side.

**Probe E7 — the macro / line-continuation question.** Appended a
`concat!("rewrite_merge_store", "_record")` macro and a `"rewrite_merge_store\` +
`_record"` line-continued literal: both invisible (`2 passed`). **This one is
harmless**, and the reason matters: stable Rust has no `concat_idents!`, so
neither `concat!` nor a string continuation can produce a *callable identifier*.
A real call to the door must spell the name contiguously. Fail-loud therefore
holds against every genuine call shape — including `use … as`, fn-pointer
binding and turbofish, all of which name the item at least once.

**Why this is P3 and not P2.** The threat the tripwire exists to stop — a real
E4.3 conversion of `commit` — is caught independently by three belts, any one
of which suffices: (a) P-1's exact counts (probe D2 fires on exactly that
swap); (b) the `v1_lifecycle/mod.rs` TREE digest, which refuses every byte of
`store/rewrite.rs` (see [P1-1]); (c) `entry.rs`'s per-file byte pin in
`PROTECTED_SOURCE_DIGESTS`. The tripwire's marginal contribution is the stated
PROPERTY, and it is that property — not any belt — which the evasion dents.

**Why it still needs a line.** The house precedent this idiom comes from,
`catalog_activation_pin.rs:69-77`, **names its trade explicitly** ("`/* … */`
blocks, inline `#[cfg(test)] mod tests` and off-convention test files are
scanned — all the loud direction"). There the strip errs LOUD. Here, on the
absence half, the same strip errs **QUIET** — the trade inverted, and the new
module's doc does not say so. Measured surface today: **12 lines across 4
production files** (`git/git_host.rs`, `operation/operation_runtime.rs`,
`status/workspace_path.rs`,
`workspace_ops/workspace_bootstrap/claude_settings.rs`) carry a `//` inside a
string literal, so a live blind region already exists.

**On flag 5 ("same trade as the house pin"): I do not concur.** It is the
opposite trade on the half that matters, and it should be recorded as a named
residual in the module doc.

### [P3-4] The walk is strictly narrower than `production_rust_files` by 17 files — documented, sound today, unpinned

The mandate asked me to confirm the exclusions match `production_rust_files`
**exactly**. They do not, by design. Measured:

```
python house rule files: 428
rust walk files        : 411
in python NOT in rust  : 17 files, every one a *_tests.rs
in rust NOT in python  : []
```

The walk adds `name.ends_with("_tests.rs")`, which the module doc declares
("extended with the `_tests.rs` stem it misses"). I checked all 17 declaration
sites: **every one is `#[cfg(test)]`-gated** (four at `provider.rs:313-320`,
two at `commit_log/mod.rs:507-510`, nine at `validate/mod.rs:49-66`, two via
`#[cfg(test)] #[path = "…_tests.rs"]` at `participant_semantics/`). So the
narrowing is sound today and the direction is right (a door in a `tests*` file
is legitimately invisible; a door in any *genuine* production file is caught —
probes E1/E2 confirm both the near and far case). The residual: nothing pins
the 17 to `cfg(test)`, so a future non-test `*_tests.rs` would be an invisible
hole that the house rule would have scanned. The floor (411 → 350) does not
catch a 1-file drift.

### [P3-5] Stale cite, made stale by this very commit

`record_root_exception.rs:42-44` cites `production_rust_files` at
`check_checked_artifact_boundaries.py:942-949`. That is its range **at main**;
this commit's own checker insertions moved it to **`:974-981`**. Verified both
by `git show 7f28907:` and at HEAD. (The house precedent
`catalog_activation_pin.rs:69` carries an even older cite, `:829-836`, so
there is a pattern — but a cite invalidated by the same commit is the cleanest
kind of miss.)

### [P3-6] Flag 4 — mixed moves classify as SHRANK and suppress the GREW clause; I concur the message stays *sufficient*, not *unambiguous*

`shrank = any(actual.get(w, 0) < c for w, c in counts.items())` — any single
writer shrinking wins. **Probe C4** (expected `{rename_durable: 3, sync_dir: 1}`,
actual `{rename_durable: 2, sync_dir: 2}` — one down, one up):

```
… the raw-writer count SHRANK (expected={'rename_durable': 3, 'sync_dir': 1}
actual={'rename_durable': 2, 'sync_dir': 2}) -- a PARTIAL conversion may not land. …
```

The label is one-sided and the "not a new raw writer" clause is suppressed in
exactly the scenario where a conversion swapped one writer for another. But the
finding prints **both complete maps**, so the evidence is not lost, and
`carved_finding`'s tail — the operative instruction — is identical in both
branches. It fails closed, loudly, with the right file and the right authority.
**Disposition: accept as P3.** If it is ever touched, `"MOVED"` with both
directions named would be strictly better than picking one.

### [P3-7] §7's declined `create_dir_all` → refusal option leaves no record of the declination in the tree

The ground is sound (see the item-3 analysis below) and the substitute bound is
real. But amendment §2 tells a future reader "E4.3-B may replace it with a
refusal (§7)", and the tree records only the bound, not the choice. The test's
assertion message carries the *latency* argument ("race-only code that
`read_regular` at the head of `commit` proves unreached in every driven
behavior") but not the *declination* argument (structurally undrivable, so a
refusal would ship unexercised). One clause on that assertion closes it.

### [P3-8] The `commit` bypass denylist is not exhaustive while its message says "and nothing else"

```rust
for bypass in ["fs::rename(", "fs::write(", "fs::copy("] {
    …"the exception carves out `durable_fs::rename_durable` + `sync_dir` and nothing else"
```

A publication via `File::create(path)` + `write_all`, `fs::OpenOptions`,
`fs::hard_link` or a symlink is not in the list and would not move the
`durable_fs` counts either — so neither P-1 nor P-2 would see it. It is caught
by the `v1_lifecycle/mod.rs` tree digest, which sees every byte. The message's
"nothing else" overstates what *this assertion* proves; it is true of the
belt-set as a whole.

### [P3-9] The floor comment overstates the floor

```rust
/// 411 production files at this landing; fewer means a lost subtree.
const PRODUCTION_FILE_FLOOR: usize = 350;
```

"Fewer" fires at 350, not 411 — up to 61 files can vanish unseen by the floor.
The **exact** positive control (`LANDED_DOOR` named by exactly `entry.rs` +
`store/rewrite.rs`) covers the case that matters, since any blinding that
removes either control file fires. The 411 is measured correct today (I
reproduced it independently). Standard house floor idiom; noted for precision
only.

---

## PROBE TRANSCRIPTS

All probes run against the worktree at `60072a7`, each reverted immediately;
`git status --porcelain` re-checked between blocks and empty at exit.

### P-1 — the four claimed directions, source-side (D1-D3) and pin-side (C1)

**D1 — count UP.** A second `sync_dir(path.parent()…)` added to `commit`:

```
checked-artifact boundary: failed
- protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs
- permanent writer exception (the record-root exception, dev-docs/GwzM5-8R2E-RecordRootAmendment.md §2/§3),
  workspace_ops/merge/v1_lifecycle/store/rewrite.rs: the raw-writer count GREW
  (expected={'rename_durable': 2, 'sync_dir': 2} actual={'rename_durable': 2, 'sync_dir': 3})
  -- the exception blesses `commit`'s existing publication primitive only, not a new raw writer.
  That path is PERMANENT-DOCUMENTED: revise its amendment first -- the conversion is re-decided
  there (for the record root, at O14's fork), not by a commit that moves this pin
EXIT=1
```

**D2 — count DOWN.** `commit`'s `rename_durable(&temporary, path, true)` →
`std::fs::rename(&temporary, path)`:

```
- protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs
- permanent writer exception (the record-root exception, …RecordRootAmendment.md §2/§3),
  …store/rewrite.rs: the raw-writer count SHRANK
  (expected={'rename_durable': 2, 'sync_dir': 2} actual={'rename_durable': 1, 'sync_dir': 2})
  -- a PARTIAL conversion may not land. That path is PERMANENT-DOCUMENTED: …
EXIT=1
```

**D3 — `durable_fs` gone entirely.** `use crate::durable_fs::{…}` removed and
both call sites rewritten:

```
- protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs
- permanent writer exception (the record-root exception, …RecordRootAmendment.md §2/§3),
  …store/rewrite.rs: it no longer names durable_fs AT ALL -- a conversion of it may not land.
  That path is PERMANENT-DOCUMENTED: …
EXIT=1
```

**C1 — pin row DELETED (exception row kept).** Fires, but through the guard,
not through the finding the code appears to intend (see [P3-1]):

```
AssertionError: a permanent writer exception names no pinned row
EXIT=1
```

**C2 — pin row AND exception row both deleted** (the wholesale-retirement
shape). Fires, generically, without naming the amendment:

```
- v1 lifecycle gained a raw durable_fs writer outside the O13 accepted residual:
  workspace_ops/merge/v1_lifecycle/store/rewrite.rs
EXIT=1
```

**C3 — pin-side count down** (`rename_durable` pinned at 3, source has 2):
SHRANK, amendment named. **C4 — mixed move:** see [P3-6].

### P-1 — the two extra directions the mandate asked for

**Probe A — an exception key naming no pinned row.** Added
`"…/store/nonexistent_row.rs"` to the exceptions map:

```
File ".../check_checked_artifact_boundaries.py", line 383, in <module>
    assert V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS.keys() <= V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES.keys(), …
AssertionError: a permanent writer exception names no pinned row
EXIT=1
```

**Probes B1/B2/B3 — the second exception row.** Transcripts under [P2-2].

### P-2 — the four claimed directions

**E1 — door added to `checked_artifact/entry.rs`:**

```
… panicked at src/workspace_ops/merge/v1_lifecycle/tests/store/record_root_exception.rs:154:5:
production sources name the checked rewrite door `rewrite_merge_store_record` at
{"checked_artifact/entry.rs"} — the rejected E4.3 conversion, whose detach-then-publish window
no shipped reconciler closes. GwzM5-8R2E-RecordRootAmendment.md §2 must be revised, at O14's
fork, before it may exist
test result: FAILED. 1 passed; 1 failed
```

**E2 — door added FAR AWAY (`git/gitbackend/repository_support.rs`):** same
message, `at {"git/gitbackend/repository_support.rs"}`, FAILED.

**F1 — `commit`'s `rename_durable` → `std::fs::rename`** (rebuilt; `include_str!`
binds at compile time):

```
… panicked at …record_root_exception.rs:100:5:
the record's rewrite no longer publishes by atomic in-place replacement — the carved-out path of
GwzM5-8R2E-RecordRootAmendment.md §2, where a detach-then-publish shape leaves a window in which
no shipped discovery path finds the open merge
test result: FAILED. 1 passed; 1 failed
```

**E4 — scan root blinded.** `src/checked_artifact` (107 files) moved aside,
411 → 304; restored by inverse `mv` and re-verified green:

```
… panicked at …record_root_exception.rs:141:5:
the production scan reached 304 files, under the 350 floor: a subtree is unreachable, so the
absence below would be an artefact of the blinding
test result: FAILED. 1 passed; 1 failed
```

### P-2 — anti-vacuity, probed rather than asserted

**F2 — region lookup panics BY NAME.** `pub(super) fn commit(` →
`pub(super)  fn commit(` (double space; still compiles):

```
… panicked at …record_root_exception.rs:57:28:
store/rewrite.rs no longer declares `pub(super) fn commit(`
test result: FAILED. 1 passed; 1 failed
```

**F3 — `create_dir_all` file-wide count 1 → 2** (a second one added inside
`commit`):

```
… panicked at …record_root_exception.rs:118:5:
assertion `left == right` failed: the rewrite path's parent-creation surface moved;
row `:274`'s clause still binds
test result: FAILED. 1 passed; 1 failed
```

**F4 — the one `create_dir_all` MOVED OUT of `create_temporary`** (count still
1, containment broken):

```
… panicked at …record_root_exception.rs:123:5:
the one admitted `create_dir_all` — race-only code that `read_regular` at the head of `commit`
proves unreached in every driven behavior — left `create_temporary`'s shape
test result: FAILED. 1 passed; 1 failed
```

**Not probed, reasoned:** "a vanished subject is a COMPILE error" —
`include_str!("../../store/rewrite.rs")` is resolved by rustc at compile time;
removal or rename is a hard build failure by construction. Driving it would
have required a full-crate red build for no additional information.

**Self-exclusion, both belts verified.**

* Belt 1 (the `tests/` directory): the walk never descends it. Confirmed by the
  positive control itself — under the **Python house rule**,
  `create_merge_store_record` is named by exactly
  `['checked_artifact/entry.rs', 'workspace_ops/merge/v1_lifecycle/store/rewrite.rs']`,
  while a whole-tree scan finds a **third** namer, the tripwire file. If belt 1
  ever failed, `LANDED_DOOR_FILES`'s exact set equality would fire. The
  positive control doubles as a self-exclusion tripwire — a good property, and
  worth stating in the module doc, where it currently is not.
* Belt 2 (the split needle): `grep -c "rewrite_merge_store_record"` over
  `record_root_exception.rs` returns **0**. The joined door never appears in the
  file's own text; nor anywhere else under `src/` at HEAD.

**CRLF normalization at every read: confirmed.** Both reads pass through
`code()`, which does `.replace("\r\n", "\n")` before `.lines()`. It is
load-bearing for `body()`, whose terminator is the literal `"\n}\n"` — on a
CRLF checkout that is `"\r\n}\r\n"` and the region lookup would panic
"scan unbounded". The module doc's claim that normalization matters "because
both halves are region-scoped and multi-line" is accurate.

---

## MANDATE ITEM 3 — item 5's ground, and whether the substitute discharges §2

**The structural claim, verified in the tree.** At HEAD (line numbers shifted
+7 by this commit's `///` block; the amendment's `:89`/`:103`/`:171` are
main-relative and correct there):

* `read_regular(path)?` at **`rewrite.rs:96`**
* `create_temporary(path)?` at **`rewrite.rs:110`**
* `fs::create_dir_all(parent)` at **`rewrite.rs:178`**, inside `create_temporary`

Between `:96` and `:110` lie only: `from_open_bytes`, the `same_source_as`
guard, `verify_known_diff`, `to_value`, `unknown::overlay`, `to_string`. **No
fault hook.** `CommitFault` has exactly two variants (`store/mod.rs:17-20`),
`AfterTemporarySync` and `AfterPublish`, consulted only at `:133` and `:143` —
both *after* `create_temporary`. There is no thread-local or global injection
hook anywhere under `v1_lifecycle/store/` (grepped:
`fail_next_checked_artifact_at`, `CheckedArtifactFault`, `thread_local` — zero
hits).

**Verdict on the ground: SOUND.** `read_regular` proves the record — hence its
parent — resident, and no in-process mechanism can remove `.gwz/merge` between
that proof and the `create_dir_all`. A refusal substituted there would ship
permanently unexercised, which is a worse pin than a driven bound. Declining
the option was the better call, and it was explicitly the builder's to make
(§7: "at the builder's option").

**Verdict on the discharge: YES, in both halves, driven.** §3 P-2 requires that
"the latent `create_dir_all` stays bounded to `create_temporary`'s shape on the
commit path". The package asserts exactly that, in two independent ways, and
both fire red:

* file-wide count `== 1` on the comment-stripped source — **probe F3** fires
  when a second one appears;
* the one occurrence lies inside `body(&rewrite, "fn create_temporary(")` —
  **probe F4** fires when it is moved out while the count stays 1.

Neither assertion is satisfiable vacuously: `body()` panics by name (probe F2)
and the subject is `include_str!`-bound. §2's "Acknowledged latent … P-2 bounds
it" clause is **discharged**.

---

## MANDATE ITEM 5 — the commit doc sentence

`src/workspace_ops/merge/v1_lifecycle/store/rewrite.rs:77-83`, 7 lines,
immediately above `pub(super) fn commit(`. Claim by claim:

| Claim | Status |
| --- | --- |
| Dated `2026-09-02` | TRUE |
| Cites `GwzM5-8R2E-RecordRootAmendment.md` §2 | TRUE — RR §2 is where the exception text lives |
| "operator-authorized" | TRUE — RR status block, "proceed with (c)" |
| "the ROOT of reconciliation, which recovers from nothing **in the shipped tree**" | TRUE, and correctly carries §2's post-[P2-1] narrowing — it does **not** revive the overstated "no reconciler CAN close" |
| "opens a discovery-dead window **no shipped reconciler** closes (§1a, driven)" | TRUE, same narrowing preserved |
| "keeps `rename_durable(replace = true)` + `sync_dir` permanently" | TRUE against the code (`:139`, `:146`) |
| "pinned both ways by the O13 row and `tests/store/record_root_exception.rs`" | TRUE — probes D1/D2 (both ways) and E1/E2/F1 |
| "re-examined at O14's fork" | TRUE per RR §2 (now carrying a dated bracket re-pointing the fork to DR-1; the `///` predates that bracket and is not falsified by it) |

**Does it cite a "digest backstop"? NO.** The `///` is clean.

**Does any other comment? YES —** `check_checked_artifact_boundaries.py:365-366`:

> "class scope is `durable_fs` only, a std::fs writer here being backstopped by
> `PROTECTED_SOURCE_TREE_DIGESTS` and stated as a property by P-2."

That is the sentence the instruction says must not be present. **It is true**
(see [P1-1], three measurements). My recommendation is C3 — withdraw the
instruction — not that the builder remove a correct statement.

---

## COMPANIONS — all measured, none taken on trust

| Companion | Method | Result |
| --- | --- | --- |
| Moved tree digest `v1_lifecycle/mod.rs` | Re-executed the checker's own `source_tree_digest` on the worktree | `c99ae927dd0c33033f4e81c870d7ccc097eb9a90ecde66cb02b4907783d29cc7` — **matches the pin** |
| "The other six recomputed unchanged" | Recomputed all 7 entries | **7/7 OK**, six unchanged from main |
| "`bootstrap/managed.rs` is under NO entry — measured" | Enumerated each tree root's covered path set; checked all 17 `PROTECTED_SOURCE_DIGESTS` keys | **TRUE.** Covered by no tree digest, absent from the per-file table |
| Checker green on pristine | `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | `checked-artifact boundary: ok (18 visible entries, 8 classified modules)`, **EXIT=0** (run twice: on entry and after every probe was reverted) |
| **Checker UNITTEST SUITE (the builder's flag 2 — closed)** | `python3.13 -m unittest -v` on `test_check_checked_artifact_boundaries`, in its own `…/scratchpad/target/checked-boundary-probe` (deleted by me on completion; see the environment note) | **`Ran 69 tests in 700.107s` — `OK`, exit 0.** Includes the 30+ `cargo +1.95.0 clippy` compiler probes on mutated tree copies |
| `v1_lifecycle::` 260 → 262 | Snapshot binary, `workspace_ops::merge::v1_lifecycle:: --skip root_fault_matrix` | **262 passed; 0 failed** (484.42s) |
| `checked_artifact::` 457 UNMOVED | Snapshot binary | **457 passed; 0 failed** (66.48s) |
| lib remainder 1110 + 1 ignored UNMOVED | Snapshot binary, both skips | **1110 passed; 0 failed; 1 ignored** (92.33s) |
| Partition disjointness | Arithmetic against the snapshot's `--list` | `262 + 457 + 1110 + 1 ignored + 1 (root_fault_matrix, run separately in release) = 1831` = the binary's own total. **Exact** |
| Dated provenance block, house form | Read against the four preceding blocks | **In form.** Dated, step-named, darwin MEASURED, linux DERIVED + cfg-AUDITED, FIRST-DISPATCH-EXPECTED, "A measured number wins", unmoved partitions recorded as re-MEASURED |
| cfg-audit claim ("no `cfg` gate on either row; both read source text checked out everywhere and CRLF-normalize it") | Read both rows | **TRUE.** Only `#[test]`; one `include_str!` (compile-time), one `env!("CARGO_MANIFEST_DIR")` walk with `replace('\\', "/")` for Windows separators |
| `cargo fmt --check` | Direct | **clean, exit 0** |

**Snapshot verification, as mandated:** `--list` on the copied binary reported
`1831 tests`, with both rows present:

```
workspace_ops::merge::v1_lifecycle::tests::store::record_root_exception::the_checked_rewrite_door_is_absent_from_production_sources: test
workspace_ops::merge::v1_lifecycle::tests::store::record_root_exception::the_record_root_rewrite_publishes_by_atomic_rename_and_creates_no_parent: test
```

Every partition count above was produced by executing that snapshot directly,
never through `cargo test`.

---

## CAP, RIDER AUDIT, TRAILERS

**Cap: 249 / 250. PASS**, with one line of headroom. `git diff --numstat
7f28907 60072a7`:

| File | + | − |
| --- | --- | --- |
| `scripts/checks/check_checked_artifact_boundaries.py` | 63 | 5 |
| `scripts/checks/run_r4bg_aggregate_gates.py` | 14 | 1 |
| `src/checked_artifact/bootstrap/managed.rs` | 3 | 1 |
| `src/workspace_ops/merge/v1_lifecycle/store/rewrite.rs` | 7 | 0 |
| `src/workspace_ops/merge/v1_lifecycle/tests/store.rs` | 1 | 0 |
| `src/workspace_ops/merge/v1_lifecycle/tests/store/record_root_exception.rs` | 161 | 0 |
| **TOTAL** | **249** | **7** |

Basis: whole handwritten added lines; nothing here is generated (the one digest
hex is a modified line inside the 63, and excluding it would only lower the
count).

**Rider audit: NONE.** Six files, each mapping to a §7 charter leg — P-1's
checker re-shape + the digest re-pin; the aggregate-gate pin + provenance;
`managed.rs`'s re-dated reason; the one `///` sentence; the `mod` declaration;
P-2's module. `store/rewrite.rs`'s diff is **+7 lines of `///` and nothing
else** — the "no production code moves" bound holds exactly. §7's optional
salvage (the unknown-fields independent-reread row) was not taken; that is
within the option.

**On flag 3 (prose compressed for cap): no load-bearing *explanation* is lost,
but three things a reader needs at the site are absent** — and all three are
findings above, so I record the disposition here rather than as a fourth:
the unreachability of loop-1's carved branch ([P3-1]); the direction of the
comment-strip trade, which its own house precedent documents ([P3-3]); and the
ground for declining §7's refusal option ([P3-7]). Each is one clause. There is
one line of cap headroom, so the fixes and C1/C2 together will need the lane
owner's cap disposition — which is the ordinary shape at a pins package and not
itself a finding.

**Trailers: NONE.** Commit message swept for `co-authored`, `claude`,
`anthropic`, `noreply`, `generated with`, `🤖`, `assistant` — zero hits. Added
diff lines swept for the same — zero hits. Author and committer both
`Gianni Mariani <gianni@everbility.com>`.

---

## FLAG DISPOSITIONS

| Flag | Disposition |
| --- | --- |
| **1** — shared `CARGO_TARGET_DIR` served the other tree's binary | **HONOURED, not independently reproduced.** I did not attempt to re-create the mis-serve (that would mean driving the other lane's in-flight tree, which is not mine to touch); I ran the full mandated protocol instead: `touch src/lib.rs` → rebuild → copy to `.review-snap/` → `--list` verify (1831, both rows present) → run every partition from the snapshot → delete. The hazard's precondition is visible in the tree — both worktrees are the same package name and produce `debug/deps/gwz_core-<hash>` under one shared `.fingerprint` — and my own build did emit that exact path. Good catch by the builder; without it a count could silently attest the wrong tree. |
| **2** — checker unittest suite skipped for disk | **CLOSED BY ME. `Ran 69 tests in 700.107s — OK`, exit 0**, at this sha, in the suite's own probe target dir, which is now deleted. Disk recovered to 13 GiB during the session, so the constraint that forced the skip no longer bound. |
| **3** — prose compressed for cap | **PARTIALLY UPHELD.** No explanation is lost that changes the mechanism's meaning; three one-clause gaps are recorded as [P3-1], [P3-3], [P3-7]. See the cap section. |
| **4** — shrink-vs-grow when both move | **UPHELD as raised; still sufficient, not unambiguous.** Probe C4 transcript at [P3-6]. Fails closed with both maps printed and an identical operative instruction; the label is one-sided and suppresses the GREW clause. P3. |
| **5** — comment-strip caveat, "same trade as the house pin" | **I DO NOT CONCUR.** The trade **inverts** on the absence half: the house pin's strip errs loud, this one errs quiet, and I drove the evasion (probe E5 — the door present in `entry.rs`, tripwire green). It is not a named residual today. [P3-3], with the belt analysis that keeps it at P3. |
| **6** — (not identifiable from the tree; I did not have the builder's two lane-owner messages) | Taking it to be the §7 `create_dir_all` → refusal declination: **GROUND VERIFIED SOUND, SUBSTITUTE DISCHARGES §2** — see mandate item 3. The only residual is that the declination itself is unrecorded, [P3-7]. If flag 6 was something else, it is unaddressed and should come back in round 2. |

---

## THE GENERALIZATION ANSWER (mandate item 1, stated plainly)

**Is the generalization real? Yes at the data layer; no at the prose layer.**

Real: adding a carve-out is literally one dictionary line, the module-scope
guard refuses a row that names no pinned row (probe A), the checker stays green
with two rows (probe B1), and both directions on the *second* row fire with
that row's own amendment named and the correct operative instruction (probes
B2/B3). That is exactly what the ruling's "generalize the E4.3-B / O13
inventory shape" asks for, and it works.

Not real: the finding text hard-codes two record-root specifics —
"`commit`'s existing publication primitive" and "(for the record root, at
O14's fork)" — which are emitted verbatim for any other row, naming a function
that row does not have and a re-decision point that is not its own. And the
mechanism's *reach* is `V1_LIFECYCLE_TREE` gated on `\bdurable_fs\b`, so only
2 of CapFree §3's 13 foreseen carved files are addressable by "one data row";
the other 11 need the three mechanism changes CapFree §3 already enumerates.
The commit message's and the checker comment's "one data row each" therefore
overstates. That is [P2-2] / condition C2, and the cure is two string edits.

---

## CLOSING

Worktree left byte-clean at `60072a7` (`git status --porcelain` empty).
`.review-snap/` deleted. The checker suite's probe target directory deleted by
me and subsequently re-created by another concurrent run, which I left running
(see the environment note). No commits, no pushes, no tags, no trailers.
Nothing was written into the worktree that survives this review.

**Round 1 of 2. Escalation fires (1 P1, 2 P2).**

---

# ROUND 2 — text-diff confirmation on the fold

**Candidate.** ONE commit `2b0eed9` ("E4.3-B round-1 fold: the reason made
ruling-true, the messages made row-generic, the quiet half named") on top of the
reviewed `60072a7`, branch `e4/e4-3b-record-root`.
**Worktree.** Same `…/scratchpad/e4-e43b-wt`, `git status --porcelain` empty on
entry and on exit at `2b0eed9`.
**Scope.** Text-diff confirmation of the round-1 conditions and P3 folds, plus
the two amendment corrections at the gwz-dev root, plus a rider audit on the
+24. Re-runs were held to the cheap, decisive ones (checker, guard probes,
digest recomputation, one snapshot-verified build); the partition counts are the
builder's, reasoned-through below rather than re-measured. Target directory
still shared with the GC builder — the one build I ran used the mandated
snapshot protocol (`touch src/lib.rs` → build → copy → `--list` verify → run →
delete).
**Date.** 2026-09-02. **Round 2 of 2 — terminal.**

## VERDICT

# GO

Every round-1 condition is discharged and every P3 fold lands. Two of the folds
are stronger than what I asked for. Two things are worth the operator's eye and
neither blocks: the fold **re-broke [P3-5]** (the same cite, moved again by the
fold's own insertions), and the CapFree §3 correction — the lane owner's,
uncommitted — **over-reaches by two files**. Both are recorded below as landing
nits, [P3-5R] and [P3-10]. No new P0/P1/P2. No rider in the +24. No trailers.

## THE AMENDMENT CORRECTIONS — confirmed exact (with one over-reach)

**RR §3's withdrawal note: EXACT.** I checked all five of its factual claims
against my round-1 measurements and re-verified each on this tree:

| Claim in the withdrawal note | Status |
| --- | --- |
| tree-digest roots named `mod.rs` digest their whole parent subtree | TRUE (`source_tree_digest:1057-1060`) |
| `…/v1_lifecycle/mod.rs`'s digest covers `store/rewrite.rs` | TRUE, re-verified |
| main's pinned value reproduces from the tree containing it | TRUE (`1b36abfe…` from 129 files, R1) |
| perturbing `store/rewrite.rs` alone moves the digest | TRUE (R1) |
| the live checker fires `protected source tree changed: …/v1_lifecycle/mod.rs` | TRUE (R1 probes D1/D2/D3) |
| "The sentence above is CORRECT AS WRITTEN … the backstop is that TREE digest" | TRUE |
| `store/rewrite.rs` is absent from `PROTECTED_SOURCE_DIGESTS` | TRUE (all 17 keys enumerated) |
| the withdrawn bracket conflated the per-file table with the tree table | TRUE — that is exactly the error |

The note also correctly attributes the error to the CapFree Code axis's [P2-5]
rather than inventing a fresh cause. Nothing to add.

**The checker's `:366`/`:367` sentence was never removed.** Verified present at
`2b0eed9`: `# scope is 'durable_fs' only, a std::fs writer here being backstopped by`
… `PROTECTED_SOURCE_TREE_DIGESTS`. Nothing to restore.

### [P3-10] CapFree §3's correction over-reaches by two files (lane owner's, uncommitted)

The corrected parenthetical reads:

> *"…so `workspace_ops/merge/v1_lifecycle/mod.rs`'s digest ALREADY backstops
> every v1 carved file — `store/rewrite.rs`, `store/archive.rs`, `archive.rs`,
> `store/gc.rs`, `store/retention.rs`…"*

Measured on this tree:

```
store/rewrite.rs       exists=True  covered_by_v1_lifecycle_mod_digest=True
store/archive.rs       exists=True  covered_by_v1_lifecycle_mod_digest=True
archive.rs             exists=True  covered_by_v1_lifecycle_mod_digest=True
store/gc.rs            exists=False covered_by_v1_lifecycle_mod_digest=False
store/retention.rs     exists=False covered_by_v1_lifecycle_mod_digest=False
```

`gc.rs` and `retention.rs` are not under `v1_lifecycle/` at all — they live at
`src/workspace_ops/merge/store/`, the **v0** store:

```
workspace_ops/merge/store/gc.rs:        tree-pinned-by=NONE  flat-pinned=False
workspace_ops/merge/store/retention.rs: tree-pinned-by=NONE  flat-pinned=False
```

So the digest backstops **three** of the named five, and the two it does not
cover are pinned by *nothing* — the opposite of what the sentence asserts, and
the same class of path conflation as the error it corrects, pointing the other
way. The correction's own next clause already provides the right home: "the
NON-v1 carved files' tree coverage is enumerated by the pins package per file
(flat-pinned / tree-pinned by which root / unpinned)". **Cure:** move
`store/gc.rs` and `store/retention.rs` out of the "already backstopped" list
into that per-file enumeration, and say three, not five. Uncommitted document;
does not touch the gwz-core package.

## PER-ITEM CONFIRMATION

**[P2-1] — DISCHARGED.** `managed.rs:43-53` as landed:

```rust
reason = "Step 3.3 wired the coordinator caller; REACHED from the entry point at R2-E E4.2 \
          (2026-09-01). The unconsumed imports shrink only as the remaining ACTIVATED-LEASE \
          forward arms convert (E4.5/6-B) — per the operator's 2026-09-02 ruling, \
          GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md: no E4.4-E4.6 as chartered, \
          activated-lease forward arms only; E4.3's rewrite path (RecordRootAmendment §2) \
          and the capability-free arms (CapabilityFreeAmendment §3) are permanent by decision"
```

Checked against the ruling's own text: "Convert only arms already on
`acquire_activated`" and "Do not start E4.4–E4.6 as originally chartered. After
the amendment GO: pins package + any remaining activated-lease forward arms
only." Every clause is ruling-true; the ruling is named by filename; both
amendments are named; **"E4.4 is the next mover" is gone**; "E4.5/6-B" matches
CapFree §3's own `:277`/`:278`/`:279` rows. *One observation, not a finding:*
the `CapabilityFreeAmendment §3` cite is to a document still in dual, so a
section renumber before its adoption would stale this reason a third time. The
load-bearing cite (the ruling) is adopted, so this is a landing re-check, not a
defect.

**[P2-2] — DISCHARGED, re-probed.** Both leaked clauses are now row-generic. On
a simulated second exception row (`store/archive.rs`, its own authority string):

```
… the raw-writer count GREW (…) -- the exception blesses the carved path's EXISTING
publication primitive only, not a new raw writer. That path is PERMANENT-DOCUMENTED:
revise its amendment first -- the conversion is re-decided there (each row's amendment
names its own re-decision point), not by a commit that moves this pin
```

and the SHRANK direction likewise. No `commit`, no O14 — the messages are clean
for any row. The claim is narrowed exactly as asked: "one data row each **AT THE
DATA LAYER** — each row carries its own authority and both directions fire
naming it; widening the SCAN SET to the non-v1 carved files is the pins
package's (amendment §3 (i)-(iii))", with the matching narrowing on the
exceptions-map comment. That is a true statement of what I measured in round 1.

**[P3-1] — DISCHARGED.** The second-belt branch now carries
"Unreachable while the module-scope guard above stands (an exception key must
sit on a pinned row); kept as the second belt if that guard goes."

**[P3-2] — DISCHARGED, and stronger than asked. House-consistent: yes.**

```python
for _key in sorted(V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS.keys() - V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES.keys()):
    raise SystemExit(
        f"check_checked_artifact_boundaries: permanent writer exception {_key!r} "
        f"({V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS[_key]}) names no pinned row -- "
        "an exception must sit on a row of V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES"
    )
```

Driven with a bogus key under three interpreter modes:

```
--- plain python --- EXIT=1
--- python -O     --- EXIT=1
--- python -OO    --- EXIT=1
check_checked_artifact_boundaries: permanent writer exception
'workspace_ops/merge/v1_lifecycle/store/bogus_row.rs' (a bogus carve-out naming no pinned row)
names no pinned row -- an exception must sit on a row of V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES
```

**`-O`-proof: confirmed.** It names the key, its authority string, and the table
the key must sit on — all three gaps closed. **House-consistent: confirmed** —
`SystemExit`/`sys.exit` is the idiom in four other scripts
(`check_m4_scenario_map.py`, `check_merge_docs.py`,
`check_merge_compatibility_predicates.py`, `run_r4bg_aggregate_gates.py`,
`test_release_boundary.py`), whereas the `assert` was the corpus's only one. And
the module-scope raise is safe: no gate imports this checker — grep for
`import check_checked_artifact_boundaries` across `scripts/` returns nothing;
the unittest suite runs it as `subprocess.run([sys.executable, str(SCRIPT), …])`.
So the raise can only ever be a subprocess exit, never an importer kill.

**[P3-3] — DISPOSITION ACCEPTED. My axis does NOT require the masking in this
package.** The site now reads:

> NAMED RESIDUAL (E4.3-B review [P3-3]): this strip is string-unaware, so a door
> on a line whose earlier `//` sits inside a string literal (e.g. "https://…") is
> INVISIBLE to the absence half — the strip errs QUIET here, the inverse of the
> house pin's loud trade. The real threat (a conversion of `commit`) is caught
> independently by P-1's counts, the `v1_lifecycle/mod.rs` tree digest and
> `entry.rs`'s byte pin; the pins package's shared scan helper masks string
> literals before stripping (the checker's `mask_non_code` idiom).

Three reasons the routing is right, and the cap is the least of them:

1. **What round 1 asked for is exactly what landed.** The finding was that the
   trade had inverted and was *unnamed* — less documented than its own house
   precedent on the one point where it diverged. It is now named, with the
   direction and the three belts, at the line that causes it. That is the house
   form.
2. **A correct masker is not a line or two.** Rust needs `"…"`, `'c'`, escapes,
   and raw strings (`r#"…"#`) handled, or the "cure" introduces its own quiet
   bug. That is a real component — and buying it here, for a property already
   held by three independent belts, is the wrong trade at a pins package.
3. **The pins package is its correct owner, not a convenience.** CapFree §3
   mandates a per-carved-file negative scan in this same idiom for thirteen
   files, two of them region-scoped mixed files. One shared masker there serves
   all thirteen; a bespoke one here would be duplicated and would then diverge —
   precisely the failure mode `catalog_activation_pin.rs`'s un-shared copy
   already demonstrates.

**Recommendation, not a condition** (round 2 is terminal): the lane owner should
record the shared masker as a *named obligation on the pins package*, not only
as a forward reference in this file's comment. A promise that lives only in the
prose of the package that declined it is the weaker of the two enforcement
shapes — the same argument this program applies to settings-versus-instructions.

**[P3-5] — RE-BROKEN. New [P3-5R].** The fold corrected `:942-949` → `:974-981`,
but its own checker insertions (+8 net lines above that point) moved
`production_rust_files` again:

```
982:def production_rust_files(source: Path) -> list[Path]:   ← where it is at 2b0eed9
75:/// `production_rust_files` (`check_checked_artifact_boundaries.py:974-981`)   ← what the test cites
```

The correct range at `2b0eed9` is **`:982-989`**. Same class of miss, second
time, and self-inflicted by the same commit both times — a cite that must be
re-measured *after* the last edit to the cited file, not before. It is a doc
comment; nothing depends on it; and the checker is not edited again by the
landing, so `:982-989` is stable from here. **P3, landing nit.**

**[P3-7] — DISCHARGED.** The declination is now on the assertion itself:
"DECLINED as a refusal at E4.3-B because it is structurally undrivable (no fault
hook between `read_regular` and `create_temporary`, so a refusal would ship
unexercised)". That is the ground I verified in round 1, stated where a reader
of amendment §2 will look for it.

**[P3-8] — DISCHARGED.** "…and nothing else **in this denylist** (the
`v1_lifecycle/mod.rs` tree digest sees every byte)". The message no longer
claims more than the assertion proves, and names the belt that does prove it.

**[P3-9] — DISCHARGED.** "411 production files at this landing; the floor fires
at 350 — up to 61 files may vanish unseen by it, so the EXACT positive control
(`LANDED_DOOR` named by exactly its two files) is what catches the blinding that
matters." Exact, and it correctly re-points the reader at the control rather
than the floor.

**[P3-4] / [P3-6] — accepted as recorded, no fold. I CONCUR.** [P3-4] (the walk
is narrower than `production_rust_files` by 17 `*_tests.rs` files, all
`cfg(test)`-gated today) is a documented, sound narrowing whose only residual —
an unpinned invariant — is not this package's to pin. [P3-6] (mixed moves
classify as SHRANK) fails closed with both complete maps printed and an
identical operative instruction; picking one label is a presentation choice, not
a defect. Neither warranted spending cap.

## COMPANIONS RE-MEASURED

| Companion | Result |
| --- | --- |
| Checker on pristine `2b0eed9` | `checked-artifact boundary: ok (18 visible entries, 8 classified modules)`, **EXIT=0** |
| All 7 tree digests | **7/7 OK.** `v1_lifecycle/mod.rs` = `3f2e5ec3c096ab29e4523a5441ed94eb80dff0ae926d6caa9771b3fd6a87ec2e`, recomputed with the checker's own algorithm — **matches the new pin**; the other six unchanged |
| Digest movement is correctly caused | The only `src/` change under `v1_lifecycle/` is `tests/store/record_root_exception.rs`; `managed.rs` is outside every root (R1-measured). **[P1-1]'s mechanism firing exactly as proved** |
| Guard, three interpreter modes | EXIT=1 under plain / `-O` / `-OO` |
| Second-row genericity, both directions | Clean; no record-root leak |
| Compile + row count, snapshot-verified | Built, copied, `--list` = **1831 tests** with both `record_root_exception` rows; both rows **ok**. Row count unchanged from `60072a7`, so the `v1_lifecycle::` 262 pin cannot have moved |
| Partition counts | **Builder's, not re-measured** (v1 262 / CA 457 / remainder 1110+1, from a `--list`-verified snapshot). Reasoned confirmation: the test file's diff is a doc comment, an in-chain `//` comment, one cite and three assertion *message* strings — **no assertion added, removed, or re-predicated**, and no control flow touched. No partition can move |

## CAP AND RIDER AUDIT ON THE +24

**Cap: 273 / 273-MEASURED** (`git diff --numstat 7f28907 2b0eed9`: added 273,
removed 7). The fold is +37 / −13 = **net +24**, matching the re-ruling.

**Rider audit: NONE.** All twelve hunks map to a round-1 condition or P3:

| File | Hunk | + / − | Owner |
| --- | --- | --- | --- |
| checker | tree digest value | 1/1 | forced by the test edit (§ the digest row above) |
| checker | O13 comment, "at the data layer" + scan-set | 3/1 | [P2-2] |
| checker | exceptions-map comment narrowing | 2/1 | [P2-2] |
| checker | `assert` → `SystemExit` | 6/1 | [P3-2] |
| checker | `carved_finding` re-decision clause | 1/1 | [P2-2] |
| checker | unreachable-branch annotation | 2/0 | [P3-1] |
| checker | GREW clause made generic | 1/1 | [P2-2] |
| managed.rs | allow reason | 5/3 | [P2-1] |
| test | floor doc | 3/1 | [P3-9] |
| test | `code()` named residual | 7/0 | [P3-3] |
| test | cite | 1/1 | [P3-5] (re-broken, [P3-5R]) |
| test | denylist message | 2/1 | [P3-8] |
| test | declination clause | 3/1 | [P3-7] |

Nothing else moved. No production semantics move in the fold — `store/rewrite.rs`
is untouched by it, and `managed.rs`'s change is a `reason = ` string.

**Trailers: NONE.** `2b0eed9`'s message and added lines swept for `co-authored`,
`claude`, `anthropic`, `noreply`, `generated with`, `🤖`, `assistant` — zero
hits. Author and committer `Gianni Mariani <gianni@everbility.com>`.

## CLOSING

Worktree byte-clean at `2b0eed9`. The one build I ran used the snapshot
protocol and its snapshot is deleted. No commits, no pushes, no tags, no
trailers.

**Round 2 of 2 — TERMINAL. GO.** Two landing nits for the lane owner, neither
blocking: **[P3-5R]** the cite is `:982-989` at `2b0eed9`, not `:974-981`; and
**[P3-10]** CapFree §3's correction names five v1 carved files where only three
are under the `v1_lifecycle/mod.rs` root — `store/gc.rs` and
`store/retention.rs` are v0-store files at `workspace_ops/merge/store/`, pinned
by nothing, and belong in the per-file enumeration the same sentence already
promises.
