# Step 2.2 unpark the cli placement (gwz-core df31c221, gwz-cli 54a74ecb, gwz-py bde35e92) — CODE-AXIS REVIEW

**Review object:** gwz-core `df31c22176` (one commit on `9a9a659e`, 76 files, +1995 −13180), gwz-cli `54a74ecb4e` (one commit on `9ecc2b7`, 9 files), gwz-py `bde35e923e` (one commit on `22e4f13`, 50 files), and the evidence run `2026-10-25-p2-2` at gwz-core-evidence `ecaf00322f`.
**Baseline:** gwz-transport `22b53dce5d` (unchanged). Other members: gwz-git `e3bc2a41f6`, taut `913e0dc35c`, git2-rs `d13951f7e0`, gwz-sspi `a2978a8f3f`, gwz-credential `6d78c593a4`, taut-shape-rs `959f0f048a`. The tuple was the same at the start and at the end, and the three product members had clean working trees at the end.
**Date:** 2026-10-25
**Axis:** Code, with a short Surface section
**Verdict: GO.** P0 0, P1 0, P2 0, P3 4.

## 0. Evidence base

Commands I ran myself:
- `uptime` before each run. Load was 28, 53 and 30 at the checks, and later 63.
- `python3.13 protocol/regen.py --check` in gwz-core: **OK**. `check_log_additive: OK sha256:4d377a49…`, and the generated code and both corpora are current.
- `CARGO_TARGET_DIR=…/p2-unpark-ordinary cargo test -j 8 --test protocol` in gwz-core: **45 passed, 0 failed**. This includes `ordinary_placement_and_attachment_fields_decode_when_the_writer_lacks_them`, `generated_protocol_is_current` and both `log_addition_*` rows.
- gwz-py `.venv/bin/python -m pytest -q -p no:cacheprovider src/tests/test_codec.py`: **12 passed**. This is the Python gate.
- gwz-cli `cargo test --locked --offline`: I started it at load 30. It got past `--locked` resolution against the lane root lock and was compiling crates. I stopped it when load reached 63. The test results are taken from the evidence log `raw/mac/cli-tests-2.log`.

Read-only checks:
- **Owner pin.** `shasum -a 256 gwz-transport/protocol/transport.ir.json` gives `48837b7e…1fe1`. This equals `_OWNER_SCHEMA_SHA256` in `protocol/gwz.taut.py`, `owner-schema-sha256` in `tests/transport_consumer/protocol/generator.json`, and the commit in `.github/gwz-transport.commit` (`22b53dce…`).
- **Committed tree equals the evidence tree.** I rebuilt the evidence's `digest.sh` input from `git diff 9a9a659e df31c221` plus the one added file (`retained-reader.json`). The result is `bd496a9d…`, identical to `raw/digests/final.digests`.
- **Runtime unchanged.** The deleted `src/cbor.rs` (taken from `HEAD~1`) differs from `gwz-transport/src/cbor.rs` only in rustfmt line-wrapping of 5 expressions. The encoder, decoder, duplicate-key and depth semantics are identical.
- **Artifacts match the old candidate.** The new `protocol/corpus/golden.json` is byte-identical to the old `protocol/candidate/corpus/golden.json`. `vectors.rs` and `generated.rs` differ from the old candidate's copies only by where the `TransportPlacement` enum sits (36 diff lines each).
- **Lockfiles.** The gwz-core, gwz-cli, gwz-py and root locks already listed `gwz-transport` among gwz-core's dependencies, so "Cargo.lock does not move" holds.
- Diffs I read: `gwz.taut.py`, `regen.py`, `check_log_additive.py`, `generated.rs`, `lib.rs`, `protocol/mod.rs`, `transport_host/{mod,request,cli_placement,session,session/checks}.rs`, `git/gitbackend.rs` and `transport_binding.rs`, `identity.rs`, `tests/protocol.rs`, the transport-candidate workflow, `prepare.py`, the checkers and inventories, gwz-cli `build.rs`/inventory/tests, gwz-py native dispatch, `api.py`, `check_protocol_drift.py`, `test_codec.py`, the workflow and `run_tests.py`.
- Evidence read: the README, `raw/schema/ir-delta.txt`, the red logs, the remote `final-r3` summary and results, the Windows receipt-b log, and the manifest.

## Problem, requirement provenance, and scope

