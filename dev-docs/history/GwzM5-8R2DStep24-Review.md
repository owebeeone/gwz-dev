# R2-D Step 2.4 — authority parse / streamed proof split — Code-axis review

- **Date:** 2026-08-22
- **Axis:** single-axis, **Code-lead** (adopted plan §4 Phase-2 note: "Code-axis
  first for 2.1/2.4"). Round 1 of a two-round cap.
- **Object:** working tree over `origin/main` = `6c7c8f3` ("Open interior slot
  directories with delete sharing on Windows (matrix os-32 residue)") in the
  `gwz-core` member repository.
- **Escalation:** auto-escalation on any P0/P1/P2 (plan §4 Phase-2 gate line).
  Round 1 **RECORDED** the trigger for the lane owner to route (§5); round 2
  **DISCHARGES** it (§7). This review makes no assumption that a second axis
  runs.
- **Rounds:** round 1 NO-GO (§5, one P0 + one P1 + one P2); round 2 **GO** (§7,
  all three closed). Two-round cap now spent.
- **Final verdict: GO** — see §7 for the [P0-P3] grade table.

## 0. Object, and how it was isolated

The reviewed object is:

| File | Shape |
| --- | --- |
| `src/checked_artifact/protocol/authority_record.rs` | +46 code |
| `src/checked_artifact/protocol/authority_record/owner.rs` | +9 code |
| `src/checked_artifact/protocol/authority_record/owner/host.rs` | new, 55 code |
| `src/checked_artifact/protocol/authority_record/owner/provider_compile.rs` | **deleted**, −46 code |
| `src/checked_artifact/capability/pre_catalog/provider/authority_record_binding.rs` | new, 461 code |
| `src/checked_artifact/capability/pre_catalog/provider/tests_authority_record.rs` | new, 449 lines |
| `src/checked_artifact/capability/pre_catalog/provider/tests_authority_record_matrix.rs` | new, 553 lines |
| `src/checked_artifact/capability/pre_catalog/provider.rs` | +17 (object's share) |
| `src/checked_artifact/interface_tests/fault_expected_keys.rs` | the `record` region only |
| `scripts/checks/check_checked_artifact_boundaries.py` | the `authority_record_binding.rs: 2` entry |

**Tree caution honoured.** The live tree carries 63 changed/untracked files
across three lanes (Step 2.4, the parallel Step 2.3 managed-namespace lane, and
merge-side work). Per the implementer's own method an **attributable scratch
copy** was assembled rather than gating the live tree: `git archive 6c7c8f3`
extracted to a clean directory, the object's six source files copied verbatim
from the working tree, `provider_compile.rs` deleted, and the three *shared*
files (`provider.rs`, `fault_expected_keys.rs`,
`check_checked_artifact_boundaries.py`) reconstructed as **base + the object's
edits only**, with Step 2.3's `managed_mutation` module declaration, re-exports,
`PartiallyExecuted` variant, `names_variant`/`sites_naming` helpers and
`pre_catalog.rs` digest bump excluded. Lane attribution was established by
reading each hunk, not assumed.

Two attribution facts worth recording:

- `src/checked_artifact/capability/pre_catalog.rs` and
  `src/checked_artifact/interface_tests/capability_permit.rs` are **entirely**
  Step 2.3's; the object needs neither.
- The `PROTECTED_SOURCE_TREE_DIGESTS` bump for `pre_catalog.rs` in the boundaries
  script is Step 2.3's, not the object's. Only the
  `CATALOG_PUBLICATION_CALL_COUNTS` entry belongs here.

**Frozen seams confirmed untouched by the object**: `leaf.rs`,
`capability/pre_catalog/provider/leaf_observation.rs`, `protocol/codec.rs` and
`interface_tests/r2d_seam_freeze.rs` all show an empty diff against `6c7c8f3`.
Freeze §3.3 is therefore genuinely read-only to this object, and the object
consumes the shared `read_bounded_bytes` primitive without altering it.

---

## 1. Findings

### [P0-1] The `cfg(windows)` arm of `require_authority_strength` cannot compile — the headline E9 property is unbuildable on Windows

`authority_record_binding.rs:267-279`:

```rust
#[cfg(windows)]
const fn require_authority_strength(
    writer_class: ObservedLeafWriterClassV1,
    fact: &'static str,
) -> Result<(), CheckedFsError> {
    match writer_class {
        ObservedLeafWriterClassV1::GwzWritten => Ok(()),
        ObservedLeafWriterClassV1::Foreign => Err(CheckedFsError::ambiguous(
            fact,
            "a foreign-written leaf's durable proof is namespace ordering only on this platform",
        )),
    }
}
```

The function is declared `const fn`. `CheckedFsError::ambiguous` is **not**
const — `capability.rs:78`:

```rust
    pub(super) fn ambiguous(fact: &'static str, detail: impl Into<String>) -> Self {
```

It takes `impl Into<String>` and allocates a `String`. A `const fn` body is
const-checked at definition time regardless of call context, so this is a hard
compile error on every Windows build.

**Proven in-crate, not merely by inspection.** A second scratch copy was made
with *only* the two `cfg` attributes on this one function swapped, so the
Windows arm compiles on this host. `cargo check --lib` output:

```
error[E0015]: cannot call non-const associated function `capability::CheckedFsError::ambiguous::<&str>` in constant functions
   --> src/checked_artifact/capability/pre_catalog/provider/authority_record_binding.rs:274:51
error: could not compile `gwz-core` (lib) due to 1 previous error
```

The line and column are the object's own. The `cfg(not(windows))` sibling at
`:281-287` is a plain `fn`, which is why every local gate is green and why
nothing on this host can see the defect.

**Why this is P0 rather than P1.** This is the review's designated headline. The
freeze §4.3 E9 activation annotation assigns the writer-class condition to this
exact caller — "Step 2.4's production caller binding must carry this condition"
— and the whole of the object's claim to carry it "in code, because the nominal
case is a claim rather than a proof" (`authority_record_binding.rs:38-43`) rests
on this function. The consequences compound:

1. `gwz-core` does not build on Windows with this object applied. Every Windows
   matrix run, every `record.*` row on a Windows target, and the M5-8 Windows
   matrix generally are blocked, not merely unverified.
2. The arm test `the_e9_writer_class_condition_is_carried_on_this_platform`
   (`tests_authority_record.rs:339-365`) is written correctly — it does assert
   whichever arm runs, with `#[cfg(windows)] foreign.expect_err(...)` at `:360`
   — but it can never execute, because the crate it lives in will not compile
   there. It therefore reads as Windows coverage while providing none. That is
   worse than an absent test.
3. The Foreign-refusal property that the annotation exists to secure is, on the
   only platform where it matters, not merely unproven but unreachable.

**Minimal remediation.** Drop `const` from the `cfg(windows)` arm so it matches
its `cfg(not(windows))` sibling:

```rust
#[cfg(windows)]
fn require_authority_strength(
```

Then re-run the Windows matrix — this arm has never been compiled, so its first
green run is the evidence, and the `record.*` Windows rows must be re-recorded
rather than carried over.

**Process note for the lane owner.** No local gate can catch this class of
defect. A `cargo check` for a Windows target (or a cfg-flip smoke check) over
any `cfg(windows)`-only code introduced by an R2-D step would have caught it at
authoring time. Steps 2.2, 2.3 and Phase 4 all add `cfg(windows)` arms; this is
worth a standing check rather than a one-off fix.

---

### [P1-1] The retained parent and the streamed proof are independent arguments — the coherence property the seam exists to enforce is documented but not constructed

`authority_record_binding.rs:305-318`:

```rust
impl AuthorityTransactionV1 {
    pub(in crate::checked_artifact) fn from_streamed_proof(
        parent: &RetainedActionDirectoryV1,
        request_owner_binding: RequestOwnerBindingV1,
        proof: StreamedPayloadProofV1,
    ) -> Self {
        Self {
            request_owner_binding,
            artifact_root: parent.path_profile().clone(),
            retained_parent_identity: parent.identity().clone(),
            proof,
        }
    }
}
```

`parent` and `proof` are **separate parameters**, and nothing relates them.
`StreamedPayloadProofV1` (`:110-117`) records `source`, `expected_sha256`,
`goal_sha256`, `source_length`, `goal_length` — and **no parent identity and no
path profile**. It also derives `Clone`. So an in-crate caller may stream both
payloads under retained parent *A* and then build the transaction with parent
*B*, yielding an observation whose `artifact_root` and
`retained_parent_identity` are B's while its source fingerprint and both digests
are A's.

The chain accepts this end to end: the record issues from that observation,
`matches_observation` passes (it is the same observation),
`install_authority_record` writes into B, and `validate_terminal_relation`
passes (it is the same proof). Nothing anywhere compares the proof's provenance
to the parent's identity.

This is precisely the property the seam was built to deliver.
`host.rs:21-23` states it as the file's justification:

> A consumer therefore still cannot assemble an observation from a path, a
> parent and a source it looked up separately
> (`GwzM5-8R4bR2ConsumerCheckpoint.md` §8 :239-240, §14 first bullet).

And `authority_record_binding.rs:291-297` restates it as achieved:

> the artifact root and the retained parent identity are read **out of the
> retained capability** the payloads were streamed through, never accepted from
> a caller

The first half of that sentence is true — the two facts *are* read out of a
retained capability rather than from loose caller values, which is a real
improvement over the R1 placeholder. The second half is not: nothing establishes
that it is *the* capability the payloads were streamed through. The doc asserts
a construction guarantee where the code provides a calling convention.

**Scope, honestly stated.** This is not exploitable today: the binding has no
production consumer (that is Step 3.3), and both in-repo call sites — the test
fixture at `tests_authority_record.rs:177-182` and the matrix driver at
`tests_authority_record_matrix.rs:304-309` — pass the same `parent` they
streamed through. The finding is that the invariant is upheld by discipline
rather than by type, in the one file whose entire purpose is to make that
invariant structural, and immediately before Step 3.3 builds a coordinator on
top of it. The R1 seam was frozen with an unnameable facts type and a
call-scoped issuer specifically so that coherence would not depend on callers
being careful; leaving the parent unbound reintroduces at the transaction
boundary exactly the class of mistake the issuer eliminated at the facts
boundary.

**Minimal remediation.** Carry the two capability-derived facts inside the proof
that only `observe_streamed_payloads` can mint, and drop the `parent` parameter:

```rust
pub(in crate::checked_artifact) struct StreamedPayloadProofV1 {
    artifact_root: CanonicalPathIdentityV1,
    retained_parent_identity: DurableObjectIdentityV1,
    source: DurableLeafFingerprintV1,
    // ... unchanged
}
```

populated in `observe_streamed_payloads` from the `parent` it already holds
(`:148-196`), with

```rust
pub(in crate::checked_artifact) fn from_streamed_proof(
    request_owner_binding: RequestOwnerBindingV1,
    proof: StreamedPayloadProofV1,
) -> Self
```

This makes the mismatch unrepresentable rather than merely unattempted, costs
roughly fifteen lines, and lets `host.rs:21-23` and `:291-297` be read literally.

---

### [P2-1] `REPEATABLE_BOUNDARIES` is under-populated against its own stated criterion, and the exclusion rationale accounts for two of nine exclusions

`tests_authority_record_matrix.rs:81-103` states the criterion and the set:

> A boundary is re-crossable when the crash leaves the cycle in a window the
> next resolution re-enters at the same point. [...] The other three are
> read-only boundaries reached with the active record resident, so the crash
> mutates nothing and the next resolution reaches them again.
>
> The install and retire *edges* (`active_publish`, `retire_exact`) are
> deliberately absent: each consumes the name it moves [...]

```rust
const REPEATABLE_BOUNDARIES: [Fault; 4] = [
    Fault::RecordScratchCreate,
    Fault::RecordBoundedRead,
    Fault::RecordTerminalRelationValidate,
    Fault::RecordRetirementReserve,
];
```

Thirteen boundaries, four included, **nine** excluded — and the doc names two of
the nine. Tracing `attempt` (`:271-329`) against the one-shot fault harness
(`fault_v1.rs:212-220`, `run_next_at` arms exactly one key; `hit` takes it):

- Genuinely **not** re-crossable: `active_publish`, `active_reobserve` (both
  leave the active slot resident, so the next `attempt` takes the
  `if !action_root.join(...Authority).exists()` branch at `:315-323` and skips
  the install entirely) and `retire_exact`, `retired_reobserve` (both leave the
  retired alias resident, so the next `attempt` returns `Ok(())` at the
  early-exit at `:283-288`). That is **four**, not two — the doc names only the
  two rename edges and is silent on the two reobservations.
- Re-crossable by the doc's own criterion but **excluded without a reason**:
  `RecordScratchWrite` and `RecordScratchFlush` (the resumed install re-enters
  `write_authority_scratch`, which is `create(true)` plus `set_len(0)` over a
  compile-time slot name at `:412-422` — exactly the rationale given for
  including `scratch_create`), and `RecordDecode`,
  `RecordCanonicalReencode`, `RecordBindingValidate` (all three are read-only
  boundaries reached with the active record resident — verbatim the rationale
  given for including `bounded_read`, and they sit in the same
  `read_and_bind_authority_record` call the included `bounded_read` sits in).

So five boundaries meet the stated inclusion test and are absent, and the stated
exclusion argument covers two of the four genuine exclusions. The repeated-crash
evidence is 8 rows (4 boundaries x 2 target variants x 12 rounds) where the
criterion as written supports 18 (9 x 2), and the reader cannot tell from the
comment which of the five was a judgement call and which an oversight.

Nothing here is a production-code defect and the 26 interruption/restart rows
are unaffected. It is graded P2 because per-boundary repeated-crash stability is
the evidence the frozen map's RemPlan §10 duty is discharged with, and because a
taxonomy whose comment and whose constant disagree will be copied forward by
Phase 3 and Phase 4 the way this one was copied from Phase 1.

**Minimal remediation.** Either add the five to `REPEATABLE_BOUNDARIES` and
re-record the repeat rows (the driver at `:487-527` is already general and needs
no change), or keep the four and extend the doc-comment to state, per excluded
boundary, which of the two reasons applies — naming `active_reobserve` and
`retired_reobserve` alongside the two rename edges, and giving the positive
reason the other five are held back.

---

### [P3-1] `payload_lengths()`'s doc-comment names a consumer that does not consume it

`authority_record_binding.rs:132-136`:

```rust
    /// The streamed payload lengths, which the terminal relation compares
    /// against the record bound rather than against each other.
    pub(in crate::checked_artifact) const fn payload_lengths(&self) -> (u64, u64) {
```

`validate_terminal_relation` (`:350-375`) never calls `payload_lengths`. Its
only caller is the acceptance test at `tests_authority_record.rs:207`. The
related claim in the function's own doc — "3. **The payloads were never treated
as record-sized.** The relation holds with payload lengths on either side of the
record bound" (`:347-349`) — is likewise demonstrated by that test, not proved
by the function, though the function's preamble says it "proves three things".

Remediation: reword to "which the Step 2.4 acceptance test compares against the
record bound", and soften the function preamble from "proves three things" to
two proved here plus one the acceptance test establishes.

### [P3-2] The second guard in `validate_terminal_relation` is unreachable

`authority_record_binding.rs:364-369`:

```rust
    if bound.record_bytes() > ProtocolRecordKindV1::Authority.max_bytes() {
```

`record_bytes()` is `bytes.len()` from `read_and_bind_authority_record`
(`authority_record.rs:371`), and `read_bounded_bytes` already returns
`Oversize { limit }` whenever `bytes.len() > limit` (`codec.rs:180-182`). A
`BoundCheckedAuthorityRecordV1` past the bound cannot exist, so this branch is
dead. It is harmless defence-in-depth, but it is presented as proof obligation
2 of 3, which overstates what the join contributes.

Remediation: keep the check and mark it as a redundant invariant assertion, or
state in the doc that it restates a bound the parse already enforced.

### [P3-3] The `dead_code` reason misdescribes what plan §4 Step 3.3 does

`provider.rs:14-18` and the matching import allows in `authority_record.rs:50-53`
and `owner.rs:5-8` all read "plan §4 Step 3.3 wires its production consumer".
Plan §4 Step 3.3 (`GwzM5-8R2D-Plan.md:384-385`) says, in bold:

> **This step wires machinery; it does not convert consumers** (that is R2-E).

The distinction is load-bearing in this program. Remediation: "plan §4 Step 3.3
wires this binding into the coordinator; R2-E converts consumers".

### [P3-4] `observed_record` privately re-implements the shared bounded read

`authority_record_binding.rs:599-626` hand-rolls `try_reserve_exact(limit)` +
`take((limit + 1) as u64)` + `len() > limit`, duplicating
`read_bounded_bytes_inner` (`codec.rs:167-184`) — in the same package whose
parse owner explicitly praises using "the same shared `read_bounded_bytes`
primitive, not a private re-implementation of it"
(`authority_record.rs:325-327`).

The duplication is **forced, not careless**: `read_bounded_bytes` is
`pub(in crate::checked_artifact)` only under `#[cfg(test)]` and `pub(super)`
under `#[cfg(not(test))]` (`codec.rs:152-165`), so in a production build it is
unreachable from `capability::pre_catalog::provider`. Recording it because the
two copies can drift silently and only the non-test build would notice.

Remediation (Step 3.3 or a follow-up, not necessarily here): widen the non-test
visibility of `read_bounded_bytes` to `pub(in crate::checked_artifact)` and have
`observed_record` call it.

---

## 2. Attack results

### (1) The join at `validate_terminal_relation` — PASS, with [P3-1]/[P3-2]

The record/proof binding is **complete**. `:355-357` compares the source
`DurableLeafFingerprintV1` (identity + length + sha256), `expected_sha256` and
`goal_sha256` field by field. The goal leaf's identity and length are not
compared, but `goal_sha256` is, and a sha256 match determines content, so no gap
follows. A mismatched pair cannot pass: the negative case is executed at
`tests_authority_record.rs:296-303`, which takes the relation against a
*different action's* proof and requires `expect_err`, then re-proves the true
relation still holds.

The record-bound assertion is sound but tautological — [P3-2].

**The >1 MiB acceptance test genuinely streams.** `tests_authority_record.rs:46-47`
sets the default payloads to `1024 * 1024 + 37` and `1024 * 1024 + 111`, so every
test in the file carries the ConsumerCheckpoint §12 row rather than one test
carrying it. The headline test
`the_bounded_record_and_the_streamed_payloads_are_separate_budgets` (`:194-233`)
asserts, in order: the encoded record is within the frozen bound (`:202-205`);
both payload lengths exceed the record bound (`:208-212`); both exceed one MiB
(`:213-216`); and — the part that makes it streaming rather than merely large —
`assert_eq!((source.opens(), goal.opens()), (2, 2), "each payload is opened
twice by the observer and never rewound")` at `:167-171`, plus
`namespace.crossed() == 2` at `:172-176`. Two opens with no rewind is the
`ExpectedLeafContent` contract (`leaf.rs:57-63`) being exercised, not asserted.
Finally `bound.record_bytes() == encoded.len()` (`:227-231`) ties the accepted
byte count to the installed record.

No payload sizing leaks into the parse owner: `authority_record.rs` names no
payload, no leaf and no filesystem type, and its only budget expression is
`ProtocolRecordKindV1::Authority.max_bytes()`.

`read_bounded_bytes` limit+1 semantics confirmed at `codec.rs:167-184`:
`take(limit + 1)` then `if bytes.len() > limit { Oversize }`, with
`Authority.max_bytes()` = `16 * 1024` (`codec.rs:56-62`). The inlined four-stage
parse is behaviourally equivalent to the `read_bounded_record_inner` it replaces
(`codec.rs:107-118`): `decode_record` delegates to `decode_canonical`
(`authority_record.rs:290-292`) and `KIND` is `Authority` (`:284`). The only
behavioural difference is the error string, now "authority record is not
canonically encoded". `read_bounded_record_inner` retains eight other callers,
so nothing was orphaned.

### (2) The sealed owner seam — PASS

- `AuthorityObservationFactsV1` is declared bare `struct` at `owner.rs:23`, so it
  is private to the owner module and unnameable outside it. `host.rs` reaches it
  only via `use super::{...}`.
- Facts are mintable only inside `observe_retained_request`.
  `RetainedAuthorityFactsV1` (`host.rs:36`) is a tuple struct with a **private**
  field; `AuthorityFactsIssuerV1` (`:43-45`) has a private `_sealed: ()` field.
  Neither is constructible outside `host.rs`. The sole issuer instance is the
  temporary built at `:95` inside the adapter's own
  `observe_retained_request`, handed out as a borrow that no implementor can
  store (the method takes `&self` and the issuer's lifetime is unconstrained by
  `Self`), and it is neither `Clone` nor `Copy`. The module doc's three claims at
  `:10-19` are all delivered.
- `AuthorityTransactionV1` reads identity from the retained capability, never a
  caller — `parent.path_profile()` and `parent.identity()` at `:313-314`.
  `request_owner_binding` does come from the caller, which is correct: it is the
  request's identity, not the artifact's. **However**, which retained capability
  is not constrained — see [P1-1].
- The deleted placeholder leaves **no orphan references**: `grep -rn
  "provider_compile\|ProductionShapedAuthorityProvider"` over `src/`, `scripts/`
  and `dev-docs/` returns only the unrelated, still-present
  `namespace/provider_compile.rs` and
  `protocol/infrastructure_record/owner/provider_compile.rs` and their pins in
  `interface_tests/r2d_seam_freeze.rs:67-76`. Nothing referenced the authority
  one.

### (3) Matrix substance — PASS, with [P2-1]

- **26 rows, real process stops.** `RECORD_MATRIX` is 13 boundaries
  (`:60-74`) x 2 frozen target variants (`Workspace`, `GitDirectory`, `:105-116`)
  driven by `run_interruption_matrix` (`:447-485`). Each row arms a one-shot
  fault that panics, runs `attempt` under `catch_unwind`, and **asserts the fault
  point was actually reached** (`interrupted.is_err()`, `:464-468`) — so a row
  that silently stopped injecting fails rather than passing. `attempt`
  (`:271-329`) is a genuine fresh resolution derived from resident state alone:
  retired alias present means settled, active record present resumes at the
  parse, anything else installs first.
- **Censuses converge.** `settle` then `assert_settled` then a second `settle`,
  with `assert_eq!(first, second)` (`:470-483`). The settled predicate
  (`:385-420`) is substantive, not a smoke check: retired alias resident, active
  slot free, write-ahead scratch consumed, and **both payload leaves byte-length
  unchanged** — the last being the property that makes "the record cycle never
  touched the payloads" checkable.
- **Reconciliation is machine-checked.** `reconcile_executed_keys` (`:427-441`)
  compares `RECORD_MATRIX` against `Fault::all()` filtered on `record.`, so a
  fourteenth key could not be added without a row.
- **The 8 repeated-boundary rows** are 4 boundaries x 2 variants x
  `REPEATED_CRASH_ROUNDS = 12` (`:79`), asserting slot-name-set stability across
  rounds and settling afterwards (`:487-527`). The rows themselves are sound;
  the **taxonomy** behind the set is [P2-1].

### (4) The 13 sites' placement — PASS

All thirteen `fault_v1::hit` calls sit **immediately after** the edge they
announce, guarded `#[cfg(test)]`, matching the house crash-after-edge idiom so a
restart observes the edge as done. Four in `authority_record.rs` (after
`read_bounded_bytes?`, after `decode_canonical?`, after the re-encode
comparison, after the reservation/observation check) and nine in
`authority_record_binding.rs` (`:418` after `open_with?`, `:426` after
`write_all?`, `:439` after `sync_directory_edge?`, `:467` after
`publish_verified_no_replace?`, `:484` after the reobserve `file_identity?`,
`:371` after both terminal comparisons, `:538` after the reserve check, `:551`
after the retire `publish_verified_no_replace?`, `:569` after the retired
reobserve). 4 + 9 = 13, matching freeze §3.5 exactly.

### (5) The integration finding — REAL, correctly scoped, precedent matched

The finding is real and independently reproduced. The consumer-visible minting
route `ActionNamespace::retained_parent` (`namespace/host.rs:322-326`) returns
`RetainedDirectory<ActionNamespaceHandleV1, ...>`, where
`ActionNamespaceHandleV1` is an opaque `[u8; 32]` token (`:61`). The object needs
`RetainedActionDirectoryV1 = RetainedDirectory<Dir, ...>`
(`authority_record_binding.rs:85-86`) because it hands `parent.handle()` to
`cap_std` operations. The only `Dir`-carrying route,
`namespace_mutation::retain_action_namespace` (`:135`), is `pub(super)` — visible
only inside `pre_catalog::provider`. The binding therefore cannot be wired from
the namespace owner today; that is a genuine type-level blocker, not a
convenience deferral.

Scoping to Step 3.3 is correct: plan §4 Step 3.3 is "coordinator execution glue
— schedule + `AdmittedActionV1` binding" (`GwzM5-8R2D-Plan.md:379-383`), which is
exactly where an admitted action is turned into a retained action directory. The
`dead_code` posture matches the 2.1/2.2 precedent — a module-level
`#[allow(dead_code, reason = ...)]` naming the step that wires it, and
`#[allow(unused_imports, reason = ...)]` on the re-export chain, the same shape
Step 2.1 used for `leaf_observation` and Step 2.2 for its namespace re-exports.
Step 2.1's allow is correctly *updated* rather than left stale
(`provider.rs:19-26`): Step 2.4 really is `leaf_observation`'s production caller
(`authority_record_binding.rs:224`), and the honest reason the allow survives is
that the caller is itself unwired. Only the wording is off — [P3-3].

### (6) TDD honesty note and `retain_action` — PASS

`retain_action` (`tests_authority_record.rs:83-103`) is **production-shaped and
test-only**. It lives in a `tests_`-prefixed file, which the file header at
`:10-13` correctly notes keeps it out of both `production_rust_files` in the
boundaries checker and the injection-site rescan in `fault_expected_keys.rs`. It
builds the capability the way the production namespace owner does — a **bound**
`CanonicalComponent` carrying the live parent's durable identity, invocation
identity, parent mode and rename domain, all read through the real
`HostPlatform` provider (`:89-101`) — rather than the bare component the Step 2.1
harness uses, because `DurablePathV1::from_live` accepts only bound components
and the capability's path profile becomes the record's `artifact_root`. It
terminates in the pre-existing `namespace::test_support::retained_directory`,
which is present at `6c7c8f3` (`namespace/mod.rs:20`) and was not added by this
object.

**It adds no authority leak**: it names no authority type, cannot construct
`StreamedPayloadProofV1` (private fields, and it is in a different module), and
cannot construct `AuthorityFactsIssuerV1` or `RetainedAuthorityFactsV1`. The
seam is reached only through the same public-to-the-crate entry points a
production caller would use.

The seam-plumbing-before-behavioural-tests admission is consistent with what the
tree shows and is not concealed.

### (7) LOC — arithmetic verified, structural cause genuine

Code-only counts (blank and comment lines stripped), measured rather than taken
from the note:

| | code LOC |
| --- | --- |
| `authority_record_binding.rs` (new) | +461 |
| `host.rs` (new) | +55 |
| `authority_record.rs` | +46 / −4 |
| `owner.rs` | +9 |
| `provider.rs` (object's share) | ~+17 |
| `provider_compile.rs` (deleted) | −46 |
| **net** | **~538** |

The "~544 net" figure is confirmed to within counting convention (whether the
four removed lines in `authority_record.rs` are netted, and whether
`provider.rs` is counted raw or code-only). It is over the aspirational <500 by
roughly 8%.

**The structural cause is real and not padding.** Plan §4 Step 2.4's prose
describes only the parse/proof split, but freeze §3.5 assigns **all thirteen**
`record.*` keys to Step 2.4, and RemPlan §10's duty requires a family to "gain
injection sites and matrix rows in the same package that converts its edges".
Nine of those thirteen are the record's durable install/retire lifecycle. The
step therefore cannot land only the parse split without either leaving nine keys
unsited or activating keys it has not converted — both worse. The binding file
alone (461) is 92% of the budget before the parse owner and the host seam are
counted. The overage is a consequence of the frozen activation map, is
attributable, and is within the "target to aim for, not a hard limit" reading.
No finding is raised.

---

## 3. Contract conformance

Assessed at round 1; the E9 row is updated inline with its round-2 outcome. All
other rows were met at round 1 and are untouched by the remediation.

| Contract | Verdict |
| --- | --- |
| Plan §4 Step 2.4 — bounded parse split from streamed proof | Met. |
| Freeze §3.3 — `LeafObserver` seam read-only | Met; `leaf.rs` and `leaf_observation.rs` diff empty against `6c7c8f3`. |
| Freeze §4.3 E9 annotation — writer-class condition carried | Round 1: **not met on Windows — [P0-1]**; carried in doc form and, on unix, in code, but the Windows arm did not compile. `ObservedLeafWriterClassV1` **is** a required, no-default argument on every entry point with no `Default` impl — that half was always met. **Round 2: MET** — one always-compiled body, Windows behaviour executed under a forced constant (§6). |
| ConsumerCheckpoint §8 :232-237 — 16 KiB bounded parse never the streamed path | Met; `limit + 1` semantics confirmed, no payload sizing in the parse owner. |
| Freeze §3.5 — `record.*` 13 keys, none minted | Met. Recounted: 13 keys at `fault_expected_keys.rs:70-82`; 4 sites in `authority_record.rs` + 9 in `authority_record_binding.rs` = 13. **No key minted** — the first hunk of the fixture's diff starts at line 187, so the key list and `EXPECTED_KEY_COUNT = 165` are untouched. |
| `CATALOG_PUBLICATION_CALL_COUNTS` entry = 2 | **Count verified correct.** Exactly two `publish_verified_no_replace(` call sites in the binding (`:457` install, `:541` retire), both at line-start indentation, which is what the script's `(?m)^\s*publish_verified_no_replace\s*\(` regex counts after `mask_non_code` (`check_checked_artifact_boundaries.py:860-866`). Pin ownership remains the lane owner's. |

---

## 4. Gates

Run on the attributable scratch copy, `CARGO_TARGET_DIR` set to a scratch target.

Two runs are recorded: (a) an independent pre-remediation run by the lane
owner's gate agent on a scratch of the round-1 object, and (b) a
post-remediation run on the implementer's attributable scratch tree at
`scratchpad/p24-verify/gwz-core`. The load-bearing gates in (b) were **re-run by
this reviewer directly**, not accepted as relayed; the actuals below are what
this reviewer observed.

**Attributability of the post-remediation tree — verified before use.** All six
Step-2.4 source files in `p24-verify/gwz-core` were `diff`ed against the live
tree and are **byte-identical**; `provider_compile.rs` is absent in both. The
live tree is not gateable as a union because the in-flight 2.3 lane shares it.

### (a) Pre-remediation, independent run (round-1 object)

| Gate | Result |
| --- | --- |
| `cargo check` | 0 warnings |
| focused `checked_artifact::` | 315 passed / 0 failed |
| 2.4 suites | 7 + 4 |
| clippy, plain and house pattern (`CLIPPY_CONF_DIR="$PWD" ... -D warnings`) | 0 |
| `rustfmt --edition 2024 --check`, six files | no diff |
| boundaries checker | single finding: the stale `pre_catalog.rs` tree pin (see below) |
| `CATALOG_PUBLICATION_CALL_COUNTS` | verified caller-by-caller; all five entries match, including `authority_record_binding.rs: 2`; no `rename_relative` token anywhere |

These gates were green **on the object this review graded NO-GO**, which is the
point worth recording: no local gate on a unix host could have surfaced [P0-1].
That is the process finding, not an incidental detail.

### (b) Post-remediation, re-verified by this reviewer

| Gate | Actual |
| --- | --- |
| `cargo check --all-targets` | exit 0, zero code warnings (cold-verified after `cargo clean`) |
| focused `cargo test --lib checked_artifact::` | `test result: ok. 318 passed; 0 failed; 0 ignored; 0 measured; 1137 filtered out` |
| `tests_authority_record` | `ok. 9 passed; 0 failed` |
| `tests_authority_record_matrix` | `ok. 5 passed; 0 failed` |
| clippy, house pattern `-D warnings` | exit 0 (sole emission is cargo's non-root-package profile notice, not a lint) |
| `rustfmt --edition 2024 --check`, six files | exit 0, no diff |
| boundaries checker (`python3.13`) | exit 1 with **exactly one** finding: `protected source tree changed: checked_artifact/capability/pre_catalog.rs` |
| `#[cfg(` in the binding | 10 of 10 are `#[cfg(test)]`; **zero platform predicates** |

Test-count arithmetic reconciles independently: +2 in `tests_authority_record`
(the platform pin and the provenance negative test) and +1 in the matrix (the
partition test) accounts for 315 -> 318 and 7/4 -> 9/5 exactly. No test was
removed or renamed to reach the number.

`python3.13` is required; the host's default `python3` is 3.10 and fails on the
`tomllib` import. That is a local toolchain artifact, not a finding.

**The single checker finding is owed, but its attribution needs correcting.**
The lane owner recorded it as the 2.3 lane's delta. That is incomplete.
`source_tree_digest` (`check_checked_artifact_boundaries.py:763-779`) hashes the
root file **plus every file under the sibling directory, recursively**
(`descendant_root.rglob("*")`). Step 2.4 itself adds three files under
`checked_artifact/capability/pre_catalog/` — `provider/authority_record_binding.rs`,
`provider/tests_authority_record.rs`, `provider/tests_authority_record_matrix.rs`
— and edits `provider.rs`. So **Step 2.4's own files necessarily move this pin**,
independently of 2.3.

This is not a review finding: the pin is recomputed from the pristine overlay by
the land ritual. It is recorded so the lane owner does not treat 2.3's landing as
discharging it — the recompute is owed after 2.4 lands **whichever lane lands
first, and again if both land separately**.

The absence of the `catalog publication seam changed` finding is itself positive
evidence: that finding fires on any `CATALOG_PUBLICATION_CALL_COUNTS` mismatch,
so its absence confirms the `authority_record_binding.rs: 2` entry is exact on
the post-remediation tree.

---

## 5. Round-1 verdict (2026-08-22) — superseded by §7

**NO-GO.**

The GO condition for this gate is zero P0 and zero P1. This object carries one
P0 and one P1.

[P0-1] alone is disqualifying and is not a judgement call: the object does not
compile on Windows, proven by compiling the crate itself with only the two `cfg`
attributes on the affected function swapped. The defect sits precisely on the
review's designated headline — the freeze §4.3 E9 writer-class condition whose
carriage this step was assigned — and its arm test, though correctly written,
cannot run there, so the package presents Windows assurance it does not have.
The fix is one keyword; the cost is a Windows matrix re-run, which this step
needs regardless since that arm has never been compiled.

[P1-1] is a design finding rather than a live bug, and it should be settled now
rather than after Step 3.3 builds a coordinator on the seam: the coherence
guarantee the R1 owner was frozen to make structural is, at the transaction
boundary, upheld only by callers passing matching arguments, while two doc
passages state it as constructed.

The rest of the object is strong and should be said so plainly. The parse/proof
split is real and legible; the sealed issuer genuinely delivers its three
unnameability properties; the 13-key accounting is exact with nothing minted;
the 26 interruption rows are real process stops with a substantive convergence
predicate and machine-checked reconciliation; the >1 MiB streaming evidence is
the two-opens-no-rewind kind rather than the merely-large kind; and the
integration blocker is correctly diagnosed and correctly deferred. Nothing here
suggests a rewrite — the remediation is one keyword, one struct field move, and
a doc-comment pass.

**ESCALATION TRIGGER — RECORDED.** P0/P1/P2 findings exist ([P0-1], [P1-1],
[P2-1]), which meets the plan §4 Phase-2 auto-escalation condition. This is
recorded here for the **lane owner to route**; this review is single-axis
Code-lead and makes no assumption that a State axis runs. Round 1 of 2; the
two-round cap leaves one round for re-verdict after remediation.

Suggested routing, for the lane owner's decision: [P0-1] is a
correctness-and-platform matter squarely on the Code axis and needs a Windows
matrix run as its evidence; [P1-1] is a seam/consumer-contract matter that a
State axis would have an independent view on; [P2-1] is evidence-taxonomy and
sits with whichever axis owns the fault-matrix form.

---

## 6. Round 2 — remediation re-verdict (2026-08-22)

Round 2 of the two-round cap. Scope held to the three fixes, per the lane
owner's instruction; unchanged code was not re-reviewed.

### [P0-1] CLOSED — and closed better than the remediation asked for

The two `#[cfg]` arms are gone. `require_authority_strength`
(`authority_record_binding.rs:311-320`) is now a single always-compiled plain
`fn` over one hoisted constant:

```rust
pub(super) const FOREIGN_EXACT_DURABLE_IS_WEAKER: bool = cfg!(windows);
pub(super) const FOREIGN_AUTHORITY_REFUSAL: &str =
    "a foreign-written leaf's durable proof is namespace ordering only on this platform";
```

Verified directly: all **10 of 10** `#[cfg(` occurrences in the file are
`#[cfg(test)]`; **zero platform predicates remain**. The `const fn` that could
not call the `String`-allocating `CheckedFsError::ambiguous` is now a plain
`fn`, so the E0015 defect is not merely fixed but structurally unreachable.

**On the disclosed proof deviation — CONFIRMED, and I contest nothing.** My
round-1 cfg-swap proof genuinely no longer applies: there are no predicates left
to swap. I judged reproduction necessary, since this was the finding the NO-GO
rested on, and ran it myself in an isolated copy (not the verification tree),
forcing `FOREIGN_EXACT_DURABLE_IS_WEAKER = true`:

```
test ...tests_authority_record::the_platform_with_the_weaker_foreign_durability_is_windows ... FAILED
test ...tests_authority_record::the_e9_writer_class_condition_is_carried_on_this_platform ... ok

thread '...the_platform_with_the_weaker_foreign_durability_is_windows' panicked at
  src/checked_artifact/capability/pre_catalog/provider/tests_authority_record.rs:384:5:
assertion `left == right` failed: the E9 annotation names Windows as the platform whose
read-only observation handle cannot flush; no other platform may claim the weaker semantics
  left: true
 right: false
```

The verification tree and the live tree were confirmed still at `cfg!(windows)`
afterwards; the flip copy was isolated throughout and has been removed.

The lane owner's read is correct and I endorse it as **strictly stronger** than
my original proof, on three counts:

1. My swap proved the Windows arm *compiles*. This proves it **executes** — and
   with the right refusal, not merely some error: the behaviour test matches
   `Err(CheckedFsError::Ambiguous { detail, .. })` and asserts
   `detail == FOREIGN_AUTHORITY_REFUSAL` (`tests_authority_record.rs:361-369`).
2. With `cfg!` there is one body on every platform, so the whole condition is
   type-checked *and lint-checked* on every host. The "arm compiled only on
   Windows can rot unnoticed" class — the class that produced [P0-1] — is
   eliminated by construction, not by a test.
3. The pairing is deliberate and correct: the behaviour test *reads* the
   constant so it follows a scratch flip, while
   `the_platform_with_the_weaker_foreign_durability_is_windows` (`:382-390`)
   asserts the constant *equals* `cfg!(windows)` so the flip cannot be left
   behind. I confirmed the detector fires. The doc at `:379-381` states exactly
   this division of labour.

The implementer also justified in-file why this is `cfg!` rather than `#[cfg]`
and correctly contrasted it with `leaf_observation::flush_observed_leaf`, whose
two arms are genuine because the *implementations* differ. That distinction is
right.

One duty from round 1 survives and should ride to landing notes: this arm's
behaviour has now been executed on a unix host under a forced constant, which is
strong, but the Windows matrix has still never run this code on Windows. The
land-time Windows matrix run remains owed — as ordinary evidence, no longer as a
blocker.

### [P1-1] CLOSED — by construction on the observation side, by a fail-closed guard on the join

Both halves verified in the live tree:

- `StreamedPayloadProofV1` (`:121-129`) now carries `action`, `artifact_root`
  and `retained_parent_identity`, populated once at the single mint site
  (`:213-216`) from `parent.path_profile()` / `parent.identity()` — the
  capability actually streamed through.
- `AuthorityTransactionV1` has **no parent field and no parent parameter**
  (`:341-357`); `observe_retained_request` reads every fact from `self.proof`
  (`:364-371`). Grepping the file, `artifact_root` and
  `retained_parent_identity` appear only on the proof, at the mint site, in the
  transaction's read, and in the guard. The mismatch is therefore
  **unrepresentable**, which is what the round-1 remediation asked for.
- `validate_terminal_relation` (`:400-414`) gained a typed provenance guard that
  runs **first** — before the payload comparison, before the record-bytes check,
  and, decisively, **before**
  `fault_v1::hit(RecordTerminalRelationValidate)`. So an incoherent join is
  refused rather than announced as a crossed boundary.

**The binding is real, not a tautology.** The guard compares three facts
captured at mint time against two arguments supplied at the join, which are
independent sources. The negative test
`a_proof_streamed_under_another_retained_directory_is_refused_at_the_join`
(`tests_authority_record.rs:401-430`) is well built: it uses two fixtures with
the **same action digest** but distinct durable directories, and first asserts
`assert_ne!(left_proof.retained_parent_identity(), right_proof.retained_parent_identity())`
— so the test cannot pass trivially, and it is the identity clause rather than
the action clause that must fire. It drives **both** directions and then
re-proves the coherent join still holds.

Forward-looking note for Step 3.3, not a finding here: the guard closes the
designated join, but `install`/`read`/`retire` remain individually callable with
a mismatched `parent`, and nothing forces a caller through
`validate_terminal_relation`. That is a coordinator-wiring obligation and
belongs to 3.3's review, not to this step. The implementer's doc at `:394-399`
scopes this accurately rather than overclaiming.

### [P2-1] CLOSED — and the split matches my own trace exactly

`REPEATABLE_BOUNDARIES` is 4 -> **9**, and `UNREPEATABLE_BOUNDARIES` is a new
constant naming the **4** exclusions with per-boundary reasons. Both lists match
the trace I performed independently in round 1, boundary for boundary:

- Included (9): `scratch_create`, `scratch_write`, `scratch_flush`,
  `bounded_read`, `decode`, `canonical_reencode`, `binding_validate`,
  `terminal_relation_validate`, `retirement_reserve` — the five I named as
  meeting the stated criterion but excluded are now included.
- Excluded (4): `active_publish`, `active_reobserve`, `retire_exact`,
  `retired_reobserve` — including the two reobservations whose omission from the
  round-1 comment was half of the finding. Each now carries its own reason, and
  the doc correctly distinguishes "unreachable a second time" from "untested",
  noting every one still has an interruption/restart/convergence row.

`the_repeatability_taxonomy_accounts_for_every_boundary`
(`tests_authority_record_matrix.rs:577-594`) genuinely pins **both** lists: it
sorts the concatenation and asserts equality with sorted `RECORD_MATRIX`, so a
key in neither list, in both lists, or duplicated within one fails on content or
length. The comment can no longer drift from the constants — which was the
substance of the finding, not merely the count.

Repeat rows 8 -> **18** (9 boundaries x 2 target variants), consistent with the
constants.

**Unprompted self-correction, credited:** `REPEATED_CRASH_ROUNDS` 12 -> **14**,
with the reasoning that the cycle crosses thirteen boundaries so rounds must
exceed thirteen to be "past nominal capacity" in the Phase 1 sense — 12 was
inherited from Step 2.1, whose cycle crosses ten. This was **not** in my
finding; I missed it. It is a real strengthening of the repeated-crash evidence
and the kind of self-audit worth recording.

### New in round 2

- **[P3-5]** `validate_terminal_relation`'s doc preamble still reads "it proves
  three things" while four numbered points now follow
  (`authority_record_binding.rs:377`, points at `:380/:382/:385/:389`).
  Introduced by the P1-1 fix. One word; rides to landing notes.

### Carried, open by scoping decision

[P3-1] through [P3-4] are untouched and remain open by explicit scoping
decision, riding to landing notes rather than blocking:

- **[P3-1]** `payload_lengths()` doc names a consumer that does not consume it.
- **[P3-2]** the `record_bytes()` guard remains unreachable, and the guard
  itself is unchanged. Its context did change: it now sits behind a genuinely
  reachable typed provenance guard in the same function, so
  `validate_terminal_relation` is no longer a function whose only novel
  assertions are tautological. The grade is unchanged.
- **[P3-3]** the `dead_code` reason still says Step 3.3 "wires its production
  consumer" where the plan says 3.3 wires machinery and R2-E converts consumers.
- **[P3-4]** `observed_record` still privately re-implements the shared bounded
  read, forced by `read_bounded_bytes` being `pub(super)` in non-test builds.

---

## 7. Final verdict

**GO.**

| Grade | Finding | Status |
| --- | --- | --- |
| **P0-1** | `cfg(windows)` arm was a `const fn` calling non-const `ambiguous` (E0015) — crate unbuildable on Windows | **CLOSED**, reproduced |
| **P1-1** | Retained parent and streamed proof were independent arguments | **CLOSED** |
| **P2-1** | `REPEATABLE_BOUNDARIES` under-populated; taxonomy could drift | **CLOSED** |
| P3-1 | `payload_lengths()` doc names a non-consumer | Open, landing notes |
| P3-2 | `record_bytes()` guard unreachable | Open, landing notes |
| P3-3 | `dead_code` reason misdescribes Step 3.3 | Open, landing notes |
| P3-4 | `observed_record` duplicates the bounded read | Open, landing notes |
| P3-5 | "proves three things", four points listed | Open, landing notes |

Zero P0, zero P1, zero P2 outstanding. The GO condition for this gate is met.

**Escalation status — DISCHARGED.** The round-1 auto-escalation trigger
(P0/P1/P2 present) is discharged by this round-2 re-verdict: all three findings
are closed, each verified against the code rather than against the remediation
note, and the P0 closure reproduced independently. No further escalation is
recorded. Round 2 of 2 — the two-round cap is now spent, so any further defect
found in this object is a new review object, not a third round.

**Land-time obligations** (evidence, not blockers): the Windows matrix run for
the E9 arm, which has still never executed on Windows; the
`PROTECTED_SOURCE_TREE_DIGESTS` recompute for
`checked_artifact/capability/pre_catalog.rs`, which **Step 2.4's own three new
files move independently of the 2.3 lane**; and the five P3s.
