# taut 0.10.0 adoption

Date: 2026-09-30. Status: **accepted at the round-2 bundle (manifest SHA-256 `58b7c925…`, 61 entries) after [GwzTaut010Adoption-ReviewConsistency-1.md](GwzTaut010Adoption-ReviewConsistency-1.md) and [GwzTaut010Adoption-ReviewSafety-1.md](GwzTaut010Adoption-ReviewSafety-1.md) reported GO; this accepts the taut 0.10.0 adoption only.** It went through the wire-format dual review of the review granularity ruling:
- **Round 1** reported GO on both axes with six P3s, all taken into the step ([RemPlan](GwzTaut010Adoption-RemPlan.md)).
- **Round 2** closed all six. Consistency raised two text P3s (P3-5 and P3-6), which this document takes in at landing: §5's first paragraph, and claim 5 with §6.

## 1. Why

taut 0.10.0 was released on 2026-09-30 as one train: taut-proto 0.10.0 on PyPI, taut-shape 0.10.0 on PyPI and on crates.io, and the tag `v0.10.0` in taut (`a7cab03`), taut-shape, taut-shape-py and taut-shape-rs.

CS1.1, the next step on the transport release's path, appends `TransportCapabilitiesResponse.cancellation` (8) to the production schema as `optional=MISSING_OK` ([GwzCoreSessionDesign.md](GwzCoreSessionDesign.md) §13, line 534; [GwzCoreSessionPlan.md](GwzCoreSessionPlan.md) CS1.1). taut-proto 0.9.1 has no `MISSING_OK`, gwz-core's production generator takes only a published taut-proto (`protocol/regen.py`), and gwz-py's runtime dependency can only name a published package.

Operator decisions (2026-09-30):
- adopt 0.10.0 now, as one step ahead of CS1.1;
- drop `gwz_core::decode`, which the 0.10.0 runtime no longer has, instead of keeping a deprecated shim.

## 2. What changes

### gwz-core

- **Production protocol.** `protocol/regen.py` pins taut-proto 0.10.0. Regenerated: `src/protocol/generated.rs` and `src/checked_artifact/protocol/generated.rs` (additive: each message gains `MAX_DEPTH`, `MAX_ENCODED_LEN` and `decode`), `src/cbor.rs` (the 0.10.0 runtime), and `rust/vectors.rs` in both corpora. Both corpora's `golden.json` are unchanged.
- **Public API.** `src/lib.rs` re-exports `try_decode` in place of `decode` (operator decision). `diff::output::decode_record` uses the generated `DiffOutputRecord::decode`, and the D17 contract comment on `encode_record` names it. `tests/protocol.rs` and the candidate-only `src/transport_host/message_embedding_tests.rs` use `try_decode` or a message's `decode`. The glob re-export of the generated protocol adds two root items, `gwz_core::MAX_DEPTH` (32) and `gwz_core::MAX_ENCODED_LEN` (`None`). `gwz_core::cbor` also breaks at source level: it loses the panicking accessors (`get`, `int`, `float`, `text`, `bytes`, `boolean`, `array`, `get_opt`), `DuplicateMapKey` carries a `MapKey` instead of an `i64`, and `DecodeError` gains `TooDeep` and `TooLarge`. `docs/RustApi.md` states all of this, and `docs/Protocol.md` names `try_decode`.
- **taut-shape 0.10.0** in `Cargo.toml` and `Cargo.lock`. Its one user, `diff/log_service.rs`, is unchanged.
- **`protocol/check_log_additive.py`.** 0.10.0 exports IR version 2, which adds `options` at every level, `member_options` on enums and `effective` on the file and each message. The check fingerprints `ir_version_1` of the export: it drops those keys, sets `version` to 1, and refuses a schema that declares any option. The pinned fingerprint does not move. A new `tests/protocol.rs` test, `log_addition_check_refuses_a_schema_that_declares_a_taut_option`, proves the refusal; it fails against a projection that drops a declared option instead.
- **Dead imports** (remove-dead-code rule). Three imports that the candidate test build reports as unused, and that no inclusion uses: `protocol::*` in `src/transport_host/cancellation_tests.rs` and `https_cancel_mux_tests.rs`, and `BodyExt` in `src/git/endpoint/https_fixture.rs`, which four test modules include.
- **The candidate and consumer generators** (`protocol/candidate/`, `tests/transport_consumer/protocol/`) move from a taut commit pin to the release pin. They generate with the taut-proto release installed in the interpreter's site directories at the pinned version. They refuse metadata that lies anywhere else (checked before taut is imported), and any `taut` module that is not that distribution's own file, so a copy on `sys.path` or `PYTHONPATH` can't shadow the release, with or without its own metadata. `taut-source-revision`, `taut-source-sha256`, `taut-extension-sha256` and `--taut-source` are gone: nothing uses the checkout mode any more. 0.10.0's `emit` no longer takes `fail_closed`. `owner-schema-sha256` moves to `205c19ab…`, gwz-transport's IR version 2 export. Their tests replace the checkout checks (revision, dirty tree, canonical `src/`) with three refusals: a wrong release, a shadowing package, and a shadowing package that carries its own release metadata. The pins lose `codec`, which nothing reads any more, and the consumer generator loses its inert `codec` guard.
- **Regenerated candidate artifacts.** `src/protocol/candidate_generated.rs` (additive, as above), `protocol/candidate/corpus/rust/vectors.rs`, and the consumer's `src/generated.rs`. `candidate_generated.py` and the candidate's `golden.json` are unchanged.
- **CI.** Five workflow pins (`release.yml` twice, `platform-matrix.yml`, `windows-matrix.yml`, `manual-compiler-tests.yml`) and the `tests/publish_workflow.rs` tripwire move to 0.10.0. `tests/transport_consumer/README.md` describes the release pin.

