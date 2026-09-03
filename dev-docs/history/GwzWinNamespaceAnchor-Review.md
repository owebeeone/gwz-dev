# Windows namespace anchor-readiness package — CODE-axis review

Date: 2026-08-22. Axis: **Code-lead**, single axis (freeze §9 tier: R2-D Phase 2
steps are "single-axis with automatic escalation on any P0/P1/P2"). Peer-blind:
the author's memo was read as a *claim set to be falsified*, not as a guide; every
mechanism statement below was re-derived from the source before the memo's
corresponding sentence was compared against it.

Object: **the uncommitted anchor-fix delta in the live gwz-core tree over base
`8b83a2c`, exactly six files** —

| File | role |
| --- | --- |
| `src/checked_artifact/platform.rs` | `DirentBarrierClass`, both `private_barrier` arms |
| `src/checked_artifact/capability/pre_catalog/provider/namespace_mutation.rs` | E14 call site, `ExactInterior` |
| `src/checked_artifact/cleanup.rs` | 3 legacy P5 call sites |
| `src/checked_artifact/residue.rs` | 3 legacy P5 call sites |
| `src/checked_artifact/transition.rs` | 1 legacy P5 call site |
| `src/checked_artifact/capability/pre_catalog/provider/leaf_observation.rs` | E9 comment correction |

The concurrent Phase 3.1 lane's edits (`bootstrap/managed.rs`,
`bootstrap/managed/plan.rs`, `capability/pre_catalog.rs`,
`pre_catalog/provider.rs`, `pre_catalog/provider/managed_mutation.rs`,
`catalog/bootstrap.rs`, and the two untracked `bootstrap/managed/*` files) were
treated as out of scope and are judged nowhere in this report, **except** where
the object provably collides with them ([P1-1]).

Baseline: gwz-core `8b83a2c` ("Land D3: durable reverse-preservation cursor").
Contract: `GwzM5-8R2DInterfaceFreeze.md` §4.1 (P-table), §4.3 (E-table, the E9
activation annotation, the "an arm is not a new primitive" preamble), §9 (gate
tier); `GwzWindowsMatrix-Classification.md` run-16 entry. Author's memo:
`GwzWinNamespaceAnchor-Diagnosis.md`.

Line numbers cite the **fixed** tree unless marked `@8b83a2c`.

---

## 1. Attributability, and where the gates were run

**Attributability: CLEAN.** The six object files are byte-identical between the
live tree `/Users/owebeeone/limbo/gwz-dev/gwz-core` and the author's probe
snapshot (`probe/namespace-anchor-win` @ `47279dc`, branched from `8b83a2c`).
SHA-256, both trees, identical in all six:

```
0df1abd4be837c0aa6ac56c8dc68f4d8ad08968debc2a37fbe40be72c92cffdc  platform.rs
b86350d362f6434e7a77b5fc377f4f4d6f5349628c8aff2fc36b07938981894d  namespace_mutation.rs
b34a3466061693ee27f421784ced96eb98c43fe1a039e03e7435fd6dfc398a33  cleanup.rs
8ef558bb3f2c2f5c6dbd99ccb0b1a1868342c0bcf9082de5ff228bb756e5e3fa  residue.rs
991a323ae565aa889b1ba661b6d8d0bed409a5e69cc416b665c46a65c9d464b8  transition.rs
733343c632a4af574f0bbf978ab83d42e0c21e2ac404be7c68452205f2490a4e  leaf_observation.rs
```

All gates were therefore run in the probe worktree, never in the live tree, with
`CARGO_TARGET_DIR` pinned to the mandated scratch target. One disclosure the
memo makes and this review confirms: the probe **commit** carries a seventh file,
`.github/workflows/windows-matrix.yml` (the suite step trimmed to
`--lib checked_artifact::namespace::`). It is probe-branch-only, is absent from
the live tree, and is not part of the object — but it *is* the reason the native
evidence is narrower than it looks ([P2-3]).

## 2. Gate evidence, executed

All in `probe-wt`, foreground, macOS host.

