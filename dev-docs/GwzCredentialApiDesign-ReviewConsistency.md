# GWZ Credential API design — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzCredentialApiDesign.md`, revision 0, DRAFT, composed with the accepted `dev-docs/GwzConnectionApiContract.md`, at root `158a940b961941ebe0bd5c47c771dd9658156a04`.

**Baseline:**

| Repository | Pinned HEAD |
| --- | --- |
| `.` | `158a940b961941ebe0bd5c47c771dd9658156a04` |
| `gwz-core` | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| `gwz-py` | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| `gwz-cli` | `5193db962e672c54cd20ab2ef8448d904129a559` |
| `gwz-git` | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| `gwz-transport` | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| `gwz-sspi` | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| `git2-rs` | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| `libgit2` | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All nine HEADs matched at the beginning and end. Substantive inspection used pinned Git objects.

**Date:** 2026-10-19.

**Axis:** Consistency, including API Surface: the complete Credential/Connection composition, controlling requirements, lifecycle completeness, provider mapping and independent-test feasibility. Independent, adversarial, read-only. Nothing here relies on the other axis’s current-round report. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. No P0, P1 or P3 findings. The draft identifies useful boundaries, but expressly leaves central lifecycle and provider-composition decisions unresolved. These are design-gate defects, not demands for finished implementation.

---

## 0. Evidence base

Credential’s public API was read before implementation source, followed by the accepted Connection public surface. The complete 241-line Credential draft was inspected.

Consequential evidence:

- Credential design §§1–8, particularly:
  - responsibility and dependency boundary, lines 41–90;
  - public API and signing question, lines 92–148;
  - lifecycle rules and unresolved lifecycle statement, lines 150–182;
  - test layers and outstanding assessment, lines 184–227;
  - adoption boundary, lines 229–241.
- Accepted Connection contract:
  - §§3–5: dependencies, capabilities, admission, domain retention and revocation;
  - §7, lines 456–527: original-entry inputs, provider ports, retained charge and shared SSH proofs;
  - §7.2, lines 548–565: request-scoped and connection-bound authentication;
  - §10, lines 663–706: budgets and timeout behavior;
  - §§12–15: composition, proof gates and amendments.
- `GwzConnectionApiContract-Verdict.md`, lines 1–93.
- `GwzConnectionRemediationPlan.md`, especially responsibility, authentication and independent-proof requirements.
- Core `GwzRemoteTransportRequirements.md:244–272`, placement P1–P9.
- Core `GwzTransportTR18-OperatorDecisions.md`, including TD1–TD4 and the explicit-identity SSPI/Digest release disposition.
- Pinned implementation seams:
  - `gwz-core/src/git/endpoint/https_worker/native/sspi.rs:1–224`;
  - `https_worker/native.rs:1–300`, including challenge selection;
  - `https_worker/native/exchange.rs:1–190`;
  - `https_worker/native/owners.rs:1–163`;
  - `ssh_worker/open_request.rs:1–226`;
  - `agent_auth.rs:1–169, 182–311`;
  - `ssh_setup.rs:110–220`;
  - `gwz-py/native/src/route/transport.rs:32–65, 131–158, 176–207`;
  - `gwz-sspi/src/lib.rs:1–31`;
  - `gwz-sspi/docs/Supervision.md:1–220`.

A pinned diff confirmed that the accepted Connection body and root process instructions did not change substantively from the previously reviewed Connection checkpoint; Connection’s change was its acceptance header.

Commands were inspection-only: `git rev-parse`, `git show`, `git diff`, `rg`, `nl`, `sed` and `cat`. No files, builds, tests, network operations, mutations or further agents were created. No current-round peer report or excluded draft was read.

## Problem, requirement provenance, and scope

The operator wants fewer conflated connection/credential responsibilities and cheaper independent tests for small changes. A separate Credential repository was selected as a reasonable design direction, followed by an explicit request to review it together with Connection. That supports proposing the boundary; it does not authorize repository creation, source migration or a provider rewrite.

The cited coupling is real. NativeHost currently combines SSPI supervision, its spawn gate and Git2Backend construction; NativeCaller additionally carries machine-proxy data (`native/sspi.rs:8–27, 57–71, 100–129`). SSH OpenRequest combines identity authority with Git service/path, pool admission and setup state. Python composes captured inputs with its operation transport route.

Those observations do not establish that these responsibilities must move together. Existing source supplies narrower supported routes:

- NativePort retains original caller provenance and exposes start/session operations without constructing a connection pool (`native/sspi.rs:141–224`).
- Starting/Finishing owners retain native work independently of an abandoned exchange (`native/owners.rs`).
- SSH authentication already has a bounded signing callback after host trust validation (`agent_auth.rs:41–53, 82–85, 197–228`).

These are concrete feasibility evidence for a localized interface extraction. No dependency upgrade, replacement SSH implementation or new supervisor is demonstrated necessary.

The draft correctly limits its editable scope to itself and keeps Connection/reference repositories unchanged. Its unresolved questions nevertheless prevent accepting the *composed API*. P2-1 concerns retained-resource lifecycle; P2-2 concerns the operation/provider mapping needed to establish both real consumption and the promised test boundary.

