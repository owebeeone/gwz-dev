# Python concurrent transport session v2 — CONSISTENCY-AXIS RE-REVIEW

**Review object:** Committed `dev-docs/GwzPyTransportSessionV2Design.md` and `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`.  
**Baseline:** root `59c13d6184c82789d9dc558ca436b3a43be8bdcd`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `ffa184bbb50bf56ff49ebee65a79c228e0bf6489`. All three heads matched at review start and end.  
**Date:** 2026-09-24.  
**Axis:** Consistency — internal coherence, controlling contracts, supersession, and satisfiable evidence. Independent, adversarial, read-only.

**Verdict: NO-GO.** One P2 compatibility finding remains open. One P3 proof-specification finding does not independently block. I pre-commit to **GO** if bounded corrections close these findings without introducing another blocking defect.

## Prior-finding closure table

| First-round Consistency finding | Re-trace at this tuple | Disposition |
| --- | --- | --- |
| P2-1 — runtime rollover contradicted the accepted stable-host and never-reused-request-ID clauses | Root design §1 explicitly supersedes both clauses; §2 binds cancellation to public operation ID and original core generation; §5 preserves captured endpoint configuration across rollover. The Python accepted-design pointer now repeats this distinction at lines 287–292. | **Closed in the document contract.** The operation-257/old-handle case remains an implementation test. |
| P2-2 — eight accepted operations could exhaust capacity for their first event readers | §§5–6 put one primary cursor inside each operation’s pre-acceptance 8 MiB reservation; stream helpers claim it before worker release, and a handle’s first iterator claims it without another aggregate charge. Optional readers alone may refuse. | **Closed in the document contract.** The full-ledger reader case remains an implementation test. |
| P2-3 — detached unary-cancellation result had no named accessor | §2 and §7 specify `GwzOperationCancelled.response: OperationResult | None`, with admitted and pre-admission meanings; the caller guide names the same property. | **Closed in the document contract.** The cancelled-push/close case remains an implementation test. |

## Changed-range analysis

Against original root `f874d421d9f0f453b673d1de17a1cf4fa4fc621e` and Python `24a4314487ff477eaca0535bdeae3289fead7ab9`, the root design added explicit generation supersession and ID-retry semantics; a reserved primary reader; a named cancellation response; Client-owned retention and stream abandonment recovery; Client-level per-host capacity inheritance; and an explicit outer-protocol compatibility statement for two appended error codes. The caller guide added the corresponding usage and recovery text, while the accepted Python design gained the generation pointer. Core stayed at `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; its candidate plan clauses and schema were unchanged. The findings below concern the revised contract and its proof, not implementation behavior.

## 0. Evidence base

I read the committed review object; the first-round Consistency, Safety, and Surface reports and remediation plan; the predecessor verdict; the accepted Python transport design; the core retry, remote transport, and v1.1.0 plans; the relevant GWZ protocol enum and `OperationResult` schema; and the process rules and review-loop skill. I compared the named document ranges with `git diff` and used `git show` for committed content. No files were changed, and no build or test ran. Implementation, platform/source checks, and physical wire or iroh proof were outside this review.

## 1. Findings

### [P2-1] The new `Client.close()` return contract conflicts with the retained custom-bridge contract

**Root cause and location:** Root design §6, line 45, and caller guide line 71 state that `Client.close()` returns a `TransportCleanup` containing physical facts and operation summaries. The accepted `gwz-py/dev-docs/GwzPyTransportDesign.md` §4, lines 168–179, still requires `Client.close() -> TransportCleanup | None` and says a custom bridge without `close` returns `None` rather than claiming cleanup. Its lines 181–196 preserve custom `CoreBridge` test doubles. The v2 precedence clause at line 9 does not supersede this custom-bridge rule or limit the new return promise to the native bridge.

**Violated invariant:** One public `Client.close()` call must have a truthful, compatible result for every supported bridge.

**Credible sequence and impact:** Construct a `Client` with an existing custom bridge that has no `close`, then await `client.close()`. The accepted contract requires `None`; the v2 design and guide require a physical `TransportCleanup` that this bridge cannot supply. Implementers must either break the retained custom-bridge behavior or fabricate cleanup facts. This is a concrete Python compatibility and diagnosability conflict.

**Remedy:** State explicitly whether v2 applies only when `Client` uses `NativeCoreBridge`. If custom bridges remain supported, preserve `None` for a bridge without `close` and specify how `close_report` behaves there. If support is intentionally removed, name and review that compatibility change.

**Closure test:** Exercise `Client.close()` and context-manager exit with a custom bridge lacking `close`, alongside a native Client with live work. Assert the documented result and report for each without invented physical facts.

### [P3-1] The required stream-drop proof asks for an outcome from a discarded object

**Root cause and location:** Root design §8, line 61, says to “Break or drop a push stream before completion” and then verify both its recovered final result and its `aclose()` summary. After the stream object is dropped, its `aclose()` method is no longer callable. The caller guide line 69 describes two distinct recovery paths: call `aclose()` while retaining the object, or discover the dropped stream through `recent_operations()`.

**Violated invariant:** Each required proof case must be executable and must establish the behavior it claims.

**Credible sequence and impact:** A test retains a stream to call `aclose()` and reports that as coverage of the drop path. That run never exercises object drop, so it cannot prove Client-owned recovery after abandonment. Conversely, a true drop test cannot obtain an `aclose()` summary. The combined instruction permits a false coverage claim.

**Remedy:** Split the proof into two cases: retain the stream and verify `aclose()`’s summary and by-ID result; separately record its ID, drop the stream, and verify discovery and result through `recent_operations()`.

**Closure test:** Review the two separately named test cases and confirm the drop case holds no stream reference when recovery occurs.

## 2. Invariant analysis

The generation and old-handle cancellation rules now agree with the corrected Python pointer. Successful core registration consumes a request ID in that generation; a pre-registration capacity refusal does not. The first-reader reservation is charged before effects and remains available at the aggregate limit. Admitted cancellation and record overflow have allocated typed enum values (`73` and `74`) and a named Python result accessor. Early-abandoned accepted streams retain Client-owned records through the advertised deadline. Omitted per-operation host capacity inherits the Client value, while a differing four-field physical capacity refuses during live work or unfinished cleanup.

The remaining P2 concerns a supported `Client` variant outside the native session’s physical-cleanup authority. I found **no new architectural root cause** in this round; its correction can be bounded to scope and return semantics.

## 3. Risks and next action

Keep the design **NO-GO** until the custom-bridge close contract is reconciled and re-reviewed. Split the stream abandonment proof so the later implementation gate can verify both ownership paths. These corrections do not require reopening the three closed first-round Consistency findings.
