# GWZ M5–M8 Merge-Lifecycle Refactor Proposal — Re-review 8

Date: 2026-07-30

Reviewed:

- `dev-docs/GwzM5-8Refactor.md`, revision with SHA-256
  `3f7363f7f05553f75b633a31371228832ae4a35fabce1b1e09ee06b4d9457a50`;
- numbered reviews 1–7;
- `dev-docs/GwzM5-8Refactor-ReviewF5.md`;
- `gwz-core/dev-docs/GwzMergePlan.md`, revision with SHA-256
  `c19312413bef6b5aa2f8cf2d3044bd22543344b26a164a647e2cf9ef85b66cc0`;
  and
- the current merge wire record, mode handling, archived-record
  loader/retention/GC, module ownership, publication, finalization,
  preservation, and rollback implementation.

## Verdict

Both review-7 findings are resolved:

- every supported v1–v4 archive now has a version-specific, archive-only
  persisted-acceptance projection; and
- the envelope classifier is ordered, disjoint, duplicate-aware, and covered
  by a complete cross-driver header matrix.

The independent F5 review also found an important issue that the numbered
reviews missed: `mode: no_ff` is not safe as a newly writable v0 value. The
proposal's correction is sound. Custom messages remain in the v0-safe M5a
slice, while deterministic `--no-ff` moves to M5b/v1 and activates with A1.
The newly-writable-shape gate, `merge/mod.rs` ownership map, message-byte owner,
retained-reader harness, orthogonal archive outcome, and I1/I6/I8 sequencing
have all been materially improved.

**R0, R1, R2a, and the M5a custom-message architecture remain approved.**

One P1 planning inconsistency must be corrected before implementation follows
the new split: `GwzMergePlan.md` still instructs agents to implement and ship
`--no-ff` and custom messages together in M5. Two additional P2/P3
clarifications should be made before I2 freezes the v1/archive/proof contracts.

## Disposition of review 7

| Review-7 finding | Re-review result |
| --- | --- |
| P2-1: supported older v1–v3 archives were absent from the archive projection | **Resolved.** §15.3.3 now separates terminal outcome from acceptance, uses `SupportedPersisted` with exact source version and version-specific projections, requires archive decoders for every supported version, protects unsupported future archives from older retention/GC, and adds A2–A4 archive matrices. |
| P3-1: envelope mismatch rows overlapped | **Resolved.** §15.2 now defines a duplicate-aware header parse, a header-only allocation registry, five ordered disjoint classes, raw-pair reporting, and a full malformed/mismatched/unsupported cross-product through every driver. |

## Disposition of the independent F5 review

| F5 finding | Re-review result |
| --- | --- |
| P1-1: v0 `mode: no_ff` is silently mis-executed by released readers | **Resolved in the proposal.** M5 is split into v0-safe M5a custom messages and v1-only M5b no-ff; open legacy-v0 no-ff is rejected before migration or mutation. The remaining cross-document release-plan conflict is the new P1 below. |
| P2-1: the old-reader gate covered fields but not newly writable values/variants/combinations | **Resolved.** §15.1 now governs every newly writable same-version shape and explicitly tests the v0.9.2 field-unknown and v0.10.2 variant-known decode generations. |
| P2-2: `merge/mod.rs` had no target owner map | **Resolved.** §§13–14 assign dispatch, open-gate, mutation-guard, store seam, and persistence helpers to explicit runtime/store owners and keep root orchestration under a review limit. |
| P3-1: exact M5 message bytes had two owners | **Resolved.** R2a alone freezes the exact custom-message bytes before M5a. |
| P3-2: retained-reader matrices lacked an operational specification | **Substantially resolved.** §15.8 defines a checksum-pinned manifest, immutable artifacts, cache, isolation, timeouts, observations, expectations, and fail-not-skip behavior. Reader-surface/platform scope still needs the P3 clarification below. |
| P3-3: archived acceptance could not represent accepted-then-aborted | **Resolved.** `ArchivedMergeProjection` makes terminal outcome a sibling of acceptance availability. |
| P3-4: full M6/M8 product design unnecessarily blocked v1 | **Resolved.** I1 freezes only v1 directional boundaries; full policies move to I6 and I8 before v2/v4. |
| P3-5: the review basis omitted numbered Review 7 | **Resolved.** The status and review basis include all seven numbered reviews and the independent F5 review. |

