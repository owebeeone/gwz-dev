# Local clone — F51 reconciliation and library-policy adoption

Date: 2026-09-05. Lead reconciliation, **not a fourth independent review**.
Inputs: design revision 7, plan revision 3, architecture revision 2,
the second GPT-6/Sol reports and the independent F51 report. Output:
design revision 8, plan revision 4, architecture revision 3 and
[library boundaries revision 1](GwzLocalCloneLibraryBoundaries.md).
The original independent reports have not been edited.

## Decision

Adopt the shared library policy for the **new local-clone libraries** in
the proposed architecture. It directly addresses the operator's requirement
for parallel ownership and independently compiled seconds-scale tests.
There is no existing-core refactor, public plugin ABI, new filesystem
catalog, transaction journal or deletion-replay work in this adoption.

F51's **NO-GO for parallel feature lanes / GO for the interface checkpoint**
is accepted. Documentation now specifies the package/dependency/ownership
and test contract. It does not supply compiled interfaces, implementations,
an installed architecture gate, or measured new-library performance.
Those remain LCM1.0 exit requirements before opening feature lanes.

The operator additionally requires an independent review once the skeleton
is built. Plan LCM1.0d reviews the exact implementation commit and measured
evidence, resolves blocking findings and records GO before parallel feature
development. This review has not yet occurred.

## Findings and disposition

| Finding | Planning correction | Evidence still required |
|---|---|---|
| F51 P2-1: ownership, missing interfaces/types | Boundary document assigns each package/API/files to a lane and names public operations, result/error/evidence owners and partial-effect rules | Complete compiled structs/enums/signatures, contract tests and consumer review in LCM1.0 |
| F51 P2-2: pinned compiler roots shared across lanes | C owns one initial integration skeleton and reviewed re-pins; other lanes own package directories | Existing boundary/per-commit checker passes on actual skeleton and later changes |
| F51 P2-3: anonymous transport absent | Plan now calls it a new port/adapter; C owns fetch and push together, including backend delegates and tracking double | Real anonymous transfers through the port, explicit refspecs, failure injection and no persisted remote |
| F51 P2-4: remote resolver unowned | One pure family-model resolver with explicit merge versus pull/push fallback/state behavior | Shared table tests and all three core wrappers use it |
| F51 P3-1; GPT-6/Sol isolation | All ten functional libraries get independent path crates, including orchestration; three small contracts prevent concrete implementation dependencies | Cargo graph/negative fixtures; target builds without unrelated implementation; real allowed dependency closures remain |
| F51 P3-2: tiers/timing | Tier A ≤2 s execution / ≤10 s warm per package; Tier B slices separated; cold builds and broad acceptance separate | Host/toolchain/SHA measurements at checkpoint and milestone exits; proposed numbers are not achieved guarantees |
| F51 P3-3: store primitives | Store owns its small OS try-lock/ordinary publication adapter; sole family metadata writer; no use of private/pinned checked-artifact helpers | Busy/error/release/partial-write conformance, platform and subprocess smoke |
| F51 P3-4: shared fixture owner | T owns dev-only local-testrepo; pure-library tests use deterministic plain values | Tiny real Git fixture API and consumers; no core/test-harness dependency in leaves |
| F51 P3-5: prerequisites/start order | Stage table separates contract prerequisites from real-adapter integration; work/history can start with MVP libraries | Checkpoint lands before feature branches; lane files remain disjoint |
| Second-round dry-run | Request validation/unsupported family dry-run refusal before any writes, including lock-file creation | Zero-write/zero-transport tests; ordinary Git-remote dry-run unchanged |
| Second-round relative hooks | Effective absolute/relative hooks paths resolved for destination before reservation, using applicable Git hook working directories | Escaping and valid-internal fixtures, including bare/push hook bases |

The earlier seven GPT-6/Sol concerns were already closed at the contract
level by their originating reviewers, as recorded in
[round-2 reconciliation](GwzLocalClone-ReviewPlanArch-Summary-2.md).
No independent reviewer has reviewed these new output revisions yet.

## Recommendations adapted rather than copied verbatim

- F51 proposed six leaf crates with orchestration remaining in core. The
  user's stronger isolation requirement and the supplied policy justify
  independent orchestration crates with narrow ports; core retains thin
  adapters and real-engine integration tests.
- F51 suggested a tiny “std-only” lock implementation. The current lock
  source contains OS FFI and substantially more than forty lines. Specify
  behavior and small tested platform wrappers, not an arbitrary size or
  portability guarantee. Private checked-artifact helpers stay private.
- Do not move Git-remote lookup into the pure resolver: only the existing
  per-repository lookup can establish fallback existence. Merge non-ready
  rows retain the product contract's `UnknownLocal` with state detail;
  pull/push non-ready rows are lifecycle refusals. Both refuse, neither
  silently falls through.
- “One skeleton re-pin” describes initial coordinated registration.
  Later legitimate protected edits still need reviewed evidence; no
  blanket promise that they can never be required.
- Small source-structure checks alone cannot prove contracts. Use compiler
  witnesses and shared behavior tests, review API/type leakage, and report
  the metadata gate's coverage honestly. Do not build a general semantic
  Rust analyzer as part of local clone.

## Cost and verification

LCM1.0 grows from 1–2 to **2–4 engineer-days** for explicit package/contracts,
fixtures and a bounded local architecture gate. LCM1 is now **9–16**, LCM2
**7–12**, LCM3 **4–7**: total **20–35 engineer-days**. These are planning
judgments, not measured productivity or elapsed time with parallel agents.
The best-effort product scope remains intact.

The lead reread F51 and the policy, inspected current manifests, backend
transport, merge dry-run entry and protected lock/pin sources, and checked
document consistency. Git's official hooks/config documentation confirms
relative paths use hook working directories; those sources are linked from
design §4.0. No source implementation, full suite, native acceptance or
new-library timing run was performed for this documentation change.

F51's measurements of existing core (0.20 s execution / 1.2 s wall for
12 filtered tests; 5.4 s for a separate mtime-triggered rebuild) are useful
context, not evidence for the proposed packages. Sol's different 22.65 s
recompiling command is likewise not a comparative benchmark. Record the
three timing categories on the same declared workload at implementation.

F51 input report SHA-256:
`6a02784defae0e1ecb906222cb13406255e59767599bf7108ca1c9b54956bd8c`.
Canonical policy baseline SHA-256:
`dcc4fbd2b45caf928978a090208951ef14759ef0589e820b94f8475fccf07c10`.
The local adoption links to that policy rather than duplicating its rules.
