# GwzSspiHttpsCompositionImplementation — Code-AXIS REVIEW

**Review object:** Cohesive HTTPS SSPI step4b implementation ranges and root caller guide/checkpoint at the exact tuple below. Implementation acceptance pending. Controlling contract: `dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md`, accepted despite its historical filename.

**Baseline:**

| Repository | Reviewed HEAD | Implementation range base |
|---|---|---|
| root | `b72dccf813816f41a508eb0fc2f9b2f071b323a0` | Root caller guide and implementation checkpoint |
| gwz-core | `efdd0a2cf66be889364466c7d69c97cc2736c278` | `56f56a3d5ea3c9f8a50ec9e4c42453c1b92d9c79` |
| gwz-sspi | `58cc87c99bca21874a63d0ba11209b75a3e0c50a` | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |

Sources were read using `nl`, `sed`, `rg`, read-only Python inspection, and baseline-to-HEAD `git diff`/`git show`. Tracked implementation files were unchanged in the inspected working trees. All six HEADs matched at both the start and end.

**Date:** 2026-10-04

**Axis:** Code — architecture, interfaces, call graphs, ownership, error projection and compatibility. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — four P2 findings block acceptance; one P3 finding corrects the gate evidence. No P0 or P1 finding. I pre-commit to GO on a revision that resolves P2-1 through P2-4 and P3-1 as specified.

---

## 0. Evidence base

Authority inspected:

- Root and member AGENTS instructions, including `AGENTS_GWZ.md`.
- `AgentProcessRules.md` and its controlling `GwzProcessOptimization.md` amendment.
- `CurrentProgramCheckpoint.md`, top live entry only.
- Composition Design §§1–9, Acceptance, BudgetDisposition, implementation Checkpoint and caller guide.
- Core `GWZDesign.md` and `GWZRequirements.md`, relevant composition, transport and observation clauses.
- Core WindowsParityDesign §7 and CredentialHelpersDesign §4, including helper absence versus terminal timeout/cancellation.
- Root `GwzSspiDesign.md` §6. The prompt’s member-local design path does not exist; the member documentation references this root design.
- SSPI Architecture, Testing, Supervision and CallerValues documentation; core TransportPlacement and changed member/API documentation.

Principal source inspected:

| Area | Inspected sources |
|---|---|
| Native bridge | Core `https_worker/native.rs:1–1554`, including parser, history, real NativePort, Guard, retained Finish, generation owner, authentication and regression tests |
| Preparation and serving | Core `https_worker/prepare.rs:1–560`; `serve.rs:1–282`; `budget.rs:1–75`; credentials and worker changes |
| TLS and routing | Core `https_connection.rs:438–477`; `https_destination.rs:1–115`; `https_policy.rs:45–190`; `https_operation.rs:1–103` |
| Host graph and cleanup reporting | Core transport-host changes; `https_endpoint.rs:1–415`; endpoint poll/retry paths; session requests and driver pump/opening paths |
| Caller capture | SSPI API, futures, ports and remediation diff; `futures.rs:33–139`; Windows origin adapter; compiled capture regression sources |
| Protocol | Transport taut changes, generated projections, policy, binding, codec validation and mux routing/begin changes |
| Original entries | CLI `dispatch.rs:11–36`; Python `client_host.rs:32–193` and route capture/run changes |
| Error consumers | Core `request/https_failure.rs:21–152`; `https_remote.rs:194–227`; gitbackend `transport.rs:735–763`; materialize `apply.rs:51–91` |

I also inspected the installed `native-tls 0.2.18` dependency source. Its macOS, OpenSSL and Schannel implementations return the certificate digest without the `tls-server-end-point:` prefix.

Executed independently, using exactly the permitted external-target commands:

1. SSPI `captured_` filter: exit 0; **3 passed**, no failures.
2. Core `git::endpoint::https_worker::native::tests` filter through the supplied external prepared manifest: exit 0; **22 passed**, no failures, 2.67 seconds.

These passes establish their tested paths, not the counterexamples below. I did not write additional tests or execute the full build, Clippy, packaging or Windows matrix.

For the strict-core lint claim, I inspected the retained JSON diagnostics at:

`/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-clippy-target/debug/.fingerprint/gwz-core-b001b55e91425256/output-lib-gwz_core`

It contains 47 coded error diagnostics. Their primary spans were compared against baseline source using read-only Python and `git show`.

The final provisioned Python wheel, extension and worker exist at the checkpoint’s stated external paths. I inspected the relevant packaging and ClientHost test sources. The disclosed host tests exercise installed descriptor selection, ordinary HTTPS helper/proxy refusal, and overlapping SSH operations; they do not cross the real native HTTPS request validator.

