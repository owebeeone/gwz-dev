# R2-F R1.2 — the A1 activation tripwire — single-axis peer-blind review

**Date:** 2026-09-01
**Axis:** single-axis, peer-blind (builder's report not seen, not sought; the
delivery claims were taken as claims to falsify, not as evidence)
**Tree under review:** gwz-core `eb606ef5f3ab50306e9eb9e5b80094e5042a48ab`
(branch `r2f/r1-2-tripwire`), one commit
**Base:** gwz-core main `ea3a924` (`chore(release): gwz-core 0.12.1`)
**Charter:** `dev-docs/GwzM5-8R2F-RelocationPlan.md` §3 R1.2 (`:138-142`)
**Reviewer worktree:**
`.../scratchpad/r2f-r12-wt`, on `r2f/r1-2-tripwire`. Four transient probe edits
were made (three evasion probes plus one anti-vacuity probe); all were reverted
with `git checkout --`, and `git status --porcelain` is empty and
`git rev-parse HEAD` is `eb606ef…` at open and at close. Nothing was committed;
no tag created, moved or deleted; no branch created or re-pointed.
**Host:** Darwin 25.6.0 / arm64; cargo 1.95.0 (f2d3ce0bd 2026-03-21), rustc
1.95.0 (59807616e 2026-04-14); Python 3.13.12
(`/opt/homebrew/bin/python3.13`). `CARGO_TARGET_DIR` =
`.../scratchpad/r2f-r12-review-target`.

**Verdict: NO-GO.** 0 [P0] / **1 [P1]** / 2 [P2] / 4 [P3].

The step's own purpose is stated in its module doc: *"Nothing FIRES when a
caller appears, so the gate is a fact about the tree rather than a check on
it. This file is that check: first activation becomes loud instead of
silent."* That purpose is not met. Three of the four natural spellings of a
production activation call — fully-qualified, module-qualified, and aliased —
pass the tripwire **green**, verified by probe with three real, compiling
call sites in `src/checked_artifact/admission/driver.rs` (§2). The
`::`-prefix exclusion, which
exists to skip one line of the owner's own internal delegation
(`catalog/bootstrap.rs:236`), buys that one suppression at the cost of every
qualified call site in the whole production subtree. A tripwire that a
`use`-statement defeats is not a tripwire.

Everything else in the delivery holds up. The visibility premise is true, the
scan's file set is right, the four `_tests.rs` exclusions are real
`#[cfg(test)]` modules, the over-eagerness claims are true, the anti-vacuity
anchor fires and teaches, the negative probe replicates exactly as claimed,
the darwin pin 447 → 448 is MEASURED true by me on this tree, the linux
DERIVED block conforms to the driver's docstring convention, the diff touches
exactly three files, and the commit message carries no trailer of any kind.
The remedy for the [P1] is small — three independently-verified one-clause
shapes are given in §2.4, all of which keep the pin at zero on this tree —
which is why this is a NO-GO with a cheap round-1 fix rather than a terminal
one.

---

## Contents

- §1 What was verified, in one table
- §2 [P1-1] — the evasion hole, with probe transcripts
- §3 [P2-1] — the exhaustiveness premise is documented but unasserted
- §4 [P2-2] — the "58/60" cap figure is not on the house LOC basis
- §5 [P3] findings
- §6 Mandate item dispositions (one line each)
- §7 Findings summary table
- §8 Verdict
- Appendix A — every command run, with its result
- Appendix B — what was not run, and why

---

## 1. What was verified, in one table

| Claim (delivery) | Verified? | Evidence |
| --- | --- | --- |
| One commit, 3 files, +118/−1 | YES | `git show --stat eb606ef` |
| New `interface_tests/catalog_activation_pin.rs`, 100 lines | YES | file read |
| `PRODUCTION_CALLER_COUNT = 0`, `DEFINITION_COUNT = 2` | YES | `:30`, `:35` |
| Scan excludes definitions (`fn ` prefix) and qualified calls (`::` prefix) | YES — **and that second exclusion is the [P1]** | `:70-74` |
| House production rule extended by the `_tests.rs` stem; the two provider test modules were misclassified | YES (four files, not two — see §5 [P3-3]) | `provider.rs:313-320`; `boundaries.py:829-836` |
| Over-eager in the failing direction (inline `cfg(test)` mods, off-convention test files counted) | YES | `namespace/test_support.rs` and six inline `#[cfg(test)] mod tests {` blocks are scanned |
| Partition pin 447 → 448 darwin MEASURED | YES — **I measured 448 independently** | Appendix A, cmd 1 |
| 457 → 458 linux DERIVED, dated block, driver convention | YES | `run_r4bg_aggregate_gates.py:268-282` |
| Negative probe RED with a teaching message | YES — replicated | §2.3 |
| 58/60 handwritten | Arithmetic reproduces on a code-lines basis; **the basis is not the house one** — see §4 | §4 |
| No scope smuggled | YES | `git show --stat` — exactly the three files |
| Commit message trailer-free | YES | no `Co-Authored-By`, no `Generated with`, no AI attribution of any form |

Independently confirmed tree facts underpinning the above:

- `src/checked_artifact/catalog/bootstrap.rs:233`
  `pub(in crate::checked_artifact) fn recover_or_create(` — the free function.
- `src/checked_artifact/catalog/bootstrap.rs:236`
  `CatalogOwnerV1::recover_or_create(lease)` — the internal delegation, the
  **only** thing the `::` exclusion suppresses anywhere in the tree.
- `src/checked_artifact/catalog/bootstrap.rs:240` `fn recover_or_create(` —
  module-private inherent method (no visibility modifier) in
  `impl CatalogOwnerV1`.
- `src/checked_artifact/catalog.rs:35-37` re-exports it at
  `pub(in crate::checked_artifact)`; `mod bootstrap;` at `catalog.rs:16` is
  private.
- Repo-wide `grep -rn recover_or_create` (excluding `.git`, `target`) finds
  **zero** occurrences outside `src/checked_artifact/` and two `scripts/`
  files, so the scan root `src/checked_artifact` is the right root **today**
  (see §3 for why "today" is load-bearing).
- `mod interface_tests;` at `src/checked_artifact/mod.rs:132-133` is
  `#[cfg(test)]`, so the new test lands under the `checked_artifact::`
  partition and under no `cfg` OS gate — the linux `+1` cfg-independence
  claim is sound.
- The house boundary gate is green on this tree
  (`check_checked_artifact_boundaries.py` → `ok (15 visible entries, 5
  classified modules)`, exit 0) and it excludes `interface_tests/` entirely,
  so the new file carries no companion obligation there.

