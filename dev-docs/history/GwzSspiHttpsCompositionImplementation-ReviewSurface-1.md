# GwzSspiHttpsCompositionImplementation — Surface-AXIS REVIEW

**Review object:** Round 1 closure of the settled HTTPS SSPI caller surface: `dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md`, `gwz-sspi/docs/Supervision.md` and `CallerValues.md`, `gwz-core/docs/TransportPlacement.md`, and transport/CLI/Python README documents. Implementation review remains separate from Windows activation and runtime qualification.

**Baseline:**

| Repository | Corrected HEAD |
|---|---|
| root | `7066ff222abf7995e7e0103e9ad38543173d6079` |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` |
| gwz-sspi | `c88fa0e174b185a43e0d0d0c91660cb0957e380a` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` |
| gwz-core-evidence | `ba70034feaeb384619d48dbfaa06c02df6f508b3` |

Caller documents were read using `git show HEAD:<path>` with numbered output. All seven HEADs matched the corrected tuple at both start and end.

**Date:** 2026-10-04

**Axis:** Surface — independent, adversarial, read-only cold caller review of API discovery, defaults, ownership and lifecycle completeness. The other axes run separately; nothing here relies on their current-round reports. Filed verbatim by the lane owner.

**Verdict: GO** — P3-1 closed; zero open P0, P1, P2 or P3 findings in the reviewed caller surface. This verdict preserves the original documentation-only scope.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P3-1 | Add exact teardown signatures, receipt/report fields and a compiled cancel → cleanup_status → bounded shutdown recipe. | Retraced the original counterexample using Supervision lines 194–219 and caller guide lines 172–182. Cancel timing/result, RecordId ownership, shutdown future output and public report fields are explicit. The recipe retains the receipt and queries status without consuming its ID. Compilation is owner/document testimony, not a test executed by this reviewer. | Closed for this caller-document Surface review. |

## Changed-range analysis

The supplied correction ranges are:

- Root: `b72dccf813816f41a508eb0fc2f9b2f071b323a0` → `7066ff222abf7995e7e0103e9ad38543173d6079`.
- Core: `efdd0a2cf66be889364466c7d69c97cc2736c278` → `28f564a674574eaefa43266d3137be9a4ddc38b8`.
- SSPI: `58cc87c99bca21874a63d0ba11209b75a3e0c50a` → `c88fa0e174b185a43e0d0d0c91660cb0957e380a`.

Comparing the permitted documents with the original review’s reads:

- Supervision adds exact cancellation, cleanup-status and shutdown signatures; public receipt/report field names and types; the `cancel_and_account` recipe; and explicit Pending/Unknown and retained-proof guidance.
- The caller guide adds the corresponding signatures, synchronous/asynchronous distinction, RecordId cloning instruction, lifetime-count meaning and a pointer to the recipe.
- The caller guide also adds a visibility statement for local native identity/capture/launch refusals through request and Git clone/materialize consumers. It distinguishes those from quiet remote private-member refusals without changing the documented capture or cleanup lifecycle.
- CallerValues and TransportPlacement retain their previously reviewed caller contracts. Transport, CLI and Python HEADs are unchanged.

The added refusal-visibility statement is outside P3-1’s narrow teardown correction but coherent with the guide’s existing fixed-error/availability contract. Its implementation fidelity cannot be established by this caller-document review.

No new architectural root cause was found in the permitted surface. Source changes, inventory ledgers, implementation checkpoints and private evidence contents were not inspected.

## 0. Evidence base

This round read:

| Document | Lines |
|---|---:|
| `dev-docs/GwzSspiHttpsCompositionImplementation-PromptSurface-1.md` | Complete focused brief |
| `dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md` | 1–187 |
| `gwz-sspi/docs/Supervision.md` | 1–219 |
| `gwz-sspi/docs/CallerValues.md` | 1–125 |
| `gwz-core/docs/TransportPlacement.md` | 1–363 |

The original review had read the complete transport README, lines 1–502; CLI README, lines 1–84; and Python README, lines 1–228. Their repository HEADs are unchanged in this round, so those earlier reads remain applicable.

