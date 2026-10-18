# HTTPS SSPI composition — parked restart checkpoint

2026-10-04. **Paused at the operator's explicit request for a Codex restart.**
The single implementation drafter `/root/sspi_secret_codec_drafter` was
interrupted. Do not resume work until the operator resumes. No active Cargo,
rustc, rustfmt, cc or clang process was observed after interruption. No native
campaign was running; no test/exec session handle is known to remain active.

## Accepted authority and committed baseline

The documents-only design gate is complete:
[acceptance](GwzSspiHttpsCompositionAcceptance.md),
[Consistency re-verdict](GwzSspiHttpsComposition-ReviewConsistency-1.md) and
[first completed Safety](GwzSspiHttpsComposition-ReviewSafety.md) are GO.
Original caller Surface GO is retained for unchanged interface shape.
Consistency P2-1 is independently closed for the contract; implementation must
still execute its regressions. Earlier quota failure was an incomplete attempt.

Reviewed root: `721e07d65aa78a8bd79d41dae86ad99629a7aefc`.
Acceptance landing: root `52fd929`; core `56f56a3d` contains authority docs only.
Root before this parking commit: `9c2841d5d0c7fbfdeae841d00ae7ac30e0a0c3a2`
(the budget disposition).

| Member | Committed HEAD at park |
|---|---|
| gwz-core | `56f56a3d5ea3c9f8a50ec9e4c42453c1b92d9c79` |
| gwz-sspi | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |

**Partial source remains uncommitted and unreviewed. Do not reset, discard or
stage it as a complete implementation. CLI/Python source is unchanged.**

## Completed partial work and test receipts

Drafter-reported executed focused gates (not an independent review or full suite):

- SSPI capture: missing CallerCapture/capture_caller/start_captured produced
  eight compiler errors (exit 101). After implementation, the same pinned 1.95
  `cargo test --lib --locked --offline captured` gate passed three tests.
  Tests use actual Start polls/charged launch queue, context mismatch,
  simultaneous Starts, capture Drop and closed admission. Six existing source
  files changed. Private Origin is now Send + Sync and tickets retain Arc
  ownership; ordinary start preserves its previous closed classification.
- Transport: after taut regeneration, two new tests genuinely failed at runtime
  with UnsupportedOperation binding and InvalidMessage valid native facts.
  The same focused gate then passed both. A preliminary test-name typo was
  corrected before the meaningful RED; it is not claimed as that runtime RED.
  Taut adds WindowsConfigured/WindowsDefault, Sspi and optional NativeFacts;
  profiles remain 1/2/3. Validation/mux admission and reuse are partially wired.
- WindowsConfigured can select existing Basic/Gh facts on a reused connection;
  routing admission was corrected to permit that while retaining native gating.
- Regeneration used the generator's unchanged pinned rustfmt `ac68faa20c`.
  An initial attempt with 1.95 rustfmt `59807616` was refused. Rust test gates
  remain 1.95; do not silently change either pin.

External build/candidate root:
`/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition`.
SSPI target is its `sspi` child. A `core-prepared` candidate exists and names
the local SSPI dependency. Core dependencies are injected by
`gwz-core/tests/transport_backend/prepare.py`; ordinary core Cargo is unchanged.
Prepared src/build/tests are symlinks to the product source. Inspect its manifest
before reuse; no complete core RED/GREEN result has been reported.

Core partial work is deliberately incomplete: candidate dependency injection,
`mod native` wiring and an **untracked 58-line**
`src/git/endpoint/https_worker/native.rs` containing initial tests only.
Offers/History/Header/raw_limit/decode_challenge are not implemented yet.
The newly referenced core module is therefore not a buildable completion.
No core integration, host handoff, final-origin CBT or cleanup acceptance is claimed.

## Resume implementation contract

Read the full accepted
[composition design](GwzSspiHttpsCompositionDesign-DRAFT.md),
[caller contract](GwzSspiHttpsCallerGuide-DRAFT.md), SSPI §6 and core
GWZDesign/GWZRequirements before resuming. Existing authority is sufficient;
do not reopen the already-settled zero choice:

- One positive fixed logical Open deadline captured before first physical
  checkout/adoption. Preserve through discovery, helper work, carried/reused
  leases, native admission/Hello/rounds and valid remote acceptance. Never reset
  at 401 or borrow allocation/network/helper allowances. Effective zero refuses
  native before Begin/credential publication; expired positive is Timeout.
- Preserve cumulative active HTTP I/O accounting and helper M4/M10 provenance.
  Enclosing setup expiry during helper work cancels without default-logon fallback.
- Capture original caller on actual CLI entry before fanout and Python network
  entry before GIL detach/submission. Share the issuing Supervisor capacity,
  not a fresh per-remote eight-worker allowance. Native availability errors must
  not break anonymous, existing Basic or SSH paths.
- Keep every auth round on one exclusive physical lease with verified
  final-origin CBT, not proxy TLS. Preserve mechanism authority on Continue;
  track native Complete independently from remote acceptance.
- Advertisement-to-exchange: reuse only a still-live authenticated scoped
  physical generation. If lost, conservatively refuse **before POST**.
  No POST authentication replay or extra step on Complete.
- Initial nonempty server token in a Negotiate/NTLM offer cannot enter the current
  SSPI first-step(None) API. Draft plan is strict pre-Begin refusal, truthful
  selected source/scheme + NotStarted/credential_offered=false, no token discard
  or downgrade. Add actual production-path regression and disclose this limitation
  for reviewers. Do not claim all server/provider handshake forms qualified.
