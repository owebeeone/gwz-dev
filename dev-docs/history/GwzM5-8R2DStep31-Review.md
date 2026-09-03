# R2-D Phase 3 Step 3.1 — the managed-parent bootstrap consumer: single-axis State review

Date: 2026-08-22. **Single-axis State-lead** (`GwzM5-8R2DInterfaceFreeze.md` §9:
"Everything else is **single-axis with automatic escalation on any P0/P1/P2**:
… Phase 3 steps 3.1-3.3 **including the Phase 3 settle**"). Round 1 of a
two-round cap. Peer-blind: the implementer's report was read as *claims to
verify*, never as evidence. **Escalation is TRIGGERED** — see §11; routing is
the lane owner's.

Object: the uncommitted Step 3.1 edits over gwz-core base `8b83a2c` ("Land D3:
durable reverse-preservation cursor"), exactly eight files:

| File | Disposition |
| --- | --- |
| `checked_artifact/bootstrap/managed/provider.rs` | **NEW** |
| `checked_artifact/bootstrap/managed/tests_provider.rs` | **NEW** |
| `checked_artifact/bootstrap/managed.rs` | modified |
| `checked_artifact/bootstrap/managed/plan.rs` | modified |
| `checked_artifact/capability/pre_catalog.rs` | modified |
| `checked_artifact/capability/pre_catalog/provider.rs` | modified |
| `checked_artifact/capability/pre_catalog/provider/managed_mutation.rs` | modified |
| `checked_artifact/catalog/bootstrap.rs` | modified |

Baseline: gwz-core `8b83a2c`; gwz-dev repo HEAD `a442971`.

**Concurrent-lane caution, honoured and re-verified.** Other lanes' uncommitted
files coexist in this tree and are **attributed, not judged**. The brief named
six (`platform.rs`, `namespace_mutation.rs`, `leaf_observation.rs`,
`cleanup.rs`, `residue.rs`, `transition.rs`); during this review the foreign set
**grew to ten** — `provider/authority_record_binding.rs`,
`provider/tests_authority_record.rs`, `git/tests/g15/root_preservation/support.rs`
and `workspace_ops/merge/v1_lifecycle/tests/fixtures.rs` appeared after 21:36,
and `platform.rs` was rewritten at 21:38:34, mid-review. Attribution is
unaffected because my gate tree is built **from `git archive HEAD`** and only
the eight object files are overlaid — every foreign file is at HEAD by
construction, not by enumeration. The last object write was 21:30:59; every
gate below ran after 21:49 on a tree re-verified byte-identical to
`HEAD + the 8` immediately before the run.

---

## 1. Object inventory, as counted

Independent count, non-blank and non-comment (`//`, `///`, `//!` excluded):

| File | Raw | Code |
| --- | --- | --- |
| `bootstrap/managed/provider.rs` | NEW 411 | **278** |
| `capability/pre_catalog/provider/managed_mutation.rs` | +345/−20 | **+220** |
| `capability/pre_catalog.rs` | +49/−12 | **+27** |
| `catalog/bootstrap.rs` | +34/−2 | **+23** |
| `bootstrap/managed.rs` | +14/−0 | **+12** |
| `capability/pre_catalog/provider.rs` | +12/−2 | **+5** |
| `bootstrap/managed/plan.rs` | +8/−0 | **+3** |
| **Production subtotal** | | **568** |
| `bootstrap/managed/tests_provider.rs` | NEW 533 | 412 |
| **Total** | | **980** |

The lane's self-reported 568 / 412 / 980 is **confirmed exactly**.

---

## 2. Gate train — executed by this review, on an attributable tree

**Bisection method.** `git archive HEAD | tar -x` into
`…/scratchpad/p31-scratch/gwz-core`, workspace `Cargo.toml`/`Cargo.lock` copied,
`gwz-cli` and `taut-shape-rs` symlinked, then the eight object files overlaid
from the live tree. `diff -rq` against the live `src` was run **before and
after** the gate train and returned exactly the foreign set and nothing else —
i.e. the scratch is provably `HEAD + the 8`. All cargo runs used the mandated
`CARGO_TARGET_DIR=…/scratchpad/p31-target`.

| Gate | Result |
| --- | --- |
| `check_checked_artifact_boundaries.py` on **pristine HEAD** | `ok (15 visible entries, 5 classified modules)` — the control |
| `check_checked_artifact_boundaries.py` on the object | **exactly two findings**: `protected source tree changed: checked_artifact/capability/pre_catalog.rs`; `protected source tree changed: checked_artifact/catalog.rs` |
| `cargo check --all-targets` | clean, zero warnings |
| `cargo test --lib checked_artifact::` | **341 passed; 0 failed** (330 → 341 = +11) |
| `cargo test --lib …managed::tests_provider` | **11 passed; 0 failed** |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings` | clean |
| `rustfmt --edition 2024 --check` on the 8 files | clean |

**The two checker findings are land-time digest pins, not findings.** Both are
`protected source tree changed`, the mechanism by which the lane owner must
deliberately re-pin a protected tree at land. The pristine-HEAD control proves
neither pre-exists. `capability/pre_catalog.rs` follows from the pre-catalog
edits; `catalog.rs` from `catalog/bootstrap.rs`. Correctly **not** refreshed by
the implementation lane. **Matches the brief's expected set exactly**, so the
gates above are trustworthy.

**Companion pins, verified independently of the lane's claim.**
`grep -rc 'publish_verified_no_replace('` over `src/`: **21 at HEAD, 21 on the
object**, and the per-file diff shows the only new entries are the two new files
at **zero**. The P2-only claim for `stage_component` is therefore true by count,
not by assertion: `CATALOG_PUBLICATION_CALL_COUNTS` is untouched and
`interface_tests/capability_permit.rs:139`'s pin stays at **13** (green inside
the 341).

**Byte-untouched, confirmed by `git diff`/`git status` returning empty:**
`interface_tests/fault_expected_keys.rs`, `fault_v1.rs`,
`interface_tests/capability_permit.rs`. ✅

**Lint-suppression note (method, not a finding).** I probed the dead-code lint
with a deliberately unused private function and it did **not** fire: `mod
capability` carries a blanket `#[allow(dead_code, reason = "R1 freezes
interfaces before R2 consumer conversion")]` at `checked_artifact/mod.rs:20-23`.
So a clean `cargo check`/clippy on this subtree is **not** evidence that every
item has a caller. That matters for §5 and is why §5 rests on `grep`, not on the
compiler.

---

## 3. Plan conformance — the deciding document, read directly

`GwzM5-8R2D-Plan.md` §4 Phase 3, Step 3.1, quoted in full:

> **Step 3.1 — real provider** (<500 LOC): `observe_preflight` /
> `revalidate_plan` / `execute_bound` on the retained provider; per missing
> component: staged directory, ownership marker, installed observation, durable
> successor, prior-generation retirement, marker retirement, final reproof;
> restart consumes the resident intent and scheduled slots, never replans a
> partially completed live path. Depends on 2.2/2.3 shapes.

### 3.1 All three trait methods — delivered

`impl ManagedParentBootstrap for RetainedManagedParentProviderV1` states
`provider_instance_id`, `observe_preflight`, `revalidate_plan`, `execute_bound`
(`provider.rs:287-349`). Freeze §3.4's signatures are **implemented, not
reshaped** — I compared the frozen block character-for-character against
`bootstrap/managed/owner.rs:16-32` on the object tree: unchanged. Freeze §3.2
(the `RawNamespaceBackend` delta) is untouched — `namespace/backend.rs` is not
in the object and not in the working set at all. ✅

### 3.2 Zero key conversion here — the lane's claim is CORRECT

The lane claims the plan assigns `managed_bootstrap.*` **activation** to Step
3.2, so Step 3.1 owes no key flip and no §3.5 edit. I read the plan myself:

- Step 3.2 is titled "**`managed_bootstrap.*` activation + purpose policy
  matrix** (<500 **test** LOC)". The plan splits activation into its own step,
  with a test-only budget.
- The plan's own end-of-§4 activation map says `managed_bootstrap.*` → **Phase
  3**, not Step 3.1.

So the plan does **not** require 3.1 to flip keys, and it does not require a
§3.5 activation-status edit. **No finding on that axis.** The related recording
duty is a different matter — see §4 and [P2-1].

### 3.3 The item list — 5 of 7, and the shortfall has no home

| Plan §4 Step 3.1 item | Verified |
| --- | --- |
| `observe_preflight` | ✅ `provider.rs:294`, over the bounded prefix walk |
| `revalidate_plan` | ✅ `provider.rs:320` |
| `execute_bound` | ✅ `provider.rs:337` |
| staged directory | ✅ `RetainedManagedParentV1::stage_component` |
| ownership marker | ✅ `write_or_rewrite_marker`, P2 write-through + flush |
| installed observation | ✅ `recover_installed_bootstrap_component` on every drive |
| **durable successor** | ❌ **absent** |
| **prior-generation retirement** | ❌ **absent** |
| marker retirement | ✅ `retire_bootstrap_marker` / `recover_retired_bootstrap_marker` (E16) |
| final reproof | ✅ `observe_spec` + `depth != components.len()` guard |
| restart consumes the **resident intent** | ⚠️ re-derived, not resident — bounded (§6) |

The successor chain exists **in memory** (`successor_after_component`,
`successor_after_marker_retirement`, `transition.rs:8/:52`, correctly driven);
what is missing is its **durable** half — freeze §4.3 row **E17** ("managed
successor publish, prior-generation retirement, final reproof") and the intent
record's own publish/retire lifecycle. That is 2 of the 7 named per-component
items, and it is also the precondition for the clause immediately following
them. See **[P1-1]**.

---

## 4. The five edges converted without sites — adjudication

`stage_component` (`managed_mutation.rs:436-497`) plus `write_or_rewrite_marker`
(`:866-908`) cross exactly five reserved `managed_bootstrap.*` boundaries. I
mapped each to its physical statement and each to its key in
`interface_tests/fault_expected_keys.rs:121-150`:

| Key (reserved) | Physical boundary now crossed |
| --- | --- |
| `staging_directory_create` | `self.handle.create_dir(&name)` |
| `ownership_marker_create` | `staged.open_with(&name, durable_write_options(true))` |
| `ownership_marker_write` | `file.write_all(&marker.encode_canonical())` |
| `ownership_marker_flush` | `file.sync_all()` |
| `staging_directory_flush` | the two `sync_directory_edge` calls (staged interior; parent on create) |

All five are in the **22 reserved**, none in the Step-2.3 executed 8. None gains
an injection site here.

**Ruling: the deferral of the sites and matrix rows to Step 3.2 STANDS.** Three
reasons, in order of weight:

1. The plan — the deciding document — assigns activation to Step 3.2 by name and
   gives it a test-LOC budget. A reviewer may not re-legislate that from the
   RemPlan §10 text.
2. RemPlan §10's duty is "the same **package** that converts its edges". Both
   the converting step (3.1) and the activating step (3.2) sit inside
   `managed_bootstrap.*`'s own owning package, **Phase 3**, whose gate is the
   phase settle. The Step-2.3 annotation had to move keys early precisely
   because 2.3 sits in a *different* phase; that condition does not hold here.
3. Nothing unactivated is reachable: `mod provider` carries
   `#[allow(dead_code, reason = "Step 3.1 lands the provider; plan §4 Step 3.3
   wires its production consumer")]`, and R2-D forbids production catalog
   activation outright (plan §5 item 2). No production path crosses these five
   boundaries in the interim.

**The freeze §3.5 record is NOT false-in-fact.** I read the annotation
literally: it states the 22 reserved keys "are Phase 3's writer, intent-record
and successor halves" and "are proved siteless **key by key**". Both remain
true — sitelessness is asserted, edge-unconvertedness is not. So no correction
to a landed statement is owed.

**What IS owed, and is missing: the record itself.** See **[P2-1]**.

**Exactly what Step 3.2 owes** (stated here so the settle can check it off):

1. Five injection sites in `capability/pre_catalog/provider/managed_mutation.rs`
   at the five statements tabled above. No new fault source is needed —
   `managed_mutation.rs` is already declared in `FAULT_INJECTION_SOURCES`
   (`fault_expected_keys.rs:408`), so the completeness anchor keeps covering it.
2. Interruption/restart/convergence matrix rows for all five, **on both target
   variants**. Note the constraint in [P3-3]: the Git-directory arm cannot run
   through this provider and must use the Step-2.3 door.
3. The §3.5 `PartiallyExecuted` list edit: **8 → 13 executed, 22 → 17 reserved,
   count held at 165, no key minted, no publication companion** (this step adds
   zero publish sites — §2).

---

## 5. The landed-surface change — `retain_managed_parent` dropped from both hops

**The structural argument is SOUND, and stronger than the lane states.** I
audited every `Dir`-typed surface leaving the pre-catalog provider owner:

- `RetainedPlatformRoot` is `pub(in …::capability::pre_catalog)`; its `root()`
  and `RetainedDirectory::handle()` (`provider/retained.rs:21/:139`) are
  `pub(super)` — provider-module only.
- `RetainedActionNamespaceV1::handle()` (`namespace_mutation.rs:193`) is
  `pub(super)`.
- The only `Dir` reachable at `pub(in crate::checked_artifact)` is via the alias
  `RetainedActionDirectoryV1 = RetainedDirectory<Dir, …>`
  (`authority_record_binding.rs:98`), whose `handle()` **is**
  `pub(in crate::checked_artifact)` (`namespace/backend.rs:39`). But no
  production route **constructs** one outside the owner:
  `BackendIssuer::retained_directory` is `pub(super)` to `namespace`, and
  `namespace/test_support.rs`'s public variant is test-only.

So today no consumer outside the owner can obtain a `&Dir`, hence the re-export
was structurally uncallable. And forward-looking the removal is *load-bearing*,
not cosmetic: when Step 3.3 wires `authority_record_binding`'s consumer, that
consumer **will** hold a `&RetainedActionDirectoryV1` outside this owner and
`.handle()` **will** hand it a `&Dir` — at which point the removed re-export is
the only thing standing between it and an out-of-owner `retain_managed_parent`
call. Removing it strengthens the boundary precisely because "no `Dir` leaves
the owner" is a weaker property than the type surface suggests.

**Freeze §3.2 signatures are unreshaped** — `retain_managed_parent` is not a
frozen signature; it appears nowhere in `GwzM5-8R2DInterfaceFreeze.md` (grep:
zero hits). It is a Step-2.3 *landed surface* item, named in
`GwzM5-8R2DStep23-Review.md:290-298`.

**Ruling on changing a landed surface inside 3.1 without a freeze edit:
PERMITTED.** The item is not frozen, and the change is not silent — it trips the
`protected source tree changed: checked_artifact/capability/pre_catalog.rs`
digest pin, which is exactly the sanctioned mechanism forcing a lane-owner
decision at land. Plan §4's "each step converts-or-pins the files it takes
ownership of" is satisfied.

**But the stated justification is factually wrong** — see **[P3-1]**.

---

## 6. Restart / convergence state machine

`classify_resume` (`provider.rs:132-153`) counts resident
`ActionSlotV1::RetiredBootstrapMarker(ordinal)` rows across the row's
`component_range`:

- `retired == 0` → `FromFirstGeneration` (re-derive)
- `retired == len` → `Settled` (short-circuit to final reproof)
- otherwise → typed refusal, `CheckedFsError::ambiguous("managed-parent
  provider", "…partially retired and needs its resident intent to resume")`

**Fail-closed: VERIFIED STRUCTURALLY, not just by test.** In `execute_row` the
only statements preceding `classify_resume` are `ManagedParentBootstrapIntentV1::try_initial`
(pure), `retain_action_namespace` (I read `namespace/host.rs:91-105` — it
retains handles from the opaque catalog and constructs a backend; no durable
edge), and pure accessors. The first writer is `parent.stage_component(...)` at
`provider.rs:231`, strictly inside the loop, which the refusal never reaches.
Nothing is mutated before classification. ✅

**The `Settled` short-circuit cannot skip a row with missing install evidence.**
`resume` is computed once and is loop-invariant, so `Settled` runs zero loop
iterations and falls to `observe_spec(components)` with
`if depth != components.len() { return Err(…"not fully resident after
execution") }` (`provider.rs:272-277`). Every declared component must be a
durably-present, no-follow-openable directory or the row refuses. Soundness of
the classification itself also holds: a retirement row is only resident if that
component's marker retirement completed, which requires it to have been
installed — so `retired == len` ⟹ all installed. ✅

**Depth arithmetic is correct in both phases** — the non-obvious part, checked
against `matches_component_parent` (`managed_bootstrap_record.rs:373-396`) and
`bind_current_component` (`namespace/managed.rs:282-330`):

- `InstallComponents`, cursor *c*: `components[c].installed_path` is `None`, so
  the matcher demands `parent == intent.retained_parent`. The intent's retained
  parent advances by exactly one component per install
  (`transition.rs:38-49` passes `evidence.installed_path()` forward). The
  provider retains at `base_depth + cursor` — exact match.
- `RetireMarkers`, cursor *c*: `installed_path` is `Some`, so the matcher demands
  `installed_path == parent_path + 1`. `installed_path` for component *c* is the
  profile of `components[..base_depth+c+1]`, so the parent must be at depth
  `base_depth + c` — the same expression. ✅

**Convergence windows the writer itself can leave** are genuinely closed:
`installed_resident` gates both `stage_component` and the install edge, so a
crash after install/before retirement replays through
`recover_installed_bootstrap_component` (the marker is still resident);
`write_or_rewrite_marker` re-derives identical bytes and re-proves the interior.

**Test coverage of the three arms — all three are genuinely driven:**

| Arm | Test |
| --- | --- |
| none resident → re-derive | `execute_bound_bootstraps_every_declared_managed_parent`; `a_staging_row_left_without_its_marker_converges`; `a_staging_row_left_with_a_partial_marker_converges`; `a_staging_row_carrying_a_foreign_child_is_refused` |
| all resident → settled | `a_settled_row_re_executes_to_the_identical_proof_without_touching_the_namespace` (asserts the action directory gains **no** row and the proof is identical) |
| some resident → refusal | `a_partially_retired_row_is_refused_rather_than_replanned` (removes retirement row 2 of the two-component bundles row; asserts refusal **and** that `.gwz/stash/bundles` is unmutated) |

Plus `preflight_plans_only_the_missing_suffix…`,
`the_retained_parents_proof_reports_each_purpose_at_its_full_declared_depth`,
`revalidate_plan_refuses_a_substituted_retained_parent`,
`a_git_directory_target_refuses_the_workspace_rooted_managed_paths`,
`the_provider_instance_binds_one_target_and_separates_two`. Every row runs a
**fresh session** over the durable state the previous one left — a real restart,
not a simulated one. 11/11 green, independently re-run.

**Verified-and-benign, recorded not filed:** `execute_bound` is not atomic
*across rows* — a refusal in row *k* leaves rows `0..k` bootstrapped. No
authority claims cross-row atomicity (freeze §4.3's atomicity record is about
one rename), each row converges independently, and the E16 record's boundary
covers the rest. Not a finding.

**The one window that does not close** is the reason for [P1-1]: with `0 <
retired < len`, no future drive can ever converge, because the derivation is
deterministic and every retry re-enters the same refusal. Reachable for any row
with ≥2 missing components — e.g. `PreservationBundles` (`.gwz/stash/bundles`)
whenever only `.gwz` exists, which is the ordinary first-merge state and is
exactly what the test fixture reproduces.

---

## 7. Depth-profile composition — claim VERIFIED, and it cannot widen authority

The lane claims the parent's one-component profile is insufficient below the
first component and that `.gwz/stash/bundles` "would refuse its second
component". **True, and I traced the exact refusal:**
`matches_component_parent` compares the intent's `retained_parent_path`
(a `DurablePathV1`) against the retained parent's `path_profile()` by full
component vector. After installing `stash`, the intent's retained parent path is
the **2-component** profile `[.gwz, stash]` (set from `installed_path`, which
`matches_installed_component` pins at `retained_parent_path.len() + 1`). A
one-hop `retain_managed_parent(enclosing = .gwz, leaf = "stash")` yields the
**1-component** profile `[stash]` — mismatch, refusal at
`bind_current_component`. The shared `retain_managed_child(enclosing, prefix,
leaf, reservation)` closes it.

**It cannot widen authority.** In `retain_managed_prefix`
(`managed_mutation.rs:299-325`): `require_bounded_prefix` enforces
`1 ≤ len ≤ MAX_MANAGED_PARENT_COMPONENTS`; `depth == 0 || depth >
components.len()` is a typed refusal; the walk starts from `clone_root(root)`
(the permit-retained root, already revalidated by `CompletedCatalogPermitV1::revalidate`)
and takes one `open_dir_nofollow` hop per component of `components[..depth-1]`;
each pushed `CanonicalComponent` is bound by `bind_child_component` to **its
own** enclosure's freshly observed `dir_identity`, `parent_mode` and
`rename_domain`. The resulting profile is therefore exactly
`components[..depth]`, every element bound to its real parent — a strict prefix
of the declared path, never a superset. `retain_managed_parent`'s own signature
and one-component behaviour are unchanged (it now delegates with an empty
prefix; the only observable difference is that an invalid ASCII leaf is rejected
*before* the open instead of after, which is strictly more fail-closed).

---

## 8. The preflight walk — the Step-2.3 caution is DISCHARGED

`GwzM5-8R2DStep23-Review.md:282-286` cautioned: "that bound is exact for a
*bootstrapping* component; a component that has since been populated by writers
would exceed it, and Phase 3.1's `observe_preflight` must keep such components
out of plans (plan only for a missing suffix)."

`observe_managed_prefix` (`managed_mutation.rs:262-297`) stops at the **first
absent** component (`ErrorKind::NotFound → break`) and returns the facts of
every present depth. `preflight_inputs` then plans
`components[retained_existing_parent_count..]` only, and `continue`s the row
entirely when the whole path is resident. A populated component is therefore
always part of the *retained prefix*, never a planned row, and its interior is
never subjected to `MAX_MANAGED_COMPONENT_ENTRIES`. ✅ **Discharged.**

**Primitives:** P3 + P4 only. `symlink_metadata` + `open_dir_nofollow` per hop,
`dir_identity` proved before the next hop, `parent_mode`, `rename_domain`. No
`create_*`, no rename, no publication, no flush anywhere in the walk. ✅

**Cannot observe outside the declared prefix:** components originate only from
`ManagedParentPurpose::path()` — four frozen `&'static [&'static [u8]]`
constants — routed through `ManagedParentSpec::for_purpose`; there is no
caller-supplied path anywhere on the request constructors
(`for_merge_start`, `try_for_durable_merge`, `for_archive`).
`AsciiComponent::parse` (`capability/path.rs:16-31`) rejects empty, `.`, `..`,
`/`, `\`, NUL, non-ASCII and >255 bytes. Non-directory or symlinked components
are a **typed refusal**, not a silent stop
("managed parent prefix component is not a canonical directory"). The walk is
rooted at the permit-retained root behind `self.revalidate()?`, so it cannot
read through a substituted root. ✅

---

## 9. The deterministic ownership token — explicit verdict, as requested

`ownership_token(plan)` = SHA-256 over a domain-separated prefix ‖ plan digest ‖
action digest ‖ reservation digest ‖ schedule digest ‖ request-owner binding
(`provider.rs:394-403`).

**VERDICT: CORRECT AS BUILT. Keep it.** Reasoning, on the three axes asked:

1. **Determinism is forced, not chosen.** The token seeds `intent_id`
   (`intent_id = Sha256(digest_material())`, `managed_bootstrap_record.rs:315`),
   `intent_id` seeds the `OwnershipMarkerV1` each staged component carries, and
   a restart re-derives the chain **before** comparing markers on disk
   (`matches_installed_component` requires `evidence.intent_id() ==
   self.intent_id`). A random token makes the second drive of an interrupted
   bootstrap fail to recognise its **own** staged marker — an unrecoverable
   wedge — and allocating one is the nondeterminism the RemPlan-4 §4 R2 stop
   clause forbids outright. The alternative is not merely worse; it is
   prohibited.
2. **No collision or aliasing across rows or generations.** The token is
   plan-scoped (one per `BoundManagedParentPlanV1`), but it is never used as an
   identity — `intent_id` is, and `digest_material()` additionally covers
   `purpose`, `spec_digest`, `bootstrap_ordinal`, `generation_ordinal`,
   `component_start`, the component vector, `phase` and `cursor`. Distinct rows,
   distinct generations and distinct cursors therefore carry distinct intent
   ids from a shared token. Cross-action reuse is impossible: `action_digest`,
   `reservation_digest` and `schedule_digest` are all inputs. The nonzero
   requirement (`try_initial_fields`) is satisfied with overwhelming
   probability, and a zero digest is a typed refusal rather than a silent
   acceptance.
3. **No replay/adoption hazard the typed refusals do not already close.** The
   token carries no secret and is reproducible by anyone who can read the
   durable records — but the marker it seeds is **intra-action evidence, not a
   capability**. Forging one requires write access inside the permit-retained
   root, i.e. the same-user boundary the E16 record already declares outside
   scope, and the best an attacker achieves is to pre-plant exactly the state
   the drive was about to create (a staging directory containing only the
   correct marker), which `stage_component`'s interior re-proof then accepts as
   its own — a no-op, not an escalation. Anything else is refused:
   `a_staging_row_carrying_a_foreign_child_is_refused` pins the extra-child
   case, and a foreign directory at `final_leaf` fails
   `recover_installed_bootstrap_component` for want of the exact marker.

**One boundary condition to carry forward, recorded not filed.** The token's
guarantee is *self-consistency* ("this staged marker belongs to this admitted
action's intent chain"), **not exclusion**. If any later step — E17's read-back,
or an R2-E consumer adopting a pre-existing managed parent — ever uses the
marker to decide adoption of a directory this action did not create, determinism
becomes load-bearing for exclusion and the choice must be re-litigated then. The
follow-up as named (read the token back from the resident record once the intent
lifecycle lands) is the right shape and preserves this property.

---

## 10. Follow-ups — deferrability confirmed, with one correction

| # | Follow-up | Deferrable today? | Correctly named? |
| --- | --- | --- | --- |
| 1 | E17 durable intent-record lifecycle | **Nothing *unsound* ships** — no production caller, no catalog activation. But it is a **plan item**, not a follow-up: see [P1-1] | Names the right authority (freeze §4.3 E17 + §4.4 Class 1 arms). I verified §4.4's arm table: "managed source-interior \| E15, E17" and "managed destination \| E16, E17", both Phase 3 for the E17 half. Step 3.1 crosses **neither** — it publishes nothing — so the arms are genuinely not owed *today* |
| 2 | 3.2's activation debt | Yes — §4 | Yes; §4 states the debt precisely |
| 3 | Git-directory workspace-root owner decision | Yes, **but** it constrains 3.2 — see [P3-3] | Yes. I confirmed the mechanism: `CatalogPrivateNameV1::relative_path` uses prefix `".gwz"` for `Workspace` and `"gwz"` for `GitDirectory` (`catalog_names.rs`), so a Git-directory catalog target genuinely has no `.gwz` ancestor, and the refusal is typed at preflight, not a silent fallback |
| 4 | Token read-back | Yes | Yes — §9 |

---

## 11. Findings

### [P1-1] Plan Step 3.1's durable-successor and prior-generation-retirement items are undelivered, and no plan step owns them

Two of the seven per-component items the plan names for this step — "durable
successor" and "prior-generation retirement", i.e. freeze §4.3 row **E17** and
the intent record's own publish/retire lifecycle — are absent (§3.3). The clause
that immediately follows them, "restart consumes the **resident** intent and
scheduled slots", is consequently delivered only as a re-derivation from durable
evidence, which leaves the permanently-wedging partial-retirement window of §6.

The severity is not about today's safety — nothing unsound ships, the refusal is
fail-closed and typed, the module is `allow(dead_code)`, and R2-D forbids
production catalog activation (plan §5 item 2). It is about **assignment**:

- Phase 3's milestone is "all four frozen purposes bootstrap physically and
  **restart-closed** through `ManagedParentBootstrap`". With this window open,
  that milestone is unmet.
- Neither remaining step can absorb the work. Step 3.2 is "<500 **test** LOC";
  Step 3.3 is coordinator glue that "wires machinery; it does not convert
  consumers". So the missing implementation has **no budgeted home**, and the
  Phase 3 settle would have to either accept an unmet milestone or absorb an
  unplanned step.
- Freeze §4.3 assigns E17 to Phase 3 and §4.4 owes it *two* Class 1 arms
  (managed source-interior **and** managed destination). Leaving E17 unassigned
  inside its own phase is the condition §4.4 warns about: "Had this freeze kept
  the 'one extension' wording, Phase 2.3 would have hit a mid-lane freeze-level
  stop the first time it published a staged component directory."

**Required remedy — a lane-owner scope decision, not an implementer round.**
Either (a) open a named Step 3.1b carrying E17's durable intent lifecycle plus
its two §4.4 Class 1 arms, budgeted and sequenced before the Phase 3 settle; or
(b) amend the plan to move E17 and the restart-closure milestone to a named
later step, recording the open window and its reachability (any row with ≥2
missing components — the ordinary first-merge `.gwz/stash/bundles` case) as an
accepted mid-lane state. Silence is not available: the plan's item list is the
gate the settle checks against.

### [P2-1] The five reserved keys whose edges this step converts are recorded nowhere durable

§4 rules that deferring *activation* to 3.2 stands. What does **not** stand is
that the conversion exists only in a transient implementer report. The freeze
§3.5 machinery exists precisely to make "which edges converted, in which step,
owing what" survivable across sessions, and the lane owner cannot check 3.2 off
against a record that was never written.

**Direct precedent, same lane, same shape:** `GwzM5-8R2DStep23-Review.md:868`
files "[P2-1] §3.5 map annotation unfiled" at **P2**, closed by a dated §3.5
blockquote in the `admission.*` form. The implementer here correctly declined to
edit a frozen memo unilaterally ("that is the lane owner's reviewed edit") —
that judgement is right, and it is exactly why the reviewer must file it.

**Required remedy — a dated §3.5 annotation at land, in the Step-2.3 form**,
naming the five keys (`staging_directory_create`, `ownership_marker_create`,
`ownership_marker_write`, `ownership_marker_flush`, `staging_directory_flush`),
stating that their **edges** converted in Step 3.1 while their **activation**
remains Step 3.2's by plan assignment, and pinning the three-part debt of §4.
Counts stay at **8/30 executed, 22 reserved, 165 total** until 3.2 lands.

### [P3-1] The stated production caller of `retain_managed_parent` does not exist; the item is now test-only

Three comments assert that `retain_managed_prefix` is `retain_managed_parent`'s
production caller — `pre_catalog.rs:38`, `provider.rs:74`, and
`managed_mutation.rs:293` ("This is the *only* caller of
[`retain_managed_parent`] that exists at all"). It is not:
`retain_managed_prefix` calls `retain_managed_child` directly
(`managed_mutation.rs:324`). `grep -rn 'retain_managed_parent\b' src/` returns
the definition, the three comments, and **one** call site —
`managed_mutation.rs:380`, inside the `#[cfg(test)]` door
`retain_managed_parent_at_for_test`.

So after this step `retain_managed_parent` has **zero production callers** and
is, in production terms, a test-support shim — a materially stronger change to
Step 2.3's landed surface than "dropped a re-export", and one a future reader
would take the comments' word against. No lint catches it (§2: `mod capability`
carries a blanket `allow(dead_code)`).

The §5 ruling is unaffected — the removal is right, and my independent
constructibility audit supports it on better grounds than the comment does.
Remedy: correct the three comments to the real call graph, and either fold the
one-component wrapper into the test door or state plainly that it is retained as
the door's shared retainer.

### [P3-2] No test pins the settled short-circuit against a missing component

The `Settled` arm's only defence against a row whose component vanished after
its retirement rows landed is the `depth != components.len()` guard
(`provider.rs:273`). §6 verifies that structurally, but no test drives it — the
settled test only exercises the converged case. A one-row addition to
`tests_provider.rs` (remove `.gwz/merge`, keep its retirement row, assert typed
refusal) would pin the guard that makes the short-circuit safe.

### [P3-3] Step 3.2's Git-directory matrix arm cannot run through this provider

`ManagedParentPurpose::path()` is workspace-relative and a Git-directory
catalog's private root is `gwz`, not `.gwz` (`CatalogPrivateNameV1::relative_path`),
so a Git-directory target refuses at preflight — pinned by
`a_git_directory_target_refuses_the_workspace_rooted_managed_paths`. Step 3.2 is
required to produce matrix rows "on both target variants". It must therefore
drive the Git-directory arm through the Step-2.3 door
(`retain_managed_parent_at_for_test`, exactly as `namespace/tests_managed_matrix.rs`
already does for its own eight keys), or resolve follow-up 3 first. Naming it
now prevents 3.2 discovering it mid-step. Follow-up 3 stays deferrable on this
route.

---

## 12. Budget

**980 code (568 production + 412 test) against an aspirational <500** — ~2×.
Recorded for the settle.

**Judgement: the mass is load-bearing, and the overrun is structural rather than
discretionary.** The plan's own §4 File-ownership note anticipates it
("1,000-line cohesion trigger, sub-500 targets"), the target is explicitly
aspirational, and the immediate precedent — Step 2.3 at 864 production code —
landed at the same order after review. Concretely, the 568 production lines are
not decoration:

- The prefix walk, the depth-composed retainer and `bind_child_component`
  (~120 lines) are the *only* route from the permit-retained root to a `Dir` a
  managed parent can be retained under; §7 shows the composition is required for
  correctness, not tidiness.
- `stage_component` + `write_or_rewrite_marker` (~110 lines) are the P2 writer
  half the plan names.
- The three forwarding hops (`pre_catalog.rs`, `catalog/bootstrap.rs`,
  `provider.rs`, ~55 lines) exist to keep the catalog **opaque** — each is a
  `revalidate()`-then-forward, and collapsing them would hand the provider a
  permit or a root.
- `provider.rs`'s 278 lines carry three trait methods, the resume classifier and
  the drive loop.

The one place I would have expected *less*: the module and item documentation is
unusually dense even by this lane's standard. That is a net positive for a
durable-transition kernel and I do not count it against the budget — but see
[P3-1], because dense prose that is wrong is worse than none.

---

## 13. Verdict

**GO**, conditional, with **escalation TRIGGERED** under freeze §9 ("automatic
escalation on any P0/P1/P2"). Routing is the lane owner's; no second axis is
assumed.

The object is technically sound and landable as an increment. Every gate is
green on an independently-constructed, twice-verified attributable tree; the
checker's two findings are exactly the expected land-time digest pins and
nothing else; the fault fixtures and the permit pin are byte-untouched and the
publication inventory is unmoved by count; the state machine is fail-closed by
construction and its three arms are genuinely driven by fresh-session tests; the
depth composition, the preflight bound, and the deterministic token all verify.

Both escalating findings are **lane-owner dispositions, not implementer
remediation rounds**:

- **[P1-1]** — a scope decision assigning E17's durable intent lifecycle to a
  named step, or a plan amendment moving the restart-closure milestone. Must be
  resolved **before the Phase 3 settle**, which cannot otherwise check the Step
  3.1 item list off.
- **[P2-1]** — a dated §3.5 annotation at land recording the five converted
  edges and 3.2's three-part debt.

The three P3s file and continue.

| Finding | Grade | Disposition |
| --- | --- | --- |
| [P1-1] E17 / durable successor + prior-generation retirement undelivered, unassigned | P1 | lane-owner scope decision before the Phase 3 settle |
| [P2-1] five converted reserved-key edges unrecorded in the freeze | P2 | lane-owner §3.5 annotation at land |
| [P3-1] `retain_managed_parent`'s stated production caller does not exist | P3 | file and continue |
| [P3-2] settled short-circuit missing-component guard untested | P3 | file and continue |
| [P3-3] 3.2's Git-directory arm must use the Step-2.3 door | P3 | file and continue |

---

## 14. Verification appendix — commands executed for this review

Scratch tree: `…/scratchpad/p31-scratch` = `git archive HEAD` of gwz-core
(`8b83a2c`) + the eight object files overlaid; workspace `Cargo.toml`/`Cargo.lock`
copied; `gwz-cli` and `taut-shape-rs` symlinked. `CARGO_TARGET_DIR` =
`…/scratchpad/p31-target` on every cargo call. Control tree:
`…/scratchpad/p31-head` = pristine `git archive HEAD`.

```
# bisection proof, run before and after the gate train
diff -rq p31-scratch/gwz-core/src gwz-core/src
  → differs only in the concurrent lanes' files; the 8 object files identical

# control
(p31-head) python3.13 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: ok (15 visible entries, 5 classified modules)   EXIT=0

# object
(p31-scratch/gwz-core) python3.13 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: failed
  - protected source tree changed: checked_artifact/capability/pre_catalog.rs
  - protected source tree changed: checked_artifact/catalog.rs
  EXIT=1

(p31-scratch) cargo check --all-targets
  Finished `dev` profile … in 3.69s          [no warnings]

(p31-scratch) cargo test -p gwz-core --lib 'checked_artifact::'
  test result: ok. 341 passed; 0 failed; 0 ignored; 0 measured; 1169 filtered out

(p31-scratch) cargo test -p gwz-core --lib 'checked_artifact::bootstrap::managed::tests_provider'
  test result: ok. 11 passed; 0 failed; 0 ignored; 0 measured; 1499 filtered out

(p31-scratch/gwz-core) CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings
  Finished `dev` profile … in 6.24s          [clean]

(gwz-core) rustfmt --edition 2024 --check <the 8 files>
  RUSTFMT_EXIT=0

# companion pins, counted not assumed
grep -rc 'publish_verified_no_replace(' src/     HEAD 21 | object 21 | new files 0/0
grep -n 13 interface_tests/capability_permit.rs:139
  assert_eq!(callers.matches("publish_verified_no_replace(").count(), 13);
git diff/status -- fault_expected_keys.rs fault_v1.rs capability_permit.rs   [empty]

# lint-suppression probe (method note, §2)
appended an unused private fn to managed_mutation.rs → cargo check emits no
dead_code warning; source: #[allow(dead_code)] on `mod capability`,
checked_artifact/mod.rs:20-23. Probe reverted and the scratch re-verified
byte-identical before the gate train above was re-run in full.
```

No git write operation was performed at any point; every git call was
`archive`/`show`/`diff`/`status`/`log`. The only repo-adjacent file written by
this review is this document.