**Requested outcome (plan §4 step 2.2).** The placement and attachment fields join the ordinary schema as an additive change. The Rust and Python missing-field gate runs first. The code is regenerated (L2-02) and `check_log_additive` is re-pinned. The Cli code, its inventories, the `check-cfg` declarations, the workflow session legs, the 29 `reason` attributes and the "1.2.0" comments leave `gwz_session_candidate`. The guide is rewritten.

**Coverage, all observed:**
- Gate: red logs, then green.
- Inventories: 36 session lines removed in gwz-core and 2 in gwz-py. The checker now refuses the retired switch with a `RETIRED` error.
- `check-cfg` declarations removed from gwz-core and gwz-cli `build.rs` and from gwz-py `Cargo.toml`.
- Reasons: 19 in gwz-core plus 10 in gwz-cli = 29. Zero remain.
- Comments: zero "1.2.0" strings remain outside lockfiles and dev-docs.

**Provenance of the scope-changing items:**
- *gwz-transport becomes a plain `[dependencies]` entry.* This traces to D-J and the lane owner's decision (1), whose outcome is deferred. It is also required by the schema: `RequestMeta.transport_message: Option<Envelope>` exists in every build. Its shape matches the existing `gwz-git = { path = … }` entry.
- *The ordinary-build refusal of an explicit `cli` placement* (`src/git/gitbackend.rs:237–246`). It is necessary for the requested outcome: once `placement` decodes in every build, an ordinary core would otherwise run a `cli` request on its own credentials. That breaks the invariant in the placement design §3 and in the guide: the runtime never silently switches an explicit `cli` request to local credentials. The fix is small and stays inside gwz-core, so it is authorized and in scope.
- *Deleting `protocol/candidate/`, `candidate_generated.rs` and `src/cbor.rs`.* The candidate artifacts are dead once the projection is ordinary (consistent with "remove dead code now"). `src/cbor.rs` cannot coexist with external types (`regen.py` docstring).

**Components:** gwz-core, gwz-cli, gwz-py and the evidence member only. gwz-transport is reference-only and unchanged. No `crates/session-*` file and nothing belonging to 2.2b is touched.

## Surface section

**Python callers.** The generated dataclasses have no defaults, so these constructors now require new keyword arguments, each `None`-able:
- `TransportOptions(placement=, endpoint_path_base=)`
- `RequestMeta(transport_message=)`
- `ResponseMeta(transport_message=)`
- `TransportCapabilitiesResponse(message_versions=, placements=, schemes=, auth_policies=, message_limits=)`

An old call raises `TypeError`. This follows the C2.1 precedent (`max_retries`), which `gwz-py/docs/Migration-1.1.0-DRAFT.md` §1 documents.

**gwz-py `--json` / `--jsonl`.** These render with `dataclasses.asdict` (`src/gwz/cli_render.py:79`, `json_default`). Every response's `meta` therefore gains `"transport_message": null`. The Rust `gwz --json` builds `meta` from a fixed field list (`gwz-cli/src/response_meta_json.rs`) and does not change.

**Namespaces.** `gwz.protocol` and the Rust crate root (`pub use protocol::generated::*`, `src/lib.rs:86`) now export the owner's 42 declarations: `Envelope`, `Open`, `Data`, `Close`, `Failure`, `Identity`, `Limits`, `Window`, `Scheme`, `AuthPolicy`, `ErrorCode` and the rest. `gwz_core::ErrorCode` and `gwz.protocol.ErrorCode` are therefore the *transport's* error code, separate from `GwzErrorCode` and `gwz_core::model::ErrorCode`. This compiles today, but `use gwz_core::*` together with `use gwz_core::model::*` becomes ambiguous on `ErrorCode`.

**Capabilities.**
- The ordinary handler returns the five extras as absent, which means unsupported.
- `TransportRuntime::capabilities` in the transport build advertises `local`, plus `cli` when a client endpoint is installed. It advertises `ssh` only through the public `TransportRuntime::new`, and `https` only through the crate-internal `with_https`. This is consistent with 2.0 §4.8 for an in-process core.
- Rust struct literals that list fields exhaustively stop compiling. The placement design §3 says this kind of source compatibility is not promised.

**Release notes.** P3-3 covers the missing release-notes entries.

## 1. Findings

