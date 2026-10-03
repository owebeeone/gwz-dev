# HTTPS SSPI composition implementation checkpoint — review draft

This is the cohesive step4b source draft under the accepted composition design,
Acceptance and BudgetDisposition. It is not a settlement, platform activation or
Windows runtime qualification. The existing `all(unix, gwz_transport_candidate)`
endpoint boundaries and profile-1 default offer remain unchanged. No Git state,
publishing, installation or remote credentials were changed by this drafter.

## Implemented production graph

CLI selected dispatch and Python ClientHost network entry capture the original
caller before fanout or Python detach/submission. Each context retains one SSPI
Supervisor; `CallerCapture` is opaque, Send+Sync+'static, non-Clone and non-Debug.
Capture holds the original thread without admitting a record. `start_captured`
validates issuer/request/admission synchronously; charged launch later rechecks
original-thread identity and liveness. There is no metadata lookup in captured
start or poll. Capture/start do not create a second capacity domain.

The actual HTTPS prepare/serve bridge performs source selection and native rounds
on the exclusive origin lease, using final-origin TLS channel binding captured
before type erasure, after proxy and origin TLS. A positive logical Open D is
anchored after admission and before checkout/adoption, and retained through
redirects, helper work, challenge carry, native rounds and authenticated reuse.
The existing cumulative HTTP I/O allowance remains independent. Zero refuses
native before Begin; anonymous, configured Basic and SSH remain usable without a
native worker. Initial nonempty Negotiate/NTLM offer tokens and Digest refuse
preBegin: the existing `step(None)` cannot consume these tokens, and the draft
neither discards them nor adds an API workaround.

Configured identity follows OD10 selection, including DOMAIN\\user splitting and
UPN preservation. WindowsParityDesign §7 and CredentialHelpersDesign §4 classify
no credential, unusable helper output and missing/unstartable git as no helper
identity; timeout/cancel/cleanup uncertainty remain terminal. Explicit identity
rejected after publication never falls through to current logon. Negotiate-only
and helpers-disabled routes do not call a helper. Native context redirects
refuse; configured Basic retains TR1.6 OQ4(a), validates the new discovery URL,
forwards no header and performs a new lookup if challenged there.

Transport Facts preserve source, scheme, unresolved/selected mechanism and
separate authoritative Continue. Native Complete is independent of remote
acceptance, and both are required for authenticated success. Authorization
publication is recorded at the send boundary after successful challenge drain.
Replacement/revocation of an authenticated generation refuses before POST; no
POST authentication replay was introduced. Setup merging retains native facts.
Existing application observations retain their lossy Unknown native method
projection: Unknown proves neither native selection nor Complete; the success
flag comes from independently validated remote facts.

Guard retains session/Finish receipt, operation dependency and endpoint request
slot. Finish ownership is installed before await; abort, cancellation or expiry
transfers its owned future and both charges into existing endpoint retained
cleanup. Reaping takes pending entries out of the shared lock, polls work and
checks proof outside locks, and drops confirmed entries outside locks. Completed
futures are never repolled. Pending/Unknown keep both charges; only Confirmed
releases them. Retained entries are bounded by the existing 64 charged request
slots, independently of SSPI's bounded tombstone history. No process owner,
public cleanup API or new runtime framework was added.

The final private wipe regression also observes the secondary core SecretHeader destructor before deallocation; its wrapper overwrites before the existing inner SecretBuffer Drop, which overwrites again. There is no observer field in production. This postimplementation receipt passed on first execution and is not claimed as RED first.

Owned challenge/Authorization storage is initialized at fixed size inside its
wipe owner before secret copying/encoding; fallible decode/parse/encode and Drop
are observed before deallocation. Existing SSPI identity/token/CBT storage is
retained. `http::HeaderValue`, Hyper/native-TLS and provider/dependency copies are
outside this owner's wipe guarantee; marking a header sensitive is not a wipe.
CBT's returned dependency Vec is wiped immediately after copying into SecretBytes.

Absolute-D checkout expiry deliberately reports Timeout with Phase::Other, no
setup cause and FirstConnect::None. The pool API does not expose whether the
outer observer expired during allocation or connection establishment; inventing
fresh-connect provenance would make an ineligible setup retry possible. Pool-
returned eligible first-connect failures retain their existing provenance. This
is a conservative classifier limitation, not a new pool API or borrowed cause.

