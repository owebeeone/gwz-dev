# R2-F E0.1-pattern — the relocation reality trace (dated note, 2026-09-01)

Read-only, filed before any R2-F relocation plan is chartered, in the shape
`GwzM5-8R2E-E01ReachTraces.md` established: a tree-verified trace whose job is
to say what the code *is*, not what the package should do. **No verdicts, no
design.** Where the tree and the records disagree, both are stated and the
disagreement is left for the plan.

Traced at gwz-core `main` = `ea3a9246e5230a2b6ef04e269c9454283bd6db0a`
(`chore(release): gwz-core 0.12.1`, one commit past `8ce9281`); working tree
clean. Nothing was mutated: no commit, no edit, no build. All line numbers are
that commit's.

Controlling records consulted: `GwzM5-8R2DPhase4Closure.md` §2.2-§2.7 (the
2026-08-23 quarantine/relocation decision and the A1 gate),
`GwzM5-8R2E-Plan.md` §1.1 (the single-owner claim, the operator's 2026-08-27
ruling (a), and OPEN-R1), `GwzM5-8R2F-EvidenceMap.md` §2.3 rows F-4/F-5/F-7.

---

## 0. One vocabulary correction the records predate

The records speak of "the legacy leaf private root" and "the catalog Final
directory" as two descriptions of one path. The tree now carries a typed
vocabulary for that path which did not exist when the decision was written, and
it is **dual-rooted**. Everything below uses the tree's names:

`src/checked_artifact/catalog_names.rs:5-18`

```rust
pub(in crate::checked_artifact) enum CatalogPrivateRootV1 {
    Workspace,
    GitDirectory,
}
...
    const fn prefix(self) -> &'static [u8] {
        match self {
            Self::Workspace => b".gwz",
            Self::GitDirectory => b"gwz",
        }
    }
```

`catalog_names.rs:20-26` adds `CatalogPrivateNameV1 { Final, BootstrapScratch,
BootstrapActive, BootstrapStaging }`, with `Final`'s leaf `b"checked-artifacts"`
(`:38`), and `relative_path` composing the two (`:53-61`).

**Two different things are called "private parent" in this tree, and the
distinction is the whole of the coexistence problem:**

| Caller | Meaning | Site |
| --- | --- | --- |
| `policy::CheckedArtifactPolicy::private_parent()` | the **Final directory itself** — `.gwz/checked-artifacts` or `gwz/checked-artifacts` | `policy.rs:33-42` |
| `provider::retained::RetainedPlatformRoot::private_parent()` | the **container** — `.gwz` or `gwz`, of which the Final directory is a child | `retained.rs:151-153`, populated `:207-208` / `:241-246` |

The catalog opens its Final directory as a child of the container
(`completed.rs:75-84`, `open_observed_directory(parent.handle(),
OsStr::new(private_name(CatalogPrivateNameV1::Final)), …)`). The legacy leaf
writer opens the Final directory *as* its private area. Phase4Closure §2.2's
"there is one directory, and two grammars claim it" is re-verified verbatim at
`ea3a924` — now stated in the dual-rooted vocabulary rather than by path
literal.

---

## 1. The `CatalogPrivateRootV1` story

### 1.1 Every variant and every construction site

Two variants (`catalog_names.rs:6-9`). Construction sites, non-test:

| Site | Variant | Path |
| --- | --- | --- |
| `policy.rs:36` | `Workspace` | `.gwz/checked-artifacts` |
| `policy.rs:39` | `GitDirectory` | `gwz/checked-artifacts` |
| `capability/pre_catalog/provider/filesystem.rs:178` | `Workspace` | catalog observation of a workspace root |
| `filesystem.rs:196-197` | `GitDirectory` | catalog observation of a Git-directory root |
| `capability/collision.rs:236` (`checked_v1()`) | `Workspace` | the private-control-domain constructor |
| `capability/collision.rs:239` (`for_root`) | either | the general constructor |

Test-only construction sites appear in seven `tests_*` modules
(`bootstrap/managed/tests_provider.rs:65-69`,
`pre_catalog/provider/tests_leaf_fault_matrix.rs:117-121`,
`tests_terminal_fault_matrix.rs:145-149`, `tests_retired_root.rs:55-59`,
`tests_authority_record_matrix.rs:163-167`,
`admission/tests_fault_matrix.rs:99-103`,
`namespace/tests_fault_matrix.rs:120-124`), each a `private_root()` helper that
maps a `TargetVariantV1` onto the enum so the matrices run both roots.

### 1.2 What selects Workspace vs GitDirectory today

**There is no relocation selector.** Two independent selectors exist, and
neither is a policy knob:

1. **Legacy leaf writer** — the root follows *which filesystem owns the
   artifact*, fixed by which `entry.rs` constructor the caller names:
   `entry.rs:155`, `:164`, `:173` construct `CheckedArtifactPolicy::workspace`;
   `entry.rs:182` constructs `CheckedArtifactPolicy::git_directory`. The choice
   is made by purpose at the call site, not by configuration.
2. **Catalog** — the root comes from the lease target:
   `filesystem.rs:173-200` splits `observe_workspace` / `observe_git_directory`,
   driven by `PreCatalogRootKindV1`. `interface_tests/capability_permit.rs:64`
   forbids the token `"fn git_directory("` on the catalog surface and `:79`
   *requires* `"repository_common_git_directory"` — the design pin is that the
   catalog has exactly one root-agnostic entry (`recover_or_create`) and the
   root rides the lease target.

### 1.3 Is GitDirectory live machinery, test scaffolding, or wired-but-unreachable?