### gwz-py

- **Pins.** `pyproject.toml` (runtime and dev), `scripts/regen_protocol.py`, `scripts/release.py`, `package-smoke.yml` and `publish.yml` move to 0.10.0. `uv.lock` moves taut-proto only, and `Cargo.lock` moves taut-shape only.
- **Generated protocol.** `src/gwz/protocol/generated/gwz.ir.json` is regenerated as IR version 2. The generated `api.py` is unchanged.
- **`scripts/check_protocol_drift.py`.** The pre-log fingerprint is taken over the same `ir_version_1` projection. The pin does not move. `src/tests/test_protocol_drift.py` and `src/tests/test_log_protocol.py` project the packaged IR the same way, and a new test proves the projection refuses a declared option.
- **`native/src/codec.rs`.** `decode_cbor` uses `gwz_core::try_decode` and maps its `DecodeError` to the protocol error. It keeps the unwind guard, as `decode_message` does.

### gwz-transport

- **Generator.** `protocol/generator.json` pins the release, with no taut commit and no extension digests. It also drops `codec` and `forward_compat`, which `scripts/regen.py` never read. `scripts/regen.py` has the same release provenance, and its checkout mode is removed. `scripts/test_regen.py` replaces the checkout test with the same three refusals.
- **Regenerated.** `src/protocol.rs`, `src/cbor.rs` and `protocol/transport.ir.json` are regenerated; `src/admission.rs` is unchanged.
- **`src/codec/validate.rs`.** It imports `super::MAX_DEPTH` explicitly. The generated protocol now exports a file-level `MAX_DEPTH` (32, taut's decode bound), which collided through glob imports with the codec's own limit (16).
- **CI.** `.github/workflows/contracts.yml` no longer checks out taut: it installs taut-proto 0.10.0 and runs `regen.py --check`. The job has been red since `f8ebef7` because its taut checkout ref lagged the generator pin.
- **`tests/pool.rs`.** `cargo fmt` fixes the formatting `cargo fmt --check` already failed on at HEAD.
- **Docs.** The README's generation paragraph and a `src/lib.rs` comment name 0.10.0.

### Root and workspace

- **`Cargo.lock`.** It moves to taut-shape 0.10.0. The root `[patch.crates-io]` on taut-shape-rs (already at 0.10.0) applies again. Before this step a `--locked` build at the root failed, and without `--locked` Cargo would fall back to crates.io 0.9.2.
- **CI.** `.github/workflows/debt-recovery.yml` pins taut-proto 0.10.0.
- **The workspace's taut checkout** is fast-forwarded to `v0.10.0` (`a7cab03`). This is the checkpoint ritual's third leg: tests that put `taut/src` on `PYTHONPATH` (`gwz-core/tests/protocol.rs`, gwz-py's drift check) then see the release's source.
- **The root lock** records it at commit time, together with another session's `gwz pull` of taut-shape, taut-shape-py and taut-shape-rs, already staged. taut-shape (`5779c96`) and taut-shape-py (`f86f0e9`) are at their `v0.10.0` tags. taut-shape-rs is at `5026715`, its `origin/main`, one commit past `v0.10.0` (`8f86a9c`). That commit changes only its release workflow (`publish.yml` renamed to `release.yml`, `gearu.toml`), so the crate source is the tag's. The ritual's third leg governs the taut checkout, and another session owns taut-shape-rs's position, so this step leaves it there.

## 3. What this step claims

1. **The wire bytes don't change.** Regenerated under 0.10.0, gwz-core's protocol corpus (155 vectors), checked-artifact corpus (17) and the candidate corpus (175) produce byte-identical `golden.json`. Both pre-log fingerprints (`check_log_additive.py`, `check_protocol_drift.py`) are unchanged over the version 1 projection.
2. **The generated Python surfaces don't change.** gwz-py's `api.py` and the candidate's `candidate_generated.py` are byte-identical.
3. **Decoding changes, as 0.10.0 specifies** (taut `docs/RUST_API.md` §8, `docs/PYTHON_API.md` §8):
   - input nested deeper than a message's `MAX_DEPTH` (32 for every gwz message, since gwz declares no option) is `TooDeep`. `TooLarge` needs a length bound, and gwz declares none, so its decodes never return it;
   - `DuplicateMapKey` carries a `MapKey`, and a `map<K,V>` refuses a repeated key;
   - Python refuses an absent optional field as `MissingKey` unless it is `MISSING_OK`. Rust already refused it, and both encoders write every field (null for None), so gwz's own traffic is unaffected.
4. **The public API loses `gwz_core::decode`,** and `gwz_core::cbor` changes as §2 lists. The panicking decode goes with the 0.10.0 runtime. Its three in-tree users move: gwz-py's native codec and gwz-core's `tests/protocol.rs` to `try_decode`, and gwz-core's `diff/output.rs` to the generated `DiffOutputRecord::decode`. gwz-cli has none.
5. **No generator reads a taut checkout.** Five generator paths take the released taut-proto 0.10.0, and a taut commit no longer forces re-pins. Each checks provenance its own way:
   - **gwz-core's production generator** (`protocol/regen.py`) runs `tautc` from a uv venv that it provisions with the pinned PyPI release.
   - **The candidate and consumer generators, and gwz-transport's**, accept the release only from the interpreter's site directories.
   - **gwz-py's `scripts/regen_protocol.py`** checks that the installed version equals its pin and drops `PYTHONPATH` for the generation it runs. It has neither the site-directory binding nor the module-origin check, so a copy of taut with its own metadata ahead of site-packages passes its version check. Its outputs are still guarded by `--check`, the drift check and the byte-identity of the generated API. Giving it the uniform rule is recorded as open in the checkpoint.

     *Dated note, 2026-09-30, after acceptance:* `regen_protocol.py` now has the uniform rule, on the operator's word. Its generating children, still run without `PYTHONPATH`, accept taut-proto only from the interpreter's site directories at the pinned version, and check every loaded taut module's origin. See the checkpoint entry of the same date.

## 4. Not in this step

- **gwz-cli's standalone `Cargo.lock`.** It is refreshed at each gwz-core release ("Refresh the standalone Cargo.lock for gwz-core 1.0.17"). It has been stale since the git2-rs rename, and the next release refresh brings taut-shape 0.10.0 with it.
- **`MODULE.bazel.lock`**, already recorded as stale.
- **The consumer's archive proof** (`tests/transport_consumer/package_proof.py`). It refuses an archive packed from a dirty tree, so it runs after gwz-transport is committed. Before the commit, the same isolated layout ran against the live gwz-transport through a path patch (§5).
- **The candidate CI job**, which stays the next planned step.
- **Declaring taut options** (`max_depth`, `max_encoded_len`) in gwz schemas. Both projection checks refuse a declared option until a step adopts them deliberately.
- **Other lanes' uncommitted work**, gwz-transport's `Cargo.toml` among it.
- **taut `main`'s `f6d6276`**, one commit past the tag, which only sets `fallback_version`. The workspace checkout stays at the tag.
- **Candidate build warnings that predate this step.** The candidate `--lib --tests` check reported 14 warnings at HEAD and reports 11 here; the difference is the three dead imports above. Six are helpers in `https_fixture.rs` that the `transport_host` inclusion does not use; the compiler reports none of them for the three `git/endpoint` inclusions, and none of the four has a `dead_code` allowance. Five are candidate transport-host code that the non-test candidate build never calls: `Request::open_https` (`request.rs:74`), `with_https` (`mod.rs:302`), the fields `stream_id` and `policy` (`request.rs:408`, `:415`), and the `SshOpenFailure` re-export (`mod.rs:38`, used only by tests). Whether each is staged for activation or dead is for the operator and the transport lane.

## 5. Evidence

The round-1 runs in the first table are over the code files of the pre-evidence snapshot (manifest SHA-256 `b5a14898…`, 61 entries), re-verified with 0 discrepancies after the runs. The round-1 bundle (`ecaf0ce9…`) differed from that snapshot only in this document. The round-2 bundle (`58b7c925…`) differs from round 1's in the 14 files the RemPlan's dispositions touched, and the second table lists the reruns over it. Logs are in the bundles' `evidence/`.

| Run | Result |
|---|---|
| gwz-core `python3.13 scripts/run_tests.py --no-fail-fast` (the ordinary gate) | exit 0; 2,321 passed, 0 failed, 1 ignored. Source checks: filesystem boundary, process-global guard (gwz-core, and gwz-transport beside it), conditional-compilation guard (gwz-core, gwz-cli, gwz-py) and crate versions all report nothing new |
| gwz-core candidate (`RUSTFLAGS='--cfg gwz_transport_candidate'`, `run_tests.py`'s four phases over a tree `tests/transport_backend/prepare.py` made) | every phase exits 0; 2,524 passed, 0 failed, 1 ignored |
| gwz-core `protocol/regen.py --check` (taut-proto 0.10.0 from PyPI) | committed artifacts current |
| candidate and consumer regeneration `--check`, and their pytest suites | verified; 8 and 7 passed |
| gwz-core `cargo fmt --check` | clean |
| gwz-transport `scripts/` unittest, `regen.py --check`, `cargo +1.96.0 fmt --all -- --check`, `cargo +1.95.0 test --locked`, `cargo +1.95.0 package --locked` (`--allow-dirty` locally) | 5 tests OK; 4 artifacts verified; clean; 166 passed, 0 failed, 2 ignored; the package verifies |
| the consumer proof's isolated layout (`package_proof.py`'s copy, with gwz-transport patched in by path instead of an archive), `cargo +1.95.0 test --offline --locked` | 31 passed, 0 failed |
| gwz-py `maturin develop`, then pytest with `GWZ_RUST_BIN` = the root workspace's freshly built `gwz` | 919 passed, 0 failed |
| gwz-py `scripts/check_protocol_drift.py` | OK `sha256:413fffd1…` |
| root `cargo metadata --locked` | resolves; it failed before this step |

Round 2 (the corrections in the [RemPlan](GwzTaut010Adoption-RemPlan.md)) changed no generated file; in Rust it changed only the D17 doc comment. Rerun over the round-2 tree (log `evidence/round2-quick.log`):

| Run | Result |
|---|---|
| candidate and consumer regeneration `--check`, and their pytest suites | verified; 9 and 8 passed (each one more: the metadata-carrying shadow) |
| gwz-transport `scripts/` unittest, `regen.py --check`, `cargo +1.95.0 test --locked`, `cargo +1.95.0 package --locked` | 6 tests OK; verified; 166 passed, 0 failed, 2 ignored; the package verifies |
| gwz-core `protocol/regen.py --check`, `cargo fmt --check`, `cargo check --locked --lib` | current; clean; builds |
| gwz-core `cargo doc --no-deps --lib` | the new `decode_record` link resolves; rustdoc's two unresolved links (`src/artifact/store.rs:10`, `src/diff/render/options.rs:38`) predate this step |
| the Safety reviewer's round-1 attack (a modified copy of taut plus `taut_proto-0.10.0.dist-info` on `PYTHONPATH`) against all three generators | each refuses it: the metadata lies outside the interpreter's site directories |

Probes run before the step, in scratch:
- taut-proto 0.10.0 regenerated gwz-core's production and checked-artifact protocol with byte-identical `golden.json`.
- gwz-core's IR under 0.9.1 and 0.10.0 differs only in version 2's option maps and `version`.
- gwz-core builds against the taut-shape 0.10.0 crate, and its 114 diff and log-service tests pass.
- The candidate `--lib --tests` check reports the same warnings at HEAD, less the three dead imports (§4).

## 6. Records written at commit

- **The checkpoint entry** for this step. Release pins replace commit pins in the three generators that pinned taut's commit, and every generator path now takes the release (claim 5), so the current top entry's standing items on commit pins lapse: "every taut commit costs three re-pins" and the idea of pinning taut's `src/` tree instead.
- **Plan text for the plan's next revision**, through the checkpoint as the plan's §7 routes it:
  - [GwzCoreSessionPlan.md](GwzCoreSessionPlan.md) line 109. Regenerating the candidate protocol needs taut-proto 0.10.0 installed in the interpreter's site directories, not a taut checkout at `bcf98b64…` plus taut-proto 0.9.1.
  - CS1.1's regeneration inputs, likewise.
- **The landing order** (Safety residual): commit gwz-core, gwz-py and gwz-transport; `gwz capture` the root lock from the observed state; commit the root. A root commit whose lock names the members' pre-step heads never happens.
- **Deferred to the step that adopts taut options**: `ir_version_1` drops `effective` without comparing it to taut's defaults.