## Modified file inventory and budget

Handwritten production/test files (34):

- SSPI (6): src/lib.rs; src/supervisor/{api,futures,mod,ports,remediation_tests}.rs.
- Transport (7): protocol/transport.taut.py; src/binding.rs; src/codec/validate.rs;
  src/mux/{mod,routing}.rs; src/policy.rs; tests/https_reuse.rs.
- Core (18): src/git/endpoint/https_auth/secret.rs; https_connection.rs;
  https_policy.rs; https_worker.rs; https_worker/{budget,credentials,native,
  prepare,serve}.rs; setup_retry.rs; src/git/gitbackend/transport_binding.rs;
  src/transport_host/{cancellable,https_endpoint,local_command,mod,session}.rs;
  src/transport_host/session/driver/opening.rs; tests/transport_backend/prepare.py.
- CLI (1): src/globalargs/dispatch.rs.
- Python (2): native/src/client_host.rs; native/src/route/transport.rs.

Python scripts/candidate_switch_inventory.txt is the separately authorized
administrative ledger (two exact network capture/attach rows), making 35 files
including ledger. Six member/API documents: SSPI docs/{CallerValues,Supervision}.md;
core docs/TransportPlacement.md; transport README.md; CLI README.md; Python README.md.
Generated outputs separately: transport protocol/transport.ir.json,
src/protocol.rs and src/admission.rs. External prepared manifests/derived locks
are not product source changes. Final added-line recount: 2,757 handwritten source/test added lines (1,927 production/source documentation; 830 test lines), plus two administrative ledger lines, 369 generated lines and 113 member/API documentation lines. Test classification uses the existing globals guard lexical test ranges, plus standalone test files; Rust doctest text stays with its API source. Whole changed-file format reflow is included, not discounted.

## Executed RED then GREEN evidence

All Cargo commands use Rust 1.95.0 and external target roots. Let
`C=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition`.
Core command `RUSTFLAGS='--cfg gwz_transport_candidate' CARGO_TARGET_DIR=$C/core-target
cargo +1.95.0 test --manifest-path $C/core-prepared/Cargo.toml --lib --locked --offline FILTER`.
The first derived-lock foundation invocation omitted --locked; subsequent runs
use the derived external lock. The following are actual production bridge
regressions, rather than pass counts attributed from unrelated SSPI tests:

| RED | Production fix and GREEN filter |
| --- | --- |
| Initial missing foundation symbols/Facts field; native initial/zero returned InvalidRequest | Real prepare bridge; production_initial_nonempty_and_zero_refuse_without_publication |
| WindowsConfigured Basic advertisement succeeded but exchange refused | Basic route pin; production_configured_basic_stays_usable_without_native_availability |
| Provisional Selected mechanism could switch/become Unresolved | History validation; provisional_resolution_cannot_switch_or_become_unresolved |
| Challenge drain failure claimed Authorization offered | Send-boundary publication; production_drain_refusal_never_claims_publication |
| Aborted pending Finish returned cleanup count zero | Retained Finish future/receipt; production_drop_during_finish_retains_both_charges |
| Occupied physical pool waited its 500ms allocation bound past 30ms D and reported allocation cause | Exact outer D, PhaseOther/no invented provenance; production_pool_wait_cannot_extend_fixed_deadline |
| Empty helper username and unusable/nonzero/unstartable helper outcomes became credential rejection | Accepted no-identity mapping; production_source_matrix_uses_current_logon_only_when_no_identity |
| Remote 200 final challenge after Continue returned Protocol | Consume final token without extra GET; production_final_remote_token_completes_without_extra_request |
| Basic302 was recorded as credential rejection and blanket-refused | Ordinary validated discovery without header forwarding; production_configured_basic_redirect_requeries_without_forwarding_credentials |
| Direct NTLM private port accepted Unresolved/Kerberos | Independent producer validation; direct_ntlm_observation_cannot_be_unresolved_or_kerberos |
| Anonymous native-policy advertisement could not open its POST | Preserve anonymous ordinary route; production_anonymous_advertisement_and_post_do_not_require_native_availability |