**All three, in different subsystems. The precise answer:**

**(a) Live production, read-only — the legacy leaf writer's GitDirectory root
is reached on every merge preservation preflight.**

`src/git/gitbackend/preservation_root/files.rs:28-35`

```rust
pub(super) fn observe_boundary(root: &Path, expected: &[u8]) -> ModelResult<bool> {
    let repo = git2::Repository::open(root).map_err(git_error)?;
    crate::checked_artifact::entry::observe_merge_preservation_git_directory(
        repo.path(),
        Path::new("info/exclude"),
        Some(expected),
    )
}
```

Callers: `preservation_root.rs:20`, `:177`, `:250`, `:284`. Chain:
`entry.rs:86-92` → `entry.rs:180-187` (`preservation_git_directory`) →
`CheckedArtifactPolicy::git_directory(root)` → `policy.rs:38-40` →
`gwz/checked-artifacts`. This is the **only** production consumer of the
`GitDirectory` arm of `CheckedArtifactPolicy`.

**(b) Wired-and-reached but inert — no production write lands under the git
directory through the legacy machinery.**

`CheckedArtifact::acquire` computes the value unconditionally
(`observation.rs:92-93`):

```rust
let private_root = policy.artifact_root().to_path_buf();
let quarantine_parent = policy.private_parent();
```

so `gwz/checked-artifacts` *is* computed on that production path. But it is a
pure `PathBuf`; the only code that turns it into filesystem I/O is
`residue.rs::open_private` (`residue.rs:45-104`, using the field at `:49` and
`:59`), and `observe()` (`observation.rs:123-135`) never calls it. The
git-directory entry point reaches only `observe()`:
`observe_merge_preservation_git_directory` → `observe_expected`
(`entry.rs:189-191`) → `artifact.observe()`. **No production path constructs a
`git_directory` policy and then mutates.**

**(c) Test-reachable only — the catalog's own writer under `<git-dir>/gwz`.**

`capability/pre_catalog/provider/mutation.rs:66-99`
(`create_git_private_parent`) does create `<git-dir>/gwz`, and hardcodes the
name at `mutation.rs:70` (`let name = OsStr::new("gwz");`) rather than calling
`catalog_names.rs`. It is reached from `pre_catalog.rs:587-600`
(`execute_owner_create_and_retry`) ← `catalog/bootstrap.rs:258` ←
`recover_or_create`, which has zero production callers (§3). So the catalog's
git-directory write path is live code with a complete fault-instrumented edge
and no production reach.

### 1.4 What the fault-key census and the freeze annotations say

**The census.** `src/checked_artifact/interface_tests/fault_expected_keys.rs`
is the only one: `EXPECTED_STABLE_KEYS` (`:7-173`, declaration order),
`EXPECTED_KEY_COUNT = 165` (`:174`), a per-family activation map (`:307-461`,
ten families summing to 165), and a declared injection-source list (`:534-623`).
`fault_vocabulary_exactly_matches_the_independent_stable_key_fixture` (`:1012`)
asserts both length and set equality (`:1020-1021`); the completeness anchor
`the_declared_injection_sources_are_every_production_source_holding_sites`
(`:854-868`) rescans the real `src/checked_artifact` tree for
`CheckedArtifactFaultKeyV1::` and asserts set equality against the declared
list. `assert_eq!(declared.len(), 10, …)` at `:874-879`.

**Three keys name this territory**, so the census does speak to it:

- `:8` `"runtime.git_dir_retain"`
- `:26` `"catalog_bootstrap.git_parent_create"` — injected at `mutation.rs:73`
- `:27` `"catalog_bootstrap.git_parent_reobserve"` — injected at `mutation.rs:85`

The whole `catalog_bootstrap.*` family (25 keys, `:26-50`) is declared
`FaultFamilyActivationV1::Executed("R2-C2 physical first-catalog owner")` at
`:313-317`. No key is *per-root*: the two above name the git-parent **edge**,
not the root selection, so a relocation that changes only the private-area name
does not by itself move the 165 census. The legacy enum is explicitly exempt —
`fault.rs:66`: `// free — this enum is census-free, unlike
\`CheckedArtifactFaultKeyV1\`.`

**Freeze annotations.** There is **no `#[expect(...)]` anywhere** in
`src/checked_artifact/`, and `Cargo.toml` has no `[lints]` section enabling
`unfulfilled_lint_expectations` — every freeze is an `#[allow]`, which produces
no diagnostic when it becomes unnecessary. `mod.rs` carries seven blankets
(`:10-14`, `:16-20`, `:27-33`, `:35-39`, `:42-46`, `:48-52`, `:57-61`); six
share `"frozen interface awaiting R2-E consumer conversion (plan §5 item 1);
narrowed at Phase 4 Step 4.3 to the subtrees that still carry one"`, and the
seventh (on `pub(crate) mod entry`) reads `"…production activation of the
remainder is R2-E's (plan §5 item 1)"`. Neither `mod catalog;` (`:22`) nor
`mod catalog_names;` (`:23`) carries one.

**The one R2-F sequencing note that lives in the code**, verbatim from
`src/checked_artifact/namespace/mod.rs:8-23`:

