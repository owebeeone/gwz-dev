# Step 2.2 unpark the cli placement (gwz-core df31c221, gwz-cli 54a74ecb, gwz-py bde35e92) — STATE-AXIS REVIEW

**Review object:** gwz-core `df31c22176` (one commit on `9a9a659e`), gwz-cli `54a74ecb4e`, gwz-py `bde35e923e`, evidence run `gwz-core-evidence@ecaf00322f` `campaigns/transport-qualification/runs/2026-10-25-p2-2`.
**Baseline:** gwz-core `9a9a659e`, gwz-cli `9ecc2b7`, gwz-py `22e4f13`, gwz-transport `22b53dce5d` (reference only).
**Date:** 2026-10-25. **Axis:** State.
**Verdict:** **NO-GO.** Counts: P0 0, P1 0, P2 1, P3 3. I commit in advance to GO once the bounded remedy for P2-1 lands with its closure test. The P3s do not block.

## 0. Evidence base

- **Tuple.** Checked at start and at end; it did not move. Lane `/Volumes/projects/limbo/gwz-dev-p2-unpark` (root `a32ae33f60`). gwz-core `df31c22176`, gwz-cli `54a74ecb4e`, gwz-py `bde35e923e`, gwz-core-evidence `ecaf00322f`, gwz-transport `22b53dce5d`. All are clean, except one untracked directory in gwz-core-evidence (`runs/2026-09-22-alpha-setup-timeout/`), which is unrelated and older than this run.
- **Other members' HEADs.** git2-rs `d13951f7e0`, gwz-credential `6d78c593a4`, gwz-git `e3bc2a41f6`, gwz-sspi `a2978a8f3f`, libgit2 `b172e3d187`, taut `913e0dc35c`, taut-shape `89916c4d01`, taut-shape-py `da79bfb743`, taut-shape-rs `959f0f048a`.
- **Commands I ran.** The 1-minute load was between 27 and 42; cargo ran only once it fell below 40.
  - `python3.13 protocol/regen.py --check`: OK.
  - `CARGO_TARGET_DIR=…/p2-unpark-ordinary cargo test --test protocol`: **45 passed**, including `generated_protocol_is_current` and `ordinary_placement_and_attachment_fields_decode_when_the_writer_lacks_them`.
  - gwz-py `src/tests/test_codec.py` with the lane `.venv` and `PYTHONPATH=<lane>/gwz-py/src`: **12 passed**, including both gate tests.
- **Probes I ran** (scratchpad only; `gwz.taut.py` loaded through `GWZ_TRANSPORT_SCHEMA`):
  - the pinned export loads (179 messages);
  - a copy with one `Limits` tag changed is refused with `RuntimeError: … not the pinned one`;
  - a copy with one trailing newline added is refused;
  - **a CRLF copy of the unchanged export is refused** (this is P2-1).
- **Digest binding.** I rebuilt the evidence digest from the commit: HEAD `9a9a659e`, the M/D/R/T diff of `9a9a659e..df31c221`, and the sha of the one added file. It reproduces `bd496a9d…`, the value in `final.digests` and in both Windows `-b` digests. So the committed tree is the tree those runs used.
- **Artifact identity.**
  - `diff 9a9a659e:src/protocol/candidate_generated.rs src/protocol/generated.rs` is 36 lines, and all of it is the `TransportPlacement` block moving position.
  - `9a9a659e:protocol/candidate/corpus/golden.json` is identical to the new `protocol/corpus/golden.json` (179 entries).
  - Against the old ordinary corpus: 67 entries changed, all of them messages that carry `RequestMeta`, `ResponseMeta`, `TransportOptions` or capabilities. 24 owner entries were added and none were removed.
  - Old `src/cbor.rs` against `gwz-transport/src/cbor.rs`: 38 diff lines, all rustfmt layout and no semantic change.
- **Evidence read:** the README, `raw/red/*`, `raw/remote/final-r3/{summary.txt,results.tsv,candidate-test.log}`, `raw/windows/p2-2-20261025-b.log`, `receipt-b/receipt.json`, `raw/digests/*` and `manifest.json`.

