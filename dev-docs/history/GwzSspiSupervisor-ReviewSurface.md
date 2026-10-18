# GwzSspiSupervisor — Surface-AXIS REVIEW

**Review object:** Root `526451cea593e7d9550a28b3c86cf09a0dee2e20..d2821a2db90aa641b1af6f80cadaf1aba9b35a0c`; gwz-sspi `44879481fbd54dab84b99fecdadc89a34a84dcbd..fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a`. Caller-facing parent-supervision API and documentation. Parent implementation is awaiting acceptance; native authentication, worker_entry and publication remain deferred.

**Baseline:** gwz-dev `d2821a2db90aa641b1af6f80cadaf1aba9b35a0c`; gwz-sspi `fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a`; reference-only gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Caller documents were read with `git show HEAD:<path>` and numbered with `nl -ba`. All three HEADs matched at start and end.

**Date:** 2026-10-03

**Axis:** Surface — cold Rust caller construction, matching, lifecycle, ownership, defaults and cleanup discoverability. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1 or P2 findings; one P3 documentation finding.

---

## 0. Evidence base

Read the applicable workspace/member instructions, the assigned Surface prompt, and process authority concerning independent reviews, surface shape, severity and actionable findings. No implementation code, tests, design, plan, checkpoint or peer reports were read.

Caller evidence:

| Source | Lines read | Surface examined |
|---|---:|---|
| gwz-sspi/README.md | 1–54 | Package purpose, refusal/publication status, caller-doc discovery |
| gwz-sspi/Cargo.toml | 1–41 | Package identity, version, features, worker executable, publication status |
| gwz-sspi/docs/CallerValues.md | 1–113 | Owned values, validation boundaries, source wiping, request construction |
| gwz-sspi/docs/Supervision.md | 1–80 | Constructors, lifecycle, metadata timing, future movement, cancellation and cleanup |
| dev-docs/GwzSspiCallerGuide-DRAFT.md | 1–124 | Full lifecycle signatures, integration responsibilities, token/deadline rules, host walkthrough |

Read the exact-range diffs for these documents and Cargo.toml. Targeted searches across the caller guide and Supervision.md checked fingerprint instructions, constructors and defaults.

Executed inspection commands included:

- `git rev-parse HEAD` and equivalent commands in gwz-sspi and gwz-core, at start and end.
- Read-only `git status --short` for the root and reviewed/reference members.
- `git show HEAD:<path> | nl -ba`.
- Exact-range `git diff` restricted to the permitted caller documents and Cargo.toml.
- `rg`, `cat` and `sed` for instructions and targeted documentation inspection.

gwz-sspi was clean. Root untracked prompts/route mapping and the reference core’s untracked bug report were outside scope and were not read. No writes, builds, compiler probes, workers or runtime tests were executed. This object exposes a Rust API; executable help was not run under the prompt’s command restrictions.

## 1. Findings

### [P3-1] Required worker fingerprint lacks a caller-facing source or derivation contract

**Location:** gwz-sspi/docs/Supervision.md:10; dev-docs/GwzSspiCallerGuide-DRAFT.md:15–20, 33 and 95.

**Violated invariant:** A caller must be able to supply required construction inputs without guessing their meaning. Deferred installed composition does not defer documentation of the fingerprint parameter’s interface contract.

**Reproduction:** Follow the caller docs from a fresh host integration:

1. Select an absolute installed worker path.
2. Reach `WorkerExecutable::new(PathBuf, [u8;32])`.
3. Read that the second argument is the “trusted installed artifact-set fingerprint.”
4. Search every permitted caller document for its source or derivation.

The docs identify neither an exported value nor a trusted manifest/build output supplying those bytes. They also do not define a derivation procedure. The caller must guess whether to hash the executable, a set of artifacts, build metadata, or something else. The only supervisor recipe starts with an “already trusted Supervisor,” bypassing this unresolved setup step.