## Findings

### [P1-1] The executable merge plan still tells agents to ship unsafe v0 no-ff

**Where:** `gwz-core/dev-docs/GwzMergePlan.md` §§1, 6, 17, 18, and 19,
especially lines 66–70, 85–88, 500–504, 1539–1550, 1745–1747, and 1751–1760.

The revised proposal correctly concludes that no released v0 reader safely
handles a newly writable `mode: no_ff` record. It therefore:

- forbids M5a from writing v0 no-ff;
- releases only custom messages in M5a;
- places executable no-ff in v1;
- implements it behind the disabled writer as M5b; and
- activates it only through A1.

The existing implementation plan still says the opposite:

- “Git-expected `--no-ff` and custom messages follow in M5”;
- “M5 ships Git-expected `--no-ff` and custom-message behavior”;
- the dependency graph runs M5-A no-ff and M5-B custom messages in parallel
  into one G5 release gate;
- Wave M5 says both additions ship together;
- the milestone table defines M5 as both features; and
- the recommended next run simply says “begin M5.”

This is not a harmless milestone-name mismatch. `GwzMergePlan.md` is the
workspace's executable implementation plan. An agent following it can still
build the exact unsafe v0 release that the proposal now prohibits, including
the same combined gate and sequencing that F5 proved unsound.

**Required correction before M5 implementation resumes:**

Amend `GwzMergePlan.md` to make the new release sequence authoritative:

1. M5a contains only `-m`/custom messages and remains v0.
2. M5a rejects `--no-ff` before record creation.
3. The first M5 release gate covers custom messages only.
4. I1, I2, R4a, disabled-writer R3, R4b, and disabled M5b precede A1.
5. M5b contains deterministic no-ff under v1 and has no independently
   releasable v0 path.
6. A1 activates the v1 writer, eligible v0 migration, and no-ff start surface
   together.
7. I6/M6 follows the accepted v1/A1 boundary rather than the obsolete combined
   G5 edge.

Update all affected plan artifacts together:

- release-boundary prose;
- dependency graph;
- change budgets and ownership for the old M5-A/M5-B packages;
- Wave M5 definition and release gate;
- milestone table;
- recommended next implementation run; and
- any public capability/deferred-feature checklist that treats no-ff as
  available with M5a.

Add a document-consistency gate that fails while the plan claims no-ff is
writable or releasable in v0. The proposal and plan should use the same
M5a/M5b/A1 names so later agents do not have to infer which document
supersedes the other.

### [P2-1] The archive projection sketch predeclares unresolved v2–v4 types at the v1 freeze

**Where:** §15.3.3; I2; R3; I6/M6, M7, and I8/M8; §§22.3–22.5.

The revised archive architecture correctly says each installed binary has
decoders only for versions it supports. Its concrete type sketch nevertheless
defines:

```rust
enum VersionedAcceptedWorkspaceProjection {
    V1(AcceptedWorkspaceV1Projection),
    V2(AcceptedWorkspaceV2Projection),
    V3(AcceptedWorkspaceV3Projection),
    V4(AcceptedWorkspaceV4Projection),
}
```

At I2/A1, the v2 branch model and v4 participation/composition model are
deliberately unresolved until I6 and I8; M7's v3 source projection is likewise
not yet frozen. The proposal also repeatedly forbids the A1 canonical model
from compiling dormant v2–v4 variants.

The archive projection is read-only and cannot authorize mutation, so this is
not the same safety defect as dormant executable lifecycle state. It is still
an incompatible freeze instruction: I2 cannot define the exact
`AcceptedWorkspaceV2Projection`–`V4Projection` types without pre-empting their
later interface checkpoints, while compiling placeholder variants would create
the same speculative type pressure the version-per-wave design was introduced
to avoid.

**Required correction before I2:**

State explicitly that the four-variant sketch is the eventual cumulative A4
shape, not the A1 compiled or protocol model.

At each activation:

