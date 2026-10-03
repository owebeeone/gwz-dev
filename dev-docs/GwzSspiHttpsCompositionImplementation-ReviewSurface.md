# GwzSspiHttpsCompositionImplementation — Surface-AXIS REVIEW

**Review object:** Cold caller surface comprising `dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md`, `gwz-sspi/docs/Supervision.md`, `gwz-sspi/docs/CallerValues.md`, `gwz-core/docs/TransportPlacement.md`, and the README files of `gwz-transport`, `gwz-cli` and `gwz-py`, at the exact tuple below. Caller contract dated 2026-10-04; implementation review pending; Windows activation and runtime qualification remain deferred.

**Baseline:**

| Repository | HEAD |
|---|---|
| root | `b72dccf813816f41a508eb0fc2f9b2f071b323a0` |
| gwz-core | `efdd0a2cf66be889364466c7d69c97cc2736c278` |
| gwz-sspi | `58cc87c99bca21874a63d0ba11209b75a3e0c50a` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` |

Caller documents were read using `git show HEAD:<path>`, with numbered output. All six HEADs matched the required tuple at both start and end.

**Date:** 2026-10-04

**Axis:** Surface — adversarial cold caller review of API discovery, names, defaults, ownership, lifecycle pairs and a documented first-use/disposal walkthrough. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1 or P2 findings; one nonblocking P3 documentation finding. This verdict covers the listed caller surface only.

---

## 0. Evidence base

Read the generated review prompt and workspace instructions:

- Root `AGENTS.md` and `AGENTS_GWZ.md`.
- Member `AGENTS.md` files for gwz-core, gwz-sspi, gwz-transport and gwz-cli.
- Checked the corresponding root/member docs directories and root `dev-docs` for additional `AGENTS.md`/`AGENTS_GWZ.md`; none were present at the checked locations.
- Process authority, specifically `AgentProcessRules.md` L1-17 through L1-24, including the Surface amendment and severity contract.
- `GwzProcessOptimization.md`, lines 1–206, including the controlling review-granularity amendment.

Read the complete caller-document object:

| Document | Lines |
|---|---:|
| `dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md` | 1–170 |
| `gwz-sspi/docs/Supervision.md` | 1–192 |
| `gwz-sspi/docs/CallerValues.md` | 1–125 |
| `gwz-core/docs/TransportPlacement.md` | 1–363 |
| `gwz-transport/README.md` | 1–502 |
| `gwz-cli/README.md` | 1–84 |
| `gwz-py/README.md` | 1–228 |

Commands comprised:

- `git rev-parse HEAD` in each of the six repositories, twice; all twelve checks matched.
- `git show HEAD:<listed-document> | nl -ba`, with `sed` windows where needed.
- Targeted `rg -n` searches of the caller guide, Supervision and CallerValues for cancellation, receipts, shutdown, cleanup status and signatures.
- Read-only instruction/process-document reads.

One attempted `git show HEAD:docs/Supervision.md` ran in the root repository and failed because that path does not exist there. The document was successfully read and searched in gwz-sspi.

No code, source diffs, design documents, implementation checkpoints or peer reports were read. No network, builds, tests, writes or Git mutations were performed. References to compiled examples and tests below describe documentation claims; this review did not execute them.

## 1. Findings

### [P3-1] Receipt-producing teardown is documented without complete callable signatures

**Location:** `gwz-sspi/docs/Supervision.md`, lines 14–20 and 35–60; `dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md`, lines 143–170.

**Root cause:** The public lifecycle table abbreviates teardown methods and report structures without supplying the complete signatures or a receipt-processing example. It lists `Conversation::cancel(self)`, `Supervisor::cleanup_status(RecordId)` and `shutdown(separate_deadline)`, but omits their precise return types and the record argument’s ownership form. `CancellationReceipt` has “record_id and cleanup fields”; `ShutdownReport` has a “confirmed lifetime count and outstanding Vec<RecordId>”, without naming the confirmed-count field or its type.

**Violated invariant:** A cold caller must be able to execute the explicit cancellation/shutdown half of the documented lifecycle and inspect cleanup evidence from the permitted caller documents alone.

**Reproduction:** Follow the documented capture/start/first-token recipe. Cancel the resulting Conversation explicitly, retain its receipt, query subsequent cleanup using its record, then await shutdown and record both confirmed and outstanding work. The documents identify those operations and explain Pending/Unknown correctly, but do not let the caller determine the exact cancellation result, whether cancellation itself must be awaited, or the complete expressions needed to inspect the returned reports. Neither provided Supervision recipe exercises receipt inspection; the shutdown handoff recipe only passes the future to a generic ownership checker.

**Impact:** The caller must guess signatures/field names or consult an additional API source before implementing explicit cleanup accounting. The documented Drop path remains usable and supervision retains resources, so this is a bounded documentation defect rather than evidence of abandoned cleanup or an incompatible API.

**Required correction:** Add the exact teardown signatures and report field declarations, or a self-contained caller recipe demonstrating explicit cancel, receipt inspection, subsequent `cleanup_status`, and bounded shutdown report inspection. State which calls are synchronous and which return futures.

**Closure/regression test:** Compile that exact documentation recipe as a downstream caller fixture. Independently repeat the cold walkthrough using only the revised listed docs and verify that no teardown signature or report field requires guessing.

## 2. Invariant analysis

The concrete first-use walkthrough was:

1. **Construct trusted worker and Supervisor.** Supervision lines 90–117 identify the absolute path and exact 32-byte trusted packaging fingerprint, show `WorkerExecutable::new`, and then `Supervisor::new`. Options has one field, `max_workers`, default eight and checked range 1–64. There is no caller-selected fingerprint fallback or PATH discovery. Production packaging qualification remains explicitly separate.

2. **Capture on the original entry thread.** Caller guide lines 33–52 and 97–107 say to capture before fanout or executor handoff, preserve the Result as native availability, and share a successful capture through `Arc`. Capture has no worker permit and no native-handle getters. Send/Sync does not authorize substituting another thread’s identity.

3. **Construct an owned Start before host submission.** Caller guide lines 119–135 give the complete `start_captured` recipe. Supervision lines 172–182 construct Start outside the async block. The future owns its request and origin reference and borrows neither Supervisor nor CallerCapture. Dropping the caller capture after this construction does not invalidate Start; dropping Supervisor initiates cancellation.

4. **Preserve one deadline and availability result.** Caller guide lines 79–93 distinguish the absolute positive logical Open deadline from active HTTP I/O accounting. Discovery, helper work, capacity, launch, Hello and rounds retain the timestamp. Zero refuses native selection; an expired positive deadline produces Timeout. Anonymous, configured Basic and SSH can proceed despite native setup refusal.

5. **Produce and consume tokens.** `step(None)` begins only after verified Hello; later steps require owned nonempty challenges. The docs cap the exchange at eight rounds and prohibit another step after Complete. Nonempty initial Negotiate/NTLM offers and Digest are explicitly refused before Begin. CallerValues supplies owned request construction and separate wiping of borrowed constructor sources.

6. **Validate remote acceptance.** Caller guide lines 136–141 and TransportPlacement lines 348–361 require the same exclusive origin/connection generation and independent remote acceptance. Native Complete and authoritative Continue are not HTTP authentication success. Replacement/loss refuses before service POST; challenged or completed POSTs must not be replayed for authentication.

7. **Finish, cancel and shut down.** The successful first-token recipe demonstrates awaited `finish`. Drop cancellation and retained supervised cleanup are stated. Explicit receipt-producing cancellation and report inspection require the guesses recorded in P3-1.

The following attacks did not establish defects:

- **Foreign or closed capture:** The guide explicitly classifies foreign capture as pre-registration InvalidRequest and closed capture/admission as Closed. A refused Start does not dispose the caller-owned capture.
- **Exited or changed original thread:** Launch rechecks liveness, impersonation and primary identity. Moving or sharing the capture does not replace those checks.
- **Capacity escape through fanout or Drop:** Concurrent Starts share the issuing Supervisor’s capacity; pending/quarantined work retains permits. Capture disposal cannot release another conversation’s permit.
- **False cleanup confirmation:** Pending and Unknown are distinguished from Confirmed. EOF, Finished, process kill and advisory I/O cancellation are insufficient proof. Tombstone eviction and foreign IDs produce Unknown; the endpoint documentation keeps Unknown charged.
- **Late success after cancellation/expiry:** Supervision describes publication revocation, retained first-terminal cause and ready-result rechecking.
- **Opaque error leakage:** CallerValues lists fixed ErrorKind classifications and restricts optional native status to ProviderRejected. The caller guide supplies capture/Start refusal classifications without credential strings.
- **Missing lifecycle partner:** Capture has reference release through Drop; Start has cancellation on Drop; Conversation has finish/cancel/Drop; Supervisor has bounded shutdown and Drop cancellation. The explicit call details need P3-1’s documentation correction.
- **Activation overclaim:** CLI/Python README additions and the caller guide consistently describe candidate composition and deferred Windows activation/provider qualification. The general installation commands do not claim native HTTPS activation.
- **Defaults:** Worker capacity has a stated default/range; Deadline and TokenLimit explicitly have no default. No new public option or command family is introduced.

## 3. Risks and next action

This review establishes documentation-level coherence, not implementation fidelity or executed native authentication. Windows activation, provider/EPA/CBT/live identity qualification, Digest, the broader platform/package matrix and release publication remain outside this verdict.

The next action is to add the exact receipt-producing teardown signatures and a compiled cleanup-accounting recipe for P3-1 while preserving the documented ownership and Pending/Unknown rules. All six repository HEADs remained unchanged through the final check.
