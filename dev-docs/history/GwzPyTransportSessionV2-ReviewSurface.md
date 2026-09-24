# GwzPyConcurrentOperationsV2 caller guide — SURFACE-AXIS REVIEW

**Review object:** `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`, DRAFT at `24a4314487ff477eaca0535bdeae3289fead7ab9`  
**Baseline:** `gwz-py` HEAD was `24a4314487ff477eaca0535bdeae3289fead7ab9` at review start and end. The guide and README pages were read with `git show HEAD:`.  
**Date:** 2026-09-24  
**Axis:** Python caller surface. Independent, adversarial, read-only. Nothing here relies on another reviewer. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 interface finding blocks. Two P3 documentation findings remain. I pre-commit to GO on a revision that resolves P2-1 as specified, provided it introduces no new blocking surface defect.

---

## 0. Evidence base

Read `dev-docs/GwzPyConcurrentOperationsV2.md` lines 1–67, `README.md` lines 1–95, and `src/README.md` at the stated HEAD. Verified the SHA with `git rev-parse HEAD` before and after inspection. No code, tests, or design/plan documents were read; no files were changed. The guide explicitly says the API is a DRAFT and is not active in the current candidate, so implementation status is not a finding.

## 1. Findings

### [P2-1] A Client-wide pool limit is configured on individual operations

**Location:** `dev-docs/GwzPyConcurrentOperationsV2.md:51–59`. **Violated invariant:** An option placed on each network operation should have a usable per-operation meaning, or a shared setting should be established at the Client boundary with an inherited default.

**Reproduction:** Accept an operation with `max_connections_per_host=16`; while it remains live, start another operation and omit the keyword. The second call gets the documented default of 32 and `accepted()` refuses it with `TransportCapacityConflict`, even between physical leases. The calls are individually valid, but the shared setting forces every concurrent caller to coordinate an otherwise optional keyword. This defeats the guide’s primary one-Client concurrency workflow and makes independently written fetch/push callers incompatible by default.

**Remedy:** Put the shared physical connection policy on `Client`, and have operation calls inherit it when the keyword is omitted; alternatively, give the per-operation keyword an actual independent limit. State the inheritance and conflict rules explicitly. **Closure test:** With a Client configured for 16, accept overlapping fetch and push handles where neither operation repeats that setting; verify both are admitted within the documented limits.

### [P3-1] The guide does not give enough call syntax for a first push or event reader

**Location:** `dev-docs/GwzPyConcurrentOperationsV2.md:36–47`; comparison: `README.md:26–39,69–93`. **Violated invariant:** A caller guide for new handle methods should let a Python caller invoke and observe each named operation from user-facing documentation.

**Reproduction:** The table identifies `start_push` and `start_clone_workspace`, but describes their arguments only as “Same as existing stream method.” Neither README page gives those stream signatures. The guide also names `events()` without showing whether to await it or iterate it directly. A reader can reproduce the fetch example and the README’s member-clone arguments, but cannot write a push or workspace-clone handle call, or confidently consume its events, from these pages alone.

**Impact:** First-day callers must inspect implementation or runtime signatures for the draft’s central workflows. **Remedy:** Add caller-facing signatures or complete short examples for clone, fetch, and push, including an event iteration example. **Closure test:** A docs-only walkthrough can construct each handle, await `accepted()`, consume `events()`, obtain `result()`, and release it without consulting source.

### [P3-2] The capacity-conflict retry instruction omits the request-ID change

**Location:** `dev-docs/GwzPyConcurrentOperationsV2.md:49,59`. **Violated invariant:** A documented recovery instruction must remain valid when the caller uses a documented option.

**Reproduction:** Supply `request_id="sync-1"` to a handle that receives the documented pre-effect capacity refusal. Following “Create a new handle and retry” with the same arguments reuses that ID, while line 49 says an ID already used by an operation cannot be reused in the Client generation. The retry therefore reaches `InvalidRequest` instead of admission.

**Impact:** Callers using request IDs for correlation can turn a transient capacity conflict into a persistent retry failure. **Remedy:** State whether a refused handle consumes its request ID and give the valid retry sequence, including a new request ID when required. **Closure test:** A documented example retries a refused, explicitly identified operation after the conflict retires and reaches admission without `InvalidRequest`.

## 2. Invariant analysis

The guide names the handle factories for clone, fetch, and push; returns an operation ID before admission; requires explicit `accepted()`; and defines `result()`, `cancel()`, and `release()` as lifecycle counterparts. It distinguishes pre-effect refusal from possible-effect cancellation and warns against automatically replaying a push. It also describes unary/stream cancellation, retained inspection after close, record expiry, and cross-thread loop use. The setting table supplies defaults for every option it lists, and the local-only placement rule is explicit. These attacks did not produce further findings.

## 3. Risks and next action

This documentation-only review cannot establish whether the eventual implementation meets the draft’s cancellation, retention, or cross-thread promises. Revise the shared pool option’s placement or semantics, then update the caller examples and retry guidance before treating the draft as a stable Python interface.