> ```
> /// R2-D Phase 2 Step 2.2 — the retained-handle production backend.
> ///
> /// **The allow's expiry, re-anchored at R2-E Phase E2** (E2 review [P3-5]). Its
> /// reason has said "entry-point reachability is R2-E" since Step 2.2. Two things
> /// have moved under it since: E2 adds six barrier role methods with no
> /// production caller (only `tests_barrier_matrix` reaches them), and Phase E4 —
> /// the step that was to supply every entry point — was re-scheduled behind
> /// R2-F's quarantine/relocation package at the E0 close, under the operator's
> /// 2026-08-27 ruling (a). So the allow's expiry is **E4's landing, wherever
> /// R2-F puts it**, not "R2-E" generically; if E4 has not landed by the R2-E
> /// settle, E7 owes this allow a dated re-owning rather than letting it become
> /// permanent by silence.
> ```
> `reason = "the managed-parent provider has driven this backend since Step 3.1;
> entry-point reachability is Phase E4, itself sequenced behind R2-F's relocation"`

### 1.5 Five spellings of the root prefixes; one guarded

`CatalogPrivateRootV1::prefix()` (`catalog_names.rs:12-18`) has exactly one
caller — `relative_bytes` at `catalog_names.rs:47`. The other four re-spell the
literals:

- `catalog_names.rs:55-56` — `relative_path` re-matches `".gwz"` / `"gwz"`
  inside the owner itself
- `capability/pre_catalog/provider/filesystem.rs:211-212` — `b".gwz"` / `b"gwz"`
- `capability/pre_catalog/provider/namespace.rs:73-74` — `b".gwz"` / `b"gwz"`
- `capability/pre_catalog/provider/mutation.rs:70` — bare `OsStr::new("gwz")`

Only the `policy.rs` routing is pinned (`interface_tests/catalog_recovery.rs:235-242`),
and that pin is tautological against `policy.rs:36`/`:39` — it proves policy
does not hardcode, not that the string values are right.

---

## 2. The legacy private parent's current definition and complete production
consumer set

### 2.1 Definition and binding

`src/checked_artifact/policy.rs:33-42` — the whole of it:

```rust
    pub(super) fn private_parent(&self) -> PathBuf {
        match self {
            Self::WorkspaceArtifact { .. } => {
                CatalogPrivateNameV1::Final.relative_path(CatalogPrivateRootV1::Workspace)
            }
            Self::GitDirectoryArtifact { .. } => {
                CatalogPrivateNameV1::Final.relative_path(CatalogPrivateRootV1::GitDirectory)
            }
        }
    }
```

Bound at `observation.rs:93` into the `quarantine_parent` field
(`mod.rs:115`).

### 2.2 The complete consumer set, by layer

**Sole call site of `private_parent()`:** `observation.rs:93`. (Plus
`interface_tests/catalog_recovery.rs:236`/`:240`, tests.)

**Sole filesystem consumer of `quarantine_parent`:** `residue.rs::open_private`
(`residue.rs:45-104`), reading the field at `:49` (parent preparation) and `:59`
(component walk).

**All six callers of `open_private`, none test-gated:**

| Site | Function | `create` |
| --- | --- | --- |
| `residue.rs:111` | `inspect_family` | `false` |
| `residue.rs:232` | `ensure_authority` | `true` |
| `residue.rs:334` | `ensure_goal` | `true` |
| `transition.rs:203` | `detach_existing` | `false` |
| `transition.rs:342` | `publish_goal` | `false` |
| `cleanup.rs:62` | `finish` | `false` |

**Production entry points that reach them** — all through the *workspace*
policy:

- `entry.rs:37-45` `replace_merge_root_artifact`, `:47-57`
  `classify_replace_merge_root_artifact`, `:59-65`
  `remove_merge_root_artifact`, `:67-76` `classify_remove_merge_root_artifact`
  → `root_artifact` (`:153-160`, `CheckedArtifactPolicy::workspace`). Callers:
  `workspace_ops/merge/root/artifact_facts.rs` and
  `workspace_ops/merge/abort/evidence.rs:148`, `:154`, `:160`, `:277`, `:292`,
  `:307`.
- `entry.rs:94-110` `replace_/classify_merge_preservation_workspace` →
  `preservation_workspace` (`:171-178`, workspace). Callers:
  `git/gitbackend/preservation_root/files.rs:43`, `:57`.
- `entry.rs:112-142` the preservation-bundle trio → `preservation_bundle`
  (`:162-169`, workspace). Caller:
  `workspace_ops/merge/preserve/checked_bundle.rs:57`.

Note that the **read-only classifiers reach it too**: `classify_replace` /
`classify_remove` (`classification.rs:62-75`) call `inspect_family`
(`classification.rs:108`, `:140`), so the production `observe_transition`
(`files.rs:51-63`) path already enumerates the legacy directory on every merge
preservation classification.

### 2.3 Re-verifying the single-owner claim

`GwzM5-8R2E-Plan.md` §1.1 (O3) states: *"the legacy private parent has exactly
one non-test owner (`policy.rs:33-42` defines it; `observation.rs:93` is its
only consumer)."* Against `ea3a924`:

- **`policy.rs:33-42` defines it — TRUE**, and the line range is still exact.
- **`observation.rs:93` is its only consumer — TRUE only of `private_parent()`
  as a function call.** The only *filesystem* consumer is
  `residue.rs::open_private`, reached from six sites in three files (§2.2). A
  relocation touches `open_private`'s behaviour, not `observation.rs:93`'s.
- **Two roots, not one, reach the definition in production.** `entry.rs:155`,
  `:164`, `:173` (workspace) and `entry.rs:182` (git directory). The
  git-rooted one is observe-only (§1.3(b)), so it is inert — but it is on a
  production path, and the record's "one owner" framing does not carry that.
