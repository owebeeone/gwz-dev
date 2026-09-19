# Shared-schema integration — merged remediation 1

Status: **correction implemented and verified locally; findings remain open until their original reviewers verify closure**.
The first [Code](GwzRemoteTransportIntegration-ReviewCode.md) and
[State](GwzRemoteTransportIntegration-ReviewState.md) reviews returned NO-GO on
workspace `5559184c118d937cff01c57fb202cb4affb92b91`, taut
`3b8436508ecc6b74b5efa0f1e642d0698b708bf7`, core
`3b4b632dfba5b0f2f154f50b45d9056a21a9ce99` and unchanged transport
`e8b9a1c5408cc9ea9528939b3a602acbeb697814`.

Four P2 findings are four distinct roots, with no blind convergence. No P0, P1
or P3 was reported. This is one merged correction across the package, not four
separate review objects. Transport implementation stays unchanged.

| Finding | Disposition and correction | Closure evidence |
|---|---|---|
| Code P2-1: external types plus vendored runtime | Accept. Reject a nonempty Rust external-type mapping combined with runtime vendoring before writing any target output. Give a clear diagnostic that the consumer must share the owner's runtime; document the option interaction. | Generator tests cover Rust-only and mixed-language invocations, assert refusal and no partial files; valid shared-runtime compilation and default generation still pass. |
| State P2-1: verified path differs from imported source | Accept. Resolve the Git top level, require its canonical `src`, verify pinned checkout as before, and refuse cached or imported taut modules from outside that exact source tree before using the generator. Verify `taut`, scaffold and external-type module locations explicitly. | The reviewer's `taut/docs` counterexample refuses. A same-version foreign preloaded module refuses before generation; canonical source still reproduces the artifact. |
| State P2-2: clock initialization and snapshots | Accept. Draft requires initial `advance` before checkout, one monotonic origin, and a periodic tick or mutation-driven re-query with bounded fallback. `next_deadline` is a snapshot; pending `next_action` supplies no timer service. | A test-only consumer host fixture starts at a large nonzero clock and proves the elapsed connect budget; another services a newly earlier deadline while the action receiver is pending. |
| State P2-3: final Pool owner shutdown | Accept. Draft requires a retained Pool clone for endpoint lifetime; final Pool drop initiates shutdown despite live driver/checkout/lease. Keep driver-disposal duties distinct. | Add a consumer host-lifetime test showing a retained clone preserves a live lease, and final owner drop invalidates it and begins cleanup. Cite the existing accepted transport regression in the draft. |

Run failing regressions before each correction, then the combined taut/consumer
tooling suite, explicit regeneration, formatting and isolated archive proof.
Commit the taut change first to establish its revision, refresh the consumer
generator pin/hashes, and settle the complete core/root tuple before either
re-review. These ordered dependency commits form one correction round.
The original reviewers then independently recheck their counterexamples and
changed-range interactions. No adapter work or interface freeze is authorized
by this correction. One of two permitted remediation rounds is in progress.

## Corrected object and local evidence

- Taut `733e8a78897a90f017f4726e4331aed95e8cb977`.
- Core `435e936b593476f24fad4cc4e70f5d06b784ed7d`.
- Transport remains `e8b9a1c5408cc9ea9528939b3a602acbeb697814`.
- Workspace input revision is pinned by the focused re-review prompts.

The combined tooling command from the initial prompts passes 40 tests (23 taut,
17 consumer). Explicit regeneration and consumer formatting pass. The isolated
offline archive proof now passes nine Rust tests (four identity/admission, two
typed/encoded close paths, three host-contract regressions). The source archive
and digest are unchanged. The nonzero-clock fixture checks pending state just
before the elapsed connect deadline and timeout at the exact deadline. The
earlier-deadline fixture keeps an exclusive lease, introduces a short waiter
while the action future stays pending, and services it by an independent tick.
Canonical preloaded modules are refused as well as foreign modules; the positive
regeneration test uses a fresh interpreter. No reviewer finding is self-closed.
