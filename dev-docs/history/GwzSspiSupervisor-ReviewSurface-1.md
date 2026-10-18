# GwzSspiSupervisor — Surface-AXIS REVIEW

**Review object:** Root `526451cea593e7d9550a28b3c86cf09a0dee2e20..255d05e09c0433e27dd7afeaa9f9fe04699cad05`; gwz-sspi `44879481fbd54dab84b99fecdadc89a34a84dcbd..a75485cbdd03607909d11637c07f97548dd7902c`. Caller-facing parent-supervision API and documentation, remediation round 1. Parent supervision awaits acceptance; native authentication, worker_entry, installed composition and publication remain deferred.

**Baseline:** gwz-dev `255d05e09c0433e27dd7afeaa9f9fe04699cad05`; gwz-sspi `a75485cbdd03607909d11637c07f97548dd7902c`; reference-only gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Sources read with `git show HEAD:<path>` and numbered with `nl -ba`. All three HEADs matched at start and end.

**Date:** 2026-10-03

**Axis:** Surface — cold Rust caller construction, matching, lifecycle, ownership, defaults and cleanup discoverability. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2 or P3 findings. Original Surface P3-1 is independently verified closed.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Surface P3-1 | Identify trusted host packaging metadata as the expected fingerprint source, require exact matching Hello bytes, and provide a synthetic construction recipe. | Supervision.md:89–96 names manifest/build-output field `build_fingerprint` and its relationship to Hello. Lines 103–117 show WorkerExecutable/Supervisor construction. Lines 133–137 clearly mark the fixture as synthetic. CallerGuide:20–26 repeats the contract and rejects guessed executable hashes or Cargo versions. | Closed |

The original counterexample was retraced: a fresh caller reaches the required 32-byte WorkerExecutable argument, searches the caller docs, and can now identify its producer, name, matching rule and construction sequence. The caller no longer has to invent a digest algorithm. The production producer’s absence is expressly deferred rather than concealed.

## Changed-range analysis

Compared original root `d2821a2db90aa641b1af6f80cadaf1aba9b35a0c` and member `fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a` against the corrected tuple, restricted to the permitted surface files.

Only these caller documents changed:

- **CallerGuide:** Added the fingerprint provisioning contract at lines 20–26 and synchronous captured-handle destruction/polling qualifications at lines 92–95.
- **Supervision.md:** Added the WorkerExecutable constructor’s return type; refined pre-registration disposal and polling guarantees; explicitly documented owned `Send + 'static` start/shutdown futures; added trusted-metadata construction and future-handoff examples; clarified completed-error input disposal and terminal-cause preservation.

README.md, CallerValues.md and Cargo.toml were unchanged from the original reviewed tuple.

Changes beyond the original Surface disposition clarify timing, future ownership and failure behavior. They introduce no new caller verb, required option, default, lifecycle operation or trust-metadata source. The owned-future guarantee is now explicit and illustrated after Supervisor Drop; step retains its mutable borrow. The disposal exception is disclosed without weakening the documented prohibition on polling metadata/provider queries, worker/thread creation, IPC or waits/joins.

**NEW ARCHITECTURAL root causes:** None established on the Surface axis. The caller docs are consistent with retained OS ownership and effects, but a docs-only review cannot independently verify private extraction or completion-port implementation. No implementation-conformance claim is made here.

## 0. Evidence base

Read the round-1 Surface prompt, the original Surface report, corrected caller documents and the Surface disposition. Workspace instructions and process authority were retained from the original review.

| Source | Corrected lines read |
|---|---:|
| gwz-sspi/README.md | 1–54 |
| gwz-sspi/Cargo.toml | 1–41 |
| gwz-sspi/docs/CallerValues.md | 1–113 |
| gwz-sspi/docs/Supervision.md | 1–144 |
| dev-docs/GwzSspiCallerGuide-DRAFT.md | 1–134 |
| dev-docs/GwzSspiSupervisor-ReviewSurface.md | Original finding and analysis |
| dev-docs/GwzSspiSupervisor-RemPlan.md | Surface disposition at line 18 |

A context search of the disposition incidentally displayed neighboring prior-round disposition rows. They were not used as Surface evidence. No current peer report or prompt was read.

Executed read-only commands:

- Three repository HEAD checks at start and end.
- `git show HEAD:<path> | nl -ba` for corrected caller files.
- Original-to-corrected `git diff` restricted to the permitted caller files.
- `rg` and `sed` for the Surface disposition and original report.
- Final `git status --short` for root, gwz-sspi and reference core.

gwz-sspi was clean. Root untracked prompts/route mapping and the reference core’s untracked bug report remained outside scope. No implementation code, design, checkpoint, tests or current peer reports were read. No writes, builds, workers, compiler probes or runtime tests were executed. Example compilation claims were inspected as documentation, not independently executed.

## 2. Invariant analysis

The complete caller walkthrough was repeated on corrected documentation:

| Stage | Result |
|---|---|
| Construct request | Owned-value recipe remains complete; real TLS binding and HTTP token allowance remain explicit host inputs. |
| Install/remove | Application/library/worker installation, removal and replacement remain paired. Production installation remains deferred. |
| Construct/match worker | Named trusted metadata, exact Hello-byte relationship, constructor return type and synthetic construction recipe resolve the original gap. |
| Start | Required deadline/cancellation inputs, synchronous originating-thread capture, FIFO admission and worker matching are discoverable. Owned future transfer after Supervisor Drop is explicit. |
| Step | Initial None, subsequent nonempty challenges, mutable serialization, eight-round limit and Complete prohibition remain explicit. |
| Finish/cancel | Both consuming lifecycle operations remain documented together, with publication revocation and cleanup distinctions. |
| Cleanup | IDs, receipts, Pending/Confirmed/Unknown and bounded tombstones remain discoverable. |
| Shutdown | Separate explicit deadline, immediate admission closure, outstanding IDs, repeated observations and continued supervision remain documented. |

The attacks on defaults, lifecycle completeness, moved-future identity, unsupported platforms and refusal status found no new defect. `max_workers` remains eight by default, range 1–64. TokenLimit and both deadlines have explicit no-default contracts. Complete remains distinct from HTTP acceptance. The refusing worker is never presented as successful authentication.

Minor inferences from the original review remain: Cancellation’s initial uncancelled state and some receipt/report field details are conveyed through abbreviated prose rather than complete declarations. No concrete incompatible behavior or failed lifecycle sequence was established from those omissions.

## 3. Risks and next action

This verdict closes the original documentation counterexample; it does not qualify native authentication, production packaging or Windows runtime behavior. The corrected docs accurately expose those deferrals.

The next action is to record Surface P3-1 closed at this tuple and proceed according to the independent aggregate gate.

Final HEAD checks returned root `255d05e09c0433e27dd7afeaa9f9fe04699cad05`, gwz-sspi `a75485cbdd03607909d11637c07f97548dd7902c` and reference gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`, unchanged from the start.