Transport actual mux RED twice (native policy profile admission then incorrect
configured-source acceptance under WindowsDefault) → GREEN8 targeted HTTPS reuse
rows after production mux/routing correction. Coarse platform policy selector
compile RED (missing private selector) → GREEN actual transport_binding selector;
explicit typed Anonymous remains unchanged. Codec boolean-match new Clippy RED →
`matches!` correction → strict transport Clippy GREEN.

The last full focused core `https_` run before the final secondary wipe observer: GREEN195/195 (20.71s), including the final three ordinary-policy/producer regressions. After the secondary SecretHeader destructor observer, the same Cargo command with FILTER=storage_wipes_before_deallocation passed1/1; current strict-core Clippy and provisioned host proofs were refreshed after this final edit. An earlier broad run was RED187/189
because a too-broad redirect guard hit existing Gh cases; narrowing to actual
native facts preserves both Gh and WindowsConfigured Basic. Earlier fixture
setup mistakes (checkout before holding a lease; shell DOMAIN printf escaping)
were corrected before meaningful RED claims. Additional postimplementation
regressions were GREEN on first execution and are not claimed as RED first.

## Composition §9 matrix tied to executed rows

| Obligation | Actual executed evidence |
| --- | --- |
| Immutable D, pool wait, helper crossing D/cancel, zero, independent HTTP expiry, eight rounds | production_pool_wait_cannot_extend_fixed_deadline; production_helper_crossing_deadline_or_cancel_never_falls_back; production_initial_nonempty_and_zero_refuse_without_publication; production_round_cap_and_http_allowance_do_not_reset_native_deadline; inherited redirects_do_not_refill_network_budget and anonymous_auth_transition_does_not_refill_the_network_allowance |
| Original capture, moved owned futures, context/capability lifetime, shared capacity, no metadata in poll | SSPI captured_starts_are_owned_context_bound_and_share_one_original_capture and capture lifecycle tests; compiled Supervision recipe and Clone/Debug compile-fail probes; CLI selected-dispatch wiring and callable worker_host::tests; loaded provisioned Python ClientHost call and overlapping submit rows |
| Source/challenge matrix and truthful mechanism/remote facts | production_source_matrix_uses_current_logon_only_when_no_identity; source_order_and_challenge_decode_are_closed_and_bounded; authority_continue_is_preserved_but_does_not_authorize_remote_success; provisional_resolution_cannot_switch_or_become_unresolved; production_remote_complete_and_cleanup_ownership_are_independent; production_final_remote_token_completes_without_extra_request; transport strict native vector/real mux rows |
| Lease generation, CBT, redirects, late cancellation/cleanup/publication | production_native_exchange_keeps_facts_and_refuses_replacement_before_post; production_configured_basic_redirect_requeries_without_forwarding_credentials; production_finish_observes_cancel_and_expiry_while_receipt_is_pending; production_drop_during_finish_retains_both_charges; production_seeded_finish_publication_and_retention_orders (seed0x53435049,8 actual composition-port schedules); inherited origin/proxy TLS verification and lease/reuse/retirement rows |
| Receive-pack Possible and post-byte failures/no replay, eligible fresh setup retry | Inherited actual HTTPS serve/opening receive-pack/retry/effect rows in final https_ run; native generation guard exercised before POST by production_native_exchange_keeps_facts_and_refuses_replacement_before_post |
| Owned wipe before deallocation, failure paths | storage_wipes_before_deallocation_on_encoding_decode_parse_and_drop (five live observations: failing encode, failing decode, failing duplicate-offer parse, primary header Drop, secondary SecretHeader Drop); inherited SSPI identity/CBT/token failure/cancel/revocation wipe rows; dependency copy exceptions above |

Inherited rows are unchanged coverage of the production paths composed here;
they are not attributed as new native provider tests. Native Windows campaign
rows remain deferred: original-thread/native OS behavior, actual NTLM/Kerberos
HTTP completion, final-origin EPA/TLS algorithms, native physical cleanup and
installed Windows host Hello mismatch. SSPI Windows source check is not native
execution. Portable fake native ports are private production orchestration seams.

## Final gate commands, results and disclosures