| Gate | Result |
| --- | --- |
| `cargo check --all-targets` | Finished, zero diagnostics |
| `cargo test --lib checked_artifact::` | **330 passed; 0 failed; 0 ignored; 1169 filtered out** |
| same, at base `8b83a2c` (extracted via `git archive`, same target dir) | **330 passed; 0 failed; 1169 filtered out** — *the "no delta" claim is independently confirmed, not taken from the memo* |
| `--lib …namespace::tests_fault_matrix` | 4 passed; 0 failed |
| `--lib …namespace::tests_managed_matrix` | 4 passed; 0 failed |
| `--lib …namespace::tests_managed` | 12 passed; 0 failed |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings` | zero warnings — **forced**: the `gwz-*` fingerprints were deleted first so gwz-core genuinely re-linted rather than replaying a cached verdict |
| `cargo fmt --all -- --check` | clean, exit 0 |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | **FAILED, two findings** — see [P1-1] |
| same, at base `8b83a2c` | `ok (15 visible entries, 5 classified modules)` — so **both** checker findings are attributable to this object |

Native: `gh run view 32569434565 -R owebeeone/gwz-core` →
`conclusion: success`, `headSha 47279dcb…`, branch `probe/namespace-anchor-win`,
2026-08-22 11:07–11:10Z. The conclusion alone proves nothing here — the workflow
runs the suite under `|| true` and only the summarize step can fail the job — so
the log was pulled and read: `test result: ok. 22 passed; 0 failed; 0 ignored;
0 measured; 1440 filtered out`, `== failure names ==` empty, zero `failures:`
blocks. The 22 comprise all four `tests_fault_matrix` (the run-16 class), all
four `tests_managed_matrix` (the pre-attributed class), eight `tests_managed`,
five `tests_backend`, one `provider_compile`. The many `panicked at
tests_fault_matrix.rs:399/:472` lines are the matrix's own injected process
stops, not failures.

## 3. Claim-by-claim adjudication

### 3.1 Mechanism — **CONFIRMED, every link, from the code**

- `private_barrier`'s `cfg(windows)` arm demands `AnchorState::Ready`
  (`platform.rs:547-549`); the readiness predicate is a resident
  `.ca1-durability-anchor-<32hex>` file (`ANCHOR_PREFIX` `platform.rs:430`,
  classified in `anchor_state` `:611`).
- The only producer is `prepare_private(dir, create = true, …)`
  (`platform.rs:460-496`). Its **only** caller is `residue.rs:102`, inside
  `open_private` (`residue.rs:45`) — verified by exhaustive grep: the sole
  `prepare_private` references in the crate are that one and the barrier's own
  `create = false` call.
- E14 is `RetainedActionNamespaceV1::barrier` (`namespace_mutation.rs:325`,
  `@8b83a2c :316`), and it runs on `&self.handle` — the retained **action**
  directory, never anchored. `namespace/host.rs:667-681` pins the barrier target
  to exactly that directory and refuses any other.
- The refusal path is forced: `anchor_state` finds no `ANCHOR_PREFIX` entry and
  no `ca1-` entry → `Missing { family_state: false }`;
  `prepare_private`'s guard `Missing { family_state } if !family_state && !create
  => Ok(())` (`platform.rs:459`) no-ops; the re-read returns `Missing` again; the
  `let … else` fires with `"private durability anchor is not ready"`. Typed and
  unconditional, matching the run-16 message verbatim.

The memo's explicit **refutation of the run-16 entry's own hypothesis** ("the
readiness evidence … presumably unobtainable through the Windows no-op flush
arm") is correct: no flush participates in the readiness predicate at all. That
is a genuine correction to the classification ledger and is credited.

### 3.2 The fix, and the semantic core — **SOUND; clearance granted, with conditions**

Mechanically verified:

- `DirentBarrierClass{AnchoredPrivateArea, ExactInterior}` is declared
  unconditionally (`platform.rs:280-291`), so both arms and all callers see it.
- The `cfg(windows)` arm returns `Ok(())` for `ExactInterior` at
  `platform.rs:514-544`, **before** `prepare_private` at `:546`. Correct
  placement — the round trip is never entered.
- The `cfg(not(windows))` arm (`platform.rs:293-301`) binds the parameter as
  `_class` and performs the identical `sync_parent(dir)`. Non-Windows behaviour
  is byte-identical for every caller; the base-vs-fixed 330/0 measurement above
  is the empirical half of that.

**Adjudication of the question that decides this review.** Is `Ok(())` on Windows
for `ExactInterior` a sound writer-class-conditional no-op arm in the freeze's E9
sense, or does it silently weaken a durability claim a caller or an E-row relies
on?

*It is sound, and no caller relies on the weakened claim.* Four independent
grounds, each checked in code rather than accepted from the memo:

1. **The form is the sanctioned one, and it is not novel in this tree.** The
   freeze's §4.1 P2 Windows cell is already "a documented no-op"
   (`platform::sync_parent`, `platform.rs:420-424`); `sync_directory_edge`'s
   Windows arm is already a documented no-op
   (`directory_mutation.rs:733-742`); `finish_ready_edge_root_barrier`
   (`mutation.rs:274`) already records "Windows remains the deliberate no-op".
   §4.3's preamble is explicit that "an arm is a protocol-typed extension of an
   admitted primitive, not a new one". No primitive family moves: E14 and E10
   still enter P5 through `platform::private_barrier`.
2. **The class is a caller fact and cannot be defaulted.** It is a required
   positional enum argument with no `Default`; a directory cannot be asked which
   class it is, and the memo's reason is correct and load-bearing — "no anchor
   resident" is exactly the state a legitimate exact interior and a *damaged
   private area* both present, so inferring the class from the contents would
   convert the private area's real corruption signal into a silent skip. That
   refutes the obvious third alternative (make the Windows arm degrade when the
   anchor is missing), and it is the strongest single argument in the package.
3. **The exactness the arm protects is real.** `observe_action_interior`
   (`interior.rs:549-589`) counts **every** child other than the deterministic
   reservation as an extra child — no dot-file exemption — and `has_exact` /
   `has_rewritable_reservation` (`protocol/admission/owner.rs:29-38`, `:40-50`)
   both require `extra_children: 0`. A resident anchor would therefore fail
   admission permanently, since `finish()` never removes it.
4. **No production path converts an unbarriered Windows observation into a
   durability claim.** This is the decisive check. E10's consumer chain is
   `authority_record_binding::stream_payload` (`:238`) →
   `HostLeafObserverV1::observe_durable` → `cross_namespace_barrier` →
   `NamespaceProtocol::barrier` → `host.rs:667` → the `ExactInterior` arm. On
   `DurableLeafProof::ExactDurable` it immediately applies
   `require_authority_strength` (`:268`, `:321-329`), and
   `FOREIGN_EXACT_DURABLE_IS_WEAKER = cfg!(windows)` (`:305`) **refuses the
   `Foreign` writer class outright on Windows**. The only class for which the
   barrier was the sole residual property is already refused there, and refused
   more conservatively than before, not less. Nothing in the crate dereferences
   the `DurableNamespace` witness into a stronger fact (`backend.rs:164`,
   zero-sized sealed marker).

What the arm *does* change, and what therefore genuinely must be written down:
the freeze's §4.1 P2 Windows cell states that "Windows durability is the
write-through open **plus the anchor barrier in P5**", while the new arm's
substituting property is "the P2 family's own". For the exact-interior class the
composite Windows story now contains no dirent barrier from either family. That
is defensible — every row of an exact interior is gwz-written through
`FILE_FLAG_WRITE_THROUGH` and moved by the sealed exact-handle rename, and no
portable directory flush exists on Windows — but it is a **new** claim, not one
the freeze already made. That is precisely why the annotation is load-bearing,
and why its absence is graded rather than waived ([P2-2]).

### 3.3 The two refuted alternatives — **BOTH REFUTATIONS CONFIRMED**

- **Anchor the action directory.** Refuted from code: §3.2 ground 3 above, plus
  the anchor's permanence and the `MAX_ACTION_SLOTS` bound
  (`interior.rs:574`, `protocol/bounds.rs:16`). The memo's contamination limb is
  also verified: `catalog_names.rs:14-15` is `.gwz` / `gwz`, while
  `preservation_image.rs:8` excludes only `.gwz/checked-artifacts`. Its stop-clause
  limb is verified: the creation path mints a random scratch name at
  `platform.rs:467` (`@8b83a2c :440`), which §4.3's E22 row names as the R2
  stop-clause violation. **Not viable, and not cleaner.**
- **Re-route E14 to `sync_directory_edge` (P2).** Refuted, and more strongly than
  the memo argues: `sync_directory_edge` is `platform::sync_parent`
  (`directory_mutation.rs:725-731`), which §4.1's P2 row explicitly claims for P2
  "not … P5 (Code P3-6)" — so the family moves — **and its Windows behaviour is
  already `Ok(())`, identical to the chosen arm**. The alternative buys exactly
  zero physical difference at the cost of a family move. **Not viable, and
  strictly worse.**

No third alternative was found that is both viable and cleaner. (§3's refutation
does omit one variant a reader will reach for — see [P3-2].)

### 3.4 Call-site coverage — **CONFIRMED COMPLETE**

Exhaustive grep for `private_barrier` across the whole repository returns exactly
nine references: the two definitions (`platform.rs:294`, `:506`); the seven legacy
production call sites `cleanup.rs:89/:134/:169`, `residue.rs:407/:505/:539`,
`transition.rs:427` — **all seven pass `AnchoredPrivateArea`**; the internal
recursive call inside `prepare_private` (`platform.rs:457`), which also passes
`AnchoredPrivateArea` and is correctly not counted among the seven; and the one
new `ExactInterior` site (`namespace_mutation.rs:328`). No caller is missed, no
test calls it, and no `interface_tests` module pins its signature or its caller
set. The legacy sites' "not ready" refusal — a real signal for a private area
that lost its anchor — is preserved exactly.

### 3.5 The `leaf_observation.rs` comment — **LITERALLY ACCURATE, MATERIALLY INCOMPLETE**

The new sentence (`leaf_observation.rs:316-323`) attaches "whose Windows column
is the anchor round trip … and the writer-class-conditional arm … for an exact
interior" to *family P5*, and as a description of P5 it is accurate. But the
paragraph exists to answer "what ordering does the observer still owe the
caller?", and for **E10 specifically** the answer on Windows is now: *none*.
`host.rs:667-681` pins E10's barrier target to the retained action directory and
`namespace_mutation.rs:325-328` hardcodes `ExactInterior`, so E10 can never take
the anchor arm. The corrected comment leaves a reader believing it might. Folded
into [P2-1] rather than graded separately, because it is one instance of a
defect that recurs in three unedited sites.

### 3.6 Checker deltas — **CLAIM FALSIFIED: there are two, not one**

See [P1-1]. Everything else the memo claims here is confirmed by the checker's
own silence at the fixed tree: no publication-caller movement
(`CATALOG_PUBLICATION_CALL_COUNTS`), no fault-key census movement, no
`RAW_RENAME_CALL_ALLOWLIST` movement.

### 3.7 Freeze-doc obligation — **memo §4/§5 is NOT fit to lift verbatim**

See [P2-2] for the enumerated deficiencies.

---

## 4. Findings, most severe first

### [P1-1] A second, undisclosed protected-source delta: the `pre_catalog` **tree** pin also moves

`scripts/checks/check_checked_artifact_boundaries.py` fails at the fixed tree
with **two** findings, not one:

```
- protected source allowlist changed: checked_artifact/platform.rs
- protected source tree changed: checked_artifact/capability/pre_catalog.rs
```

The same checker at base `8b83a2c` is `ok`, so **both** are this object's.
`source_tree_digest` (`:769-785`) hashes the root file *plus every descendant*,
and the object edits two descendants — `pre_catalog/provider/namespace_mutation.rs`
and `pre_catalog/provider/leaf_observation.rs` — so the tree pin at
`PROTECTED_SOURCE_TREE_DIGESTS` (`:154`) moves:

```
checked_artifact/capability/pre_catalog.rs
  6071965c19a011fbc17b8ff1be7b50ebd697c90e722b1e3cd895da91ef572d7f
