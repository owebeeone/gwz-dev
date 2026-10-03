# GwzSspiHttpsCompositionImplementation — Code-AXIS REVIEW

**Review object:** Round1 closure of the cohesive HTTPS SSPI step4b implementation, its bounded correction patch, caller guide and implementation checkpoint. Controlling contract: root `dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md`, accepted despite its historical filename.

**Baseline:**

| Repository | Reviewed HEAD | Round1 correction base |
|---|---|---|
| root | `7066ff222abf7995e7e0103e9ad38543173d6079` | `b72dccf813816f41a508eb0fc2f9b2f071b323a0` |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` | `efdd0a2cf66be889364466c7d69c97cc2736c278` |
| gwz-sspi | `c88fa0e174b185a43e0d0d0c91660cb0957e380a` | `58cc87c99bca21874a63d0ba11209b75a3e0c50a` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` | Unchanged |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` | Unchanged |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` | Unchanged |
| gwz-core-evidence | `ba70034feaeb384619d48dbfaa06c02df6f508b3` | Supporting private receipts |

Sources were inspected with `nl`, `sed`, `rg`, read-only Python, `git diff` and `git show`. All seven HEADs matched the required tuple at both start and end. Final `git diff --name-only HEAD` checks returned no tracked changes in any reviewed repository.

**Date:** 2026-10-04

**Axis:** Code — architecture, interfaces, call graphs, ownership, error projection and compatibility. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their current re-verdicts. Filed verbatim by the lane owner.

**Verdict: GO** — all four prior P2 findings and the prior P3 finding are verified closed. No new finding or NEW ARCHITECTURAL root cause was established. This accepts the scoped implementation correction; full strict-core Clippy remains RED45, and Windows activation, runtime qualification and release remain deferred.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 | Convert raw TLS digest into the required prefixed CBT using initialized wiping storage; retain real SSPI validation. | Retraced the actual TLS producer through `Connection.binding` and native `AuthRequest`. Independently executed the production TLS test crossing the existing private SSPI validator. Supported 32/48/64-byte shapes admit; raw and malformed bindings refuse; unsupported digest lengths refuse and their source bytes are wiped. | **CLOSED** |
| P2-2 | Retain owned Start before its first await, including both composed charges and late cancellation receipt. | Retraced Guard installation, Start outcome transfer and cleanup retention. Independently executed production preparation abort before and after modeled registration. Pending/Unknown retain the endpoint slot and sealed operation dependency; confirmed cleanup or an effect-free pre-registration refusal permits release. No late token step occurs. | **CLOSED** |
| P2-3 | Count records claimed by a reaper until final disposal or restoration. | Independently executed the barrier-controlled production cleanup-count regression for Pending, Unknown and Confirmed, including concurrent insertion. Retraced request retirement’s use of that same count. Claimed work remains visible; zero becomes legal only after disposal of all confirmed records. | **CLOSED** |
| P2-4 | Preserve local Sspi refusal as a visible failure through model, Git and private materialize consumers. | Retraced the actual IdentityMismatch producer and both consumers. Independently executed private materialize against the native refusal path: it returns GitCommandFailed, sends no Authorization, performs one discovery request, preserves the lock and avoids quiet success. Existing private access-refusal/public-failure regressions also pass. | **CLOSED** |
| P3-1 | Correct the introduced serve guard and replace the inaccurate all-baseline lint claim with bounded attribution. | Inspected the corrected guard and refreshed raw JSON: 45 coded errors, with no diagnostic on the introduced generation guard. Independently reproduced the disclosed primary-snippet comparison: 44 exact baseline matches, one inherited serve guard differing in formatting. The checkpoint withdraws the inaccurate RED47 attribution and expressly retains full-gate RED status. | **CLOSED** |

## Changed-range analysis

Core’s correction range changes ten files, including the separately authorized inventory ledger: 431 insertions and 54 deletions. Its substantive changes are the CBT constructor and call site, owned Start retention, conservative reaper accounting, native failure projection, the collapsed serve guard and the corresponding regressions.

