# Shared-schema transport integration checkpoint — 2026-09-19

Status: **accepted at the tuple below after original
[Code](GwzRemoteTransportIntegration-ReviewCode-1.md) and
[State](GwzRemoteTransportIntegration-ReviewState-1.md) reviewers reported GO;
this accepts the shared-schema implementation checkpoint and draft host contract only**.
The reviewed scope is the taut external Rust type generator, test-only core
consumer, archive/regeneration proofs and draft pool host contract. This is an
implementation checkpoint within Phase 1, not a Phase 1/2 interface freeze.

| Review object | Revision |
|---|---|
| taut generator and documentation | `733e8a78897a90f017f4726e4331aed95e8cb977` |
| gwz-core consumer and controlling pool/interface draft | `435e936b593476f24fad4cc4e70f5d06b784ed7d` |
| gwz-transport unchanged accepted implementation | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` |
| workspace review inputs and merged correction | `23617273932031a346c6fd772e1df99fd68e2706` |

The first review's workspace commit is `5559184c118d937cff01c57fb202cb4affb92b91`.
Taut baseline is `7a5f616c3a9f72e143b6e20dab41ffa6e20e240a`;
core baseline is `e21250ce0d6e9b5fce02154595db5ba2e451ae4c`.

Both original reviewers independently verified the corrected tuple and all
finding closures. Corrected local evidence is 40 tooling tests and nine
isolated Rust consumer tests, plus regeneration and formatting, all passing;
both reviewers independently reproduced those results.
The initial evidence counts below describe the first reviewed tuple.

## Scope and ownership

Taut generates Rust re-exports for explicitly mapped dependency types. The
consumer composes the exported transport schema and generates one test wrapper
whose field is the owner's native `Envelope`. It shares the owner's CBOR runtime.
Normal Cargo compilation needs no schema or generator. An explicit regeneration
command verifies pinned source/schema inputs; the unpublished package is tested
through a temporary local archive patch, not a registry publication claim.

The archive runner checks a caller-supplied digest, manifest package identity,
Cargo VCS revision metadata and clean-source metadata before isolated extraction.
Links, special files, duplicate names and path escapes are refused. These checks
establish consistency with caller-pinned inputs, not cryptographic authorship of
Cargo's metadata.

The [pool/runtime draft](../gwz-core/dev-docs/GwzRemoteTransportPool-InterfaceGate.md)
records actual callbacks, defaults, capacity accounting and cleanup responsibility.
Transport source is unchanged from the accepted stream/pool checkpoint. No
production GWZ fields, service methods, wire framing, connections or credentials
are added. The encoded consumer exchange is a payload fixture; the supplied
outer communication layer still owns pre-allocation bounds and delivery.

## Local verification

Run commands from the workspace root. Product tests are public; no raw campaign
artifact or private CI dependency was added. All commands below passed locally.
The consumer tooling suite passed 14 tests on Python 3.10; the isolated archive
proof passed six Rust tests, including the two typed/encoded close/cleanup paths.
The actual-archive metadata test may skip in a fresh checkout until that explicit
archive is supplied; the standalone archive proof is the required success gate.

- Taut: `PYTHONPATH=taut/src .venv/bin/python -m pytest taut/src/tests/test_rust_external_types.py taut/src/tests/test_scaffold.py taut/src/tests/test_cli.py taut/src/tests/test_rust.py -q` — 21 passed.
- Transport: `cargo +1.95.0 test --locked --manifest-path gwz-transport/Cargo.toml` — 66 passed, two extended campaigns intentionally ignored. Fixed seeded campaigns run in this suite; the prior 50,000-case pool evidence was not rerun because transport source is unchanged.
- Consumer tooling: `PYTHONPATH=taut/src .venv/bin/python -m pytest gwz-core/tests/transport_consumer/protocol/test_regen.py gwz-core/tests/transport_consumer/test_package_proof.py -q`.
- Consumer regeneration: `gwz-core/protocol/.regen-venv/bin/python gwz-core/tests/transport_consumer/protocol/regen.py --owner-schema gwz-transport/protocol/transport.ir.json --taut-source taut/src --check`.
- Consumer formatting: `cargo fmt --manifest-path gwz-core/tests/transport_consumer/Cargo.toml -- --check`.
- Archive proof: `gwz-core/protocol/.regen-venv/bin/python gwz-core/tests/transport_consumer/package_proof.py --archive gwz-transport/target/package/gwz-transport-0.1.0.crate --archive-sha256 24c9d7a839b1a23ae1f188541ac87092550dcae99bd9cf6e14df4c900b1a7dd9 --source-revision e8b9a1c5408cc9ea9528939b3a602acbeb697814`.

## Review routing and remaining work

Reuse the original Code and State reviewers under the operator's explicit
direction. They review independently against one committed tuple using prompts
generated from the review-loop template. The first [Code](GwzRemoteTransportIntegration-ReviewCode.md)
and [State](GwzRemoteTransportIntegration-ReviewState.md) reviews returned NO-GO
with four P2 findings (four distinct roots; no blind convergence, P0, P1 or P3).
The [merged remediation](GwzRemoteTransportIntegration-RemPlan.md) addresses
generator/runtime option compatibility, exact imported-source provenance,
clock-origin/timer duties and final Pool ownership. The original reviewers
verified their counterexamples on the corrected tuple and closed all four
findings, with no new findings or architectural roots. Two completed review
rounds, one of two permitted remediation rounds used; no open findings or
observed production escape. All reports are filed verbatim. Subsequent
documentation commits record acceptance without changing reviewed source.

Phase 1 still needs the complete message/admission integration evidence and CI
drift wiring. Phase 2 still needs active-I/O clock semantics and its remaining
full contract evidence. Complete those obligations and their named interface
gates, including Surface review at freeze, before dependent adapter integration.
No native Windows, physical network performance, publication or production
activation is claimed here.
