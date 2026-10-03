# GWZ SSPI installed hosts4a — Surface-AXIS REVIEW

**Review object:** Final narrow caller cleanup at the corrected tuple below, focused on the remaining Surface P3-1 delegated-option defaults and the documented build-scratch selection. HTTP composition, native installed qualification, activation and publication remain separate gates.

**Baseline:**

| Repository | Committed HEAD |
|---|---|
| gwz-dev | `a17a7b07becb1e92da5519b64add39c41d6ee09e` |
| gwz-sspi | `14d834b311200b0984e7041e4a34419c59501119` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Sources were read using `git show HEAD:<path>` and permitted caller-document diffs from round 1. All five HEADs matched the canonical prompt at the start and end.

**Date:** 2026-10-04

**Axis:** Surface — cold caller discoverability, defaults, lifecycle pairs and build/install/use/upgrade/remove walkthrough. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on their current reports. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2 or P3 findings. Original Surface P3-1 is closed.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Surface P3-1 | Document wrapper requirements/defaults, delegated choices and absence behavior, output locations and host-native examples | CLI contracts remain explicit. Python `HostPackaging.md:86–113` now supplies accepted delegated values, omitted behavior, configuration precedence, Linux/platform limits and precise primary-contract links. The default and explicit recipes no longer require guessing the auditwheel/compatibility policy. | CLOSED |

## Changed-range analysis

Within the permitted caller documents, only `gwz-py/docs/HostPackaging.md` changes from round 1:

- Replaces the delegated-option name list with a values/behavior/omission table.
- States auditwheel modes and default policy, compatibility aliases and supported forms, configuration-dependent omission behavior, legacy-tag limits and repair prerequisites.
- Clarifies that `CARGO_TARGET_DIR` selects a scratch root containing unique private target directories. The root remains; per-build directories are disposed.
- States candidate `--target-dir` precedence and its `DESTINATION/target` default, while ordinary backend scratch defaults to `--out`.
- Preserves transaction staging under `--out` and the documented no-replace publication boundary.

SSPI, CLI and reference core HEADs are unchanged. Python RELEASE and the root caller guide have no caller-document delta.

The option-table change is a documentation correction for the original finding. Scratch-root selection is a caller-visible build-placement change beyond that finding; it is explicitly disclosed without changing the documented ownership of private per-build directories or final publication staging. Its implementation is outside this Surface review.

No **NEW ARCHITECTURAL** root cause was found. The reviewed text introduces no runtime worker owner, authentication authority, wire change or platform activation.

## 0. Evidence base

Read the complete corrected permitted caller documents:

| Document | Lines |
|---|---:|
| `gwz-sspi/README.md` | 1–59 |
| `gwz-sspi/docs/HostPackaging.md` | 1–59 |
| `gwz-sspi/docs/WorkerEntry.md` | 1–93 |
| `gwz-cli/docs/HostPackaging.md` | 1–73 |
| `gwz-py/docs/HostPackaging.md` | 1–152 |
| `gwz-py/RELEASE.md` | 1–214 |
| `dev-docs/GwzSspiCallerGuide-DRAFT.md` | 1–145 |

Also read the complete canonical `GwzSspiHosts-PromptSurface-2.md`, my round-1 report, and the narrowly extracted Surface P3-1 disposition and remainder. My original report remains part of the continued review context.

Commands were limited to `cat`, `git show`, numbered document reads, caller-document-only `git diff`, an exact disposition-row extraction, and five-repository `git rev-parse HEAD` / `git status --short`.

SSPI, CLI and Python were clean at both checks. Root and reference core contained only the declared out-of-scope untracked items; their status lists remained unchanged.

No implementation, tests, design, checkpoint, drafter narrative or current peer contents were read. No build, install, executable/help execution, native execution, write or Git mutation occurred. The linked upstream references were inspected as part of the caller page’s attribution; their external contents were not fetched.

## 2. Invariant analysis

**Remaining default-policy counterexample:** With no delegated arguments and no overriding configuration, the page now predicts release profile, frontend interpreter, documented target precedence, auditwheel repair and the lowest compatible manylinux tag—or native Linux where none matches. Configured auditwheel/compatibility policy is expressly distinguished from that baseline. The original omission ambiguity is resolved.

**Explicit policy selection:** The page describes `repair`, `check`, `warn` and `skip`, plus compatibility aliases and tag forms. It explains the Linux scope of libc tags, cross-platform `pypi` choice, unsupported legacy compiler targets and the patchelf prerequisite. A caller can predict the intended policy of an explicit auditwheel/compatibility selection from this page alone.

**Other delegated defaults:** Features, default features, locked/offline/frozen resolution, stripping and verbosity now have stated omission behavior. Value and alias conflicts remain documented as refusal before provisioning.

**Scratch placement and disposal:** The page names ordinary backend and candidate scratch-root defaults. It distinguishes a retained root from disposed private build directories and keeps wheel transaction staging beside the destination output. This avoids implying that selecting a scratch root authorizes deleting the root or reusing a shared mutable build artifact.

**Host-native and explicit builds:** CLI target/profile/output requirements remain explicit. Python’s host-native and development examples remain consistent with the new tables. External scratch selection is now an adjacent documented recipe.

**Lifecycle and trust:** CLI removal remains paired with its self-executing executable. Python uninstall remains paired with the complete RECORD-managed wheel, worker and receipt. Matching compiled metadata and Hello remain authoritative; diagnostic receipts and runtime variables do not provision trust.

**Capability boundaries:** Ordinary Cargo and editable installations remain unprovisioned. Callable descriptors remain distinct from Supervisor creation, native execution and HTTP authentication. Digest, registry availability, installed Windows qualification and activation remain disclosed separate gates.

These attacks produced no new findings.

## 3. Risks and next action

This verdict establishes the corrected caller contract and closes its documentation finding. It does not verify delegated execution, scratch placement, publication implementation, installed artifacts or native qualification.

The next action is to accept Surface P3-1 as closed at this exact tuple and carry this GO into the bounded hosts4a aggregate decision.
