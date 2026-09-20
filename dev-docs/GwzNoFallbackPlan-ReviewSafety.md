# GwzNoFallbackPlan — SAFETY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackPlan.md` at core `6c592c924e248399cedbdffaafe49d5012870ce5`  
**Baseline:** root `d3b6cada131fde80726703236ac8eec694a89ccb`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`  
**Date:** 2026-09-20  
**Axis:** Safety — what the plan permits to go wrong. Independent, peer-blind review.

**Verdict: GO**

Open findings: **P0 0 · P1 0 · P2 0 · P3 0**.

The plan is safe to accept as a draft execution plan. It does not authorize implementation, activation, publication, or an interface freeze.

## 0. Evidence base

I read the review object and controlling design, requirements, transport, native-binding, adapter-foundation, checkpoint, process, and repository instruction documents from their committed objects. I inspected the relevant current subprocess boundaries in local fetch, commit, tag creation/deletion, and path history.

The exact commit/tree tuple was verified both before and after review:

| Repository | Commit | Tree |
|---|---|---|
| root | `d3b6cada131fde80726703236ac8eec694a89ccb` | `d162523a2ac732f5e0621d3c6b2eac2c81e046c8` |
| core | `6c592c924e248399cedbdffaafe49d5012870ce5` | `acefb822df32f06cf70d46f3b3439538b6537c69` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

No builds, tests, network operations, file writes, or repository mutations were performed.

## 1. Invariant analysis

**Sequencing and reversibility:** The plan first establishes the dependency baseline and refreshes the complete launch-site inventory. Existing paths remain in place until each replacement satisfies its acceptance gate. Adding the fork as a workspace member explicitly does not activate, publish, or route production behavior through it.

**Local object transfer and reference publication:** The local-fetch lane requires characterization of non-committish references and `FETCH_HEAD`, comparison of implementation routes before coding, complete object closure, cancellation and failure semantics, and prevention of incomplete-reference exposure. These gates cover the dangerous boundary where transferred objects become visible through refs.

**Commit and tag mutation:** The lane requires an explicit design for identity, configuration, environment, dates, messages, hooks, signing formats, staging, tracked-only `-a`, ref and index effects, concurrent ref changes, interruption, worktrees, and failure aftermath. This is consistent with the product design’s documented possibility of aggregate partial failure after unexpected post-mutation errors. The standing durable-owner and full-preflight process rules remain applicable at the eventual mutation boundary.

The source also contains a direct `git tag -d` path beyond the concise gaps summary. It is still captured by the mandatory refreshed call-site inventory, the lane’s commit/tag deliverable, launch guard, and final product-wide audit; it is not an uncovered escape.

**No-Git verification boundary:** The plan distinguishes forbidden product delegation from permitted user-configured hooks, filters, helpers, and signers. It requires auditing direct and indirect launch paths and testing in an environment that blocks executable launch independently of `PATH`, including absolute paths. Completion cannot be claimed while an in-scope product route remains.

**Path history:** Replacement is gated on exact sequence parity, range and ordering semantics, first-parent behavior, pagination, pathspec handling, correct history simplification, bounded incremental work, cancellation, deterministic fixtures, seeded randomized testing with replay, and absence of writes or lazy fetches.

**Mixed-version and degraded states:** Each route is replaced independently while standard Git repository state remains the interoperability boundary. The safe per-remote transport callback, packaging, and production activation retain their separate gates; this plan does not silently activate the transport work.

## 2. Risks and next action

The principal risks remain in decisions intentionally assigned to later gates: hook and signing fidelity, direct pack/object and ref-transaction behavior, history-simplification equivalence, and reproducible fork distribution. These are declared work, with decision records and lane-specific acceptance reviews required before replacement.

Accept this plan-stage gate and keep implementation paused until the operator resumes P0/P1. Each later lane design should identify the durable operation owner, complete preflight boundary, interruption state, and recovery or disclosure behavior before its first mutation.
