# GWZ SSPI native worker — Code-AXIS REVIEW

**Review object:** Native implementation checkpoint controlled by `dev-docs/GwzSspiNativeCheckpoint.md` at root `40fd121fbc2d9e6e727ed1a1b2501275200a1797`, dated 2026-10-03, implementation in progress and not accepted. Member diff `aec1b9c65b75ad53dd3ae780fc1e04af18de795c..610964282663c3b7844c9620d063e40d9ee76258`; root diff `e220886f0641e4d3e5e67443171808666e191ff7..40fd121fbc2d9e6e727ed1a1b2501275200a1797`; bounded native evidence campaign.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| gwz-dev | `40fd121fbc2d9e6e727ed1a1b2501275200a1797` |
| gwz-sspi | `610964282663c3b7844c9620d063e40d9ee76258` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-core-evidence | `57d132b823d405bf91b698c74aaaf75b9bee5980` |

The four HEADs and statuses were checked at the beginning and end and remained unchanged. The member had no reported changes. Root, reference core and evidence had only the identified out-of-scope untracked material. Root controlling documents were inspected with `git show HEAD:` and direct reads; source was read from the tracked, unchanged working files.

**Date:** 2026-10-03

**Axis:** Architecture, interface contracts, call graphs, ownership, compatibility and error paths. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — No P0, P1 or P2 findings. Two P3 findings remain. This is bounded acceptance of the implemented worker, bootstrap and refusal behavior; it does not close the deferred qualification or composition work.

---

## 0. Evidence base

Read:

- Workspace `AGENTS_GWZ.md`, `EVIDENCE.md`, member `AGENTS.md` and evidence-member `AGENTS.md`.
- `dev-docs/AgentProcessRules.md`, its controlling amendment `GwzProcessOptimization.md`, `GwzSspiNativeCheckpoint.md`, accepted `GwzSspiDesign.md` revision 2 including §8, and `GwzSspiPlan.md`, especially step 3.
- Member `docs/Architecture.md`, `docs/WorkerEntry.md`, `docs/WireProtocol.md`, `docs/Testing.md`, `docs/NativeFixtures.md`, `Cargo.toml` and `.github/workflows/ci.yml`.
- Worker production code: `session.rs:1–208`, `bootstrap.rs:1–57`, `framing.rs:1–54`, `storage.rs:1–67`, `native.rs:1–21`, `platform.rs:1–14` and `mod.rs:1–17`.
- Windows worker code: `conversation.rs:1–277`, `owners.rs:1–152`, `observation.rs:1–60`, `provider.rs:1–39`, and worker platform entry.
- Protocol projections: `worker_codec.rs:1–221`, retained parent `supervision.rs:1–186`, and `profile.rs:1–262`.
- Public values and secret storage; minimal executable `gwz-sspi-worker.rs:1–45`; public bootstrap/entry tests.
- Parent factoring and inherited-pipe paths: `windows/inherited.rs:1–55`, `windows/launch.rs:90–233`, `windows/identity.rs:1–172`, changed handle I/O behavior, platform exports, and parent `io.rs:1–178`.
- Worker fake tests and support, including status/completion, cap, mechanism, phase, partial I/O, wipe and seeded schedule cases.
- Native ownership fixture `native_owner_tests.rs:1–54`, native process fixtures `native_tests.rs:1–194`, and public Supervisor fixture `tests/native/windows_worker.rs:1–91`.
- Reference Windows parity design §8.
- Private campaign `2026-10-03-sspi-native-bec3ba15`: README, candidate-source receipt, native Supervisor and native-owner outputs, native-owner receipt, and final inventory output.

Read-only diff inspection confirmed no changes to `protocol/` or `src/protocol/generated.rs` in the member review range. The root diff changes checkpoint/report metadata and managed member-lock metadata rather than adding host composition.