**Impact:** The documented first-day construction and matching walkthrough stops before Supervisor construction. Guessing a conventional file hash can produce an incompatible expected fingerprint and an unexplained matching failure. This finding concerns the required input’s documentation, not the deferred availability of a working native worker.

**Required correction:** Document the precise supported source of the 32 bytes and how a host obtains them. If supplied by trusted packaging metadata, name that contract and distinguish it from hashing the worker executable. Include a construction example; a clearly marked synthetic input is sufficient while installation remains deferred.

**Closure/regression test:** A fresh caller, restricted to the published caller docs, must identify the fingerprint producer and write the WorkerExecutable/Supervisor construction sequence without inventing a hash rule or reading private protocol/design material. A documentation check should preserve the named source and construction recipe.

## 2. Invariant analysis

The following attacks did not establish blocking defects:

- **Lifecycle pairs:** Supervisor construction has shutdown and documented Drop behavior. Conversation creation has consuming finish and cancel operations. The guide documents application/worker removal and replacement together.
- **Defaults and bounds:** `max_workers` defaults to eight and accepts 1–64. TokenLimit is mandatory, immutable, accepts 1–65,536 and has no default. Operation and shutdown deadlines are explicitly supplied; neither has a default timeout or extension.
- **Ownership:** Request and challenge values are owned. Step holds a mutable Conversation borrow. Finish and cancel consume the conversation. Caller sources retain a separate wiping obligation. Cancellation can be cloned as a shared signal.
- **Timing and moved futures:** Construction/start metadata probes occur synchronously without a numeric bound. Later native work is offloaded. The originating thread remains the identity authority when a future moves between executor threads.
- **Publication and cleanup:** Complete is distinguished from HTTP acceptance. Cancellation/expiry revoke publication. Pending cleanup is distinguished from Confirmed; exit, Job emptiness and completed resource ownership disposal are required. Evicted or foreign record IDs are Unknown.
- **Unsupported/refusing operation:** Non-Windows Supervisor construction returns UnsupportedPlatform after options validation. The worker deliberately refuses calls. The recipe explicitly promises a Failure with the current worker rather than authentication success.

Cold caller walkthrough:

| Stage | Result and guesses |
|---|---|
| Construct request | The owned-value example supplies a complete synthetic request. Production binding and HTTP token allowance are explicitly host responsibilities. |
| Install/remove | Matching application/library/worker installation and paired removal are described. A runnable released installation is explicitly deferred. |
| Construct/match worker | Absolute-path construction is discoverable; fingerprint provisioning requires the guess recorded in P3-1. The abbreviated constructor entry also requires inferring its return signature. |
| Start | Request, deadline and cancellation inputs are discoverable. FIFO admission, synchronous capture and Hello matching are described. |
| Step | Initial None, subsequent nonempty challenges, eight-round bound and Complete prohibition are explicit. |
| Finish/cancel | Both operations are discoverable, consuming and documented together. Pending step borrowing requires ending/dropping that borrow before consuming the conversation. |
| Cleanup | Failure IDs, cancellation receipts, status lookup and bounded tombstones are described. Receipt types and the confirmed-count field spelling require inference from the abbreviated table; no incompatible lifecycle consequence was established. |
| Shutdown | Admission closure, separate deadline, outstanding IDs, repeated calls and continued reaping are explicit. |

The docs call Cancellation a shared signal but do not explicitly state the initial `is_cancelled()` value. I inferred an initially uncancelled signal from `new`/Default; this did not establish an option-default or compatibility defect.

## 3. Risks and next action

This is a documentation-only Surface verdict. It establishes neither implementation conformance nor native Windows/runtime correctness. Authentication, worker_entry, installed composition and release qualification remain deferred as specified.

The next action is to document fingerprint provisioning and add the missing construction recipe before callers rely on this surface. No blocking Surface finding requires another implementation revision.

Final tuple recheck returned the same root, gwz-sspi and reference gwz-core SHAs listed above.