No current peer report was opened. No source, Git state, formatting, generation or installation was changed.

## 1. Findings

### [P2-1] Core supplies a raw certificate digest where SSPI requires a prefixed channel binding

**Location:** Core [https_connection.rs:447](/Volumes/projects/limbo/gwz-dev/gwz-core/src/git/endpoint/https_connection.rs:447), native bridge `native.rs:744–774` and `397–419`; SSPI [profile.rs:102](/Volumes/projects/limbo/gwz-dev/gwz-sspi/src/protocol/profile.rs:102), `adapters.rs:51–57`, `supervisor/api.rs:135–140`.

**Violated invariant:** The real producer and consumer must agree on final-origin CBT representation. CallerValues specifies `tls-server-end-point:` followed by a supported certificate digest.

**Counterexample:** A verified origin TLS connection returns a 32-byte SHA-256 digest from `native_tls::TlsStream::tls_server_end_point()`. Core copies those 32 bytes directly into `Connection.binding`. The native bridge copies that value unchanged into `AuthRequest.channel_binding`. `start_captured` invokes the real request validator, which requires lengths 53, 69 or 85 and the literal prefix. It returns InvalidRequest before registration.

The same mismatch exists for raw SHA-384 and SHA-512 digests. The inspected dependency implementations confirm this is an API representation mismatch, independent of deferred EPA/runtime qualification.

**Impact:** A usable Supervisor/capture cannot start native HTTPS authentication with CBT produced by the real TLS path. The production bridge tests conceal the defect: `FakePort::start` at `native.rs:1013–1019` checks only that CBT is nonempty and then returns a fake session, bypassing SSPI request admission.

**Required correction:** Convert the dependency’s supported digest into the required prefixed binding inside initialized fixed zeroizing storage. Preserve immediate wiping of the dependency allocation and refuse unsupported digest shapes. Do not weaken the SSPI validator.

**Closure test:** Drive the existing production TLS checkout/authentication bridge to request construction, then cross the actual SSPI request validator using a portable test seam or a real Supervisor with private fake platform ports. Verify supported digest sizes admit; raw, malformed and unsupported shapes refuse. This test must fail on the present tuple. An assertion confined to FakePort’s nonempty check is insufficient.

### [P2-2] Dropping preparation during pending Start releases core charges before native cleanup confirmation

**Location:** Core [native.rs:776](/Volumes/projects/limbo/gwz-dev/gwz-core/src/git/endpoint/https_worker/native.rs:776), especially `784–797`, Guard fields/drop at `541–574`; SSPI `futures.rs:127–135`.

**Violated invariant:** Drop during Start must retain the actual operation dependency and endpoint capacity slot until native cleanup is Confirmed.

**Counterexample:**

1. Preparation creates Guard and transfers the endpoint slot into it.
2. The local `start` future registers a native record and remains pending during launch or Hello.
3. The enclosing preparation future is dropped or its task is aborted.
4. Dropping SSPI Start cancels the registered record, whose native resources remain supervised and potentially Pending.
5. Guard contains neither a session nor a retained Start owner. Its Drop has nothing to transfer to `native_cleanup`.
6. Guard’s operation dependency and endpoint slot therefore drop immediately.

Awaiting Start after an observed cancellation covers the select branches at `789–790`; it does not cover destruction of the enclosing future. The existing abort regression at `1095–1118` begins after token production and tests pending Finish only.

**Impact:** Native cleanup remains owned by SSPI, but the composed core operation can retire and release its endpoint charge prematurely. Cleanup reporting can miss that native record, and endpoint capacity can be reused before the required composed cleanup proof.

**Required correction:** Install retained ownership of the owned Start future before its first await, analogous to retained Finish. On enclosing Drop, retain cancellation, the Start outcome/receipt, operation dependency and endpoint slot. If the retained Start yields a conversation, cancel it and retain its cleanup receipt. Only confirmed cleanup or an effect-free pre-registration refusal releases the charges.

**Closure test:** Add a production-bridge schedule that holds an admitted Start pending, aborts preparation, seals the operation and verifies the slot remains charged and the operation remains dependent. Pending and Unknown must not release either charge; Confirmed must release both. Also cover a Start still waiting before registration, without repolling a completed future.

### [P2-3] Concurrent reaping can report zero pending native cleanup while unresolved records are outside the holder

**Location:** Core [native.rs:467](/Volumes/projects/limbo/gwz-dev/gwz-core/src/git/endpoint/https_worker/native.rs:467); `https_worker.rs:220–229`; `https_endpoint.rs:334–342`; session driver `pump.rs:304–343`.