The additional Git binding code supplies a test-only Windows-policy override for the portable private-materialize regression. The production branch continues to use the existing platform policy selector. Conditional declarations remain inside explicit `cfg_if` boundaries. The ledger changes match the two authorized adjustments: the relocated Runtime syntactic owner and the new private-materialize test site.

SSPI changes are limited to its private validator fixture and disposal documentation: 71 insertions across two files. The fixture invokes the existing private validator; it adds no public admission API, production dependency or wire field. Ordinary standalone execution supplies its own synthetic values.

Root changes record the correction, evidence, merged dispositions, preserved initial reports, caller teardown recipe and managed settlement. Transport, CLI and Python source HEADs remain unchanged. The private evidence member records supporting receipts and fingerprints; it is not a public test dependency.

The correction preserves the accepted architecture, deadline, credential-source, capacity, protocol and activation boundaries. No change outside the authorized dispositions established a defect. **No NEW ARCHITECTURAL root cause was found.**

## 0. Evidence base

I read the focused Code closure brief, merged RemPlan, my original filed Code report, corrected checkpoint, caller-guide correction and the top live program-checkpoint entry. The original review’s authority analysis remains applicable to unchanged ranges. The actual SSPI design authority is root `dev-docs/GwzSspiDesign.md §6`; the original prompt’s member-local path was erroneous.

Principal corrected sources and retained consumers inspected:

| Boundary | Sources inspected |
|---|---|
| CBT construction and admission | Core `https_auth/secret.rs:48–61`, `https_connection.rs:447–455`, native request construction `native.rs:864–894`; SSPI `secret.rs:15–60`, `protocol/profile.rs:102–109`, private validator fixture |
| Start and cleanup ownership | Core `native.rs:373–424`, `451–694`, `896–929`; `https_operation.rs:36–79` |
| Production regressions | Core `native.rs:1171–1326`, including bounded real-validator execution, pre-/post-registration abort and concurrent reaping |
| Cleanup consumers | Core `https_worker.rs:220–229`, transport-host `https_endpoint.rs:334–343`, session driver `pump.rs:304–343` |
| Error projection | Core native availability path `native.rs:833–855`; request `https_failure.rs:21–48`; `https_remote.rs:216–286`; gitbackend `transport.rs:738–763`; materialize `handle_materialize/apply.rs:55–92` |
| Private materialize and policy seam | Core correction diffs for `private_members.rs` and `transport_binding.rs` |
| Documentation and lint | Corrected serve guard, root checkpoint and caller guide, SSPI Supervision correction, refreshed private diagnostic receipt |

Executed independently, from the workspace root, using only the permitted targeted commands:

```sh
RUSTFLAGS='--cfg gwz_transport_candidate' \
CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-target \
cargo +1.95.0 test \
--manifest-path /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/core-prepared/Cargo.toml \
--lib --locked --offline git::endpoint::https_worker::native::tests
```

Result: exit 0, **26 passed**, no failures, 5.17 seconds. This includes the subprocess calls to the real SSPI request validator.

The same command with filter `private_members::` returned exit 0, **8 passed**, no failures, 1.15 seconds.

```sh
CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/https-composition/sspi \
cargo +1.95.0 test --manifest-path gwz-sspi/Cargo.toml \
--doc --locked --offline
```

Result: exit 0, **27 passed**: seven compiled recipes and twenty compile-fail probes.

The prepared core `src` resolves to the reviewed member’s actual source directory. All build outputs remained external.

I inspected the committed private round1 evidence README, manifest and refreshed Clippy JSON. Read-only hashing verified:

- All **50** recorded source/document file hashes match the corrected tree.
- All **11** retained raw-file hashes match their manifest entries.
- The refreshed JSON contains **45** coded error diagnostics.
- Its only serve-file diagnostic concerns the inherited credential-rejection guard at lines 5–9.
- Baseline primary-snippet comparison reproduces **44 exact matches and one formatting difference**.

I did not independently rerun Clippy, the 199-test HTTPS filter, source guards, packaging or installed-host tests. Their checkpoint receipts remain owner evidence, with their disclosed scope. No current peer re-verdict was opened. No source, formatting, generation, installation or Git mutation was performed.

