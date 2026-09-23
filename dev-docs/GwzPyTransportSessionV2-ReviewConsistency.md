# Python concurrent transport session v2 — CONSISTENCY-AXIS REVIEW

**Review object:** DRAFT `dev-docs/GwzPyTransportSessionV2Design.md` at root `f874d421d9f0f453b673d1de17a1cf4fa4fc621e`, with the committed core candidate amendments and Python caller guide/design pointer.  
**Baseline:** root `f874d421d9f0f453b673d1de17a1cf4fa4fc621e`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `24a4314487ff477eaca0535bdeae3289fead7ab9`. Sources were read with `git show HEAD:<path>`.  
**Date:** 2026-09-24.  
**Axis:** Consistency — internal agreement, controlling contracts, exact supersession, and satisfiable evidence. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — three P2 findings block design acceptance.

---

## 0. Evidence base

I read the complete v2 design (`:1–61`), the Python v2 caller guide (`gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md:1–67`), the accepted Python transport design (`gwz-py/dev-docs/GwzPyTransportDesign.md:49–229,274–292`), the rejected concurrency design and its prior Consistency findings, and `GwzPyTransportConcurrencyDesign-Verdict-2.md`. I checked the candidate amendments in core’s retry plan (`:465–467`), remote transport plan (`:444–452`), and v1.1.0 plan (`:446–453`), plus the accepted sequenced-stream design’s request, ticket, generation and compatibility clauses. I inspected `gwz-core/protocol/gwz.taut.py:669–830,1146–1157,1696–1737`, the outer request/response compatibility boundary, and the existing Python event-reader path.

Process authority was `dev-docs/AgentProcessRules.md` as amended by `dev-docs/GwzProcessOptimization.md`, and the review-loop skill and canonical report template. Inspection used `git rev-parse HEAD`, `git show HEAD:<path>`, `rg`, `sed`, and `nl`. No file was changed; no build or test ran. The three heads matched the baseline at both the start and end of review.

## 1. Findings

### [P2-1] Rollover leaves accepted Python lifetime and cancellation clauses unsuperseded

**Location and invariant:** The v2 design `:9,19,39` permits a new runtime generation after 256 registrations and reuse of a caller request ID in that later generation. Its supersession list names the one-active-operation and related rules, but does not replace the accepted Python design’s statement that the installed host is stable until close (`GwzPyTransportDesign.md:55–64`) or its cancellation rationale that a retained handle cannot cancel a later request because “the session never reuses a request ID” (`:153–164`). The Python candidate pointer at `:274–286` describes concurrency and post-close access, but does not resolve these generation promises.

**Counterexample and impact:** Run 256 sequential network operations without closing the Client, retain an old cancellation handle, and attempt operation 257 with a caller request ID used in the old generation. V2 requires shutdown and replacement of the runtime and permits that ID. The still-accepted Python clauses require the host to remain installed and the ID never to recur. An implementation cannot meet both contracts; carrying a cancellation lookup keyed only by request ID into the replacement generation could target the new operation.

**Required correction:** Explicitly supersede the two accepted Python clauses for runtime generations. State that endpoint configuration remains captured while runtime instances may roll over, and that cancellation authority is pinned to the original operation and core generation, even when a caller request ID recurs.

**Closure test:** Trace operation 257 through a completed rollover, reuse an old-generation caller request ID, then invoke the retained old cancellation handle. The new operation must remain unaffected; the old handle must report only its own retained outcome.

### [P2-2] Accepted stream work can be denied its event reader after effects begin

**Location and invariant:** V2 `:35,37,47` makes result-budget refusal a pre-acceptance outcome and says aggregate pressure cannot turn a within-limit success into a post-effect failure. Yet each of eight accepted operations may reserve 8 MiB, exhausting the 64 MiB ledger exactly. Reader cursors are charged to that same ledger only when a reader is created; `:47` explicitly permits reader creation to refuse `TransportSessionFull`. The caller guide `:36,59` promises event iteration for an admitted handle and says a stream helper performs admission on its first iteration.

