# GwzPyTransportSessionV2 — Surface review, round 2

**Object:** Python caller guide, `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`  
**Baseline:** root `59c13d6184c82789d9dc558ca436b3a43be8bdcd`; `gwz-core` `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; `gwz-py` `ffa184bbb50bf56ff49ebee65a79c228e0bf6489`  
**Date:** 2026-09-24  
**Axis:** Python caller surface; independent, adversarial, read-only  
**Verdict: GO.** The prior blocking surface finding is closed. One new P3 documentation finding does not block this gate.

## Prior-finding closure table

| Prior finding | Disposition | Evidence in revised guide |
| --- | --- | --- |
| P2-1 — Shared pool limit placed on each operation | **Closed for the caller surface.** | Lines 58 and 63 place the default on `Client`, make omitted operation settings inherit it, and show overlapping fetch and push using the same configured limit. Implementation behavior was outside this review. |
| P3-1 — First push and event-reader syntax missing | **Closed.** | Lines 29 and 38 show direct asynchronous event iteration. Line 51 gives clone, fetch and push handle calls and their admission, result and release sequence. |
| P3-2 — Request-ID retry rule unclear | **Closed.** | Lines 53 and 63 distinguish pre-registration refusal, which permits the same ID on a fresh handle, from post-registration worker-launch failure, which consumes it. |

## Changed-range analysis

Compared `gwz-py` `24a4314487ff477eaca0535bdeae3289fead7ab9..ffa184bbb50bf56ff49ebee65a79c228e0bf6489` for the caller guide. The revision adds the missing event and common-operation examples, Client-level pool inheritance, request-ID retry rules, and explicit stream inspection and cleanup behavior. Those changes close the three prior findings. **No NEW ARCHITECTURAL root cause was found** on this surface axis.

## 0. Evidence base

Read only the committed caller guide, the committed `gwz-py/README.md` and `gwz-py/src/README.md`, my first-round Surface report, and the specified guide diff. No code, design document, other reviewer report or unrelated working-tree content was inspected. The guide identifies this API as a draft that is not active in the current candidate.

The exact three-repository baseline tuple above was verified at both the start and end of inspection; it did not move.

## 1. Findings

### [P3-3] Optional push controls have no stated defaults or omission behavior

**Location:** `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md:51`; comparison: `gwz-py/README.md:26–39,69–93`.

**Invariant:** A caller-facing optional argument needs a stated default or an explanation of what omission does, especially when it controls a push destination or check.

**First-day sequence and impact:** A caller follows the documented `start_push(refspec="main:main", targets=["mem_app"])` example in a workspace with more than one remote. The guide says `remote=` and `remote_check=` are optional but does not say which remote omission selects or what check runs by default. The README does not answer either question. The caller must inspect another source before deciding whether that example targets the intended remote and applies the intended check.

**Remedy:** State the defaults and omission behavior for `remote=` and `remote_check=` next to the push example. **Closure test:** A docs-only reader can determine the destination and checking behavior of the example without consulting source or runtime signatures.

## 2. Invariant analysis

From the guide and README alone, a caller can install the package, construct a `Client`, start workspace clone, fetch and push handles, await admission, iterate retained events, inspect a result or typed failure, cancel and join work, release records, and inspect retained outcomes after close. The guide states the main capacity defaults, refusal behavior, record deadline and request-ID retry rule. The remaining optional push defaults are the bounded gap above.

## 3. Risks and next action

This verdict assesses the documented Python surface, not whether the draft is implemented. Add the push defaults before publishing the caller guide as an active API reference.
