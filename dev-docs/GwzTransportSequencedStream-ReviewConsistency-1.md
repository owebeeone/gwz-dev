# GWZ sequenced virtual-stream design — CONSISTENCY-AXIS RE-REVIEW 1

Date: 2026-09-24  
Review object: `gwz-core/dev-docs/GwzTransportSequencedStreamRequirements.md` and `GwzTransportSequencedStreamDesign.md` at core `b00a59229a3bc48fdb102cad61b9c9889a2ab1d6`, changed from `bf60d472f41404316d35927624b82600d0713c63`.  
Axis: Consistency; independent, read-only.  
Exact tuple, verified at start and end: root `5774bd5293f68f1685df5352251c90a2318ca374`; core `b00a59229a3bc48fdb102cad61b9c9889a2ab1d6`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`.

**Verdict: GO — design contract only. P0: 0; P1: 0; P2: 0; P3: 0.** Both prior consistency findings are closed. No new architectural root cause was found. This verdict does not accept implementation or activation.

## Prior-finding closure table

| Prior finding | Closure evidence | Disposition |
| --- | --- | --- |
| P2-1: `Close` incorrectly fixes the initiator’s final message sequence | Requirements Q4 now calls `Close` a cleanup request and permits later `Window` and owed `Flushed` during bounded reverse drain (requirements line 13). The design makes `EndWrite` a half-close, reserves final sequence watermarks for actual sender-final messages, orders post-`Close` controls, and requires complete reverse drain before successful `Closed` (design lines 11, 13, 27). Its directed proof case covers a response larger than initial credit (line 43). | Closed |
| P2-2: remote design §4.1 remains an unqualified ordered-carrier authority | Both drafts now identify the exact profile-3 exceptions to remote design §2 and §4.1 and placement design §4, while preserving reliability, validation and limits (requirements line 25; design line 5). Profiles 1/2 explicitly retain ordered-carrier behavior. | Closed |

## Changed-range analysis

The revision changes only the two reviewed drafts: six insertions and six deletions in the requirements, and ten insertions and ten deletions in the design. The changed clauses give `Close` an ordered prerequisite without making it sender-final; allow only reverse-drain `Window` and owed `Flushed` afterward; retain the finite close deadline; and name the superseded carrier-ordering sentences precisely. I traced those changes against the prior findings and the existing close and credit rules. They introduce no contradictory authority or unsatisfiable terminal path.

The endpoint can withhold successful `Closed` until reverse bytes are drained and required flush acknowledgements have arrived. An initiator `Window` needed to finish that drain is applied in its own sequence after `Close`. A late control that is no longer needed remains charged and contained by the cleanup fence; it cannot revise successful facts or dispose of a lease twice (design lines 27, 29–31). The specified proof case must exercise this distinction.

## 0. Evidence base

I read the committed review objects and their change from the prior core commit; the historical Consistency and Safety reports and remediation plan; root process rules and amendment; workspace and core instructions; remote-transport requirements §5.4, remote-transport design §§2, 4.1 and 6, placement design §§4 and 6, the sequencing direction, and the authoritative GWZ requirements and design. I inspected the Taut envelope and current codec/stream shape only for design feasibility. The Taut envelope has an unused tag 5 position (`gwz-transport/protocol/transport.taut.py`, lines 72–85); the design correctly makes retained-reader qualification a gate before activation (design line 41).

`git status --short` showed unrelated root, core and transport working-tree changes. None supplied review evidence. No files were modified; no builds or tests were run.

## 2. Invariant analysis

| Authority | Profiles 1 and 2 | Candidate profile 3 |
| --- | --- | --- |
| Remote requirements §5.4 | S1–S9 apply, including preserved bytes and order. | S1–S9 protections remain; Q2 places same-stream order reconstruction in the virtual stream (requirements lines 9, 25). |
| Remote design §2 | Carrier adapter provides ordered delivery; in-process delivery enforces the same ordering (lines 58, 69–72). | Those ordering responsibilities are superseded by the draft’s explicit profile-3 clause (design line 5). Size, lifecycle and flow-control protections remain. |
| Remote design §4.1 | Carrier supplies reliable ordered delivery (lines 250–255). | Only its carrier-ordering obligation is superseded. Reliability, offset, credit, envelope-validation and limit protections remain (design line 5). |
| Placement design §4 | Host adapter provides asynchronous, ordered bidirectional delivery (lines 173–178). | The ordered-delivery obligation is superseded; asynchronous handoff, request correlation, bounded queues and registration remain (design line 5; requirements line 25). |
| Placement design §6 | `CheckIdentity` is profile-2-only under the accepted text (lines 269–291). | Draft expressly extends the same check body and terminal rules to profile 3 (design line 9). |

The sender-final grammar is coherent. Each sending direction sequences all message kinds. `EndWrite` and `Close` require their own earlier messages but do not end the initiator sequence; a successful endpoint `Closed` or identity terminal requires its full preceding sending-direction prefix. A validated abortive terminal at sequence N can instead supersede missing predecessors below N, while a frame above N from that sender is invalid (design lines 11–13, 27–29). The independent byte offsets check continuity, and the real `Close` and required `EndWrite` remain cross-direction prerequisites for `Closed` (line 27). Thus sender sequence order does not falsely claim a total order between directions.

The receiver’s `Buffered` result is bounded ownership rather than application. An Open/check receives `Applied` only after action-queue and route admission; a cancelled waiter cannot drop the host’s unresolved ticket (design lines 19–23). The fixed gap deadline, predecessor reserve and finite cleanup deadline provide failure outcomes rather than a successful close over missing controls (lines 31, 35–37). These clauses make the proposed directed and seed-replayable proof cases satisfiable in principle.

## 3. Risks and next action

Accepting these drafts would freeze a design contract, not prove its implementation. Before activation, the implementation gate must establish the aggregate memory and ticket budget, retained profile-1/2 decoding, and the directed post-`Close` reverse-drain case, including permuted `Window` and `Flushed` delivery, exact bytes, finite cleanup and one lease disposition (design lines 37, 41–45). The accepted Q-rules must first be incorporated into `GWZDesign.md` and `GWZRequirements.md` as the drafts require (requirements line 25; design line 45).
