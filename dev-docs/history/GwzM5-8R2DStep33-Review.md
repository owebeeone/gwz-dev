# R2-D Phase 3 Step 3.3 — coordinator execution glue: single-axis Code review

Date: 2026-08-23. **Single-axis Code-lead** (`GwzM5-8R2DInterfaceFreeze.md` §9
:1329-1332: "Everything else is **single-axis with automatic escalation on any
P0/P1/P2**: … Phase 3 steps 3.1-3.3 **including the Phase 3 settle**"). Round 1
of a two-round cap. Peer-blind: the implementer's report was read as *claims to
verify*, never as evidence; every count below was recounted and every gate
re-run by this review in the live tree.

**Escalation IS triggered** — one P1. Per freeze §9 the trigger is recorded
here; routing is the lane owner's, not this review's.

Object: the uncommitted Step 3.3 edits over gwz-core base `7169d89` ("Land R2-D
Step 3.2: writer-key activation and the purpose policy matrix (review GO,
round-1 clean)"), exactly nine files:

| File | Disposition |
| --- | --- |
| `checked_artifact/coordinator/execution.rs` | **NEW** (352 lines / 196 code) |
| `checked_artifact/coordinator/tests_execution.rs` | **NEW** (317 lines / 252 code) |
| `checked_artifact/coordinator/mod.rs` | modified |
| `checked_artifact/bootstrap/managed.rs` | modified |
| `checked_artifact/bootstrap/managed/tests_provider.rs` | modified |
| `checked_artifact/bootstrap/managed/tests_writer_matrix.rs` | modified |
| `checked_artifact/bootstrap/managed/tests_purpose_policy.rs` | modified |
| `checked_artifact/namespace/mod.rs` | modified |
| `checked_artifact/capability/pre_catalog/provider.rs` | modified |

Baseline: gwz-core `7169d89`; gwz-dev repo HEAD `36d2af7`.

**The dirty set is exactly these nine, and no other lane is in the tree.**
`git status --porcelain` in `gwz-core` returns exactly seven ` M` and two `??`
rows, matching the table; `git status` in `gwz-dev` was clean at review start.
Every gate below ran in the live tree. Checker attribution was **constructed,
not assumed** (§10).

*Concurrency note.* Mid-review a second lane wrote `dev-docs/GwxMergeFindingsDraft.md`
into `gwz-dev` (08:16, not this review's). It is docs-only, outside `gwz-core`,
and does not touch the object; the nine-file gwz-core dirty set was re-checked
after every gate and never changed. This review's own only write is this file.

---

## 1. Verdict

**GO with one P1 remediation owed before the Phase 3 settle.**

The step is real, in scope, and structurally careful. Admission binding (§3),
the opaque facade (§4), the wires-not-converts bar (§5), the allow corrections
(§6) and the three folded 3.2 findings (§7) all verify. The single P1 is in the
§8 write-authority gate itself — the security-shaped core — and it is a missing
comparison rather than a design error: the gate proves the observation was
*issued against* this action's reservation, but nothing proves the observation
was *streamed from* this action. Two of the three remedies are one comparison.

Findings, most severe first:

| # | Sev | One line |
| --- | --- | --- |
| P1-1 | P1 | The §8 gate binds a caller-chosen reservation, not observed provenance: a proof streamed under another action of the same owner mints authority. |
| P2-1 | P2 | "Writers receive **only** the opaque proof" is unenforced — `bootstrap_managed_parents` returns path-bearing rows, and the facade is optional. |
| P3-1 | P3 | The report calls the Phase 3 settle "the dual settle"; freeze §9 makes it single-axis. |
| P3-2 | P3 | Two landed comments name Step 3.3 as owner of an obligation Step 3.3 declined, and the step list is now closed. |
| P3-3 | P3 | `RevalidatedManagedParentV1` is `Clone` and `RetainedWriteAuthorityV1` is `Copy`; the module doc claims re-proving is the only route. |
| P3-4 | P3 | The pairing gate's negative row varies all four bound fields at once; no retry/stale-schedule row. |
| P3-5 | P3 | `pub use execution::*;` is a live `unused_imports` allow with zero consumers — the exact shape this step spent its budget correcting. |

---

## 2. What the authorities owe, quoted whole

Plan §4 Step 3.3 (`GwzM5-8R2D-Plan.md:379-385`), the scope sentence entire:

> **Step 3.3 — coordinator execution glue** (<500 LOC): schedule +
> `AdmittedActionV1` binding so that "replacement/removal executes only after an
> admitted action and an owner-private coherent authority observation"
> (ConsumerCheckpoint §8 :239-240); writers receive only the opaque
> retained-parent proof (:264-266). Depends on Phase 1 and 3.1. **This step
> wires machinery; it does not convert consumers** (that is R2-E).

`GwzM5-8R4bR2ConsumerCheckpoint.md` §8 :239-240 and §9 :264-266:

> Replacement/removal executes only after an admitted action and an
> owner-private coherent authority observation.

> Writers receive an opaque retained-parent proof. They may write only through a
> facade operation that revalidates that proof. A path string or successful
> `exists()` check is not parent authority.

Plan §5 item 2 (:470-474): "`recover_or_create` gains no production caller in
R2-D." Freeze §3.1 (:154-232) fixes the admission seam; §3.2 (:233-…) the
`RawNamespaceBackend` delta — untouched by this step and correctly so.

---

## 3. The §8 write-authority gate — the security-shaped core

### 3.1 What verifies

`AdmittedCheckedActionV1::authorize_write` (`execution.rs:201-224`) is fail-closed
and typed at every exit. It refuses unless the operation is `Replace` or
`Remove`; converts the observation through `CheckedAuthorityRecordV1::issue`;
and refuses unless `record.matches_reservation(self.admitted.reservation())`.

`matches_reservation` (`protocol/authority_record.rs:195-203`) compares exactly
the four fields claimed — action digest, request owner binding, schedule digest,
reservation digest — and `ActionCapacityReservationV1::record_digest` is a
SHA-256 over action digest + owner binding + schedule (`protocol/admission.rs:36-44`),
so the four-field test is equivalent to full reservation equality. `Observe` and
the no-op `Replace` cannot even reach this method: `derive_new_reservation`
returns `ProofOnly` for both (`coordinator/schedule.rs:31-35`), and `ProofOnly`
carries no `ScheduledCheckedActionV1`. `ParentOnly` reaches it and is refused,
with a row for it (`tests_execution.rs:249-273`).

The Step-2.4 composition claim is checked and **not restated**: 2.4's coherence
lives entirely in `protocol/authority_record/owner{,/host}.rs` — the facts type
is unnameable outside the owner, `AuthorityFactsIssuerV1` is neither `Clone` nor
`Copy` nor storable past one call, and `CheckedAuthorityObservationV1::owner_issue`
has exactly one caller (`owner.rs:84`). Nothing in `execution.rs` weakens or
duplicates any of it. `CheckedAuthorityRecordV1::issue` adds only
`validate_profiles` (:217-227), a support-profile consistency check — it is a
pure conversion, so the gate is genuinely `matches_reservation` and nothing else.

### 3.2 [P1-1] The gate binds a caller-chosen reservation, not observed provenance

**Escalation trigger (freeze §9 :1329).**

`CheckedAuthorityObservationOwnerV1::observe` (`protocol/authority_record/owner.rs:74-92`)
copies all four binding fields *from the reservation argument the caller passes
in*, and the only cross-check against the observed facts is:

```rust
if facts.request_owner_binding != reservation.request_owner_binding() {
```

The production transaction adds no more.
`AuthorityTransactionV1::observe_retained_request`
(`capability/pre_catalog/provider/authority_record_binding.rs:378-391`) issues
from a `StreamedPayloadProofV1` and never compares `proof.action()` — which the
proof carries (`:137-152`) — against the reservation being issued for.

`request_owner_binding` is per merge owner, not per action:
`CheckedActionOwnerV1::for_merge_start(workspace_id)` yields one binding that
every leaf request of that workspace shares — the suite relies on exactly this
when it builds its "different action" from the same `owner()`
(`tests_execution.rs:42-44`, `:239`).

So the adversarial case the brief asks about is realizable through the
production route: **a `StreamedPayloadProofV1` taken under action B's retained
action directory, issued against action A's reservation, produces an observation
that `authorize_write` on A accepts, and mints `RetainedWriteAuthorityV1` for
A.** Nothing in 2.4 or 3.3 refuses it.

The module doc asserts the opposite as a structural property
(`execution.rs:190-195`): "the observation must bind **this** admitted action's
reservation … so an observation coherent in itself but taken for another action
is refused." That is true only for an observation *issued against* another
reservation — which is what the suite's negative row tests
(`tests_execution.rs:237-245`, where the foreign observation is built by passing
`other_scheduled.reservation()` to the synthetic issuer). It is false for an
observation *streamed from* another action. The passing test does not distinguish
the two readings, so the claim reads as verified when it is not.

Compounding it: `RetainedWriteAuthorityV1` carries `{action, record_id}` only
(`:234-238`) and discards every leaf-binding fact the record holds — source
fingerprint, artifact root, retained parent identity, `expected_sha256`,
`goal_sha256`. A downstream consumer handed this token cannot re-derive any of
them, so the token's own shape forecloses the check being made later.

**Severity.** P1, not P0: nothing is reachable from an entry point, no consumer
takes the token, no durable record or wire shape is involved, and R2-E must build
the replacement path anyway. P1 rather than P2 because this *is* the step's
headline gate, the property is asserted as structural in the code's own doc, and
the suite reads as proving it.

**Remedies — any one closes it.**

1. Have `authorize_write` take the `StreamedPayloadProofV1` and mint the
   observation inside the gate. This is the strongest form: it makes the
   provenance check unavoidable, and it simultaneously gives
   `authority_record_binding` the production consumer whose absence §6 below has
   to document.
2. Compare `proof.action()` against `reservation.action_digest()` in
   `AuthorityTransactionV1::observe_retained_request` or in
   `CheckedAuthorityObservationOwnerV1::observe`. This is 2.4's seam, so it is a
   cross-step edit — but it is one comparison and it closes the hole for every
   future consumer, not just this one.
3. Minimum strengthening available inside `execution.rs` today, needing nothing
   from another owner: compare `record.expected_sha256()` and
   `record.goal_sha256()` against `self.request.expected()` / `self.request.goal()`
   (`CheckedLeafFactV1::Exact { sha256, .. }`). `authorize_write` already holds
   both sides — the request's expected/goal are canonical fields of the action
   digest (`coordinator/identity.rs:401-420`) and the record's accessors are
   `pub(in crate::checked_artifact)` (`protocol/authority_record.rs:181-193`).
   This does not close the case where two actions share expected/goal, so it is a
   floor, not a fix.

Whichever is taken, the doc sentence at `execution.rs:190-195` needs to say what
is actually proved, and `tests_execution.rs` needs a row whose foreign
observation is *streamed elsewhere but issued here*.

---

## 4. The admit binding — verifies

`ScheduledCheckedActionV1::admit` (`execution.rs:125-140`) drives the Phase-1
owner and refuses unless `admitted.reservation() == &self.reservation`.
`ActionCapacityReservationV1` derives `Eq`/`PartialEq` over all four fields
including the full schedule, so the comparison is structural, not a digest
shortcut.

**One request's schedule genuinely cannot pair with another action's admission.**
Three facts together:

- `AdmittedActionV1` has exactly one construction site in the whole crate —
  `protocol/admission/owner.rs:124` — gated on idle admission, missing staging,
  and `final_directory.exact_identity_for(expected)`, which itself requires
  `extra_children: 0` and `RecordObservationV1::Exact(value) if value == expected`
  (`:54-68`). The only other route is `#[cfg(test)] admit_observed_action`
  (`protocol/admission/test_support.rs:67-79`), behind `#[cfg(test)] mod test_support;`
  (`protocol/admission.rs:16-17`).
- `ScheduledCheckedActionV1` and `AdmittedCheckedActionV1` both have private
  fields and exactly one constructor each, both inside `execution.rs`
  (`schedule_checked_action` and `admit`). A crate-wide grep for either type
  outside `coordinator/execution.rs` and `coordinator/tests_execution.rs` returns
  nothing.
- The driver itself keys on `expected` throughout (`admission/driver.rs:35-…`),
  so the belt-and-braces comparison in `admit` is redundant-but-correct, exactly
  as its doc says.

`bootstrap_managed_parents` (`:167-184`) additionally refuses a managed request
whose action digest or owner binding is not this request's, with a row
(`tests_execution.rs:199-221`) that also asserts nothing was created. The
preflight→bind→execute ordering is the owner's own, and `bind`
(`bootstrap/managed/owner.rs:179-209`) re-proves provider instance, action
digest, owner binding, managed-plan digest, row count **and** full schedule
reproduction, while `execute` (`:249-261`) re-checks the instance and calls
`revalidate_plan`. The doc claim "a stale plan cannot execute" holds.

Cross-catalog abuse (admit under one target, execute under another) fails closed
one layer down: `retain_action_namespace` re-proves the action directory identity
against the resident directory. Not a finding, but worth recording that the
coordinator does not itself bind the two sessions' catalogs.

---

## 5. The opaque facade — verifies, with [P2-1] one level up

### 5.1 The facade itself is opaque, at the type level

`ManagedParentFacadeV1` (`execution.rs:257-327`) has private fields and exactly
one constructor, `from_retained_row` (`:267`), plus `all` which delegates to it.
There is no route from a path, a name, or an `exists()`.

Type-level leak audit of everything reachable from a facade:

- `purpose() -> ManagedParentPurpose` — the only reader. `ManagedParentPurpose::path()`
  is **module-private** (`bootstrap/managed.rs:80`, no visibility modifier), so a
  facade holder outside `bootstrap::managed` cannot call it.
- `revalidate() -> RevalidatedManagedParentV1`, which exposes `purpose()` and
  `identity() -> &DurableObjectIdentityV1`. An object identity is volume/object
  material, not a path.
- No `Dir`, no `PathBuf`, no `&Path`, no path string, no handle, no `exists()` is
  reachable. §9 :264-266's "a path string or successful `exists()` check is not
  parent authority" **is** a property of this type, as claimed.

One nuance for the record, below finding level: `ManagedParentSpec::for_purpose(p).components()`
is `pub(in crate::checked_artifact)`, so any crate-internal holder of a
`ManagedParentPurpose` can re-derive the purpose's *declared, static,
root-relative* component list. That is a compile-time constant available to
anyone who can name the enum, it conveys no root and no authority, and it is not
what §9 is about.

`revalidate` (`:300-326`) does what it claims: it re-walks the prefix through the
opaque catalog with `observe_managed_prefix` — one no-follow hop per component
from the permit-retained root, each hop's identity proved before the next,
stopping at the first absent component
(`capability/pre_catalog/provider/managed_mutation.rs:261-290`) — then refuses on
**depth** (`at(components.len())` is `None` → "no longer resident"; `at` is
1-based, `:239-247`), **identity**, **child mode**, and **canonical path**. The
substitution row (`tests_execution.rs:303-316`) removes and recreates the managed
parent and asserts the refusal. Verified.

### 5.2 [P2-1] "only" is not enforced at the step's own production entry

`AdmittedCheckedActionV1::bootstrap_managed_parents` returns
`RetainedManagedParentsV1`, and `RetainedManagedParentRowV1::path() -> &CanonicalPathIdentityV1`
is `pub(in crate::checked_artifact)` (`bootstrap/managed/provider.rs:118-120`).
The facade is an *optional* wrapper a caller may or may not apply. So the plan's
"writers receive **only** the opaque retained-parent proof" and §9's "Writers
receive an opaque retained-parent proof. They **may write only** through a facade
operation that revalidates that proof" are, as landed, conventions at the R2-E
boundary rather than properties of the delivered API.

The module doc states the stronger thing: "The result is the opaque
retained-parent proof and nothing else" (`execution.rs:166`). That is not what
the signature returns.

Remedy: return `Vec<ManagedParentFacadeV1>` (or a facade set) from
`bootstrap_managed_parents`, which makes "only" structural at negligible cost
since nothing consumes the rows yet; or correct the sentence and docket the
"only" explicitly for R2-E so the settle rules on it rather than inheriting it.
Note that §9's second sentence has no enforcement point at all today — the facade
has a revalidating operation but no write operation, so "may write only through"
is a rule R2-E must honour, not one the type imposes.

---

## 6. Wires-not-converts, and the reachability statement — verifies

- `entry.rs` is **untouched** (absent from the dirty set) and still routes parent
  preparation through `CheckedArtifact::prepare_parent` (`entry.rs:145`).
- The whole owner is `pub(in crate::checked_artifact)`; every item in
  `execution.rs` carries that visibility, verified item by item.
- **No consumer calls it.** Crate-wide grep for `AdmittedCheckedActionV1`,
  `ScheduledCheckedActionV1`, `CheckedExecutionPlanV1`, `ManagedParentFacadeV1`,
  `RetainedWriteAuthorityV1`, `schedule_checked_action` outside
  `coordinator/execution.rs` + `coordinator/tests_execution.rs`: zero hits (the
  one `schedule_checked_action` hit is a doc comment in `coordinator/mod.rs:20`).
- **Plan §5 item 2 intact.** `recover_or_create` has no production caller: every
  one of its ~25 call sites is inside a `#[cfg(test)]` module, and
  `execution.rs` never names it — the owner receives an already-recovered
  `OpaqueRetainedCatalogV1` as a parameter.

**The reachability statement is accurate.** Verified against base by
caller-provenance, not by assertion:

| Item | Callers before 3.3 | After |
| --- | --- | --- |
| `ActionAdmissionOwnerV1::from_retained_catalog` | `admission/tests*.rs`, `admission/driver/tests.rs`, `bootstrap/managed/tests_provider.rs`, `namespace/tests_fault_matrix.rs`, two `pre_catalog/provider/tests_*` — all `#[cfg(test)]` | + `coordinator/execution.rs:129` (production) |
| `RetainedManagedParentProviderV1::from_retained_catalog` | `bootstrap/managed/tests_provider.rs` only — `#[cfg(test)]` | + `coordinator/execution.rs:179` (production) |
| `derive_new_reservation` | `interface_tests/coordinator_contract.rs`, `interface_tests/coordinator_remediation.rs` only — `#[cfg(test)]` | + `coordinator/execution.rs:90` (production) |

"`derive_new_reservation`'s first production caller" is therefore literally true.
And the correct reading of "production" here — a non-`#[cfg(test)]` caller inside
the crate boundary, still unreachable from any entry point — is the one the
implementer states, not a stronger one.

---

## 7. The allow corrections — all four reasons verify as TRUE

**The inertness probe replicates.** `checked_artifact/mod.rs` carries blanket
`#[allow(dead_code)]` on `mod bootstrap` (:14-18), `mod capability` (:19-23),
`mod coordinator` (:36-40) and `mod namespace` (:58-62). `allow` is inherited by
descendants, so every inner `#[allow(dead_code)]` in this phase is inert, exactly
as claimed — and "the allow fell away" is indeed not a reachability argument
anywhere in this subtree. Correctly scoped, too: the blanket allows cover
`dead_code` only, so the `unused_imports` allows on the `pub use` hops are
**live**, and the implementer's note does not overclaim there.

| Allow | Corrected reason | Verdict |
| --- | --- | --- |
| `bootstrap/managed.rs:15-19` (`mod provider`) | "Step 3.3 wires the coordinator caller; reachability from an entry point is R2-E" | **TRUE** — `execution.rs:179` is the caller; `entry.rs` untouched |
| `namespace/mod.rs:9-12` / `:30-33` (`mod host`, `use host::…`) | "the managed-parent provider has driven this backend since Step 3.1" | **TRUE** — `bootstrap/managed/provider.rs:410-411` calls `retain_action_namespace`, landed at 3.1. The prior text ("plan §4 Step 3.3 wires its production consumer") had been stale since 3.1, as stated |
| `capability/pre_catalog/provider.rs:14-18` (`authority_record_binding`) | "its consumer is the replacement path R2-E converts, not Step 3.3" | **TRUE** — `authorize_write` consumes an already-minted `CheckedAuthorityObservationV1`, never `AuthorityTransactionV1`; the binding's only callers remain `tests_authority_record*.rs`. (See P1-1 remedy 1: wiring it here would have closed the P1 *and* discharged this allow.) |
| `capability/pre_catalog/provider.rs:25-34` (`leaf_observation`) | "Step 2.4 binds the observer; the binding's consumer is the replacement path R2-E converts" | **TRUE** — `authority_record_binding.rs:74` uses `HostLeafObserverV1`, and that binding has no production consumer |
| `coordinator/mod.rs:20-31` (`use schedule::*`) | "the schedule facade's production consumer is coordinator-internal; this hop serves the interface-test contracts" | **TRUE** — `execution.rs:55` reaches `derive_new_reservation` via `super::schedule`, and both `interface_tests/coordinator_contract.rs:9-14` and `coordinator_remediation.rs:10-14` import through the hop; `interface_tests` is `#[cfg(test)]` |

This is the right disposition. Correcting the text rather than deleting the
attributes is the outcome that leaves the next reader with true statements, and
the four reasons now say what the code does.

### [P3-5] One new allow repeats the pattern being corrected

`coordinator/mod.rs:11-15` adds `pub(in crate::checked_artifact) use execution::*;`
under an `unused_imports` allow. That glob has **zero** consumers outside
`coordinator` — unlike the `schedule::*` hop, no interface test reaches through
it — and unlike the `dead_code` allows it is a live suppression. A re-export
kept alive by an allow, for a consumer that does not exist yet, is precisely the
shape whose stale reasons this step spent its budget fixing. Delete it until
R2-E needs it, or its reason is next round's stale text.

---

## 8. The three folded 3.2 findings — all verify

**[P3-1 of 3.2] — shape sentences.** Both now state the real property.
`tests_writer_matrix.rs:42-56` and the `SINGLE_CROSSING_BOUNDARIES` entry at
`:104-116` say what actually holds: `staging_directory_flush` has two sites, both
fire on a creating drive, and the order is fixed because site B runs only when
`created` and a fresh row always has an inexact interior — so the arm always
resolves to the interior's site and the parent's is announced but never the
interruption point. The `tests_provider.rs:344-354` matrix comment carries the
matching correction. The sixth-key/census consequence is named as a settle
question, and the census it would move is real: `EXPECTED_KEY_COUNT: usize = 165`
(`interface_tests/fault_expected_keys.rs:174`, asserted twice at `:849-850`).

**[P3-5 of 3.2] — dead fixture helpers.** `Fixture::managed_root` and
`prepare_managed_root` are deleted; a crate-wide grep for either name returns
nothing. The widening in the same file is minimal and matches its own comment —
`TargetVariantV1`, `Fixture`, `new`, `path`, `target_root`, `prepare_prefix`,
`with_catalog` move to `pub(in crate::checked_artifact)`; `mod matrix` and every
assertion helper stay `pub(super)`.

**[P3-3 of 3.2] — the §9 refusal pins its typed reason, and the negative test
distinguishes.** `tests_purpose_policy.rs:181-189` matches
`Err(CheckedFsError::Ambiguous { fact, detail })` and asserts
`fact == "managed-parent plan"` and
`detail == "provider retained prefix violates the purpose ownership policy"`,
with `Err(other) => panic!` and `Ok(_) => panic!` arms. That string has exactly
one production source, `bootstrap/managed/owner.rs:113-116`, raised when the
observed retained count is below the purpose's minimum or the retained path does
not match the spec. The discrimination is genuine: a lease failure is a different
`CheckedFsError` variant and hits the `other` arm; a bad prerequisite is
`Ambiguous { fact: "managed-parent request", … }` and fails the `fact` assert;
the sibling policy refusal in the same function carries a different `detail`
("provider plan is reordered, different, or exceeds the fixed path") and fails
the `detail` assert. Suite green: 10/0.

---

## 9. The settle inventory — items 1-7 checked, plus what it misses

Each of the seven is checked against the code, not the report.

1. **`preflight` / `plan_complete` disposition — ACCURATE.** Both keys exist
   (`fault_v1.rs:148`, `:177`); `ManagedBootstrap*` is 30 keys; 28 are the
   executed set. Correctly characterised as docketed and settle-blocking, and
   correctly not decided by the implementer.
2. **`retain_managed_parent`'s production visibility — ACCURATE.** Zero
   production callers; the only route is the `#[cfg(test)]` door
   `retain_managed_parent_at_for_test`
   (`capability/pre_catalog/provider/managed_mutation.rs:383-391`), used only by
   `namespace/tests_managed.rs`.
3. **Git-directory catalog bound to its workspace root — ACCURATE.**
   `ManagedParentPurpose::path()` is workspace-relative (`bootstrap/managed.rs:80-87`),
   and both matrices fixture-place the prefix under `Fixture::target_root(variant)`,
   which for the Git-directory arm is the repository's own path.
4. **`staging_directory_flush`'s second site — ACCURATE.** See §8; the freeze-level
   consequence (moving 165) is real.
5. **Multi-component writer interruption unexercised — ACCURATE.**
   `SHAPE: RowShapeV1 = RowShapeV1::OneComponent` (`tests_writer_matrix.rs:57`);
   coverage not gained rather than removed, as stated.
6. **Resume of a partially executed action's handoff — ACCURATE in every
   particular.** `resume_or_admit` refuses a non-exact action directory:
   `owner.admit` requires `exact_identity_for`, which requires `extra_children: 0`
   **and** `RecordObservationV1::Exact(value) if value == expected`
   (`protocol/admission/owner.rs:54-68`, `:114-128`) — and after the first
   namespace edge the directory carries scheduled-slot children. The driver then
   takes its "a conflicting or ambiguous action occupies the derived final action
   name" stop (`admission/driver.rs:81-87`). The only reconstruction route is
   `#[cfg(test)] admit_observed_action`, and both matrices in this phase use it
   (`bootstrap/managed/tests_provider.rs:243-256`,
   `namespace/tests_fault_matrix.rs:251-265`). Closing it does touch freeze §3.1.
   One qualification: the seam pin
   (`interface_tests/r2d_seam_freeze.rs:110-140`) asserts the **presence** of the
   struct, `from_retained_catalog` and `resume_or_admit` plus a forbidden-token
   list; it does not assert exhaustiveness, so adding a fourth method would not
   fail it. "Fixes the owner's public surface at three items" is therefore a
   slightly stronger claim than the pin actually makes.
7. **The blanket-allow blind spot — ACCURATE.** Replicated in §7. The settle
   recommendation (narrow the blanket allows before R2-E) is well founded: they
   are what let two orphaned helpers and several stale forward references survive.

### What the inventory misses

- **P1-1 is not on it.** The observation-provenance gap is the most
  settle-relevant open item in the step and does not appear.
- **P3-1 — the settle's own tier is wrong in the report.** The inventory is
  headed "open at the dual settle" and repeats "dual settle" throughout. Freeze
  §9 :1329-1332 explicitly supersedes plan §4's Phase-3 dual line: Phase 3 steps
  3.1-3.3 **including the Phase 3 settle** are single-axis with automatic
  escalation. Routing is the lane owner's, but the settle should not inherit a
  wrong tier from its own inventory.
- **P3-2 — two landed comments now name a closed step as a future owner.**
  `namespace/tests_fault_matrix.rs:23-29` ("Resuming that handoff from durable
  state is the coordinator glue of plan §4 Step 3.3, not this step") and
  `bootstrap/managed/tests_provider.rs:228` ("resuming a durable handoff is plan
  §4 Step 3.3's coordinator"). Step 3.3 correctly declined the obligation — the
  plan's 3.3 sentence does not name it and closing it needs a freeze edit — but
  Phase 3's step list is now declared complete with those comments still pointing
  at it. The settle must either take item 6 or re-point both comments; a
  completed step named as the future owner is how an obligation gets lost.
- **P2-1's "only" gap** (§5.2) is not on the list either.
- **P3-3 — the two proof types are freely duplicable.**
  `RevalidatedManagedParentV1` derives `Clone` and borrows nothing, so
  `execution.rs:331-333`'s "it borrows nothing and carries no handle, so
  re-proving is the only way to obtain another one" is false: `clone()` obtains
  another one and the value can be stashed indefinitely. `RetainedWriteAuthorityV1`
  is `Copy` for the same reason. Harmless today because neither carries a
  capability, but both are cited as structural properties and R2-E will read them
  as such.
- **P3-4 — the pairing gate's negative row is one-dimensional.** The foreign
  observation at `tests_execution.rs:239-241` differs by *operation* (Replace vs
  Remove), so all four bound fields differ at once. There is no row for the same
  action digest with a different schedule (a retry) or a stale reservation. The
  property holds by construction — `record_digest` is a SHA-256 over action digest
  + owner binding + schedule — so this is coverage, not correctness, but the
  brief's "another schedule / stale reservation" cases are asserted by the doc
  and demonstrated by neither test.

---

## 10. Gates — all re-run in the live tree

```
$ CARGO_TARGET_DIR=…/p31-target cargo check --all-targets
    Finished `dev` profile … in 0.11s                       [clean]

$ cargo test --lib checked_artifact::
test result: ok. 367 passed; 0 failed; 0 ignored; 1170 filtered out; 34.80s
                                                            [360 → 367, +7]

$ cargo test --lib checked_artifact::coordinator::tests_execution
test result: ok. 7 passed; 0 failed                                   0.63s

$ cargo test --lib checked_artifact::bootstrap::managed
test result: ok. 30 passed; 0 failed                                 14.37s
  …::tests_purpose_policy   10 passed; 0 failed                       1.03s
  …::tests_writer_matrix     4 passed; 0 failed                       4.03s

$ cargo clean -p gwz-core && CLIPPY_CONF_DIR="$PWD" \
    cargo clippy --all-targets --all-features -- -D warnings
    Checking gwz-core v0.10.4 … Finished in 12.26s          EXIT 0
    [forced full recompile; the first run returned a cached result and was
     re-run from clean so the lint actually saw the new files]

$ rustfmt --edition 2024 --check <the 9 files>              EXIT 0

$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: failed
- protected source tree changed: checked_artifact/capability/pre_catalog.rs
EXIT 1                                                      [exactly one finding]
```

**Checker attribution, constructed not assumed.** `src/checked_artifact` at
`7169d89` was extracted to scratch and the checker's own `source_tree_digest`
re-run over both trees for all four `PROTECTED_SOURCE_TREE_DIGESTS` entries:

| Tree | base `7169d89` | live |
| --- | --- | --- |
| `bootstrap/runtime/catalog_lease.rs` | matches pin | matches pin |
| `capability/path.rs` | matches pin | matches pin |
| `capability/pre_catalog.rs` | matches pin | **`ef0bb7e9…` ≠ pin** |
| `catalog.rs` | matches pin | matches pin |

The base is clean on all four; exactly one diverges live; the only dirty file
inside that tree is `capability/pre_catalog/provider.rs`, whose sole change is
the allow-reason text. Attribution confirmed: the single finding is Step 3.3's,
it is a tree-digest pin, and it is the lane owner's at landing.

**Publication seam, recounted.** `grep -rn 'publish_verified_no_replace(' src/`
→ **21**, unchanged from base. `interface_tests/capability_permit.rs:139` asserts
the caller inventory at **13**, and that test is green inside the 367. No caller
changed, so no companion is owed. `capability_permit.rs`, `publication.rs`,
`protocol/` and `scripts/checks/` are all absent from the dirty set.

**LOC, recounted** (non-blank, non-comment):

```
coordinator/execution.rs        352 lines   196 code
coordinator/tests_execution.rs  317 lines   252 code
7 modified files                +101/−41     42 added code lines
                                       TOTAL 490 code
```

**490 against the aspirational <500** — the implementer's figure reproduces
exactly.

---

## 11. Escalation record

Freeze §9 :1329-1332 mandates automatic escalation on any P0/P1/P2 for Phase 3
steps 3.1-3.3. **This review files one P1 (§3.2) and one P2 (§5.2), so the
trigger is hit and is recorded here. Routing the escalation is the lane owner's
call, not this review's.** The five P3s file and continue (§9 :1333-1334: "P3
findings file and continue; they do not become packages and do not enlarge
R2-D").

The verdict remains **GO**: the object is in scope, the machinery is sound, the
gates are green, the allow corrections are true, and the settle inventory is
accurate on all seven of its own items. P1-1 is a missing comparison in a gate
that no production caller can reach, with three named remedies, two of them one
line. It should be closed — or explicitly ruled on — before the Phase 3 settle
declares this phase's step list complete, because the property it fails to prove
is the one the plan sentence exists to buy.

---

## 12. Verification appendix — commands executed by this review

```
git -C gwz-core status --porcelain      7 ' M' + 2 '??' = the nine, nothing else
git -C gwz-dev status --porcelain       empty
git -C gwz-core log --oneline -1        7169d89  [base confirmed]

grep -rn '<type>' src/checked_artifact  caller provenance for:
  ActionAdmissionOwnerV1 | RetainedManagedParentProviderV1 |
  derive_new_reservation | recover_or_create | retain_action_namespace |
  retain_managed_parent | owner_issue | AdmittedActionV1 { |
  CheckedAuthorityObservationV1 { | synthetic_authority_observation |
  managed_root | prepare_managed_root | the five new execution.rs types
EXPECTED_KEY_COUNT                      165  (fault_expected_keys.rs:174)
ManagedBootstrap* keys in fault_v1.rs   30
publish_verified_no_replace( in src/    21   | capability_permit.rs:139 → 13
source_tree_digest over base vs live    4 pins, 1 divergence  (§10)
LOC recount, non-blank non-comment      196 + 252 + 42 = 490
```

No git write operation was performed at any point; every git call was
`status`/`log`/`diff`/`archive`. `cargo clean -p gwz-core` touched only the
scratch target directory
(`…/scratchpad/p31-target`). The only file written by this review is this
document.

---

# Round 2 — re-verdict (single-axis Code-lead, round 2 of 2)

Date: 2026-08-23. Same reviewer, same axis, same protocol: live tree,
`CARGO_TARGET_DIR=…/scratchpad/p31-target`, clippy re-run from
`cargo clean -p gwz-core`. The implementer's round-2 report was again read as
claims to verify. Two-round cap: **this is the final round.**

**Verdict: GO. All seven round-1 findings CLOSED. Escalation discharged — no
P0/P1/P2 stands. Two new P3s file and continue (freeze §9 :1333-1334).**

The object grew from nine files to **eleven**: `namespace/tests_fault_matrix.rs`
(comment repoint) and `protocol/authority_record.rs` (one additive accessor)
joined the set. `git status --porcelain` in `gwz-core` returns nine ` M` and two
`??`, matching. Base still `7169d89`.

## R2.1 [P1-1] — CLOSED, and the red independently reproduced

The fix (`execution.rs:261-265`) compares the record's
`retained_parent_identity()` against `self.admitted.directory_identity()`.

**That field is genuinely observed, not restated.** Traced end to end:
`observe_streamed_payloads` sets `retained_parent_identity: parent.identity().clone()`
from the `RetainedActionDirectoryV1` the payloads were streamed through
(`capability/pre_catalog/provider/authority_record_binding.rs:227-232`, under its
own comment "Provenance, taken from the capability these payloads were actually
streamed through — not from a caller"); `AuthorityTransactionV1` carries it
forward untouched (`:378-391`); `owner_issue` takes it from the facts while
copying the four reservation fields from its argument
(`protocol/authority_record.rs:93-123`); `CheckedAuthorityRecordV1::issue` copies
it again unmodified (`:126-143`). It is the only binding field on that path not
sourced from the caller's reservation.

**(a) The red drives the actual cross-action shape — reproduced by this review,
not taken on trust.** `an_observation_streamed_under_another_action_must_not_authorize`
(`tests_execution.rs:369-399`) admits two real actions in one fixture under one
merge owner, asserts their directory identities differ, then builds the
observation by passing the *other* action's `directory_identity()` as
`retained_parent_identity` while issuing against the *target's* reservation.
That is the production shape.

I copied the workspace to scratch, deleted **only** the five-line provenance
check, and re-ran the suite:

```
$ cargo test --lib checked_artifact::coordinator::tests_execution   [scratch copy,
                                       provenance check removed, all else intact]
test …::an_observation_streamed_under_another_action_must_not_authorize ... FAILED
panicked at tests_execution.rs:394:5:
  an observation streamed under another action's retained directory must not
  authorize a write on this one
test result: FAILED. 8 passed; 1 failed; 1530 filtered out
```

Three things follow, and all three matter:

1. The round-1 code **did** mint `RetainedWriteAuthorityV1` for the smuggled
   observation. P1-1 was real, and the implementer's red tail is accurate.
2. The provenance check is the **load-bearing** fix. Remedy 3's digest floor does
   *not* catch this case — the smuggled observation is built with the target
   operation's own leaf digests, so the floor passes. Isolation is clean.
3. The other eight rows still pass without the provenance check, so the new row
   is the only detector: no accidental double-coverage masking a regression.

**(b) Typed and fail-closed** — `execution_error` → `CheckedFsError::Ambiguous
{ fact: "checked action execution", detail: "the authority observation was
streamed under another action's retained directory" }`. See [P3-6] below for the
one thing the row does not assert.

**(c) The premise — two admitted actions never share a directory identity —
holds.** The active action directory's name is
`format!("{ACTION_PREFIX}{}{PROTOCOL_VERSION_SUFFIX}", action.hex())`
(`protocol/slots.rs:371-379`), derived from the action digest alone. Distinct
action digests therefore name distinct live directories with distinct durable
identities. The same-digest case cannot masquerade either: `authorize_write`'s
*first* check is still `matches_reservation`, which is full reservation equality
(the record digest is a SHA-256 over action digest + owner binding + schedule), so
a same-name/different-schedule reservation is refused before provenance is
reached. Recorded caveat, not a finding: the residual is sequential reuse of one
name after retirement *plus* durable-identity recycling, and that rests on the
same identity-as-substitution-detection primitive (Track P §4.1, P3 no-follow open
+ identity compare) the whole subsystem already rests on. No new assumption is
introduced here.

**(d) The module doc now claims only what is proved.** Bullet 2
(`execution.rs:41-52`) separates *restated* from *observed* fields, names
`retained_parent_identity` as the compared one, and states plainly that checking
the restated fields alone "would accept an observation streamed under another
action, which is the defect the Step-3.3 review's [P1-1] found in the first
landing of this file". The `authorize_write` doc (`:200-241`) repeats the
distinction and does not claim the 2.4 seam closes it. The round-1 overclaim is
gone; nothing replaced it with a new one.

**Token shape.** `RetainedWriteAuthorityV1` now carries
`{action, record_id, retained_parent_identity, expected_sha256, goal_sha256}`, so
the round-1 objection — a `{action, record_id}` token forecloses the check being
made downstream — no longer applies. The digest floor is honestly documented as a
floor (`:282-289`): `Missing` facts have no digest, so a removal's goal and a
creating replacement's expected pass on the three checks before it.

### Ruling on the disclosed residue — it does **not** block

The verdict-deciding question was whether consumer-side closure plus a docketed
seam fix suffices for this landing. **It does.** Four reasons, in order of weight:

1. **Remedy 1's rejection is correct on the evidence, not evasive.**
   `observe_streamed_payloads` takes `parent: &RetainedActionDirectoryV1` and
   `namespace: &mut Protocol` where `Protocol::DirectoryHandle = Dir`
   (`authority_record_binding.rs:190-207`). Wiring it into the coordinator means
   handing this owner a `Dir`-bearing capability — and freeze §3.1's forbidden-token
   list for the admission seam names `cap_std` and `&Path` precisely to keep that
   out of this layer. Rejecting remedy 1 protects a frozen property; adopting it
   would have traded one finding for a worse one.
2. **Remedy 2's rejection is correct on scope — and the landed fix is *stronger*
   on the evidence axis.** Remedy 2 would have carried `proof.action()` to the
   seam. But `action` is a caller-supplied *argument* to
   `observe_streamed_payloads` (`:192`), bound to reality only indirectly, via the
   payload slot names derived from it. `retained_parent_identity` is taken from
   the capability itself. The check that landed compares the better field. My
   round-1 remedy 2 was not the strongest option available and the implementer
   found one that is.
3. **The residual gap is unreachable during R2-D.** Re-verified this round:
   `mod execution` is now private to `coordinator` with no re-export (P3-5's
   deletion), `entry.rs` is untouched, `recover_or_create` still has zero
   production callers, and no consumer names any type in the file. A future
   consumer that omits the provenance check is the exposure — and no such consumer
   can exist before R2-E, which is separately reviewed.
4. **The residue is disclosed, docketed, and breadcrumbed at the seam.** The new
   accessor's doc (`protocol/authority_record.rs:195-209`) tells the next author
   at the point of use that this is "the only field a consumer can check an
   observation's *origin* against rather than its caller's restatement of it".
   That is the right place for the warning.

One thing for the settle to weigh on **docket item 8**: as phrased it carries the
streamed proof's *action digest* into `AuthorityObservationFactsV1`. That is the
workable seam-level form — the seam cannot compare directory identity, because a
reservation names none — but its strength rests on slot-name derivation rather
than on direct observation. The settle should adopt it knowing that, and should
consider pairing it with an explicit obligation sentence ("a consumer must check
observed provenance") rather than only an informational one.

## R2.2 [P2-1] — CLOSED

`bootstrap_managed_parents` now returns `Vec<ManagedParentFacadeV1>`
(`execution.rs:181-198`), consuming the rows in the same expression that produces
them. `from_retained_row` and `all` are private `fn` (`:379`, `:389`) — no
visibility modifier — so the only route to a facade is through that method.
Grepped: no `RetainedManagedParentsV1` or `RetainedManagedParentRowV1` value
reaches any caller of the coordinator surface; the two type names appear in
`execution.rs` only as parameter/return types of the two private constructors.
**No path-bearing row escapes.** The clause is now a property of the signature.

**The disclosed gap is accurately scoped, not a dodge.** Two halves of §9
:264-266: "writers receive an opaque proof" is now structural; "may write only
through a facade operation that revalidates that proof" has no enforcement point
because the facade has `revalidate` and no write operation. Verified — that is
simply true of the type, and inventing a write operation here would be exactly
the consumer conversion plan §4 Step 3.3 forbids. Leaving `RetainedManagedParentsV1`
and `row.path()` as they are is also right: the provider computes and re-proves
those paths for `revalidate`, and that surface is the provider's own, landed at
3.1, not this step's to narrow.

## R2.3 The five P3s — all closed

| # | Verification |
| --- | --- |
| P3-1 | Report now states single-axis with automatic escalation. Freeze citation re-checked: §9 :1329-1332 — "Everything else is **single-axis with automatic escalation on any P0/P1/P2**: … Phase 3 steps 3.1-3.3 **including the Phase 3 settle** … This supersedes the plan §4 gate lines that assign a dual at the Phase 3 and Phase 4 settles". Correct as now stated. |
| P3-2 | Both comments repointed. `namespace/tests_fault_matrix.rs:27-34` now says resuming the handoff is "**not owned by any landed step**", why 3.3 declined, and that it is docket item 6. `bootstrap/managed/tests_provider.rs:228-231` carries the matching text. No comment now names a closed step as a future owner. |
| P3-3 | `RetainedWriteAuthorityV1` is `#[derive(Debug, Eq, PartialEq)]` — no `Copy`, no `Clone`; `RevalidatedManagedParentV1` likewise. Accessors are `&self`. The doc claims match the derives. |
| P3-4 | `the_pairing_gate_refuses_each_varied_reservation_input` (`:301-358`): three rows — other action digest, stale schedule for the same action, other owner binding. **Attribution claim verified**: every row passes `&admitted` as `streamed_under`, so provenance matches and only the pairing can fire; each asserts the exact detail string. Independently corroborated — in my scratch run with the provenance check deleted, this test still passed, which is only possible if its refusals come from the pairing gate. The honest note that a strictly one-*field* variation is unconstructible (the record digest covers all three inputs) is correct. |
| P3-5 | `pub use execution::*` and its allow are gone; `mod execution` is private with a doc explaining why, and `tests_execution.rs:19-22` names `super::execution::…` directly. |

## R2.4 New this round — two P3s, file and continue

- **[P3-6] The suite's highest-value row is the one not pinned to its gate.**
  `an_observation_streamed_under_another_action_must_not_authorize` asserts
  `is_err()` only, while the [P3-4] rows beside it assert their detail strings.
  Attribution is sound by construction and by my scratch run, but the row that
  exists to prove *which* gate fires does not say so, and a future edit that made
  some earlier check fire first would leave it green for the wrong reason. One
  line: match `CheckedFsError::Ambiguous { detail, .. }` and assert
  `"the authority observation was streamed under another action's retained directory"`.
- **[P3-7] Stranded doc line.** `leaf_digests`'s doc comment
  (`tests_execution.rs:53-56`) opens with `leaf_request`'s old first sentence
  ("A leaf request of the given operation, over a fixed workspace-relative path.")
  before its own. Cosmetic, but this step's P3 set was about comment accuracy.

## R2.5 Gates — all re-run in the live tree

```
$ cargo clean -p gwz-core                            Removed 644 files, 1.2GiB
$ CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings
    Checking gwz-core v0.10.4 … Finished in 12.78s              EXIT 0
$ cargo check --all-targets                          Finished in 9.34s
$ cargo test --lib checked_artifact::
    369 passed; 0 failed; 1170 filtered out; 34.90s             [367 → 369, +2]
$ cargo test --lib checked_artifact::coordinator::tests_execution
    9 passed; 0 failed                                          [7 → 9]
$ cargo test --lib checked_artifact::bootstrap::managed     30 passed; 0 failed
$ cargo test --lib checked_artifact::namespace::            22 passed; 0 failed
$ rustfmt --edition 2024 --check <all 11 dirty files>        EXIT 0
$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: failed
- protected source tree changed: checked_artifact/capability/pre_catalog.rs
EXIT 1                                                       [exactly one finding]
```

**Checker attribution re-constructed after the object grew.** `source_tree_digest`
re-run over base `7169d89` and live for all four protected trees: base matches all
four pins, live diverges on `capability/pre_catalog.rs` alone — unchanged from
round 1, still the allow-reason correction. The new `protocol/authority_record.rs`
edit draws no finding because `protocol/` carries no tree pin, no source-allowlist
pin, and no raw-rename allowlist entry; verified by grepping the checker's pin
sets, not merely by the exit line.

**Publication seam unmoved:** `publish_verified_no_replace(` in `src/` = **21**;
`capability_permit.rs:139` still asserts **13**, green inside the 369.

**`protocol/authority_record.rs` is additive only.** The whole diff is one
`const fn retained_parent_identity(&self) -> &DurableObjectIdentityV1` plus its
doc. No field, no constructor, no validation, no encode/decode path, no seam
behaviour changed — so the implementer's "remedy 2 is a seam change I am not
making in round 2" is consistent with what landed: a read accessor is not a seam
change. Zero new durable records; plan §4's Track-W rule untouched.

**LOC, recounted:**

```
coordinator/execution.rs        463 lines   239 code   (196 → 239, +43)
coordinator/tests_execution.rs  472 lines   363 code   (252 → 363, +111)
9 modified files                +132/−45     42 added code lines
                                       STEP TOTAL 644 code   (round 1: 490)
```

**Load-bearing judgment on the 644 vs <500.** Recorded as an overrun and
**accepted**. Three reasons. (i) The plan's own text makes <500 aspirational —
"a target, not a hard limit" (§4 :230-232) — with the ChangeBudget stop-rules
(`GwzM5-8ChangeBudget.md:16-22`) as the real gate. (ii) The overrun is evidence,
not scope: of round 2's +154, **111 is test code** and the production delta is +43
— the provenance check, the digest floor, three token fields with accessors, and
the facade-set return type. Production code alone stands at 239. (iii) The test
mass is the P1 red/green row and the three granular pairing rows — that is,
exactly the evidence this review demanded in round 1. Refusing the mass would
have meant refusing the proof. It should be recorded in the ChangeBudget ledger
as `644 = 239 production + 363 test + 42 modified`, with "round-2 P1 remediation
evidence" as the reason, so the ledger shows why rather than only how much.

## R2.6 The settle docket — eight items, and they are accurate

Items 1-7 were verified against the code in §9 of round 1 and are unchanged.
Item 6 is now referenced by both repointed comments, so the docket and the tree
agree. Item 8 is new and correctly stated:

> **8.** Carry the streamed proof's action digest into `AuthorityObservationFactsV1`
> so the 2.4 seam itself refuses a mis-paired observation, rather than relying on
> each consumer to check observed provenance.

That is a true statement of a real residual, with the qualification in R2.1 above
about which field the settle should choose. Round-1's [P1-1] and [P2-1] are
**closed rather than docketed**, correctly — a closed finding does not belong on a
settle docket. Two new P3s (R2.4) are review findings that file and continue under
freeze §9; they are not docket items and should not inflate it to ten.

**The docket stands at eight, and every item on it is true of the tree.**

## R2.7 Escalation status

The round-1 trigger (one P1, one P2) stands as recorded — that history should not
be erased, because it is why the provenance gate exists. Both findings are now
closed by remediation, which is the trigger's intended resolution. **No P0, P1 or
P2 stands at the close of round 2, so nothing is escalated out of this lane.** The
two-round cap is satisfied without reaching a third round, and no third new
architectural root cause arose (§4.1's redesign-or-accept clause is not engaged).

## R2.8 Verification appendix — round 2

```
git -C gwz-core status --porcelain       9 ' M' + 2 '??' = 11, base 7169d89
git -C gwz-core diff -- protocol/authority_record.rs     one accessor, +18
rsync workspace → scratch, delete the 5-line provenance check, re-run suite
                                         → the [P1-1] row FAILS, 8 others pass
                                           [round-1 hole reproduced first-hand]
source_tree_digest, base vs live, 4 protected trees      1 divergence (§R2.5)
grep checker pin sets for protocol/authority_record.rs   absent from all
publish_verified_no_replace( in src/     21   | capability_permit.rs:139 → 13
RootEntryNameV1::ActiveAction name        ACTION_PREFIX + action.hex() + suffix
observe_streamed_payloads provenance      parent.identity().clone()  [observed]
LOC recount, non-blank non-comment        239 + 363 + 42 = 644
```

No git write operation was performed in either round; every git call was
`status`/`log`/`diff`/`archive`. Both `cargo clean -p gwz-core` invocations and
the scratch red-reproduction touched only scratchpad directories — the live tree's
eleven-file dirty set was re-checked after the reproduction and is byte-identical
to what the implementer left. The only file this review has written, in either
round, is this document.