Existing Windows launch-safety and runtime-qualification shortcomings remain separate obligations. Discovery here grants no authority to repair them.

## 1. Findings

### [P2-1] Credential’s new owner lacks a defined terminal and retained-resource contract

**Location:** Credential lines 99–121, 133–140 and 150–182, especially the explicit unresolved close/drop statement at lines 179–182. Compare Connection §§4–5 and §10: shared CleanupDomain, retention before effects, immediate sealing, finite cleanup and retained-charge accounting.

**Violated invariant:** Both APIs must compose through one explicit admission/cleanup authority, with observable terminal outcomes and retained ownership after cancellation, dropped futures and owner shutdown.

The draft introduces CredentialOwner, PreparedCredential, AuthOpening, AuthSession, AuthFinishing, AuthClosing and ProviderClosing. It does not define:

- the outputs and cleanup states of the finishing/closing values;
- when owner close seals admission;
- what happens to already prepared but unstarted selections;
- drop behavior for preparation, opening, session and terminal futures;
- who progresses abandoned generic-provider work;
- how provider reservation/retained-byte ownership connects to Connection’s existing domain;
- finite bounds/defaults or explicit inherited bounds for these new owners.

Saying to retain work through the existing supervisor and compose receipts once states the desired result, but does not supply the resource-transfer contract. The draft expressly confirms that these rules remain unresolved.

**Credible trace:**

1. Two Connection operations use one CredentialHandle.
2. A native authentication start registers work; its result or Finish is still pending.
3. Operation A cancels and its authentication future disappears. Operation B remains live.
4. CredentialOwner is closed or dropped while Connection’s domain still holds physical resources.
5. The composition must decide which outstanding credential work is revoked, what terminal receipt Connection receives, which charge remains held, and who advances eventual cleanup after all ordinary handles disappear.

The new API provides no defined result or ownership transition for steps 3–5. Implementers must invent it or import provider-specific lifecycle behavior into the composition. The latter would defeat the claimed generic independent-test boundary.

The existing native route demonstrates why this is substantive: it retains Starting, Session or Finishing plus cleanup probes and charged operation/slot ownership (`native/exchange.rs:5–42`). The proposed wrapper cannot discard that information merely because gwz-sspi itself remains supervised.

**Impact:** The joint design cannot yet establish honest completion, exactly-once accounting or final teardown without additional lifecycle choices. Portable owner-teardown and pending-cleanup tests lack a specified oracle.

**Required correction:** Define a small lifecycle/receipt table covering every owned public stage and its drop behavior. Map its reservation and retention capabilities to the accepted Connection domain and existing native supervisor, including sealing, late completion and final-owner drop. State the finite bounds or the precise inherited allowances. Identify any necessary Connection amendment in the Credential document; do not edit or silently reinterpret the accepted contract.

**Smallest closure tests:** Specify deterministic fake traces for cancellation during opening, dropped finishing, owner close before begin, and owner close with A pending/B active. Assert publication revocation, the exact cleanup receipt, retained bytes/permits and eventual exactly-once release after all caller futures disappear.

**NEW ARCHITECTURAL ROOT: YES.** The new provider owner introduces an unresolved lifetime/accounting boundary between Credential and Connection. This is one root, not a request for broader native qualification.

### [P2-2] The operation selection and provider-conversation mapping remains undecided

**Location:** Credential lines 72–76, 106–118, 124–138, 159–165 and 203–222. Compare Connection lines 152, 458–498 and 504–520.

**Violated invariant:** The design must show one usable production path through both APIs and enough provider-specific responsibility to establish independently testable behavior without moving connection/Git responsibilities into Credential.

PreparedCredential is described as “one operation’s admitted selection,” but `begin(self, …)` consumes it for one authentication session. Connection permits multiple concurrent exchanges, per-open identity selection and shared per-binding SSH proof work. The draft does not select how those lifetimes compose.

It also leaves SSH’s central provider seam as “may need a signing callback,” while deferring the responsibility/consumer map and actual SSH/HTTPS traces to a later assessment.

**Credible composition trace:**

1. One original-entry caller capture and operation AuthContext are prepared.
2. A Connection operation opens two HTTPS exchanges concurrently, potentially against different origins, and may also require an SSH setup or reuse proof.
3. The first `PreparedCredential::begin(self, …)` consumes the documented operation selection.
4. The second exchange needs independently scoped conversation state while preserving the same original caller, operation cancellation and charge ownership. An SSH reuse proof may additionally be shared by concurrent waiters.
5. The document does not say whether the operation selection is reusable, issues one-use conversation tickets, or is reconstructed through repeated prepare calls. Nor does it define the retained operation object and retirement rule for the chosen route.

Repeated prepare calls could be a valid design; a reusable operation capability could also be valid. The defect is that neither route is specified, including its relationship to captured provenance, per-open identity and shared proof state.

