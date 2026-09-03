# Windows namespace anchor-readiness class — diagnosis and repair

Package scope: the four `checked_artifact::namespace::tests_fault_matrix`
failures of Windows matrix run 16 (`32559979514` on `6c7c8f3`), recorded in
`GwzWindowsMatrix-Classification.md`, plus the managed matrix that entry
pre-attributes to the same cause. Diagnosed and repaired against gwz-core main
`8b83a2c`; the repair is left uncommitted in the live tree for the lane owner to
land. Native validation is done: probe run `32569434565`, green.

**Round 2 (2026-08-22).** Code-axis review returned **GO** with 1 P1, 4 P2,
2 P3 (`GwzWinNamespaceAnchor-Review.md`). This document is the remediated
version: [P2-1] corrected in code and recorded in §4, [P2-2] discharged by the
new §9 annotation, [P2-4] folded into that annotation's negative space, [P3-1]
and [P3-2] fixed in place, and [P1-1]'s second pin disclosed in §5. [P1-1]'s
recompute and [P2-3]'s tracked acceptance item are the lane owner's at landing
and are stated as owed in §5.

## Verdict

One cause, and it is a **production** gap, not a fixture gap. P5's Windows arm
(the durability-anchor round trip) carries an unstated precondition — the
directory must be the *anchored private area* — and edge E14 is the first caller
in the tree that does not satisfy it. The repair makes the precondition a typed
caller fact and gives the other class the documented Windows arm the freeze's
E9 annotation already established as the sanctioned form.

**No new primitive row, no frozen semantics moved.** E14 still routes through
the admitted P5 family entry `platform::private_barrier`; only that function's
Windows column gains a second arm, which §4.3 states explicitly is "a
protocol-typed extension of an admitted primitive, not a new one". No
publication call site is added or removed, no fault key is minted, and the
non-Windows behaviour of every caller is byte-identical.

## 1. The symptom

All four `tests_fault_matrix` tests (both matrices × both target variants) die
on windows-latest at **baseline**, before any fault injection, at
`tests_fault_matrix.rs:376` — the first `settle()` of
`run_interruption_matrix`/the repeated-crash driver — with one deterministic
typed message:

```
Ambiguous { fact: "action namespace barrier",
            detail: "checked action namespace barrier: private durability
                     anchor is not ready" }
```

macOS and Linux are green on the same code because the whole mechanism below is
`#[cfg(windows)]`.

## 2. Mechanism, proven from the code

The drive reaches the barrier at `tests_fault_matrix.rs:320`
(`namespace.barrier_namespace(&parent, &slots)`), which resolves through
`namespace/mod.rs:275` → `namespace/host.rs:667` → `namespace_mutation.rs:316`
`NamespaceOwner::barrier`, and that calls
`platform::private_barrier(&self.handle, …)` on **the retained action
directory** — the one directory this owner holds for its whole life, reached by
one identity-proved no-follow hop from the permit-retained completed catalog.

`platform::private_barrier`'s two arms are not equivalent:

- `platform.rs:271` (`cfg(not(windows))`) is `sync_parent(dir)` — a directory
  `fsync`, which requires nothing of the directory's contents;
- `platform.rs:479` (`cfg(windows)`) is the anchor round trip, and its first two
  statements are

  ```rust
  prepare_private(dir, false, code, label)?;
  let AnchorState::Ready { final_name } = anchor_state(dir, code, label)? else {
      return Err(error(code, label, "private durability anchor is not ready"));
  };
  ```

The refusal is therefore reached deterministically, and the exact path is
forced:

1. `anchor_state` (`platform.rs:537`) enumerates the directory. It classifies an
   entry as an anchor only by the `ANCHOR_PREFIX` `.ca1-durability-anchor-`
   (`:403`), and as "family state" only by a `ca1-` prefix. An action
   directory's rows are schedule-derived slot leaves, so the scan yields
   `anchors.is_empty()` and `family_state == false` →
   `AnchorState::Missing { family_state: false }`.