- A1 implements legacy v0 plus persisted V1 projection only.
- I6/M6 freezes and adds V2 projection together with the v2 archive decoder.
- M7's reviewed source contract freezes and adds V3.
- I8/M8 freezes and adds V4.

The common `ArchivedMergeProjection` may retain `source_version`, sibling
terminal outcome, and acceptance quality from A1 onward, but an unsupported
version must stop at the header classifier and never instantiate a future
projection variant.

Also define how these later projection additions remain append-only in the
protocol. Add a compile-time/fixture assertion that the A1 model and generated
protocol contain no V2–V4 projection body or discriminant requiring knowledge
of those unresolved semantics, while the A1 binary still recognizes their
header pairs solely through the header-only registry.

Revise I2 acceptance item 14 to freeze the general per-supported-version rule
and the concrete v0/v1 implementation, leaving exact V2–V4 projection bodies
to their own checkpoints.

### [P3-1] The retained-reader harness does not explicitly inventory both driver surfaces or the full release-platform evidence

**Where:** §15.8; R0; §§18.8–18.10; §§22.1–22.5.

Section 15.8 says the manifest covers “each supported reader,” but its artifact
and invocation language is written around standalone executables. It does not
state whether the retained support window includes:

- only the Rust `gwz` executable;
- released `gwz-py` wheels/native extensions as independent readers; or
- both.

That matters because the rest of the proposal treats Rust CLI and
Python/native behavior, JSON, and JSONL parity as release requirements.
Core mutation policy is shared, but actual old Python packaging and rendering
can still differ in command availability, error preservation, and machine
projection.

The platform statement also calls Linux x86_64 and Windows x86_64 “the
supported release line,” while GWZ release evidence also includes macOS and
Linux arm64 builds. A two-OS behavioral matrix can be a reasonable deliberate
choice, but it should not redefine the supported release targets.

**Required clarification before the harness is frozen:**

1. Enumerate the retained reader surfaces explicitly: Rust CLI and, if it is
   inside the supported downgrade contract, `gwz-py` wheel/native plus its
   invocation/runtime metadata.
2. If actual old Python artifacts are excluded, document why the Rust
   executable proves mutation compatibility and which Python compatibility
   claims remain covered only by current cross-driver tests.
3. Separate **behavioral retained-reader lanes** from **platform release
   evidence**:
   - state which OS/architecture pairs run the complete downgrade fixture
     matrix; and
   - retain the existing release-build evidence for every supported
     distribution target, including macOS and Linux arm64.
4. Make unsupported artifact/runtime combinations explicit manifest entries,
   not implicit omissions.

This clarification need not multiply every semantic fixture across every
architecture. It needs to make the support claim and the evidence supplied for
each released surface unambiguous.

## Positive assessment

The proposal is now substantially stronger than the revision reviewed in
numbered Review 7.

The no-ff correction is particularly important:

- it applies the old-reader gate to values and variants, not only fields;
- it uses actual differing decode generations as evidence;
- it avoids inventing another version between v0 and v1;
- it makes malformed legacy no-ff fail closed rather than guessing which
  semantics created it; and
- it reuses the already necessary A1 envelope boundary.

The other F5 remedies improve implementability rather than merely adding
prose:

- the largest merge module now has an ownership map;
- exact message bytes have one pre-release owner;
- actual-reader evidence has an operational harness;
- archive terminal outcome and acceptance are correctly orthogonal; and
- v1 durability no longer waits for full M6/M8 product policy.

The numbered Review-7 corrections are also complete. Supported older archives
are no longer forced into “current” or “legacy” categories, and envelope errors
now have deterministic typed classification.

## Recommended disposition

1. Keep R0, R1, R2a, and the M5a custom-message architecture approved.
2. Amend `GwzMergePlan.md` before using it to dispatch M5 work.
3. Make the archive projection type phase-specific before I2 freezes v1.
4. Clarify retained Rust/Python reader surfaces and distinguish behavioral
   lanes from full release-platform build evidence.
5. Re-review the synchronized plan and frozen I1/I2 contract before R3.

The proposal's principal architecture is now sound. The immediate risk is no
longer the proposal's no-ff decision; it is that the still-authoritative merge
plan directs implementation toward the superseded and unsafe release shape.