- **The claim is false at whole-tree scope: a second, independent production
  spelling of the legacy path exists outside the module.**
  `src/git/gitbackend/preservation_image.rs:8`:

  ```rust
  pub(super) const CHECKED_ARTIFACT_PRIVATE_PATH: &str = ".gwz/checked-artifacts";
  ```

  consumed at `preservation_image.rs:225` (seeding `raw_excluded_paths`, the
  preservation-image blindness prefix) and at
  `git/gitbackend/repository_support.rs:133`
  (`checked_artifact_private_status_path`, the git-status dirt classifier —
  `repository_support.rs:115` documents "The private area
  (`.gwz/checked-artifacts`) is permanent product infrastructure"). Neither
  derives from `catalog_names.rs`. A relocation that changes only
  `catalog_names.rs`/`policy.rs` silently desynchronises both.

---

## 3. The A1 coexistence gate now

### 3.1 `recover_or_create` still has zero production callers — verified

Definition: `catalog/bootstrap.rs:233-237`, visibility
`pub(in crate::checked_artifact)`. Every call site outside that definition is
in a `#[cfg(test)]` module; module gating verified at:

| Module holding callers | Gate |
| --- | --- |
| `catalog/bootstrap/tests.rs` | `catalog/bootstrap.rs:338-339` |
| `bootstrap/managed/tests_provider.rs` | `bootstrap/managed.rs:28-29` |
| `capability/pre_catalog/provider/{mutation_tests, directory_mutation_tests, tests_leaf_fault_matrix, tests_terminal_fault_matrix, tests_retired_root, tests_authority_record_matrix}.rs` | `provider.rs:313-336` |
| `admission/{tests, tests_fault_matrix}.rs` | `admission/mod.rs:57-62` |
| `admission/driver/tests.rs` | `admission/driver.rs:229-230` |
| `namespace/tests_fault_matrix.rs` | `namespace/mod.rs:40-41` |

**The gate is fail-closed at two independent levels**, not one. Besides
`recover_or_create` itself, the lease that feeds it is dead too —
`operation/workspace_mutator_lock.rs:43-52`:

```rust
    #[allow(
        dead_code,
        reason = "R2-C0 freezes the checked catalog borrow before the C1 owner consumes it"
    )]
    pub(crate) fn catalog_mutation_lease(
        &self,
    ) -> crate::checked_artifact::CatalogMutationLeaseV1<'_> {
```

The Phase4Closure §2.7 record's *"That gate is already fail-closed in code
rather than only in prose — `recover_or_create` has no production caller"*
holds unchanged at `ea3a924`.

### 3.2 Where the gate would lift, and what does **not** fire when it does

A production caller must live inside `crate::checked_artifact` (the visibility
at `catalog/bootstrap.rs:233`) and be handed a `CatalogMutationLeaseV1` from
`bootstrap/runtime/mod.rs:52`, exposed crate-wide at
`workspace_mutator_lock.rs:48`. The freeze that documents the gate is
`catalog.rs:10-16`:

> ```rust
> #[allow(
>     dead_code,
>     reason = "the sealed catalog owner is complete but unactivated: `recover_or_create` gains \
>               its first production caller in R2-E, behind the Phase 4.3 coexistence criterion \
>               (plan §5 item 2)"
> )]
> mod bootstrap;
> ```

**Mechanically, nothing fires when a production caller is added:**

- It is `#[allow]`, not `#[expect]`, and there is no
  `unfulfilled_lint_expectations` lint configured — a no-longer-needed allow is
  silent, and its reason string goes stale without a diagnostic.
- `interface_tests/capability_permit.rs:312` only asserts the *token*
  `"recover_or_create"` is present in `catalog.rs`; it does not count callers.
- What *would* fire: naming a caller `recover_or_create_workspace` or
  `recover_or_create_git_directory` — both are on the **forbidden** provisional
  list, in Rust (`capability_permit.rs:59-60`) and tree-wide in Python
  (`scripts/checks/check_checked_artifact_boundaries.py:757-765`, enforced
  `:1264-1273`); or a caller that publishes, moving the
  `publish_verified_no_replace(` count off 14 (`capability_permit.rs:158` plus
  the per-file dict `check_checked_artifact_boundaries.py:773-826`).

So the A1 gate today is a *prose* gate resting on a *fact* (zero callers), with
no check that fires the moment the fact changes.

---

## 4. What "the relocation package" still requires, concretely

Given §§1-3, the honest shortest work-list. Items marked **[code]** change
source; **[record]** changes documents only; **[pin]** is mechanical refresh
forced by a code item.

### 4.1 It is **not** a selection switch

There is no root selector to flip. The root already follows which filesystem
owns the artifact (§1.2), and the `GitDirectory` arm already exists and is
already exercised in production (read-only). What the records call "relocating
under `.git/`" cannot be spelled as `git_directory` instead of `workspace` —
that would move the private area of an artifact whose *own* root is the
workspace onto a different filesystem, and it is already what the git-rooted
entry point does for its own artifacts. **The relocation is a change of the
private-area's leaf name (or the introduction of a second, non-catalog
container), not a change of variant.**

### 4.2 The work-list

