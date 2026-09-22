# GWZ Remote Transport Placement C — STATE-AXIS REVIEW

**Review object:** Phase 4 Placement C candidate aggregate, core `feeab90ce11804a2983a1f53d8c12f9e54cb4d27..c5dd307142e6958160efabf36a8521b5f104c157`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPlacementC.md`; implemented proof pending aggregate review, dated 2026-09-22.  
**Baseline:** root `f3ad29ae5aa55ebd4e558f3f11a078e6b837196e`; core `c5dd307142e6958160efabf36a8521b5f104c157`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; Taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `d096a9dcf0d43e79ea32bced5b802bf8a877ce1d`. Committed sources were read with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** State—message ownership, bounded progress, cancellation/closure, cleanup accounting, Python interpreter state, and fail-closed dispatch. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P2 finding blocks. One P3 evidence defect remains; it is not a new architectural root.

---

## 0. Evidence base

I read the exact nine-repository tuple, complete core diff, Placement C checkpoint, operator clarification and controlling Placement sections, embedding guide lifecycle, the complete 440-line Rust embedding fixture, Python/PyO3 bridge, candidate preparation changes, actual `gwz-py` codec, and the unchanged CLI request/dispatch sites.

Evidence attribution was checked against the private Placement C run. Committed hashes for the Rust fixture, both Python bridge files, actual Python codec, and two CLI consumer sources match `inputs.json`.

The permitted focused `message_embedding_tests` run passed all eight tests serially. I also ran targeted healthy, cancellation, and carrier-close cases while examining bounded cleanup behavior. All tuple revisions matched at both review boundaries; member trees remained clean. Root contained only the excluded generated prompts and four old N2b prompts. No current peer report was present or read.

## 1. Findings

### [P3-1] The lifecycle fixture discards every cleanup report

**Location:** `gwz-core/src/transport_host/message_embedding_tests.rs:366-370`; cleanup semantics in `src/transport_host/mod.rs:181-211,223-225` and `docs/TransportPlacement.md:252-261`.

**Violated invariant:** The C evidence should distinguish successful physical retirement from bounded logical return with retained local work. `CleanupReport.pending_local_work` is the contract’s mechanism for making that distinction.

**Counterexample:** Introduce a regression that leaves a stream, check, or endpoint job owned through the cleanup deadline. The operation can still return an error, the member remains absent, and each `finish`/`shutdown` future returns normally with a nonzero report. The fixture discards the results of `scope.finish`, `client_request.finish`, `endpoint.shutdown`, and `runtime.shutdown`, so all cancellation/closure tests remain green.

**Impact:** The proof establishes waiter release and bounded teardown calls, but cannot substantiate worker retirement or retained-work accounting. A resource-retention regression at this embedding boundary would be accepted silently.

**Required correction:** Capture each cleanup report. Assert the intended intermediate accounting and that final same-process endpoint/runtime shutdown leaves no unaccounted local work; if an intermediate cancellation report legitimately remains nonzero, assert it explicitly and prove later owner shutdown retires it.

**Closure test:** Make a retained job observable through the existing test seam and show the lifecycle test fails on unexpected pending work, then passes when disposal completes.

## 2. Invariant analysis

The central embedding attacks otherwise held:

- Rust uses the same generated `InitFromSourcesRequest` held by `gwz-cli` and the same direct core handler boundary; operation dispatch is counted exactly once.
- Python runs the actual `gwz.protocol.codec` under PyO3 in the Rust process. The bridge verifies the codec’s source origin, installs only candidate dataclasses/IR in the test interpreter, and serializes bytes rather than carrying Python or Rust owners.
- Full request/response wrapper roundtrips preserve request ID, ordinary fields, session/stream identity, and transport payload. A malformed present Python attachment fails decoding rather than becoming omission.
- Each forwarding direction has one `Option<Attachment>` pending slot. A received item is either delivered or followed by disconnect; two independent directions preserve order and progress.
- Delivery is first paused before preflight and later after removing the first Data message but before delivery. The 256 KiB payload exceeds the 64 KiB receive window, so exact reconstructed bytes plus blocked completion demonstrate Data/Window credit progress rather than a single buffered transfer.
- Cancellation and port loss wake the blocked operation, produce an error, avoid member creation, and allow the explicit finish/shutdown sequence to return. They do not claim rollback or peer cleanup.
- The docs accurately limit the result to same-process embedding and future-wire plausibility. They do not claim a physical carrier, separate processes, frontend activation, platform qualification, or wire interoperability.

## 3. Risks and next action

Physical wire and split-process behavior, iroh, platform and selected-source qualification, frontend activation, HTTPS, publication, and release remain correctly deferred.

Accept Placement C on the State axis. Before citing this fixture as physical-cleanup evidence in a later activation gate, close P3-1 by asserting the returned cleanup accounting.
