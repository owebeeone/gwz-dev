# Step 2.2 unpark the cli placement: remediation plan (round 1)

Date: 2026-10-25. Lane owner's plan after [Code](GwzSplitCommandChannel-Step2.2-ReviewCode.md) (GO: 0 P2, 4 P3) and [State](GwzSplitCommandChannel-Step2.2-ReviewState.md) (NO-GO: 1 P2, 3 P3) on gwz-core `df31c221`, gwz-cli `54a74ecb`, gwz-py `bde35e92` (lane p2-unpark). State pre-committed to GO on the bounded remedy. One patch; State re-verdicts P2-1; Code re-verdicts its P3s if touched. No architectural root cause.

## Blocking

| ID | Disposition | Where | Closure |
| --- | --- | --- | --- |
| S-P2-1 | `_with_owner` normalizes line endings before hashing and parsing (`raw.replace(b"\r\n", b"\n")`); the same normalization in `tests/transport_consumer/protocol/regen.py`'s raw pin so the two stay consistent (gwz-transport is reference-only here; no `.gitattributes` change there). | `gwz-core/protocol/gwz.taut.py` `_with_owner`; `tests/transport_consumer/protocol/regen.py` | Rust or Python test: loading `gwz.taut.py` with `GWZ_TRANSPORT_SCHEMA` pointing at a CRLF copy of the pinned export succeeds; a copy with one tag changed is refused. One Windows run on a CRLF checkout WITH taut-proto 0.10.0 installed (a venv on dabeest under `E:/gwz-tests/<label>/`, nothing system-wide) with `generated_protocol_is_current` and both `log_addition_*` rows green; the receipt then stops listing those three rows as host gaps (classify gaps by failure text, not test name). |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| S-P3-1 | gwz-py `Client`: when `transport.placement == cli`, require `cli` in `capabilities.placements` and a common `message_versions` before the call, else raise `UnsupportedOperation` locally (placement design §3). Test with a fake bridge returning an old two-key capabilities answer: the Client raises and the bridge records no call. | `gwz-py/src/gwz/client.py` `_require_transport_capability`, tests |
| S-P3-2 | The trait default `with_transport` (and `has_options`) also refuses `placement == Some(Cli)` with `UnsupportedOperation`; row: `FakeGitRepository::with_transport` given `cli` is refused. | `gwz-core/src/git/gitbackend/contract/methods_transport.rs`, `transport_support/identity.rs` |
| C-P3-2 | The ordinary build refuses a present `RequestMeta.transport_message` and an `endpoint_path_base` without `cli` as `InvalidRequest`, the same as the transport build's `validate_meta` (one place: beside the refusal in `with_transport` or in `OperationContext::from_meta`); rows in `cli_placement_refusal_tests`. | `gwz-core/src/git/gitbackend.rs:237–246` |
| C-P3-1 | Run and archive `cargo test --locked --manifest-path tests/transport_consumer/Cargo.toml --test candidate_compat` and `pytest tests/transport_consumer/candidate/test_candidate.py` at the final tuple (new-writer → old-reader direction). | evidence run `raw/` |
| S-P3-3 | The remote matrix run of the final tree records the tree digest (`tools/digest.sh`) at start and end; rerun the remote matrix against the remediated tree with the digest. | evidence run |
| C-P3-3 | Release-notes entries: `gwz-py/docs/Migration-1.1.0-DRAFT.md` §1 rows for the four constructors' new keyword arguments, §2 sentence for `meta.transport_message: null` in gwz-py `--json`/`--jsonl`, and a namespace note that `gwz_core::ErrorCode`/`gwz.protocol.ErrorCode` now name the transport's code beside `gwz_core::model::ErrorCode`/`GwzErrorCode`; cite step 2.2. | gwz-py docs |
| C-P3-4 | One sentence in `docs/TransportPlacement.md` "Transport-build Rust interface": `CliEndpoint::new(SshEndpointConfig)` is transitional; step 3.1 replaces it with the client owner's `HostHandle` form. The guide test still compiles the example. | `gwz-core/docs/TransportPlacement.md:160–165` |

## Observations carried (no change in this patch)

Stale workflow comments ("behind gwz_transport_candidate"); `tests/protocol.rs` at 2,290 lines (operator decision on the split); the capability-list version-skew decision before 2.3; the gwz-py `--json` renderer dumping whole dataclasses (keep the `transport_message` slot absent in the served form, 2.0 §3.1, or give the renderer an allow-list: 2.2b/3.2); shape-divergent validation of `transport_message` in the served form belongs to 2.2b.

## Process

One patch in lane p2-unpark; gates: remote matrix with digests (`p2-2-rem1-20261025-a`), the Windows run described above (label `p2-2-rem1-20261025-a`), gwz-cli and gwz-py suites; evidence appended to run `2026-10-25-p2-2`; the lane owner commits (gwz-core with `Windows-receipt: p2-2-rem1-20261025-a`; gwz-py; gwz-core-evidence); re-verdict by the same reviewers (`-ReviewState-2.md`; Code re-verdict of P3s optional). Round count after this patch: 1 of 2.