2. In `prepare_private` the arm
   `AnchorState::Missing { family_state } if !family_state && !create => Ok(())`
   matches, because the barrier passes `create = false`. So preparation
   **succeeds without creating anything**.
3. The barrier then re-reads `anchor_state`, gets `Missing` again, and the
   `let … else` fires. Typed, closed, and unconditional.

So the readiness evidence the barrier demands is a *resident
`.ca1-durability-anchor-<32hex>` file*, and the only producer of that file is
`prepare_private(dir, create = true, …)`, whose one caller is
`residue.rs:102` — the checked-artifact private area's own `open_private`. The
initial hypothesis (the readiness proof is produced by a flush routed through a
Windows no-op arm) is **not** the mechanism: no flush is involved in the
readiness predicate at all. `sync_parent`'s Windows no-op (`platform.rs:393`) is
why the anchor round trip exists, not why it fails. The failure is a missing
*precondition*, and the precondition is unstated in the signature: every one of
P5's seven pre-existing production call sites (`cleanup.rs:89/:129/:159`,
`residue.rs:407/:500/:529`, `transition.rs:427`, plus the internal
`platform.rs:430`) operates on an already-anchored private area, so the
precondition held silently until E14 landed. Step 2.2's review recorded E14 as
routing "through the admitted P5 family entry … the already-spiked shared
primitives"; the spike covered the primitive, not this precondition.

## 3. Why the anchor cannot simply be prepared there

Ruled out from the code, not by preference:

- **Exactness.** The anchor is permanent by design — `finish()` never removes
  it (`GwzWindowsMatrix-ExactEvidenceDiagnosis.md` §3 Class B, and the E22 row
  that schedules its retirement). An action directory admitted with a permanent
  extra child fails admission's own exactness predicate, which requires
  `extra_children: 0` (`protocol/admission/owner.rs:29-38`). Preparing an anchor
  in the directory the barrier protects would destroy the property the barrier
  exists to serve.
- **Contamination.** The catalog private root is `.gwz` / `gwz`
  (`catalog_names.rs:14-15`), *not* `.gwz/checked-artifacts`, which is the only
  path the preservation-image model excludes
  (`preservation_image.rs:8`). Planting anchors under the catalog would
  reproduce, in a new place, the exact-evidence contamination class already
  diagnosed and fixed for the private area.
- **Stop clause.** The anchor's creation path mints a random scratch name
  (`platform.rs:440`), which §4.3's E22 row names as "the R2 stop-clause
  violation this step removes". A fix that spreads it is moving in the wrong
  direction.