**Violated invariant:** Cleanup observations must not report completion during ownership transfer or inspection. Pending/Unknown native cleanup remains counted until Confirmed.

**Counterexample:**

1. A failed or cancelled native Finish leaves one retained Pending record. HTTP physical cleanup has finished and its endpoint entry has retired.
2. The HTTPS runtime calls `reap_cleanup`; `native::reap` takes the entire shared Vec at line 469.
3. Before that reaper restores its unresolved record, the host thread asks `pending_request_count`.
4. Its `Client::pending_cleanup` invokes a second `native::reap`. The shared Vec is empty, so it returns zero.
5. With no remaining physical/entry count, request retirement can save `CleanupReport { pending_local_work: 0, ... }`.
6. The first reaper subsequently puts the unresolved record back.

The slot is still held by the first reaper’s local record. Endpoint shutdown explicitly includes occupied slots, but the request-level observation path does not. Single-threaded assertions of `reap(...) == 1` do not exercise this transfer window.

**Impact:** The recorded operation cleanup report can falsely claim zero local work, even though native cleanup is unresolved and an operation dependency remains retained. The saved report is not corrected by the later return of the record.

**Required correction:** Maintain a conservative count covering records both in the holder and being inspected. Keep status observation separate from polling/disposal where practical; no observer may infer zero from a temporarily emptied collection. Preserve polling and final dependency disposal outside owner/state locks.

**Closure test:** Use a barrier-controlled Probe to pause one real `reap` after it removes the record. Concurrently execute the production request cleanup observation/retirement path and assert it still reports pending work. Repeat with Pending, Unknown and Confirmed; only the final confirmed disposal may expose zero.

### [P2-4] Local native identity refusal is projected as suppressible remote access rejection

**Location:** Core [native.rs:369](/Volumes/projects/limbo/gwz-dev/gwz-core/src/git/endpoint/https_worker/native.rs:369), `728–735`; `request/https_failure.rs:29–40`; `https_remote.rs:217–226`; gitbackend `transport.rs:738–760`; materialize `apply.rs:69–84`.

**Violated invariant:** Local caller/native failure must not manufacture remote rejection or acquire the recovery behavior reserved for accepted helper/access outcomes.

**Counterexample:**

1. Original-caller capture or charged launch verification fails with IdentityMismatch.
2. A native HTTPS scheme is selected after anonymous discovery.
3. `native_code` maps IdentityMismatch to Authentication. Its facts are Sspi with no credential publication and no authenticated=false remote-rejection proof.
4. The retained application consumers treat Authentication without rejection proof or a Negotiate diagnostic list as an ordinary access refusal: `HttpsOpenFailure::model_error` returns RemoteRejected, and `map_open_error` produces libgit2 Auth.
5. For a fresh private member clone, the existing RemoteRejected suppression path removes the partial member and returns success-with-skip.

This can occur for capture refusal before CBT request construction, so P2-1 does not prevent this counterexample. The inherited consumers were written for the named helper/access outcomes; introducing Sspi makes their current broad fallback incorrect.

**Impact:** A local original-caller identity failure can quietly skip a private member rather than report the native failure. It also hides the distinction between local refusal and a server rejecting credentials.

**Required correction:** Make native method/facts explicit in error projection. A local Sspi refusal with no remote rejection proof must remain a visible operation failure and must not receive private-member access suppression. Preserve existing helper suppression and actual remote-rejection behavior without adding secret diagnostics or broadening the application schema.

**Closure test:** Feed a production native availability or Start failure of IdentityMismatch through request error projection and the real private-member clone/materialize path. Assert visible failure, no quiet skip, no publication/default fallback, and unknown authentication. Retain regressions for accepted helper absence and actual repository refusal.

### [P3-1] The RED47 receipt incorrectly attributes an introduced native guard diagnostic to baseline debt

**Location:** Root [GwzSspiHttpsCompositionCheckpoint.md:158](/Volumes/projects/limbo/gwz-dev/dev-docs/GwzSspiHttpsCompositionCheckpoint.md:158); core [serve.rs:119](/Volumes/projects/limbo/gwz-dev/gwz-core/src/git/endpoint/https_worker/serve.rs:119); retained strict-core diagnostic artifact identified in §0.

**Violated invariant:** Gate evidence must distinguish existing debt from introduced diagnostics.

**Reproduction:** Read the retained 47-error JSON output. It includes `clippy::collapsible_if` on `serve.rs:119–124`, the newly added nested native-generation usability guard. The baseline-to-HEAD diff shows this entire guard was introduced by this object. It cannot be an untouched baseline diagnostic.

