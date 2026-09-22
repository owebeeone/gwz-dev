# GWZ Remote Transport Placement C — CODE-AXIS REVIEW

**Review object:** Phase 4 Placement C candidate aggregate at root `f3ad29ae5aa55ebd4e558f3f11a078e6b837196e`, controlling document `gwz-core/dev-docs/GwzRemoteTransportPlacementC.md` at core `c5dd307142e6958160efabf36a8521b5f104c157`; implemented proof pending aggregate review.  
**Baseline:** root `f3ad29ae5aa55ebd4e558f3f11a078e6b837196e`; core `c5dd307142e6958160efabf36a8521b5f104c157`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Sources were read from immutable objects with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** Code—architecture, interfaces, call graphs, compatibility, ownership, and claim reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I verified all nine commits and trees at review start and end; they remained unchanged. All member worktrees were clean. Root contained only the six explicitly permitted generated prompt files.

I inspected:

- The complete core delta `feeab90ce11804a2983a1f53d8c12f9e54cb4d27..c5dd307142e6958160efabf36a8521b5f104c157`, especially `message_embedding_tests.rs:21-440`, `python_embedding.rs:1-41`, `python_embedding.py:1-90`, preparation checks, README, and the explicit test-module boundary in `transport_host/mod.rs`.
- Placement C, the controlling design clarification, the accepted public placement guide, and the root checkpoint.
- CLI `clirequest/common.rs:16-39` and `globalargs/dispatch.rs:93-116`: `CliRequest::InitFromSources` owns the core-generated `InitFromSourcesRequest`, and dispatch passes it directly to the core handler.
- The pinned Python codec’s real `decode_message`/`encode_message`, schema and generated-class selection, and the bridge’s origin check and candidate-only substitution.
- Candidate schema composition and generated Rust/Python projections: optional `RequestMeta.transport_message` tag 10 and `ResponseMeta.transport_message` tag 9 use the shared owner `Envelope`; missing new fields are accepted.
- The transport owner’s generated `Envelope`, including version/session/stream/kind and byte-valued `Data.payload`.
- The retained evidence README, raw result tails, inputs, and hashes. Evidence source and consumer hashes exactly matched the reviewed committed blobs.
- Production isolation: the object changes neither production manifests/locks nor production schema/generated artifacts; PyO3 is added only to the externally prepared candidate manifest.

I ran the permitted focused suite against the prepared external candidate. All eight Placement C tests passed in 7.07 seconds, including Rust/Python live exchange, paused-delivery cancellation and closure, malformed Python attachment rejection, and correlation/payload preservation.

## 1. Invariant analysis

The CLI identity claim held. The candidate exercises the same core-generated request/response types and exact handler used by current CLI dispatch. It does not claim executable activation.

The Python claim held. The embedded bridge imports `gwz.protocol.codec` from the pinned `gwz-py/src`, invokes its real decode/encode path, and substitutes only candidate dataclasses, schema, and IR inside the test interpreter. Positive live tests prevent malformed-input coverage from passing merely because codec setup is broken.

Attachment routing and ordinary dispatch remain distinct. Each carried envelope is wrapped with the existing request ID, decoded through the selected consumer, extracted, compared exactly, and delivered to the transport port. The ordinary request is restored byte-semantically without its attachment before the handler runs; the handler invocation counter remains one. Attachment wrappers are not published as operation results, and the final response is separately round-tripped and attachment-free.

Correlation, ordering, and payload attacks failed. The link permits one pending item per direction, preserves direction-local order, pauses before delivery, and carries a deterministic 256 KiB payload larger than the receive window through observed Bind/Bound/Open/Opened/Data/Window traffic. The reconstructed repository bytes equal the source bytes. Cancellation and logical disconnect while delivery is paused terminate with error and leave no member.

Compatibility and isolation held for the claimed stage. Candidate fields are optional and missing-field tolerant; ordinary fields survive round trips. Shared transport definitions come from the owner schema. Production artifacts and dependencies remain unchanged. The external candidate and `cfg_if!` boundary contain the proof.

The future-wire statement is appropriately limited. The wrapper and envelope are serializable CBOR values with explicit correlation, version, session, stream, kind, byte payload, and lifecycle obligations. No pointer or process-local owner crosses the message boundary. The document does not claim framing, carrier authentication, reconnection, process-death handling, split-process interoperability, or iroh qualification.

## 2. Risks and next action

The proof uses an embedded interpreter, loopback SSH, and test-owned forwarding loops. It therefore does not establish production CLI/Python dispatcher activation, physical carrier behavior, platform coverage, selected-source qualification, or wire interoperability; these limits are stated consistently and are not defects in this checkpoint.

Accept Placement C for its narrowed same-process embedding scope. Any later activation or physical carrier must pass its separately deferred gates without treating this GO as that evidence.
