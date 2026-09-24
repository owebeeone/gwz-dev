# Python transport session v3 caller note — SURFACE-AXIS REVIEW

**Review object:** `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`, specifically the committed addition defining `request_id_consumed` and its retry example. Compared with the previous committed guide and the committed `gwz-py/README.md` and `gwz-py/src/README.md`.

**Tuple:** root `b35ea74bf7e73c15777a3e0fb18587d05faffb17`; `gwz-core` `58e25012449ee8f4609daba4939e7157e99ea488`; `gwz-py` `d29d450508138bda9251e797afdb21d71d20d8dc`.

**Date:** 2026-09-24. **Axis:** Surface. **Independence:** Peer-blind review; no other-axis reports consulted. **Method:** Read-only; the specified commit tuple matched at both the start and end. The guide labels the API and addition as draft; implementation absence is outside this review.

**Verdict: GO** — no open P0, P1, or P2 findings.

## 0. Evidence base

Read the review-loop process authority, `AGENTS_GWZ.md`, the guide at `HEAD` and `HEAD^`, and the two specified README files at `HEAD`. The guide’s new material is the status-note addition, the `request_id_consumed` explanation, a retry code block, and a clarification that pre-effect refusal leaves a typed failure with this field. README content is unchanged. No implementation, design, or plan was read.

## 1. Findings

**P3-1 — The retry example leaves the rest of the guide inside a code block**  
Location: `dev-docs/GwzPyConcurrentOperationsV2.md:61`

The closing fence is followed on the same line by prose: ````` A fresh generated request ID is used when you omit it.`` Markdown treats that as an opening fence with an info string, not a closing fence. A caller reading the guide in a Markdown renderer can therefore see the remaining settings, cancellation, and recovery guidance rendered as code.

**Impact:** Bounded documentation usability defect; the guidance remains present in the source.

**Correction:** Put the closing fence on its own line and move the generated-ID sentence into ordinary prose.

**Closure test:** Render the guide and confirm the retry snippet ends at its intended boundary and subsequent paragraphs and tables render normally.

**P3-2 — The retry example abandons the refused handle and never attempts admission on the replacement**  
Location: `dev-docs/GwzPyConcurrentOperationsV2.md:56–60`

In the documented refusal path, the example assigns a fresh handle over the refused one without releasing it, then ends without calling `accepted()` on the replacement. The guide says refused handles can be released and describes a 64-record retention limit. A caller who repeats this pattern for pre-registration refusals can retain refused records until capacity is exhausted, and the shown code does not actually retry admission.

**Impact:** Bounded lifecycle and example defect; repeated use can block new work with `TransportSessionFull`.

**Correction:** Show the caller releasing the refused handle before replacing it, then awaiting admission on the new handle with the same request ID. Show the consumed-ID branch choosing a new ID or stopping the retry.

**Closure test:** Walk the example through both `request_id_consumed` values; confirm the reusable-ID branch releases the refused record and attempts admission again, while the consumed-ID branch does not reuse that ID.

## 2. Invariant analysis

**First-day walkthrough:** From the draft caller guide, a reader can create a client, start fetches, await acceptance, inspect events and results, cancel and release a handle, and distinguish operation IDs from caller-provided request IDs. The new explanation states when IDs are consumed, what each boolean value means, and that error codes alone are insufficient. The only illustrated reuse path is incomplete as described in P3-2.

The guide specifies a fresh generated request ID when `request_id=` is omitted. It explains request-ID validity constraints, per-client and per-generation reuse, consumed IDs after core registration, and reuse after pre-registration refusal. It also describes finding retained stream results through `recent_operations()` and inspecting or releasing them by ID. For task cancellation, it documents the detached response and identity; for `handle.accepted()` cancellation, it says the handle remains inspectable and releasable. These paths were sufficiently discoverable in the guide.

**Attacks that held:** The property name `request_id_consumed` is explicitly defined as a boolean, and both values have stated caller actions. The guide names the refusal cases it covers and says descriptors from `recent_operations()` carry the field. Default omission behavior is stated. Retained results after cancellation or early stream abandonment have documented discovery and inspection routes.

The package README introduces the Python API and existing stream methods, but does not link to or describe this draft session guide. Since the guide explicitly marks the methods inactive in the current candidate, I treat that as draft placement, not a release-blocking surface defect.

## 3. Risks and next action

The guide is marked DRAFT and says the described methods and concurrent behavior are not active in the current candidate. Before presenting this note as a caller-ready guide, fix the code-fence boundary and complete the retry example’s release-and-retry lifecycle.
