# GwzM5-8 R2-F R1.1 — single-axis peer-blind review (Opus)

**Step:** R2-F R1.1 — the catalog/legacy private-name split (the package's
load-bearing step).
**Candidate:** `4ba9071eff8579bcd95dc653778ad8629ee9b9ad`, branch
`r2f/r1-1-split`, worktree
`/private/tmp/claude-501/-Users-owebeeone-limbo/3a4f2794-661b-40df-b586-1f52221017f3/scratchpad/r2f-r11-wt`.
**Base:** gwz-core main `ea3a924` (v0.12.1).
**Authorities:** `GwzM5-8R2F-RelocationPlan.md` (ADOPTED; §1 design, §3 R1.1
charter incl. the lane owner's 2026-09-01 cap ruling), `GwzM5-8R2F-RelocationTrace.md`,
`GwzM5-8R2F-RelocationPlan-Review.md` (all three rounds).
**Round:** 1 of the two-round cap.
**Reviewer conditions honoured:** read-only outside the worktree; every probe
reverted and the tree verified clean at
`git rev-parse HEAD == 4ba9071eff8579bcd95dc653778ad8629ee9b9ad` with empty
`git status --porcelain`; `CARGO_TARGET_DIR` =
`.../scratchpad/r2f-r11-review-target` throughout; no commits made; no trailers
added anywhere.
**Date:** 2026-09-01.

---

## VERDICT: **GO-WITH-CONDITIONS**

**0 P0 / 1 P1 / 1 P2 / 6 P3.**

The design is faithful to §1 row by row. The split's two consumers are
genuinely split — I traced every runtime path and **no consumer is left on the
wrong side** (§1 below). The decisive test is real: it FAILS under the round-2
dead design when I revert `policy.rs` myself, exactly as the builder claims,
on the disjointness assertion. Every companion the mandate names verifies: the
four boundary-checker digests recompute byte-for-byte from the committed tree
and the checker is green on the pristine tree; the aggregate pin 451 is
MEASURED by me; the lib remainder is genuinely unmoved at 1097; both exemption
surfaces are covered, load-bearing (each proved by a mutation probe), each
under its own written ground, and prefix-exact rather than over-broad. The cap
holds at **394 whole handwritten added lines against the HARD 420**, with
code substance at 178–200, and the prose audit finds **no rider** — every
comment block maps to a named mandate item.

The one P1 is FLAG 5. The structural no-anchor guarantee — the thing
[RC-P3-1] says "is what makes the Windows arm safe", and which the charter
carries into the Windows first-dispatch obligation — is implemented as a
source scan that matches exactly one spelling of the call. I added two real,
compiling production callers of `prepare_private` in the two most plausible
alternative spellings and **the guarantee stayed green**. This is the same
hole the R1.2 review found and closed on the sibling step the same day
([P1-1] there, `GwzM5-8R2F-R1.2-Review.md` §2), in mirror image.

The P2 is a sharpening of the decisive row, not a defect in it: the charter's
literally named criterion — "re-observe the catalog and assert it is still
recoverable" — turns out to be **non-discriminating on darwin and linux**. I
proved it. The row is still decisive, but solely through the [RC-P3-1]
companion assertion, and both the plan review's text and the builder's in-code
doc comment attribute the failure to a mechanism that does not occur on the two
platforms CI actually runs.

Neither blocks the design. Both are fixable inside a remediation round without
touching the split.

---

## 1. DESIGN FIDELITY — row by row against §1/§3

### 1.1 The split's two consumers actually split — NO P0

I enumerated **every** `CatalogPrivateNameV1::` reference in `src/` and every
production spelling of the leaf, and classified each by side.

**Composes `catalog-final` (the CATALOG side) — all production:**

| Site | Role |
|---|---|
| `src/checked_artifact/catalog/enumeration.rs:384` | `fixed_roles`' third entry — the recognition table |
| `src/checked_artifact/capability/pre_catalog/provider/completed.rs:77`, `:325` | opens/validates the Final directory |
| `src/checked_artifact/capability/pre_catalog/provider/directory_mutation.rs:207`, `:321` | publishes staging → final |
| `src/checked_artifact/capability/pre_catalog/provider/aggregate.rs:90` | `directory_fact(Final, …)` |
| `src/checked_artifact/capability/pre_catalog/provider/interior.rs:178` | Final's interior arm |
| `src/checked_artifact/capability/pre_catalog/provider/namespace.rs:67` | `CatalogRecognizedNameV1::Final` → bytes |
| `src/checked_artifact/protocol/catalog_bootstrap_record.rs:75`, `:242` | `final_name`, issued and validated |
| `src/checked_artifact/protocol/infrastructure_record/owner.rs:271` | candidate validation |

**Composes `checked-artifacts` (the LEGACY side) — production:**

| Site | Role |
|---|---|
| `src/checked_artifact/policy.rs:49`, `:51` | `private_parent()`, BOTH arms → `LegacyPrivate` |
| `src/git/gitbackend/preservation_image.rs:8` | `CHECKED_ARTIFACT_PRIVATE_PATH` — legacy exemption, untouched |

`policy.rs::private_parent` had exactly one production reader,
`src/checked_artifact/observation.rs:93` (`let quarantine_parent =
policy.private_parent();`) — the legacy leaf writer's quarantine parent. It is
now on the legacy side. **That is the whole split, and it is complete.**

Two distinct things are both spelled `private_parent` in this tree and they are
NOT the same concept: `CheckedArtifactPolicy::private_parent()` returns the
legacy LEAF (`.gwz/checked-artifacts`), while
`provider/retained.rs:151::private_parent()` returns the enclosing `.gwz` /
`gwz` DIRECTORY handle used by the catalog. The change touches only the first.
Pre-existing naming collision, not introduced here; noted so the next reader
does not misread the diff.

**Checked for stragglers and found none:** `snapshot.rs:369-370`'s
`.gwz/checked-artifacts` literals are inside `#[cfg(test)] mod tests` (block
opens at `:337`); `src/checked_artifact/tests/{leaf_publication,recovery_protocol,staging_recovery,removal_recovery}.rs`
hard-code `.gwz/checked-artifacts` and are LEGACY-writer tests, correctly
unchanged; `admission/tests.rs:59` and `admission/driver/tests.rs:60` use
`Final` and are genuinely catalog-side (they join `InfrastructureSlotV1`
names).

### 1.2 Load-bearing prose claims — each independently verified in the tree

| Claim (site) | Verdict | Evidence |
|---|---|---|
| `ALL`'s only two consumers are the collision domain and a test pin (`catalog_names.rs`) | TRUE | `capability/collision.rs:244` and `interface_tests/catalog_recovery.rs:229` — those two, nothing else |
| De-recognition comes solely from `Final.leaf_bytes()`; `fixed_roles` is a fixed variant-keyed array (`catalog_names.rs`) | TRUE | `catalog/enumeration.rs:373-388` is a literal `[( … ); 3]`, keyed by variant, never consulting `ALL` |
| `directory_fact` already has a `_ => Other` arm, "checked, not missed" (`catalog_names.rs`, [RC-P3-4]) | TRUE | `provider/interior.rs:186`; and `LegacyPrivate` is never passed — `aggregate.rs:86,90` pass only `BootstrapStaging`/`Final` |
| `prepare_private` is `pub(super)` on `platform` so no caller can exist outside the subtree (`catalog/bootstrap/tests.rs:542`) | TRUE | `platform.rs:345` (non-windows), `:688` (windows); `mod platform;` at `checked_artifact/mod.rs:55` |
| `catalog-final` collides with none of the ten infrastructure slot names | TRUE | `protocol/slots.rs:47-87` — ten `*-v1` names, none case-folding to `catalog-final` |
| MAX_PATH: `<root>/.gwz/checked-artifacts/<173>` = root + **197**, prefix 23 (`max_path.rs`, correcting the former 198) | TRUE | `1 + 4 + 1 + 17 = 23`; `23 + 1 + 173 = 197`. The catalog's 13-char leaf gives `1+4+1+13 = 19`, the stated −4 |
| Collision-domain guard over the live legacy area is preserved | TRUE | `for_root` maps `ALL` → members; the member set is a strict SUPERSET of the pre-split set (adds `catalog-final`, keeps `checked-artifacts`). Zero guard lost |

### 1.3 The decisive test IS the executable criterion — my own falsification

**PROBE 1 — revert `policy.rs` to the round-2 shared-name design.**

```
$ perl -0pi -e 's/CatalogPrivateNameV1::LegacyPrivate/CatalogPrivateNameV1::Final/g' \
      src/checked_artifact/policy.rs
$ cargo test --lib -p gwz-core a_legacy_drive_after_bootstrap_leaves_the_catalog_recoverable

test checked_artifact::catalog::bootstrap::tests::a_legacy_drive_after_bootstrap_leaves_the_catalog_recoverable ... FAILED

panicked at src/checked_artifact/catalog/bootstrap/tests.rs:608:5:
the legacy writer creates its own private area, not the catalog's

test result: FAILED. 0 passed; 1 failed; …
```

**Confirmed.** The builder's claim is exact: it fails, and on the disjointness
assertion. See [P2-1] for what this does and does not prove.

---

## 2. FINDINGS

### [P1-1] The structural no-anchor guarantee is spelling-specific and evadable — FLAG 5

`src/checked_artifact/catalog/bootstrap/tests.rs:545-566` scans production
sources under `src/checked_artifact/` for the literal
`"platform::prepare_private("` and asserts
`production_callers_of_prepare_private() == ["residue.rs"]`
(`:621`). That assertion is the whole content of the guarantee that
`.gwz/catalog-final` can never acquire a `.ca1-durability-anchor-*` — the
guarantee [RC-P3-1] calls "a real structural guarantee of this design … what
makes the Windows arm safe", and which §3 carries into the Windows
first-dispatch obligation.

The scan sees the call only when the author writes the `platform::`
module-qualifier. It is blind to a `use` import and to an alias — the two
spellings a future author is most likely to reach for. This is the R1.2
[P1-1] hole in mirror image (R1.2's scan excluded `::`-prefixed calls; R1.1's
requires one), found and closed on the sibling step the same day.

**PROBE A/B — two real, compiling production callers, evasion.** Appended to
`src/checked_artifact/observation.rs` (a production file inside the scan root,
not a `tests*` file, not under a `tests`/`interface_tests` directory):

```rust
// ===== REVIEW PROBE A (use-import evasion) — TO BE REVERTED =====
use super::platform::prepare_private;

#[allow(dead_code)]
pub(super) fn probe_use_import(dir: &cap_std::fs::Dir) -> crate::model::ModelResult<()> {
    prepare_private(dir, true, crate::model::ErrorCode::UnsupportedOperation, "probe")
}

// ===== REVIEW PROBE B (alias evasion) — TO BE REVERTED =====
use super::platform::prepare_private as pp;

#[allow(dead_code)]
pub(super) fn probe_alias(dir: &cap_std::fs::Dir) -> crate::model::ModelResult<()> {
    pp(dir, true, crate::model::ErrorCode::UnsupportedOperation, "probe")
}
```

```
$ cargo test --lib -p gwz-core a_legacy_drive_after_bootstrap
   Compiling gwz-core v0.12.1 …
   Finished `test` profile …
test checked_artifact::catalog::bootstrap::tests::a_legacy_drive_after_bootstrap_leaves_the_catalog_recoverable ... ok
test result: ok. 1 passed; 0 failed; …
```

**GREEN. Hole confirmed, both spellings.** The `use` line contains
`platform::prepare_private` but no `(`, so it does not match; the call sites
contain `(` but no `platform::`.

**PROBE C — same file, matching spelling, control:**

```rust
// ===== REVIEW PROBE C (matching-spelling control) — TO BE REVERTED =====
#[allow(dead_code)]
pub(super) fn probe_qualified(dir: &cap_std::fs::Dir) -> crate::model::ModelResult<()> {
    super::platform::prepare_private(dir, true, crate::model::ErrorCode::UnsupportedOperation, "probe")
}
```

```
panicked at src/checked_artifact/catalog/bootstrap/tests.rs:621:5:
assertion `left == right` failed
  left: ["observation.rs", "residue.rs"]
 right: ["residue.rs"]
```

RED, naming exactly the offending file. The scan works — for one spelling.

**PROBE D — anti-vacuity (the R1.2 [P3-1] lesson).** Repointed the scan root
from `src/checked_artifact` to `src/checked_artifact/catalog`:

```
panicked at src/checked_artifact/catalog/bootstrap/tests.rs:621:5:
  left: []
 right: ["residue.rs"]
```

**This is a genuine strength and I credit it:** because the assertion is
list-EQUALITY rather than R1.2's original count-of-zero, a blinded scan root
fails loudly instead of passing vacuously. The builder got the anti-vacuity
property right for free.

**Judgment on the mandate's framing.** The charter's "fix exactly the pins the
checker names" governs the boundary-checker digests, and the builder did that
exactly (§3 below). The no-anchor guarantee is a *test assertion*, which is
what [RC-P3-1] asked for ("it belongs in the named first-dispatch obligation
as an assertion"). So the **home is right** — an in-tree scan inside the
decisive row, where the claim is made and where a reader looks; a
boundary-checker pin would put the guarantee far from the claim and would need
the checker to grow a new concept. The **shape is wrong**: spelling-specific
where R1.2 converged on spelling-blind.

**Remedy (small, and the precedent exists):** match the bare identifier
`prepare_private` with no qualifier requirement — over-eager toward RED, the
direction R1.2 accepted — or count definitions plus uses. Either keeps the
list-equality anti-vacuity property.

**Sub-point, same finding:** the scan pushes `path.file_name()`, not the
relative path (`:559`). Two files named `residue.rs` in different directories
are indistinguishable in the assertion, so a *different* `residue.rs` acquiring
the call while the legacy one lost it would read green. Low probability;
one-token fix (push the relative path).

### [P2-1] The charter's named criterion — "still recoverable" — is non-discriminating on darwin/linux; the stated mechanism does not occur

Plan §3 names the decisive row as "bootstrap the catalog, run one legacy
checked-artifact drive, re-observe the catalog and assert it is still
recoverable ([R2-P2-1] — the executable §2.7 criterion)". Plan-review
[RC-P3-1] records the verification as: "under round 2's the drive writes
`ca1-*` into `catalog-final` and re-observation refuses at
`interior.rs:437-440`". The builder's in-code doc comment
(`catalog/bootstrap/tests.rs:588-591`) repeats that mechanism verbatim.

**PROBE 2 — under the round-2 dead design, with ONLY the disjointness proxies
disabled** (`policy.rs` still reverted to `Final`):

```rust
    // REVIEW PROBE (depth of the criterion) -- disjointness proxies disabled.
    let _ = &published;   // replacing the two disjointness assertions
```

```
test checked_artifact::catalog::bootstrap::tests::a_legacy_drive_after_bootstrap_leaves_the_catalog_recoverable ... ok
test result: ok. 1 passed; 0 failed; …
```

**PROBE 2b — instrumented, same dead design:**

```
PROBE published-at-bootstrap = ["catalog-anchor-a-v1", "catalog-bootstrap-retired-v1",
    "catalog-format-v1", "retired-actions-descriptor-v1", "retired-actions-v1",
    "roaming-anchor-home-v1"]
PROBE interior-after-drive   = ["catalog-anchor-a-v1", "catalog-bootstrap-retired-v1",
    "catalog-format-v1", "retired-actions-descriptor-v1", "retired-actions-v1",
    "roaming-anchor-home-v1"]
PROBE legacy-dir-exists      = false
```

**The dead design leaves no `ca1-*` residue at all.** On a clean, successful
drive the write-ahead names are removed by `finish()`, and on non-Windows
`prepare_private` is a **no-op** (`platform.rs:344-351`,
`#[cfg(not(windows))] … Ok(())`) so no anchor is planted either. The
`interior.rs:441-445` refusal never fires. It is genuine on Windows, where
`prepare_private` → `anchor::prepare` plants
`.ca1-durability-anchor-<32hex>` and "`finish()` never removes" it
(`tests/leaf_publication.rs:82-85`).

**What this means:**

1. The row **is** decisive — but on darwin and linux (the two legs CI actually
   runs) decisiveness rests **entirely** on the single assertion
   `assert!(fixture.path().join(".gwz/checked-artifacts").is_dir(), …)` at
   `:606-609`. The second disjointness assertion
   (`assert_eq!(interior_names(&catalog), published)`) and the two
   recoverability lines are all non-discriminating there; they discriminate on
   Windows.
2. **[RC-P3-1] worked exactly as intended and I credit it as such.** The plan
   review asked for a disjointness assertion because "'Still recoverable' is
   the consequence; disjointness is the property, and it fails louder and
   earlier". That is precisely what saves the row. Without it, R1.1 would ship
   a criterion that passes under the design it is meant to kill on every
   platform CI runs.
3. The defect is that the *evidence record* — plan review [RC-P3-1], plan §1,
   and the in-code comment at `tests.rs:588-591` — states a universal
   mechanism that is Windows-only. A reader of that comment believes the
   recoverability line is doing the work. It is not.

**Remedy (prose, 2–3 lines, no design change):** qualify the doc comment — the
`ca1-*` interior refusal is the **Windows-arm** mechanism; on non-Windows
`prepare_private` is a no-op and a clean drive leaves nothing, so the
disjointness assertion at `:606` is what discriminates there. Optionally
strengthen by faulting the drive so write-ahead residue survives, but that is
scope the charter did not buy.

### [P3-1] `vectors.txt` carries no in-tree record of the reviewed re-derivation — FLAG 1 condition

Legitimacy ruled in §4 below. But the edit moves a **binding digest** in three
vectors, and `protocol/checked-artifact-semantic-v1/README.md:26-28` says such
a change "requires a deliberate reviewed edit". The procedural requirement is
met by this review; nothing in the tree records it. The next reader sees three
changed hex blobs under a header still saying `hand-authored;generator=none`
with no way to learn they were mechanically re-derived and round-trip
validated.

**Remedy:** one dated `#` comment line in `vectors.txt`'s header block naming
R2-F R1.1, the three vectors, the field moved (`final_name`, CBOR key 8, and
its dependent `record_id`, key 11), and the round-trip validation. Mechanically
safe: `semantic_vectors.rs:56-59` pins only the first two lines via
`starts_with`, and `vectors()` filters every `#` line
(`semantic_vectors.rs:62`).

### [P3-2] The trace's §7.6 is still false in the trace — route to R1.3

`GwzM5-8R2F-RelocationTrace.md:763-770` still reads "The relocation is not a
semantic-vector event." The commit message records the falsification; the
evidence base does not. R1.3 is the records train and this belongs on it,
alongside the MAX_PATH rider's falsification.

### [P3-3] The catalog exemption's ground overstates the interior grammar

`preservation_image.rs:14-15` grounds the exemption on an interior "which
admits only the ten infrastructure slot names". `provider/interior.rs:438-441`
also admits `CatalogRootRowClassV1::ActiveAction(_)` rows. The **exemption is
still correct** — the ground that carries it is "exact-managed product state
the user never authored", and action rows are exactly that — but the sentence
as written is narrower than the code. One-word fix.

### [P3-4] The pre-existing singular framing above the dirt classifier is now stale

`repository_support.rs:108-114` still opens "The private area
(`.gwz/checked-artifacts`) is permanent product infrastructure". The new block
at `:132-137` correctly says "the reason stated above", so the composite reads
right — but the older comment now describes one of two areas in the singular.
Cosmetic.

### [P3-5] FLAG 4's one-time red is unattributed in every filed record

See §5, FLAG 4. Not reproduced in my batched run and not reproduced in the
builder's three. The residual is that no filed artifact says *what* went red.
Worth one dated line in the delivery record so the linux/Windows first-dispatch
leg knows to capture full output rather than rediscovering the question.

### [P3-6] `PRE_R2F_WORKSPACE_COLLISION_DIGEST` is unreproducible from the tree

`interface_tests/contracts.rs:174-181` hard-codes the pre-split digest
"measured at gwz-core `ea3a924`". Correct, well-dated, and the right shape for
a movement assertion — but nothing in the tree can re-derive it, so a future
reader must trust the comment. Acceptable as-is (it is a historical constant by
construction); recorded so it is not mistaken for a live pin.

---

## 3. COMPANIONS — recomputed, not taken on trust

### 3.1 The four boundary-checker digests

Recomputed from the committed tree with my own implementation of the checker's
two digest algorithms (flat `sha256(file)` for `PROTECTED_SOURCE_DIGESTS`;
length-framed `relative || content` walk for `PROTECTED_SOURCE_TREE_DIGESTS`,
per `check_checked_artifact_boundaries.py:912-928`):

| Pin | Committed | Recomputed | |
|---|---|---|---|
| `git/gitbackend/preservation_image.rs` | `38bf7ce9…95d3c` | `38bf7ce9…95d3c` | MATCH |
| `checked_artifact/bootstrap/runtime/catalog_lease.rs` | `91ac3dfa…b0b2` | `91ac3dfa…b0b2` | MATCH |
| `checked_artifact/capability/pre_catalog.rs` | `d9236742…d045` | `d9236742…d045` | MATCH |
| `checked_artifact/catalog.rs` | `7b70d71e…c32e` | `7b70d71e…c32e` | MATCH |

The three tree digests the builder did **not** move (`capability/path.rs`,
`platform.rs`, and both `v1_lifecycle` entries) also recompute unchanged —
so the moved set is exactly the set that had to move, no more and no less.

**The check the mandate names — checker green on the pristine tree:**

```
$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
EXIT=0
$ git status --short          # empty — the checker does not dirty the tree
```

### 3.2 The aggregate pins 451/461, run by me

```
$ cargo test --lib -p gwz-core checked_artifact::
test result: ok. 451 passed; 0 failed; 0 ignored; 0 measured; 1355 filtered out;
             finished in 52.27s
```

**451 CONFIRMED**, MEASURED on darwin as the driver's provenance block claims.
The +4 is exactly four new rows and I verified there are no others:

```
$ git diff ea3a924 HEAD -U0 | awk '/^\+\+\+ b\//{f=$2} /^\+#\[test\]/{print f}' | sort | uniq -c
   3 b/src/checked_artifact/catalog/bootstrap/tests.rs
   1 b/src/checked_artifact/interface_tests/contracts.rs
```
(and zero `#[test]` removed anywhere). 447 + 4 = 451. The linux 461 is
correctly marked DERIVED / FIRST-DISPATCH-EXPECTED and neither of the four rows
carries a `cfg` gate — verified by reading all four.

### 3.3 The lib remainder is genuinely unmoved

```
$ cargo test --lib -p gwz-core -- --skip checked_artifact:: \
      --skip workspace_ops::merge::v1_lifecycle::
test result: ok. 1097 passed; 0 failed; 1 ignored; 0 measured; 708 filtered out
```

**1097 + 1 ignored — UNMOVED.** The g12/g15/stash edits are body-only, as
claimed: the `#[test]` census above shows zero tests added under
`src/git/tests/**`, and every one of those four files' diffs is comments plus
assertions inside existing bodies.

### 3.4 The full batched fault battery, as the lane runs it

```
$ python3.13 scripts/checks/run_r4bg_aggregate_gates.py fault
=== fault -- aggregate fault/restart matrices (TransitionDesign:1469-1475)
    ok    v1 lifecycle fault and restart matrices (402.1s, '256 passed')
    ok    root physical/successor boundary matrix (release profile) (412.8s, '1 passed')
    ok    checked-artifact fault census (165 keys) (71.0s, '451 passed')
    ok    lib remainder, completing the four disjoint partitions (141.5s, '1097 passed')

=== R4b-G aggregate gate summary
    ok      fault
AGGREGATE: this selection's mechanical gates pass; the settled-tree review is not.
```

All four disjoint partitions green at their pinned counts, back-to-back.

### 3.5 Lane hygiene

```
$ cargo clippy --all-targets -- -D warnings     # clean
$ cargo fmt --check                             # exit 0
$ git log -1 --format='%B' | grep -iE "co-authored|generated with|noreply@anthropic"
  → no match.  TRAILER-FREE.
```

---

## 4. FLAG 1 — the vectors. RULING: **LEGITIMATE, with a P3 condition.**

### 4.1 The trace's §7.6 was FALSE — confirmed, and the change was FORCED

The base vectors do contain `checked-artifacts`, hex-encoded at CBOR key 8:
`…0851636865636b65642d6172746966616374 73…` (`0x51` = bstr, length 17). The
trace ran a plain-text grep, which cannot see it. And the change was not
elective: `CatalogBootstrapRecordV1::decode_canonical` refuses any record whose
`final_name != catalog_component(CatalogPrivateNameV1::Final)`
(`protocol/catalog_bootstrap_record.rs:242`), so leaving the vectors alone
would have turned `semantic_vectors.rs` RED. **The builder found a real,
undocumented pin the trace missed. That is a credit, not a debit.**

### 4.2 I verified the round-trip claim myself, independently

I wrote my own deterministic-CBOR walker and re-emitter from the Rust source
alone (`record_id = SHA256(canonical_cbor(record with record_id = h''))`,
`catalog_bootstrap_record.rs:142` + `:259`), importing no production code and
not seeing the builder's script. Vectors loaded via
`git show ea3a924:protocol/checked-artifact-semantic-v1/vectors.txt` (base) and
`git show 4ba9071:…` (candidate).

```
provenance header @ea3a924: # provenance=independent-hand-authored-semantic-recipes;generator=none;regen-owned=false
provenance header @4ba9071: # provenance=independent-hand-authored-semantic-recipes;generator=none;regen-owned=false
header changed: False

[catalog-workspace-linux]
  final_name(orig) = 'checked-artifacts'
  STEP A round-trip on ORIGINAL: derived record_id == stored -> True
    stored  639b0881c4c01b79f22f735341ca8ab97fdeba70f24ae2765e7b0dbdc878c56d
    derived 639b0881c4c01b79f22f735341ca8ab97fdeba70f24ae2765e7b0dbdc878c56d
  STEP B re-derived vector == COMMITTED vector -> True
    new record_id df2919691a3ee1e72da7f580e5ec179e6f8487c958610c5888325cbfd793ce70
  fields that differ orig->committed: [8, 11]

[catalog-git-mac]
  STEP A round-trip on ORIGINAL: derived record_id == stored -> True
    stored  f956ce83f7a107a619b12968d5731152af2f65195395e8b560ffaafb6773b743
    derived f956ce83f7a107a619b12968d5731152af2f65195395e8b560ffaafb6773b743
  STEP B re-derived vector == COMMITTED vector -> True
    new record_id cbf6c7fd955bffaf3db8756757260e010457d48cb6fd0f066831c9c3b493e62e
  fields that differ orig->committed: [8, 11]

[catalog-workspace-windows]
  STEP A round-trip on ORIGINAL: derived record_id == stored -> True
    stored  c17d6467240fc788dfec65f6264cbba2082f9377b10fc2a02eedd565df815052
    derived c17d6467240fc788dfec65f6264cbba2082f9377b10fc2a02eedd565df815052
  STEP B re-derived vector == COMMITTED vector -> True
    new record_id 112253d1ec7b34bbc53491caaeddb11d942c738ee89753bef1270a81dcb3e53a
  fields that differ orig->committed: [8, 11]

ALL CHECKS PASS
```

**STEP A** validates the derivation against the untouched originals — the
safeguard the builder claims, independently reproduced. **STEP B** shows the
committed vectors are byte-for-byte what moving key 8 alone produces. **Only
keys 8 and 11 differ, in all three vectors, and no other vector in the file
changed.**

### 4.3 The ruling

**Script re-derivation is acceptable here, and the provenance header does not
need to move.**

1. The fixture's own README records that the ORIGINAL literals were produced by
   "a one-shot, standalone deterministic-CBOR encoder" that "did not import the
   Rust adapters, generated Taut code, or generated shape corpus and is not
   retained" (`README.md:14-18`). **"Hand-authored" in this fixture's own
   vocabulary means independently derived, not typed by a human.** The
   builder's ad-hoc, unretained Python re-derivation is the same class of act
   as the original authoring — not a departure from it.
2. The contract is explicitly the bytes, not the recipe: "The committed
   bytes—not a recipe—are the compatibility contract" (`README.md:18`). I
   audited the bytes and they are exactly right.
3. The header's three clauses each remain literally true:
   `independent-hand-authored-semantic-recipes` — the recipes (name / kind /
   coverage) are untouched and the literals were re-derived from them;
   `generator=none` — nothing is retained in-tree, exactly as before;
   `regen-owned=false` — `protocol/regen.py` still has no reference to the
   directory, asserted at `semantic_vectors.rs:210` and confirmed by me. The
   header claims nothing that this edit falsifies, so **moving it would break a
   compiled pin (`semantic_vectors.rs:56-59`) for no gain in truth.**
4. **No freeze the change must also move.** `vectors.txt` appears in no
   `PROTECTED_SOURCE_DIGESTS` / `PROTECTED_SOURCE_TREE_DIGESTS` table (both are
   `src/`-rooted) and in no other pin; `protocol/**` appears only as a
   *workflow trigger* in `retained-readers.yml`. The one pin over the file —
   the verbatim header check — is satisfied unchanged.

**The condition is [P3-1]:** legitimacy does not require hand re-derivation,
but auditability requires the reviewed derivation be recorded in the vectors'
own header. One `#` line, mechanically safe.

---

## 5. THE SEVEN-FLAG DISPOSITION TABLE

*Recording a limitation honestly: the builder's numbered flag list is not in
any filed artifact I could reach (no R1.1 delivery report exists in
`gwz-dev/dev-docs/`, and nothing in the worktree or scratchpad carries it).
The mandate numbers FLAGS 1, 4 and 5 and names "the cap flag"; I dispose of
those by number, and of the delivery's remaining self-flagged items by name so
nothing rides undisposed.*

| # | Flag | Disposition | Evidence |
|---|---|---|---|
| **1** | `vectors.txt` re-derived by ad-hoc script; trace's "not a semantic-vector event" falsified | **ACCEPTED — legitimate.** Change was FORCED by `catalog_bootstrap_record.rs:242`, not elective. Round-trip claim independently reproduced (§4.2). Header stays — all three clauses still true; moving it would break a compiled pin. No freeze must move. **Condition [P3-1]:** record the reviewed derivation in the vectors' own header | §4, `rev_vectors.py` transcript |
| **2**† | The cap: whole-line 250 unsatisfiable, flagged not trimmed | **CLEARED.** 394 whole handwritten added lines vs HARD **420**; code substance 178–200 vs the ~200 expectation; prose audit finds no rider (§6) | §6 |
| **3**† | The digest's ~57 read references deliberately NOT enumerated; one assertion per persisted field | **ACCEPTED.** Exactly what [RC-P2-1] ruled. The trade statement at `contracts.rs:183-199` names all three durable surfaces (`historical_collision_digest` key 4, `final_name` key 8, the scratch directory name) and the freedom ground. Not a rider | `contracts.rs:183-206` |
| **4** | Unattributed one-time remainder red in a batched run; timed out before naming the test; unreproduced ×3 | **NOT REPRODUCED — graded P3.** I ran the full batched `fault` battery once: all four legs green at 256 / 1 / 451 / 1097 (§3.4), plus a standalone remainder at 1097. Five clean runs now. **Grading honestly:** `run_battery` prints the failing blob's last 20 lines on FAIL (`run_r4bg_aggregate_gates.py:417-419`), so a genuine command failure *would* have named the test — "timed out before naming it" points at the observation being killed rather than the driver reporting FAIL. Residual risk low. **[P3-5]:** record the run's parameters so the first-dispatch leg captures full output. **Upgrade to P2 and attribute before landing if the builder's record shows the driver itself printed `FAIL`** | §3.4 transcript |
| **5** | Structural no-anchor guarantee as an in-tree source scan rather than a boundary-checker pin | **P1-1 — HOLE CONFIRMED.** Home is RIGHT (an assertion, where [RC-P3-1] asked for it, beside the claim). Shape is WRONG: spelling-specific. Two compiling production callers (`use` import; alias) evade it; a same-file matching-spelling control turns it RED naming the file; a blinded scan root also turns it RED (anti-vacuity property is genuinely good and credited) | §2 [P1-1], probes A/B/C/D |
| **6**† | linux 461 DERIVED, not measured; Windows arm owed | **ACCEPTED.** Correctly marked DERIVED / FIRST-DISPATCH-EXPECTED with "A measured number wins", placed beside the existing WINDOWS-ARM-OWED note so the dispatch cannot forget one and remember the other. The four new rows carry no `cfg` gate — I read all four | `run_r4bg_aggregate_gates.py:267-290` |
| **7**† | `GitDirectoryArtifact` arm is symmetry, not behaviour ([P3-7]); scratch stem unchanged ([R2-P3-3]) | **ACCEPTED.** Both stated where §1 requires. Verified: `entry.rs:182` is the only production construction site, reached only from `observe_merge_preservation_git_directory`, which never mutates; `catalog/scratch.rs:6`'s stem is compared only against itself | `policy.rs:41-45`, `catalog_names.rs:52-56` |

† Flag numbers 2/3/6/7 are my mapping onto the delivery's self-flagged items,
not the builder's numbering, which I could not obtain. Every self-flagged item
I could find in the commit and the tree is disposed of above.

---

## 6. CAP, PROSE AUDIT, SCOPE, TRAILERS

### 6.1 My own whole-line count

Counting added lines per the lane owner's basis (whole handwritten lines
including comments, generated excluded):

```
 148 tot |   97 code   37 cmt  14 blank   src/checked_artifact/catalog/bootstrap/tests.rs
  49 tot |    4 code   45 cmt   0 blank   src/checked_artifact/catalog_names.rs
  45 tot |   19 code   24 cmt   2 blank   src/checked_artifact/interface_tests/contracts.rs
  26 tot |    0 code   26 cmt   0 blank   src/git/tests/g15/root_preservation/max_path.rs
  25 tot |   23 code    0 cmt   2 blank   scripts/checks/run_r4bg_aggregate_gates.py
  21 tot |   18 code    3 cmt   0 blank   src/git/tests/g15.rs
  16 tot |    3 code   13 cmt   0 blank   src/checked_artifact/policy.rs
  15 tot |    9 code    6 cmt   0 blank   src/git/gitbackend/repository_support.rs
  13 tot |    3 code   10 cmt   0 blank   src/git/gitbackend/preservation_image.rs
  11 tot |    5 code    6 cmt   0 blank   src/git/tests/g12.rs
   8 tot |    6 code    2 cmt   0 blank   src/checked_artifact/interface_tests/catalog_recovery.rs
   7 tot |    3 code    4 cmt   0 blank   src/git/tests/g15/root_preservation/stash.rs
   4 tot |    4 code    0 cmt   0 blank   scripts/checks/check_checked_artifact_boundaries.py
   3 tot |    3 code    0 cmt   0 blank   protocol/checked-artifact-semantic-v1/vectors.txt   (GENERATED)
   2 tot | …grammar.rs / production_tests.rs / catalog_lease/tests.rs / preflight.rs (6 total)

TOTAL added: 397     generated (vectors.txt): 3
CAP BASIS: 394  vs HARD 420  →  INSIDE, 26 to spare
```

**Code substance:** the naive classifier scores 200, but 22 of the driver's 25
added lines are docstring prose inside `_fault_count`'s `"""…"""`, not code.
Correcting for that: **code ≈ 178, prose ≈ 198, blank 20.** Either way at or
below the plan's "~200 code-substance expectation". The lane owner's 420 reset
was correctly sized: prose is genuinely half the delivery, and it is mandated
prose.

### 6.2 Prose audit — nothing beyond the mandate rides

Every added comment block maps to a named §3/[RC-*] item:

| Block | Mandate item |
|---|---|
| `catalog_names.rs` ordering-ground para | §3 "ordering ground (pre-activation freedom)" |
| `catalog_names.rs` "Legacy stays inside `ALL`" para | §1 name-set coupling / [RC-P1-1] |
| `catalog_names.rs` ASCII para, three sites | [RC-P3-2] |
| `catalog_names.rs` scratch-stem para | [R2-P3-3] / §1 |
| `catalog_names.rs` exhaustive-match para | [RC-P3-4] second bullet, verbatim |
| `policy.rs` split para + GitDirectory-arm para | §3 pinning; [P3-7] |
| `contracts.rs` THE TRADE + digest-constant doc | [RC-P2-1] |
| `preservation_image.rs` catalog ground | [RC-P3-3] |
| `repository_support.rs` two-grounds block | [RC-P3-3] |
| `max_path.rs` module doc, three arms | [P3-2] |
| `catalog/bootstrap/tests.rs` decisive-row + scan + recognized-by-nothing docs | [R2-P2-1], [RC-P3-1], [R2-P3-2] |
| `g12` / `g15` / `stash` both-path notes | §3 fixture extension |
| `run_r4bg_aggregate_gates.py` provenance block | §3 "driver's dated provenance block" |

**No rider found.** The one thing that could read as unmandated — `max_path.rs`
correcting the pre-existing `198` to `197` — is forced by §3's "MAX_PATH
constants re-measured", and I verified the new arithmetic is the correct one
(§1.2). Nothing self-promotional, no scope narration, no unrelated cleanup.

### 6.3 Scope — 18 files, each justified

Split core (`catalog_names.rs`, `policy.rs`); charter-named pins
(`contracts.rs` 4→5 + per-persisted-field, `catalog_recovery.rs` the `ALL`/
policy pin); decisive rows (`catalog/bootstrap/tests.rs`); exemption surfaces
(`preservation_image.rs`, `repository_support.rs`); charter-named fixtures
(`g12.rs`, `g15.rs`, `stash.rs`, `max_path.rs`); charter-named companions
(`check_checked_artifact_boundaries.py`, `run_r4bg_aggregate_gates.py`);
leaf-rename fallout in four existing test fixtures
(`catalog_lease/tests.rs`, `catalog_tests/grammar.rs`,
`catalog_tests/preflight.rs`, `production_tests.rs`); and `vectors.txt`,
forced by `catalog_bootstrap_record.rs:242` (§4.1). **All 18 justified; no
stray file.**

### 6.4 Trailers

None. Verified by direct grep of the commit body (§3.5).

---

## 7. EXEMPTION SURFACES — both paths, distinct grounds, load-bearing, not over-broad

Both grounds are written and genuinely distinct: the legacy path's is
anchor-specific (a Windows durability anchor held for the life of the
repository, `repository_support.rs:108-114`); the catalog's is its own
(exact-managed product state the user never authored, plus the structural
impossibility of an anchor, `preservation_image.rs:9-19`). [RC-P3-3]
satisfied.

I did not take the fixture extensions on trust. Four probes:

**PROBE E — drop `CATALOG_PRIVATE_PATH` from the DIRT classifier only**
(`repository_support.rs::checked_artifact_private_status_path`):

```
test git::tests::g12::checked_rollback_tolerates_checked_artifact_private_residue ... FAILED
      panicked at src/git/tests/g12.rs:841:10
test git::tests::g15::complete_checkout_excludes_only_… ... ok
test git::tests::g15::root_preservation::stash::create_stash_remains_exact_over_… ... ok
```
→ the g12 extension is **load-bearing** for dirt classification.

**PROBE F — drop `CATALOG_PRIVATE_PATH` from the PRESERVATION-IMAGE blindness
only** (`preservation_image.rs::raw_excluded_paths`):

```
test git::tests::g15::complete_checkout_excludes_only_… ... FAILED   (g15.rs:502)
test git::tests::g15::root_preservation::stash::create_stash_remains_exact_over_… ... FAILED   (stash.rs:321)
test git::tests::g12::checked_rollback_tolerates_… ... ok
```
→ preservation blindness is **load-bearing in two independent fixtures**.

**PROBE G1 — my own scenario extension (the mandate's hazard): nested residue
under the OLD path AND under `catalog-final`, through a checked rollback:**

```rust
fs::create_dir_all(private.join("sub")).unwrap();
fs::write(private.join("sub/legacy-nested"), b"legacy nested\n").unwrap();
fs::create_dir_all(catalog.join("sub")).unwrap();
fs::write(catalog.join("sub/catalog-nested"), b"catalog nested\n").unwrap();
…
assert_eq!(fs::read(private.join("sub/legacy-nested")).unwrap(), b"legacy nested\n");
assert_eq!(fs::read(catalog.join("sub/catalog-nested")).unwrap(), b"catalog nested\n");
```
```
test git::tests::g12::checked_rollback_tolerates_checked_artifact_private_residue ... ok
```
→ **both paths stay exempt and unmodified at depth.**

**PROBE G2 — the boundary: prefix-siblings must NOT inherit the exemption:**

```rust
fs::write(repo.join(".gwz/catalog-final-extra"), b"user work\n").unwrap();
fs::write(repo.join(".gwz/checked-artifacts-extra"), b"user work\n").unwrap();
```
```
test … ... FAILED
called `Result::unwrap()` on an `Err` value: ModelError { code: DirtyMember,
  message: "rollback requires a clean index and worktree", … }
```
→ the exemption is **prefix-EXACT, not over-broad**: `.gwz/catalog-final-extra`
correctly stays user dirt. The `suffix.is_empty() || suffix.starts_with('/')`
guard is doing real work for the new constant as well as the old.

All four probes reverted; tree verified clean.

---

## 8. ONE-LINE MANDATE DISPOSITIONS

1. **Design fidelity row-by-row against §1/§3** — PASS; the two consumers are
   genuinely split, every runtime path traced, **no consumer on the wrong
   side, no P0**.
2. **Decisive test genuinely executable; my own falsification** — PASS; it
   fails under the reverted shared-name design exactly as claimed, **but** the
   charter's literally named "still recoverable" line is non-discriminating on
   darwin/linux and the stated `ca1-*` mechanism is Windows-only → **[P2-1]**.
3. **FLAG 1 (vectors)** — **LEGITIMATE**; change was forced not elective,
   round-trip independently reproduced, header stays true and pinned, no
   freeze to move; record the derivation in the header → **[P3-1]**, and route
   the trace's §7.6 falsification to R1.3 → **[P3-2]**.
4. **FLAG 4 (one-time remainder red)** — **NOT REPRODUCED** in a full batched
   run (256/1/451/1097 all green); risk graded low with the reasoning stated
   and an explicit upgrade condition → **[P3-5]**.
5. **FLAG 5 (structural no-anchor scan)** — home right, shape wrong;
   **evadable by `use` import and by alias, proved with compiling code and a
   same-file control** → **[P1-1]**; anti-vacuity property credited.
6. **FLAGS 3/6/7 + cap flag** — all ACCEPTED/CLEARED with evidence (§5).
7. **Companions** — four checker digests recomputed byte-for-byte and checker
   green on pristine; 451 MEASURED by me; remainder 1097 UNMOVED and the
   g12/g15/stash edits confirmed body-only (zero `#[test]` added there).
8. **Exemption surfaces** — both paths, distinct grounds, each surface proved
   load-bearing by a mutation probe, and proved prefix-exact by my own
   extension.
9. **Cap / prose / scope / trailers** — 394 vs 420 INSIDE; no rider in the
   prose; 18 files all justified; trailer-free.

---

## 9. CONDITIONS FOR GO

1. **[P1-1]** Make the no-anchor scan spelling-blind (match the bare
   identifier, over-eager toward RED, per the R1.2 precedent), keeping the
   list-equality anti-vacuity property; and push the relative path rather than
   the bare file name.
2. **[P2-1]** Correct the decisive row's doc comment to state the mechanism
   accurately — the `ca1-*` interior refusal is the Windows arm; on
   non-Windows `prepare_private` is a no-op and the disjointness assertion at
   `:606` is what discriminates.
3. **[P3-1]** One dated `#` line in `vectors.txt`'s header recording the
   reviewed re-derivation.
4. **[P3-2]** Route the trace §7.6 falsification to R1.3's records train.
5. **[P3-3] / [P3-4] / [P3-5] / [P3-6]** at the lane owner's discretion —
   all prose or record-keeping, none blocking.

The split itself, its coupling decision, the companions and the exemption
surfaces need no change.

---

# ROUND 2 — verification (focused, round 2 of 2)

**Verified at:** `1e2a106a98902d15b827488cc9e18b6f621eb96c` (branch
`r2f/r1-1-split`), the tip after the coordinator's addendum. Fold =
`3d417f7` (4 files, +59/−32); digest companion = `1e2a106` (1 file, +2/−2).
Probes A/B/C were executed at `3d417f7`; the companion changes only two hex
literals in `check_checked_artifact_boundaries.py` and no Rust, so those
transcripts hold unchanged at the tip — D, E and F were run at the tip.
Worktree verified clean at the tip after every probe; read-only outside it;
`CARGO_TARGET_DIR` unchanged; no commits; no trailers.

## R2.1 — [P1-1] the spelling-blind scan: DISCHARGED

`production_callers_of_prepare_private` → `production_namers_of_prepare_private`
(`catalog/bootstrap/tests.rs:541-581`): bare-identifier
`code.contains("prepare_private")` over `//`-stripped production sources,
definitions plus uses, relative paths pushed, list-equality kept, expected set
`["platform.rs", "residue.rs"]`.

**Scan reach independently confirmed.** Every file under
`src/checked_artifact/` naming the identifier at all:

```
src/checked_artifact/catalog/bootstrap/tests.rs      (excluded: `tests*` stem — the scanner itself)
src/checked_artifact/platform.rs                     (definer ×2, :345 / :688 → in the list)
src/checked_artifact/residue.rs                      (the one caller, :102 → in the list)
src/checked_artifact/tests/leaf_publication.rs       (excluded: under a `tests` directory)
```

The expected set is exactly right, and `platform.rs` appears **once** — one
`Vec` entry per file, so its `:437-439` doc mentions cannot double it by
construction.

**The new `_tests.rs` exclusion audited** (it is fresh attack surface, and the
R1.2 [P3-1] lesson is that a new exclusion can blind a scan). It removes
exactly four files, all `#[cfg(test)]` at their declaration
(`capability/pre_catalog/provider.rs:313-320`), and **none names
`prepare_private` today**:

```
capability/pre_catalog/provider/catalog_tests.rs
capability/pre_catalog/provider/directory_mutation_tests.rs
capability/pre_catalog/provider/mutation_tests.rs
capability/pre_catalog/provider/production_tests.rs
```

**PROBE A — `use` import, the round-1 evasion. Was GREEN; now RED.**

```rust
use super::platform::prepare_private;
pub(super) fn probe_use_import(dir: &cap_std::fs::Dir) -> crate::model::ModelResult<()> {
    prepare_private(dir, true, crate::model::ErrorCode::UnsupportedOperation, "probe")
}
```
```
test …a_legacy_drive_after_bootstrap_leaves_the_catalog_recoverable ... FAILED
panicked at src/checked_artifact/catalog/bootstrap/tests.rs:640:5:
  left: ["observation.rs", "platform.rs", "residue.rs"]
 right: ["platform.rs", "residue.rs"]
```

**PROBE B — alias, the round-1 evasion. Was GREEN; now RED.**

```rust
use super::platform::prepare_private as pp;
pub(super) fn probe_alias(dir: &cap_std::fs::Dir) -> crate::model::ModelResult<()> {
    pp(dir, true, crate::model::ErrorCode::UnsupportedOperation, "probe")
}
```
```
  left: ["observation.rs", "platform.rs", "residue.rs"]
 right: ["platform.rs", "residue.rs"]
```

**PROBE C — qualified call, same file. Stays RED**, naming the offending file:

```
  left: ["observation.rs", "platform.rs", "residue.rs"]
 right: ["platform.rs", "residue.rs"]
```

**PROBE D — blinded scan root (root repointed to
`src/checked_artifact/catalog`). Stays RED via list inequality**, so the
anti-vacuity property survives the rewrite:

```
  left: []
 right: ["platform.rs", "residue.rs"]
```

**PROBE E (adversarial, the coordinator's ask) — comment-only mention in a
third production file must NOT trip it.** Appended to `catalog_names.rs`:

```rust
// This plain comment names prepare_private.
/// This doc comment also names prepare_private.
```
```
test …a_legacy_drive_after_bootstrap_leaves_the_catalog_recoverable ... ok
```

**GREEN.** The `split_once("//")` strip handles `//` and `///` alike (a `///`
line strips to empty), so prose naming the identifier — including
`platform.rs:437-439`'s — cannot move the list.

**PROBE F (my own adversarial extension) — the one residual of a textual
strip, and it is CLOSED by the lane gate.** A real call placed on the same
physical line as, and after, a `//` occurring inside a *string literal*
evades the strip:

```rust
let _doc = "https://example.invalid/x"; super::platform::prepare_private(dir, true, …)
```
```
test …a_legacy_drive_after_bootstrap_leaves_the_catalog_recoverable ... ok     ← false green
```

But `cargo fmt` splits that statement, and the lane gate runs
`cargo fmt --check` on every commit:

```
$ cargo fmt --check
Diff in …/src/checked_artifact/observation.rs:376:
-    let _doc = "https://example.invalid/x"; super::platform::prepare_private(dir, true, …)
+    let _doc = "https://example.invalid/x";
+    super::platform::prepare_private(
…
$ cargo fmt && cargo test --lib -p gwz-core a_legacy_drive_after_bootstrap
  left: ["observation.rs", "platform.rs", "residue.rs"]
 right: ["platform.rs", "residue.rs"]                     ← RED once formatted
```

**The hazard is unreachable in a fmt-gated tree.** Recorded as a closed
observation, not a finding. No further condition.

**Disposition: [P1-1] DISCHARGED.** Both round-1 evasions now fail loudly, the
control still names the file, anti-vacuity is preserved, the comment strip
works as intended, and the only residual is foreclosed by an existing gate.

## R2.2 — [P2-1] the platform-accurate decisive-row mechanism: DISCHARGED

`catalog/bootstrap/tests.rs:591-601` now reads: "The killing teeth there differ
by platform ([R1.1-P2-1]): on Windows the drive plants a `.ca1-*` anchor that
makes re-observation refuse (`provider/interior.rs:437-440`); on darwin/linux
`prepare_private` is a no-op (`platform.rs:344-351`) and a clean drive removes
its write-ahead names, so what discriminates is the disjointness assertion
below — the legacy area appears as its OWN directory — not the recoverability
lines."

**This matches my round-1 PROBE 2/2b exactly**, clause by clause: PROBE 2
showed the recoverability lines pass under the dead design once the
disjointness proxies are removed; PROBE 2b showed
`interior-after-drive == published-at-bootstrap` and
`legacy-dir-exists = false`, i.e. no residue survives a clean drive; and
`platform.rs:344-351` is the `#[cfg(not(windows))] … Ok(())` no-op I cited.
Both citations are correct. The claim is now true on every platform.
**DISCHARGED.**

## R2.3 — [P3-1] the vectors' provenance record: DISCHARGED

`vectors.txt` line 3 (after the two pinned lines, before the column header)
carries the dated record: R2-F R1.1, the reviewed-edit basis (`README:26-28`),
this review's FLAG-1/[P3-1] ruling, the three vectors by name,
`final_name` key 8 and its dependent `record_id` key 11, the round-trip
validation of the ORIGINALS, and "no generator retained".

Both mechanical requirements verified empirically, not by inspection:

```
$ cargo test --lib -p gwz-core independent_semantic_vectors
test checked_artifact::interface_tests::semantic_vectors::independent_semantic_vectors_bounded_decode_and_reencode_exact_literals ... ok
```

The `starts_with` header pin (`semantic_vectors.rs:56-59`) covers only the
first two lines and is untouched; `vectors()`' `!line.starts_with('#')` filter
(`:62`) drops the new line. **DISCHARGED.**

## R2.4 — [P3-3] / [P3-4] the two prose corrections: DISCHARGED

- **[P3-3]** `preservation_image.rs:13-14` now reads "admits only the ten
  infrastructure slot names **and the catalog's own action rows — exact-managed
  either way**". That is exactly the gap I named: `interior.rs:438-441` admits
  `CatalogRootRowClassV1::ActiveAction(_)`, and the widened wording keeps the
  ground ("exact-managed") intact. **DISCHARGED.**
- **[P3-4]** `repository_support.rs:108-117` is now two-area accurate — plural
  throughout, both areas named with their distinct grounds (legacy = the
  Windows durability anchor; catalog = exact-managed product state), pointing
  at `checked_artifact_private_status_path` where both are stated in full.
  **DISCHARGED.**

## R2.5 — the digest companion at `1e2a106`: VERIFIED

The fold moved `catalog/bootstrap/tests.rs` (under `catalog.rs`'s TREE digest)
and `preservation_image.rs` (a FLAT allowlist digest), so both pins had to
follow. I recomputed all six protected digests in the two tables that touch
this change, from the pristine folded tree, with my own implementation of the
checker's algorithms:

| Pin | Kind | Committed | Recomputed | |
|---|---|---|---|---|
| `git/gitbackend/preservation_image.rs` | flat | `b45057e1…bac5` | `b45057e1…bac5` | MATCH |
| `checked_artifact/catalog.rs` | tree | `05918c36…e670` | `05918c36…e670` | MATCH |
| `checked_artifact/bootstrap/runtime/catalog_lease.rs` | tree | `91ac3dfa…b0b2` | `91ac3dfa…b0b2` | MATCH (unmoved) |
| `checked_artifact/capability/pre_catalog.rs` | tree | `d9236742…d045` | `d9236742…d045` | MATCH (unmoved) |
| `checked_artifact/capability/path.rs` | tree | `23e46dbd…a91c6` | `23e46dbd…a91c6` | MATCH (unmoved) |
| `checked_artifact/platform.rs` | tree | `5405b692…b366` | `5405b692…b366` | MATCH (unmoved) |

Exactly two moved — the two the fold touched — and the four that should not
move are genuinely unchanged.

**Checker on the pristine tree at the tip:**

```
$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
EXIT=0
$ git status --porcelain      # empty — the checker does not dirty the tree
```

## R2.6 — gates re-run by me at `1e2a106`

```
$ cargo fmt --check                                   → exit 0
$ cargo clippy --all-targets -- -D warnings           → clean
$ cargo test --lib -p gwz-core checked_artifact::     → 451 passed
$ cargo test --lib -p gwz-core -- --skip checked_artifact:: \
      --skip workspace_ops::merge::v1_lifecycle::     → 1097 passed; 1 ignored
$ git diff 4ba9071 1e2a106 -U0 | grep -c "^+#\[test\]"  → 0
```

451 and 1097 both **unchanged from the delivered candidate**, and the fold adds
zero test rows — so the aggregate pins 451/461 and the remainder 1097/1098 need
no further movement. `git log -1 --format='%B' | grep -i co-authored` → no
match; **trailer-free**.

## R2.7 — CAP: the rider audit, and my ruling

**Measurement (mine, at the tip vs base `ea3a924`):**

```
TOTAL added lines: 430
  generated (regenerated hex data lines in vectors.txt): 3
  CAP BASIS (whole handwritten added lines, generated excluded): 427
```

**427 confirmed** — identical to the coordinator's disclosure. Note the
`vectors.txt` line count is 4, of which **3** are generated hex and **1** is the
handwritten [P3-1] header line; only the 3 are excluded. Composition after
correcting for the driver's 22 docstring-prose lines and the vectors header
line (both of which a naive classifier scores as "code"): **code ≈ 187, prose
≈ 222, blank 18** — code substance still at or below the plan's ~200
expectation. The overage is entirely prose, and entirely cure prose.

**The +33 accounts for exactly, with no residue:**

| File | Δ added | Condition cured |
|---|---|---|
| `catalog/bootstrap/tests.rs` | +22 (148→170) | [P1-1] scan rewrite + its rationale doc; [P2-1] decisive-row doc; the assertion reshape the rename forces |
| `repository_support.rs` | +9 (15→24) | [P3-4] two-area doc |
| `preservation_image.rs` | +1 (13→14) | [P3-3] action-rows widening |
| `vectors.txt` | +1 (3→4) | [P3-1] provenance record |
| `check_checked_artifact_boundaries.py` | 0 (4→4) | the two digests rewrite lines the delivery already moved and counted |
| **Total** | **+33** | 394 + 33 = **427** ✓ |

**Rider audit — line by line, every added line is condition-mandated:**

- **Scan doc (13 lines).** Five clauses: why spelling-blind (the R1.2 [P1-1]
  precedent and the three spellings); the over-eager-toward-RED direction; the
  `pub(super)` exhaustiveness premise (**retained from the delivery**, not new);
  the comment strip and the file whose prose would otherwise trip it; the
  `_tests.rs` exclusion and why it is safe. The last two document **attack
  surface the cure itself introduces** — documenting them is not optional, and
  I had to audit both to discharge [P1-1]. Proportionate, not decorative.
- **Scan body.** Root-threading, the `continue` restructure, the exclusion
  clause, the strip, the relative-path push (my own round-1 sub-point). All
  mechanism.
- **Decisive-row doc (+8/−3).** The platform split with its two citations.
  Exactly my [P2-1] remedy, no more.
- **Assertion block (+4/−3 comment, reshaped `assert_eq!`).** "exactly ONE
  production caller" → "the only production files NAMING it are its definer and
  its one caller" — forced by the semantic change; the multi-line `assert_eq!`
  is `cargo fmt`'s doing.
- **`repository_support.rs` (+9/−6).** Singular → plural with both grounds
  named and a pointer to where each is stated. Exactly my [P3-4] remedy.
- **`preservation_image.rs` (+1).** Exactly my [P3-3] remedy.
- **`vectors.txt` (+1).** Exactly my [P3-1] remedy, and every element I
  specified is present.

**No rider. Nothing beyond the round-1 conditions rides in the fold**, and
nothing self-promotional, unrelated, or decorative. I also find no cure line
that could be trimmed without losing content a future reader needs, so the
flag-not-trim ruling was applied correctly rather than used as cover.

**CAP RULING: the overage is warranted. I verify the HARD cap should be
re-ruled 420 → 427-MEASURED for R1.1**, on the same basis the lane owner used
for the 250 → 420 reset: the mandated volume is prose, the prose is
condition-mandated cure, and code substance (~187) remains inside the ~200
expectation. The delivered candidate was compliant at 394; the overage is
purely the price of the review's own conditions, which is the one thing a cap
must never punish. Record it in the plan at landing with this verification
cited.

## R2.8 — outstanding P3s

`[P3-2]` routed to R1.3's records train — accepted. `[P3-5]` gets its dated
line in the landing record; the builder's record says the red run "timed out
before naming the test", i.e. the observation was killed rather than the driver
printing `FAIL`, so **my stated upgrade condition is not met and it stays P3**.
`[P3-6]` stands as recorded. None blocks.

---

# FINAL VERDICT: **GO**

All five round-1 conditions discharged and independently verified: the
no-anchor scan is now spelling-blind and both round-1 evasions fail loudly,
with anti-vacuity preserved and the one residual foreclosed by the existing
`cargo fmt` gate; the decisive row's mechanism is platform-accurate and matches
what my probes established; the vectors carry their dated provenance record
with both pins still green; the two prose corrections are exact. The digest
companion moves exactly the two pins the fold touched and both recompute
byte-for-byte, with the checker green on the pristine tree. Gates re-run by me:
fmt clean, clippy `-D warnings` clean, `checked_artifact::` 451, remainder
1097 + 1 ignored, zero test rows added, trailer-free. Cap at 427 measured,
audited rider-free, and ruled warranted.

**0 P0 / 0 P1 / 0 P2 / 3 P3 carried** (`[P3-2]` routed to R1.3, `[P3-5]` a
landing-record line, `[P3-6]` recorded) — none blocking.

**R1.1 is clear to land at `1e2a106a98902d15b827488cc9e18b6f621eb96c`.**
