# GWZ sequenced virtual-stream design — CONSISTENCY-AXIS REVIEW

Date: 2026-09-24  
Review object: `gwz-core/dev-docs/GwzTransportSequencedStreamRequirements.md` and `GwzTransportSequencedStreamDesign.md` at core HEAD, introduced against `999dbd89ef816888e77984aff28f48ab4b845722`  
Axis: Consistency; peer-blind, read-only  
Baseline and final recheck: root `38d5235926c61c66f3fda4cc37f4da01b3c140d0`; core `bf60d472f41404316d35927624b82600d0713c63`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All three matched at start and end.

**Verdict: NO-GO — 2 P2 findings.**

## 0. Evidence base

I read both review objects with `git -C gwz-core show HEAD:<path>` and used numbered reads of the unchanged tracked copies for citations. I inspected the root process rules and amendment, workspace and core instructions, `EVIDENCE.md`, the remote-transport requirements and design, placement design, sequencing direction, authoritative GWZ documents, Taut schema, and current mux/stream code. `git status --short` showed unrelated root changes, an untracked core bug report, and a transport `Cargo.toml` change; none was used as review evidence. No files were changed, and no builds or tests were run.

## 1. Findings

### [P2-1] `Close` is given a final sequence although reverse-drain controls can follow it

- **Where:** Sequenced design lines 11–13 and 27; remote-transport design lines 371–378 and 406–426; current `gwz-transport/src/stream/outgoing.rs` lines 18–43 and 90–113.
- **Violated invariant:** Graceful close must drain bounded reverse traffic and replenish its receive credit while preserving each sending direction’s message order. The draft says a sender emits nothing after its terminal, treats `Close` as a graceful terminal, and calls its sequence the sending-direction *final* watermark.
- **Credible sequence and impact:** The initiator sends `EndWrite`, then `Close`. The endpoint continues sending a reverse response during cleanup. After the initial receive window is consumed, the initiator must send another `Window`; it may also owe `Flushed`. Current stream ordering permits those controls after `Close`, and the controlling design requires credit during drain. Under the draft’s final-watermark rule, either those controls are prohibited and cleanup times out, or they are sent after the asserted final sequence. A normal large reverse response can therefore lose graceful close and healthy reuse.
- **Required correction:** Define `Close` as an ordered end-of-forward-write/cleanup request, not the initiator’s final message-sequence watermark. State which `Window` and `Flushed` controls remain legal after it, and reserve final-watermark semantics for actual sender-final messages. Keep `Close`’s own prerequisite that preceding `Data` and `EndWrite` apply first.
- **Closure test:** With a reverse response larger than the initial receive window, send initiator `Close`, then require later initiator `Window` and any owed `Flushed` to advance the drain. Permute delivery within both directions and verify exact bytes, no premature `Closed`, bounded cleanup, and one lease disposition.

### [P2-2] The explicit §4.1 ordered-carrier obligation is left in the controlling graph

- **Where:** Sequenced requirements lines 9 and 25; sequenced design line 5; `GwzRemoteTransportDesign.md` lines 251–255; placement design lines 175–178.
- **Violated invariant:** One profile must have one clear owner for same-stream ordering. Q2 permits same-stream carrier reordering, while remote-transport design §4.1 still says, without a profile exception, “The carrier supplies reliable ordered delivery for an established session.” The new documents explicitly supersede the older design’s §2 ordering text and the placement assumption, but do not name §4.1.
- **Credible sequence and impact:** An implementer following §4.1 rejects or serializes an out-of-order profile-3 frame at carrier admission; an implementer following Q2 buffers it in the virtual stream. Both can cite a controlling clause. This defeats the profile’s central interoperability and proof contract.
- **Required correction:** Explicitly supersede the ordering obligation in §4.1 for profile 3, while retaining its reliability, offset, limit, and malformed-envelope protections. Identify the placement design’s ordered-delivery sentence by section or exact phrase in the same precedence statement.
- **Closure test:** Build an authority matrix for profiles 1, 2, and 3 covering the §2, §4.1, and placement sentences; then use a deterministic in-process case where a valid same-stream higher sequence arrives first and is buffered and later applied under profile 3, while retained profiles preserve their ordered-carrier behavior.

## 2. Invariant analysis

The draft otherwise gives Open and CheckIdentity one positive-ID namespace, makes first initiator frames sequence 1, and distinguishes bounded `Buffered` ownership from `Applied` action admission. Its proposed `message_seq` tag 5 is compatible in shape with the Taut envelope, subject to the draft’s explicit retained-reader qualification. Current `Port::deliver` returns only `Result<(), Error>` and silently accepts a foreign session, so the proposed generation pinning and profile-3 ticket are substantive requirements, not existing guarantees. I found no contradiction in deferring their implementation behind this design gate.

Per-direction ordering can coexist with cross-direction prerequisites: `Closed` must still follow a real initiator `Close` and required `EndWrite`, while byte offsets separately check continuity. The `Close` finding concerns the later controls needed during that same handshake.

## 3. Risks and next action

Correct the two bounded contract defects and re-review the revised settled tuple. A consistency-axis GO would be supportable if the revised text gives `Close` a workable post-close control grammar and makes the profile-3 supersession of §4.1 and placement ordering exact. This review does not qualify implementation, a physical carrier, platform behavior, or release.