- SSPI `CARGO_TARGET_DIR=$C/sspi cargo +1.95.0 test --manifest-path gwz-sspi/Cargo.toml --locked --offline`: PASS79unit+5integration+26doctests, including compiled caller recipe and20 compile-fail probes. Strict `cargo +1.95.0 clippy --lib --locked --offline -- -D warnings`: PASS. Owner Windows source-only `cargo +1.95.0 check --locked --offline --lib --target x86_64-pc-windows-msvc`: PASS.
- Transport external target `cargo +1.95.0 test --manifest-path gwz-transport/Cargo.toml --locked --offline`: owner PASS, existing long seeded campaign ignored. Strict `clippy --lib --locked --offline -- -D warnings`: PASS. `$C/../schema-tools/bin/python gwz-transport/scripts/regen.py --check`: PASS4artifacts verified; generator rustfmt pin unchanged.
- Core strict candidate `RUSTFLAGS='--cfg gwz_transport_candidate' CARGO_TARGET_DIR=$C/core-clippy-target cargo +1.95.0 clippy --manifest-path $C/core-prepared/Cargo.toml --lib --locked --offline --message-format=json -- -D warnings`: full gate RED47 pre-existing candidate diagnostics. New native/prepare/secret/host bridge diagnostics zero after correcting new warnings. No full strict-core GO and no lint allowances are claimed. Legacy warnings in changed files belong to untouched existing connection/endpoint if bodies.
- CLI external cli-prepared `test --lib --locked --offline worker_host::tests` with candidate RUSTFLAGS: PASS1. Python external py-prepared `test --lib --locked --offline route::transport` with candidate RUSTFLAGS and PYO3_PYTHON=gwz-py/.venv/bin/python: PASS4.
- Supported local Python wheel producer: import scripts/build_candidate_extension.py, construct Prepared($C/py-provisioned-wipe-final,$C/core-prepared,$C/py-prepared/Cargo.toml), call build with absolute unresolved venv interpreter and external py-packaging-target, then unpack. RUSTUP_TOOLCHAIN=1.95.0, CARGO_NET_OFFLINE=true. Worker and extension share producer fingerprint. Final artifact/source refresh: PASS supported producer rebuild after final source, format and secondary wipe receipt settled, yielding gwz-0.0.0-cp310-abi3-macosx_11_0_arm64.whl and extracted _gwz_core.abi3.so.
- `GWZ_PY_NATIVE_MODULE=$C/py-provisioned-wipe-final/extension/gwz/_gwz_core.abi3.so gwz-py/.venv/bin/python -B -m pytest -q gwz-py/src/tests/test_worker_packaging.py -k 'actual_loaded_extension_selection or candidate_and_backend_preserve_owned_scratch_root or real_handoff_and_bundler_isolate_same_name_builds'`: PASS6,1existing macOS nonUTF8 fixture skip,18deselected.
- Same module pytest test_client_host_transport.py `-k 'gh_failure_and_an_unsupported_proxy or two_overlapping_operations_complete_independently'`: PASS2,8deselected; actual loaded constructor, synchronous call and asynchronous submit through candidate host, HTTPS helper/proxy refusal and independent SSH runtimes.
- Packaging setup failures are distinct: bare Cargo.dylib lacks a Python extension suffix (1failed/1skip); corrected unprovisioned run5PASS2skip was insufficient proof; resolving the venv interpreter symlink to base Python caused missing maturin. Corrected supported producer with the venv path provides the above provisioned proof. Homebrew dylib wheel dependency warning remains; no distribution portability claim.
- External CLI/Python prepared locks resolved offline against available cache (libssh2-sys0.3.3,openssl-sys0.9.117); these local derived locks are not clean packaged-release qualification or product lock changes.
- Final changed-source Rust formatting, cfg boundaries (including disabled branches), candidate inventory, process globals and diff whitespace: PASS: rustfmt +1.95.0 --edition2024 --config skip_children=true --check on all34 changed Rust files (including two generated Rust outputs); git diff --check on all5members; python3 gwz-core/scripts/checks/check_cfg_boundaries.py (1388files,433existing occurrences); check_process_globals.py (1183files,19existing items); check_candidate_switches.py with default core and --repo gwz-cli/--repo gwz-py (22/18/5 sites). No new conditional occurrences, globals or inventory debt.

No private raw evidence or campaign runner was added to the parent repository.
This checkpoint is a bounded review handoff; owner settlement/reviews remain.
