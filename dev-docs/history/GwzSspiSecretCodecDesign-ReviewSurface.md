# GWZ SSPI secret codec and caller values — SURFACE-AXIS REVIEW

**Review object:** New value ergonomics and caller documentation only: root `85b4bcc2d8230c0f28672a02bb99997b8a8e79c3..e9f80c697acc5860ad90dbf5acd5888ccf2bd586`; gwz-sspi `2e3b646411645d0bbe1081e4dcc15d1e0b971a4e..e3851768da8d58140d92590bf61575f6edbe333c`. Implemented values remain pending their separate review gate; authentication and release remain gated. Reviewed 2026-10-03.

**Baseline:** Root `e9f80c697acc5860ad90dbf5acd5888ccf2bd586`; gwz-sspi `e3851768da8d58140d92590bf61575f6edbe333c`; unchanged reference gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Caller documents were read using `git show exactSHA:path`, with numbered output. All three HEADs matched at both start and end.

**Date:** 2026-10-03

**Axis:** Surface: cold discovery, names, construction, ownership lifecycle, units, defaults and documented availability. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1 or P2 findings; one bounded P3 documentation finding.

---

## 0. Evidence base

Process instructions read:

- Root `AGENTS.md` and `AGENTS_GWZ.md`.
- Member `gwz-sspi/AGENTS.md`.
- `dev-docs/AgentProcessRules.md`, read completely in segments after the initial output was truncated.
- `dev-docs/GwzProcessOptimization.md`, including the controlling review-granularity amendment.
- Complete canonical `dev-docs/GwzSspiSecretCodecDesign-PromptSurface.md`.

Caller documents read at the reviewed commits:

| Document | Lines | Evidence examined |
|---|---:|---|
| Root `dev-docs/GwzSspiCallerGuide-DRAFT.md` | 1–122 | Availability, full future API, token limits, request profiles, lifecycle, platform behavior |
| Member `README.md` | 1–51 | Implemented subset, discoverability, worker refusal and release status |
| Member `docs/CallerValues.md` | 1–60 | Constructors, borrowed sources/accessors, Drop, traits, fields, errors and validation boundary |
| Member `RELEASE.md` | 1–155 | Publication guard, qualification and activation prerequisites |

For baseline comparison, the root caller guide was also read at `85b4bcc2d8230c0f28672a02bb99997b8a8e79c3` and the member README at `2e3b646411645d0bbe1081e4dcc15d1e0b971a4e`.

Targeted `rg -n` checks over exact-commit caller-document output examined constructor names, import paths, defaults, ownership and implemented-status wording. They confirmed that the allowed caller documents provide no `use` statement or fully qualified public paths for the newly implemented types.

`git rev-parse HEAD` succeeded in root, gwz-sspi and gwz-core at both review boundaries, returning the exact tuple above.

No code, design, implementation plan or current peer report was read. No builds, tests, worker execution, writes or Git mutations were performed. The prompt states that these Rust values add no public `--help` surface.

## 1. Findings

### [P3-1] The implemented-value guide omits public import paths and a complete construction walkthrough

**Location:** Member `docs/CallerValues.md:7–10` and `:22–42`; member `README.md:27–28`; root `dev-docs/GwzSspiCallerGuide-DRAFT.md:21–24` and `:91–109`.

**Violated invariant:** A first-day caller should be able to discover where the implemented interface lives and construct, inspect and dispose of a value from caller documentation without guessing its public API placement.

**Reproduction:** Start at the README and follow its implemented-caller-values link. The guide supplies unqualified names such as `SecretBytes`, `SecretText`, `TokenLimit` and `AuthRequest`, plus a field inventory. It supplies neither public import paths nor a complete Rust example. The README’s layout lists source filenames but explicitly warns that boundary directories are not public Rust modules. The root guide’s walkthrough begins with the unimplemented Supervisor API and does not supply a separate implemented-value example. A caller must therefore guess crate-root exports versus module paths or consult material outside the documented walkthrough before writing a self-contained consumer.

**Impact:** Initial use requires avoidable source/API exploration. The prose describes ownership well, but cannot serve as a reproducible construction recipe for the newly available surface. This is a documentation correction and requires no compatibility break.

**Required correction:** Add a small, complete example to `docs/CallerValues.md` with the actual public imports, construction of both secret types and a `TokenLimit`, borrowed accessor use, construction of an `AuthRequest`, and explicit disposal. State that constructing the request does not validate its full profile. Show caller-owned source cleanup separately from dropping the copied secret, using synthetic inputs.

**Closure/regression test:** The lane owner should compile the documented example as a downstream consumer or doctest against the corrected checkpoint. Independently repeat the walkthrough using only the caller documents, confirming that no import path, field type or cleanup step requires guessing. No such compilation was executed during this read-only review.

## 2. Invariant analysis

The following attacks did not produce findings:

- **Borrowed-source responsibility:** `CallerValues.md:7–14` explicitly says constructors copy borrowed inputs, cannot wipe them, and leave source cleanup with the caller. Accessor borrows remain tied to their owner.
- **Construction/disposal pairing:** Normal Drop wipes owned bytes. The guide limits that promise and does not imply physical erasure or disposal of provider/LSASS memory.
- **Secret trait exposure:** `CallerValues.md:44–46` identifies the secret-bearing types that lack Clone and Debug, states Send + Sync, and distinguishes nonsecret traits.
- **Token-limit ambiguity:** The constructor parameter and accessor name explicitly use raw bytes. The inclusive range is 1–65,536, invalid endpoints are illustrated, and there is no Default or implicit fallback. HTTP scheme/base64 overhead belongs to the host.
- **Validation timing:** `CallerValues.md:35–38` expressly warns that struct construction does not admit the full request profile; the private adapter validates before encoding. Digest requirements and prohibition on other packages are stated.
- **Implemented versus future APIs:** Both caller guides identify Supervisor, Conversation and worker entry as unimplemented. Pure constructors work on every platform; future native Supervisor construction on non-Windows is distinguished.
- **False worker availability:** The README and implemented-value guide state that the packaged executable refuses calls. Release instructions retain the publication and qualification guards.
- **Private IPC exposure:** Private records/codecs are expressly inaccessible. Token projection is described as conversion requiring future publication eligibility, not as authentication or cleanup proof.
- **Lifecycle/default shape of the future surface:** The documented full API pairs start with finish/cancel and Supervisor ownership with shutdown/drop behavior. Capacity defaults and explicit deadline requirements are stated. Their runtime outcomes remain deferred.

The first-day walkthrough therefore reaches construction semantics, inspection and disposal in prose, but encounters the import/example gap recorded as P3-1. Authentication and worker installation remain expressly unavailable at this checkpoint; that deferred outcome is not a finding.

## 3. Risks and next action

This Surface verdict establishes documentation fitness only. It does not verify implementation behavior, zeroization, disabled platform branches, codec correctness or native qualification. The documented runtime deferrals remain outside this review.

The single next action is to add and compile the implemented-value walkthrough described in P3-1. The finding is nonblocking under this gate’s severity contract. The final tuple recheck matched the starting tuple exactly.