1. **[code] A private-area name that is not the catalog's `Final`.** Today
   `policy.rs:36`/`:39` return `CatalogPrivateNameV1::Final.relative_path(…)` —
   literally the catalog's own leaf. `CatalogPrivateNameV1`
   (`catalog_names.rs:21-26`) has no quarantine/legacy variant. Files:
   `catalog_names.rs`, `policy.rs`.

   **Hard constraint the plan must weigh before choosing where the new name
   lives:** if it is added to `CatalogPrivateNameV1::ALL`
   (`catalog_names.rs:29-34`), it enters `PrivateControlDomain::for_root`'s
   members (`collision.rs:244-249`), which changes
   `PrivateControlDomain::version_digest()` (`collision.rs:261-271`), which is
   framed into `historical_digest` (`digests.rs:136-145`) — and
   `historical_collision_digest` is **both a persisted wire field** of
   `CatalogBootstrapRecordV1`
   (`protocol/catalog_bootstrap_record.rs:45`; CBOR key 4 at
   `protocol/generated.rs:402`, `:418`, `:434`) **and an input to the on-disk
   scratch directory name** (`pre_catalog.rs:332-335`,
   `CatalogScratchNameV1::new(durable_target_digest, historical_collision_digest,
   token)`). Since `expected_for` (`pre_catalog.rs:474-489`) derives the
   expected record freshly and `retain_completed_catalog` compares it against
   the on-disk record (`completed.rs:64-73`), any change to `ALL` makes every
   previously-written catalog unrecoverable. This is presently harmless — there
   are no production catalogs (§3) — and it is a **second, independent
   mechanical reason the relocation must precede first activation**, alongside
   the interior-grammar reason the records already carry. A name added *outside*
   `ALL` has none of this coupling.

2. **[code] The second production spelling must move with it.**
   `git/gitbackend/preservation_image.rs:8` and its two consumers
   (`preservation_image.rs:225`, `repository_support.rs:133`). The plan must
   also decide the *semantics*: `repository_support.rs:111-130` exempts
   untracked residue under the legacy path from git-status dirt, and
   `preservation_image.rs:225` makes the preservation image blind to it. If
   residue is left in place (§6), both exemptions must keep covering the old
   path as well as the new.

3. **[code, conditional on OPEN-R1] A migration/adoption verb.** None exists.
   `open_private` (`residue.rs:45-104`) only ever looks at the current
   `quarantine_parent`; no code reads any other location, and `inspect_family`
   returns `FamilyResidue::empty()` when the parent is absent
   (`residue.rs:111-113`). If OPEN-R1 is answered "move", the verb is new code
   with no existing skeleton. If "leave in place", this item is zero code and
   the consequences in §6 apply instead.

4. **[code] Whether the `.git/info/exclude` generator still needs its row.**
   `workspace_ops/sync_workspace_boundary.rs:67` emits `/.gwz/` into the
   managed block (block markers `:12-13`, path built `:88-90`). A private area
   under `<git-dir>/gwz/` needs no exclude rule at all — so that line becomes
   the only thing keeping the *legacy* location invisible to git.

5. **[record] Gate lift and the dated re-ownings.** `catalog.rs:10-16`'s reason
   string; `namespace/mod.rs:8-23`'s "E4's landing, wherever R2-F puts it"
   note; `GwzM5-8R2E-Plan.md` §1.1's O1/O2 cross-lane dependency row, O3, and
   OPEN-R1; `GwzM5-8R2DPhase4Closure.md` §2.7's A1 gate sentence.

6. **[pin] The mechanical refreshes** — enumerated in §7. They are forced by
   items 1-4 and are not optional: `checked-artifact-boundary.yml` runs the
   checker per-commit, not per-PR-head.

7. **[record] Not required by the relocation but adjacent:** the R2-F evidence
   map's F-5 "interim evidence" action **G-5 has already landed** as
   `src/git/tests/g15/root_preservation/max_path.rs`, and F-4's DirentBarrier
   [P3-1] correction (a) has landed as `ready_edge_prologue`
   (`pre_catalog.rs:313-324`). Both rows in `GwzM5-8R2F-EvidenceMap.md` §2.3 are
   stale as written.

---

## 5. MAX_PATH arithmetic, measured at the git-directory root

### 5.1 The name constants, measured (not trusted)

From `authority.rs`: `family_key` and `action_key` are SHA-256 hex, 64 chars
each (`:196-223`, `hex()` at `:293-295`); the identity digest is
`hex(&[u8; 16])`, 32 chars.

| Builder | Site | Composition | Length |
| --- | --- | --- | --- |
| `authority_name` | `:226-228` | `ca1-` +64+ `-` +64+ `.authority` | **143** |
| `source_name` | `:238-240` | `ca1-` +64+ `-` +64+ `-` +32+ `.source` | **173** |
| `goal_name` | `:234-236` | same with `.goal` | **171** |
| `scratch_name` (authority) | `:271-273` | `.ca1-` +64+ `-` +64+ `-authority.scratch` | **152** |
| `scratch_name` (goal) | `:271-273` | `.ca1-` +64+ `-` +64+ `-goal.scratch` | **147** |

**The recorded 173 is confirmed**, and independently pinned in the tree:
`src/git/tests/g15/root_preservation/max_path.rs:20-29` builds the exact shape
and asserts `name.len() == 173`. Phase4Closure §2.2's whole table (143 / 173 /
171 / 152 / 147) reproduces exactly.

### 5.2 The prefix arithmetic — and the discrepancy with the record

Beyond the artifact root, including the leading separator:

| Root | Path | Chars beyond root |
| --- | --- | --- |
| Workspace | `/.gwz/checked-artifacts/<173>` | **197** |
| Git directory | `/.git/gwz/checked-artifacts/<173>` | **201** |

`.gwz/checked-artifacts/` is 23 chars; `.git/gwz/checked-artifacts/` is 27.
**Relocating the same leaf under the git directory makes the path four
characters LONGER, not shorter.**

