# GWZ internal Taut compatibility amendment — CONSISTENCY-AXIS RE-REVIEW 1

**Date:** 2026-09-24  
**Object:** `gwz-core/dev-docs/GwzTransportInternalTautCompatibilityAmendment.md` at core HEAD  
**Reviewed tuple:** root `ac72a3c984348d9c9a8c7bc56c4590fee17bf378`; gwz-core `343cccc4032be47f8369c4eb297e3a6f50c87f11`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Axis:** Consistency; independent, peer-blind, read-only re-review  
**Verdict: GO — P0: 0, P1: 0, P2: 0, P3: 0.**

## 0 Evidence base

I verified the exact tuple at the start and end; it did not move. I reviewed the committed amendment and its eight-line change from core `e063bb020bc0d9023eff9fc0fa3f6bacbc2f8d8e`. I used the prior Consistency and Safety reports and remediation plan as historical inputs, without reading the current peer report. I checked the remote transport requirements §5.1 G6, remote design §10, placement design §§3, 6, 8 and 9, accepted sequenced requirements Q8 and design §5, `GWZRequirements.md`, `GWZDesign.md`, and the transport Taut schema. I also checked the governing process rules. Unrelated working-tree changes were excluded. No files were changed; no builds or tests were run.

### Prior-finding closure

| Prior finding | Re-review result | Evidence |
| --- | --- | --- |
| Consistency P2-1: the internal transport waiver could extend to outer GWZ requests/responses and ordinary-local old/new core/driver combinations | **Closed** | C1 now limits waived bytes and readers to `gwz-transport` Envelopes and expressly excludes the outer schema and ordinary-local interoperation. C2 preserves G6, ordinary-local old/new fixtures, capability affinity, and explicit-cli/old-core refusal before operation submission. The precedence paragraph repeats that boundary. |

### Changed-range analysis

The change makes the operator’s internal-only decision explicit at the Envelope boundary. Its §3 and §8 placement-design references are now conditional: they supersede historical reader and byte gates only where those gates concern internal transport Envelope bytes as prerequisites for profile 3. Placement §3’s additive GWZ request/response tags and ordinary-local wire compatibility therefore remain controlling. Placement §8’s old/new core/driver and gwz-py ordinary-local fixtures, including explicit-cli/old-core refusal before submission, also remain controlling. I found no new architectural root cause in the changed range.

## 2 Invariant analysis

The waiver of unchanged profile-1/2 Envelope encodings in Q8 and retained transport-reader qualification in sequenced design §5 matches the stated operator decision. C2 still requires current profile-1/2 operations to work after regeneration; C3 keeps `message_seq` absent in those profiles and requires a positive sequence on profile-3 stream frames. The Taut schema currently has no tag 5, so this remains a prospective design amendment, not an implementation claim.

The separate GWZ boundary retains G6’s existing-local-request wire compatibility and remote design §10’s advertised-feature rule for a new driver talking to an old core. Placement §3 requires receiver affinity from capability query through dispatch, and §8 requires refusal of unsupported explicit CLI placement before an operation is sent. The revised amendment preserves all of these obligations. The accepted sequencing, authority, bounded validation, and functional profile-1/2 rules are not superseded.

## 3 Risks and next action

This GO applies to the amendment’s consistency scope at the reviewed tuple. Implementation must still demonstrate the retained ordinary-local old/new core/driver fixtures and explicit-cli/old-core refusal before operation submission, alongside the current Rust/Python Envelope paths. The amendment remains a draft until its required independent review gate is merged; this report does not establish implementation, activation, physical-wire, or release evidence.
