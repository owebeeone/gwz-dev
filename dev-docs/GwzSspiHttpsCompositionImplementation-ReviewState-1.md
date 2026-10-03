# GwzSspiHttpsCompositionImplementation — State-AXIS REVIEW

**Review object:** Round1 closure of the cohesive HTTPS SSPI step4b implementation at the corrected tuple below. The accepted contract remains [GwzSspiHttpsCompositionDesign-DRAFT.md](/Volumes/projects/limbo/gwz-dev/dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md), despite its historical filename. Implementation acceptance remains pending, 2026-10-04.

**Baseline:**

| Repository | Reviewed HEAD | Round1 comparison baseline |
|---|---|---|
| root | `7066ff222abf7995e7e0103e9ad38543173d6079` | `b72dccf813816f41a508eb0fc2f9b2f071b323a0` |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` | `efdd0a2cf66be889364466c7d69c97cc2736c278` |
| gwz-sspi | `c88fa0e174b185a43e0d0d0c91660cb0957e380a` | `58cc87c99bca21874a63d0ba11209b75a3e0c50a` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` | Unchanged |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` | Unchanged |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` | Unchanged |
| gwz-core-evidence | `ba70034feaeb384619d48dbfaa06c02df6f508b3` | Private supporting receipts |

The original implementation-range baselines remain core `56f56a3d5ea3c9f8a50ec9e4c42453c1b92d9c79`, SSPI `616e32cceeea1b7df1d7bbe1c1695a409a733f6d`, transport `1aab733783e06b25cb5d2321d71ec0b34417a29c`, CLI `0c7dfaf0199731648d2360358284010b2b4575c1`, and Python `ded47130af23720099e7b6a92ccb9a161bb5db9a`.

Sources were inspected through Git range diffs, committed documents and numbered working files. All seven HEADs matched the prescribed tuple at start and end. Final tracked `git diff --stat HEAD` was empty in every repository.

**Date:** 2026-10-04

**Axis:** State machines, interruption, races, cleanup ownership, capacity and fail-closed reporting. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their current reports. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2 or P3 findings. The original P2-1, P2-2 and P3-1 are independently verified closed. No new architectural root cause was found.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 — Start retention | Install an owned Starting holder before await; retain both core charges after preparation destruction; cancel late conversations. | Retraced `NativePort` and SSPI Start registration/error/drop behavior. Executed the production preparation-abort regression for simulated registered and pre-registration outcomes. Pending and Unknown retain the native record, request permit and sealed operation dependency; confirmed cancellation or a known no-resource outcome permits release. No native step occurs after abort. | Closed |
| P2-2 — Concurrent reaper visibility | Count records claimed by reapers while checks remain outside the cleanup lock. | Executed the barrier-controlled regression through the real `Client::pending_cleanup` method, with actual operation dependencies and semaphore permits. A second observer sees one claimed record; concurrent insertion produces two outstanding records. Pending/Unknown preserve both charges; confirmed removal releases only the corresponding owner. Retraced retirement/capacity consumers to this same method. | Closed |
| P3-1 — Lint attribution | Correct the introduced guard diagnostic and withdraw the inaccurate RED47 all-baseline claim. | Inspected the corrected braced guard and parsed the archived refreshed diagnostics: 45 coded errors, with the introduced guard absent. Independently compared all primary snippets to the original range baseline: 44 exact matches and one whitespace-only match. Both checkpoint documents disclose the limited attribution and retain the full strict-core RED result. | Closed |

## Changed-range analysis

Core changes comprise ten files, including its inventory ledger, with 431 insertions and 54 deletions relative to the initial reviewed revision. The substantive changes are the Starting holder, authoritative claimed-record count, prefixed CBT construction, visible local SSPI failure projection, corrected generation guard, and regression fixtures. The backend Windows-policy override is enclosed in test conditional sections; its production selector preserves the existing platform behavior.

SSPI adds a private request-validator fixture and exact disposal documentation. The fixture invokes the existing private validator without introducing a public API or production dependency. Root changes update the caller guide, implementation receipt, live checkpoint, review artifacts and managed member revisions. Transport, CLI and Python source HEADs remain unchanged.

The CBT, failure-projection and teardown-documentation changes belong to the merged dispositions. Formatting and inventory adjustments support those changes. I found no substantive change outside the authorized correction scope, no public wire/API/runtime expansion, and no activation-boundary change.

**NEW ARCHITECTURAL root causes:** None found.

## 0. Evidence base

### Authority and documents

This closure continues the original review’s authority and unchanged-range analysis. I read the focused round1 brief, original State report, merged remediation plan, corrected implementation receipt and top live checkpoint. The corrected authority reference is root [GwzSspiDesign.md](/Volumes/projects/limbo/gwz-dev/dev-docs/GwzSspiDesign.md) §6.

The original review inspected root/member instructions, evidence rules, process authority, accepted composition design and acceptance, budget disposition, core requirements/design, Windows parity, credential-helper design and caller/API documentation. The revised brief authorizes the 37-file/3,500-added-line budget and two separate inventory ledgers. No current peer re-verdict was read.

### Sources and evidence inspected