→ cdaabfdc1ebe27cbf879dd697abcaf46a84c894722b6a055c686414b7efe6611
```

(The disclosed flat pin is confirmed correct: `PROTECTED_SOURCE_DIGESTS` `:46`,
`82d4906176…` → `0df1abd4be…`, and `:46` is the right line.)

Memo §5's "Owner follow-up, deliberately not done here" bullet names **only** the
`platform.rs` flat digest, and the same section asserts "No census or count
movement". A lane owner who lands from that bullet lands with a red gate.

Graded P1 by this program's own calibration: freeze §4.2 records exactly this
class — a package that disturbed the pre_catalog tree pin without refreshing it —
as the **Code round-2 [P1-3]** finding, refreshed at `c40e712`. The deferral of
the recompute itself is not the finding; the *non-disclosure of which pins move*
is. Aggravating: the concurrent Phase 3.1 lane edits four more files inside the
same protected tree, so the two lanes contend for this one pin and the memo tells
the owner about neither the contention nor the pin.

**Remedy:** add the tree pin to memo §5's follow-up bullet with the value above,
and note that its final value must be recomputed *after* whichever of the two
lanes lands second.

### [P2-1] The E9 residual-ordering claim is now false in four production sites, including a shipped error string; the object corrected the wording of one and the logic of none

After this arm, E10's Windows barrier is unconditionally `Ok(())` (§3.5). The
following all still assert that a foreign-written leaf on Windows retains
*namespace ordering* as its residual guarantee:

| Site | Text |
| --- | --- |
| `leaf_observation.rs:316-323` | edited by the object, but its conclusion ("the observer adds no ordering of its own here") still rests on an E10 ordering that no longer exists on Windows |
| `authority_record_binding.rs:20-32` | the doc block the freeze **designates** as the E9 condition's carriage, quoting the annotation verbatim: "namespace ordering via the E10/P5 anchor round-trip only" |
| `authority_record_binding.rs:99-101` | `ObservedLeafWriterClassV1::Foreign` doc: "namespace ordering only, no byte-flush claim" |
| `authority_record_binding.rs:309-310` | `FOREIGN_AUTHORITY_REFUSAL`, a **shipped error message**: "a foreign-written leaf's durable proof is namespace ordering only on this platform" |
| `authority_record_binding.rs:312-320` | `require_authority_strength`'s doc, same claim |
| freeze §4.3, E9 activation annotation | the source text all four quote |

There is **no behavioural exposure** — `FOREIGN_EXACT_DURABLE_IS_WEAKER` refuses
the class on Windows (§3.2 ground 4), so the overstated residual is never relied
upon, and the refusal is conservative in the safe direction. That is the whole
reason this is P2 and not P1.

What makes it a finding rather than housekeeping is that the object's **central
justification is circular as written**: `platform.rs:540-542` argues "For a
FOREIGN-written row the claim would be strictly weaker — the same negative space
E9's annotation records", but this arm is precisely what falsifies the record it
points at. The author demonstrably knew the rule (they corrected the identical
sentence in `leaf_observation.rs` for exactly this reason) and applied it to one
of five sites.

**Remedy:** state the negative space in the arm's own terms rather than by
reference; correct the four `authority_record_binding.rs` sites and the refusal
string; and have the annotation ([P2-2]) supersede E9's residual clause rather
than cite it.

### [P2-2] Memo §4/§5 is not fit to lift verbatim as the §4.3 E10/E14 activation annotation

Judged against the E9 annotation that §5 itself names as the model. Deficiencies,
exactly:

- **(a) Form.** §4 is a changelog — "`platform.rs` gains …", "`private_barrier`
  takes it", "The seven legacy call sites pass …", "Comment only." — describing a
  *diff*. The E9 annotation is a *contract statement*: dated, bound to a landing
  step, naming the review finding it discharges, then the arm, the reason the
  portable operation is unavailable, the substituting property, the writer-class
  condition, the carriage assignment, the negative space. Lifted verbatim, §4
  would put implementation-diff prose into a frozen contract table.
- **(b) No date / landing-step / discharge header.** E9's opens "(2026-08-22, Step
  2.1 landing; discharges the Step-2.1 State escalation [P2-1] and Code round-1
  settle duty)". §4/§5 carry none; this arm's discharge target is the
  `GwzWindowsMatrix-Classification.md` run-16 class and it is never named as such.
- **(c) The affected table cells are never identified.** E9's precedent edited its
  own row's win cell to "documented no-op (see annotation)". E10's win cell
  currently reads "`platform.rs:458` anchor" and E14's reads "`platform.rs:458`";
  after this arm **both are false for every production path**, since both route
  through `RetainedActionNamespaceV1::barrier` and therefore through
  `ExactInterior`. Neither the memo nor the code says these cells must change.
- **(d) §5 asserts a freeze edit that does not exist and assigns it to nobody.**
  "the §4.1 P5 row's Windows cell gains a second arm beside the round trip rather
  than being replaced" is written in the indicative, but §5's own follow-up bullet
  lists only "the §4.3 E10/E14 rows". The §4.1 P5 edit is asserted, unassigned,
  and undone.
- **(e) No carriage assignment.** E9's annotation ends "Step 2.4's production
  caller binding must carry this condition", and that carriage is implemented and
  tested at `authority_record_binding.rs`. §4/§5 assign no carrier for the new
  class condition and never mention the file — which is also how [P2-1] escaped.
- **(f) The negative space is delegated to a record this arm falsifies** — see
  [P2-1].

**Remedy:** the annotation must be *written*, not lifted. §4/§5 supply the raw
material for its middle (the arm, the unavailability, the substituting property)
and are accurate there; the header, the cell edits, the carriage assignment and
the negative space must be authored fresh.

### [P2-3] No tracked acceptance item for the workflow-trimmed probe run

Run `32569434565` executed `--lib checked_artifact::namespace::` only:
**1440 tests filtered out**. It proves the run-16 class discharged and confirms
the managed-matrix pre-attribution, and the memo discloses the trim honestly. But
it exercises **none of the seven legacy `AnchoredPrivateArea` call sites** — and
those are the sites that still run the real anchor round trip on Windows, i.e. the
only ones whose Windows behaviour the object could have broken. They live under
`checked_artifact::tests::*`, entirely inside the filtered-out set. Their
preservation is sound by construction (same enum variant, same code path) and is
covered by the macOS 330/0 base-vs-fixed comparison, but it has **no Windows
execution** in this package.

Freeze §4.2 sets the exact precedent and the exact remedy: for a "branch-scoped
workflow-trimmed run", it records a **Tracked acceptance item** ("must come back
green on the next full Windows matrix run … the full matrix is still owed") that
travels with the package into `CurrentProgramCheckpoint.md`. This package owes the
same item and does not record it.

**Remedy:** record the tracked acceptance item — the full Windows matrix must come
back with the four `tests_fault_matrix`, the four `tests_managed_matrix`, **and**
the legacy `checked_artifact::tests::*` recovery suites green at the landing
commit.

### [P2-4] Unrecorded negative space: the `DurableNamespace` witness and the `namespace.parent_barrier` matrix row on Windows

Two consequences of the arm that the memo's negative space does not name:

- `host.rs:681` still returns `Ok(self.issuer().durable())` — the sealed
  `DurableNamespace` witness — on Windows when no physical edge occurred. Nothing
  dereferences it today (§3.2 ground 4), so this is a naming/contract hazard, not
  a bug; but a witness whose name asserts durability and whose issuance is now
  unconditional on one platform belongs in the annotation's negative space.
- `CheckedArtifactFaultKeyV1::NamespaceParentBarrier` (`fault_v1.rs:119`) is a
  live matrix key driven at `tests_fault_matrix.rs:85/:464`. Its `hit` is at
  `namespace_mutation.rs:334`, *outside* `private_barrier`, so the row still
  executes and the census is untouched — the memo is right about that. But on
  Windows that row is now a process stop across a **no-op** edge, so the matrix's
  own claim that every row is "a real process stop across a real durable edge"
  does not hold for it there. The freeze's E4-retire activation record is the
  precedent for exactly this situation, and its resolution was to *record the
  accepted deviation explicitly* rather than leave it implicit.

**Remedy:** name both in the annotation's negative space. Neither requires a code
change.

### [P3-1] Three stale line citations in the memo, inherited from the freeze's already-drifted P5 row

Verified against `platform.rs @8b83a2c`:

| Memo cite | Actual `@8b83a2c` |
| --- | --- |
| `anchor_state` `platform.rs:517` | **537** |
| `ANCHOR_PREFIX` `:383` | **403** |
| Windows `sync_parent` no-op `:372` | **393** |

All three are the freeze §4.1 P5 row's numbers, reused unchanged while the memo
*did* freshen others in the same paragraph (`:479`, `:430`, `:440` are all
correct, as are `namespace_mutation.rs:316`, `directory_mutation.rs:734`,
`catalog_names.rs:14-15`, `preservation_image.rs:8`, and
`check_checked_artifact_boundaries.py:46`). Root cause is upstream: the freeze's
own P5 Windows block is uniformly ~20 lines stale (`:458`, `:380`, `:383`, `:386`,
`:517`, `:575`, `:582`, `:410` are all short by that amount) — pre-existing drift,
not this object's. Graded P3 by direct precedent: freeze §4.2 grades an in-code
stale citation as **Code P3-4**. Files and continues; but the annotation
([P2-2]) will land *in* that drifted row, so refreshing the P5 cell's numbers is
the natural moment.

### [P3-2] §3's refutation of "prepare the anchor there" omits the transient-anchor variant

§3 refutes a *permanent* anchor. The variant a reader reaches for next — create
the anchor, round-trip it, remove it, all inside the barrier — is not named. Its
conclusion is unchanged and the package's answer survives: the creation path mints
a random scratch name (`platform.rs:467`), which is the R2 stop-clause violation
E22 exists to remove; the removal would itself need a barrier to be durable
(regress); and a crash inside the window would leave a resident anchor that breaks
that action's admission exactness **permanently** — a strictly worse failure mode
than the typed refusal being fixed. Worth one sentence in §3 so the refutation is
closed rather than merely correct.

---

## 5. Verdict

# GO

The diagnosis is right, and it corrects the run-16 ledger's own hypothesis. The
fix is the smallest correct shape available: the class is a caller fact because it
provably cannot be a directory fact; the non-Windows arm is inert and measurably
so; the seven legacy sites are preserved exactly; both alternatives are refuted
from code, and the P2 re-route is refuted more strongly than the memo argues
(identical Windows behaviour at the cost of a family move). The semantic core
clears: `Ok(())` for `ExactInterior` is a sound writer-class-conditional arm in
the E9 sense, and the one production path that could have relied on the weakened
claim — a foreign-written leaf's `ExactDurable` becoming authority — already
refuses that class on Windows outright. Every local gate is green, the 330/0
no-delta claim is independently reproduced against the base tree, and the Windows
class is discharged natively.

Every finding is a **landing-time obligation** — a pin, an annotation, a tracked
item, a comment set — not a defect in the arm. None of them changes the arm's
code. That is why this is a GO rather than a NO-GO, and why [P1-1] is a
disclosure failure rather than a correctness failure.

**Findings: 1 × P1, 4 × P2, 2 × P3.** All must be discharged before or at
landing; [P1-1] and [P2-3] cannot be deferred past it.

## 6. Escalation trigger — RECORDED

Freeze §9: R2-D Phase 2 steps are "single-axis with automatic escalation on **any
P0/P1/P2**". This review is single-axis (Code-lead) and returns one P1 and four
P2s. **The escalation trigger is therefore fired and is recorded here.** Routing
— which second axis, which round, whether the two-round remediation cap of §9 is
engaged — is the lane owner's decision, not this review's.

State-checklist note for the escalating axis: this axis judged the object's code
and its two contract documents. It did **not** judge the concurrent Phase 3.1
lane, `CurrentProgramCheckpoint.md` bookkeeping, or the run-17 matrix
expectations, and [P1-1] is the one place where the object and the concurrent lane
are known to collide.