- Pending native record cleanup retains its operation dependency through existing
  endpoint cleanup polling; physical and native owners must both be accounted for.
  Keep revocation, route retirement, late-publication refusal and wipe guarantees
  truthful. Hyper/native-tls-owned copies remain disclosed.

Budget-only [owner disposition](GwzSspiHttpsCompositionBudgetDisposition.md):
32 handwritten source/test files, 3,500 handwritten added lines including tests,
eight member/API docs. Generated projections/locks separate. Stop before revised
budget excess or structural change. Current partial diff has 15 handwritten files
plus three generated outputs; no full exit gates or implementation review.

Next exact action after resume: continue the one drafter's core production
prepare/serve bridge TDD from the new native module; implement deadline/lease,
helper identity, CBT, native history and retained cleanup, then actual CLI/Python
context handoff and candidate schema regeneration. Test genuine production
orchestration with private fake ports and seeded cancel/late/cleanup histories.
Run focused pinned gates, syntax scope (actual script:
`gwz-core/scripts/checks/check_cfg_boundaries.py`), generation and host regression.
Settle exact implementation tuple via GWZ; then dual Code/State secret-adapter
review plus installed-caller Surface. Prefer retained reviewers
`/root/sspi_native_code`, `/root/sspi_native_state`, `/root/sspi_native_surface`
if available after restart; no implementation self-GO.

Windows activation guards stay intact. Full Windows release is NO-GO. Provider,
EPA/Digest/trust/proxy/Pageant, installed Windows, performance, selected sources
and distribution/aggregate release remain separate gates. Windows test fixtures
use E:/gwz-tests (operator override). No push, tag, publication or activation.

## Exclusions and workspace mechanics

Root SSH N2b prompts, route-mapping draft, core bug report and private alpha
setup-timeout untracked evidence are unrelated dirt; leave them untouched.
Read AGENTS_GWZ.md, root/member instructions, EVIDENCE.md, CurrentProgramCheckpoint
and review-loop skill. All staging/commits/structure via GWZ from workspace root;
no manual managed lock edits. Conditional sections have enclosing modules/cfg_if
and braced control-flow bodies, including disabled platforms. No mutable globals.
Public product tests/runners stay public, raw campaigns private, builds external.

## Working-file fingerprints at park

These fingerprints identify the incomplete working bytes, not a committed or
reviewed implementation. Reconcile newer work rather than trusting this record
over later evidence.

| Workspace-relative file | SHA-256 |
|---|---|
| `gwz-sspi/src/lib.rs` | `9a324f7644e6ff2f0b565f22fa4dc259e62f708c7fac7bc41c5c341118196424` |
| `gwz-sspi/src/supervisor/api.rs` | `68f21bf3b9e98331fcf970f6beaca7ad980f530ed9094b04250c57f1cc08fadb` |
| `gwz-sspi/src/supervisor/futures.rs` | `f8943dc3ea46fcc2c795d6ef58ec4e035f8186aa06f53ab01fcb634475aefbbe` |
| `gwz-sspi/src/supervisor/mod.rs` | `f625cece8e382146b90524aa128b6c7ed6a2753400614a87ca868c8f9e11c9a6` |
| `gwz-sspi/src/supervisor/ports.rs` | `624d867d23819e122534f4e23ceb4bc7c872d6daf1d8442d58ff8dd97f45f268` |
| `gwz-sspi/src/supervisor/remediation_tests.rs` | `8e92983bc1fe1743ad0fe9e8848ea73542231c8341c833e02fada1283a1a095e` |
| `gwz-transport/protocol/transport.ir.json` | `c5667a0d23197d1ac155c2cc5a95573ea0aee0b77b439f66512115f527353490` |
| `gwz-transport/protocol/transport.taut.py` | `fe5c11c0b3c68ca2a6dd6679705ebfbc517cab5cf22dfbd53e6fe17fb85433ee` |
| `gwz-transport/src/admission.rs` | `210539acde72febf74ac986b5cab5dbbfa8a2020c26fbdb33408971d2f835c2f` |
| `gwz-transport/src/binding.rs` | `de89e4d402ac288a5912a2045160d402857469f0379b98c475a925462935374a` |
| `gwz-transport/src/codec/validate.rs` | `049f470c91342d8bd744a3764333ffdbc5a102415a589ed9e823144e7f065091` |
| `gwz-transport/src/mux/routing.rs` | `47873c326f55f9c5e380c6ea9440ff5082f17a6429833a0a6dd8f6a375731b5f` |
| `gwz-transport/src/policy.rs` | `4159f9c7a1116e4c11d10d9414ae6a31c3cd70ab8da774e7b890a0d03c960c02` |
| `gwz-transport/src/protocol.rs` | `588d638be4826bf5c2038325371720636f8117b4340d1ec7967ee8740708d2b3` |
| `gwz-transport/tests/https_reuse.rs` | `4db3e80270452bfe98f8ad47e7f83dd8dfefc997503253ca74a532940216dde8` |
| `gwz-core/src/git/endpoint/https_worker.rs` | `d772bf63cbf4b130cad0b24f951d2090e4f4b0ced4603b3f51b0f4ac3518a19b` |
| `gwz-core/tests/transport_backend/prepare.py` | `4fe702916546f5e22965f063c7c89f20a168a80f43361ea8c44ea64cbd2eef5a` |
| `gwz-core/src/git/endpoint/https_worker/native.rs` | `03e822312cb04674dba95a2ca03a7d0461d7bc94f6d0e4e17f1ba6b926fbfac2` |

