# GWZ M5–M8 Refactor Proposal — Independent Re-Review (F5-2-2)

Date: 2026-07-30. Reviewer: Claude (Fable 5).

Subject: `dev-docs/GwzM5-8Refactor.md`, revision "Review 8 and independent
F5-2 incorporated" (3,147 lines; prior reviewed revision was 2,984).

Method: full read of every changed region against the prior revision (held in
context from the F5-2 review), plus code and repository checks for the new
factual claims (gwz-py release tags, validate.rs diagnostics, and the
already-updated `gwz-core/dev-docs/GwzMergePlan.md`). As before, the numbered
reviews — including the new Review-8 — were not read; Review-8's contributions
were evaluated from the document text itself.

---

## 1. Findings

No P0, P1, or P2 findings. Two P3 findings.

### [P3-1] The stale `--no-ff` rejection diagnostic is not addressed by the M5a package or the consistency gate

The shipped rejection is
`"no_ff merge mode is not available until M5"`
([validate.rs:13](gwz-core/src/workspace_ops/merge/validate.rs:13), pinned by
the test at
[validate.rs:216](gwz-core/src/workspace_ops/merge/validate.rs:216)). Two
independent reasons make this text wrong under the new sequence:

1. **It becomes factually false at M5a.** The M5a release *is* "M5" from a
   user's perspective, yet it still rejects `--no-ff` (now deferred to
   M5b/v1/A1). A user reading "not available until M5" on an M5a binary
   reasonably concludes their install predates M5 and upgrades or reinstalls
   pointlessly.
2. **It already violates the plan's own diagnostic rule.** The executable
   plan's frozen contract states "user-visible diagnostics describe
   capabilities without M0/M1/M2/M3/M4/M5/M6/M7/M8 names"
   ([GwzMergePlan.md:717](gwz-core/dev-docs/GwzMergePlan.md:717)); this
   message is an existing escapee of that rule.

Neither the proposal's M5a package ("Keep the `--no-ff` CLI/protocol
activation unavailable in this release", [:1934](GwzM5-8Refactor.md)) nor the
new R0 document-consistency gate ([:2615](GwzM5-8Refactor.md)) reaches CLI
error strings — the gate's named sources are the proposal, plan, design, and
public documentation, not diagnostics in code.