---

## 2. [P1-1] The `::`-prefix exclusion lets any qualified or aliased production caller evade the tripwire

**Severity: P1.** The step exists to make first activation loud. Under the
shipped scan, activation is loud only if the author happens to write the
call unqualified. `bootstrap::recover_or_create(lease)`,
`catalog::recover_or_create(lease)`,
`crate::checked_artifact::catalog::recover_or_create(lease)`, `super::…`,
`Self::…`, and `use … as rc; rc(lease)` are all silent.

### 2.1 The mechanism

`catalog_activation_pin.rs:69-75`:

```rust
for (offset, _) in code.match_indices("recover_or_create(") {
    if code[..offset].ends_with("fn ") {
        *definitions += 1;
    } else if !code[..offset].ends_with("::") {
        sites.push(format!("{}:{number}", path.display()));
    }
}
```

`ends_with("::")` does not distinguish *which* qualifier precedes the call.
The owner's inherent-method receiver (`CatalogOwnerV1::`) and an arbitrary
module path (`catalog::`, `crate::checked_artifact::catalog::`) are the same
string suffix. Separately, an aliasing import
(`use …::recover_or_create as rc;`) never produces the literal
`recover_or_create(` at the call site at all, so the scan never sees it —
and the `use` line itself has no `(` after the name, so it is not seen
either.

I re-implemented the shipped rule in Python over the same tree to establish
what the exclusion is actually buying:

```
DEFINITIONS (shipped rule): 2
    src/checked_artifact/catalog/bootstrap.rs:233  pub(in crate::checked_artifact) fn recover_or_create(
    src/checked_artifact/catalog/bootstrap.rs:240  fn recover_or_create(
SITES (shipped rule, `::` excluded): 0
SITES (if `::` exclusion dropped entirely): 1
    src/checked_artifact/catalog/bootstrap.rs:236  CatalogOwnerV1::recover_or_create(lease)
```

So the exclusion suppresses exactly **one** line in the entire production
subtree — the owner's own delegation — and in exchange blinds the pin to
every qualified call anywhere.

### 2.2 PROBE A/B — three real production call sites, tripwire GREEN

Applied to `src/checked_artifact/admission/driver.rs` (a production file, not
excluded by the scan, and the single most plausible real future activation
site — it is the admission driver). All three are genuine, type-checking
call sites: the crate **compiled**.

Probe text as applied (`driver.rs:233-258`):

```rust
// ===== REVIEW PROBE A/B (qualified + alias evasion) — TO BE REVERTED =====
use crate::checked_artifact::bootstrap::CatalogMutationLeaseV1;
use crate::checked_artifact::catalog;
use crate::checked_artifact::catalog::recover_or_create as rc;

#[allow(dead_code)]
pub(super) fn probe_fully_qualified(
    lease: CatalogMutationLeaseV1<'_>,
) -> Result<OpaqueRetainedCatalogV1<'_>, CheckedFsError> {
    crate::checked_artifact::catalog::recover_or_create(lease)   // :242
}

#[allow(dead_code)]
pub(super) fn probe_module_qualified(
    lease: CatalogMutationLeaseV1<'_>,
) -> Result<OpaqueRetainedCatalogV1<'_>, CheckedFsError> {
    catalog::recover_or_create(lease)                            // :249
}

#[allow(dead_code)]
pub(super) fn probe_aliased(
    lease: CatalogMutationLeaseV1<'_>,
) -> Result<OpaqueRetainedCatalogV1<'_>, CheckedFsError> {
    rc(lease)                                                    // :256
}
// ===== END REVIEW PROBE =====
```

Transcript:

```
$ cargo test --lib -p gwz-core checked_artifact::interface_tests::catalog_activation_pin
   Compiling gwz-core v0.12.1 (.../r2f-r12-wt)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 11.60s
     Running unittests src/lib.rs (.../gwz_core-dba41e97d4500b7d)

running 1 test
test checked_artifact::interface_tests::catalog_activation_pin::the_catalog_owner_gains_its_first_production_caller_only_at_e4_1 ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 1802 filtered out; finished in 0.02s
```

**Three production callers of `recover_or_create` present and compiling; the
A1 activation tripwire GREEN.** This is the finding.

### 2.3 PROBE C — the same file, one unqualified call added: RED, and only that one site named

With Probe A/B still in place, one additional call site was added to the same
file, differing from the others **only in spelling**:

```rust
// ===== REVIEW PROBE C (unqualified control) — TO BE REVERTED =====
use crate::checked_artifact::catalog::recover_or_create;

#[allow(dead_code)]
pub(super) fn probe_unqualified(
    lease: CatalogMutationLeaseV1<'_>,
) -> Result<OpaqueRetainedCatalogV1<'_>, CheckedFsError> {
    recover_or_create(lease)                                     // :267
}
// ===== END REVIEW PROBE C =====
```

Transcript:

```
running 1 test
test checked_artifact::interface_tests::catalog_activation_pin::the_catalog_owner_gains_its_first_production_caller_only_at_e4_1 ... FAILED

thread '…the_catalog_owner_gains_its_first_production_caller_only_at_e4_1' panicked at
src/checked_artifact/interface_tests/catalog_activation_pin.rs:92:5:
assertion `left == right` failed: production catalog activation appeared at
["…/src/checked_artifact/admission/driver.rs:267"]. A1's coexistence gate forbids it until
the R2-F relocation lands, and E4.1 is the step that deliberately adds the first caller AND
moves PRODUCTION_CALLER_COUNT to 1 in that same reviewed commit. If you are E4.1, move the pin
with the caller; if you are not, the caller does not belong here yet
  left: 1
 right: 0

test result: FAILED. 0 passed; 1 failed; 0 ignored; 0 measured; 1802 filtered out
```

