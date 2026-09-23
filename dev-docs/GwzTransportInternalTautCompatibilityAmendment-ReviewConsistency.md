# GWZ internal Taut compatibility amendment — CONSISTENCY-AXIS REVIEW

**Object:** `gwz-core/dev-docs/GwzTransportInternalTautCompatibilityAmendment.md` at core HEAD  
**Tuple:** gwz-dev `a70fd7557762449174218dafd465dd2632dd964d`; gwz-core `e063bb020bc0d9023eff9fc0fa3f6bacbc2f8d8e`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Date:** 2026-09-24  
**Axis:** Consistency; independent, peer-blind, read-only review  
**Verdict: NO-GO — one P2 finding.**

## 0 Evidence base

I verified the tuple at the start and end; it did not move. I reviewed the committed amendment against the accepted sequenced requirements and design, `GWZRequirements.md`, `GWZDesign.md`, placement design §§3, 6, 8 and 9, and `gwz-transport/protocol/transport.taut.py`. I also checked the controlling process rules and the remote transport requirements and design for compatibility obligations. Uncommitted workspace changes were excluded. No files were changed and no tests were run.

## 1 Findings

**P2-1 — The old-core/new-driver waiver crosses from the internal transport Envelope into the GWZ request boundary.** At amendment **C1, line 7**, “old-core/new-driver interoperation” is waived without limiting that phrase to transport Envelope projections. The **Precedence paragraph, line 13**, also waives placement §3’s “retained old readers” and “wire compatibility is tested” gates, even though §3 defines the outer GWZ request and response schema.

The controlling [remote transport requirements §5.1 G6](gwz-core/dev-docs/GwzRemoteTransportRequirements.md) require existing local requests to remain wire-compatible. [Remote transport design §10](gwz-core/dev-docs/GwzRemoteTransportDesign.md) says a new driver talking to an old core may use only advertised features, and [placement design §8](gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md) requires old/new core/driver ordinary-local combinations to be tested. These obligations cover separately installed GWZ drivers and cores, unlike the internal transport messages addressed by the operator’s clarification.

A concrete sequence is a new driver generated after this change sending an ordinary local GWZ request to an older core. That request need not contain a transport Envelope. If C1 is used to remove the old-core/new-driver qualification, regeneration of the outer request projection can add or alter an encoded slot that the older core rejects or misreads. The accepted ordinary-local compatibility guarantee would then be lost, despite no profile-3 transport use.

**Correction:** Limit C1 and the placement §3/§8 supersession expressly to the *internal transport Envelope* and its generated projections. State that GWZ request/response compatibility, ordinary-local old/new core/driver combinations, capability affinity, and refusal of unsupported explicit placement remain controlling. **Closure:** review the revised boundary against G6 and placement §§3 and 8; retain an ordinary-local new-driver/old-core and old-driver/new-core fixture, plus explicit-cli/old-core refusal before operation submission. These are GWZ boundary checks, not historical transport-byte or old transport-reader tests.

## 2 Invariant analysis

The amendment correctly preserves the distinction between profile-3 sequencing and current profile-1/2 functional behavior. Its Q8 and sequenced-design §5 citations are accurate: Q8 requires unchanged profile-1/2 encodings, while §5 conditions null encoding on old-reader acceptance. Removing those historical transport-byte gates is consistent with the stated internal-only operator outcome. C2 retains profile refusal before Git, credential or pool effects; C3 remains feasible with an optional, missing-ok tag 5 and lockstep Rust/Python generation.

The placement citations are textually accurate, but §3 concerns the outer GWZ schema as well as transport integration. Placement §6 still requires version isolation and preservation of existing v1 semantics; §8 also contains functional ordinary-local and unsupported-feature checks. `GWZRequirements.md` and `GWZDesign.md` carry the accepted profile-3 sequencing rules without independently imposing historical transport-byte compatibility. Process rule L2-04 concerns durable-record readers and does not create a transport-message reader obligation.

## 3 Risks and next action

Resolve P2-1 by drawing the transport Envelope versus GWZ request/response boundary explicitly, then re-review the amended text. The review can become **GO** without restoring historical serialized transport-byte or old generated transport-reader qualification.