**Correction:** add a one-line M5a task to reword the rejection in
capability terms without milestone names (e.g. "no_ff requires the v1 record
lifecycle and is not yet activated"), updating the pinned test; optionally add
user-visible diagnostic strings to the consistency gate's source list so
milestone-named or sequence-contradicting messages fail the gate.
**Regression test:** the existing message assertion at validate.rs:216,
updated to the new wording.

### [P3-2] The document-consistency gate's public-documentation source set and mechanism are unenumerated

The gate names two concrete files (`gwz-core/dev-docs/GwzMergePlan.md`,
`GwzMergeDesign.md`) but describes the rest only as "current public
capability/deferred-feature documentation" ([:2616](GwzM5-8Refactor.md)), and
§22.1.8 accepts any gate that "enforces the M5a/M5b/A1 and I6/I7/I8
sequence". A prose-consistency gate over an unenumerated file set can pass
trivially by checking too little; the actual public capability text lives in
identifiable files today (`gwz-cli/docs/commands/merge.md`,
`docs/Releases.md`, `docs/MachineOutput.md`, generated `CLI.md`).

**Correction:** enumerate the initial source list and the mechanism (an
assertion manifest of file + required/forbidden statements) in R0's package or
§18.10, so the gate's coverage is reviewable rather than implied.
**Regression test:** the gate's own manifest, including one deliberate
violation fixture (e.g., a doc line reintroducing "no-ff in v0") proving the
gate fails.

---

## 2. Disposition of F5-2 findings

All three F5-2 findings are fixed, precisely and with ripple-through:

| F5-2 finding | Disposition |
| --- | --- |
| **P3-1** v3/M7 lacked checkpoint parity | **Fixed.** New I7 — "M7/v3 source, record, and archive checkpoint" [:2165] — owns exactly the items I listed: v3 wire fields/adapter, the append-only `V3` archive projection body/discriminant, v3 unknown-field container-retirement entries, and the actual A1/v2 retained-reader downgrade fixtures. M7 now "consume[s] the I7-frozen source, wire, archive, unknown-field, and downgrade contracts without redefining them" [:2194]. Ripple verified through §1, §2.1 (items 15–16), §16.5, §22.3.18, §22.5.2, §22.6.12, §23, the risk table, and stop conditions. |
| **P3-2** "eligible" migration undefined | **Fixed.** §15.6 now opens with a closed definition [:1553]: any structurally valid open v0 record mapping to a legal §15.3.2 row A–M is eligible — explicitly including `RecoveryRequired` and operation-level drift — provided no `mode: no_ff` and every unknown field has a lossless destination; migration is defined as "a representation change, not an automatic lifecycle action" preserving exact state, drift result, evidence, and next action, with no Git/artifact mutation; read-only status never migrates. I2 freezes the set [:1994]; §22.4.9 and §24 restate it consistently. This answers my row-L question with the liberal option and the justification I sketched. |
| **P3-3** protocol codes unallocated | **Fixed.** §15.2 [:1243] maps the internal results to append-only taut codes `unsupported_record_version`, `unsupported_legacy_mode`, `archived_record_unreadable`; I2 allocates and pins numerics plus human/JSON/JSONL projection [:1980]; §18.9 fixtures pin the numeric allocation and projection [:2587]; reuse of the broader `record_unreadable` code is explicitly forbidden. |

## 3. Review-8 additions, independently verified

I did not read Review-8; its contributions were assessed from the revised
text and checked against the repositories:

- **gwz-py as a retained reader surface (§15.8).** Correct and previously
  missed by both of my reviews: released `gwz-py` wheels embed gwz-core
  natively and are therefore independent decode generations. Factually
  grounded — the gwz-py repository is tagged in lockstep (v0.9.0–v0.10.2,
  dynamic version from tag). The manifest handles never-distributed
  surface/platform combinations via explicit unsupported tuples with reasons
  ("cannot be replaced by current Python code calling an old Rust
  executable" is exactly the right constraint), and the two-tier platform
  policy (Linux/Windows behavioral lanes; macOS/arm64 artifact-integrity,
  launch/import, and smoke evidence) is coherent with the release line.
  §18.8, §18.10, §22.1.4, and §22.3.15 are consistently updated.
- **Archive projection appended per wave (§15.3.3).** The
  `VersionedAcceptedWorkspaceProjection` sketch is now explicitly "the
  eventual cumulative A4 shape, not an A1 compiled type" [:1424]: A1 compiles
  and publishes only `V1`; I6/I7/I8 append `V2`–`V4` with new pinned taut
  discriminants. §22.4.8 enforces the absence of V2–V4
  bodies/discriminants at A1. This closes a latent dormant-variant tension in
  the previous revision's sketch that I had not flagged — the same principle
  the document applies to wire records now applies to projection types.
- **Cross-document consistency gate.** R0 builds it; it runs at every
  M5a–A4 integration gate; §24 requires the plan and design to adopt the
  sequence "before M5 work is dispatched". Verified already satisfied for the
  executable plan: `GwzMergePlan.md` has been rewritten around
  M5a/M5b/A1 and I6/I7/I8, including per-package LOC budgets (M5b ≤500,
  A1 ≤250) and its own copy of the gate language. The remaining gap in the
  gate's own specification is finding P3-2 above.

## 4. Consistency checks performed

- **F5-2 fix ripple:** every I7/eligibility/error-code reference traced
  through §§1, 2.1, 15.1.1, 15.2, 15.3.3, 15.6, 16.5, 17 (I2/I7/M7/A3), 18.7–
  18.9, 19, 20, 21, 22.1–22.6, 23, and 24 — no stale text found; §22.5.2 now
  reads "I7 has approved v3 before M7/A3" and §23 carries the I7 sentence.
- **Naming:** `ArchivedMergeProjection` (outer struct) vs
  `ArchivedAcceptanceProjection` (inner acceptance enum) — coexist by design;
  all four references consistent.
- **Creation matrix vs prose:** unchanged from the prior revision and still
  agreeing across §15.1.1, §16.2–16.4, §18.8, §22.4/22.5, §24 (M5a → v0 with
  no-ff rejected; A1+ → v1 floor; no-ff → v1; v2/v3/v4 per feature; max rule;
  no recomputation for existing records; §15.6-eligible migration only).
- **§15.6 eligibility vs §15.3.2 row L:** the "representation-only" framing
  is compatible with row L's "no automatic mutation" — the record rewrite
  preserves classification and next action; I2's freeze bullet repeats the
  constraint ("identical next-action classification").
- **gwz-py surface reality:** tags confirm released Python readers exist for
  both decode generations; the harness's conditional unsupported-tuple path
  covers any combination that was never actually distributed.
- **M7 wording:** the package retains a runtime "freeze snapshot ID…" bullet
  (per-operation value freezing) alongside I7's contract freeze; the final
  "without redefining them" bullet disambiguates — no actionable ambiguity.

## 5. Verdict

**The revision cleanly incorporates F5-2 and Review-8; the approved immediate
scope (R0, R1, R2a, M5a) is unchanged and remains sound.** The versioning
architecture is now closed end-to-end: every wire version, archive projection
variant, protocol error code, migration-eligibility rule, and retained reader
surface (Rust and Python, both decode generations) has a named owner and a
named freeze point. The two P3 findings — the milestone-named `--no-ff`
diagnostic that M5a must reword, and the unenumerated source set for the
document-consistency gate — are small, concrete, and non-blocking; both fit
naturally into R0/M5a scope or the next editorial pass.
