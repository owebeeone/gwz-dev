# GWZ SSPI native worker — Surface-AXIS REVIEW

**Review object:** `gwz-sspi` caller-facing `docs/WorkerEntry.md`, `README.md`, `docs/CallerValues.md`, `docs/Supervision.md` and `docs/NativeFixtures.md` at `610964282663c3b7844c9620d063e40d9ee76258`. Shared worker entry and Windows Negotiate/NTLM processing are documented as implemented pending review; installation, qualification and release remain gated.

**Baseline:**

| Repository | Committed HEAD |
|---|---|
| gwz-dev | `40fd121fbc2d9e6e727ed1a1b2501275200a1797` |
| gwz-sspi | `610964282663c3b7844c9620d063e40d9ee76258` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-core-evidence | `57d132b823d405bf91b698c74aaaf75b9bee5980` |

Caller documents were read using `git show HEAD:<path>` with numbered lines. All four HEADs matched the prompt at initial verification and final recheck.

**Date:** 2026-10-03

**Axis:** Surface — cold caller discoverability, names, defaults, lifecycle pairs and first-use/undo walkthrough. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1 or P2 findings; two nonblocking P3 documentation findings. This verdict covers the bounded caller surface, not deferred installation or native qualification outcomes.

---

## 0. Evidence base

Read the following committed caller pages in full:

| Page | Lines | Purpose |
|---|---:|---|
| `README.md` | 1–57 | Capability description, status and caller-document navigation |
| `docs/WorkerEntry.md` | 1–92 | Bootstrap recognition, early dispatch, metadata refusal, native lifecycle and Digest refusal |
| `docs/CallerValues.md` | 1–113 | Owned request/token values, secret disposal, validation and construction recipe |
| `docs/Supervision.md` | 1–144 | Parent configuration, defaults, deadlines, cancellation, cleanup and caller recipes |
| `docs/NativeFixtures.md` | 1–53 | First native fixture run, synthetic provenance and qualification disclosures |

Process inputs inspected were the canonical Surface prompt, workspace `AGENTS_GWZ.md`, member `AGENTS.md`, `EVIDENCE.md`, `dev-docs/AgentProcessRules.md`—particularly L1-17 through L1-24 and its review/report procedures—and `dev-docs/GwzProcessOptimization.md`.

Inspection commands comprised `cat`, `rg`, numbered `sed` reads, `git show`, a caller-page-only `git diff`, and `git rev-parse HEAD` / `git status --short` for the four repositories. The caller-page diff was empty. The initial literal `evidence` path did not exist; `EVIDENCE.md` identified the actual member as `gwz-core-evidence`, whose HEAD then matched the supplied tuple.

At both checks, `gwz-sspi` was clean. Root, core and evidence contained only the prompt-declared out-of-scope untracked items. Their status lists were unchanged at final verification.

No implementation, design, plan, owner checkpoint or current peer report was read. No build, test, native worker execution, helper invocation or Git mutation occurred. This is a Rust caller API and private bootstrap surface; the permitted commands excluded executable/help runs. The walkthrough therefore used the named caller pages alone.

## 1. Findings

### [P3-1] CallerValues still claims that the implemented native worker is deferred and refuses every call

**Location:** `docs/CallerValues.md:3–6`, especially “Native SSPI and worker_entry remain deferred. The packaged worker refuses every call.” Contradictory current descriptions appear in `docs/Supervision.md:3–5,61–64`, `docs/WorkerEntry.md:48–69` and `README.md:7,54–55`.

**Violated invariant:** Caller documentation must consistently distinguish implemented capability from deferred qualification and installation. A cold reader must be able to determine whether the documented first-token path is available under its stated prerequisites.

**Reproduction:** Follow the README’s caller-values link before implementing the request recipe. The opening says native processing and `worker_entry` are deferred and the packaged worker refuses every call. Follow its link to Supervision; that page instead says shared entry and Negotiate/NTLM processing are implemented and that a matching trusted worker can produce the initial native token. WorkerEntry describes metadata-dependent refusal and native processing. The reader must choose which status statement to believe.

**Impact:** A downstream caller can incorrectly conclude that the documented native use path is wholly unavailable, or spend time resolving an apparent contradiction. The correction is confined to documentation and requires no compatibility change.

**Required correction:** Replace the stale opening with the current bounded status: caller values are inert by themselves; parent supervision and shared Negotiate/NTLM processing are implemented pending their gates; missing or invalid trusted metadata causes refusal; installation/composition and full qualification remain deferred; Digest remains unavailable.

**Closure/regression test:** Inspect the corrected committed caller pages together and verify that they describe the same availability boundary. Add an assertion to the existing documentation checks, if applicable, preventing the obsolete unconditional “packaged worker refuses every call” claim from returning.

