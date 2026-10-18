# SSPI caller values and secret codec acceptance

2026-10-03. **Accepted bounded implementation** after independent Code, State
and Surface GO on the exact tuple below. This closes GwzSspiPlan.md step 1's
secret-codec/caller-value gate only. No authentication, process supervision,
native secret disposal, release or Windows activation is accepted.

| Repository | Reviewed commit |
|---|---|
| Root | e9f80c697acc5860ad90dbf5acd5888ccf2bd586 |
| gwz-sspi | e3851768da8d58140d92590bf61575f6edbe333c |
| gwz-core, unchanged reference | 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31 |

Controlling [checkpoint](GwzSspiSecretCodecDesign.md). Verbatim reports:
[Code](GwzSspiSecretCodecDesign-ReviewCode.md),
[State](GwzSspiSecretCodecDesign-ReviewState.md),
[Surface](GwzSspiSecretCodecDesign-ReviewSurface.md).
Canonical prompts and the drafter's complete report are adjacent documents.
All reviewers verified the same tuple at both boundaries and remained peer-blind.

Code and State: zero P0–P3 findings. Surface: one nonblocking P3-1, missing public
imports and complete value construction/disposal example. No blocking remediation,
architectural causes, blind convergence or escaped defects. Remediation rounds 0.
The post-review member 44879481fbd54dab84b99fecdadc89a34a84dcbd adds that caller
walkthrough, compiled through crate Rustdoc, and status/testing documentation.
No runtime/API/protocol behavior changed. The owner compiled the example and all
twelve negative trait doctests successfully, reran strict Clippy/fmt and verified
the 61-file standalone Cargo archive. The same Surface reviewer independently
retraced its original counterexample and closed P3-1 at the revised member tuple:
[Surface closure](GwzSspiSecretCodecDesign-ReviewSurface-1.md). Zero findings remain.

## Accepted scope and evidence limits

Owned SecretBytes/SecretText, checked required TokenLimit, request/identity/Digest/
token/error values; generated private borrowed/owned records; strict canonical
CBOR walks; bounded profile/context admission before owned copies; validated
size-first encoding; exact partial framing and explicit caller adapters.
Sole dependency zeroize =1.9.0, defaults disabled, alloc only, explicitly reviewed
before addition and independently inspected by both code reviewers.

29 unit tests, caller integration, unchanged worker refusal and twelve negative
trait doctests passed, plus the later compiled example (44 Rust checks total).
Thirteen Python checks and sixteen-artifact regeneration passed. Fourteen pinned
reference fixtures each receive 128 seeded framing schedules. Syntax-aware scope
checking covered disabled branches. Drafter passed the original 43-check suite
from the extracted archive; final archive verification and walkthrough compilation
also pass. These are local tests. Remote CI is authored but unexecuted.

Borrowed constructor sources stay caller-owned. Wipe tests inspect initialized
live storage after zeroization and before release; they do not prove physical
secret erasure, provider/LSASS cleanup or process cancellation. Codec admission
uses supplied expectations; it does not establish native identity, conversation
phase, publication eligibility or confirmed worker disposal.

## Next boundary

GwzSspiPlan.md step 2: supervision kernel, deterministic race/seeded schedule
checks, retained launch/I/O ownership, capacity/cancellation/quarantine/shutdown
and Code/State review. Native UTF-16/provider storage and process/Job disposal,
worker bootstrap, CLI/Python installation, core composition, provider parity and
full Windows release qualification remain open. The worker still refuses;
publishing remains disabled. No push, tag, registry or endpoint activation.