**P3-1 — The new-writer → old-reader direction has no run at this tuple, although this step edited the suites that prove it.**
- **Location:**
  - `gwz-core/tests/transport_consumer/tests/candidate_compat.rs:47` (`candidate_writer_is_readable_by_retained_old_rust_decoder`). Its `new` module is now `core_generated` = `src/protocol/generated.rs` (`tests/transport_consumer/src/lib.rs`, changed by this step).
  - `gwz-core/tests/transport_consumer/candidate/test_candidate.py:226` and `:240`, where the projections and pins were rewritten by this step.
  - The ordinary gates, `tests/protocol.rs:173` and gwz-py `src/tests/test_codec.py:559`, decode only old-writer bytes with the new reader.
- **Invariant:** placement design §3: "keep retained old readers working". The brief's gate needs both directions to hold for mixed versions.
- **Reproduction:**
  - `grep -rn "candidate_writer_is_readable\|test_retained\|test_candidate.py" raw/` in the evidence run returns nothing.
  - The only runner of these suites is `.github/workflows/transport-candidate.yml:145` and the consumer crate, which run after push.
  - The property does hold by construction: the old decoders look keys up with `Cbor::try_get` (`gwz-transport/src/cbor.rs:250`) and ignore unknown ones. The red log `raw/red/py-codec-placement-before.log` also shows the pre-step Python decoder silently accepting key 4 ("DID NOT RAISE").
- **Impact:**
  - Two compatibility suites changed in a wire-format step but have no recorded green run. If the rebased `core_generated` compile or the rewritten projection is broken, that is found only by CI after push.
  - This is not a demonstrated defect.
- **Correction:** run `cargo test --locked --manifest-path tests/transport_consumer/Cargo.toml --test candidate_compat` and `pytest tests/transport_consumer/candidate/test_candidate.py` at the tuple, and archive the logs in the run.
- **Closure test:** both logs green in `2026-10-25-p2-2/raw/`, with the digest equal to `bd496a9d…`.

**P3-2 — The ordinary build validates only one of the three new request-side fields. The transport build validates all three.**
- **Location:**
  - `gwz-core/src/git/gitbackend.rs:237–246`: the ordinary arm refuses only `placement == Cli`.
  - `src/transport_host/request.rs:458–495`: `validate_meta` also refuses a present `transport_message` (`:463`, `invalid`) and an `endpoint_path_base` without `cli` (`:493`). This runs only in the transport build, behind `TransportRuntime::request`.
- **Invariant:**
  - The same wire request should get the same answer, or a documented refusal, from each build.
  - 2.0 §3.1: a present `transport_message` is `invalid_request`.
  - Placement design §3: missing and null are equivalent, and a field the build cannot honour must not be silently dropped.
- **Reproduction:**
  - Ordinary build, any network operation with `RequestMeta { transport_message: Some(_) }`, or `TransportOptions { endpoint_path_base: Some(..), placement: None }`: the request proceeds and the field is ignored.
  - Transport build (gwz route): the same request fails with `InvalidRequest`.
  - Both fields are now constructible by Python callers, as required keyword arguments.
- **Impact:**
  - Diagnosability and parity: a caller bug is silent on one build and an error on the other.
  - No credential or effect hazard; the placement refusal covers the credential case.
- **Correction:** apply the same two checks wherever the ordinary path reads `meta.transport`, for example beside the refusal in `with_transport`, or once in `OperationContext::from_meta`. Each build then refuses `transport_message` and a stray `endpoint_path_base` as `InvalidRequest`.
- **Closure test:** extend `cli_placement_refusal_tests` (`gitbackend.rs:485`) with both cases in the ordinary build.

**P3-3 — The step's user-visible surface changes have no release-notes entry.**
- **Location:**
  - `gwz-py/docs/Migration-1.1.0-DRAFT.md` §1 (constructors) and §2 (`--json` keys) are unchanged by this step.
  - Plan step 6.4's notes list does not name these items.
  - The evidence README item 5 records only the constructor change.
- **Invariant:** user-facing changes carry a documented default or migration note (the precedent in Migration DRAFT §1 and §2); discoveries are recorded where they will be acted on.
- **Reproduction:**
  - `TransportOptions(default_identity=None, remote_identities=[], url_scheme=None)` raises `TypeError` at bde35e92.
  - `gwz-py … --json` output now contains `meta.transport_message: null` (from `json_default` → `asdict`). This is not recorded anywhere.
  - `gwz_core::ErrorCode` and `gwz.protocol.ErrorCode` now name the transport's code. This is not recorded either.
- **Impact:** Python callers and scripts that parse output meet unannounced changes. Rust users of the crate root get a confusing `ErrorCode`.
- **Correction:** add rows to Migration DRAFT §1 for the four classes, a §2 sentence for `meta.transport_message`, and a namespace note for the owner names (Rust and Python), or record them on 6.4's list.
- **Closure test:** these entries exist in the notes vehicle and cite step 2.2.