## Problem, requirement provenance, and scope

- **What the step must do.** Plan §4 step 2.2: move the placement projection and the cli code out of `gwz_session_candidate`. The ordinary schema gains the placement-design §3 fields as an additive change. The §3 decoding gate must pass in Rust and Python first, followed by L2-02 regeneration and the `check_log_additive` pins.
- **Controlling text.**
  - Placement design §3: "Missing fields or an old core imply unsupported; the driver refuses locally instead of allowing an old decoder to ignore the placement."
  - 2.0 §3.1 and §3.6: the attachment slots are absent in the served form, and `endpoint_path_base` is sensitive metadata.
  - Plan §1, "Windows is a full peer".
- **Components and authorization.** gwz-core, gwz-cli and gwz-py are inside the brief. The refusal added in `with_transport` is outside the brief's file list, but it closes a hazard this step created, so it is a demonstrated necessity and is in scope. The move of gwz-transport to a plain dependency and the removal of `src/cbor.rs` follow from a deferred lane-owner decision; I judge their fail-closed properties only, not the decision itself.
- **Scope creep.** I found no unauthorized expansion that blocks delivery.

## 1. Findings

### P2-1 — The owner-schema pin hashes raw bytes, so every hosted Windows checkout fails the protocol gates

- **Location.** `gwz-core/protocol/gwz.taut.py` `_with_owner`: `hashlib.sha256(path.read_bytes())` against `_OWNER_SCHEMA_SHA256`. The file it reads is `gwz-transport/protocol/transport.ir.json` (2,402 lines), which has no `.gitattributes`. `git ls-files --eol` shows `attr/` empty for it. gwz-core's own `protocol/**` files are `text eol=lf`.
- **Violated invariant.** The pin must be durable on every supported checkout. A pinned input must fail closed only on a real change; a platform's line-ending conversion is not one. Windows is a full peer (plan §1).
- **Reproduction.**
  1. Hosted `windows-2022` sets `core.autocrlf=true` at system level. `.github/workflows/transport-candidate.yml:226` and `windows-matrix.yml:38` say so in comments.
  2. `.github/checkout-siblings.sh` checks out gwz-transport with `git checkout`, so `transport.ir.json` lands with CRLF line endings.
  3. `windows-matrix.yml` (every push and pull request) and `release.yml` `verify-windows` install `taut-proto==0.10.0` "so Windows keeps that signal", then run `scripts/run_tests.py`. Its "real" group runs `cargo test` over every target, `tests/protocol.rs` included.
  4. `generated_protocol_is_current`, `log_addition_preserves_the_complete_pre_log_wire_projection` and `log_addition_check_refuses_a_schema_that_declares_a_taut_option` each load `gwz.taut.py`, so `_with_owner` raises.
  5. The same happens in gwz-py: `package-smoke.yml` (windows-2022, 4 Python versions) runs `scripts/check_protocol_drift.py` and `regen_protocol.py --check`, both of which export gwz-core's `gwz.taut.py`. `publish.yml`'s Windows wheel job does the same.
  6. I reproduced the failure: loading `gwz.taut.py` against a CRLF copy of the unchanged pinned export gives `RuntimeError: the transport owner's schema export is not the pinned one`.
  7. The test harness itself already expects CRLF on Windows: `assert_same` calls `normalize_line_endings`. The candidate workflow avoids the problem only by running its generator checks on Linux, because "this image's CRLF checkouts of gwz-transport and taut do not" keep checked-in artifacts byte-exact.