Executed only the expressly permitted prebuilt pure test command:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/native/debug/deps/gwz_sspi-e95571679a8a0127 worker::tests
```

It exited 0; all selected tests passed and no ignored tests ran. This is execution of the permitted prebuilt Darwin binary, not an independent rebuild or Windows native execution. No writes, Git mutations, builds, compiler probes, helpers or native workers were run.

## 1. Findings

### [P3-1] Testing documentation retains an obsolete unconditional worker-refusal claim

**Location:** `gwz-sspi/docs/Testing.md:87–96`, particularly lines 93–96.

**Violated invariant:** The current testing guide must distinguish implemented behavior, executed evidence and remaining qualification accurately.

**Reproduction:** Read the “Parent supervision checkpoint” section. It says native provider buffers/disposal remain later gates and that the production worker bootstrap “continues its fixed refusal.” The same document at lines 39–42 and 137–182 describes the implemented serial native entry and opt-in native fixtures. The actual executable accepts valid compile-time metadata and calls `worker_entry`; the archived Supervisor fixture exercised primary Hello, NTLM first tokens and normal Finish.

**Impact:** A reader following the older section receives an incorrect description of the current executable and test boundary. The section has no explicit historical status or supersession notice, so the reader must resolve contradictory assertions.

**Required correction:** Mark the older section’s evidence statements as historical and superseded, or replace the stale statements with the current bounded native status and a reference to `NativeFixtures.md`. Preserve the genuine qualification deferrals.

**Closure check:** Read the complete testing guide after the correction and confirm that every remaining refusal statement is conditional on missing/malformed metadata, invalid bootstrap, unsupported platform, or explicitly unavailable Digest. No product-code test is needed for this documentation correction.

### [P3-2] Native fixtures bypass the implemented Negotiate observation query

**Location:** `gwz-sspi/src/worker/windows/native_owner_tests.rs:10–42`; `tests/native/windows_worker.rs:27–45,66–90`; production query at `src/worker/windows/observation.rs:23–59`.

**Violated invariant:** Accepted design §3 requires production native tests to verify mechanism observation for each supported provider. Negotiate’s observation has a distinct native query/allocation path.

**Reproduction:** Both native authentication fixtures construct `Package::Ntlm`. In `Conversation::observation`, NTLM returns its known mechanism directly at `conversation.rs:255–258`; only Negotiate calls `observation::query`. The portable mechanism matrix uses `FakeSession::observation`, which returns a supplied enum at `test_support.rs:159–162`. Consequently, neither the executed native fixtures nor the pure mechanism tests call the production `QueryContextAttributesW` negotiation-info path.

**Impact:** The native query, package-name interpretation and corresponding query-allocation disposal lack execution evidence. Passing NTLM owner fixtures cannot detect a defect specific to that path. This is a bounded coverage gap, not evidence that its implementation is incorrect.

**Required correction:** Add an opt-in public native fixture that performs an initial Negotiate operation and exercises the production observation query and its allocation owner. Keep the test local, use synthetic inputs, and allow the documented provisional/unresolved intermediate outcome. Record its native result separately.

**Closure test:** Run that fixture on the pinned Windows snapshot and verify the query’s intermediate observation and successful disposal of any returned allocation. The test must call the real query rather than supply a fake observation. Completed remote NTLM/Kerberos authentication and authoritative final selection remain separate qualification work.

## 2. Invariant analysis

**Frozen wire and retained parent agreement held.** The implementation adds worker projections over the existing generated borrowed records. It does not change schema, generated codec or contract fingerprint artifacts. Parent Begin/Challenge/Finish encodings and worker Hello/Token/Finished encodings use the same profile. Error.phase maps to the worker’s active Begin, Challenge or Finish phase and agrees with the retained parent decoder.

**Admission precedes credential work.** The worker parses borrowed records, checks envelope/profile and permitted phase/kind, queries package maximum, and applies the provider intersection before constructing owned request copies or acquiring credentials. Challenge bounds and correlation are checked before another Initialize call. Output is checked against the same intersection before publication.

**Serial state transitions held.** Legal pre-Begin Finish avoids provider work. Begin performs only the first Initialize; later initialization requires the expected Challenge round. Complete permits Finish and rejects further challenges. Round-eight Continue refuses publication; round-eight Complete is legal. Malformed, partial and I/O failures terminate the serial session and attempt normal cleanup.

**Native ownership and normal disposal held under the native contracts.** Explicit Unicode fields and their identity record have stable boxed ownership. CurrentLogon passes NULL authentication data. Target and aligned CBT allocations survive native processing and handle cleanup. Input tokens are copied into mutable fixed storage before native use. Provider output remains owned through CompleteAuthToken; its original extent survives an accepted length reduction. Normal output is copied, wiped and checked-free before a Token is written. Context deletion precedes credential free; initialized handle disposal is attempted once, and recorded failure prevents Finished.

**Mechanism publication held by inspection.** Direct NTLM reports its requested provider. Negotiate queries actual negotiation info, frees the returned allocation, and treats only a known package with COMPLETE negotiation state as authoritative. The serial bridge rejects unresolved or provisional Complete publication and incompatible mechanisms. P3-2 limits the execution evidence for the native query.

**Bootstrap and parent factoring held.** The minimal executable requires exact compile-time fingerprint metadata before pipe ownership or Hello. Bootstrap accepts only the exact marker and two distinct nonzero decimal handles. Both endpoints validate before adoption; inheritance is removed and actual child primary identity is queried. The new owned-launch factoring retains the existing suspended creation, Job/HANDLE attributes, immediate child-end closure and membership check. Parent broken-pipe reads become EOF without making EOF a completion proof.

**Secret/API/package boundaries held.** Public bootstrap and secret-bearing values expose no Clone/Debug or native handles. Errors contain fixed classifications and optional numeric status. Production paths introduce no payload logging, core dependency, runtime dependency or public fake-provider injection. Windows bindings stay within the approved feature additions. Public fixtures and CI remain self-contained; ordinary CI does not require private evidence.

**Digest refusal is honest.** A valid Digest Begin undergoes profile/cap validation and then refuses before Acquire/Initialize. No H(Entity) fabrication, wire amendment or success-shaped substitute was introduced.

## 3. Risks and next action

The reviewed evidence establishes local first-token NTLM operation, normal owner cleanup and the named process/EOF/parent-loss fixtures. It does not establish completed remote authentication, native CompleteAuthToken reachability, TLS/EPA provenance, blocked-provider cancellation, descendants, installed artifact provenance or full Windows parity. Those remain explicit deferrals.

The pure tests exercise the production serial bridge but substitute private native ports; they do not execute Windows FFI ownership behavior. The native fixtures exercise normal NTLM allocation and handle disposal, not every provider error or completion-buffer mutation.

**Next action:** The lane owner may record this Code-axis GO for the bounded checkpoint, retain the two P3 items, and proceed through the remaining recorded review gates. This report authorizes no activation, publication or closure of full plan step 3.
