# R2-E Phase E4 Step E4.2 — single-axis peer-blind review (Opus)

**Step.** E4.2 — the first merge record: ConsumerCheckpoint §10 row `:273`
(`MergeStore` + `PreservationBundles` when missing) converted onto the checked
artifact boundary, with O13's substantive half on the store's CREATION path
(row `:280`) riding it per addendum §7.6.1, and the §11.3-item-2 duties
answered.

**Candidate.** ONE commit `7214010` ("E4.2: the first merge record, on the
checked artifact boundary") on branch `e4/e4-2-first-merge-record`.
**Base.** gwz-core main `e56124b` (E4.1's landing).
**Worktree.** `…/scratchpad/e4-e42-wt`, byte-clean on entry and on exit
(`git status --porcelain` empty at `7214010` both times). All probes were run
on copies under `--source`, in the checker suite's own idiom; nothing was
written into the worktree.
**Environment.** darwin 25.6.0 / arm64. `CARGO_TARGET_DIR` pointed at the
builder's warm cache (reused, not re-created). Boundary checker driven with
`/opt/homebrew/bin/python3.13`. Direct exit codes throughout.
**Date.** 2026-09-01. **Round 1 of 2.**

---

## VERDICT

# GO-WITH-CONDITIONS

The frozen ordering is implemented and driven. The conversion is real —
`create_open`'s raw `durable_fs` staged/rename/fsync publication is GONE, not
wrapped, and the refusal of an unbootstrapped parent is driven on the
production door. Both §11.3-item-2 duties are answered with tree evidence I
re-verified piece by piece. Every companion measured green on my own runs. The
cap is exact. No rider found. No trailers.

**One [P2] fires, so escalation fires.** It is not a defect in what E4.2
*does*; it is that the largest obligation E4.2 *surfaces* — ConsumerCheckpoint
§8 `:239-240` / §9 `:264-266`, the admitted-action + facade write gate — is now
demonstrably unowned, is not recorded anywhere a downstream builder will read
it, and is actively contradicted by the module doc of the very file E4.2
converts. That determination shapes E4.3–E4.6's charters, which is why it
cannot ride as a P3.

**Conditions (both are documents; neither is a code defect):**

* **C1** — [P2-1]: a dated record of the §8 `:239-240` / §9 `:264-266`
  authority-gate obligation, minted in §7.6.1's own O13 form (a plan obligation
  with an owner) or as an explicit scope clause on E4.3–E4.6; **plus** the
  `coordinator/execution.rs:10-38` module-doc cure, whose three assertions are
  now false. The document half is the lane owner's; the door half may ride
  E4.3.
* **C2** — [P3-5]/flag 1: the dated plan correction at
  `GwzM5-8R2E-Plan.md:398`. CONFIRMED against the tree by me independently;
  the builder recorded it at the pin, the plan still carries the false
  sentence.

---

## FINDINGS

### [P2-1] The §8 `:239-240` / §9 `:264-266` write-authority gate is unowned, unrecorded, and contradicted at its own door

**What E4.2 does.** `entry::bootstrap_merge_start_parents`
(`src/checked_artifact/entry.rs:341-360`) is the **first production code in the
tree** to mint an `AdmittedCheckedActionV1` and to obtain
`ManagedParentFacadeV1`s. `coordinator/execution.rs:145-158`
(`execute_merge_start_managed_parents`) calls `facade.revalidate(catalog)` on
each — §9 `:266`'s facade operation — and then keeps only the purpose:

```rust
    admitted
        .bootstrap_managed_parents(catalog, &action)?
        .iter()
        .map(|facade| facade.revalidate(catalog).map(|proved| proved.purpose()))
        .collect()
```

The proof is then dropped. `entry::create_merge_store_record`
(`entry.rs:368-390`) writes the record leaf through
`CheckedArtifact::acquire(CheckedArtifactPolicy::workspace(root), relative, …)`
— i.e. **from a path** — and gates on `artifact.parent_is_canonical()`, which
is a re-traverse-and-compare-identity check
(`src/checked_artifact/observation.rs:159-166`).

**What the frozen text says.** ConsumerCheckpoint `:239-240`: *"Replacement/
removal executes only after an admitted action and an owner-private coherent
authority observation."* `:264-266`: *"Writers receive an opaque retained-parent
proof. They may write only through a facade operation that revalidates that
proof. **A path string or successful `exists()` check is not parent
authority.**"*

**Measured.** `authorize_write` / `RetainedWriteAuthorityV1` have **ZERO**
production consumers:

```
$ grep -rn "RetainedWriteAuthorityV1\|authorize_write" --include='*.rs' src/
coordinator/tests_execution.rs:284,296,359,410,430,448     <- tests only
coordinator/execution.rs:48,55,292,295,322,373,381         <- definition + doc
capability/.../tests_terminal_fault_matrix.rs:16           <- comment
interface_tests/fault_expected_keys.rs:447                 <- comment
```

**The contradiction at the door.** `coordinator/execution.rs:373-371`'s own
type doc says: *"R2-E's consumers take it as the argument that says 'this
replacement may proceed'; nothing in R2-D consumes it yet."* E4.2 is the first
R2-E consumer performing a replacement on this store, and it does not take it.
Worse, `coordinator/execution.rs:10-38` — the module doc of the file E4.2
converts, left untouched by this commit — still asserts three things that are
now all false:

* *"nothing here is reachable from an entry point"* — it is, via
  `entry.rs` → `v1_lifecycle/checked.rs:175` → `start.rs:84` → merge dispatch;
* *"no consumer calls it"* — `entry.rs:13-15` imports two of its functions;
* *"Production catalog activation also remains forbidden outright"* — E4.1
  landed it, and *"the gate stays shut, deliberately"* is no longer the state
  of the world.

**Grading.** E4.2 is **not** in breach of row `:280` (see the flag-6 judgment
below) and this is **not** E4.2's miss. But "unowned-and-recorded" requires the
*recorded* half, and it does not exist: the only record is a commit-message
argument that E4.2 complies, not a statement that a frozen clause has no owner.
§7.6.1 set the precedent for exactly this situation — an unowned frozen clause
became **O13, a minted plan obligation with a dated correction**. The same is
owed here, or O1 will be reportable DISCHARGED with §8 `:239-240` never
realized in production.

**Remedy (C1).** (i) Mint the obligation with an owner, dated, in §1's ledger;
(ii) cure `coordinator/execution.rs:10-38`; (iii) state at
`execute_merge_start_managed_parents` which of its two gates E4.2 opened
(admission + facade re-proof, for **durability**) and which it left closed
(`authorize_write`, for **authority**), so E4.3's builder cannot read the
re-proof as satisfying §9 `:266`.

### [P3-1] Four stale `#[allow]` reasons the caller-adding commit owes, by the lane's own standing order

Plan §E4's attention set, item (2), is a standing order: a stale `#[allow]`
reason is *"rewrite[n] in the caller-adding commit"*. E4.2 cured one
(`coordinator/mod.rs`, flag 3 — correctly) and left four whose stated
precondition E4.2 itself satisfies:

| site | reason text, now false |
| --- | --- |
| `src/checked_artifact/bootstrap/managed.rs:15-18` | "reachability from an entry point is R2-E" |
| `src/checked_artifact/bootstrap/managed.rs:41-45` | "reachability from an entry point is R2-E" |
| `src/checked_artifact/namespace/mod.rs:47-51` | "entry-point reachability is R2-E" |
| `src/checked_artifact/capability/pre_catalog/provider.rs:25-28` | "the production consumer is R2-E Phase E4" |

All four are now reached: `bootstrap::managed`'s owner/provider via
`admit_merge_start_managed_parents`; `namespace::host::retain_action_namespace`
via `provider.rs`'s `execute_row`; the barrier/managed-mutation edges via
`advance_one`'s `parent.stage_component(…)`.
`bootstrap/managed.rs:10-14`'s companion sentence — *"`entry.rs` calls
`CheckedArtifact::prepare_parent` until R2-E converts it"* — remains literally
true (`entry.rs:151` still does, for the bundle path) and needs no change.

*Not a finding, routed:* `observation.rs:155-158`'s
`reason = "v1 bundle lifecycle remains disabled until A1"` on
`parent_is_canonical` was **already** inert at base — `entry.rs:264` called it
pre-E4.2 — so E4.2 neither created nor inherited a new obligation there. It
belongs with [P3-5]'s A1-era allowances at **E4.7**.

### [P3-2] The proof-only early return does not barrier for itself

`bootstrap_merge_start_parents` returns `Ok(())` immediately on a proof-only
plan:

```rust
    let Some(admitted) = admitted else {
        return Ok(());
    };
```

On that path residency comes from `observe_preflight` →
`OpaqueRetainedCatalogV1::observe_managed_prefix`
(`catalog/bootstrap.rs:199-204` → `capability/pre_catalog.rs:222-228`), which
is a bounded, identity-proved **walk** — an observation, not a flush. So on the
already-resident path row `:273`'s *"both parents durable"* is **inherited**,
not established by this step; only the bootstrap path barriers (the provider's
per-generation install-and-reobserve, `provider.rs:404-458`).

This is **not a regression** — the pre-E4.2 `create_open` fsynced only
`.gwz/merge` itself, never `.gwz` — and it is arguably inside the row's
*"when missing"* qualifier. But the E7.2 annotation (plan `:322-339`) was
written against precisely this shape (*"converged does not imply flushed"*;
*"E4 consumers that need ordering must barrier for themselves"*), and the
door's own doc says a proof-only plan *"IS the row's 'when missing'
qualifier"*, which reads as though the clause is discharged rather than
unexamined. State the scope at the door.

### [P3-3] The [P3-3] doc-scoping's stated ground is broader than the tree supports

`identity.rs:210-221` grounds the Linux/macOS-only downgrade on: *"an errno
allowlist fits only where a probe reports 'this filesystem does not do that'
through a specific code… **Windows' one capability-shaped gap carries no errno
at all**."*

The arithmetic **inside identity.rs's own `mod platform` is exactly right** —
I verified all three arms of `identity.rs:551-586`:

| arm | line | disposition |
| --- | --- | --- |
| no `\` separator after the volume-GUID prefix (structural) | `:583` | → `persistent_identity_unsupported()` ✔ the "one capability-shaped gap" |
| `GetFileInformationByHandleEx` fails | `:563` | bare `last_os_error()` ✔ "hard failure" |
| `GetFinalPathNameByHandleW` fails | `:575` | bare `last_os_error()` ✔ "hard failure" |

But the **general** claim is contradicted by the sibling catalog provider:
`capability/pre_catalog/provider/platform/windows.rs:182-188`

```rust
fn query_error(capability: PlatformCapability, operation: &'static str) -> CheckedFsError {
    let source = io::Error::last_os_error();
    match source.raw_os_error() {
        Some(1 | 50 | 87) => CheckedFsError::unsupported(capability, source.to_string()),
        _ => CheckedFsError::io(operation, source),
    }
}
```

— a three-code Win32 downgrade allowlist (ERROR_INVALID_FUNCTION /
ERROR_NOT_SUPPORTED / ERROR_INVALID_PARAMETER) applied to **the same two APIs**
the doc calls unconditionally loud (`windows.rs:85-88`, `:150-154`). So the
lane has already decided those two Windows calls *can* report capability-shaped
failures by code.

The **conclusion survives** ("no Windows code moves here"), and the exposure is
narrow — the catalog half already downgrades, so precondition 1's actionable
sentence still fires from the gate that refuses at activation; identity.rs's
arm merely yields an `Io`-class message instead. But the disposition should
name the asymmetry rather than assert its absence.

**No landing-dispatch proof is owed.** `identity.rs`'s change is 13 added
lines, 0 deleted, every one a `///` line. Verified from the diff.

### [P3-4] Commit-message arithmetic: "the other six recomputed unchanged"

`PROTECTED_SOURCE_TREE_DIGESTS` holds **seven** entries; E4.2 moved **two**
(`capability/pre_catalog.rs`, `v1_lifecycle/mod.rs`), so **five** remain. The
in-tree checker comment says *"The other five were recomputed and are
unchanged"* and is correct; the commit message says *"the other six"*, which is
E4.1's wording (7 − 1 = 6) carried forward. Record nit only — the checker being
green proves every one of the 17 file digests and 7 tree digests matches the
tree exactly.

### [P3-5] The plan's O13 retirement sentence is false and still stands

See flag 1 below. `GwzM5-8R2E-Plan.md:398` — *"the O13 checker inventory
retires to empty across E4.2/E4.3"* — is contradicted by the tree. Correction
owed at the plan (C2); the builder correctly recorded it at the pin.

---

## THE FROZEN ORDERING, VERIFIED

Row `:273`, verbatim: **"both parents durable before record; record durable
before Git"**.

### Clause one — both parents durable before record

**Implemented.** `V1MutationLease::acquire_for_merge_start`
(`v1_lifecycle/checked.rs:175-183`) takes the workspace mutator lock, then runs
`bootstrap_merge_start_parents` through the coordinator in two lease sessions,
then returns the lease. `start.rs:83-86` calls `store.create_open(&lease, …)`
only after that returns. The two are sequential in one function; the mutator
lock is a workspace-wide OS advisory exclusive lock, so no second process can
interleave.

**Driven, not asserted.**
`v1_lifecycle/tests/checked.rs::the_creation_lease_bootstraps_both_managed_parents_before_any_record_exists`
is an ordering witness: after `acquire_for_merge_start_for_test` and **before**
`create_open`, it asserts `.gwz/merge` and `.gwz/stash/bundles` are directories
and `.gwz/merge/merge_1.yaml` does **not** exist. It additionally asserts
`!git.join(".gwz").exists()` — the §11.3-item-2(b) answer, driven.

**Is "durable" real?** On the bootstrap path, yes, and through the checked
actions:
`admitted.bootstrap_managed_parents` → `owner.execute` → `execute_bound` →
`execute_row` (`bootstrap/managed/provider.rs:404-458`), which settles each
generation's intent, retires the prior one, and closes with *"the final reproof
… the whole declared path re-observed through the same bounded walk"*, refusing
if `depth != components.len()`. Then `execute_merge_start_managed_parents`
re-proves each installed prefix through `ManagedParentFacadeV1::revalidate`,
which re-observes through the opaque catalog and rejects on any identity/mode/
path drift. **It does not lean on either forbidden implication** — the doc says
so explicitly and the code matches: durability comes from the provider's own
install-and-reobserve, never from a settled ordinal or a converged restart.
*Caveat: the proof-only path — [P3-2].*

**Adversarial: can any interleaving produce record-before-parent-durable?** No.
`create_open` has exactly **one** production caller (`start.rs:85`; grep over
`src/` returns that plus test callers only), and that caller's lease is
`acquire_for_merge_start`. Even if the ordering were somehow inverted,
`create_merge_store_record` refuses a non-canonical parent (`entry.rs:383-389`)
and, behind it, `classify_exact` returns `Ambiguous` for a `ParentState` that
is not `Open`, so `replace_exact` errors rather than publishing.

### Clause two — record durable before Git

**Implemented.** `create_merge_store_record` → `CheckedArtifact::replace_exact`
(`transition.rs:38-113`) → `publish_goal` (`transition.rs:329-425`), which
after `publish_verified_leaf_no_replace` performs

```rust
        self.sync_dir(dir, BeforeManagedDestinationDurability, AfterManagedDestinationDurability)?;
        self.sync_private(&private, …)?;
        let managed = observe_leaf_exact(dir, &self.leaf, …)?;
```

— an fsync of the managed parent (`platform::sync_parent`, `transition.rs:428-444`)
after the rename, then re-observation, then `finish_replace`, then a final
`classify_replace == After`. **Real barrier semantics, self-performed.**

Git only runs afterwards: `handle_start_durable_v1` scopes the creation lease,
drops it, and only then builds `ForwardRuntime` and calls `service::run`. The
outer `merge/start.rs:105-107` drops the start guard and hands off before any
mutating Git work; the plan/record construction above it is read-only.

**Driven under interruption.**
`a_faulted_record_publication_leaves_the_parents_installed_and_no_record`
injects `CheckedArtifactFault::BeforeManagedPublication` and asserts: refusal
carries "injected failure", **both parents still installed**, and the record
leaf absent — so nothing can be ordered behind a record that never came into
being. The forward half is proved in the first row by an **independent re-read**
(`std::fs::read` of the leaf, re-decoded, digest compared), not by the returned
handle.

### Answer, in one line

**Yes — the converted path implements the frozen ordering, the builder's rows
drive it by fault injection and ordering witnesses rather than doc claims, and
"durable" is real fsync/barrier durability performed by the conversion itself
on both clauses — with one stated gap: the proof-only branch inherits rather
than establishes clause one ([P3-2]).**

---

## THE REFUSAL SEMANTICS

**Who bootstraps.** `V1MutationLease::acquire_for_merge_start`, and only it.

**Is the bootstrap always ordered before `create_open` on every production
route into a v1 start?** Yes, and the route set is a singleton:

```
$ grep -rn "create_open" --include='*.rs' src/     (production hits only)
v1_lifecycle/start.rs:85          store.create_open(&lease, root, &record)   <- THE caller
v1_lifecycle/store/mod.rs:36-42   the delegating wrapper
v1_lifecycle/store/rewrite.rs:43  the definition
```

`handle_start_durable_v1` is reached from exactly one place
(`runtime/dispatch.rs:83`), and its lease is `acquire_for_merge_start`.

**The A1 adaptation route — does an ADAPTED ordinary merge reach
`create_open`?** **No, and it structurally cannot.** I drove the trace:
`dispatch_merge` → `adapt_before_mutating` (`dispatch.rs:400-427`) →
`upgrade_open_v0` → `store/atomic_upgrade.rs:104-140`, which stages and renames
**onto the existing v0 record's own path**. It is a rewrite-in-place of a
record that already exists, so (a) it never calls `create_open` (which refuses
an existing record at its `path_exists` pre-flight anyway), and (b) its parent
`.gwz/merge` exists by construction — the v0 record is inside it. The adapted
record then enters the v1 **service**, which takes `acquire_activated`
(`service.rs:117-119`) and creates no record. **The E4.1-round-1 wedge class is
not reborn here**: there is no adapted resume that can hit the new refusal.

*Residual noted, not a finding:* the adapted route never bootstraps the
`PreservationBundles` parent. That is correct today — the preservation writers
are unconverted and still create their own parent; row `:275`/E4.5 owns them.
E4.5 must not assume E4.2 bootstrapped that parent for an adapted record.

**Abort / plain-lease paths untouched — [P3-C1] stays closed.** Verified two
ways. *Structurally:* `service.rs:116-121` still splits
`ResumeStart | Continue` → `acquire_activated`, `_` → plain `acquire`; E4.2
changed only the start path, and `acquire_for_merge_start` is reachable from
nowhere else. *Driven:*
`the_converted_creation_path_refuses_a_record_whose_parent_was_never_bootstrapped`
takes the **plain** lease, asserts `.gwz/merge` does not exist before, and
asserts it **still does not exist after** the refusal — i.e. the plain lease
bootstraps nothing and the refusal mutates nothing.

**[P3-8] — did any "rejects without mutation" row need to grow?** No, and the
builder's ground is correct. Those rows are
`assert_entry_rejected_without_mutation`
(`tests/reverse_rollback/entry_service.rs:59`), which drives
`V1LifecycleRequest::Abort` under a `NoMutationSnapshot`. Abort takes the plain
lease (`service.rs:120`), which bootstraps nothing and activates nothing. E4.2
adds no catalog mutation to any path those rows cover. No exclusion list
needed to grow.

---

## THE EIGHT FLAGS — DISPOSITION

| # | Flag (builder's) | My verification | Disposition |
| --- | --- | --- | --- |
| **1** | The plan's *"O13 retires to empty across E4.2/E4.3"* is FALSE against the tree | **CONFIRMED, independently.** File-by-file ownership: `store/rewrite.rs` — `rename_durable:132` / `sync_dir:139` sit inside **`commit`** (fn boundaries: `commit` at `:77`, next fn at `:153`) → **E4.3**. `v1_lifecycle/archive.rs:207` `sync_dir(done)` is inside **`remove_archive`** (`:191-208`), the archive deletion/GC surface → **E4.4** (row `:275` + O8). `v1_lifecycle/store/archive.rs:55-79` is `fs::create_dir_all(&done)` + `rename_noreplace` + six `sync_dir`s publishing into `.gwz/merge/done` — the `MergeArchive` purpose's raw when-missing bootstrap → **E4.4**. Only ONE of three files retires across E4.2/E4.3. | **ACCEPTED.** Plan owes a dated correction at `:398`. → **C2 / [P3-5]** |
| **2** | Pin WIDENED from file-set to per-file counts, red-probed both directions | **RE-RUN, all three directions fire.** Counts verified against the tree by hand first: `archive.rs {sync_dir:2}` (`:13` use, `:207` call); `store/archive.rs {rename_noreplace:2, sync_dir:7}` (`:3` use ×2, `:63`, and `:55,56,65,66,78,79`); `store/rewrite.rs {rename_durable:2, sync_dir:2}` (`:6` use ×2, `:132`, `:139`) — **exact match**. Probe transcripts below. | **ACCEPTED.** Fail-closed in three directions, not two. |
| **3** | `coordinator/mod.rs` module-privacy contradiction | **CONFIRMED.** Base `e56124b:coordinator/mod.rs:13` is bare `mod execution;` while `:8-9` of its own doc says *"R2-E's consumer names `coordinator::execution::…` directly"* — unimplementable as written. Widening is `pub(in crate::checked_artifact)` — **minimal** (`entry.rs` is inside that subsystem; not `pub(crate)`, not the rejected re-export), **dated** 2026-09-01 at the site, and the `#[allow]` reason was rewritten in the same edit. | **ACCEPTED.** Model of the staleness cure the lane wants — which makes [P3-1] the more conspicuous. |
| **4** | [P3-2] measured ~55, not the plan's ~10-15 | **CONFIRMED and pre-blessed.** Renderer is `pub(super) fn render_catalog_refusal` — **not** `pub(crate)`, with the `clippy::private_interfaces` ground stated (`CheckedFsError` is subsystem-private). The row `the_catalog_refusal_renderer_carries_the_remedy_into_every_arm` drives **all four** rendered shapes through the named fn, asserts `CATALOG_LABEL` present in each, asserts `--abort` present **iff** index 0 (so the no-remedy arm cannot borrow the actionable sentence). Measured by me: entry.rs renderer block **+33** whole-added, contracts.rs **+45** whole-added, **−23** inline arms deleted → **55 net / 78 whole-added**. The E4.1 review supports the figure at `:822` (*"The measured 44-line cost of the [P3-2] extraction justifies the carry; my round-1 ~10-line estimate was for the row alone, not the extraction, and I accept the correction"*) and `:906` (*"−44 for [P3-2]'s extraction"*). | **ACCEPTED.** 55 net vs the pre-blessed 44; the delta is the fourth shape and the `--abort` assertion. In charter. |
| **5** | [P3-3] resolved by DOC-SCOPING, not a Windows cure | **Structural claim verified against identity.rs's own `mod platform` and it is exactly right** (three arms, table in [P3-3] above). **No landing-dispatch proof owed** — the change is 13 lines, all `///`. **But the stated ground over-reaches**: the sibling `provider/platform/windows.rs:182-188` downgrades Win32 1/50/87 from the *same two APIs* the doc calls unconditionally hard. | **ACCEPTED WITH FINDING → [P3-3].** Conclusion survives; ground must name the asymmetry. |
| **6** | THE BIG ONE — `RetainedWriteAuthorityV1` has NO consumer; the record leaf goes through the `CheckedArtifact` boundary | **JUDGED. See below.** | **Row `:280` NOT breached; §8/§9 obligation real, unowned, under-recorded → [P2-1]** |
| **7** | The ~8-line `parent_is_canonical` pre-check — rider or load-bearing? | **LOAD-BEARING, in charter.** Without it, `classify_replace_exact(Missing, goal)` → `classify_exact` → `ParentState` not `Open` → `ExactTransition::Ambiguous` → *"replacement evidence is ambiguous"*, which names neither the parent nor the ordering. With it, row `:273`'s clause becomes the refusal's own sentence, and the driving row asserts on it (`refused.message.contains("parent")`). It **weakens nothing**: it is a strictly-earlier refusal over the same `parent_is_current` identity comparison `replace_exact` performs anyway. | **ACCEPTED as in-charter**, not a rider. |
| **8** | Environment only | Nothing in the tree to verify. My own environment notes, for the record: disk at **99% / 5.2 GiB free** — I killed the checker suite's compiler-probe subset rather than let it create a second target tree (it hard-codes `CARGO_TARGET_DIR = ROOT.parent/target/checked-boundary-probe` and needs `cargo +1.95.0`); `pytest` is absent, the suites are `unittest`. | **NOTED.** See the companion transcript for exactly what I ran and what I did not. |

### Flag 6 — the judgment, in full

**Is the `CheckedArtifact`-boundary route row `:280`'s own meaning?**
**Yes, for the row.** Three grounds:

1. **The row's own words.** `:280`'s declared purpose/operation is *"same
   purposes and artifact actions"* — *same as* its siblings. Its siblings
   `:278`/`:279` are spelled *"checked workspace artifact action"* / *"checked
   Git-directory artifact action"*, and their landed implementations are
   `entry.rs`'s `replace_merge_root_artifact`,
   `replace_merge_preservation_workspace` and kin — every one of them
   `CheckedArtifact::acquire` + `replace_exact`, the exact pair
   `create_merge_store_record` uses. E4.2 is *the same* action on *the same*
   boundary.
2. **"No legacy raw writer" means `durable_fs`.** §7.6.1's own analysis
   (`:1046-1054`) identifies the clause's referent precisely:
   *"`v1_lifecycle/store/rewrite.rs:6` is `use crate::durable_fs::{rename_durable, sync_dir};` — a **raw** durable writer"*.
   E4.2 removes that writer from `create_open` (3 references → 2, both now
   `commit`'s), which is exactly the half §7.6.1 assigned it
   (*"E4.2 owns the store's creation path and E4.3 its rewrite path"*).
   The clause is discharged for E4.2's half.
3. **§10's own retirement sentence points the same way.** *"`CheckedArtifact::prepare_parent`
   … [has] no successful converted caller and [is] removed after parity tests
   pass"* — i.e. what §10 retires is the boundary's *recursive parent creation*,
   not the boundary. E4.2 does precisely that: the parent now comes from the
   managed provider and `create_open` refuses rather than creates.

**Does "artifact actions" additionally demand the admission/`authorize_write`
path?** **That demand exists, but it is `:239-240`/`:264-266`'s, not `:280`'s
— and it is boundary-wide, not E4.2-specific.** Every already-converted §10
consumer writes through the same path-derived `CheckedArtifact`; if
`authorize_write` were `:280`'s meaning, every landed conversion would be in
breach too. So grading it as E4.2's miss would be wrong.

**Is "unowned-and-recorded" the right grade?** **Unowned: yes. Recorded: no —
and that is the finding.** No plan step names it; §7.6.1's ownership analysis
does not reach it; `RetainedWriteAuthorityV1`'s own doc *promises* R2-E
consumers will take it; and `coordinator/execution.rs`'s module doc still says
the gate is shut. E4.2 is also the step that makes the gap *demonstrable* for
the first time — it mints the proof and discards it — so a future auditor
reading `execute_merge_start_managed_parents`' "re-prove each through its own
facade before the caller may write" could reasonably conclude §9 `:266` is
satisfied. It is not: the caller writes from a path, which `:266` names
explicitly as *not* parent authority.

**Consequence for E4.3/E4.4's charters.** They should be chartered knowing
that the boundary they are converting onto is the `CheckedArtifact` boundary,
that this is correct for their rows, and that a **separate, later** step owns
routing leaf writes through `authorize_write` + a facade that can actually
carry a write (today `RevalidatedManagedParentV1` exposes only purpose +
identity — plumbing it is new API surface, i.e. a design step, not something
E4.3 can absorb inside 500 lines). Without that record, each of E4.3–E4.6 will
reproduce E4.2's shape and O1 will close over a clause nobody implemented.
→ **C1.**

---

## §11.3-ITEM-2 VERIFICATION

### (a) The door SURVIVES with a dated reason — VERIFIED

* **Disposition is AT the door**, dated:
  `capability/pre_catalog/provider/managed_mutation.rs:382-388`, immediately
  above `#[cfg(test)] pub(in crate::checked_artifact) fn retain_managed_parent_at_for_test`
  (`:390`). Carries the date (2026-09-01), the item (§11.3 item 2(a)) and the
  verdict (SURVIVES). §7.7's requirement — *"the door's disposition recorded —
  either the door is retired … or it survives with a dated reason"* — met.
* **The one-component vs depth-d evidence — VERIFIED.** The door's signature
  takes `enclosing: &std::path::Path` plus one component and *"is the only
  place in this owner that opens an ambient path"* (its own doc, `:379-381`).
  The production caller E4.2 creates reaches a retained managed parent by a
  different shape entirely: `advance_one`
  (`bootstrap/managed/provider.rs:346-350`) calls
  `self.catalog.retain_managed_prefix(row.components(), row.retained_existing_parent_count() + cursor, plan.reservation_digest())`
  — components + depth, through the retained catalog, no ambient path. The two
  shapes do not overlap.
* **The three test callers — VERIFIED.**
  `namespace/tests_managed.rs:100`, `:250`, `:297` (plus the `use` at `:28`).
  Retiring the door would delete Step 2.3's only route to the one-component
  enclosure shape — coverage, not duplication. **SURVIVES is correct.**
* **The no-production-caller doc — VERIFIED.** The `#[cfg(test)]` gate is the
  hard guarantee; grep confirms the only non-test references are the two
  re-export lines (`pre_catalog.rs:27`, `provider.rs:91`) and three comments.

### (b) The Git-directory answer — all four pieces of tree evidence VERIFIED

Recorded in `entry.rs:322-340`, against freeze `:672-680`, dated 2026-09-01.

| # | Claim | Tree evidence | ✔ |
| --- | --- | --- | --- |
| (i) | purposes' declared components are workspace-relative | `bootstrap/managed.rs:80-87` — `MergeStore => [".gwz","merge"]`, `PreservationBundles => [".gwz","stash","bundles"]`. Under a Git directory there is no `.gwz` ancestor, so both fail `minimum_retained_parent_count` (`:89-95`). Pinned as production behaviour by `bootstrap/managed/tests_provider.rs:964-971` `a_git_directory_target_refuses_the_workspace_rooted_managed_paths` (asserts `preflight(&fixture, GitDirectory).is_err()`). | ✔ |
| (ii) | `CheckedActionRequestV1::for_managed_parents` pins `Workspace` unconditionally | `coordinator/identity.rs:307-327`, line **`:323`**: `root_kind: PreCatalogRootKindV1::Workspace,` — no branch, no parameter. So no managed-parent action can be *identified* against a Git-directory root kind at all. | ✔ |
| (iii) | this door's lease is workspace-rooted by construction | `bootstrap/runtime/catalog_lease/witness.rs:52-70`, line **`:56`**: the `WorkspaceRuntime` arm of `facts()` returns `root_kind: PreCatalogRootKindV1::Workspace`. E4.2's lease is `WorkspaceMutatorLock::catalog_mutation_lease()` → `runtime/mod.rs:52` → `CatalogMutationLeaseV1::from_workspace_runtime` (`catalog_lease.rs:200-204`) — that arm and no other. | ✔ |
| (iv) | no production caller builds a Git-directory catalog lease | The only other lease source is `CatalogMutationLeaseSourceV1::LeaseSet` (`catalog_lease.rs:196`), whose `facts()` arm (`witness.rs:71-93`) *can* carry `root_kind: GitDirectory`. Its sole constructor is the private `from_held` (`:206-210`), reachable only via `CatalogLeaseSetV1::leases()` (`:177-181`). Grep for `.leases()` and `CatalogLeaseSetV1::try_acquire`: **every** hit is a test file. **No production Git-directory catalog lease exists.** | ✔ |

**Freeze `:672-680` record — accurate and dated.** I read the freeze block: it
is the purpose-policy-matrix paragraph, and `:682-685` is where *"the managed
parent's home on that variant is fixture-placed, with follow-up 3 (the
Git-directory catalog's workspace-root binding) attached as the owner decision
it awaits"* sits. §7.7 itself cites the range as `:672-680`; the builder's
citation matches the authority it was given. The recorded answer — *"the
workspace root binds it, the other variant carrying no production parent"* —
is the answer the evidence supports, and it disposes freeze follow-up 3.

---

## O13 — THE INVENTORY, MEASURED

**Per-file count map matches the tree exactly.** Hand-counted on masked-code
semantics (the `use` line contributes one reference per named writer):

```
archive.rs        :13 use crate::durable_fs::sync_dir;      :207 sync_dir(done)
                  -> {sync_dir: 2}                                   PIN {sync_dir: 2}          ✔
store/archive.rs  :3  use …::{rename_noreplace, sync_dir};   :63 rename_noreplace
                      :55 :56 :65 :66 :78 :79 sync_dir
                  -> {rename_noreplace: 2, sync_dir: 7}      PIN {rename_noreplace: 2, sync_dir: 7} ✔
store/rewrite.rs  :6  use …::{rename_durable, sync_dir};     :132 rename_durable  :139 sync_dir
                  -> {rename_durable: 2, sync_dir: 2}        PIN {rename_durable: 2, sync_dir: 2}  ✔
```

(`rewrite.rs:33`'s doc mention of `durable_fs` is inside a `///` line and is
masked out; it also names no writer.)

**`create_open`'s raw writers are GENUINELY GONE — converted, not wrapped.**
The diff deletes the whole staged-write / read-back / `rename_durable` /
`sync_dir(&directory)` block and its `create_temporary` call, replacing it with
a single `crate::checked_artifact::entry::create_merge_store_record(&root, &relative, &encoded)?`.
`create_open` no longer names `create_temporary`, `rename_durable` or
`sync_dir` at all, and no longer constructs `.gwz/merge`. The
`fs::create_dir_all` that was the raw when-missing bootstrap is gone from this
path.

**`commit`'s pair remains for E4.3.** `rename_durable` at `rewrite.rs:132` and
`sync_dir` at `:139` are both inside `commit` (`:77`–`:152`). Exactly E4.3's
half, exactly as §7.6.1 assigned.

### Red-probe transcripts (all run on copies via `--source`; worktree untouched)

```
--- PROBE A  moved count: rewrite.rs regains a third sync_dir      : exit=1
- O13 raw-writer count moved and must move the pin with it:
  workspace_ops/merge/v1_lifecycle/store/rewrite.rs:
  expected={'rename_durable': 2, 'sync_dir': 2} actual={'rename_durable': 2, 'sync_dir': 3}

--- PROBE B  regained reference OUTSIDE the pin: start.rs          : exit=1
- v1 lifecycle gained a raw durable_fs writer outside the O13 accepted residual:
  workspace_ops/merge/v1_lifecycle/start.rs

--- PROBE C  retirement direction: archive.rs loses durable_fs     : exit=1
- O13 accepted-residual entry no longer names durable_fs and must be retired
  from the pin deliberately: workspace_ops/merge/v1_lifecycle/archive.rs

--- PROBE D  FULL UN-CONVERSION (e56124b's create_open restored)   : exit=1
- O13 raw-writer count moved and must move the pin with it:
  workspace_ops/merge/v1_lifecycle/store/rewrite.rs:
  expected={'rename_durable': 2, 'sync_dir': 2} actual={'rename_durable': 3, 'sync_dir': 3}
- checked entry reference set changed: create_merge_store_record:
  expected=['workspace_ops/merge/v1_lifecycle/store/rewrite.rs'] actual=[]
- checked entry module user set changed: … (rewrite.rs dropped from the user set)

--- PROBE E  entry.rs byte-pin tamper                              : exit=1
- protected source allowlist changed: checked_artifact/entry.rs

--- PROBE F  pre_catalog tree-digest tamper (the 2(a) site)        : exit=1
- protected source tree changed: checked_artifact/capability/pre_catalog.rs
```

Probe D is the decisive one: silently reverting the conversion fires the pin
**three independent ways**. Both directions the mandate named — a moved count
and a regained reference — fire, and so does the retirement direction the
widening added.

---

## COMPANION TRANSCRIPT — MEASURED BY ME

| Gate | Pinned | My measurement | ✔ |
| --- | --- | --- | --- |
| boundary checker | ok | `checked-artifact boundary: ok (18 visible entries, 8 classified modules)` — **exit 0** | ✔ |
| checker self-suite | green | `Ran 69 tests … OK (skipped=15)` — `RUN=69 FAIL=0 ERR=0 SKIP=15`, **exit 0**. The 15 skips are the `run_compiler_probe` subset, stubbed by me for the disk reason in flag 8, not by the tree. | ✔ (partial, disclosed) |
| privacy probe | green | Covered by the same suite module set; no separate `pytest` runner exists (`test_v1_lifecycle_privacy_probe.py` is `unittest`, and it is not a compiler-probe file). | ✔ |
| `v1_lifecycle::` (skip `root_fault_matrix`) | 260 | **`260 passed; 0 failed; 1558 filtered out`** — MEASURED, darwin | ✔ |
| `checked_artifact::` | darwin 457 | **`457 passed; 0 failed; 1361 filtered out`** — MEASURED, darwin | ✔ |
| `root_fault_matrix` (release) | 1 | **`1 passed; 0 failed; 1817 filtered out`** — MEASURED | ✔ |
| lib remainder | 1099 + 1 ignored | **`1099 passed; 0 failed; 1 ignored; 718 filtered out`** — MEASURED | ✔ |
| build | — | `cargo build --tests` exit 0 on the warm cache | ✔ |

**Digests.** Three moved (`checked_artifact/entry.rs` in
`PROTECTED_SOURCE_DIGESTS`; `capability/pre_catalog.rs` and
`v1_lifecycle/mod.rs` in `PROTECTED_SOURCE_TREE_DIGESTS`), the rest unchanged.
The checker's green run recomputes all 17 file digests and all 7 tree digests
against the tree, so every pinned value is proved to match; probes E and F
confirm the pins are live in both sets. *Arithmetic nit: five other tree
digests, not six — [P3-4].*

**Driver provenance block, house form — VERIFIED.**
`run_r4bg_aggregate_gates.py:360-387` follows the established shape: each
partition, old → new, with `MEASURED` / `DERIVED` and the ground.

* `v1_lifecycle 257 → 260` — **MEASURED**, and the three rows are named
  individually. **I read the new tests for cfg gates myself**: none of the
  three carries any `cfg`. They plant directories and a YAML leaf; the fault
  row uses `CheckedArtifactFault::BeforeManagedPublication` via
  `fail_next_checked_artifact_at`, which is cross-platform and census-free
  (it is an existing key — the fault census stays at 165, unmoved). The claim
  "no per-OS split in this partition" holds.
* `checked_artifact darwin 456 → 457` — **MEASURED**; the row is the [P3-2]
  guard in `interface_tests::contracts`, again ungated. It constructs typed
  errors via `CheckedFsError::unsupported/io/ambiguous` and reads rendered
  strings — no filesystem, no platform probe.
* `checked_artifact linux 466 → 467` — **DERIVED (+1), cfg-independent,
  FIRST-DISPATCH-EXPECTED**, with *"a measured number wins"* stated. Correct
  form, and the cfg-audit ground is one I independently confirm from the test
  source.
* `lib remainder 1099 / 1100` — **UNMOVED**, re-measured by me at 1099 + 1
  ignored.

**Tripwire — green and UNMOVED at one, argument verified.**
`interface_tests/catalog_activation_pin.rs:46` `PRODUCTION_CALLER_COUNT = 1`,
unmodified by this commit. The pin's mechanism (`:31-46`, `:76-...`) counts
**files** outside the owner's two that *name* `recover_or_create`, not call
sites. E4.2's two new `recover_or_create(...)` calls are both inside
`bootstrap_merge_start_parents` in `checked_artifact/entry.rs` — the one file
the pin already counts. So the count is correctly unmoved, and the pin's own
forward-looking sentence (*"A SECOND production namer is E4.2-E4.6 work and
must move this pin again"*) remains accurate, since E4.2 added no second file.
The disposition is recorded at the `ENTRY_REFERENCES` pin in the checker. The
pin passed inside the measured 457.

---

## CAP, RIDER AUDIT, TRAILERS

**Cap — EXACT-500 claim verified independently.**

```
$ git diff --shortstat e56124b 7214010
 12 files changed, 500 insertions(+), 75 deletions(-)
```

All twelve files are handwritten (ten Rust, two Python checkers); no generated
file is in the changeset, so whole-added = 500 on the stated basis. At the
ceiling, not over it; the plan's `<500` is aspirational per the standing lane
rule, and E4.1(c)'s 331-MEASURED precedent shows the lane rules on measured
figures, not estimates.

**Reduction trail — spot-checked for lost load-bearing content.** All 75
deletions are visible in the diff and account for themselves:

| site | deleted | judgment |
| --- | --- | --- |
| `entry.rs` −23 | the three inline `match` arms of `activate_workspace_catalog`'s `map_err` | **MOVED, verbatim**, into `render_catalog_refusal`, including the "Precondition 1" comment. Nothing lost. |
| `rewrite.rs` −31 | the raw staged-write/verify/rename/`sync_dir` block (code) + two doc sentences | The *"in `commit`'s own staged/fsync/rename/verify shape, so a crash leaves either no record or one complete valid v1 record"* sentence is superseded by the checked boundary's own atomicity. The load-bearing sentence — the **single-open-record invariant** — is **preserved and restated**: *"The single-open-record invariant survives: the pre-flight existence check stays and the checked replacement publishes no-replace onto an absent leaf."* Both halves of the original ground (`path_exists` pre-flight; no-replace publication) are still named. **No load-bearing content lost.** |
| `coordinator/mod.rs` −2 | the old `#[allow]` reason string | Replaced with a truer one; the [P3-5]-class citation is preserved. |
| `start.rs` −3 | the E4.1 comment | Replaced with the E4.2 comment stating the stronger ordering. |
| checkers −16 | the `frozenset` → `dict` conversion and the two membership loops | Behaviour strictly widened; probes A–D prove no direction was lost. |

**Scope, file by file — NO RIDER FOUND.**

| file | +/− | traces to |
| --- | --- | --- |
| `scripts/checks/check_checked_artifact_boundaries.py` | 85/14 | O13 pin widening (charter); entry.rs digest + `ENTRY_*` inventories (ritual); [P3-4] scan-hole record (carrier) |
| `scripts/checks/run_r4bg_aggregate_gates.py` | 30/2 | provenance block + pin moves (ritual) |
| `capability/pre_catalog/provider/managed_mutation.rs` | 7/0 | §11.3 item 2(a) — charter |
| `coordinator/execution.rs` | 44/0 | row `:273`'s two sessions — charter |
| `coordinator/mod.rs` | 6/2 | the privacy widening that makes the charter implementable (flag 3) |
| `entry.rs` | 116/23 | rows `:273`/`:280`'s doors + item 2(b) record (charter); [P3-2] renderer + label (carrier) |
| `identity.rs` | 13/0 | [P3-3] carrier, doc-only |
| `interface_tests/contracts.rs` | 45/0 | [P3-2]'s driving row (carrier) |
| `v1_lifecycle/checked.rs` | 24/0 | `acquire_for_merge_start` + its test door — charter |
| `v1_lifecycle/start.rs` | 4/3 | the call site — charter |
| `v1_lifecycle/store/rewrite.rs` | 16/31 | the conversion — charter |
| `v1_lifecycle/tests/checked.rs` | 110/0 | the three frozen-ordering rows — charter |

Every added line traces to row `:273`, row `:280`/O13, §11.3 item 2, one of the
five inherited carriers, or the standing companion ritual. Flag 7's pre-check
is the only "beyond the literal charter" candidate and I judge it in-charter
(above).

**Trailers.**

```
$ git log --format='%B' e56124b..HEAD | grep -icE 'co-authored-by|generated with|noreply@anthropic'
0
```

Clean. One commit on the branch; message is prose + structure, no AI
attribution of any form.

---

## ONE-LINE MANDATE DISPOSITIONS

1. **Frozen ordering** — IMPLEMENTED and DRIVEN on both clauses by fault
   injection and ordering witnesses; "durable" is real fsync/barrier durability
   self-performed by the conversion; no interleaving can invert either clause
   (single production caller, workspace-wide exclusive lock, and a refusal
   behind it); **one stated gap: the proof-only branch inherits rather than
   establishes clause one — [P3-2]**.
2. **Refusal semantics** — `acquire_for_merge_start` bootstraps, and it is
   ordered before the **only** production `create_open` caller; the A1 adapted
   route rewrites in place through `store/atomic_upgrade.rs` and **never**
   reaches `create_open`, so the E4.1-round-1 wedge class is not reborn;
   abort/plain-lease untouched, [P3-C1] closed structurally **and** driven;
   [P3-8] correctly needed no growth (those rows drive `Abort`, which takes the
   plain lease).
3. **Eight flags** — 1 CONFIRMED (→C2), 2 RE-RUN and fail-closed in three
   directions, 3 CONFIRMED and minimal, 4 CONFIRMED and pre-blessed at review
   `:822`, 5 accepted with [P3-3], 6 judged (row `:280` not breached; §8/§9
   real, unowned, under-recorded → **[P2-1]**), 7 load-bearing not rider, 8
   noted.
4. **§11.3 item 2** — (a) SURVIVES, disposition dated **at** the door, the
   one-component-vs-depth-d evidence and all three test callers verified; (b)
   answered, **all four pieces of tree evidence independently verified** at the
   exact lines cited, recorded against freeze `:672-680`, dated.
5. **O13** — per-file count map matches the tree **exactly**; `create_open`'s
   raw writers genuinely GONE (converted, not wrapped, and the parent
   `create_dir_all` with them); `commit`'s pair intact for E4.3; the plan's
   "retires to empty across E4.2/E4.3" is FALSE — two files are E4.4's.
6. **Companions** — checker green pristine; checker suite 69/0/0 (15 compiler
   probes disclosed-skipped for disk); **CA 457 / v1 260 / remainder 1099+1 /
   root_fault_matrix 1 all MEASURED BY ME**; digests proved by the green
   checker and by probes E/F; provenance block in house form with the cfg audit
   confirmed from the test source; tripwire green and correctly UNMOVED at one.
7. **Cap + riders + trailers** — 500/75 measured by `git` on the stated basis;
   reduction trail spot-checked, no load-bearing content lost; scope clean
   file-by-file, no rider; zero trailers.

---

## FOR ROUND 2 (if the lane calls one)

Round 1 asks for **documents, not code**:

* **C1** — mint and date the §8 `:239-240` / §9 `:264-266` obligation with an
  owner (§7.6.1's O13 form is the template); cure
  `coordinator/execution.rs:10-38`; state at
  `execute_merge_start_managed_parents` which gate E4.2 opened and which it did
  not.
* **C2** — the dated plan correction at `GwzM5-8R2E-Plan.md:398`.
* **[P3-1]** — four `#[allow]` reasons, one line each; the lane's own standing
  order names this commit as their owner.
* **[P3-2]**, **[P3-3]**, **[P3-4]** — one sentence each at their doors /
  in the message; may ride E4.3 if the lane prefers.

No code change is required by any finding. The conversion itself is sound.

---

## ROUND 2 — the folds

**Fold commit.** `1f47d6e` ("E4.2 round-1 fold: the reachability record cured,
the authority gate stated") on `e4/e4-2-first-merge-record`, on top of the
reviewed `7214010`. **5 files, +36/−13**, comments and docs only plus one tree
digest. Worktree byte-clean at `1f47d6e` on entry and exit; same warm target
dir reused; probes again run on copies via `--source`. **Zero trailers**
(`git log --format='%B' 7214010..1f47d6e | grep -icE 'co-authored-by|generated with|noreply@anthropic'` → `0`).

**Gates, re-measured by me on the fold:**

| gate | result |
| --- | --- |
| boundary checker | `ok (18 visible entries, 8 classified modules)` — **exit 0** |
| `cargo fmt --check` | **exit 0** |
| `cargo clippy --all-targets --all-features -- -D warnings` | **exit 0**, no diagnostics |
| `checked_artifact::` | **457 passed; 0 failed** |
| O13 red-probes A / B / D re-run | all still fire, D still three ways |
| `pre_catalog.rs` tree digest probe (on the folded `provider.rs`) | fires — `protected source tree changed: checked_artifact/capability/pre_catalog.rs` |

The three untouched partitions (`v1_lifecycle` 260, remainder 1099+1,
`root_fault_matrix` 1) are unaffected: the fold changes no Rust that any of
them compiles differently, and `cargo clippy --all-targets` recompiled the
whole tree clean.

### FINAL VERDICT

# GO — conditional on ONE mechanical revert

Three of the four condition folds discharge cleanly and are accurate against
the tree. **One hunk introduces a false, dated claim into a load-bearing
`#[allow]` reason**, and it does so because **my own round-1 finding
mis-attributed the site**. That is a [P2], so **escalation fires** — but the
remedy is a one-hunk revert to text that was already reviewed and green at
`7214010`, so I do not believe it needs a third round under the two-round cap.
The lane owner may treat it as a mechanical pre-landing correction.

### [P2-R2-1] `capability/pre_catalog/provider.rs:25-30` — the fold replaced a TRUE allow reason with a FALSE one

**The new text** (`provider.rs:25-30`):

> `reason = "E2 landed the barrier edges and their matrix; the first production consumer arrived at R2-E E4.2 (advance_one's stage_component, 2026-09-01) — the allow stays for the arms no converted consumer drives yet"`

**Both halves are false.**

1. **`stage_component` is not in `barrier_mutation`.** It is
   `capability/pre_catalog/provider/managed_mutation.rs:482` — a different
   module, covered by a different allow. Freeze `:592-593` places
   `stage_component` and `write_or_rewrite_marker` in `managed_mutation.rs`;
   what they *cross* are the boundary/fault KEYS named "staging/ownership
   marker", not `mod barrier_mutation`'s functions.
2. **`mod barrier_mutation` has ZERO production consumers — E4.2 reached
   nothing in it.** Its entire exported family
   (`write_barrier_intent_scratch`, `observe_barrier_intent_row`,
   `converge_target_anchor_alias`, `create_target_anchor_alias`,
   `barrier_target_parent`, `retire_target_anchor_alias`,
   `observe_barrier_completion`) is re-exported at `provider.rs:79-84`, wrapped
   as `ActionNamespace` methods in `namespace/host.rs`, and **called from
   exactly one file**:

```
$ grep -rn "\.\(write_barrier_intent_scratch\|observe_barrier_intent_row\|converge_target_anchor_alias\
\|create_target_anchor_alias\|barrier_target_parent\|retire_target_anchor_alias\
\|observe_barrier_completion\)(" --include='*.rs' src/ | grep -v host.rs | sed 's/:.*//' | sort -u
src/checked_artifact/namespace/tests_barrier_matrix.rs
```

E4.2's production chain — `execute_row` → `settle_generation` / `advance_one` /
`retire_generation` — calls only the bootstrap-intent and managed-component
namespace methods. None is a barrier or target-anchor method.

**The comment now contradicts itself.** The module doc immediately above, at
`provider.rs:20-24` and untouched by the fold, still reads: *"Its consumer is
the E4 conversion that gives the roaming anchor its first admitted action."*
E4.2's admitted action is a `ParentOnly` merge-start managed-parent action and
touches no roaming anchor — plan `:319-320` is explicit that *"E2.2 strictly
precedes any E4 row that admits actions touching the roaming anchor."* So the
doc says the consumer is still ahead; the allow now says it already arrived.

**Provenance — mine.** My round-1 [P3-1] table listed this site with the ground
*"the barrier edges … reached via `advance_one` → `parent.stage_component(...)`"*.
I conflated the freeze's "barrier edges" (fault keys crossed by
`managed_mutation.rs`) with `mod barrier_mutation` (the target-anchor/roaming-anchor
protocol). The builder folded my error faithfully. **The pre-fold reason —
"E2 lands the barrier edges and their matrix; the production consumer is R2-E
Phase E4" — was correct and was not stale.**

**Remedy.** Revert hunk 3 of `1f47d6e` (`provider.rs:25-30`) to its
`7214010` text. If the lane prefers to keep a dated note rather than a bare
revert, the true statement is: *"still no production consumer as of E4.2 —
E4.2 reached the MANAGED mutation family (`managed_mutation.rs`), not this one;
this module's consumer remains the E4 row that admits a roaming-anchor
action (2026-09-01)."* Nothing else in the commit is affected; `provider.rs`
sits inside the byte-pinned `pre_catalog` tree digest, so whichever text lands
must move that digest with it.

**[P3-1] therefore lands 3-of-4.** The other three rewrites are accurate:

| site | new reason | verified |
| --- | --- | --- |
| `bootstrap/managed.rs:15-19` (`mod provider`) | "REACHED … at R2-E E4.2 (admit/execute_merge_start_managed_parents)" | ✔ `RetainedManagedParentProviderV1::from_retained_catalog` is called from `admit_merge_start_managed_parents` |
| `bootstrap/managed.rs:43-47` (`use provider::*`) | same module | ✔ |
| `namespace/mod.rs:47-51` | "REACHED … via the provider's `execute_row`" | ✔ `provider.rs:410-411` binds `ActionNamespace<HostActionNamespaceV1>` through `retain_action_namespace` inside `execute_row` |
| `capability/pre_catalog/provider.rs:25-30` | "first production consumer arrived at R2-E E4.2" | ✘ **[P2-R2-1]** |

### C1(ii) — the module-doc cure: DISCHARGED

All three of my round-1 bullets are answered, and the cure quotes the falsified
paragraph so the record shows what was wrong:

| my round-1 bullet | cured by |
| --- | --- |
| *"nothing here is reachable from an entry point"* | *"the chain `entry.rs` → `v1_lifecycle/checked.rs` → `start.rs` → merge dispatch reaches them on every checked v1 start"* ✔ (and "every checked v1 start" is exact — `handle_start_durable_v1` runs whenever `select_record_version(...) == V1`) |
| *"no consumer calls it"* | *"`entry.rs` imports `admit_merge_start_managed_parents` and `execute_merge_start_managed_parents` from this module"* ✔ (`entry.rs:13-15`) |
| *"Production catalog activation also remains forbidden outright"* | *"production catalog activation landed (E4.1)"* ✔ |

Dated 2026-09-01, cites "E4.2 review C1". **The surviving sentence is
accurate**, and I checked it rather than taking it: *"`entry.rs` still routes
the BUNDLE parent through `CheckedArtifact::prepare_parent` — that conversion
is a later E4 row's."* `entry.rs:150-157` is
`prepare_merge_store_parents` → `CheckedArtifact::prepare_parent(root, Path::new(crate::stash::STASH_BUNDLE_DIR), …, "preservation bundle parent")`,
and its **one** production caller is `workspace_ops/merge/store/mod.rs:131` —
the **v0** record-persist path (`MERGE_DIR` / `open_path` / `record_files`),
outside `v1_lifecycle/` and so outside O13's inventory. Its conversion is row
`:276`'s — **E4.5**. Sentence stands.

### C1(iii) — the opened-vs-closed gate statement: DISCHARGED

`execute_merge_start_managed_parents`' new paragraph carries all five elements I
asked for, each accurate:

* opened gate named — *"admission plus the facade re-proof — the DURABILITY gate"* ✔
* closed gate named — *"the AUTHORITY gate stays closed: `authorize_write`/`RetainedWriteAuthorityV1` still have no production consumer"* ✔ (re-measured: still tests-only)
* §9 `:264-266` named on the right ground — *"the record leaf is written from a path, which §9 `:264-266` names as NOT parent authority"* ✔
* the misreading pre-empted — *"The re-proof here must not be read as satisfying §9 `:266`"* ✔ — this is the sentence that answers my sharpest round-1 concern
* owner named — *"that conversion is the plan's minted O14, decided at E4.6's chartering"* ✔

### [P3-2] — the proof-only qualifier: DISCHARGED

`admit_merge_start_managed_parents` now states it exactly:
*"residency comes from a bounded, identity-proved WALK (an observation, not a
flush), so 'both parents durable' is INHERITED from whoever installed them, not
established here; only the bootstrap path barriers … Converged does not imply
flushed."* Faithful to the finding and to the E7.2 clause.

*Minor, not a finding:* the clause sits on the function that **produces** the
`None`, while `entry.rs:349-351` is where the early return **consumes** it, and
that door's sentence (*"A proof-only plan … IS the row's 'when missing'
qualifier"*) is unchanged. A reader must follow one hop. Acceptable — `entry.rs`
is byte-pinned, so it cannot drift unreviewed.

### [P3-3] / [P3-4] — carried, as round 1 permitted

Not folded, consistent with round 1's *"may ride E4.3 if the lane prefers"*.
[P3-3] (the Windows ground over-reaches — `provider/platform/windows.rs:182-188`
downgrades Win32 1/50/87 from the same two APIs) and [P3-4] (the commit
message's "other six" should be five) both stand as open P3s.

### C1(i) — the O14 text: FAITHFUL, with two corrections

The text captures my [P2-1] accurately and, in one respect, improves on it: my
finding posed the question, and **THE FORK** formalises that the resolution may
legitimately be to *amend §8/§9 to the landed architecture* rather than to
convert — with the correct note that a frozen-text move needs its own review
tier. Every load-bearing element of my finding survives: ZERO production
consumers, "path-based … which §9 `:264-266` names as NOT parent authority",
the type doc's promise, "Unowned before this record", "Row `:280` is NOT
breached", and "O1's close must cite O14's disposition". **Nothing is
weakened.**

Two corrections:

1. **Scope the interim discipline to E4.3–E4.6, not E4.3–E4.5.** As written,
   the binding clause is *"binding on E4.3-E4.5's charters"* while the fork is
   decided *"at E4.6's chartering"*. E4.6 is itself a conversion step (workspace
   checked mutation + `.git/info/exclude`), so its own execution seams need the
   same opened/closed statement regardless of when the fork is decided. Say
   **E4.3–E4.6**.
2. **Add the evidentiary anchor: E4.2 is the step that made the gap
   demonstrable.** The text says the gate is unowned but not *how we know* — and
   that fact is what stops a later reader concluding the state was always fine.
   Suggested clause, after "no E4 step's charter names the conversion":
   *"E4.2 is where this became demonstrable rather than latent: it mints the
   first production `AdmittedCheckedActionV1` and the first production
   `ManagedParentFacadeV1`s, revalidates them — §9 `:266`'s facade operation —
   and then discards them, writing the leaf from a path."*

### C2 — the O13 correction: FAITHFUL but INCOMPLETE

The corrected assignment is exactly right and matches my measurements: E4.2
(creation), E4.3 (`store/rewrite.rs`'s `commit` pair — `rename_durable:132` /
`sync_dir:139`, both inside `commit` at `:77-152`), E4.4 (both archive files —
`archive.rs:207`'s `remove_archive` `sync_dir(done)` and `store/archive.rs`'s
`MergeArchive` raw bootstrap at `:55-79`). "The inventory empties across
E4.2-E4.4" is correct.

Three completions before it lands:

1. **It must correct TWO sites, not one.** The claim appears at **`:398`** (the
   E4.3 bullet, the one the coordinator names) **and at `:88`** — §1.1's O13 row
   itself: *"retired to empty in E4.2/E4.3's own commits."* Correcting only
   `:398` leaves the ledger entry carrying the same falsehood.
2. **Say that E4.2 retires no FILE.** `store/rewrite.rs` stays in the inventory
   after E4.2 with reduced counts and only leaves at E4.3. This is precisely why
   the pin had to widen from a bare file set to per-file counts (the builder's
   flag 2), so recording it keeps the correction and the widening's rationale
   coupled. Suggested: *"E4.2 reduces `store/rewrite.rs`'s counts and retires no
   entry — which is why the pin widened to per-file counts; E4.3 retires the
   entry."*
3. **Refresh the pin's description at `:84-87`.** It still describes O13's pin
   as *"a boundary-checker inventory of `v1_lifecycle/`'s non-test `durable_fs`
   writer **files**"* with an "executed surface" file list. Since `7214010` the
   pin is a per-file **count map**. One clause.

**Do NOT touch `:89-94`.** The dated accepted-residual record there is scoped to
*"the v1 store … on the no-ff path, in breach of row `:280`'s second clause"* —
i.e. to `store/rewrite.rs`, not to the archive files — so its stated expiry
"with E4.2/E4.3's landings" is **correct as written** and must not be swept
along with the retirement correction.

### Round-2 rider audit

All +36 lines are comments, docs, or the one digest re-pin, and every one
traces to a named condition: C1(ii) → `coordinator/execution.rs` module doc;
C1(iii) → `execute_merge_start_managed_parents` doc; [P3-1] → four allow
reasons; [P3-2] → `admit_merge_start_managed_parents` doc; the
`pre_catalog.rs` tree digest moves with `provider.rs`, correctly. **No rider.**
No behaviour changed; no test moved; no count moved.

### Round-2 disposition summary

| item | status |
| --- | --- |
| C1(ii) module-doc cure | **DISCHARGED** |
| C1(iii) opened/closed gate | **DISCHARGED** |
| [P3-1] four allow reasons | **3 of 4 DISCHARGED**; the 4th → **[P2-R2-1]**, one-hunk revert |
| [P3-2] proof-only qualifier | **DISCHARGED** |
| C1(i) O14 text | **FAITHFUL** — 2 corrections (scope E4.3–E4.6; add the E4.2-made-it-demonstrable clause) |
| C2 O13 correction | **FAITHFUL, INCOMPLETE** — also correct `:88`; state E4.2 retires no file; refresh the pin description at `:84-87`; leave `:89-94` alone |
| [P3-3], [P3-4] | carried open, as round 1 permitted |
| gates | checker ok, fmt 0, clippy 0, CA 457, probes all fire |
| trailers | 0 |

**FINAL: GO** once `provider.rs:25-30` is reverted (or restated true) and the
two root-side texts land with the corrections above. The conversion itself —
reviewed at `7214010` and unchanged by this fold — stands.