- **Why the lane's evidence missed it.** The Windows receipt `p2-2-20261025-b` lists exactly these three rows as host gaps. The gaps are classified by test name, not by failure cause, and dabeest has no taut-proto. So the receipt cannot see a new cause of failure in those rows. The README's note that "the Windows-gap rows run on Linux and macOS" is true, but does not cover hosted Windows.
- **Impact.** Once pushed, gwz-core's Windows matrix and its release Windows verification go red. gwz-py's Windows smoke rows and Windows wheel publication fail. The wire itself is unaffected.
- **Correction (bounded).** Normalize line endings before hashing in `_with_owner`: `raw.replace(b"\r\n", b"\n")`. Then parse the normalized bytes. Optionally apply the same normalization in `tests/transport_consumer/protocol/regen.py`, which holds the other raw pin, so the two stay consistent. Alternatively, add `protocol/** text eol=lf` to gwz-transport, but gwz-transport is reference-only here and that change needs separate authorization.
- **Closure test.**
  - A Python or Rust test that loads `gwz.taut.py` with `GWZ_TRANSPORT_SCHEMA` pointing at a CRLF copy of the pinned export, and succeeds.
  - The same test pointing at a copy with one tag changed, and refused.
  - One Windows run with taut-proto installed and the default `autocrlf=true` checkout, with all three rows green. Hosted `windows-matrix` qualifies, and so does dabeest with taut-proto added. In that run, remove the three names from the receipt's gap list, or classify gaps by their failure text.

### P3-1 — gwz-py's Client sends an explicit `cli` placement without asking for capabilities first

- **Location.** `gwz-py/src/gwz/client.py` `_require_transport_capability`. It queries `transport_capabilities` only when an identity option is set; `transport.placement` is never consulted.
- **Violated invariant.** Placement design §3: "New drivers query capabilities before sending cli placement … the driver refuses locally instead of allowing an old decoder to ignore the placement."
- **Interleaving.**
  1. A caller passes `transport=TransportOptions(placement=TransportPlacement.cli, …)`. The type is public through `gwz.protocol`.
  2. The Client goes straight to `bridge.call`.
  3. If the `CoreBridge` (a public Protocol) fronts a core older than this step, that core's decoder ignores key 4. Old `TransportOptions::from_cbor` reads tags 1–3 only. `tests/transport_consumer/candidate/test_candidate.py` confirms that retained decoders file keys `{4, 5}` under `__unknown__`.
  4. The old core then runs the request locally on its own credentials.
- **Impact.** Latent. The only bridge that ships is `NativeCoreBridge`, built from the same core, and that core refuses the request (§2). The hazard comes back with any custom bridge, and the 3.2 `SocketCoreBridge` is that kind of bridge.
- **Correction.** When `transport.placement == cli`, require `cli` in `capabilities.placements` and a common `message_versions` before the call; otherwise raise `UnsupportedOperation` locally.
- **Closure test.** A fake bridge that returns an old two-key capabilities answer: the Client raises and the bridge records no call.

### P3-2 — The trait's default `with_transport` silently ignores `placement = cli`

- **Location.** `gwz-core/src/git/gitbackend/contract/methods_transport.rs`, the default `with_transport`. It uses `transport_support::identity::has_options`, which checks identities only.
- **Violated invariant.** The default's own contract says it "refuses explicit authority rather than silently ignoring it". It refuses an explicit identity but returns `Ok(None)` for an explicit `cli` placement.
- **Reproduction.**
  1. `gwz_core::git::GitRepository` (alias `GitBackend`) is public and unsealed.
  2. `gwz_core::workspace_ops::handle_clone_workspace_request` is public and needs only `B: GitBackend + Sync`.
  3. A non-Git2 backend given `placement = cli` (with no identity) therefore clones with that backend's own credentials.
  4. `FakeGitRepository` and `TrackingBackend` behave the same way.
- **Impact.** Defense in depth only. Every production backend is `Git2Backend`, which refuses.
- **Correction.** Make the default (or `has_options`) also refuse `placement == Some(Cli)` with `UnsupportedOperation`.
- **Closure test.** `FakeGitRepository::with_transport` given a `cli` placement is refused.

### P3-3 — The remote matrix `r3` is not bound to the final tree