The provider-specific mapping has the same unresolved boundary. The current SSH function owns a handshaken SshConnection and invokes libssh2 synchronously; its signing callback validates a bounded authentication blob before agent signing (`agent_auth.rs:45–53, 197–228, 292–311`). Moving that function wholesale would move connection/native-session responsibility. A constrained credential-signing seam is feasible, but the draft has not selected it or assigned request validation, signing and verification across the two APIs.

For HTTPS, the current route selects Negotiate/NTLM over Basic, applies the original caller and actual final-origin binding, and retains native completion separately from remote acceptance. The generic names AuthChallenge/AuthStep do not yet map those production responsibilities or identify which changed modules the portable tests would exercise.

**Impact:** Acceptance would freeze vocabulary before settling the main composition boundary. An implementation lane would have to choose ownership, multiplicity and provider adaptation while supposedly implementing an accepted API. The claimed reduction in test coupling remains unassessable for those central paths.

**Required correction:** Complete the draft’s bounded responsibility/consumer assessment before acceptance. Select operation-versus-conversation lifetimes and show their public derivation/retirement operations. Give concrete SSH setup/reuse-proof and HTTPS helper/native traces through existing Connection ports, preserving current provider policy. Name the bridge owner and portable versus provider-specific code. Record exact proposed Connection amendments where necessary.

The correction can retain the existing native Port/Session and SSH signing mechanisms. No provider rewrite or dependency upgrade is required by this finding.

**Smallest closure tests:** Specify a fake composition with two simultaneous conversations from one captured operation, different origins/identities where supported, cancellation of one without cancelling the other, and shared SSH proof waiters with independent deadlines. Include a source-based change-impact comparison for one credential parser/policy change and one pool-only change, showing which portable/provider/composition suites are required. Execution belongs to later authorized implementation.

**NEW ARCHITECTURAL ROOT: YES.** This is an unresolved operation/provider interface and responsibility split, distinct from P2-1’s retained-resource termination contract.

## 2. Invariant analysis

### API Surface and first-use walkthrough

The cold surface supplies recognizable build, prepare, begin, respond, finish, cancel and owner-close verbs. It avoids an ambient default provider, arbitrary signing API and public key-export escape hatch.

The walkthrough stops at two consequential points:

- After preparing one operation, the public recipe does not define how to derive the multiple conversations and proof work required by Connection: P2-2.
- Finish/cancel/owner close have named return types but no complete outcomes, bounds or drop semantics: P2-1.

There is no CLI help for this proposed API. Missing CLI commands are therefore not findings. The missing API lifecycle/default semantics are part of the blocking roots above.

### Dependency direction is viable

Neither library needs a reverse dependency on core, Git or the other library. A small composition adapter can depend on both APIs and translate owned values. The existing native Port/Session seam supports this route.

The draft does not authorize a new contract crate or repository creation. The selected separate Credential repository is an operator-backed proposal, not a demonstrated prerequisite for fixing every observed coupling.

### Placement and secrets remain correctly constrained

The text agrees with placement P1–P9: providers execute at the selected endpoint; driver credentials and signing authority do not become core capabilities; paths remain endpoint-local; unavailable selected sources refuse without endpoint/identity fallback.

It correctly distinguishes required protocol output sent to the server from prohibited disclosure through application/control messages or logs. Server trust and physical channel verification remain network-adapter responsibilities.

### Current authentication policy is preserved in principle

The draft says existing providers/refusals remain unchanged and acknowledges later helper/native policy rather than reviving gh-only restrictions. The controlling Windows policy remains logon credentials for Negotiate/NTLM, helper credentials for Basic, no downgrade after rejected logon identity, Digest refusal as specified and no challenged-POST replay.

A future mapping must demonstrate that policy explicitly; the draft does not itself authorize a broader authentication matrix.

### Accepted Connection invariants remain intact

This object does not reopen Connection’s accepted stream algorithm. Its neutral start seam, synchronous application-before-Applied callback, no-effect WouldBlock, terminal fencing, acquired-permit revocation and final CleanupDomain retention remain controlling.

Credential must join those lifetimes through explicit ports; it cannot create a second host, reorder protocol or replacement cleanup ledger. The present findings identify where that composition remains unwritten, not a defect requiring changes to the accepted stream algorithm.

### Cheap tests are a valid goal, not yet an established consequence

The proposed test layers distinguish portable behavior, provider qualification, small composition and actual placement/client qualification. That is appropriate.

The source supports localized seams and fake substitution. It does not establish that a new repository alone produces the desired build/test isolation. Completing the concrete mapping and change-impact comparison is necessary to assess that claim; no runtime benchmark or complete implementation is required at this gate.

## 3. Risks and next action

Native launch safety, actual worker cleanup, endpoint trust qualification and installed-client adoption remain later risks. This review neither qualifies them nor authorizes their implementation.

The next action is one bounded Credential-document remediation covering P2-1 and P2-2: settle lifecycle/receipt ownership and the concrete operation/provider mapping, with exact proposed Connection amendments and portable closure traces. Keep the accepted Connection contract and all source repositories unchanged during this design correction.