This does not match the recorded claim. `GwzM5-8R2DPhase4Closure.md` §2.4
(`:127-131`) and §2.7 (`:202-206`) both record that quarantine/relocation
"retires the MAX_PATH exposure … the private-area relocation option under
`.git/` would retire it", citing checkpoint `:1022-1024`; the R2-F evidence map
carries the same at F-5 (`GwzM5-8R2F-EvidenceMap.md:154`). On today's constants
that does not follow from the move alone. The exposure is 173 characters of
name against a 23-character prefix: **160 of the 173 are the
64+64+32-hex triple**, so the retirement can only come from shortening the
names, not from moving them under a longer prefix.

Two further facts the plan should carry:

- The in-tree fixture comment says a different number.
  `max_path.rs:33-34`: *"Root long enough that
  `<root>/.gwz/checked-artifacts/<173>` breaches MAX_PATH (root + 198 > 260)"*.
  Measured, the suffix is 197, not 198. The test's assertion is
  `planted.as_os_str().len() > 260` (`:40-44`) against a fixture root built
  with `minimum_root_length` 120 (`:36`), so the off-by-one does not weaken the
  test — but the arithmetic in the comment is not the tree's.
- **The two subsystems do not target the same git directory.** The legacy leaf
  writer's git-rooted entry uses `repo.path()` (`files.rs:29-31`) — the
  *per-worktree* git dir, which for a linked worktree is
  `<common>/worktrees/<name>/` and therefore longer again. The catalog targets
  the *common* directory (`CatalogLeaseTargetRequestV1::repository_common_git_directory`,
  required by `capability_permit.rs:79`; `retained.rs:200-206` retains
  `repository.path()` and `repository.commondir()` separately). Any MAX_PATH
  figure the plan adopts has to say which of the two it measures.

---

## 6. OPEN-R1's factual basis today

The question, from `GwzM5-8R2E-Plan.md` §1.1: *"must the relocation move
resident legacy residue, or only relocate future writes?"*

### 6.1 Leave-in-place still keeps the catalog unobservable — the refusal path

Unchanged, and here is the exact chain at `ea3a924`.

`capability/pre_catalog/provider/interior.rs:87` — the enumeration propagates
the refusal immediately, per entry:

```rust
        let class = exact_row(&name, mode)?;
```

`interior.rs:422-446` — `exact_row` admits exactly the ten
`InfrastructureSlotV1::ALL` names (`:426-437`) and `ActiveAction` rows
(`:438-441`), and refuses everything else:

```rust
    Err(CheckedFsError::ambiguous(
        "catalog interior",
        "catalog directory contains an unowned child",
    ))
```

Consequence at `completed.rs:64-73`: `completed_record(...)` is `None`, so
`retain_completed_catalog` refuses with `ambiguous("completed catalog", "final
catalog does not have the exact retired layout")`. The same shape is recorded
in-tree as an already-diagnosed defect class:
`tests_retired_root.rs:183-186` — *"a **single** retired child made
`completed_record` `None`, so `retain_completed_catalog` refused, so recovery
refused — the catalog was unobservable and therefore unrecoverable"*.

**The Windows permanence half is also unchanged.** `platform.rs:344-352`:
`prepare_private` is `Ok(())` on every non-Windows platform, so no anchor is
established and the collision is transient there; the Windows arm
(`platform.rs:688-694`) calls `anchor::prepare`, and `platform.rs:361-368`
documents the *"permanent `.ca1-durability-anchor-<32hex>` file as product
infrastructure"*. So Phase4Closure `:180-190`'s evidence stands: on Windows a
workspace that has ever written a checked artifact permanently blocks an
observable catalog at that path, and leave-in-place does not free the
directory.

### 6.2 New facts since the Windows-permanence evidence

**(a) A relocation orphans resident residue from the legacy machinery too, not
just from the catalog.** `open_private` (`residue.rs:45-104`) reads only the
current `quarantine_parent`; there is no reader of any prior location. After a
relocation, `inspect_family` finds the new parent absent or empty and returns
`FamilyResidue::empty()` (`residue.rs:111-113`), so a resident in-flight family
in the old directory becomes unreachable, un-nameable, and un-reclaimable by
the writer that created it — the same failure mode the deterministic-scratch
change was made to end (`authority.rs:254-258` documents the nonce orphan it
replaced).

**(b) The classifier's behaviour over that orphaned state is fail-closed or
fail-safe, never silently wrong.** Reading `classification.rs:246-276` for a
drive whose residue sits in the old directory (so `authority = false, source =
Absent, goal = Absent`):

| Interrupted at | Managed leaf | Row | Outcome |
| --- | --- | --- | --- |
| after `detach_existing` on a replace | `Missing` | `(ExistingReplace, false, Absent, Absent, Missing)` — undeclared | `_ => Ambiguous` (`:275`) — typed refusal |
| after `detach_existing` on a remove | `Missing` | `(ExistingRemove, false, Absent, Absent, Missing)` (`:253`) | `After` — correct result; the quarantined source leaks in the old dir |
| after goal staging on a missing-source replace | `Missing` | `(MissingReplace, false, Absent, Absent, Missing)` (`:248`) | `Before` — re-drives cleanly; the staged goal leaks |

So the honest statement for OPEN-R1: **leave-in-place is not a correctness
hazard in the classifier — it is a leak plus a permanent catalog blockage.**
The blockage is the binding half.

**(c) The residue's git-visibility exemptions are keyed to the old path.**
`repository_support.rs:111-135` and `preservation_image.rs:225` exempt
`.gwz/checked-artifacts` specifically. Left-in-place residue stays exempt only
while those constants still name the old path — which is item 2 of §4.2. Two
in-tree fixtures already depend on residue being tolerated exactly there:
`src/git/tests/g12.rs:826-840` (plants a `.ca1-durability-anchor-…`, asserts
rollback preserves it) and
`src/git/tests/g15/root_preservation/stash.rs:306-313` (same shape, asserts
exact stash evidence unchanged).