- **Location.** `raw/remote/final-r3/summary.txt` and `environment.txt` record only the pre-commit HEADs (`9a9a659e`, `9ecc2b7`, `22e4f13`) and no worktree digest. The Windows `-b` runs record `bd496a9d` at booking and after the run.
- **Violated invariant.** Each gate's evidence should identify the exact tree it ran on.
- **Impact.** Nothing shows that `r3` ran on `bd496a9d`, the committed tree (r3 finished at 20:09Z; the `-b` booking was at 20:33Z). It also ran a scratch copy of `remote-gates.zsh`.
- **Correction.** Record `tools/digest.sh` output at the start and end of the remote run, or rerun `r3` against `df31c221`.
- **Closure test.** An `r3`-equivalent summary that carries `gwz-core full bd496a9d…`.

## 2. Invariant analysis (attacks that failed, with evidence)

**Mixed versions**
- *Old gwz-py (`22e4f13`) writing to this core.* Its maps lack keys 4–5, 10 and 9. The new decoder uses `try_get_opt` for MISSING_OK fields, so absent and null both read as `None` (`generated.rs`, `RequestMeta::from_cbor`). Required fields still use `try_get`.
- *This core writing to the old gwz-py.* taut's Python decode runs with `strict=True`, so a missing non-MISSING_OK key raises. Unknown keys go to `__unknown__` and `_from_wire_message` drops them, so old gwz-py decodes the new meta and capability maps.
- *This gwz-py writing to the old core.* The old Rust decoder reads tags by key and ignores unknown ones. That is safe for `local` and is the hazard behind P3-1 for `cli`.
- *Old core writing to this gwz-py.* An old two-key capabilities map reads its five extras as `None`, which means "no message placement support"; this is tested in both languages.
- *Rust gate.* `tests/protocol.rs` builds pre-step maps and null maps by hand, and rejects malformed values (text placement, value 3, integer path base, non-envelope tag 10, unknown placement in a list).
- *Python gate.* `test_codec.py` encodes with an older-schema copy, pins the shorter bytes exactly (`a3 01 … 03 f6`; `a2 01 f5 02 f5`) and rejects malformed values. It is pure Python, so the 23 candidate-extension skips do not touch it. The missing coverage is a cross-language row, listed in §3.
- *Byte pins.* The `MergeRequest` constant moves only by the header `a9 → aa` and the trailing `0a f6`, and the same constant is pinned in Rust and Python.

**What `check_log_additive` proves**
- It proves that removing exactly the 2.2 fields (with exact type, tag and MISSING_OK), `TransportPlacement` (`local=1, cli=2`) and the 42 owner declarations reproduces `4d377a49…`. I saw the red value `1001a0b8…` in `raw/red/check_log_additive-before.log`.
- It does not prove anything about owner content, because owner declarations are removed by name. The owner digest pin covers that.

**Retained readers**
- `retained-reader.json` pins `retained_old_generated.rs` (`5ba6d4ac…`), the checked-in pre-placement Rust decoder that is never regenerated.
- The retained Python decoder's fixtures show that old readers keep `{4, 5}` as unknown. They neither decode nor refuse them.

**Composition and pin (apart from P2-1)**
- A changed owner export fails closed when the schema loads, so regeneration and `generated_protocol_is_current` cannot silently produce a different wire. I probed this with a changed tag and with a whitespace change.
- `_with_owner` refuses any core/owner name collision.
- Rust has one `Envelope`: `generated.rs` re-exports `gwz_transport::protocol::*`.
- If a different gwz-transport checkout is built Rust-only, the corpus round-trip in `src/lib.rs` (`protocol_corpus`, which now carries `Envelope`, `Bind` and the other owner vectors) catches an encoding change at `cargo test --lib`.

**Removing `src/cbor.rs`**
- The two runtimes differ only in layout.
- The 112 corpus entries that are not 2.2 messages are byte-identical old to new.
- `generated_protocol_is_current` asserts that `src/cbor.rs` stays absent.