The earlier nested credential-rejection guard at `serve.rs:5–9` was present in the baseline and was merely reformatted. These two cases must not be conflated.

**Impact:** The checkpoint’s “47 pre-existing” and zero-new attribution overstates satisfaction of the strict targeted gate. This is a bounded evidence defect; the lint itself does not establish runtime failure.

**Required correction:** Correct the new guard without a lint allowance, refresh the permitted owner gate evidence and rewrite the attribution accurately. Continue to report the full strict-core gate as RED while baseline diagnostics remain.

**Closure check:** Confirm the introduced guard diagnostic disappears from refreshed strict-core output and compare remaining diagnostics with baseline evidence. This review does not certify that every other diagnostic is pre-existing merely because its primary source span is unchanged.

## 2. Invariant analysis

The following attacks did not establish additional findings:

- **Original caller and shared capacity:** CLI captures before `with_local_transport_native` enters the action that can fan out. Python captures in `ClientHost::network` before route capture’s detach, operation registration and submission. Python retains one Supervisor per ClientHost; CLI uses one per invocation. NativeCaller shares the actual port/capture rather than constructing another Supervisor per Open. SSPI issuer identity, owned Start and closed admission checks match the accepted public shape.
- **Deadline ownership:** Native-capable preparation anchors positive D after admission and before checkout/adoption. Checkout, discovery waits, helper work in authentication, token rounds and Finish use that retained instant. The SSPI adapter translates it directly into `Deadline`, without a new allowance. Zero refuses native selection before Begin. Cumulative HTTP I/O remains a separate allowance.
- **Source selection:** The parser implements challenge-dependent helper admission and the accepted Negotiate/NTLM/Digest ordering. Helpers-disabled and Negotiate-only routes avoid helper lookup. Unavailable/unusable helper outcomes can select current logon; timeout/cancellation remain terminal. Explicit source is not replaced after native publication.
- **Facts and completion:** History preserves mechanism selection and authority, rejects mechanism switching/regression, and tracks Complete independently. HTTP 200 cannot bypass that completion requirement. Final response tokens are consumed on the tested Continue path. Authorization becomes offered at the authenticated send boundary after successful challenge drain.
- **Physical generation and POST:** The authenticated owner compares exact lease generation and revocation state. Replacement refuses before POST. Serve records receive-pack Effect::Possible before request handoff. No POST authentication replay or credential-switch path was added.
- **Protocol shape:** Taut owns the additions. Codec validation checks Sspi/native equivalence, native observation structure and profile-1 exclusion. Mux routing checks policy/source consent. Profile-1 capability intersection removes native policies; the bootstrap helper retains its existing default while the actual mux offers profile 2. No profile 4 or application carrier was introduced.
- **Finish retention:** The existing retained Finish owner keeps its future and cleanup receipt, avoids completed-future repoll and holds both charges through Pending/Unknown. The independently executed Finish abort/cancel/expiry tests support this narrower claim. P2-2 identifies the missing Start counterpart; P2-3 identifies observer accounting.
- **Secret storage:** Native challenge decode and Authorization construction use initialized fixed wiping owners. Live wipe regressions cover encoding/decode/parser errors and both header owners. Dependency-owned HeaderValue, TLS/provider buffers and copies remain explicitly outside the wipe guarantee. Correcting CBT representation must retain these ownership rules.
- **Retry and activation:** Outer checkout timeout conservatively lacks fresh-connect retry provenance; eligible pool-returned setup failures retain it. No dependency-manifest expansion, Windows activation or release action appeared in the scoped diff.

The executed tests are meaningful orchestration coverage. They are insufficient for claiming complete real-adapter composition because the fake native port bypasses the decisive request validator. The installed Darwin artifact and ordinary caller tests establish packaging/host reachability within their stated scope, not native authentication.

The full strict-core gate remains RED. The retained output confirms its count but refutes the complete baseline attribution. I have not converted it to PASS.

## 3. Risks and next action

Native Windows identity/provider behavior, EPA algorithms, physical worker disposal and installed Windows host qualification remain explicitly deferred. No finding here demands their execution or changes the activation boundary.

The single next action is a bounded remediation of P2-1 through P2-4 and P3-1, followed by a settled-tuple re-review with the specified production-boundary regressions. The fixes fit the accepted architecture: correct the CBT adapter, complete retained Start ownership, make cleanup counts conservative, correct native error consumers and repair the lint evidence. Acceptance remains NO-GO until those corrections are verified.