- Core [native.rs](/Volumes/projects/limbo/gwz-dev/gwz-core/src/git/endpoint/https_worker/native.rs:386): real adapter lines 386–425; cleanup and holders lines 465–694; Start installation and transition lines 896–929; real-validator tests lines 1171–1246; abort and concurrent-reaper tests lines 1247–1327.
- Core [https_worker.rs](/Volumes/projects/limbo/gwz-dev/gwz-core/src/git/endpoint/https_worker.rs:220), lines 220–239, and [https_endpoint.rs](/Volumes/projects/limbo/gwz-dev/gwz-core/src/transport_host/https_endpoint.rs:334), lines 334–343: cleanup observers and their retirement/capacity inputs.
- SSPI [futures.rs](/Volumes/projects/limbo/gwz-dev/gwz-sspi/src/supervisor/futures.rs:63), lines 63–135: registration, terminal errors, Conversation admission and registered Start Drop.
- Complete correction diffs for CBT construction/capture, generation guard, local failure consumers, backend test policy and private-member materialization regression.
- SSPI private validator fixture, Supervision disposal recipe and root caller-guide amendments.
- Corrected checkpoint’s strict-Clippy disclosure and committed private round1 README, manifest and diagnostic JSON.

Read-only hashing verified all 50 source/document entries and all 11 archived raw files against the evidence manifest. This binds those receipts to the reviewed files; it does not independently repeat every archived execution.

### Commands actually run

All three permitted targeted commands completed successfully, with build outputs outside the reviewed tree:

```text
RUSTFLAGS='--cfg gwz_transport_candidate' CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-target cargo +1.95.0 test --manifest-path /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-prepared/Cargo.toml --lib --locked --offline git::endpoint::https_worker::native::tests
```

Result: exit 0; **26 passed**, zero failed, in 2.97 seconds. This includes the Start-abort, concurrent-reaper, real-validator, deadline, Finish, source-selection, scope, publication and wiping cases.

```text
RUSTFLAGS='--cfg gwz_transport_candidate' CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-target cargo +1.95.0 test --manifest-path /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-prepared/Cargo.toml --lib --locked --offline private_members::
```

Result: exit 0; **8 passed**, zero failed, in 1.25 seconds.

```text
CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/sspi cargo +1.95.0 test --manifest-path gwz-sspi/Cargo.toml --doc --locked --offline
```

Result: exit 0; **27 passed**: seven compiled recipes and twenty compile-fail checks.

Read-only Python inspection independently established **45 coded strict-Clippy errors** in the refreshed archive. The introduced generation guard has no diagnostic. Primary-span source comparison found 44 exact baseline snippets and one whitespace-only baseline match at `serve.rs:5–9`.

I did not rerun full Clippy, the 199-test HTTPS filter, source guards, packaging or loaded ClientHost checks. Those executions remain owner receipts. The full strict-core gate remains **RED45**. No network, live credentials, file edits or Git mutations were used.

## 2. Invariant analysis

- **Start interruption and publication:** Starting is installed in Guard before the first suspension. Cancellation, expiry or enclosing-task destruction transfers that holder and both charges to endpoint cleanup. A late successful Conversation is cancelled by the reaper; it cannot reach step/publication. A pre-registration failure releases charges only after its no-resource outcome is known. The original missing-owner sequence no longer applies.
- **Pending/Unknown/Confirmed:** An unresolved Start remains unconfirmed. A completed Start’s cancellation receipt or failure probe retains Pending and Unknown. Only confirmed cleanup, or an explicit no-resource outcome, removes the retained owner. The executed abort regression also waits for physical cleanup before accepting aggregate zero.
- **Concurrent observation:** Reaping increments `checking` while claiming records under the lock. Other observers count those claims throughout external proof checks and concurrent insertion. Unconfirmed records return before claims are decremented. The executed barrier test reproduced the original visibility interval without obtaining a false zero.
- **Lock scope and disposal:** Future polling, session cancellation, cleanup probes and confirmed Pending destruction occur outside the shared cleanup lock. Starting similarly extracts work/outcomes before polling or cancellation. Completed Start/Finish work is removed and is not repolled. No new callback or provider work was placed under these locks.
- **Deadline and charge boundaries:** Starting still receives the original immutable `D`; cancellation and expiry select against it without resetting the budget. Moving Start ownership does not create another Supervisor, capacity domain or runtime owner. Passing tests preserve zero-before-Begin, pool waits, helper deadline/cancellation terminality and native round limits.
- **CBT and physical scope:** The corrected producer constructs `tls-server-end-point:` plus a supported 32/48/64-byte digest in initialized wiping storage, then wipes the dependency buffer. Executed tests cross the existing SSPI validator with the real TLS-produced binding and supported synthetic shapes; raw/malformed bindings refuse. This establishes producer/validator agreement. Exclusive generation, replacement refusal and native redirect behavior remain intact.
- **Facts and visible failures:** Complete, mechanism authority, offered bytes and remote acceptance remain separate facts. The corrected local consumers preserve SSPI refusal as a visible error. The executed private materialization case produces `GitCommandFailed`, leaves lock bytes unchanged, creates no repository and sends no Authorization.
- **Secrets and effects:** Existing live wiping, bounded decode/refusal and receive-pack drain/publication tests pass. The correction adds no post-byte authentication replay or credential switch. Hyper/native-TLS/provider copies remain outside the declared wiping guarantee.
- **Evidence fidelity:** The inaccurate historical RED47 attribution is expressly withdrawn. Current RED45 diagnostics and their source-comparison limits are preserved. Neither a full strict PASS nor a compiled-baseline proof is claimed.

No durable format, restart-adoption rule or new filesystem recovery state was introduced by this correction.

## 3. Risks and next action

Tombstone eviction can still leave a core owner permanently charged when Confirmed was never observed. This documented behavior remains fail-closed; Unknown does not become disposal proof.

The portable tests exercise production core ownership with synthetic native sessions. They do not establish native Windows registration/provider execution, live caller identity, EPA, physical worker cleanup or installed-host qualification. Those outcomes and release activation remain explicitly deferred. Full strict-core Clippy remains RED45.

The next action is for the lane owner to aggregate this State GO with the other independent closure verdicts and record the implementation acceptance decision at the settled tuple. No further State remediation is required.