**The ordinary-core refusal**
- `Git2Backend::with_transport` refuses `cli` in the build without the transport switch. The arm is inside `cfg_if!`.
- I checked every non-test network call site. All nine handlers that reach `fetch`, `push_prepared`, `ls_remote(_url)` or `clone_repo*` (fetch, push, tag, init_from_sources, repo_lifecycle, pull_head plus its merge preflight, materialize, snapshot/pull_snapshot, clone_workspace) call `with_transport` immediately after `invocation_start` and `validate_transport_scope`. That is before identity file checks, network, or workspace mutation. `repo_sync` and `url_scheme_state` do no network.
- The error text names no path.
- The red-then-green test covers the ordinary arm (`raw/red/core-cli-refusal-before.log`). The same test exercises the transport arm with no host context.
- gwz-cli never sets `placement` (`..Default::default()`, `clirequest/invocation.rs:72`). gwz-py sets it only from what the caller supplies.

**Disclosure**
- `endpoint_path_base` is read only by `request.rs` `path_base` and validation, and is never formatted. No `Debug` formatting of meta, options or envelope exists outside tests.
- gwz-cli's `--json` is an allow-list (`response_meta_json.rs`).
- `MessageCatalog.md` lists field rows only, with no sample values.
- In the transport build, `validate_meta` refuses `transport_message` and a non-cli `endpoint_path_base`.

**Switch retirement**
- The checker's `RETIRED` arm is tested with a fixture expecting `RETIRED gwz_session_candidate  src/lib.rs  mod b`.
- `git grep` shows no remaining cfg site in core, cli or py; the only mentions left are comments and tests asserting the switch is absent.

**Gates and evidence**
- The red logs are as claimed: 23 `error` lines (the README counts 22 compile errors, which matches if one line is the summary), 2 Python failures, the pin mismatch, and the silent-accept test.
- `r3`: `run_tests` PASS 2,384; the transport leg had 3,834 passed, 1 failed. That failure is `agent_auth::native_network_wait_retries…` at `agent_fixture.rs:292`, the disposal-ack timing row, which passed in the first full leg. I accept it as load-related; it is not code this step changed.

**Platform rules**
- The added conditional code is all inside `cfg_if!` arms, and the Windows policy arm sits in a `cfg_if!` block.
- No new globals or thread-locals were added.

## 3. Risks and next action

**Next action**
- Land the P2-1 remedy and its closure test, plus one Windows run with taut-proto on a CRLF checkout. Then GO, with P3-1 to P3-3 carried as tracked items.

**Observations (not findings)**
- *Shape-divergent validation.* An ordinary build accepts a present `RequestMeta.transport_message` and a `local` request carrying `endpoint_path_base`, and ignores both. The transport build refuses them, but only on network requests (`validate_meta`). There is no served form yet, so 2.0 §3.1's `invalid_request` rule belongs to 2.2b. It is cheap to make this uniform then.
- *gwz-py `--json`.* It dumps whole dataclasses (`cli_render.py:79` → `asdict`), so every response meta now carries `"transport_message": null`. If that slot were ever filled, the `Envelope` (including `Bound` schemes and policies) would be echoed, and its `bytes` would raise `TypeError`. Keep the slot absent (2.0 §3.1), or give that renderer an allow-list.
- *Coverage.* No row sends `placement = cli` through `NativeCoreBridge` into the native refusal, in either shape. The backend unit test plus the call-site inspection above carry this step. A bridge-level row would also guard P3-1's future fix.
- *Python constructors.* The generated dataclasses have no defaults, so external Python constructions of `TransportOptions`, `RequestMeta`, `ResponseMeta` and `TransportCapabilitiesResponse` without the new keyword arguments now raise `TypeError`. This is a source break, not a wire break; the implementer has already filed it as a release-note item.
- *Public names.* `gwz_core::ErrorCode` (and `Data`, `Open`, `Close`, `Window` and the rest) now resolve to gwz-transport's owner types through `pub use protocol::generated::*`, beside `gwz_core::model::ErrorCode`. This is for the Surface review.
- *Stale CI comments.* The `release.yml` and `windows-matrix.yml` comments that say gwz-transport is behind `gwz_transport_candidate` are now stale.