**(d) Nothing in the tree has changed the recorded direction.** No adoption
verb, no dual-location reader, no migration record type exists. The question is
still open exactly as routed.

---

## 7. Blast and coupling — the pins a relocation landing would move

### 7.1 The one boundary checker

`scripts/checks/check_checked_artifact_boundaries.py` (docstring `:2`:
*"Fail-closed structural inventory for the checked-artifact boundary."*). It
enumerates three ways.

**By byte digest of a single file** — `PROTECTED_SOURCE_DIGESTS` (`:40-58`, 17
entries at `:41-57`; applied `:983-987`). Of the files at the centre of this
trace:

| File | Pinned? | Evidence |
| --- | --- | --- |
| `checked_artifact/entry.rs` | **YES**, twice | `:44` (digest) and `:27` (`PROTECTED_COMPILER_MODULES`, must carry `#![forbid(clippy::disallowed_methods)]`, enforced `:1090-1095`); also structurally frozen by `ENTRY_REFERENCES` `:296-335`, `ENTRY_ITEMS` `:337-366`, `ENTRY_USES` `:368-372`, `ENTRY_CALLS` `:374-413`, enforced `:1174-1243` |
| `checked_artifact/residue.rs` | **YES** | `:47` |
| `checked_artifact/mod.rs` | **YES** | `:46` — holds the `mod catalog_names;` / `mod policy;` / `mod observation;` declarations (`mod.rs:23`, `:56`, `:54`) |
| `git/gitbackend/preservation_image.rs` | **YES** | `:52` — the file holding `CHECKED_ARTIFACT_PRIVATE_PATH` |
| `git/gitbackend/preservation_root/files.rs` | **YES** | `:51` — the file holding `observe_boundary`, the sole production consumer of the `GitDirectory` policy arm (§1.3(a)) |
| `checked_artifact/policy.rs` | **NO** | absent from the dict; no pin anywhere outside the module |
| `checked_artifact/catalog_names.rs` | **NO** | absent; and it is not under `catalog/`, so `catalog.rs`'s tree digest does not reach it |
| `checked_artifact/observation.rs` | **NO** | absent |

So the two files a minimal relocation would edit first — `catalog_names.rs` and
`policy.rs` — trip **no** digest on their own. The moment the change reaches
`residue.rs`, `entry.rs`, `mod.rs`, `preservation_image.rs` or
`preservation_root/files.rs`, a digest moves per file touched.

**By digest of a module subtree** — `PROTECTED_SOURCE_TREE_DIGESTS` (`:185-193`,
seven entries; applied `:988-990`; `source_tree_digest` at `:912-928` hashes the
root file plus every file under the sibling directory). Covers
`checked_artifact/{bootstrap/runtime/catalog_lease.rs, capability/path.rs,
capability/pre_catalog.rs, catalog.rs, platform.rs}` and two `v1_lifecycle`
trees. Any edit under `capability/pre_catalog/**` (which is where
`filesystem.rs`, `namespace.rs`, `mutation.rs`, `interior.rs`, `completed.rs`,
`collision`-adjacent code live) or under `catalog/**` moves one.

**By directory walk / set equality (no digest — fails closed on new or moved
files):** `APPROVED_RUST_PATH_EDGES` (`:71-151`, walk `:1100-1133`, set-equality
`:1121-1126`) freezes every `#[path = …]` edge, including the four legacy leaf
test files at `:93-96`; `CATALOG_LEASE_REFERENCE_SETS` (`:596-755`, checked
`:1253-1263`) pins 25 symbols to exact file sets — including
`"create_git_private_parent"` at `:630-634`;
`CATALOG_PUBLICATION_CALL_COUNTS` (`:773-826`, checked `:1023-1037`);
`RAW_RENAME_CALL_ALLOWLIST` (`:222-233`, checked `:1038-1054`);
`FORBIDDEN_PROVISIONAL_CATALOG_INTERFACES` (`:757-765`, checked `:1264-1273`).

The checker's own suite
(`scripts/checks/test_check_checked_artifact_boundaries.py:77`,
`test_current_source_inventory_is_classified`) runs it against the real `src/`
and asserts green, so every digest must be refreshed **in the landing commit**.

### 7.2 Test-count batteries

`scripts/checks/run_r4bg_aggregate_gates.py`:

- `:300-301` — `("checked-artifact fault census (165 keys)",
  lib("checked_artifact::"), _fault_count("447 passed", "457 passed"))`. Any
  test added, removed or renamed under `checked_artifact::` moves both numbers,
  and `_fault_count` (`:67-275`) requires a dated per-OS provenance comment
  (darwin measured / linux DERIVED + FIRST-DISPATCH-EXPECTED) with each move.
- `:302-305` — the lib-remainder partition, `_fault_count("1097 passed", "1098
  passed")`. The `.gwz/checked-artifacts` fixtures in `src/git/tests/` fall in
  **this** partition, so touching them moves a different pair.

### 7.3 Fixtures that assert on the literal path

Complete set outside the module (six files):

- `src/git/tests/g12.rs:826-840` — creates `.gwz/checked-artifacts`, plants a
  durability anchor, asserts rollback preserves it.
- `src/git/tests/g15.rs:461-501` — commits `".gwz/checked-artifacts/protocol"`
  (`:466`, `:482`, `:493`) with the control sibling `".gwz/sibling"`
  (`:474`, `:497`); the test is only meaningful because the private tree is the
  *only* exclusion.
