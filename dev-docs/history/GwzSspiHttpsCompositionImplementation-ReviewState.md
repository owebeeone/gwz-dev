# GwzSspiHttpsCompositionImplementation — State-AXIS REVIEW

**Review object:** Cohesive HTTPS SSPI step4b implementation ranges and root caller guide/checkpoint at the exact tuple below. Controlling contract: `dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md`, accepted design despite its historical filename; implementation acceptance pending, 2026-10-04.

**Baseline:**

| Repository | Reviewed HEAD | Implementation-range baseline |
|---|---|---|
| root | `b72dccf813816f41a508eb0fc2f9b2f071b323a0` | Root caller guide and implementation checkpoint |
| gwz-core | `efdd0a2cf66be889364466c7d69c97cc2736c278` | `56f56a3d5ea3c9f8a50ec9e4c42453c1b92d9c79` |
| gwz-sspi | `58cc87c99bca21874a63d0ba11209b75a3e0c50a` | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |

Sources were read through `git show`, range diffs, and numbered working files. All six HEADs matched at both start and end; final `git diff --stat HEAD` was empty in every repository.

**Date:** 2026-10-04

**Axis:** State machines, interruption, races, retained cleanup ownership, capacity and fail-closed reporting. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block; one additional P3 finding corrects the evidence attribution. I pre-commit to GO on a revision that resolves P2-1, P2-2 and P3-1 as specified, preserves the accepted boundaries, and passes their closure checks.

---

## 0. Evidence base

### Authority and scope inspected

- Root `AGENTS.md`, `AGENTS_GWZ.md`, `EVIDENCE.md`; member instructions for core, SSPI, transport and CLI.
- `dev-docs/AgentProcessRules.md`, particularly L1-13 through L1-20; `GwzProcessOptimization.md`, including the review and evidence amendments.
- `GwzSspiHttpsCompositionDesign-DRAFT.md` §§1–9, `GwzSspiHttpsCompositionAcceptance.md`, and `GwzSspiHttpsCompositionBudgetDisposition.md`, including its resume and administrative-ledger dispositions.
- `CurrentProgramCheckpoint.md` top live entry, lines 3–47; `GwzSspiHttpsCompositionCheckpoint.md`, especially cleanup ownership lines 49–57, matrix lines 136–152 and gate disclosures lines 154–168.
- Core `GWZDesign.md` and `GWZRequirements.md` HTTPS composition amendments; `GwzTransportWindowsParityDesign.md` §7; `GwzTransportCredentialHelpersDesign.md` §§4–7.
- Root `GwzSspiDesign.md` §§5–6. The prompt’s member-local SSPI design path does not exist; the controlling documents link to this root document.
- Root implementation caller guide; SSPI `docs/Supervision.md`; core `docs/TransportPlacement.md` lines 335–363.

### Production and test sources inspected

- Core `https_worker/native.rs` lines 1–995 and production bridge tests, particularly lines 1001–1118, 1170–1345 and the source/redirect/wipe rows.
- Core `https_worker/prepare.rs` lines 1–565, `credentials.rs` lines 1–134, `budget.rs` lines 1–75, `serve.rs` lines 1–274.
- Core `https_worker.rs` lines 43–241, `https_operation.rs` lines 1–105, `https_policy.rs` lines 69–191, and final-origin CBT capture in `https_connection.rs` lines 445–477.
- Core `https_auth/secret.rs` lines 1–193.
- Core `transport_host/https_endpoint.rs`, especially runtime reaping lines 117–135 and cleanup observers lines 314–343; endpoint `poll.rs` lines 17–168; session retirement in `session/driver/pump.rs` lines 299–344; capacity admission in `session/capacity.rs` lines 130–190.
- Core request opening, platform policy selector, local-command and cancellable handoff paths.
- SSPI `supervisor/api.rs` lines 24–180 and 215–261; `futures.rs` lines 33–139 and 281–330; `context.rs` cleanup status and reaping; `values.rs` cleanup receipts; existing registered-Start drop test at `lifecycle_tests.rs` lines 150–177.
- CLI dispatch lines 11–35; Python `ClientHost::network` lines 159–193 and route capture/attach/run ownership.
- Transport policy, binding/profile filtering, mux native offers and routing validation; codec native facts validation lines 127–143 and 243–280.
- Baseline-to-HEAD diff of core `https_worker/serve.rs`, independently establishing that its native usability guard is newly added.

