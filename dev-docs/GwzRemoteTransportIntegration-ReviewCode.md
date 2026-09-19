# Shared-schema transport integration — Code-AXIS REVIEW

**Review object:** Taut external Rust type generation at `3b8436508ecc6b74b5efa0f1e642d0698b708bf7`, the test-only core consumer and draft `gwz-core/dev-docs/GwzRemoteTransportPool-InterfaceGate.md` at `3b4b632dfba5b0f2f154f50b45d9056a21a9ce99`, and workspace integration checkpoint `5559184c118d937cff01c57fb202cb4affb92b91`; implementation checkpoint, no interface freeze, dated 2026-09-19.  
**Baseline:** workspace root `5559184c118d937cff01c57fb202cb4affb92b91`; taut `3b8436508ecc6b74b5efa0f1e642d0698b708bf7`; gwz-core `3b4b632dfba5b0f2f154f50b45d9056a21a9ce99`; unchanged gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`. Committed sources were read using `git show PIN:path` and the pinned taut/core ranges with `git diff`.  
**Date:** 2026-09-19  
**Axis:** Code — architecture, interfaces, call graphs, compatibility, error paths and scope reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — zero P0, zero P1, one P2 and zero P3 findings. P2-1 blocks acceptance of the new generator interface. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

The exact tuple resolved unchanged at both start and end:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `5559184c118d937cff01c57fb202cb4affb92b91` | `ca81a514178868abec681044dbe018b441d1bf84` |
| taut | `3b8436508ecc6b74b5efa0f1e642d0698b708bf7` | `4349138d3516971d9e4778b0546ac01a0dcfab29` |
| gwz-core | `3b4b632dfba5b0f2f154f50b45d9056a21a9ce99` | `d276027c5b6f9e034de79b8109ae876872e112d8` |
| gwz-transport | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` | `6aedc6f4e2de3e228b03d4a66e792f94183ec429` |

Taut, core and transport remained clean. Only the two authorized untracked integration prompts were present at the workspace root. No current-round peer prompt or report was read.

The review read:

- The complete generated Code prompt, workspace/member instructions, integration checkpoint and current program checkpoint.
- The complete taut range `7a5f616c3a9f72e143b6e20dab41ffa6e20e240a..3b8436508ecc6b74b5efa0f1e642d0698b708bf7`: CLI wiring, scaffold generation, external-path validation, documentation and tests.
- The complete core range `e21250ce0d6e9b5fce02154595db5ba2e451ae4c..3b4b632dfba5b0f2f154f50b45d9056a21a9ce99`: consumer schema, pins, regeneration, checked output, archive verifier, manifest/lockfile and all consumer tests.
- The complete draft pool/interface gate and its relevant requirements, design and Phase 1/2 plan clauses.
- The unchanged transport package manifest, schema digest, regeneration path, exported CBOR/protocol modules and pool API interactions necessary to verify the draft.
- Workspace lock entries proving that root pins the reviewed core, taut and transport commits.
- Searches confirming that the core diff adds only the test consumer and draft document, with no production core protocol, service or source call site.

Targeted commands run independently:

- The combined taut and consumer tooling selection passed **35 tests**.
- Explicit consumer regeneration with the pinned owner schema and taut source passed.
- Consumer `cargo fmt --check` passed.
- The isolated archive proof passed offline against archive SHA-256 `24c9d7a839b1a23ae1f188541ac87092550dcae99bd9cf6e14df4c900b1a7dd9` and transport source revision `e8b9a1c5408cc9ea9528939b3a602acbeb697814`. It built the extracted package and passed all six consumer tests.
- Taut and core correction ranges passed `git diff --check`.
- No large randomized campaign was rerun because transport source is unchanged.

A minimal temporary-directory counterexample exercised the option interaction in P2-1. It changed no reviewed file.

## 1. Findings

### [P2-1] External dependency types and vendored Rust runtime are accepted together even though the emitted types cannot interoperate

**Location:** `taut/src/taut/gen/scaffold.py:581–629` accepts `runtime=True` and a nonempty `rust_external_types` map independently. The generator then emits external `pub use` declarations into `api.rs` at lines 660–666 and emits a fresh Rust `cbor.rs` at lines 668–680. The CLI exposes the combination directly through `--rust-external-types` and `--with-runtime` in `src/taut/cli.py:63–68, 152–154`. The README correctly states at lines 171–176 that a dependency consumer must share the owner’s CBOR runtime and must not install a second generated runtime, but the command interface does not enforce or encode that ownership rule.

**Violated invariant:** A successful code-generation command must produce a coherent runtime/type ownership arrangement for the selected options, or reject an incompatible option combination before writing output. An external generated type’s codec methods use the owner crate’s nominal `Cbor` and `DecodeError` types; a separately generated consumer runtime creates different Rust types even when its source is byte-identical.

**Reproduction:**

