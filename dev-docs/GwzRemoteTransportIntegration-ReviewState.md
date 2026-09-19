# Shared-schema transport integration — State-AXIS REVIEW

**Review object:** Shared-schema integration checkpoint at workspace `5559184c118d937cff01c57fb202cb4affb92b91`, comprising taut `7a5f616c3a9f72e143b6e20dab41ffa6e20e240a..3b8436508ecc6b74b5efa0f1e642d0698b708bf7`, gwz-core `e21250ce0d6e9b5fce02154595db5ba2e451ae4c..3b4b632dfba5b0f2f154f50b45d9056a21a9ce99`, the workspace integration checkpoint, and the draft `gwz-core/dev-docs/GwzRemoteTransportPool-InterfaceGate.md`; gwz-transport remains unchanged at accepted revision `e8b9a1c5408cc9ea9528939b3a602acbeb697814`. This is an implementation checkpoint, not a Phase 1/2 interface freeze.  
**Baseline:** workspace `5559184c118d937cff01c57fb202cb4affb92b91`; taut `3b8436508ecc6b74b5efa0f1e642d0698b708bf7`; gwz-core `3b4b632dfba5b0f2f154f50b45d9056a21a9ce99`; gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`. Committed sources were read using `git show PIN:path` and the two pinned implementation diffs.  
**Date:** 2026-09-19  
**Axis:** State — state machines, ownership, failure and recovery, filesystem/provenance ordering, bounds and fail-closed behavior. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — zero P0, zero P1 and three P2 findings block this checkpoint. I pre-commit to GO on a revision that resolves P2-1 through P2-3 as specified.

---

## 0. Evidence base

The exact tuple was verified at the beginning and end of the review. Both checks returned:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `5559184c118d937cff01c57fb202cb4affb92b91` | `ca81a514178868abec681044dbe018b441d1bf84` |
| taut | `3b8436508ecc6b74b5efa0f1e642d0698b708bf7` | `4349138d3516971d9e4778b0546ac01a0dcfab29` |
| gwz-core | `3b4b632dfba5b0f2f154f50b45d9056a21a9ce99` | `d276027c5b6f9e034de79b8109ae876872e112d8` |
| gwz-transport | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` | `6aedc6f4e2de3e228b03d4a66e792f94183ec429` |

All member repositories remained clean. Root contained only the two authorized untracked review prompts. The Code prompt and report were not read.

The review read:

- Root and member agent instructions, `AgentProcessRules.md`, `GwzProcessOptimization.md`, `CurrentProgramCheckpoint.md`, and `GwzRemoteTransportIntegration-Checkpoint.md`.
- The complete taut correction diff, including `rust_external.py`, Rust scaffold/CLI integration, documentation and regression tests.
- The complete core consumer: schema composition, generation pins, regeneration code/tests, package/archive proof code/tests, checked generated module, manifest/lockfile, typed tests and encoded bidirectional exchange.
- The complete draft pool/runtime interface gate and the applicable design, requirements and Phase 1/2 plan sections.
- Unchanged transport source only at the integration seams: owner schema/package exports, CBOR/type ownership, pool clock initialization, final-`Pool` ownership and the existing focused regressions.

Targeted verification produced:

- The combined taut and consumer-tooling suite passed all 35 tests.
- Explicit consumer regeneration with the pinned owner schema and taut checkout reported `consumer regen verified`.
- Consumer formatting passed.
- The archive digest was exactly `24c9d7a839b1a23ae1f188541ac87092550dcae99bd9cf6e14df4c900b1a7dd9`; its package file inventory included the exported schema and complete transport sources.
- The isolated archive proof verified source revision `e8b9a1c5408cc9ea9528939b3a602acbeb697814`, package `gwz-transport` version `0.1.0`, then passed all six offline consumer tests: four identity/admission tests and the typed and encoded bidirectional exchanges.
- Both implementation diffs passed `git diff --check`.
- A focused provenance counterexample called `_verify_taut_source` with `/Users/owebeeone/limbo/gwz-dev/taut/docs`; it returned successfully and printed `verification accepted non-source directory`, establishing P2-1 without modifying the reviewed trees.

The lane owner’s unchanged-transport Rust 1.95 suite and previous accepted campaigns were corroborating evidence. No large campaign was repeated.

## 1. Findings

### [P2-1] Regeneration verifies one directory but can import a different taut installation