- **The transient variant closes too** (create the anchor, round-trip it, remove
  it, all inside the barrier). It mints the same random scratch name on every
  barrier rather than once per area, so it multiplies the stop-clause violation
  instead of avoiding it, and the removal would itself need a barrier to be
  durable — the regress the anchor exists to terminate. Worse, it converts a
  crash window into a permanent defect: a stop between create and remove leaves
  a resident anchor in that action directory, and admission's `extra_children:
  0` then refuses that action **forever** — strictly worse than the typed,
  retryable refusal being fixed.

Test-side correction was also ruled out: the fixture builds a real admitted
action directory through the sealed catalog owner and the Phase 1 admission
seam, which is exactly what production builds. Any fixture change that made the
barrier pass would have to plant an anchor production never plants — it would
hide the class rather than fix it. Production on Windows fails here too.

Re-routing E14 to `sync_directory_edge` (whose Windows arm is already a
documented no-op, `directory_mutation.rs:734`) was ruled out as well: that
function belongs to family **P2**, as §4.1's P2 row states explicitly, so the
edge would silently move families. That is the STOP condition, and it is
avoided.

## 4. The fix

**Classification: production arm.** One writer-class-conditional Windows arm of
P5, plus the caller-side class declaration.

- `platform.rs` gains `DirentBarrierClass` with two variants —
  `AnchoredPrivateArea` (the checked-artifact private area, which retains the
  anchor as product infrastructure) and `ExactInterior` (a directory whose
  children are exact evidence and which may therefore retain none) — and
  `private_barrier` takes it. The distinction is a *caller* fact by
  construction: the directory's own contents must not be trusted to reveal it,
  since "no anchor resident" is precisely the state both a legitimate exact
  interior and a damaged private area present.
- The `cfg(not(windows))` arm ignores the class entirely: both classes remain
  the same directory `fsync`. macOS and Linux behaviour is unchanged for every
  caller, which is why the macOS gates move by zero tests.
- The `cfg(windows)` arm returns `Ok(())` for `ExactInterior` **before**
  `prepare_private`, with the full argument recorded inline: the round trip is
  unavailable rather than skipped; the substituting property is the P2 family's
  own (`FILE_FLAG_WRITE_THROUGH` writers plus the sealed exact-handle rename,
  and no portable directory flush on Windows); the property is
  writer-class-conditional exactly as E9's is, and the FOREIGN-writer negative
  space is the same one E9's annotation already records — an exact interior
  admits no foreign row by construction.
- The seven legacy call sites pass `AnchoredPrivateArea`, so their behaviour —
  including the "not ready" refusal, which remains a real signal for a private
  area that lost its anchor — is preserved exactly.
- `namespace_mutation.rs`'s `barrier` passes `ExactInterior` and states why in
  the doc comment.
- **The E9 residual-ordering claim the arm falsifies is corrected wherever it is
  stated** (Code review [P2-1], round 2). E9's annotation records that a
  FOREIGN-written leaf keeps "namespace ordering via the E10/P5 anchor
  round-trip only" on Windows; after this arm E10 has no anchor path at all, so
  the residual is *empty*, not weaker. Corrected at five sites, no behavioural
  change: `leaf_observation.rs`'s Windows flush arm (whose conclusion rested on
  the vanished ordering); `authority_record_binding.rs`'s module doc — the
  freeze's designated carrier of the E9 condition — which now records the
  superseded clause explicitly beside the quotation; the
  `ObservedLeafWriterClassV1::Foreign` doc; `require_authority_strength`'s doc;
  and `platform.rs`'s own arm, which had argued its negative space *by citing
  the record it falsifies* and now states it directly. One shipped error string
  moves with them, `FOREIGN_AUTHORITY_REFUSAL`: "a foreign-written leaf's
  durable proof is namespace ordering only on this platform" → "a
  foreign-written leaf carries no durable proof on this platform". No test
  expectation moves — `tests_authority_record.rs` asserts against the hoisted
  constant, not a literal — but that test's panic prose carried the same
  falsified claim and is corrected with it. The refusal gate itself
  (`FOREIGN_EXACT_DURABLE_IS_WEAKER`) is unchanged and is the true ground: the
  condition was always carried as a refusal, never as a reduced-strength
  acceptance, so an empty residual leaves it exactly as sound.

## 5. Freeze conformance

- **§4.3 E9 annotation form.** The new arm is recorded in the same shape the
  E9 activation annotation established at Step 2.1: a named platform arm, the
  reason the portable operation is unavailable, the substituting property, the
  writer class that condition depends on, and the negative space. The E9
  annotation states it is "binding on durability callers"; this arm is its twin
  one edge later, and E9's own text already anticipated a consumer-facing
  writer-class condition. **The annotation itself is written out in §9** — it is
  authored to the contract form, not lifted from this section's prose.
- **No new primitive.** §4.3's preamble: "'New primitive?' stays **no** for all
  of them — an arm is a protocol-typed extension of an admitted primitive, not a
  new one." E14 and E10 keep P5 and keep `platform::private_barrier` as the
  family entry. Two freeze cells must nevertheless be edited to stay true — the
  §4.1 P5 row's Windows cell, which today names only the round trip, and the
  §4.3 E10/E14 win cells, which name only `platform.rs:458`. **Neither edit is
  in this package**; both are owed by the lane owner at landing, and §9 states
  the exact replacement text for each.
- **No census or count movement.** No publication call site added or removed
  (`CATALOG_PUBLICATION_CALL_COUNTS` unchanged, `namespace_mutation.rs` still 1);
  no fault key minted (the 165-key census and the 11-row namespace matrix are
  untouched); `RAW_RENAME_CALL_ALLOWLIST`'s platform.rs counts (5/5/5) unchanged,
  since the arm adds no raw-rename reference.
- **Owner follow-up, deliberately not done here: TWO pins move, not one**
  (corrected per Code review [P1-1]; the first draft of this bullet named only
  the flat pin, which would have landed a red gate). `scripts/checks/` was not
  touched.
  1. `PROTECTED_SOURCE_DIGESTS["checked_artifact/platform.rs"]`
     (`check_checked_artifact_boundaries.py:46`):
     `82d4906176229066247d2b8a71c4960da67b0f377ea70f581b57786f8aacecd2` →
     `6da65329100923462b2bcd358d79c405acaec9d3eff9b8c117c2cb79141531fc`.
     (The review's `0df1abd4be…` was this file before round 2's [P2-1]
     correction; `6da65329…` is the value that lands.)
  2. `PROTECTED_SOURCE_TREE_DIGESTS["checked_artifact/capability/pre_catalog.rs"]`
     (`:154`). `source_tree_digest` (`:769-785`) hashes the root file plus every
     descendant, and this package edits four of them
     (`provider/namespace_mutation.rs`, `provider/leaf_observation.rs`,
     `provider/authority_record_binding.rs`, `provider/tests_authority_record.rs`),
     so the tree pin moves:
     `6071965c19a011fbc17b8ff1be7b50ebd697c90e722b1e3cd895da91ef572d7f` →
     `7f2135b8f4c40bb6872dfba7f9013a22119cae31bcd7cf1907b35127f249f633`
     **for this package alone, over base `8b83a2c`** (computed with a replica of
     the checker's own function, validated by reproducing the base pin exactly).
     **This value is provisional and must be recomputed at landing**: the
     concurrent Phase 3.1 lane edits four more files inside the same protected
     tree, so the two lanes contend for this one pin and whichever lands second
     recomputes it from the merged tree.
- **Owner follow-up: the tracked acceptance item** (Code review [P2-3]). The
  native evidence here is a workflow-trimmed run: `--lib
  checked_artifact::namespace::` only, 1440 tests filtered out, so **none of the
  seven legacy `AnchoredPrivateArea` call sites executed on Windows** — and
  those are precisely the sites that still run the real anchor round trip.
  Their preservation is sound by construction (same variant, same path) and is
  covered by the macOS base-vs-fixed comparison, but freeze §4.2's precedent for
  a branch-scoped trimmed run is to record a tracked acceptance item that
  travels into `CurrentProgramCheckpoint.md`: the next **full** Windows matrix
  must come back with the four `tests_fault_matrix`, the four
  `tests_managed_matrix`, and the legacy `checked_artifact::tests::*` recovery
  suites green at the landing commit.

## 6. Gates

Round 1 ran on the isolated probe tree, since the live tree had already picked
up the concurrent Step 3.1 lane's in-flight edits; the review re-executed those
gates independently, including a base-vs-fixed comparison confirming the
zero-delta claim. Round 2 ([P2-1], [P3-1], [P3-2]) ran in the live tree, whose
lib count is 341 because it carries the 3.1 lane's +11 — this package's own
delta is still 0.

| Gate | Round 1 (probe tree) | Round 2 (live tree) |
| --- | --- | --- |
| `cargo check --all-targets` | Finished in 19.04s, zero diagnostics | Finished in 6.48s, zero diagnostics |
| `cargo test --lib checked_artifact::` | `330 passed; 0 failed; 1169 filtered out` | `341 passed; 0 failed; 1169 filtered out` |
| `--lib …namespace::tests_fault_matrix` | 4 passed; 0 failed | 4 passed; 0 failed |
| `--lib …namespace::tests_managed_matrix` | 4 passed; 0 failed | 4 passed; 0 failed |
| `--lib …namespace::tests_managed` | 12 passed; 0 failed | — |
| `--lib …provider::tests_authority_record` | — | 14 passed; 0 failed |
| `clippy --all-targets --all-features -- -D warnings` | zero warnings | zero warnings, `gwz-*` fingerprints deleted first so the verdict is not replayed |
| `rustfmt --edition 2024` (scoped) + `cargo fmt --check` | clean | clean |

No test expectation moved with the error-string change: the assertion at
`tests_authority_record.rs` compares against the hoisted
`FOREIGN_AUTHORITY_REFUSAL` constant, not a literal. Its sibling panic prose did
carry the falsified claim and was corrected with the string (+3/−2).

## 7. Native verification

Probe branch `probe/namespace-anchor-win` at `47279dc` (branched from `8b83a2c`,
carrying only this repair plus the probe-only workflow trim — the
`windows-matrix.yml` suite step narrowed to
`cargo test --locked --lib checked_artifact::namespace:: -- --nocapture`, the
idiom of `probe/track-p-spike`).

**Run `32569434565`, windows-2022, 3m19s — GREEN: 22 passed, 0 failed.**

- the four `tests_fault_matrix` tests of the run-16 class: both
  `namespace_interruption_restart_convergence_matrix` variants and both
  `repeated_same_namespace_boundary_crashes_keep_stable_slots` variants — **ok**;
- the four `tests_managed_matrix` tests the run-16 entry pre-attributed to this
  class — **ok**, so the pre-attribution is confirmed and discharged by the same
  one-line cause;
- the eight `tests_managed` tests and the five `tests_backend` refusal tests,
  which drive the same barrier through the managed edges — **ok**;
- `== failure names ==` empty; `test result: ok. 22 passed; 0 failed; 0 ignored;
  0 measured; 1440 filtered out`.

**What this run does not cover.** The trim filtered out 1440 tests, among them
every legacy `AnchoredPrivateArea` call site — the only sites still running the
real anchor round trip on Windows, and therefore the only ones this change could
have broken there. Their preservation is sound by construction and covered by
the macOS base-vs-fixed comparison, but it has no Windows execution in this
package; §5 records the tracked acceptance item that closes it.

The first push of the probe branch produced a 0s `startup_failure`
(`32569355916`) from an invalid workflow file: the trimmed `run:` scalar
contained `namespace:: --nocapture`, and YAML forbids `": "` inside a plain
scalar. Fixed by writing the step as a block scalar; recorded here so the next
probe author writes it that way first.

## 8. Files touched

Left uncommitted in the live tree for the lane owner.

| File | +/− | round |
| --- | --- | --- |
| `src/checked_artifact/platform.rs` | +73 / −3 | 1 + 2 |
| `src/checked_artifact/capability/pre_catalog/provider/namespace_mutation.rs` | +10 / −0 | 1 |
| `src/checked_artifact/cleanup.rs` | +18 / −3 | 1 |
| `src/checked_artifact/residue.rs` | +18 / −3 | 1 |
| `src/checked_artifact/transition.rs` | +6 / −1 | 1 |
| `src/checked_artifact/capability/pre_catalog/provider/leaf_observation.rs` | +17 / −5 | 1 + 2 |
| `src/checked_artifact/capability/pre_catalog/provider/authority_record_binding.rs` | +26 / −7 | 2 |
| `src/checked_artifact/capability/pre_catalog/provider/tests_authority_record.rs` | +3 / −2 | 2 |
| **total** | **+171 / −24** | |

Substance is `platform.rs` and `namespace_mutation.rs` (+83 / −3); `cleanup.rs`,
`residue.rs` and `transition.rs` are the call-site class argument only; the
`leaf_observation.rs`, `authority_record_binding.rs` and
`tests_authority_record.rs` deltas are the [P2-1] truth repair — comments, docs,
one shipped error-string constant, and the test prose that quoted it. Nothing
under `scripts/checks/`, nothing in the bootstrap/driver consumer files the
concurrent Step 3.1 lane owns.

## 9. Proposed §4.3 annotation — verbatim liftable

Authored to the contract form of the E9 and E16 activation annotations, not
lifted from §4/§5 (Code review [P2-2]). It is placed under the E10/E14 rows of
§4.3, exactly as E9's sits under its own row. Every claim it makes is either
carried by code in this package or explicitly named as owed and by whom; it
asserts no edit that does not exist. Line numbers are against this package's
tree over base `8b83a2c`; if the concurrent Phase 3.1 lane lands first, refresh
the `authority_record_binding.rs` numbers with it.

**Cell edits this annotation depends on, owed by the lane owner at landing**
(stated here rather than in the annotation, so the annotation asserts nothing
undone). The §4.3 E10 win cell reads `` `platform.rs:458` anchor `` and the E14
win cell reads `` `platform.rs:458` ``; **both are false for every production
path** after this arm, because both edges barrier the retained action directory
through `RetainedActionNamespaceV1::barrier` and therefore take `ExactInterior`.
Replace both with `` `platform.rs:514` documented no-op (see annotation) `` —
the exact shape E9's own row uses. The §4.1 P5 row's Windows cell must likewise
name **two** arms rather than one: the round trip at `platform.rs:551-584` for
`AnchoredPrivateArea`, and the documented no-op at `platform.rs:514-549` for
`ExactInterior` (one `cfg(windows)` function, `platform.rs:506`). That row's
block is also ~20 lines stale on every number it cites (pre-existing drift, not
this package's); landing the cell edit is the natural moment to refresh it.

> **E10/E14 activation annotation** (2026-08-22, the Windows anchor-readiness
> repair over the Step 2.2 and Step 2.3 landings; discharges the
> `GwzWindowsMatrix-Classification.md` run-16 class — all four
> `checked_artifact::namespace::tests_fault_matrix` tests plus the managed
> matrix that entry pre-attributes to it — and the Code review's [P2-1],
> [P2-2] and [P2-4]): **P5's Windows column is two arms, and which one an edge
> takes is a property of the caller, not of the directory.**
>
> The round trip renames a resident `.ca1-durability-anchor-<32hex>` file, so it
> presupposes a directory that is *permitted to retain one*. Only the
> checked-artifact private area is: the anchor is permanent by design and
> `finish()` never removes it. An action directory is not, and the refusal is
> structural rather than stylistic — admission requires `extra_children: 0`
> (`protocol/admission/owner.rs:29-38`), so a resident anchor would make the
> action permanently inadmissible, and the catalog root (`.gwz` / `gwz`) is
> outside the private path the preservation-image model excludes, so it would
> also re-enter exact captures. The barrier is therefore given its writer class
> explicitly (`platform::DirentBarrierClass`, `platform.rs:281`):
> `AnchoredPrivateArea` keeps the round trip unchanged, and `ExactInterior`
> takes a documented no-op (`platform.rs:514`). E10 and E14 both pass
> `ExactInterior` and can never take the other arm — `host.rs:667-681` pins the
> barrier target to the retained action directory and
> `namespace_mutation.rs:328` hardcodes the class — which is why both win cells
> now name the no-op. Off Windows the class selects nothing: both arms are the
> same directory `fsync` (`platform.rs:294`), so no non-Windows behaviour of any
> P5 caller moves.
>
> The substituting property is the P2 family's own, and it is
> writer-class-conditional in exactly E9's sense: every row of an exact interior
> is gwz-written through `durable_write_options` (`FILE_FLAG_WRITE_THROUGH`) and
> moved by the sealed exact-handle rename, and no portable directory flush
> exists on Windows to add to that — the same ground `sync_parent`'s Windows
> no-op and `sync_directory_edge`'s already stand on. Carriage: the condition is
> carried by `authority_record_binding.rs` — the file E9's annotation already
> designates as the carrier — at `:34-45` (the doc contract), `:321`
> (`FOREIGN_EXACT_DURABLE_IS_WEAKER`) and `:340`
> (`require_authority_strength`), and by this package's own arm comment at
> `platform.rs:514-549`. No new carrier is created and no other consumer is
> asked to carry anything.
>
> **This annotation supersedes one clause of E9's.** E9 records that for
> FOREIGN-written leaves `ExactDurable` is "strictly weaker on Windows
> (namespace ordering via the E10/P5 anchor round-trip only, no byte-flush
> claim)". That residual no longer exists: E10 has no anchor path, so on Windows
> a foreign-written leaf's `ExactDurable` carries **no byte-flush claim and no
> namespace-ordering claim — the residual is empty, not weaker**. Nothing rests
> on it: the class was always refused rather than accepted at reduced strength
> (`FOREIGN_AUTHORITY_REFUSAL`, whose text moves with this annotation to "a
> foreign-written leaf carries no durable proof on this platform"), and an exact
> interior admits no foreign row by construction. E9's own text is the lane
> owner's to edit; until it is, this annotation is the governing statement of
> that clause.
>
> **Negative space, stated directly rather than by reference** (E9's residual
> clause is the record this arm falsifies, so it cannot serve as the citation).
> Three things this arm does *not* prove, in the form of the E4-retire
> activation record:
>
> 1. **The `DurableNamespace` witness is still issued where no physical edge
>    occurred.** `host.rs:681` returns `Ok(self.issuer().durable())`
>    unconditionally, so on Windows the witness is minted after a no-op. It
>    proves what the schedule proves — that a barrier the admitted action
>    reserved was reached at its ordinal, with the retained parent revalidated —
>    and on Windows it proves *nothing physical beyond that*. It is not a
>    dirent-flush receipt there. Nothing dereferences it for durability today;
>    the one consumer class that could is refused before it reaches the witness
>    (`require_authority_strength`). Any future consumer that wants a physical
>    claim from it must state the platform condition itself rather than infer it
>    from the type's name.
> 2. **The `namespace.parent_barrier` matrix row is a process stop across a
>    no-op edge on Windows.** The key (`fault_v1.rs:119`) is live and its row
>    executes on every platform — its `hit` is at `namespace_mutation.rs:334`,
>    outside `private_barrier`, so the 165-key census and the 11-row matrix are
>    untouched (`tests_fault_matrix.rs:85`, `:464`). But the matrix's claim that
>    every row is "a real process stop across a real durable edge" does not hold
>    for this row there. The convergence it proves on Windows is the restart's
>    re-derivation, not the edge's durability. Recorded as an accepted deviation
>    rather than left implicit; it mints no key and changes no count.
> 3. **`MissingDurable` is unchanged and still two-sided.** E9's other negative
>    space survives this annotation intact: it does not assert continuous
>    absence across the barrier window, and an arm that performs no physical
>    edge does not widen that window's claim.
>
> **Owed at landing, by the lane owner, not by this annotation:** the two win
> cells and the §4.1 P5 cell above; the two protected-source pins (§5); the
> tracked acceptance item requiring the next full Windows matrix to bring the
> legacy `AnchoredPrivateArea` suites back green, since the probe run that
> validated this arm was workflow-trimmed and executed none of them.
