# Python concurrent transport session v2 — SAFETY-AXIS REVIEW

**Review object:** DRAFT `dev-docs/GwzPyTransportSessionV2Design.md` at root `f874d421d9f0f453b673d1de17a1cf4fa4fc621e`, with candidate core amendments and Python caller documents at the tuple below.  
**Baseline:** root `f874d421d9f0f453b673d1de17a1cf4fa4fc621e`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `24a4314487ff477eaca0535bdeae3289fead7ab9`. Committed sources were read with `git show HEAD:` and numbered inspection.  
**Date:** 2026-09-24.  
**Axis:** Safety — degraded paths, irreversible effects, disclosure, stuck states, races and blast radius. Independent, adversarial and read-only. The parallel axis did not inform this report.

**Verdict: NO-GO** — two P2 findings block design acceptance.

---

## 0. Evidence base

I read the full session v2 design, the Python concurrent-operations v2 guide and transport design, the previous rejected design and second Safety review/verdict, and the relevant core retry, transport, v1.1 and accepted sequenced-stream clauses. I checked the current Taut `GwzErrorCode` and `OperationResult` definitions to assess terminal representation. Process authority was `dev-docs/AgentProcessRules.md` as amended by `dev-docs/GwzProcessOptimization.md`, using the review-loop report template.

Inspection used `git rev-parse HEAD`, `git show HEAD:<path>`, `rg`, `sed` and `nl`. I made no writes or Git mutations and ran no builds or tests. The exact three-repository tuple matched at the start and final recheck. Implementation acceptance, profile-3 host proof, platforms and publication were outside scope.

## 1. Findings

### [P2-1] Stream admission can exhaust the budget needed to read its events

**Location and invariant:** Session v2 design §§5–6, lines 35–37 and 43–49; Python caller guide lines 36 and 59–65. An accepted streaming operation must have a viable path to deliver its events and terminal outcome. Admission reserves the full 8 MiB operation allowance against a 64 MiB ledger, while event-reader cursors are charged separately to that same ledger. Reader creation may refuse `TransportSessionFull`; the design does not reserve a stream helper’s required reader before effects.

**Counterexample:** Admit eight operations, each reserving 8 MiB. The ledger has no remaining charge for a reader cursor. A `push_stream` helper can pass admission on its first iteration, allowing Git and credential work to begin, then fail when it creates its event reader. The specified reader refusal leaves the operation outcome unchanged and supplies no detached result or possible-effect classification. As the helper unwinds and loses its internal handle, the completed record can then be released under the last-handle rule. The same problem can occur with fewer operations when retained bytes leave exactly one 8 MiB allowance available.

**Impact:** A caller can receive a capacity error after a push may have changed a remote ref, without the terminal result or warning that the cancellation and record-overflow paths promise. Aggregate pressure thus reaches a post-effect failure on the streaming surface.

**Required correction:** Reserve the first stream reader’s metadata as part of admission, or establish and charge that reader before acceptance can release the worker gate. Optional additional readers may still refuse when the ledger is full. Define the attributed terminal path if a stream reader fails after acceptance for another reason.

**Closure test:** Fill the ledger so an operation’s 8 MiB allowance fits but an additional cursor does not. Start a streaming push and force remote acceptance. Show either a pre-effect `TransportSessionFull` refusal or successful delivery of its terminal result and conservative effect evidence. Repeat with eight simultaneous admissions and with an existing reader consuming charged metadata.

### [P2-2] Early stream abandonment can erase the outcome of a continuing push

**Location and invariant:** Session v2 design lines 17 and 43–49; Python caller guide lines 38–49 and 61–65. Every admitted mutating operation that may continue after its caller stops consuming events needs an attributable outcome or conservative possible-effect warning. The existing stream helpers return iterators rather than handles. Dropping a started handle does not cancel its operation, and its completed record is released when no handle or active reader remains. A close report includes only operations live when close began.

**Counterexample:** Start `push_stream`, receive one progress event, then close or drop the iterator. Its internal handle and reader disappear while the admitted push continues. The remote accepts the update and the operation terminalizes before `Client.close()`. The last-handle rule releases its record despite the Client remaining open; the close report omits it because it was no longer live when close began. Retaining the operation ID string does not pin the result. The caller has no handle, result, cancellation exception or close summary from which to determine whether replay is safe.

**Impact:** A normal early-exit path for a convenience iterator can conceal an irreversible remote effect. It also makes by-ID recovery ineffective for that operation after completion.

**Required correction:** Define ownership transfer when a stream consumer closes early. Either cancel and join the operation and deliver its attributed possible-effect outcome through a documented channel, or retain an inspectable Client-owned terminal record through its advertised deadline and provide a way to discover its ID. Keep the bound on abandoned records explicit.

**Closure test:** Start a streaming push, consume one event, close the iterator, arrange remote acceptance and let the operation finish before Client close. Verify that the caller can obtain its ID, terminal outcome and conservative effect classification without relying on an event iterator or a still-live close summary. Repeat beyond 64 early exits to verify bounded retention and recovery.

## 2. Invariant analysis

The previous close-outcome counterexample is addressed for retained handles: close joins live operations, preserves their ledger, reports bounded per-operation summaries and permits post-close reads without rebuilding the host. Cancellation while awaiting `accepted()` retains the already-returned handle; cancellation of a unary or stream helper carries a detached result and releases its inaccessible record. The new enum values give admitted cancellation and self-overflow stable terminal codes. These corrections do not cover reader-allocation failure after stream admission or early iterator abandonment.

The capacity rule prevents different physical limits from replacing a live operation’s pools, including between leases; equal limits join without reinstallation. Rollover waits for tickets and physical cleanup, uses captured endpoint configuration and separates old Port identity. The stated nonce checks isolate records across Clients with identical caller request IDs. Explicit CLI placement refuses before credential access, and the outcome and close-report fields exclude secrets. Those attacks produced no separate finding in this document contract.

## 3. Risks and next action

Keep the design gate **NO-GO**. Specify reader reservation and early stream-consumer ownership in the root design and caller guide, then re-trace both post-effect sequences on a new pinned tuple. The final tuple remained root `f874d421d9f0f453b673d1de17a1cf4fa4fc621e`, gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`, gwz-py `24a4314487ff477eaca0535bdeae3289fead7ab9`.
