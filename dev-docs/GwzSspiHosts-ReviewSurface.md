# GWZ SSPI installed hosts4a — Surface-AXIS REVIEW

**Review object:** Installed-host packaging caller surface at the five-repository tuple below. Packaging and callable descriptors are implemented pending review; HTTP composition, Windows activation, installed native qualification and publication remain separate gates.

**Baseline:**

| Repository | Committed HEAD |
|---|---|
| gwz-dev | `8cb284391dc5864c9ba6f41324cdd3c5807cf765` |
| gwz-sspi | `e31b17e95defd3468140e9b5f73fef8591766599` |
| gwz-cli | `061385fc009fd173488dd07e18d5b2d5882de05d` |
| gwz-py | `e9e228c0006d7f8b3d44c9657403dec5905c6e90` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Caller documents were read using `git show HEAD:<path>` with numbered lines. All five HEADs matched the canonical prompt at the start and end.

**Date:** 2026-10-04

**Axis:** Surface — cold caller discoverability, names, defaults, lifecycle pairs and build/install/use/upgrade/remove walkthrough. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1 or P2 findings; one nonblocking P3 documentation finding. This accepts the bounded caller shape only.

---

## 0. Evidence base

Read the complete permitted committed caller documents:

| Document | Lines | Evidence examined |
|---|---:|---|
| `gwz-sspi/README.md` | 1–59 | Status, capability and navigation |
| `gwz-sspi/docs/HostPackaging.md` | 1–52 | Artifact-set inputs, packaging APIs, refusal and qualification boundaries |
| `gwz-sspi/docs/WorkerEntry.md` | 1–93 | Early dispatch, compiled fingerprint, worker lifecycle and Digest refusal |
| `gwz-cli/docs/HostPackaging.md` | 1–43 | Explicit CLI build, self-execution descriptor, artifacts and lifecycle |
| `gwz-py/docs/HostPackaging.md` | 1–61 | Wheel/backend provisioning, editable refusal, loaded-image descriptor and lifecycle |
| `gwz-py/RELEASE.md` | 1–214 | Distribution identity, release entry points, dependency prerequisite and recovery |
| `dev-docs/GwzSspiCallerGuide-DRAFT.md` | 1–145 | Host handoff, defaults, token-versus-HTTP distinction and conversation cleanup |

Also read the complete canonical `dev-docs/GwzSspiHosts-PromptSurface.md`. Previously established workspace process instructions remained applicable.

Commands were limited to `cat`, `git show`, numbered document reads, and `git rev-parse HEAD` / `git status --short` for the five repositories.

At both tuple checks, SSPI, CLI and Python were clean. Root and reference core contained only the prompt-declared out-of-scope untracked items; their status lists did not change.

No implementation, tests, design, plan, checkpoint, drafter narrative or current peer contents were read. No builds, installs, executable help, native execution, file writes or Git mutations occurred. The prompt restricted Surface to listed caller documents and Git identity/status, so the walkthrough used those documents rather than executing wrapper help.

## 1. Findings

### [P3-1] Packaging wrapper recipes omit the option and default contract

**Location:** `gwz-cli/docs/HostPackaging.md:10–16,37–38` and `gwz-py/docs/HostPackaging.md:18–24,59–60`.

**Violated invariant:** The Surface contract requires stated defaults and a first-day walkthrough that does not require guessing. A concrete invocation demonstrates selected values; it does not establish which options are required, what omission means or which choices are supported.

**Reproduction:** Read the CLI packaging page cold on a Windows or Linux packaging machine. Its only local command supplies `--target aarch64-apple-darwin`, `--profile dev` and `--target-dir`. The page gives no option reference identifying required arguments, default target/profile/output behavior or supported profile choices. Follow the Python page: its command supplies `--out` and delegated `--build-args='--profile dev --locked'`, but likewise does not state the default delegated build arguments or the required/optional status of the wrapper options.

A packager attempting the host-native default build, or adapting the examples to a release profile, must infer the wrappers’ contract from implementation or execution outside these caller pages.

**Impact:** Callers can select an unintended build profile or output location, or encounter avoidable invocation failures while adapting the documented recipes. This is a bounded documentation gap; correcting it requires no compatibility break.

**Required correction:** Add a compact option reference for both explicit packaging entry points. State each supported option, whether it is required, its default or absence behavior, supported profile/target forms, output layout and how delegated options are passed. Include a host-native invocation and explain the relationship between local development provisioning and release provisioning.

**Closure/regression test:** Compare the documented option reference against each wrapper’s actual argument/help contract, then perform a cold-document walkthrough for host-native defaults and one explicit target/profile/output selection. Verify that the resulting artifact location and provisioning status are predictable from the page alone. No such execution was performed in this review.

## 2. Invariant analysis

**Placement and names:** Provisioning lives under packaging documentation and explicit build entry points. The private worker marker is described as internal dispatch rather than a user authentication command. The new descriptors are named for worker selection, with explicit statements that they create no Supervisor or process.

**Trusted metadata:** The library page identifies the artifact-set producer and its input boundary. It distinguishes source-set identity from binary hashing and signatures. Host and worker compile matching expected bytes; diagnostic receipts and runtime variables confer no authority. Unsupported compiler overrides are described as refused.

**Ordinary builds and provisioning:** The CLI page distinguishes ordinary Python-free, unprovisioned Cargo builds from explicit provisioning. The Python page distinguishes provisioned wheel builds from unprovisioned editable installations. Neither promises that ordinary source development implicitly supplies a working worker.

**Missing and mismatched artifacts:** The library documents WorkerUnavailable for absent metadata and WorkerMismatch for malformed metadata. Python selection requires an absolute loaded-image path, fixed adjacency and regular nonsymlink worker files. The docs explicitly reject PATH, mutable module attributes, current-directory fallback and runtime fingerprint overrides. Worker Hello remains the subsequent compiled identity check.

**Install, upgrade and remove:** CLI instructions treat self-execution as part of the single trusted executable; installing/upgrading the whole artifact replaces the worker entry and removing that executable removes it. Python instructions keep the complete wheel and adjacent worker together, with removal through wheel RECORD alongside the extension and receipt. The root guide documents the same lifecycle relationship. No separate worker installation lacking a removal counterpart is introduced.

**Use once:** The CLI descriptor and Python callable descriptor are discoverable. Their documented use is selection/description, not HTTP authentication. The broader caller guide supplies the conversation sequence and cleanup obligations for later host integration. The reader is not told to treat a returned path or portable descriptor as endpoint success.

**Defaults and cleanup:** The root guide states default worker capacity eight, accepted range 1–64, required TokenLimit without a default, explicit operation and shutdown deadlines, and at most eight steps. Finish, cancel, shutdown and Drop behavior are documented together. The only default-contract gap found concerns the new packaging wrapper invocations.

**Release entry points:** Python release documentation identifies distribution `gwz`, console command `gwz-py`, release script and publish workflow. CLI packaging describes cargo-dist provisioning. Both disclose the unpublished exact SSPI registry dependency as a release prerequisite; local builds are not presented as bypassing that block.

**Deferred outcomes:** The documents consistently distinguish provisioned artifacts from Windows installed Hello execution, complete authentication, EPA, HTTP composition and activation. Digest refusal remains explicit. No deferred outcome was treated as a finding.

## 3. Risks and next action

This review establishes caller-document consistency and discoverability. It does not verify wrapper execution, archive contents, installed-image selection, native Hello or release artifacts.

The next action is to add the two packaging option references and retain this Surface GO for the bounded hosts4a aggregate decision.
