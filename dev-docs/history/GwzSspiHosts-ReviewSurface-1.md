# GWZ SSPI installed hosts4a — Surface-AXIS REVIEW

**Review object:** Remediation 1 of the installed-host caller surface at the corrected tuple below. Packaging and callable descriptors remain distinct from deferred HTTP composition, Windows activation, installed native qualification and publication.

**Baseline:**

| Repository | Corrected committed HEAD |
|---|---|
| gwz-dev | `6b670283179f45f0dbbe38b1f6c300b326e50100` |
| gwz-sspi | `14d834b311200b0984e7041e4a34419c59501119` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `c3f5f7d6b614155e413db0036242849010ef1149` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Sources were read using `git show HEAD:<path>` and caller-document-only diffs against the original reviewed tuple. All five HEADs matched at the start and end.

**Date:** 2026-10-04

**Axis:** Surface — cold caller discoverability, defaults, lifecycle pairs and build/install/use/upgrade/remove walkthrough. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on their current reports. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1 or P2 findings; original P3-1 remains partly open as a nonblocking documentation defect. No new architectural root cause was found.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Surface P3-1 | Document required/optional options, defaults, supported profiles/targets, outputs, delegated flags and host-native/release examples | CLI requirements, defaults, profile choices and output locations are explicit. Python wrapper defaults, profile/interpreter/target precedence and examples are explicit. Python’s supported delegated compatibility/auditwheel options still lack default or absence behavior. | PARTIALLY RESOLVED; OPEN P3 |

## Changed-range analysis

The permitted document diff changes three packaging pages:

- **SSPI HostPackaging:** Adds conservative rustflags-channel recording and complete wheel RECORD validation disclosures.
- **CLI HostPackaging:** Adds local/matrix option contracts, output locations, qualified receipt names and a host-native release-profile recipe.
- **Python HostPackaging:** Adds wrapper/delegated option documentation and normalized interpreter/target selection. It also changes the documented build ownership from shared extension-cache use to private worker/extension staging, adds same-name refusal and atomic no-replace publication, and specifies lossless filesystem-string results.

README, WorkerEntry, Python RELEASE and the root caller guide are unchanged.

The CLI and Python option references directly address Surface P3-1. The staging, publication, normalized metadata and path-conversion disclosures are additional caller-visible changes beyond that disposition. They materially change documented build-time ownership and failure/publication guarantees; they are not merely wording corrections. They do not claim a new authentication process owner or alter runtime fingerprint authority.

This Surface review verifies the discoverability and consistency of those new contracts. It cannot validate their implementation or carry forward implementation proofs, because source and execution are excluded. No **NEW ARCHITECTURAL** root cause was found within the permitted evidence.

## 0. Evidence base

Read the corrected permitted caller documents:

| Document | Lines |
|---|---:|
| `gwz-sspi/README.md` | 1–59 |
| `gwz-sspi/docs/HostPackaging.md` | 1–59 |
| `gwz-sspi/docs/WorkerEntry.md` | 1–93 |
| `gwz-cli/docs/HostPackaging.md` | 1–73 |
| `gwz-py/docs/HostPackaging.md` | 1–118 |
| `gwz-py/RELEASE.md` | 1–214 |
| `dev-docs/GwzSspiCallerGuide-DRAFT.md` | 1–145 |

Also read the complete canonical remediation prompt, my original `GwzSspiHosts-ReviewSurface.md`, and the Surface P3-1 disposition.

A context-bearing extraction of that disposition inadvertently displayed adjacent prior-round disposition rows. They were excluded from this assessment. No current peer prompt or report contents were read.

Commands comprised `cat`, `git show`, numbered document reads, caller-document-only `git diff`, and five-repository `git rev-parse HEAD` / `git status --short`.

SSPI, CLI and Python were clean at both checks. Root and reference core status lists contained only the declared out-of-scope untracked items and remained unchanged.

No implementation, tests, design, checkpoint or drafter narrative was inspected. No build, install, executable help, native execution, write or Git mutation occurred.

## 1. Findings

### [P3-1] Python delegated build options still lack their default policy

**Location:** `gwz-py/docs/HostPackaging.md:82–89`, following the defaults and precedence explanation at lines 71–80.

**Violated invariant:** The original finding required supported options to state their default or absence behavior. Listing accepted delegated option names does not establish the resulting build policy when they are omitted.

**Reproduction:** Follow the new host-native default wheel recipe at lines 90–94. The page explains its default profile, interpreter and target. Then determine from the same page what happens when `--auditwheel`, `--compatibility` and `--manylinux` are absent, and how their supplied values affect that default policy. The page lists these names but supplies neither their supported values nor their absence behavior.

The reader therefore still needs external documentation, implementation inspection or execution to predict the delegated compatibility/auditwheel policy.

**Impact:** A packager can adapt the new default recipe without knowing its wheel compatibility or auditing behavior. This is the remaining portion of the original option-contract documentation defect, not a new root cause or compatibility blocker.

**Required correction:** Complete the delegated-option reference, particularly compatibility/auditwheel choices: state accepted values and default/absence behavior, including configuration-dependent defaults where applicable. A precise linked delegated contract can avoid duplicating extensive upstream documentation.

**Closure/regression test:** Compare this reference with the supported delegated argument contract, then independently predict the compatibility/auditwheel policy for empty arguments and an explicit selection from the caller page alone. No execution was authorized during this review.

## 2. Invariant analysis

**Original host-native ambiguity:** The CLI page now identifies required target and absolute target directory, default release profile, configured profile behavior and output paths. Its host-native recipe derives the compiler host rather than copying the Darwin target. This part of P3-1 is resolved.

**Python wrapper defaults:** The page now identifies required `--out`, empty default `--build-args`, profile selection, frontend interpreter, explicit interpreter selection and target precedence. It distinguishes shell tokenization from shell execution and documents refusal of unsupported/conflicting arguments. The default and explicit development recipes are usable without guessing these selections.

**Output ownership and replacement:** Python documents private staging beneath the output directory, rejection of shared mutable extension-cache capture, complete validation before publication, explicit same-name refusal and a fresh-directory replacement workflow. Unsupported hard-link filesystems fail without replacing the prior output. These new caller-visible failure rules are explicit.

**Descriptor results:** The Python page now states that the result preserves native filesystem path identity, including non-UTF8 Unix names and Windows unpaired surrogates, and names `os.fsencode` for Unix byte recovery. It does not equate a selected path with authenticated worker contents or endpoint success.

**Lifecycle pairs:** CLI installation/upgrade/removal remains tied to its single executable. Python installation/upgrade/removal remains tied to the complete wheel and RECORD-managed adjacent worker/receipt. The changed build publication behavior does not introduce an undocumented overwrite or separate installed-worker lifecycle.

**Provisioning and authority:** Ordinary Cargo and editable builds remain explicitly unprovisioned. Matching compiled metadata and later Hello verification remain authoritative; receipts, runtime variables, mutable module attributes and current-directory lookup remain excluded as trust sources.

**Deferred outcomes:** The pages continue to separate descriptor selection and packaging from HTTP composition, native installed Hello, full Windows qualification and activation. Registry availability and Digest refusal remain explicit. No deferred outcome was treated as a finding.

## 3. Risks and next action

The new build ownership and publication guarantees require implementation evidence from the appropriate reviews; this document-only verdict does not validate them.

The next action is to finish the delegated-option default reference, while retaining this nonblocking Surface GO for the bounded hosts4a decision.