`left: 1` with **four** real call sites in the file. Same file, same module,
same compilation unit, same lease type: the only variable is the call
spelling, and three of four are invisible. This also discharges mandate item
4 — the negative probe replicates exactly as claimed, naming the site and
teaching what to do (though note the message names only the *one* site it
saw, which under the hole is actively misleading: it tells the reader "you
have one caller" when the tree has four).

Both probes reverted: `git checkout -- src/checked_artifact/admission/driver.rs`;
`git status --porcelain` empty; a re-run of the single test on the restored
tree is `ok` (Appendix A, cmd 8).

### 2.4 Remedy — three shapes, all verified to keep the pin at zero on this tree

Measured with the Python replica over the shipped tree:

```
ALT-1 exclude only the inherent receivers (`CatalogOwnerV1::` / `Self::`): 0 sites
ALT-2 exclude only the owner's own file (`catalog/bootstrap.rs`):          0 sites
ALT-3 no exclusion at all, pin at 1:  ['src/checked_artifact/catalog/bootstrap.rs:236']
```

- **ALT-1** is the smallest change — one clause:
  `else if !(code[..offset].ends_with("CatalogOwnerV1::") || code[..offset].ends_with("Self::"))`.
  It suppresses the delegation and nothing else; every module-path
  qualification is caught again. It does not close the *alias* hole.
- **ALT-2** excludes the owner's own file rather than a spelling. Also does
  not close the alias hole, and blinds the pin to a caller added inside
  `bootstrap.rs` itself.
- **ALT-3** keeps the scan spelling-blind and pins at 1 with the delegation
  named in the constant's doc comment. E4.1 then moves 1 → 2. Closes the
  qualified hole; not the alias hole.
- **ALT-4 (recommended — it is the house precedent's own mechanism):** drop
  call-site counting for a *file-set* pin in the exact shape of
  `admission/tests_namer_pin.rs`, which the module doc already cites as the
  idiom being followed. That precedent uses a plain `source.contains(item)`
  against a declared allowlist compared by set equality
  (`tests_namer_pin.rs:82-92`, `:108-125`). Applied here — "the production
  files that name `recover_or_create` at all are exactly {`catalog.rs`,
  `catalog/bootstrap.rs`} once `//` lines are stripped" — it catches **all
  three** probed evasions, because a qualified call, an aliased call and its
  `use` line all put the literal name in the file. This is also the axis on
  which the shipped file *departs* from the precedent it cites: the
  precedent is deliberately spelling-agnostic, and the new file reintroduces
  spelling-sensitivity and then subtracts a spelling.

Any of these is well inside a remediation round.

---

## 3. [P2-1] The exhaustiveness premise is documented but not asserted, so a visibility widening passes green

The module doc (`:12-17`) grounds the scan root on a visibility fact:

> *"the free function is `pub(in crate::checked_artifact)` … so outside
> `catalog/bootstrap.rs` the free function is the ONLY way to activate a
> catalog and no call can exist outside `src/checked_artifact/`."*

I verified that fact is **true on this tree** (`bootstrap.rs:233`;
`catalog.rs:35-37`; repo-wide grep finds nothing outside the subtree). But
the pin does not assert it. If E4.1 — or anything before it — widens the
free function to `pub(crate)` and places the caller in, say,
`src/workspace_ops/`, then:

- the scan root `src/checked_artifact` no longer covers the caller,
- `DEFINITION_COUNT` is still 2 (the definitions did not move),
- `PRODUCTION_CALLER_COUNT` is still 0,
- and the tripwire is **green while production activation is live**.

The anti-vacuity pin explicitly covers "a moved owner, a renamed root"; it
does not cover a *widened* owner, which is the more likely shape of the very
step (E4.1) that this pin is aimed at. Precedent has the same structural gap
(`tests_namer_pin.rs` also states its visibility premise in prose only), so
this is not a novel defect — but the premise is one line to assert:
read `catalog/bootstrap.rs` and require it still contains
`pub(in crate::checked_artifact) fn recover_or_create(`. That converts the
premise from documentation into part of the check, at roughly three lines.

## 4. [P2-2] "58/60" is computed on a basis the house's own measured precedent does not use

The delivery states 58/60 handwritten. That arithmetic reproduces on a
*code-lines* basis:

| File | added raw | blank | `//` comment | code |
| --- | --- | --- | --- | --- |
| `interface_tests/catalog_activation_pin.rs` | 100 | 6 | 37 | **57** |
| `interface_tests/mod.rs` | 1 | 0 | 0 | **1** |
| `scripts/checks/run_r4bg_aggregate_gates.py` | 17 | 2 | 14 (docstring) | **1** (a one-line edit of an existing line) |
| **total** | **118** | 8 | 51 | **58–59** |

The house LOC basis, however, is `GwzWtPlan.md:45-47` — *"handwritten incl.
tests, generated excluded"* — and the one place the program has actually
*measured* it (`GwzWt-P0.1-Review.md:456-463`) counts whole-file line
counts, comments included (`tests.rs` **3,014**). On that basis this step is
**118 added lines against a stated 60**, i.e. ~197%, well past §10.2's
120%-stop-and-report threshold, and `GwzWtPlan.md:45-46` makes HARD caps
stop-and-report boundaries.

Two things pull the other way and are why this is P2, not P1:

1. The plan's own charter text for this step reads *"gwz-core; cap 60"* —
   **not** "HARD cap 60" (contrast R1.1's explicit *"HARD cap 250 incl.
   tests"*). The review mandate calls it HARD; the plan does not. That
   discrepancy should be resolved in the record rather than papered over.
2. Nothing is smuggled. Every one of the 118 lines is chartered content:
   the test, the dated E4.1 comment the charter explicitly requires, the
   provenance block the charter explicitly requires, one `mod` line and one
   pin edit. There is no scope creep to find, and I looked.

Disposition: record the measured number (118 raw / 58–59 code) rather than
letting "58/60" stand as the ledger figure, and let the lane owner rule on
which basis governs a "cap 60" step. This does not by itself block.

## 5. [P3] findings

### [P3-1] `DEFINITION_COUNT` anchors the owner's file, not the scan's coverage

The anchor works and teaches — verified. PROBE D re-pointed the scan root
from `src/checked_artifact` to `src/checked_artifact/admission` (simulating
the doc's own "a moved owner, a renamed root"):

```
thread '…' panicked at src/checked_artifact/interface_tests/catalog_activation_pin.rs:87:5:
assertion `left == right` failed: the scan no longer reaches `recover_or_create`'s own
definitions, so a zero caller count would prove nothing; re-point it at the owner it is
meant to watch
  left: 0
 right: 2
```

It fires before the caller assertion, and the message names the right
remedy. Reverted; tree clean.

Two limits worth recording, neither blocking:

- It anchors only that **`catalog/bootstrap.rs`** is still reached. A future
  change that blinds the scan to some *other* subtree (a new exclusion, a
  directory rename) still yields `definitions == 2`, `sites == 0`, and a
  vacuous green over the blinded subtree. The anchor proves the scan found
  the owner, not that it walked the tree.
- The message teaches only the "reached nothing" case. The plainest
  legitimate refactor that moves this pin is *collapsing the thin free-function
  delegate into a `pub(in crate::checked_artifact)` inherent method* — which
  takes definitions 2 → 1 and produces a message ("the scan no longer
  reaches … re-point it") that misdescribes what happened. A second sentence
  covering "or the owner's definition count changed; update
  `DEFINITION_COUNT` with it" would close that.

### [P3-2] Comment handling is asymmetric, and the doc does not say so

`code.starts_with("//")` strips only comments that begin a trimmed line.
A trailing comment (`let x = 1; // recover_or_create(y)`) and any line inside
a `/* … */` block are scanned, so a mere textual mention becomes a "call
site". That is the *loud* direction, which the file's stated policy accepts —
but the policy paragraph (`:41-44`) claims over-eagerness only for
`cfg(test)` modules, not for comments. Since three production files already
mention `recover_or_create` inside `//` comments today
(`capability/pre_catalog/provider/interior.rs:199`,
`…/publication.rs:321`, `admission/driver.rs:91`), a future reflow of any of
those into a block comment turns the tripwire red for no reason. One clause
in the doc comment, or `mask_non_code`-style stripping, resolves it.

### [P3-3] The `_tests.rs` extension is load-bearing and correct, but it forks "production" in-tree

Verified true and necessary: the four `_tests.rs` files under
`capability/pre_catalog/provider/` are exactly `catalog_tests.rs`,
`directory_mutation_tests.rs`, `mutation_tests.rs`, `production_tests.rs`,
all declared `#[cfg(test)] mod` at `provider.rs:313-320`, and two of them
(`mutation_tests.rs:76`, `directory_mutation_tests.rs:44`) hold real
`recover_or_create(` calls. Without the extension the pin would ship RED. So
the extension is not cosmetic.

Two record-only notes: the delivery says *"two provider test modules were
misclassified"* — it is **four** files (two of which happen to hold the
calls); and the house `production_rust_files`
(`check_checked_artifact_boundaries.py:829-836`) is left un-extended, so the
tree now carries two different definitions of "production Rust file", one of
them file-local. The divergence is at least stated in the file's own doc
comment, which is why this is P3 and not higher.

### [P3-4] The R1.2 provenance block does not state provenance for the partitions it says are unmoved

The block (`run_r4bg_aggregate_gates.py:268-282`) conforms to the driver's
docstring convention on everything it moves: it names the step and date,
names the moving partition and the exact test path, gives darwin MEASURED
with the number and date, gives linux DERIVED with a cfg-independence
argument, marks FIRST-DISPATCH-EXPECTED, and closes with *"A measured number
wins."* Verified conforming and verified true (448 measured, §A cmd 1).

The one place it is thinner than its siblings: it asserts *"The lib remainder
is untouched and keeps 1097/1098; v1_lifecycle:: keeps 256"* without saying
those were re-measured. Most sibling blocks in the same docstring do say so
("with the remainder re-measured unchanged at 937 + 1 ignored and
`workspace_ops::merge::v1_lifecycle::` at 256 in the same run" — `:198-201`,
and likewise `:141-145`, `:161-163`, `:177-180`, `:218-221`, `:261-263`).
The claim is structurally sound (the new test is under `checked_artifact::`,
so the other partitions cannot move), so this is a convention nit, not an
error.

---

## 6. Mandate item dispositions

1. **EVASION** — **HOLE CONFIRMED, [P1-1].** A fully-qualified call, a
   module-qualified call and a `use … as rc` aliased call, all real and
   compiling in `admission/driver.rs`, leave the tripwire **GREEN**; adding
   one unqualified call in the same file turns it RED naming only that one
   site (`left: 1` with four callers present). Transcripts §2.2/§2.3; all
   probes reverted, tree clean at `eb606ef`.
2. **Exhaustiveness** — Visibility facts verified true in the tree
   (`bootstrap.rs:233` `pub(in crate::checked_artifact)`; `bootstrap.rs:240`
   module-private; repo-wide grep finds no occurrence outside
   `src/checked_artifact/`), and the scan's file set verified to cover that
   tree; both over-eager-direction claims verified (six inline
   `#[cfg(test)] mod tests {` blocks and the off-convention
   `namespace/test_support.rs` are all scanned). The premise is true but
   unasserted — [P2-1].
3. **`DEFINITION_COUNT` anti-vacuity** — Anchors what it claims and fires
   first with a teaching message (PROBE D transcript, §5). Two limits
   recorded as [P3-1]: it anchors the owner's file only, not tree coverage;
   and the message misdescribes the likeliest legitimate refactor
   (collapsing the delegate, 2 → 1).
4. **Negative probe replication** — Replicated exactly: unqualified caller →
   RED, site named as `…/admission/driver.rs:267`, revert → green
   (Appendix A, cmds 6, 8).
5. **Pin provenance** — Block conforms to the driver's docstring convention;
   **448 MEASURED by me** on this tree (`cargo test --lib -p gwz-core
   checked_artifact::` → `448 passed`), confirming 447 → 448; linux
   457 → 458 DERIVED is sound because the test carries no `cfg` gate
   (`mod interface_tests` is `#[cfg(test)]` only). One convention nit,
   [P3-4].
6. **Cap and scope** — Diff is exactly three files, +118/−1, nothing
   smuggled; commit message carries no `Co-Authored-By`, no AI attribution,
   no trailer of any kind. The 58/60 figure reproduces only on a code-lines
   basis, not the house one — [P2-2].

---

## 7. Findings summary table

| ID | Grade | Finding | Blocks? |
| --- | --- | --- | --- |
| P1-1 | **P1** | Qualified and aliased production callers evade the tripwire entirely; proved with three compiling call sites and a same-file unqualified control | **Yes** |
| P2-1 | P2 | The visibility premise that makes the scan exhaustive is documented, not asserted; a `pub(crate)` widening passes green | Yes (fold with P1-1) |
| P2-2 | P2 | "58/60" is a code-lines figure; the house measured basis makes it 118 against a stated 60, and the plan says "cap", the mandate says "HARD cap" | Record + lane-owner ruling |
| P3-1 | P3 | `DEFINITION_COUNT` anchors the owner's file, not scan coverage; its message misdescribes the 2 → 1 refactor | No |
| P3-2 | P3 | Only line-leading `//` is stripped; trailing and block comments are scanned (loud direction, but undeclared) | No |
| P3-3 | P3 | The `_tests.rs` extension is four files not two, and forks "production" from the house rule without extending it | No |
| P3-4 | P3 | The provenance block asserts the unmoved partitions without the same-run re-measurement its siblings cite | No |

---

## 8. Verdict

**NO-GO**, round 1 of 2.

The delivery is careful, well-grounded and honest about almost everything it
does — the visibility argument is true, the anti-vacuity anchor is real, the
measured pin is right, the provenance block is conventional, and the scope
is exactly the charter's. But the single property the step was chartered to
establish — *"first activation becomes loud instead of silent"* — does not
hold against the most ordinary way a Rust author writes a cross-module call.
`catalog::recover_or_create(lease)` is not an exotic evasion; it is what
`rustfmt` and idiom produce when the caller does not import the name. E4.1's
caller is at least as likely to be written that way as not, and if it is,
this pin will say nothing.

Fold [P1-1] and [P2-1] together in remediation (both are edits to the same
twelve lines), record [P2-2]'s measured number, and take or leave the [P3]s.
§2.4's ALT-4 is the shape I would choose: it is the idiom the file already
says it is following, and it is the only one of the four that closes the
alias hole as well as the qualified one.

---

## Appendix A — every command run, with its result

All under `CARGO_TARGET_DIR=.../scratchpad/r2f-r12-review-target`, cwd
`.../scratchpad/r2f-r12-wt`.

| # | Command | Result |
| --- | --- | --- |
| 1 | `cargo test --lib -p gwz-core checked_artifact::` (pristine tree) | **`448 passed; 0 failed; 0 ignored; 1355 filtered out`**, exit 0 — confirms the darwin pin |
| 2 | `git show --stat eb606ef` / `git show eb606ef` | 3 files, +118/−1; message read in full, trailer-free |
| 3 | `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | `checked-artifact boundary: ok (15 visible entries, 5 classified modules)`, exit 0 |
| 4 | Python replica of the shipped scan over the tree | defs 2; sites 0; sites-without-`::`-rule 1 (`bootstrap.rs:236`) |
| 5 | PROBE A/B applied to `admission/driver.rs`; `cargo test … catalog_activation_pin` | **`ok. 1 passed`** — three real callers, GREEN. **[P1-1]** |
| 6 | PROBE C added (unqualified, same file); same test | **`FAILED`**, `left: 1 / right: 0`, site named `…/admission/driver.rs:267` |
| 7 | `git checkout -- src/checked_artifact/admission/driver.rs` | `git status --porcelain` empty |
| 8 | PROBE D (`root` re-pointed to `src/checked_artifact/admission`); same test | **`FAILED`** on the definition assert, `left: 0 / right: 2`, teaching message; reverted, tree clean |
| 9 | `cargo test --lib -p gwz-core checked_artifact::interface_tests::catalog_activation_pin` (restored tree) | `ok. 1 passed` |
| 10 | `git rev-parse HEAD` / `git status --porcelain` at close | `eb606ef5f3ab50306e9eb9e5b80094e5042a48ab` / empty |
| 11 | Python replica under ALT-1 / ALT-2 / ALT-3 exclusion rules | 0 / 0 / 1 site respectively (§2.4) |

## Appendix B — what was not run, and why

- **The full aggregate-gate battery** (`run_r4bg_aggregate_gates.py fault`).
  Only the `checked_artifact::` partition moves; the other three partition
  commands are untouched by this diff and their pins are unchanged, so
  running them would re-measure the base, not the step. The moved partition
  was measured directly (cmd 1) and matches 448.
- **The linux and Windows legs.** No linux or Windows host here; that is
  precisely what FIRST-DISPATCH-EXPECTED encodes, and the block marks it
  correctly.
- **Real-workspace batteries.** This commit adds no production code and no
  filesystem behavior; it adds one source-tree-walking test. Nothing in the
  diff can move a real-workspace outcome.
- **`cargo clippy` / `cargo fmt --check`.** Out of this axis, and the diff's
  Rust is a single test file that the lane gate covers at landing.

---

# Round 2

**Date:** 2026-09-01
**Axis:** same single axis, peer-blind (the builder's disposition summary was
read as claims to falsify; no builder report sought)
**Tree under review:** gwz-core `3c6fdee57a482693f81bc039a1e373645e276452`
(branch `r2f/r1-2-tripwire`), one commit amending `eb606ef`
**Base:** unchanged, gwz-core main `ea3a924`
**Reviewer worktree:** same, `.../scratchpad/r2f-r12-wt`. Eleven transient
probe edits (four spelling probes, two anchor probes, five hole-hunt probes);
every one reverted with `git checkout --`, the four spelling probes
additionally **byte-verified** against a pre-probe copy with `cmp`.
`git status --porcelain` empty and `git rev-parse HEAD` = `3c6fdee…` at open
and at close. Nothing committed; no tag or branch touched.
**Host:** unchanged (Darwin 25.6.0 / arm64; cargo 1.95.0, rustc 1.95.0;
Python 3.13.12).

**Verdict: GO.** 0 [P0] / 0 [P1] / 0 [P2] / 4 new [P3], all record-only.
Round 1's [P1-1], [P2-1], [P2-2] and all four [P3]s are cured or accepted.

The remediation is the right one. ALT-4 was adopted without dilution — bare
name, comment-stripped, file-set, owner subtracted, **no spelling exclusions
at all** — and it works: I re-ran all four spellings that round 1 probed and
**every one is now RED, each naming the offending file**, where three of the
four were green a round ago. The two added anchors are not decoration: both
fire, both fire in the right order, and the `OWNER_CALL_SHAPED = 3` anchor is
independently corroborated by a pre-existing seal test I had not previously
connected to this work. All three aggregate-gate partition counts the
provenance block now claims — 448, 1097 + 1 ignored, 256 — I measured myself
on this tree, so the newly-added same-session re-measurement claim is not
merely repeated but confirmed.

What is left is a short list of residual matcher edges, three of which I
found by probing past the mandate. None is a silent-activation path an
unwitting author falls into; each needs a deliberate, reviewable edit, and
two of them are already declared in the file (one of them under-stated).
They are recorded for E4.1's benefit, not held against the landing.

## R2.1 — Mandate item 1: the four spellings, each individually

Host `src/checked_artifact/admission/driver.rs`, one probe at a time, each a
real compiling `pub(super) fn probe_activate(l: CatalogMutationLeaseV1<'_>)
-> Result<OpaqueRetainedCatalogV1<'_>, CheckedFsError>` whose body is the
spelling under test. Between probes: `git checkout --`, then `cmp` against a
pre-probe copy.

| # | Spelling | Round 1 | Round 2 |
| --- | --- | --- | --- |
| 1 | `recover_or_create(l)` (unqualified, `use` of the name) | RED | **RED** |
| 2 | `crate::checked_artifact::catalog::recover_or_create(l)` | **green (hole)** | **RED** |
| 3 | `catalog::recover_or_create(l)` (module-qualified) | **green (hole)** | **RED** |
| 4 | `use …::recover_or_create as rc;` + `rc(l)` | **green (hole)** | **RED** |

All four produce the identical, correct failure at `pin.rs:106`:

```
assertion `left == right` failed: production catalog activation appeared in ["admission/driver.rs"].
A1's coexistence gate forbids it until the R2-F relocation lands, and E4.1 is the step that
deliberately adds the first caller AND moves PRODUCTION_CALLER_COUNT to 1 in that same reviewed
commit. If you are E4.1, move the pin with the caller; if you are not, it does not belong here yet
```

Each revert byte-verified (`[revert byte-verified OK]` ×4); restored tree
green (`ok. 1 passed`). **[P1-1] CURED.**

The mechanism is sound for the right reason, not by luck: spellings 2 and 3
put the literal name at the call site, and spelling 4 puts it on the `use`
line. Matching the bare name is what makes the matcher blind to *how* the
call is written, which was the entire defect.

## R2.2 — Mandate item 2: the two anchors

**Anchor A — blind the walk to `catalog/`** (added `&& name != "catalog"` to
the directory filter at `pin.rs:58`). Fires, and fires **before** the file-set
assertion:

```
panicked at src/checked_artifact/interface_tests/catalog_activation_pin.rs:97:5:
assertion `left == right` failed: the owner's own files hold 0 call-shaped occurrences, not 3.
A gained one is a caller inside the owner, invisible to the file set below; a lost one means the
thin delegate was collapsed, or the scan no longer reaches the owner at all
```

Note it survives the ordering trap that would have caught a lazier design:
the entry-point assertion reads `catalog/bootstrap.rs` *directly* rather than
through the walk, so a blinded walk does not short-circuit into the wrong
message. The message teaches both directions, which was round 1's [P3-1]
complaint verbatim.

**Anchor B — widen the entry point to `pub(crate) fn recover_or_create(`.**
Fires first of all three:

```
panicked at src/checked_artifact/interface_tests/catalog_activation_pin.rs:89:5:
the entry point is no longer declared `pub(in crate::checked_artifact) fn recover_or_create(`;
the root is exhaustive only while it stays `pub(in crate::checked_artifact)`, since a wider one
admits callers outside `src/checked_artifact/`, where nothing here looks
```

This is the right thing to assert. A caller outside `src/checked_artifact/`
requires the *function's own* visibility to widen first — the re-export at
`catalog.rs:35` cannot exceed the item it re-exports — so pinning the
declaration covers the necessary first step of the only escape route.
**[P2-1] CURED**, with one residual at [P3-8].

**Corroboration I did not expect.** `interface_tests/capability_permit.rs:302-345`
(`catalog_owner_surface_is_sealed_and_lease_only`) already *requires*
`"recover_or_create"` to appear in `catalog.rs` and
`"CatalogOwnerV1::recover_or_create(lease)"` to appear in
`catalog/bootstrap.rs`. So `OWNER_FILES` and `OWNER_CALL_SHAPED = 3` are not
free-floating constants: a pre-existing seal test independently holds two of
the three call-shaped occurrences in place, and the two pins agree rather
than compete. I also re-derived the pin's numbers with an independent Python
replica of the shipped `scan`: `NAMED = ['catalog.rs', 'catalog/bootstrap.rs']`,
`calls = 3`, `callers = []`.

## R2.3 — Mandate item 3: the declared residual and the declined floor

**The declined scanned-file floor — ACCEPTABLE.** The residual is real:
`OWNER_CALL_SHAPED` proves the walk reached `catalog/`, not that it reached
everything else, so a future edit blinding the walk to (say) `admission/`
leaves `calls = 3`, `named = {owners}` and passes green. I judge the decline
correct on the merits regardless of the stated reason (which does not appear
in either the file or the commit message — if it was written down, it is in a
disposition document outside the delivered artifacts, and it should be
carried into the record). A file-count floor would churn on every unrelated
file added under `src/checked_artifact/`, buying a maintenance treadmill for
an anchor whose residual can only be triggered by editing the pin file
itself — a reviewed edit by construction. Record-only.

**The declared alias residual — ACCEPTABLE, but UNDER-STATED; see [P3-7].**
The doc says *"a caller aliased inside the defining file itself stays the one
uncovered shape."* `OWNER_FILES` has **two** entries, and the natural place to
mint an alias is `catalog.rs` — the re-export file — not `bootstrap.rs`, the
definer. Probed (H4, §R2.4).

## R2.4 — Mandate item 4: hunting new holes ALT-4 introduces

Six probes past the mandate's list. Two false negatives, three loud-but-
mis-diagnosed positives, one weakened anchor.

| Probe | Construction | Result |
| --- | --- | --- |
| H1 | `const PROBE: &str = "recover_or_create";` in `driver.rs` | **RED** — mention bricks the pin |
| H2 | `const PROBE: &str = "recover_or_create_workspace";` | **RED** — substring, no word boundary |
| H3 | one line: `let _s = "//"; use …::recover_or_create; recover_or_create(l)` | **GREEN with a live compiling caller** |
| H4 | `use bootstrap::recover_or_create as activate_catalog;` in `catalog.rs` + `activate_catalog(l)` in `driver.rs` | **GREEN with a live compiling caller** |
| H5 | `/* probe: recover_or_create is mentioned here */` | **RED** — declared loud direction |
| H6 | widen to `pub(crate)` **and** leave `// was: pub(in crate::checked_artifact) fn recover_or_create(` | **GREEN — the premise assertion is satisfied by the comment** |

Answering the mandate's question directly — *does a stray mention brick the
pin, and is that failure direction acceptable-loud or a trap?* It bricks the
pin (H1, H5), and the direction is acceptable: red-on-mention is the safe
side, and the doc declares block comments and `cfg(test)` modules as scanned.
It is **not** a trap in the dangerous sense, because a person who trips it
reads a failure and fixes the pin rather than shipping a live catalog. It is
a mild trap in a different sense: the message asserts *"production catalog
activation appeared in …"* when what the check actually proves is *"this file
names the entry point"*. See [P3-6].

There is no doc-test exposure: Rust doc-tests live in `///` / `//!` blocks,
which the stripping removes, and I confirmed no production file under
`src/checked_artifact/` carries the name outside the two owners.

### [P3-5] `split_once("//")` also truncates at a `//` inside a string literal

`pin.rs:69` cuts each line at the first `//` **anywhere**, including inside a
string. Everything after it on that line is invisible to the matcher. H3
probed a real, compiling production caller on such a line and the pin stayed
**GREEN**:

```rust
pub(super) fn probe_h3(l: CatalogMutationLeaseV1<'_>) -> Result<…> { let _s = "//"; use crate::checked_artifact::catalog::recover_or_create; recover_or_create(l) }
```

Contrived, and reachable only by deliberate construction — no rustfmt output
and no ordinary code puts a `//`-bearing string before a call on one line.
But it is the same *class* of hole as round 1's [P1-1] and the file's own doc
declares only that "`//` comments are stripped to end of line", which reads as
comment-only. Two cheap closures: use the house `mask_non_code` shape
(`check_checked_artifact_boundaries.py:838`), or state the residual beside
the alias one.

Related, analysis-only: `.collect::<String>()` at `pin.rs:70` joins the
stripped lines with **no separator**, so line *N*'s tail abuts line *N+1*'s
head. This can only *add* matches, never remove them, and I found no line
pair in the tree that forms a spurious `recover_or_create` or
`recover_or_create(` across the join. Loud direction; worth a comment, not a
change.

### [P3-6] The bare-name match has no word boundary, and the failure message over-claims

`code.contains("recover_or_create")` matches any identifier or string having
it as a substring. The tree already contains two such names:
`recover_or_create_workspace` and `recover_or_create_git_directory`, listed
as **FORBIDDEN_PROVISIONAL_CATALOG_INTERFACES**
(`check_checked_artifact_boundaries.py:757-765`) and re-asserted forbidden at
`interface_tests/capability_permit.rs:55-60`. H2 confirmed that either one
appearing in a production file trips the A1 tripwire with the message
*"production catalog activation appeared in …"* — a **wrong diagnosis** of a
real but different violation.

Practical risk is near zero precisely because those two names are
independently forbidden by two other gates, so they cannot appear in a
passing tree. The finding is about the message, not the match: what the
assertion proves is that a production file *names* the entry point, and the
message should say that before it says "activation". A one-clause edit ("a
production file outside the owner names `recover_or_create` — usually a
caller, possibly a mention") makes a false trip self-diagnosing instead of
misleading.

### [P3-7] The declared alias residual is stated one file too narrow

The doc (`pin.rs:15-16`) names *"the defining file itself"*. The residual is
actually **either** `OWNER_FILES` entry, and the more natural of the two is
`catalog.rs`, which exists to re-export this symbol. H4 probed exactly that,
with a live compiling caller and the pin **GREEN**:

```rust
// src/checked_artifact/catalog.rs  (an OWNER_FILE — subtracted from the file set)
pub(in crate::checked_artifact) use bootstrap::recover_or_create as activate_catalog;

// src/checked_artifact/admission/driver.rs — never names `recover_or_create`
pub(super) fn probe_h4(l: CatalogMutationLeaseV1<'_>) -> Result<…> { activate_catalog(l) }
```

`OWNER_CALL_SHAPED` does not catch it either: the `use … as` line carries no
`(`, so `calls` stays 3. I considered grading this P2 and did not, for two
reasons: it takes two deliberate edits, one of them to the sealed catalog
surface that `capability_permit.rs:302-345` already guards; and the threat
model here is *unwitting* activation, not a person who mints an alias
re-export and then calls it. Record it, and correct the doc to say "inside
either owner file". The one-line hardening, if E4.1 wants it: assert the
owner files contain no ` as ` rebinding of the name.

### [P3-8] The premise assertion reads raw source, so a comment can satisfy it

`pin.rs:90-92` runs `.contains(ENTRY_POINT_DECLARATION)` on
`read_to_string(catalog/bootstrap.rs)` — **without** the comment stripping
`scan` applies. H6 widened the entry point to `pub(crate)` and left a
history comment quoting the old signature; the pin went **GREEN**:

```rust
/// Recovers or creates one catalog using only a target-bound lease.
// was: pub(in crate::checked_artifact) fn recover_or_create(
pub(crate) fn recover_or_create(
```

This is the one residual I think is more likely to be hit by accident than by
intent — leaving a `// was: …` line beside a widened signature is an ordinary
refactoring habit, and it silently re-arms the exact hole [P2-1] existed to
close. It does not by itself produce a silent activation (an out-of-subtree
caller still needs the `catalog.rs` re-export and `mod catalog` widened too,
and any in-subtree caller is still caught by the file set), which is why it
is P3. The fix is one line: strip comments before the `contains`, reusing the
same expression already written at `pin.rs:67-70`.

## R2.5 — Mandate item 5: cap, scope, trailer, partitions

**Cap.** Counted independently from `git show --numstat 3c6fdee`:

| File | added | deleted |
| --- | --- | --- |
| `scripts/checks/run_r4bg_aggregate_gates.py` | 14 | 1 |
| `src/checked_artifact/interface_tests/catalog_activation_pin.rs` | 114 | 0 |
| `src/checked_artifact/interface_tests/mod.rs` | 1 | 0 |
| **total** | **129** | **1** |

**129 whole handwritten lines**, confirming the delivery's figure against a
ceiling of 130 on the ruled basis — which is the basis round 1's [P2-2] asked
the lane owner to set, and it is the house one (`GwzWtPlan.md:45-47`,
`GwzWt-P0.1-Review.md:456-463`).

**Record obligation — already discharged.** I checked whether the plan text
still read the stale *"cap 60"* and found the ruling written into
`GwzM5-8R2F-RelocationPlan.md` §3 in the gwz-dev working tree (uncommitted at
the time I looked): R1.2's cap **RESET to HARD 130**, dated 2026-09-01,
citing this review's round-1 counting-basis finding by name, stating the basis
as whole handwritten lines including comments with generated excluded, and
recording the reset as *"reset recorded, not waived"* — with the same ruling
applied to R1.1 (250 → 420). That is §10.2 satisfied at the right document by
the right owner. Nothing further is owed here; R1.3 need only carry it
forward if the plan edit is not already in the landing train.

**Scope.** Exactly the same three files as round 1. Nothing else touched.

**Trailer.** `git log -1 --format=%B 3c6fdee` grepped for
`co-authored|generated with|claude|anthropic|noreply` → no match. Clean.

**Partitions — all three measured by me on this tree:**

| Partition | Command | Pinned | Measured |
| --- | --- | --- | --- |
| `checked_artifact::` | `cargo test --lib -p gwz-core checked_artifact::` | 448 | **448 passed** |
| lib remainder | `cargo test --lib -p gwz-core -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` | 1097 + 1 ignored | **1097 passed; 1 ignored** |
| `v1_lifecycle::` | `cargo test --lib -p gwz-core workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix` | 256 | **256 passed** |

The pin stays 447 → 448 across the rule swap, confirming the rewrite is
test-count-neutral; the linux 457 → 458 DERIVED / FIRST-DISPATCH-EXPECTED
framing is unchanged and correct (`mod interface_tests` is `#[cfg(test)]`
only, no OS gate). The block's newly-added same-session re-measurement claim
is therefore not just restated but independently confirmed. **[P3-4] CURED.**

## R2.6 — Round-1 finding dispositions

| ID | Round 1 | Round 2 | Evidence |
| --- | --- | --- | --- |
| P1-1 | P1 — qualified/aliased callers evade | **CURED** | Four spellings each RED naming the file (§R2.1) |
| P2-1 | P2 — visibility premise unasserted | **CURED** (residual [P3-8]) | Anchor B RED at `pin.rs:89` (§R2.2) |
| P2-2 | P2 — "58/60" off the house basis | **CURED by ruling; record discharged** | 129 counted; HARD 130 ruled and written into the plan §3 with its cause (§R2.5) |
| P3-1 | P3 — anchor scope + message | **CURED** (message); floor **DECLINED, ACCEPTABLE** | Anchor A RED at `pin.rs:97`, both directions taught (§R2.2–R2.3) |
| P3-2 | P3 — comment asymmetry | **CURED** (introduces [P3-5]) | `//` stripped to EOL incl. trailing; H3 (§R2.4) |
| P3-3 | P3 — `_tests.rs` count and forked rule | **CURED** | `pin.rs:45-52` names all four and declares the narrowing local |
| P3-4 | P3 — unmeasured "keeps" claims | **CURED** | Three partitions measured by me (§R2.5) |

## R2.7 — Round-2 findings summary

| ID | Grade | Finding | Blocks? |
| --- | --- | --- | --- |
| P3-5 | P3 | `split_once("//")` truncates at `//` inside a string literal; a caller on such a line is invisible (probed green). Line-join has no separator (analysis only) | No |
| P3-6 | P3 | Bare-name match has no word boundary (`recover_or_create_workspace`, a house-FORBIDDEN name, trips it) and the message asserts "activation" where the check proves "names it" | No |
| P3-7 | P3 | The declared alias residual applies to **either** owner file; `catalog.rs`, the re-export, is the natural aliasing site (probed green) | No |
| P3-8 | P3 | The premise assertion reads raw source, so a `// was: …` comment satisfies it after a widening (probed green) — the likeliest accidental residual | No |

## R2.8 — Verdict

**GO.** 0 [P0], 0 [P1], 0 [P2]; four [P3], all record-only.

Round 1's finding was that the tripwire did not do the one thing it was
chartered to do. It does it now, in the shape the review asked for, verified
by re-running the exact probes that failed it — and the builder went past the
remedy to assert the premise the whole design rests on, which round 1 only
asked be recorded. The step's charter is met: *"a test asserting
`recover_or_create`'s PRODUCTION caller count is exactly zero, dated comment
naming E4.1 as the step that moves it to 1"* — present at `pin.rs:27-31`,
with the pin, the date and E4.1 named.

The four [P3]s are matcher edges, not defects in the property. Three of them
([P3-5], [P3-7], [P3-8]) are narrow enough that I would not spend a round on
them; the one worth a line of E4.1's attention is [P3-8], because a habitual
`// was: …` comment is the only residual here that a careful person could
walk into without meaning to. [P3-6]'s message wording is a free improvement
whenever this file is next touched.

Recommend landing. The ceiling revision is already recorded at the plan
(§R2.5); the only record item left for R1.3 is the corrected residual
statement from [P3-7] — "inside **either** owner file", not "the defining file
itself".

**Reviewer working-tree note.** At close, `gwz-dev` also showed uncommitted
modifications to `GwxMergeFindingsDraft.md`, `GwzAiReqDd.md` and
`GwzM5-8R2F-RelocationPlan.md`. None is mine: my only write to this repository
is this report. The gwz-core worktree is byte-clean at `3c6fdee`.

## Appendix C — round-2 commands

All under `CARGO_TARGET_DIR=.../scratchpad/r2f-r12-review-target`, cwd
`.../scratchpad/r2f-r12-wt`, HEAD `3c6fdee`.

| # | Command | Result |
| --- | --- | --- |
| 1 | `git show 3c6fdee` / `--numstat` | 3 files, 129 added / 1 deleted; message trailer-free |
| 2 | `cargo test --lib -p gwz-core checked_artifact::` | `448 passed; 0 failed` |
| 3 | `cargo test --lib -p gwz-core -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` | `1097 passed; 0 failed; 1 ignored` |
| 4 | `cargo test --lib -p gwz-core workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix` | `256 passed; 0 failed` |
| 5-8 | Spelling probes 1-4, each applied alone to `admission/driver.rs` | all four **FAILED** at `pin.rs:106` naming `admission/driver.rs`; each revert `cmp`-verified |
| 9 | Anchor A — `&& name != "catalog"` at `pin.rs:58` | **FAILED** at `pin.rs:97`, `0 call-shaped occurrences, not 3` |
| 10 | Anchor B — entry point widened to `pub(crate)` | **FAILED** at `pin.rs:89`, premise message |
| 11 | H1 `const PROBE: &str = "recover_or_create";` | **FAILED** at `pin.rs:106` (mention bricks the pin) |
| 12 | H2 `const PROBE: &str = "recover_or_create_workspace";` | **FAILED** at `pin.rs:106` (substring, wrong diagnosis) |
| 13 | H3 `let _s = "//"; … recover_or_create(l)` on one line | **`ok. 1 passed`** — live caller, pin green |
| 14 | H4 alias re-export in `catalog.rs` + `activate_catalog(l)` in `driver.rs` | **`ok. 1 passed`** — live caller, pin green |
| 15 | H5 `/* … recover_or_create … */` block comment | **FAILED** at `pin.rs:106` (declared loud direction) |
| 16 | H6 `pub(crate)` + `// was: pub(in crate::checked_artifact) fn recover_or_create(` | **`ok. 1 passed`** — premise satisfied by a comment |
| 17 | Python replica of the shipped `scan` | `NAMED=['catalog.rs','catalog/bootstrap.rs']`, `calls=3`, `callers=[]` |
| 18 | `git status --porcelain` / `git rev-parse HEAD` at close | empty / `3c6fdee57a482693f81bc039a1e373645e276452` |