**P3-4 — The guide does not say that `CliEndpoint::new(SshEndpointConfig)` is transitional, although the recorded decision says it does.**
- **Location:** `gwz-core/docs/TransportPlacement.md:160–165` and the example at `:297–305`. These present `CliEndpoint::new(SshEndpointConfig)` as the interface. There is no "HostHandle" or "3.1" anywhere in the file. Only the Rust module comment `src/transport_host/cli_placement.rs:4–6` says it is replaced.
- **Invariant:** plan §1.1 row for `docs/TransportPlacement.md` (`install_cli`/`CliEndpoint::new(SshEndpointConfig)`, rewritten in 2.2 and 2.4). Decision (2) and the evidence README ("the guide says so").
- **Reproduction:** `grep -n "HostHandle\|3\.1" docs/TransportPlacement.md` returns nothing.
- **Impact:** embedders build against a constructor that step 3.1 removes, with no warning. The evidence README states something the guide does not contain.
- **Correction:** one sentence under "Transport-build Rust interface" naming step 3.1 and the client owner's `HostHandle` form.
- **Closure test:** the guide line exists, and the guide test (`tests/transport_backend/guide_test.rs`) still compiles the example.

## 2. Invariant analysis (attacks that failed, with evidence)

**Tag collisions and changes to existing fields.**
- The new tags (TransportOptions 4–5 on 1–3; RequestMeta 10 on 1–9; ResponseMeta 9 on 1–8; TransportCapabilitiesResponse 3–7 on 1–2) sit strictly above the existing ones.
- `ir-delta.txt` reports "enums removed or changed members: []" and "services identical: True".
- The placement design §3 table matches field for field. The TransportObservation 9–12 rows already landed in C2.1.

**Additive pin.**
- `regen.py --check` and the protocol suite reproduce `4d377a49…` after the projection strips exactly the 9 fields, `TransportPlacement` and the 42 owner names. The projection requires each field byte-exact (tag, type, `missing_ok`) and refuses a missing owner name (`check_log_additive.py:343–381`).
- A future addition to the owner's schema would leave the composed IR unprojected and fail the pin loudly, as would any pin drift. `_with_owner` refuses a digest mismatch and core/owner name collisions (`gwz.taut.py:2665–2678`).