Workspace/member agent instructions and the process authority were read during the original review. This continuation followed the focused brief’s narrower commands and caller-document-only scope.

Executed inspection commands:

- `git rev-parse HEAD` in all seven repositories at start and end: fourteen successful checks, all matching the required corrected tuple.
- `git show HEAD:<listed-caller-document> | nl -ba`: four successful complete document reads.

No source, source diff, design, remediation plan, implementation checkpoint, private evidence content or current peer verdict was read. No files were written and no builds, tests, network calls or Git mutations were performed.

The statement that the disposal recipe is compiled is documentary/owner testimony. This reviewer did not compile or execute it.

## 1. Findings

No new findings. Prior P3-1 is closed within this review’s caller-document scope.

## 2. Invariant analysis

The original teardown counterexample can now be followed without guessing:

1. **Cancel explicitly.** Supervision lines 194–200 declares `Conversation::cancel(self) -> CancellationReceipt` and states that it is synchronous. The caller invokes `conversation.cancel()` without awaiting it; the Conversation is consumed.

2. **Inspect and preserve the receipt.** Both documents name public `record_id: RecordId` and `cleanup: CleanupStatus` fields. The caller can retain the receipt and inspect its initial cleanup observation.

3. **Query subsequent cleanup.** `Supervisor::cleanup_status(&self, RecordId) -> CleanupStatus` explicitly takes an owned ID. The recipe uses `receipt.record_id.clone()`, preserving the original receipt for accounting. RecordId’s Clone contract is already documented.

4. **Run bounded shutdown.** The exact shutdown signature returns an owned `Send + 'static` future with `Output = ShutdownReport`. The recipe awaits it using the separately supplied shutdown Deadline.

5. **Inspect shutdown results.** The report exposes `confirmed: usize` and `outstanding: Vec<RecordId>`. The caller guide explicitly states that confirmed is a lifetime count rather than a count confined to this shutdown call. Outstanding records may remain when the bound expires.

6. **Retain unresolved ownership.** Supervision lines 215–219 and the caller guide retain the rule that Pending and Unknown do not authorize release. Dropping the shutdown future retains supervision. Previously observed confirmation must be retained by the host because an evicted tombstone can later become Unknown.

The recipe returns the receipt, observed status and report to the host. Combined with the explicit field declarations, it supplies the complete expressions and types needed for the original cancellation/accounting walkthrough.

The surrounding lifecycle remains coherent:

- Trusted absolute worker path and matching packaging fingerprint precede Supervisor construction; worker capacity defaults to eight with range 1–64.
- Original-thread capture precedes handoff, is shared through Arc, and remains scoped to its issuing Supervisor.
- Start is constructed before host submission and owns the request/origin reference independently of borrowed caller references.
- One immutable positive Open deadline covers discovery, helpers, capacity and native rounds. Zero refuses native selection; expiry of a positive deadline yields Timeout.
- Native setup refusal does not prevent anonymous, configured Basic or SSH work.
- Native Complete and authoritative Continue remain distinct from validated remote acceptance.
- Finish, synchronous cancel, capture disposal and bounded shutdown form a documented lifecycle. Pending/Unknown cleanup remains charged.
- Foreign/closed captures and original-thread liveness/identity failures retain explicit error classifications.
- Candidate API documentation continues to withhold Windows endpoint activation and installed native/provider qualification.

No new guessing point was found in the corrected teardown walkthrough. Production packaging and live provider qualification remain explicit external/deferred responsibilities rather than implied capabilities of these recipes.

## 3. Risks and next action

This GO establishes that the caller documentation now resolves P3-1 and remains internally coherent. It does not independently prove recipe compilation, implementation fidelity, native cleanup execution or authenticated Windows HTTPS operation.

Windows activation, native provider/EPA/CBT/live identity qualification, Digest implementation, the broader platform/package matrix and release publication remain deferred.

The next action is to record P3-1’s Surface closure against this exact corrected tuple. All seven HEADs remained unchanged through the final check.