- `src/git/tests/g15/root_preservation/stash.rs:306-313`.
- `src/git/tests/g15/root_preservation/max_path.rs:7`, `:33`, `:37` — the whole
  arithmetic is calibrated to the `.gwz/checked-artifacts` prefix length and a
  fixture root depth of 120 (`:36`); a relocation forces recalculation.
- `src/git/tests/g15/root_preservation/support.rs:94` — comment only.
- The `gwz/checked-artifacts` (GitDirectory) spelling appears **only** inside
  the module, at `src/checked_artifact/tests/recovery_protocol.rs:100`, `:139`,
  `:166`.

### 7.4 Ignore-rule generators

- `src/workspace_ops/sync_workspace_boundary.rs:67` — `format!("/{RUNTIME_DIR}/")`
  emits `/.gwz/` into the managed block of `<root>/.git/info/exclude` (markers
  `:12-13`, path `:88-90`). See §4.2 item 4.
- `scripts/retained_readers/generate_retained_reader_fixtures.py:24-27`, written
  at `:139` — a checked-in exclude template containing `.gwz/` and `/.gwz/`.
- The repo's own `.gitignore` does **not** mention `.gwz`; gwz writes no
  `.gitignore` (asserted `src/workspace_ops/tests/g00.rs:79-83`).

### 7.5 CI

- `.github/workflows/checked-artifact-boundary.yml:32-34` — checker + suites on
  every PR and push to main; `:47-48` runs the aggregate gates.
- `.github/workflows/checked-artifact-boundary.yml:25` →
  `scripts/checks/check_lane_commits.sh:31-42` — runs the checker against
  **each commit's own tree** (`git archive` per sha, floor at `:23`).
  **A relocation split across commits goes red at the intermediate commit even
  if the head is green.**
- `.github/workflows/release.yml:85-86` and `:210-232` — the same checker and
  the 447/457 + 1097/1098 count pins run in the release lane (Linux only).
- `scripts/release.py:349-369` / `:372-383` / `:385-400` — non-skippable gates
  that additionally fail if running them dirties the tree, so digests must
  already be correct at tag time.

### 7.6 Not pins

`protocol/checked-artifact-semantic-v1/vectors.txt`,
`protocol/checked_artifact-corpus/golden.json` and
`protocol/checked_artifact.taut.py` were checked: none contains `.gwz`, `gwz/`
or `checked-artifacts`. The relocation is not a semantic-vector event. The
`historical_collision_digest` coupling in §4.2 item 1 is a *durable-record*
event and is separate from these.

*[FALSIFIED at R1.1 (2026-09-01, R2-F R1.3): the relocation IS a
semantic-vector event. Three `catalog_bootstrap` vectors encode `final_name`
(CBOR key 8) and its dependent `record_id` (key 11) — as CBOR text INSIDE the
hex column, which the plain-text grep above cannot see; that is the method
error. `catalog_bootstrap_record.rs:242` refuses any record whose
`final_name` ≠ `Final`, so the byte change forced the vector move. The
re-derivation was reviewed and ruled LEGITIMATE (R1.1 review §4, FLAG-1:
round-trip validated independently, header clauses all still true) and is
recorded in `vectors.txt`'s own header. This section's conclusion is
retired; the mechanism lesson — verify by decoding, not by grepping the
encoding — joins the package's three-strikes record.]*

---

## What the plan must decide

Facts only above. These are the decisions the trace leaves open, each with the
evidence that forces it:

1. **Where the relocated private area lives, and whether its name joins
   `CatalogPrivateNameV1::ALL`.** Inside `ALL` it changes
   `PrivateControlDomain::version_digest()` → `historical_collision_digest` →
   both a persisted CBOR field and the on-disk scratch directory name (§4.2
   item 1). Outside `ALL` it has none of that coupling but also gains none of
   the collision-domain protection.
2. **OPEN-R1: move resident residue, or relocate future writes only.** The
   classifier is fail-closed/fail-safe either way (§6.2(b)), so the deciding
   facts are the permanent Windows catalog blockage (§6.1) and the
   unreclaimable orphan (§6.2(a)) — not correctness of the next drive.
3. **What the MAX_PATH claim actually means, since moving the same leaf under
   `.git/` costs +4 characters rather than retiring the exposure** (§5.2). If
   retirement is still a goal of this package, the 160 hex characters in the
   name are where it has to come from — a different decision from relocation,
   with its own compatibility surface (`authority.rs:226-273`,
   `inspect_family`'s `ca1-{family}-` prefix filter at `residue.rs:127`).
   Whichever is chosen, the plan must also say whether it measures the
   per-worktree git dir (`repo.path()`, what the legacy writer uses) or the
   common one (what the catalog uses).
4. **Whether the second production spelling is unified or kept parallel** —
   `preservation_image.rs:8` and its git-status/preservation-image exemptions
   (§4.2 item 2). If residue stays, the exemptions must cover both paths.
5. **Whether the A1 gate gains a check that fires**, given that it is presently
   an `#[allow]` with no `#[expect]` and no caller count (§3.2).
6. **Commit shape**, because `check_lane_commits.sh` gates every commit
   individually (§7.5): the relocation, one digest refresh per protected file
   touched, the two count-pin refreshes with dated provenance, and the fixture
   recalculations cannot be split across a series that is red in the middle.
7. **Whether `GwzM5-8R2F-EvidenceMap.md` §2.3 is re-owned first**, since F-4
   and F-5's interim actions have both landed since it was drafted (§4.2
   item 7).