**When the owner schema moves.**
- Rust re-exports the owner types (`generated.rs:4–45`) and compiles against whatever gwz-transport is checked out. Drift is caught by the digest in `gwz.taut.py` through `regen --check` and `generated_protocol_is_current`, and in CI because every gwz-core workflow checks gwz-transport out at `.github/gwz-transport.commit`.
- The pin must move in four places (`gwz.taut.py`, `generator.json`, `.github/gwz-transport.commit`, gwz-py's `gwz.ir.json`/`api.py`) and the owner lists in two (`check_log_additive.py`, gwz-py `check_protocol_drift.py`). Every one fails closed. This is a maintainability cost, not a silent hazard.

**One `Envelope`.** Rust has a single type, re-exported from `gwz_transport::protocol`. Python generates the owner types from the same pinned IR. That copy is generated, not a hand copy, and gwz-py's drift check reproduces the pin.

**Encoder semantics.** As shown in §0, `src/cbor.rs` → `gwz_transport::cbor` is a formatting-only diff, and the corpus is byte-identical to the old candidate's. The merge-request hex pins agree between Rust (`tests/protocol.rs` `EXPECTED`) and Python (`test_codec.py`). Every pre-existing byte is unchanged, plus `a9→aa` and `0a f6`.

**Missing and present-malformed decoding.**
- Rust: absent and null both decode, and text, an unknown enum value 3, a wrong-typed path base, a non-Envelope in tag 10 and an unknown placement in a capabilities list all fail (`tests/protocol.rs:173–362`).
- Python: the same, using genuinely shorter older-writer bytes (`test_codec.py:559–659`).
- I reran both; both are green.

**Does the refusal break a legitimate `cli` user?** I found none. In gwz-cli, `grep TransportPlacement` finds no use. In gwz-py, only tests and the generated code mention it, and `cli_shared.py:520` and `client.py` send `None`. The transport build keeps its own `!is_cli_context()` refusal (`gitbackend.rs:226–236`). A non-network operation with `placement = cli` runs locally with no network or credential effect, because `has_options` excludes placement.

**Leftover references.**
- `git grep` finds no `candidate_generated`, `protocol/candidate` or `src/cbor.rs` outside history and the assertions that they are gone.
- `check_checked_artifact_boundaries.py` dropped the `cbor.rs` and candidate edges.
- `prepare.py` no longer emits `candidate_source`.

**cfg and parity rules.**
- Every changed conditional is a `cfg_if!` arm or an inline braced module (`#[cfg(test)] mod cli_placement_refusal_tests { … }`). Control flow is braced.
- No globals were added.
- `tests/transport_consumer/src/lib.rs` includes `src/` rather than putting production code under `tests/`.
- The remote r3 run passed `check_cfg_boundaries`, `check_process_globals` and `check_candidate_switches` (24, 19 and 6 sites).

**gwz-cli lock.**
- gwz-cli builds in the root workspace. The lane root `Cargo.lock` has the root `248c2450` fix (gwz-connection and gwz-git entries) applied in its working tree, uncommitted, with mtime 06:53. That is lane state, not product.
- My `--locked --offline` run resolved without error before I stopped it.
- gwz-cli's member lock already lists `gwz-transport` under gwz-core.

**gwz-py's 23 skips.** These are the transport rows of the candidate extension. They cover gwz-py's transport-build bridge. The step's gwz-py changes to that build are the collapsed `ResponseMeta` arms (compiled: `raw/mac/py-candidate-check.log`) and `transport_message=None` at call sites. No gwz-py code sets `placement`. The rows ran in the old both-switches leg. This leaves no coverage hole specific to this step beyond P3-1; they run in CI after push.

**Windows.**
- Receipt `p2-2-20261025-b` PASS: transport-shape check exit 0; 3971 passed with 7 named host gaps, all of which are protocol-generator rows covered on macOS and Linux.
- The committed gwz-core tree equals the receipt's digest.

**File size.** `cli_placement.rs` is 255 lines. `tests/protocol.rs` is discussed in §3.

## 3. Risks and next action

**Convergence with 2.2b (not a finding; another lane).**
- `gwz-dev-p2-opsvc` (gwz-core at `9a9a659e`) has uncommitted edits to `protocol/candidate/candidate.taut.py`, which adds `TransportLaneFrame` there.
- It also edits `candidate_generated.rs`/`.py`, the candidate corpus, `test_candidate_regen.py`, and `protocol/gwz.taut.py`, `check_log_additive.py`, `generated.rs`, the ordinary corpus, `tests/protocol.rs`, `candidate_compat.rs` and the inventory.
- After 2.2, every candidate file is a modify/delete conflict, and the rest overlaps line for line. 2.2b must rebase onto 2.2. `TransportLaneFrame` then goes into `gwz.taut.py`, which now composes `Envelope` directly, and `check_log_additive`'s projection gains its own block. Merges must be serialized, 2.2 first.

**Version skew in the capability lists (observation, accepted design).**
- `schemes` and `auth_policies` are lists of the *owner's* enums, and an unknown value fails the whole `TransportCapabilitiesResponse` decode (`UnknownEnum`). The placement design §3 mandates this ("a present malformed value is an error").
- Once 2.3 lets client and served core differ in version, a newer core that advertises a new `Scheme` or `AuthPolicy` member makes an older client's capability query fail outright rather than degrade to "unsupported".
- Worth a decision before 2.3: tolerate unknown members in capability lists, or gate them by `message_versions`.

**Structural debt for the operator.** `tests/protocol.rs` was already 2107 lines, above the 1000-line split review. This step adds 183, to 2290. Per the split rule, that is a decision for the operator, not something to defer silently.

**Stale comments (trivial).**
- `platform-matrix.yml:35` and its siblings in the other workflows still say the two sibling path dependencies sit "behind gwz_transport_candidate".
- gwz-py `src/tests/test_client_host_transport.py:57` cites "gwz-core protocol/candidate".

**D-J shape.** `gwz-transport` is `publish = false` with no version, and gwz-core's dependency is path-only, like gwz-git. The publish chain still needs gwz-transport versioned and published before gwz-core (D-J's outcome is deferred). This step adds no new kind of blocker.

**Next action.**
1. Close P3-1: run and archive the two retained-reader suites.
2. Optionally close P3-2 to P3-4. Each is one or two lines of code or docs.
3. Run CI after the push (transport-candidate job: candidate Python tests and the consumer crate).
4. Then sequence the 2.2b rebase.
