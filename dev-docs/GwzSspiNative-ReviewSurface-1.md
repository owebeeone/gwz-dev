# GWZ SSPI native worker remediation 1 — Surface-AXIS REVIEW

**Review object:** Caller documentation at `gwz-sspi` `425e13dc011c42e94fdea31779a8e5967aedc82b`, focused on remediation of Surface P3-1 and P3-2. Native implementation review, installation, qualification and release remain separate gates.

**Baseline:**

| Repository | Corrected HEAD |
|---|---|
| gwz-dev | `bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e` |
| gwz-sspi | `425e13dc011c42e94fdea31779a8e5967aedc82b` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-core-evidence | `9adf06beab7a15d1e5c22ec1cecf8e35966e1d7c` |

Sources were read with `git show HEAD:<path>` and numbered lines. All four HEADs matched the prompt at the start and end.

**Date:** 2026-10-03

**Axis:** Surface — caller discoverability, names, defaults, lifecycle pairs and first-use/undo walkthrough. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2 or P3 findings. Both original Surface findings are closed for the reviewed documentation surface.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P3-1 | Replace stale unconditional worker refusal with current bounded availability | `CallerValues.md:3–9` now states that values are inert until admitted, matching trusted metadata enables Windows Negotiate/NTLM, malformed metadata/bootstrap refuses, Digest remains refused and qualification is separate. This agrees with README, Supervision, WorkerEntry and the corrected Testing status. | CLOSED |
| P3-2 | Restore four environment values, including absence, and document owned-root teardown on success/failure after evidence retention | `NativeFixtures.md:10–61` saves all four process environment values before mutation, restores them in `finally`, explicitly routes nonzero Cargo results into that block, records fresh-root ownership only after successful creation, and supplies guarded removal both within `finally` and as an explicit later teardown. | CLOSED |

The owner supplied a report that extracted, unchanged restoration/ownership logic was exercised with harmless command substitutions across absent/prior values × success/failure, with restoration and root removal in all four cases. I did not execute that exercise or inspect its raw receipts. Closure here rests on independent inspection of the corrected caller recipe and retracing the original documentation counterexamples.

## Changed-range analysis

The permitted caller-document diff from `610964282663c3b7844c9620d063e40d9ee76258` to `425e13dc011c42e94fdea31779a8e5967aedc82b` changes three pages:

- **CallerValues:** Corrects availability and clarifies that value constructors do not convert to UTF-16, while the native worker has separate zeroizing UTF-16 owners.
- **NativeFixtures:** Adds environment restoration, explicit Cargo failure handling, fresh-root ownership and paired teardown. Additional prose describes strengthened fixture claims and local initial Negotiate observation.
- **Testing:** Replaces the obsolete fixed-refusal status and describes the strengthened fixture evidence boundaries.

README, Supervision and WorkerEntry are unchanged.

The availability and teardown changes directly resolve the two Surface dispositions. The additional fixture prose exceeds those two narrow documentation corrections but remains within the prompt’s caller-facing honesty and qualification scope. It explicitly distinguishes suspended-child containment observations from earlier EOF-confounded receipts, and local initial Negotiate from completed remote authentication.

No reviewed change alters public API names, defaults, bootstrap syntax, production Job policy or packaging authority. No **NEW ARCHITECTURAL** root cause was found. This conclusion concerns the documented surface; implementation and raw proof changes were not inspected.

## 0. Evidence base

Read these corrected committed pages in full:

| Page | Lines |
|---|---:|
| `README.md` | 1–57 |
| `docs/CallerValues.md` | 1–117 |
| `docs/Supervision.md` | 1–144 |
| `docs/WorkerEntry.md` | 1–92 |
| `docs/NativeFixtures.md` | 1–104 |

Also inspected `docs/Testing.md:80–108,160–210`, including corrected status, qualification limits and native fixture remediation prose, and my original filed `dev-docs/GwzSspiNative-ReviewSurface.md`.

Read the generated `dev-docs/GwzSspiNative-PromptSurface-1.md`. Previously read process instructions remain applicable: workspace/member agent instructions, `EVIDENCE.md`, AgentProcessRules and its adopted optimization amendment.

Commands used were read-only `cat`, `git show`, numbered `sed` reads, the explicitly scoped caller-document `git diff`, and `git rev-parse HEAD` / `git status --short` for all four repositories.

At both checks, `gwz-sspi` was clean. Root, core and evidence status lists were unchanged and contained the prompt-declared out-of-scope untracked items only. No peer prompt or report contents were read.

No implementation, design, plan, checkpoint, tests or private campaign receipts were read. No builds, test execution, native/helper execution, file writes or Git mutations occurred.

## 2. Invariant analysis

**Availability consistency:** Retracing README → CallerValues → Supervision → WorkerEntry no longer produces incompatible answers about whether native work exists. The pages distinguish inert values, implemented native entry, trusted metadata prerequisites, malformed-input refusal and deferred qualification. Testing’s former unconditional refusal statement is also corrected.

**Four-value restoration:** The recipe captures `GWZ_SSPI_BUILD_FINGERPRINT`, `CARGO_TARGET_DIR`, `GWZ_SSPI_NATIVE_SCRATCH` and `GWZ_SSPI_NATIVE_WORKER` before mutation. Both success and exceptions traverse `finally`; restoration uses the saved process values, with null explicitly restoring absence. Each Cargo command checks its exit status and throws on failure.

**Creation/removal pairing:** The fixture root is created without `-Force` and becomes owned only after creation succeeds. Removal uses its exact literal path and requires both ownership and retained-evidence flags. Failure before acquiring the root cannot authorize its removal. Success or failure without evidence retention preserves the root and tells the caller its path; the adjacent commented teardown provides the subsequent guarded removal operation. The caller no longer has to invent an undo path.

**Cold first-use walkthrough:** The documentation still supplies request construction and source disposal, trusted worker configuration, early bootstrap dispatch, first-token use and awaited Finish. Installation/composition remains explicitly deferred. The fixture walkthrough now supplies setup, failure handling, restoration, retention and teardown together.

**Defaults and lifecycle:** The unchanged supervision page states default capacity eight, accepted range 1–64, no default Deadline, an immutable operation deadline and a separate shutdown deadline. Start remains paired with Finish/Cancel and documented Drop cancellation.

**Trust and success interpretation:** Synthetic fingerprints, bindings and credentials remain labelled. Token Complete remains native token generation, with HTTP acceptance left to the host. New fixture prose does not claim completed Kerberos/NTLM, TLS/EPA, installed provenance or full Windows qualification. Digest remains expressly unavailable pending its missing request input.

No new findings resulted from these attacks.

## 3. Risks and next action

This review verifies the corrected documentation and the discoverability of its undo paths. It does not independently execute PowerShell, validate native cleanup receipts or establish deferred qualification.

The next action is to accept the Surface remediation at this exact tuple and carry this GO into the owner’s aggregate review decision.