**Location.** `tests/transport_consumer/protocol/regen.py:82-110` derives the repository as `path.parent`, verifies that repository’s HEAD, checks dirt under its fixed `src` path, and hashes three files under that fixed path. It never requires the supplied `--taut-source` to be that repository’s `src` directory. `_generate` then adds the unverified supplied directory to `sys.path` and performs ordinary imports at `:113-127`. The checkpoint claims that the supplied generator source and revision are pinned (`GwzRemoteTransportIntegration-Checkpoint.md:20-25`; `GwzRemoteTransportPool-InterfaceGate.md:23-27`).

**Violated invariant.** The code whose revision and hashes are verified must be the code Python imports and executes. A provenance check cannot validate checkout A while module lookup falls through to a cached, installed or alternate source B.

**Reproduction.**

1. Use the exact reviewed taut checkout and generator pin.
2. Supply `taut/docs` as the taut source. It is a real directory whose parent is the pinned taut repository.
3. `_verify_taut_source` checks the parent repository’s correct HEAD, clean `src` tree and official extension hashes, then returns success. This was executed at the reviewed tuple.
4. `_generate` inserts `taut/docs`, which contains no `taut` package. Python may therefore use an already loaded module or fall through to site-packages. The configured regeneration environment already contains a separate taut 0.9.1 installation, demonstrating that lookup route.
5. The subsequent version comparison checks only `0.9.1`; it does not prove the imported module files came from the verified checkout. A same-version installation containing the external-type extension can therefore run while the command reports the workspace revision as its source.

The checked-output comparison limits silent artifact drift for this one file, but it does not restore the claimed source provenance and does not prevent execution of unverified generator code.

**Impact.** A regeneration or CI record can attribute output to the pinned taut commit when another same-version implementation actually ran. That defeats the exact-source gate used to establish reproducibility and package ownership, and it makes a later mismatch hard to diagnose.

**Required correction.** Resolve the Git top level explicitly, require `--taut-source` to equal `<verified-root>/src`, and verify the resolved `__file__` locations of `taut`, `taut.gen.scaffold` and `taut.gen.rust_external` are beneath that exact directory. Run generation in a fresh interpreter or reject preloaded modules so `sys.modules` cannot bypass the path check.

**Closure test.** Add cases that supply another directory under the correct repository and that preload or install a same-version alternate taut module. Both must refuse before generation. The exact pinned `taut/src` case must continue to reproduce the checked artifact.

### [P2-2] The draft host contract omits required pool-clock initialization

**Location.** The draft describes `advance(now_ms) / next_deadline()` only as driving a monotonic clock and rearming after mutations (`GwzRemoteTransportPool-InterfaceGate.md:60-69`). The actual accepted contract requires the host to initialize the pool clock before the first checkout and states that `next_deadline` is a snapshot rather than a timer subscription (`gwz-transport/README.md:131-136`). `PoolMachine` begins at time zero (`src/pool/machine.rs:76-107`), request and connect deadlines are calculated from that value, and `advance` expires them when the supplied absolute value reaches those deadlines (`src/pool/clock.rs:3-49`).

**Violated invariant.** Every deadline must use one established monotonic origin. The host contract must prevent the pool from creating deadlines relative to zero and later comparing them with an unrelated nonzero clock epoch.

**Reproduction.**

1. Construct the pool; its internal `now` is zero.
2. Submit a checkout before calling `advance`. Its allocation deadline becomes 30,000, and dispatching Connect establishes the default network deadline at 10,000.
3. Let the host’s first clock update use a valid monotonic value such as 1,000,000 milliseconds from its process or system origin.
4. `advance(1_000_000)` immediately reports `ConnectTimeout` or `AllocationTimeout`, although essentially no budget elapsed after the request.
5. The failure starts connector cancellation/cleanup and can discard an otherwise valid connection.

The same draft row also does not say that `next_deadline` is only a snapshot and that a pending `next_action` future is not a timer service. “Re-arm after every pool mutation” alone is insufficient for mutations performed through independently held Pool/Checkout/Lease objects unless the host uses the accepted periodic-tick alternative.

**Impact.** A host built against the draft can produce deterministic false timeouts at startup and can miss newly earlier deadlines while its action receiver remains pending. That changes allocation and cleanup state rather than merely reducing timing precision.

**Required correction.** State that the host must call `advance` with its chosen monotonic origin before the first checkout; all later values use that same origin. State that `next_deadline` is a snapshot, `next_action` does not supply timer service, and the host must either tick periodically or re-query after every mutation it controls while retaining a bounded fallback for independent mutations.

**Closure test.** Add a focused host-contract case starting at a large nonzero clock: initialize first, create and dispatch a request, then prove it remains live until the configured elapsed budget. Also demonstrate that a newly introduced earlier deadline is serviced while the action receiver is otherwise pending.

### [P2-3] The draft ownership contract omits automatic shutdown on final `Pool` drop