**Counterexample and impact:** Begin eight stream helpers with no event reader yet created. All eight pass admission and reserve 64 MiB; their workers may start Git work. When a helper creates its event reader, even the small cursor charge cannot fit. The caller receives `TransportSessionFull` after acceptance and possible Git effects, despite the guide placing session-budget refusal before effects. The operation may succeed internally while its stream caller cannot observe the promised events.

**Required correction:** Reserve the reader metadata needed by each admitted stream form before `Accepted`, within a stated allowance or a separate bounded reader budget. Define how additional optional readers are refused without changing the admitted stream’s access.

**Closure test:** Admit eight stream operations at the aggregate reservation limit, then begin each required event iterator while their workers are live. Every admitted stream must be able to receive its terminal event; an additional optional reader may receive the documented bounded refusal.

### [P2-3] The only surviving unary-cancellation result has no specified accessor

**Location and invariant:** V2 `:17` releases the internal ledger record after cancelling a unary or stream helper and says `GwzOperationCancelled` contains a detached terminal `OperationResult`. Section 7 (`:53`) names `.response` only for `GwzOperationError`. The caller guide `:63` tells callers to catch `GwzOperationCancelled` to decide whether a push may have taken effect, but does not name a property through which they obtain its detached result. The required proof at `:61` asks that the result remain reachable after close.

**Counterexample and impact:** Cancel an admitted unary push after remote acceptance. Cleanup completes, the ledger record is released, and the caller catches `GwzOperationCancelled`. The exception is now the sole owner of the detailed terminal result, but the public contract specifies only that it “contains” one. A caller or regression test must guess an attribute or parse exception text to inspect the typed result and transport facts. This reopens the prior NO-GO’s diagnosability gap for the convenience call forms.

**Required correction:** Specify a stable named result property on `GwzOperationCancelled` for both handle-admission and unary/stream cancellation, including its absent value before admission. Align the guide’s recovery instructions with that name.

**Closure test:** Cancel an admitted unary push, close the Client, then read the named property from the caught exception and assert the typed terminal code, both IDs, available nonsecret transport facts, and `effect="possible"` without consulting the released ledger or parsing text.

## 2. Invariant analysis

The prior accepted-start identity counterexample is addressed in the v2 text: a synchronous factory issues an operation ID before admission, and cancellation of `accepted()` retains a reachable handle (`:13–17`). Issued serials, a session nonce and a fixed-size high-water mark give a consistent distinction between expired same-session IDs and foreign or never-issued IDs (`:13,49`).

The prior post-close outcome and active-reader counterexamples are also answered in the document contract. The ledger survives host shutdown, retained handles remain readable, and readers already waiting at close can receive and drain a terminal event (`:43–49`; guide `:65`). The close report includes bounded operation summaries for work live when close began.

The capacity amendments agree on equal four-field capacity joining without reinstallation and different capacity refusing while work or cleanup remains (`v2:27–31`; retry candidate `:465–467`; Phase 2 candidate `:444–452`). The current outer error enum ends at `url_scheme_unavailable=72`, so the proposed appended `cancelled=73` and `transport_record_limit=74` values are available (`v2:53`; `gwz.taut.py:827–830`). This closes the prior unrepresentable-terminal-code contradiction at the design level. The accepted sequenced-stream design’s ticket, generation and host proof remain a separate activation gate.

## 3. Risks and next action

The outer GWZ request/response schema has a separate compatibility contract from the internal transport Envelope waiver. V2 adds error enum values within the append-only rule, but its broad statement that the Taut schema is internal should be narrowed when core requirements and design are updated; this observation is below the finding bar because these new terminal results are specified for the Python session path.

Keep this design **NO-GO**. Correct the generation supersession, guaranteed reader admission for stream forms, and detached cancellation-result accessor on one pinned document tuple, then re-trace the three counterexamples before implementation authority is granted.