### [P3-2] The native fixture setup has no documented teardown or environment restoration

**Location:** `docs/NativeFixtures.md:9–19`. The recipe changes four process environment variables and creates a unique external fixture root containing build and scratch directories. The remainder of the page supplies no undo recipe.

**Violated invariant:** The Surface lifecycle rule requires every create/write operation to have a discoverable removal or undo path documented together.

**Reproduction:** In an existing PowerShell session, follow the documented recipe through its two test commands. Then attempt to undo the fixture setup using that page alone. There is no command to restore previous values—or original absence—of `GWZ_SSPI_BUILD_FINGERPRINT`, `CARGO_TARGET_DIR`, `GWZ_SSPI_NATIVE_SCRATCH` and `GWZ_SSPI_NATIVE_WORKER`, nor guidance for disposing of the created fixture root.

**Impact:** Subsequent commands in the same session inherit synthetic packaging metadata and fixture-specific paths. Repeated runs also leave external build/scratch trees behind. The user must invent cleanup behavior, including how to preserve pre-existing environment settings.

**Required correction:** Document setup and teardown together. Preserve and restore prior environment values, preferably through a bounded `try`/`finally` recipe, and provide a removal path for the uniquely created fixture root after required evidence has been retained. Cover unsuccessful commands as well as the successful path.

**Closure/regression test:** On Windows, exercise the revised recipe with variables initially absent and with distinct pre-existing values, including a deliberately failing command. Verify restoration of all four variables and the documented disposition of only the newly created fixture root. This test was not executed during this read-only review.

## 2. Invariant analysis

The following attacks did not produce blocking findings:

- **API placement and names:** Worker bootstrap/entry, caller values and parent supervision have distinct linked pages. `WorkerExecutable`, `Supervisor`, `Conversation`, `Deadline` and `Cancellation` communicate their roles without requiring a design document. No new public CLI family is presented as part of this bounded object.
- **Trusted fingerprint construction:** Supervision identifies a host-owned packaging field, exact absolute worker path and identical expected/worker bytes. WorkerEntry rejects runtime hashes, Cargo versions, environment-selected executables and library defaults as substitutes. Synthetic `[0x42;32]` examples are explicitly labelled as fixtures.
- **Ordinary invocation recovery:** `WorkerBootstrap::from_args` excludes argv[0], returns an owned optional result and distinguishes ordinary invocation. Its recipe explicitly tells the host to reopen argv for ordinary parsing.
- **Early dispatch and exit:** WorkerEntry places dispatch before ordinary parsing, Git, logging and runtime initialization. The recipe distinguishes ordinary continuation from the worker branch where the host exits. Standalone metadata refusal, normal/error exit statuses and silent output behavior are stated.
- **Defaults and bounds:** `Options::max_workers` has a stated default of eight and accepted range 1–64. Deadline and TokenLimit explicitly have no default. The operation deadline is immutable across all eight rounds; shutdown has a separate explicit deadline.
- **Conversation lifecycle:** `start` has named `finish` and `cancel` counterparts. Finish is documented before Begin, between rounds and after Complete. The first-token example awaits Finish before returning its owned token. Drop cancellation and retained supervision are also described.
- **Cleanup interpretation:** Pending, Confirmed and Unknown are exposed. The text distinguishes cancellation, EOF, Finished and Job killing from the stronger proof required for Confirmed; it also documents retained capacity and tombstone eviction.
- **Token versus remote success:** Complete means native token generation, and the example leaves HTTP/mechanism acceptance to the host. Native fixtures explicitly disclaim remote NTLM/Kerberos completion, EPA/TLS fidelity and HTTP/Git success.
- **Digest shape:** Digest remains a named caller package with an explicit ProviderRejected outcome. WorkerEntry explains the missing H(Entity) request input and prohibits substituting URI or assuming an empty body. No deferred Digest outcome was treated as a finding.
- **Qualification and provenance:** Fixture credentials, binding and build metadata are labelled synthetic. Local tokens are not sent to a network. Job containment is not represented as physical wiping or external-provider cancellation.

The cold walkthrough could discover request construction and disposal, trusted worker configuration, first-token use and conversation cleanup. The two remaining points requiring a guess are the contradictory availability statement and fixture teardown.

## 3. Risks and next action

This documentation-only review establishes discoverability and consistency within the five named pages. It does not verify runtime behavior, executable help, installed artifact provenance or deferred platform/provider qualification.

The next action is to correct the two P3 documentation defects before using these pages as the downstream integration handoff. No interface redesign or compatibility break is indicated.
