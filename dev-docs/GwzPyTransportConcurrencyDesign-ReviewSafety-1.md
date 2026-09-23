# Python concurrent transport session — Safety-axis review

**Review object:** `dev-docs/GwzPyTransportConcurrencyDesign-1.md`, committed DRAFT at root `c4883f4331b983aa2920808e3798f7c94e2295db`. Candidate amendments were read at the pinned core and Python heads.  
**Baseline:** root `c4883f4331b983aa2920808e3798f7c94e2295db`; gwz-core `fcbb45f7fa1a797b1386bf0187ff6a60952bd193`; gwz-py `124e50030afb6f7c0e8edd90c37838b0136981c2`. All three heads matched at the start and end. Sources were read with `git show HEAD:`. Working-tree material was excluded.  
**Date:** 2026-09-24.  
**Axis:** Safety — degraded paths, stuck states, disclosure, irreversible effects and resource bounds. Independent, adversarial and read-only; nothing here relies on the other current-round review.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified below, provided the corrected contract introduces no new blocking defect.

---

## 0. Evidence base

I read the committed design, `dev-docs/GwzPyTransportConcurrency-RemPlan-1.md`, the prior-round Safety and Consistency reports, `gwz-py/dev-docs/GwzPyTransportDesign.md` and `GwzPyConcurrentOperations.md`, and the relevant portions of the core retry, remote transport, v1.1.0 and accepted sequenced-stream designs. I checked the design’s assumptions against the pinned `gwz-core/src/transport_host/{mod,request,session}.rs` and `gwz-py/src/gwz/bridge.py`. The process basis was `dev-docs/AgentProcessRules.md` as amended by `GwzProcessOptimization.md`, plus the review-loop skill and its prompt template.

Inspection used `git rev-parse HEAD`, `git show HEAD:<path>`, `rg`, `sed` and `nl`. No files were changed; no build or test was run. The decisive ranges are design lines 15–19, 33–35, 41 and 45–47; core `session.rs` lines 496–520, `mod.rs` lines 180–235 and `request.rs` lines 375–395; and the Python caller guide lines 31–35.

## 1. Findings

### [P2-1] Reusing a completed caller request ID conflicts with the core generation’s lifetime ID rule

**Location and invariant:** Design line 15 expressly permits a completed `RequestMeta.request_id` to be reused for a later operation with a fresh public serial. Line 41 retains the core generation’s 256 lifetime request-ID rule. The design does not specify a distinct, unique core registration ID or a mapping that preserves the caller-visible correlation key. In the pinned core, `TransportRuntime::request` passes `meta.request_id` into endpoint registration and later `begin` (`mod.rs:204–232`); `Session::register` rejects any ID already in its lifetime `used` set (`session.rs:496–520`).

**Counterexample and impact:** A client completes a fetch with `request_id="batch-a"` and submits another fetch with the same value while the generation has used fewer than 256 IDs. Native admission treats the ID as no longer live and the design says reuse is legal, but core registration refuses it. The fresh public operation serial does not change the ID core registers. This is a concrete parity and recovery defect: a documented valid sequential call cannot run, and retrying it with the same correlation key cannot recover.

**Required correction:** Either give every core registration a generation-unique internal request ID while retaining and correctly attributing the caller’s `RequestMeta.request_id`, or remove the completed-ID reuse promise from the design and caller contract. Specify which ID reaches core registration, cancellation, events and rollover accounting.

**Closure test:** Complete two sequential network operations on one Client with the same explicit caller request ID in a single generation. Verify the chosen contract: both succeed with distinct owned records and correctly correlated results, or the second receives the documented typed refusal before effects. Also verify that a stale frame from the first cannot attach to the second.

### [P2-2] Aggregate record pressure can turn an otherwise valid mutating result into an uncertain failure after acceptance

**Location and invariant:** Design lines 35 and 45 promise a pre-`Accepted` `TransportSessionFull` refusal for result-budget exhaustion and an inspectable terminal result until expiry. Yet line 45 reserves only a small terminal-failure slot on admission, while up to 64 MiB may already be charged to retained records and readers. It allows a later result that exceeds the remaining aggregate budget to become `TransportRecordLimit`, even when that operation’s own result is below its 8 MiB limit.

**Counterexample and impact:** Retained completed records and a slow reader hold nearly all 64 MiB. A push is admitted because one small failure slot fits. Its Git effect completes, but its otherwise valid result exceeds the remaining aggregate space. The session discards the success outcome and retains `TransportRecordLimit(effect="possible")`. The caller must now investigate an uncertain push solely because unrelated completed records occupied the budget. The documented pre-admission exhaustion refusal did not protect this operation.

**Required correction:** Account for enough lossless per-operation record capacity before `Accepted`, including mandatory events and a result up to the stated per-operation limit; refuse `TransportSessionFull` while that capacity is unavailable. Keep `TransportRecordLimit(effect="possible")` for an individual operation that exceeds its own declared limit, and retain the reserved charge through terminal publication.

**Closure test:** Hold aggregate charged retention just below 64 MiB, then attempt a push whose result is within 8 MiB but would exceed the unreserved remainder. Assert a typed refusal before Git or credential effects. After release or expiry frees capacity, repeat it and verify its terminal result remains inspectable. Separately, exceed the operation’s own 8 MiB limit and verify the explicit possible-effect failure.

## 2. Invariant analysis

The revised text establishes session ownership checks for event, result, merge, cancel and release lookups (lines 15–19), directly addressing the prior cross-client collision. Its separate public operation ID prevents two clients with the same caller request ID from sharing a result record. I found no remaining cross-session read path in the stated contract.

The draft also gives initial and later capacity transitions one serialized owner, keeps equal-capacity admission from resizing a pool under a live lease, and makes differing-capacity refusal precede endpoint effects (lines 23–29). It bounds top-level and member workers independently of caller-controlled `jobs` (line 33), refuses unsupported Python CLI placement before credential work (line 51), and preserves close completion across Python waiter cancellation (line 47). These attacks did not yield a separate finding.

The generation rule preserves fresh Port identity and captured endpoint configuration across rollover (line 41). The accepted sequenced-stream design requires registration to remain while tickets or physical cleanup remain and forbids stale-generation delivery (`GwzTransportSequencedStreamDesign.md:29–31`). The draft’s fail-closed `TransportGenerationBusy` outcome is explicit. Profile-3 host proof, runtime implementation and supported-platform publication remain deferred as directed; this review does not treat their absence as defects in this DRAFT.

## 3. Risks and next action

The small terminal-failure slot is useful for reporting an overflow, but it cannot substitute for admission capacity when the contract promises a lossless result up to 8 MiB. The request-ID issue likewise needs an explicit distinction between caller correlation and core registration before implementation can safely choose an ID scheme.

Keep the design **NO-GO**. Correct these two contract gaps in one revised design object, then re-verdict their counterexamples against the new pinned tuple. This verdict accepts neither implementation nor activation.
