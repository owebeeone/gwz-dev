# GWZ internal Taut compatibility amendment — SAFETY-AXIS REVIEW

Date: 2026-09-24  
Object: `gwz-core/dev-docs/GwzTransportInternalTautCompatibilityAmendment.md` at core HEAD  
Axis: Safety; peer-blind, read-only review  
Reviewed tuple: root `a70fd7557762449174218dafd465dd2632dd964d`; gwz-core `e063bb020bc0d9023eff9fc0fa3f6bacbc2f8d8e`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Verdict: GO — P0: 0, P1: 0, P2: 0, P3: 0**

## 0 Evidence base

I reviewed the committed draft against `AgentProcessRules.md` as amended by `GwzProcessOptimization.md`; the accepted sequenced requirements and design; `GWZRequirements.md`; `GWZDesign.md`; `GwzRemoteTransportPlacementDesign.md` §§3, 6, 8 and 9; and `gwz-transport/protocol/transport.taut.py`. I did not read the peer reviewer’s report. This was a document review; no implementation, build or test was run. In-flight working-tree changes were outside the reviewed object.

## 2 Invariant analysis

C1 waives historical transport bytes, old generated readers and independently deployed old-core/new-driver interoperation. C2 and the precedence clause keep active-session rules in force: profile mismatch and unsupported capability refusal, receiver-generation pinning, malformed-message rejection, bounded resources, and current profile-1/2 functionality. The accepted placement design still requires the host to pin the capability result to the receiving core instance and refuse an unsupported explicit placement before dispatch.

C1 calls for the Rust and Python projections to change together. The placement design’s remaining integration gate exercises the shared transport envelope through both CLI/core and Python/core bindings. Its old-reader qualification is waived as a prerequisite for profile 3; the same-build consumer path is not.

C3 permits a null encoding for an absent `message_seq` on bootstrap and profiles 1/2 while requiring a positive sequence on profile-3 positive-stream frames. The accepted sequenced design’s ordering, authority, ticket, terminal and reassembly rules are not superseded. I found no concrete interleaving in which the stated waiver permits an active frame to acquire authority, apply twice, bypass bounded validation or report graceful success across a missing predecessor.

## 3 Risks and next action

The result accepts the **draft amendment’s safety scope only**. Implementation and activation retain their separate proof gates, including current Rust/Python operation paths, malformed and bounded decoding, profile refusal before effects, and profile-3 stream behavior. Proceed with the independent review gate before treating the amendment as accepted.

The tuple matched the requested commits at both the start and end of this review.