1. Define owner message `Packet` and a composed consumer message `Request { packet: Packet }`.
2. Generate the owner with Rust runtime vendoring.
3. Generate the consumer with both `runtime=True` and `rust_external_types={"Packet": "owner::api::Packet"}`. This is the library equivalent of combining the two documented CLI options.
4. The emitter succeeds and writes both consumer `api.rs` and consumer `cbor.rs`.
5. Assemble the emitted modules in the ordinary vendored-runtime form and compile against the owner crate.

Generation exits successfully, but `rustc` fails:

- `Request::to_cbor` expects the consumer’s `cbor::Cbor` while `Packet::to_cbor` returns `owner::cbor::Cbor` (`E0308`).
- `Request::from_cbor` passes the consumer `Cbor` to a method expecting the owner `Cbor` (`E0308`).
- The owner `DecodeError` cannot be converted to the consumer `DecodeError` by `?` (`E0277`).

This interaction is especially credible for a composed multi-language invocation: `--with-runtime` may be requested for the other generated targets while the Rust target uses dependency-owned types.

**Impact:** The new public option can report success while emitting a Rust artifact set that fails at the first containing-message codec boundary. The documentation tells callers how to avoid the failure, but the generator’s option contract permits the contradictory ownership modes and provides no diagnostic. This weakens the claimed generator-level ownership guarantee and moves an invariant that is statically knowable into a later compiler failure.

**Required correction:** Make Rust runtime ownership explicit in `emit` and the CLI. For a nonempty external Rust type map, either:

- refuse Rust runtime vendoring with a clear pre-write error; or
- define `--with-runtime` to skip the Rust CBOR runtime while still vendoring runtimes for other selected languages, and clearly report/document that the Rust crate must re-export the owner runtime.

Do not emit a second Rust `cbor.rs` as an apparently usable output beside external dependency types.

**Closure/regression test:** Extend `test_rust_external_types.py` with the exact option combination. Assert either a clear generation-time refusal with no partial Rust output, or that no consumer Rust runtime is emitted and the generated consumer compiles after `pub use owner::cbor`. Include a mixed Rust/other-language invocation if runtime vendoring remains supported for the other targets.

## 2. Invariant analysis

The following attacks held:

- **Native type identity:** The checked consumer output contains re-exports of every owner enum/message and only one new wrapper struct. `GwzTransportDelivery.message` is the owner’s native `gwz_transport::protocol::Envelope`; assignment back to that exact type compiles without conversion.
- **Shared runtime on the reviewed path:** `gwz_transport_consumer_proof` re-exports `gwz_transport::cbor`, and generated wrapper codecs import that re-export. The isolated package proof confirms matching `Cbor` and `DecodeError` identities for the actual consumer.
- **Default compatibility:** External mapping is optional. The unchanged generation path retains ordinary declarations and codecs, and the existing taut Rust/scaffold/CLI regression suite passes.
- **Mapping validation:** Unknown schema names, malformed/injected paths, reserved path components and non-Rust-only use are refused. Paths are emitted deterministically.
- **Regeneration provenance:** The explicit consumer command verifies the owner package name/version, owner schema SHA-256, taut commit, clean taut `src` state, changed-generator file hashes, taut version, codec mode and exact rustfmt version before comparing checked output.
- **Package independence:** The consumer manifest uses exact version `=0.1.0` with no build script or sibling path. The archive proof validates caller-supplied SHA-256, normalized manifest identity, VCS revision and clean metadata, rejects links/special files/duplicate or escaping paths, extracts manually, patches only the isolated temporary consumer and passes `cargo test --offline --locked`.
- **Typed and encoded handoff:** Both directions preserve binary data through the shared `Envelope` type. The encoded fixture uses the owner codec and limits after generated wrapper decoding; graceful close completes only after endpoint cleanup. The documents accurately defer real outer-message pre-allocation bounds to the supplied communication layer.
- **Scope and retained interfaces:** No production core source, GWZ service method, optional production field, physical framing or transport source changed. The wrapper/tag is explicitly test-only. Registry publication and lockfile refresh are accurately deferred.
- **Draft pool contract:** Constructor defaults, request shortening rules, full reuse key, user/host and host capacity domains, action/callback pairs, actual-disposal acknowledgement, structured owner cancellation, independent clock driving and close-before-reuse duties match the unchanged accepted pool API. Active-I/O timing and production dispatcher wiring are explicitly left open rather than claimed.
- **Checkpoint accuracy:** The reported targeted tests, regeneration and archive evidence reproduced. The documents distinguish this checkpoint from Phase 1/2 freezes and do not authorize adapter integration.

The failed attack is confined to the generic code-generator option boundary. The committed core consumer selects the valid ownership mode and passes, but the reviewed taut feature itself exposes the contradictory mode as a successful generation request.

## 3. Risks and next action

Outer GWZ carrier allocation bounds, complete encoded-message/admission coverage, CI drift wiring, active-I/O timing, production dispatch, physical adapters, native platforms, publication and release remain explicit future gates. Nothing in this review accepts or freezes those surfaces.

Apply the bounded P2-1 correction and add the option-interaction regression. Then rerun the 35 targeted tooling tests, explicit regeneration, formatting and isolated archive proof, and return the corrected immutable taut/core/root tuple for a focused Code re-verdict.