## 2. Invariant analysis

**CBT producer and consumer now agree.** The production connection path passes the raw digest to `channel_binding_digest`, which admits only 32, 48 or 64 bytes, allocates initialized final-size storage, writes the prefix and digest, copies into SSPI’s initialized fixed wiping owner, and overwrites the dependency buffer. Unsupported shapes also overwrite that buffer. The unchanged validator still requires the literal prefix and lengths 53, 69 or 85.

The production TLS test supplies its actual constructed request to ValidatorPort. Its bounded subprocess reconstructs the asserted known-valid Ntlm/current-logon/HTTP-localhost fixture fields and invokes the existing private request validator with the produced CBT bytes. It requires exactly one admission/refusal receipt and fails when execution is unavailable. This closes the structural representation mismatch; subsequent synthetic provider behavior remains orchestration coverage.

**Pending Start owns both composed charges.** Guard receives the endpoint slot and operation dependency before Start is polled. Its shared Starting owner is installed before the select await. Cancellation, expiry and enclosing-future destruction transfer that owner to retained cleanup. A late session is cancelled outside the shared holder lock; its receipt remains retained. A completed future is removed and its stored outcome is consumed without repolling it.

The production abort regression covers modeled pre-registration and registered outcomes. It demonstrates the original failure boundary: the slot stays charged at 63 available permits, a sealed operation refuses reacquisition, Pending/Unknown remain retained, and final settlement returns capacity to 64. The retained path performs no token step.

**Reaper accounting remains conservative during transfer.** Under the holder lock, removed records are added to `checking`. Probes and final record disposal occur outside that lock. The reaper restores unresolved records before subtracting its claim. Concurrent observers count both queued and claimed records.

The barrier regression executes the same `Client::pending_cleanup` component used by production request counting, including a second insertion while the first record is claimed. I separately traced the unchanged request-retirement consumer: it adds this value directly when recording `pending_local_work`. The test does not separately execute the entire session-retirement driver, but the original false-zero interleaving is excluded at its shared counting boundary.

**Local native failure stays visible.** IdentityMismatch still maps to Authentication with Sspi facts. The availability path returns those facts before publication or native token work, leaving authentication unknown. Both application projection and Git projection now explicitly recognize Sspi. They produce GitCommandFailed and GenericError respectively, rather than the suppressible remote-access classification.

The actual private-materialize regression crosses discovery, native availability, transport failure, backend clone and materialize handling. It fails visibly and preserves the lock. Existing accepted private-access suppression and public/server failure rows pass. RepositoryRefused’s existing separate classification remains unchanged.

**Deadline, publication and compatibility contracts survive the correction.** Start’s new expiry branch uses the existing immutable `until`; it creates no additional allowance. Current tests retain coverage of pool wait, helper timeout/cancellation, zero-native refusal, native rounds, independent HTTP expiry, final remote tokens, Finish retention and exact-generation POST refusal. The collapsed serve guard preserves its revocation and refusal behavior.

No public API, wire profile, application carrier, Supervisor/runtime owner, capacity domain, credential source or activation switch was introduced. The unchanged CLI/Python capture and shared-Supervisor analysis from the initial review remains applicable.

**Lint evidence now matches its limits.** The introduced generation guard is corrected without an allowance, and its diagnostic is absent from refreshed output. The checkpoint explicitly withdraws the historical “all47 pre-existing/zero-new” assertion. The 44 matching primary snippets and inherited formatted guard support the disclosed limited comparison; they do not establish a fully compiled baseline attribution. Full strict-core Clippy remains **RED45**, not PASS.

## 3. Risks and next action

Native Windows identity/provider execution, EPA behavior, physical worker disposal qualification, installed Windows host qualification and release remain explicitly deferred. The portable validator regression proves structural request admission, not those outcomes. Full strict-core baseline debt remains unresolved within its disclosed gate disposition.

The next action is owner filing of this Code closure report and completion of the independent axis re-verdict process on the settled tuple. This Code review requires no further remediation and does not authorize activation, push, tagging or release.