### Commands actually run

Both permitted targeted commands completed successfully:

```text
CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/sspi cargo +1.95.0 test --manifest-path gwz-sspi/Cargo.toml --locked --offline captured_
```

Result: exit 0; three matching unit tests passed.

```text
RUSTFLAGS='--cfg gwz_transport_candidate' CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-target cargo +1.95.0 test --manifest-path /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-prepared/Cargo.toml --lib --locked --offline git::endpoint::https_worker::native::tests
```

Result: exit 0; 22 matching tests passed in 4.96 seconds. The prepared `src` resolves to the reviewed core source directory; its SSPI and transport paths reference the reviewed workspace members.

Read-only Python inspection parsed the saved strict-Clippy diagnostics at:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-clippy-target/debug/.fingerprint/gwz-core-b001b55e91425256/output-lib-gwz_core
```

Result: 47 coded error diagnostics plus the compiler’s final “aborting due to 47 previous errors” message. One diagnostic specifically identifies the newly added native guard at `https_worker/serve.rs:119–124`.

The final provisioned Darwin artifact directory contains the wheel, extracted extension, dedicated worker and artifact-set manifest. The manifest records Rust 1.95.0, candidate rustflags and the declared Darwin target. I did not independently load the wheel or rerun packaging, CLI, full member suites, generation, formatting or Clippy; those executions remain owner testimony. No network, credentials, source edits or Git mutations were used.

## 1. Findings

### [P2-1] Dropping a pending Start releases core cleanup charges before the worker is accounted for

**Location:** Core `src/git/endpoint/https_worker/native.rs:776–797`, with `Guard::drop` at lines 565–574 and the real adapter at lines 397–420. SSPI registered-Start drop behavior is at `src/supervisor/futures.rs:127–135`.

**Violated invariant:** Accepted composition §8 requires the operation dependency/retirement record to survive pending native cleanup. The implementation also promises retained endpoint request capacity until cleanup is accounted for.

The bridge installs ownership for a session and for Finish, but does not install ownership for Start. While awaiting `port.start`, `guard.session` and `guard.finishing` are both `None`. The Start future exists only as a local future.

**State sequence:**

1. A native Open obtains its endpoint slot and operation dependencies.
2. The real captured Start is polled, registers an SSPI record and queues or launches worker ownership, but has not returned a Conversation.
3. Drop or abort the enclosing preparation future at this suspension point.
4. SSPI `Start::drop` cancels the registered record; SSPI supervision correctly retains its worker capacity while cleanup remains pending.
5. Core `Guard::drop` has neither session nor Finish to transfer into `native_cleanup`. Its operation dependency and endpoint slot are consequently dropped.
6. After the physical lease finishes disposal, the core cleanup observer has no native entry representing the still-pending worker.

Awaiting Start after an ordinary cancellation signal covers that cooperative path. It does not cover destruction of the enclosing future—the same adversity explicitly exercised for Finish.

**Impact:** Core can report no pending native work and retire operation ownership while SSPI still owns a worker. Endpoint request capacity is released without the required cleanup proof. SSPI’s independent worker bound survives, so this is a composition/accounting defect, not a claim of an orphaned native process.

**Required correction:** Install an owned pending-Start holder before the first await, analogous to the retained Finish holder. On enclosing destruction, cancel publication and transfer that holder, the operation dependency and endpoint slot into retained endpoint cleanup. A subsequently returned Conversation must be cancelled/disposed without publication. Pre-registration refusal may release charges once its no-resource outcome is known.

**Closure test:** Through the actual preparation bridge, suspend a Start after registration and before Hello/Conversation return, then abort the preparation task. Assert pending cleanup remains visible, only 63 request permits are available, and sealing the operation prevents reacquisition. Supply Pending and Unknown observations, then confirmed disposal; only the last may release both charges. Include a separate pre-registration-drop case.

### [P2-2] Concurrent reaping temporarily hides pending cleanup from retirement observers

**Location:** Core `src/git/endpoint/https_worker/native.rs:467–478`; consumers at `https_worker.rs:220–229`, `transport_host/https_endpoint.rs:334–343`, and `session/driver/pump.rs:304–343`.

**Violated invariant:** Moving cleanup work outside a lock must not make outstanding work disappear from cleanup reporting or authorize retirement before proof.

`reap` takes the entire shared vector, checks its records outside the lock, and later returns unconfirmed records. Its returned count covers only the shared vector. There is no count for records temporarily held by another reaper.

**Interleaving:**

1. One native cleanup entry remains unconfirmed; its physical connection is already disposed and its endpoint stream entry has retired.
2. Reaper A, running on the HTTPS runtime, takes that entry out of the shared vector.
3. Pause A before or during its proof check.
4. The session thread calls `pending_request_count` or `pending`. Its reaper B sees an empty vector and returns zero.
5. The session retirement path can save `pending_local_work = 0`; capacity admission can likewise observe no pending endpoint work.
6. A subsequently returns the still-unconfirmed entry. The earlier zero was not a cleanup proof.

This is a production interleaving: the HTTPS runtime periodically invokes `reap_cleanup`, while session-side cleanup and capacity observers independently invoke `pending_cleanup`.

**Impact:** Cleanup reports can falsely claim no local work remains. Retirement and capacity replacement checks can proceed while retained native work is still active. The detached entry still owns its charges; preserving those owners alone does not preserve truthful observation.

**Required correction:** Keep an authoritative outstanding count that includes entries claimed by reapers, or use another scheme that preserves conservative visibility throughout the take/check/return interval. Polling, proof checks and final owner drops must remain outside the shared state lock.

**Closure test:** Pause reaper A immediately after claiming an unconfirmed entry. Invoke the real cleanup-count and retirement/capacity observers concurrently. Every observer must report outstanding work or conservatively refuse retirement. Resume A with Pending and Unknown, then Confirmed; zero becomes legal only after confirmed removal. Also cover insertion of a second entry while A holds the first.

### [P3-1] The RED47 receipt incorrectly attributes a new native-bridge diagnostic to the baseline

**Location:** Root `dev-docs/GwzSspiHttpsCompositionCheckpoint.md:158` and `CurrentProgramCheckpoint.md:29–31`; new core code at `src/git/endpoint/https_worker/serve.rs:119–124`.

**Violated invariant:** Evidence attribution must distinguish existing debt from diagnostics introduced by the reviewed implementation.

The saved strict-Clippy output contains:

```text
clippy::collapsible_if
src/git/endpoint/https_worker/serve.rs:119–124
```

Its rendered source is the nested `native_route`/`auth.usable` guard. The implementation-range diff adds that entire guard; the baseline `Prepared::run` begins directly with response handling.

**Reproduction:** Parse the saved diagnostic JSON and select the primary span starting at `serve.rs:119`; compare that source with `git show 56f56a3d…:src/git/endpoint/https_worker/serve.rs` or the reviewed range diff. The diagnostic is attributable to new composition code.

**Impact:** The receipt’s “pre-existing” and “new … bridge diagnostics zero” claims cannot support the changed-code strict gate. Full strict core Clippy is correctly disclosed as RED, but its baseline attribution is inaccurate.

**Required correction:** Resolve the new guard diagnostic while preserving braced control flow, refresh the relevant strict gate evidence, and correct both receipt/checkpoint attribution statements. Keep any remaining full-core RED explicit.

**Closure check:** Inspect the refreshed diagnostic output and compare remaining findings against the range baseline. The new guard diagnostic must be absent; documentation must accurately distinguish remaining baseline debt and the changed-code result. No broad runtime regression is needed for this evidence correction.

## 2. Invariant analysis

- **Fixed deadline:** The native policy anchors checked `D` after admission and before checkout. Redirect loops reuse the same Budget; native Start receives that exact instant. Checkout, helper work, HTTP waits, steps and Finish observe it. Zero refuses native selection before Begin. The permitted tests passed the pool-wait, zero, helper-crossing, round-cap and independent HTTP allowance cases.
- **Original caller and capacity:** CLI captures before invoking the transport action; Python captures before route detachment and operation registration. Captured Starts retain the issuing context and original origin reference, without late recapture or a new worker-capacity domain. Foreign-context and closed-admission targeted tests passed.
- **Source selection:** Challenge-dependent helpers-first selection matches WindowsParity §7. Missing/unusable helper identity permits current logon; timeout, cancellation and cleanup uncertainty remain terminal. Negotiate-only and WindowsDefault avoid helper lookup. Configured Basic remains viable without native availability.
- **Physical scope and CBT:** CBT is obtained from the concrete verified final-origin TLS stream before erasure, and the returned dependency buffer is wiped after copying. Native publication binds an opaque scope to one physical generation; replacement is refused before POST. Native redirects fail; configured Basic follows validated discovery without forwarding the old header.
- **Authentication facts:** Mechanism selection and authority remain independent of native Complete. History rejects mechanism switches and regression to unresolved state. Remote 200 cannot establish authenticated success before Complete, offered credentials and authoritative selection. Producer and codec validation preserve that distinction.
- **Finish ownership:** The held Finish future is installed before await, survives enclosing-task abort, and is not repolled after completion. The executed Finish cancellation/expiry/abort tests passed. P2-1 identifies the corresponding missing Start ownership.
- **Cleanup proof:** NativeProbe accepts only Confirmed. Unknown, including tombstone eviction, never releases charges. That fail-closed rule holds; P2-2 concerns the observer losing visibility while another reaper holds those charges.
- **Secrets:** New native challenge and Authorization owners allocate initialized fixed storage before copying or encoding and wipe on fallible exits and Drop. The live wipe test passed. Hyper/HeaderValue, native-TLS and provider copies remain explicitly outside that guarantee.
- **Effects and retries:** Receive-pack sets Possible before POST send; post-byte failures do not enter authentication replay. The outer checkout deadline conservatively avoids invented fresh-connect provenance, while actual pool-returned setup failures retain their classifier.
- **Evidence and activation:** The 22 executed bridge tests exercise production orchestration rather than establishing native Windows authentication. Their fake Start completes immediately, and their cleanup observations are serial, explaining the two uncovered boundaries. Windows activation remains guarded; local provisioned artifacts do not qualify the deferred platform/release matrix. RED47 remains RED, with its attribution correction required by P3-1.

No new durable filesystem format or restart-adoption rule was introduced in the reviewed range. This review does not extend its verdict to unrelated workspace recovery code.

## 3. Risks and next action

The documented tombstone-eviction behavior can leave a core receipt permanently charged if Confirmed was never observed. It remains fail-closed and is explicitly disclosed; this review does not authorize treating Unknown as disposal proof.

Native Windows caller identity, provider completion, EPA/CBT algorithms, physical cleanup and installed-host qualification remain deferred. Packaging and broader gate executions were not independently repeated under this prompt.

The next action is one bounded remediation of P2-1 and P2-2, with their production-bridge interleaving tests, plus P3-1’s diagnostic and evidence correction, followed by a settled-tuple State re-review. Implementation acceptance remains pending.