**Location.** The draft says Pool clones share one endpoint pool and documents Checkout and Lease drop behavior, but it does not state what dropping the final Pool clone does (`GwzRemoteTransportPool-InterfaceGate.md:30-40`). The implementation explicitly defines the final Pool as the shutdown owner (`src/pool/asynchronous.rs:49-50,119-132`); `PoolDriver`, Checkout and Lease hold the shared allocation but do not increment the Pool-owner counter. The existing regression is `tests/pool_async.rs:94-126`.

**Violated invariant.** The host contract must identify the object whose lifetime controls endpoint admission and active leases. Retaining a driver, checkout or lease must not be mistaken for retaining the endpoint Pool.

**Reproduction.**

1. Construct `(Pool, PoolDriver)`, clone the Pool, connect successfully and take a live Lease.
2. Retain the driver and lease, but drop both Pool handles.
3. The second drop calls `shutdown`.
4. The live lease immediately becomes stale, reusable release fails, and the driver receives a Close action.
5. A host following the draft can therefore terminate an active exchange merely by allowing its last Pool handle to leave scope, even though the driver and lease remain alive.

**Impact.** Missing one ownership sentence can make endpoint integration unexpectedly stop admission, invalidate leases and begin physical cleanup during active work. This is a lifecycle-shape defect in the document intended to guide the later host implementation.

**Required correction.** State that the host must retain at least one Pool clone for the intended endpoint lifetime; dropping the final Pool clone is equivalent to initiating shutdown even when drivers, checkouts or leases remain. Distinguish that transition from PoolDriver loss, which has its own stronger host-disposal obligation.

**Closure test.** Cite and retain `dropping_last_pool_owner_shuts_down_even_with_a_live_lease` in the focused gate table, and add a host-lifetime fixture or contract assertion showing that one retained Pool clone preserves the lease while the final clone’s drop initiates cleanup.

## 2. Invariant analysis

The following attacks held:

- **Native type identity:** Generated consumer code re-exports every owner enum and message and defines only the consumer wrapper. Its field is the native `gwz_transport::protocol::Envelope`; no conversion or duplicate transport vocabulary exists.
- **Shared codec runtime:** The consumer re-exports `gwz_transport::cbor`, so owner methods and wrapper methods exchange the same `Cbor` and `DecodeError` types. The isolated compilation and round trip prove the identity boundary.
- **Schema/package binding:** The ordinary path pins package version `=0.1.0`; explicit regeneration checks the owner manifest identity and exact schema digest. The archive proof separately binds the supplied archive digest, package identity, Cargo VCS revision and clean-source metadata.
- **Archive recovery grammar:** Duplicate names, path traversal, absolute/Windows-style paths, links and special files refuse before extraction. Extraction uses exclusive creation into a temporary root, and Cargo runs offline against the verified path patch.
- **Typed and encoded handoff:** Both directions preserve binary bytes and ordering. The encoded fixture uses the owner’s limited codec for the inner envelope, and endpoint cleanup completes before graceful close reports reusable disposition.
- **Bounds claims:** Oversized typed Data is refused by owner admission. The documents accurately limit the outer-wrapper test to a trusted fixture and retain pre-allocation, aggregate-budget and closure-notification duties for the supplied communication layer.
- **Fail-closed generation:** Invalid external names and injected Rust paths refuse before output; the generated wrapper returns typed decode errors. Unknown bounded fields survive or are ignored according to the accepted owner/consumer codecs.
- **Package independence:** The isolated archive test has no sibling checkout, schema fetch or network access. Normal Cargo compilation uses checked output and does not invoke Python.
- **Scope containment:** No production core/CLI method, optional field, carrier framing, physical adapter, credential behavior or transport source changed. The checkpoint and current-state document preserve the later Phase 1/2 freeze and Surface-review gates.
- **Filesystem failure direction:** Interrupted generation can lose or truncate developer output, but the checked artifact comparison and Git state expose that condition; it cannot silently create an accepted transport state. Temporary extraction/build state has no product ownership role.

## 3. Risks and next action

Physical carriers and adapters, native-platform qualification, publication, production dispatcher wiring, complete encoded-contract matrices, CI drift wiring and active-I/O clock semantics remain explicitly deferred. The focused consumer proves payload ownership and two exchange paths; it does not establish outer communication-layer admission or a Phase 1/2 freeze.

The next action is one bounded remediation: bind regeneration verification to the exact imported taut source, complete the draft clock-origin/timer duty, and record final-Pool-drop ownership. Rerun the targeted generator, regeneration and isolated consumer checks, then return the corrected immutable tuple for focused State re-verdict alongside the independent Code-axis disposition.
