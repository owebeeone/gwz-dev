# GWZ SSPI installed hosts4a — Code-AXIS REVIEW

**Review object:** Installed hosts4a, controlled by `dev-docs/GwzSspiHostsCheckpoint.md` at root `8cb284391dc5864c9ba6f41324cdd3c5807cf765`. The checkpoint remains DRAFT.

**Baseline and reviewed tuple:**

| Repository | Baseline | Reviewed HEAD |
|---|---|---|
| gwz-dev | `306663037d6992584f88078be5c9db36f4fa4ce4` | `8cb284391dc5864c9ba6f41324cdd3c5807cf765` |
| gwz-sspi | `84266f412b12a31b9b643a51158079a5412470e5` | `e31b17e95defd3468140e9b5f73fef8591766599` |
| gwz-cli | `f925e1165c2b2d368a00277594450b010a95867a` | `061385fc009fd173488dd07e18d5b2d5882de05d` |
| gwz-py | `5aeff4bfb11da048f1174db2b1045ed0c9b6c80a` | `e9e228c0006d7f8b3d44c9657403dec5905c6e90` |
| gwz-core, unchanged reference | — | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

**Date:** 2026-10-04  
**Axis:** Architecture, interfaces, call graphs, compatibility and error paths. Independent, adversarial, read-only. Nothing here relies on the parallel reviews. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block acceptance; one P3 finding affects release diagnostics. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified, provided the bounded changes introduce no new blocking defect.

---

## 0. Evidence base

All five HEADs matched the canonical prompt at the start and end. The SSPI, CLI and Python working trees were clean. Root and reference core contained the disclosed untracked documentation; an untracked Surface report appeared during review but was not opened. No reviewed HEAD moved.

Inspection used `git rev-parse`, `git status --short`, `git show`, `git diff`, `git diff --check`, `rg`, `cat`, `sed` and `nl`. Member diff checks returned no whitespace errors. No files were written, no Git state was mutated, and no builds, compiler probes, tests, helpers or native processes were run.

Authority read included root/member instructions, `EVIDENCE.md`, `AgentProcessRules.md` and its `GwzProcessOptimization.md` amendment; accepted SSPI Design revision 2 §§2,4–6; Plan step 4; the complete Hosts checkpoint; and member HostPackaging/WorkerEntry documentation.

The implementation review covered the changed host/package files and relevant retained call sites, particularly:

- SSPI `scripts/artifact_set.py:13–111`, `src/packaging.rs:7–54`, packaging tests, artifact-producer tests and the existing minimal worker entry.
- CLI `build.rs:3–21`, `src/worker_host.rs:4–59`, the first statements of `gwz::run`, `scripts/build_sspi.py:9–48`, release reconciliation/tests, manifest/lock changes and release workflow provisioning/upload/download steps.
- Python `build_support/sspi_backend.py:15–130`, `native/src/worker_host.rs:7–96`, `native/src/lib.rs:16–23,164–166`, pyproject configuration, candidate staging/build/unpack paths, package smoke, release reconciliation/tests and actual publish workflow entry points.
- Python packaging fixtures, including environment preservation, worker-output isolation, installed-image selection and extracted publish-step dispatch.

Two dependency implementations were inspected to establish compatibility behavior, without executing them: the installed Maturin Python backend at `gwz-py/.venv/lib/python3.12/site-packages/maturin/__init__.py`, especially lines 50–75,90–122,197–239; and resolved PyO3 0.28.3 OS-string/path conversions and their round-trip tests.

External permitted artifacts inspected included the local CLI receipt, installed wheel receipt/layout, extracted-sdist backend/configuration and `hosts/owner-tests.log` under `/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/hosts`. The recorded SSPI test/doctest log passed. These are inspected owner evidence, not tests rerun by this reviewer or Windows runtime qualification.

## 1. Findings

### [P2-1] The wheel backend loses interpreter-dependent target defaults

**Location:** [sspi_backend.py](/Volumes/projects/limbo/gwz-dev/gwz-py/build_support/sspi_backend.py:45), lines 45,53–58,61–66 and 102–107. The candidate entry point at `scripts/build_candidate_extension.py:147–150,190–193` also reaches this backend.

**Violated invariant:** Ordinary Python packaging must retain compatible frontend/interpreter behavior, and the fingerprint, worker build and extension build must use the actual intended target. Retained metadata and wheel hooks must agree on that selection.

**Reproduction/state sequence:** Use a 32-bit Python interpreter on an AMD64 Windows machine with an x86_64 Rust host, without an explicit target. The retained Maturin metadata hook supplies `--target i686-pc-windows-msvc` through `_additional_pep517_args()` and explicitly supplies the frontend interpreter.

The new wheel hook instead selects the `rustc -vV` host at line 45 and makes that x86_64 target explicit at line 57. Its manually constructed Maturin command omits the original backend’s automatic `-i sys.executable` selection. The worker command receives the same incorrectly selected x86_64 target. Thus the retained metadata path selects i686 while the new build path attempts an x86_64 artifact for the 32-bit frontend. The candidate’s advertised `--python` no longer supplies the explicit build-interpreter argument that its previous Maturin command supplied.

**Impact:** A previously supported interpreter-dependent packaging default becomes a build/install failure or an artifact with the wrong architecture for the invoking interpreter. Matching compiled fingerprints between two x86_64 outputs does not repair the wrong target selection.

This is a deterministic packaging/default-contract defect. It does not require Windows native SSPI execution or reopen the deferred Windows qualification outcome.

**Required correction:** Determine the effective interpreter and interpreter-dependent target before fingerprinting. Preserve Maturin’s frontend-interpreter default when no interpreter was supplied, honor explicit interpreter/target options, and pass the resolved target consistently to metadata, worker and extension builds. Include the normalized effective choices in the recorded packaging inputs.

**Closure/regression test:** Add a process-free fixture representing AMD64 Windows, a 32-bit frontend interpreter and an x86_64 compiler host. Assert that metadata, producer inputs, worker argv and extension argv select i686 and that the extension command names the frontend interpreter. Cover explicit overrides and verify the candidate `--python` contract. Retain the existing Darwin wheel/extracted-sdist checks.

### [P2-2] The Python descriptor changes valid filesystem paths through lossy conversion

**Location:** [native/src/lib.rs](/Volumes/projects/limbo/gwz-dev/gwz-py/native/src/lib.rs:19), lines 19–22.

**Violated invariant:** A successful installed descriptor must return the exact selected adjacent worker path. An OS-derived checked path must not be silently changed at the Python boundary.

**Reproduction/state sequence:** Load a provisioned extension from an absolute Unix directory containing a non-UTF-8 filename byte, for example a component containing `0xff`, with the regular worker beside it. The loader implementation preserves the bytes in `OsString`, and `installed_worker` checks that exact adjacent path. The exported function then calls `to_string_lossy()`, replacing the invalid byte with U+FFFD.

The returned Python string consequently names a different filesystem path. Windows paths containing an unpaired UTF-16 surrogate have the analogous conversion problem.

**Impact:** The function reports success with a pathname that may not exist or may identify a different directory. This breaks the callable descriptor’s current path contract even though the internal Rust descriptor selected the correct worker. The ASCII-only installed fixture cannot detect it.

**Required correction:** Return the OS path through PyO3’s lossless filesystem-string conversion rather than through a Rust UTF-8 `String`. PyO3 already converts `OsStr`/`OsString` to a Python string using filesystem decoding on Unix and wide characters on Windows. If a path representation is intentionally unsupported, refuse with a fixed classification instead of returning a changed path.

**Closure/regression test:** Exercise the public conversion with a Unix `OsString` containing non-UTF-8 bytes and assert that `os.fsencode(result)` reproduces the original path bytes. Cover an unpaired Windows surrogate through the corresponding conversion fixture. These conversion tests do not require native SSPI execution; keep the existing installed-selection fixture as well.

### [P3-1] Matrix receipts overwrite each other during release aggregation

**Location:** [scripts/build_sspi.py](/Volumes/projects/limbo/gwz-dev/gwz-cli/scripts/build_sspi.py:47), line 47; `.github/workflows/release.yml:164–171,195–201,250–254,269–274`.

**Violated invariant:** The diagnostic receipt accompanying release artifacts must retain the artifact-set inputs for every actual matrix target.

**Reproduction/state sequence:** Two local build jobs produce distinct target fingerprints. Each writes and uploads a file named `sspi-artifact-sets.json` containing only that job’s targets. The global and host jobs flatten the local artifacts into one destination with `merge-multiple: true`. Both receipts occupy the same destination pathname; they cannot both survive. The final release aggregation repeats that collision.

**Impact:** The released diagnostic receipt loses the records for other matrix jobs. Operators cannot recover those targets’ declared input sets from the receipt accompanying the release. Runtime trust is unaffected because receipts are not authority.

**Required correction:** Give matrix receipts unique target/job-qualified filenames and retain them through aggregation, or explicitly combine them into one validated aggregate receipt. Preserve the convenient fixed filename for a single local build if desired.

**Closure/regression test:** Stage receipts for at least two disjoint matrix target sets, perform the workflow’s intended aggregation in a fixture and assert that all targets and fingerprints survive exactly once.

## 2. Invariant analysis

The principal SSPI boundary remains intact. The diff adds packaging and descriptor handoffs without changing wire/schema, adding an HTTP carrier, introducing a core adapter or creating another process owner. Existing Supervisor/worker ownership remains the intended launch and Hello-verification boundary.

Runtime receipt, environment, PATH and Python-attribute redirection attacks failed at the native selection boundary. Both hosts use compiled metadata. The Python descriptor obtains its image from an extension-local address through OS loader APIs, rejects relative image paths and chooses the fixed adjacent worker. Missing, symlink and nonregular worker files refuse. P2-2 occurs after that correct selection, during its public representation.

CLI early dispatch precedes build-info handling and ordinary parser/application initialization. Malformed private bootstrap input returns the worker refusal exit path rather than falling through. Ordinary arguments continue through the ordinary CLI path. The callable CLI descriptor captures `current_exe` and compiled expected bytes without creating a Supervisor or process.

Fingerprint decoding distinguishes absent metadata from malformed metadata and requires exactly 64 ASCII hex digits. No runtime variable supplies a fallback fingerprint. The producer resolves package manifests through Cargo rather than assuming sibling checkout paths, follows the host dependency closure and records source/contracts, lock/configuration, compiler, target/profile/features and packaging options. Local native fork/vendor inputs are included. This remains an input-set identifier, not binary attestation.

The wheel producer uses copied subprocess environments and a build-owned worker output directory. Worker output cannot be replaced by another build sharing the extension cache before bundling. Bundling adds the fixed worker and diagnostic receipt beside the extension and rebuilds wheel RECORD hashes. Candidate unpacking restores worker executable permissions.

The PEP517, package-smoke, candidate and publish wheel entry points reach the new backend. The sdist path carries that backend at the extracted archive’s declared backend path, and producer discovery uses the extracted Cargo graph. The retained metadata hooks expose the default mismatch in P2-1.

Ordinary Cargo builds remain Python-free. Editable builds remove packaging metadata from their copied environment and remain explicitly unprovisioned. Their ordinary development behavior is preserved in intent, although interpreter defaults still need correction.

Release reconciliation recognizes the reviewed SSPI dependency form and writes the exact registry pin before existing registry checks. The unpublished `publish=false` prerequisite and disclosed lock reconciliation are explicit limitations, not hidden successful-release claims.

The modified Rust platform sections use enclosing conditional boundaries and braced control-flow bodies. No new conditional-attribute reassociation defect was found.

## 3. Risks and next action

This review establishes source/interface defects and inspected portable packaging evidence. It does not establish Windows installed Hello execution, complete Windows host qualification, Linux installed-image qualification, HTTP/core composition, remote authentication/EPA, publication or CI success. Those remain the stated deferrals. Existing untouched formatting/Clippy debt and standalone lock reconciliation were not promoted to findings.

The next action is one bounded remediation revision correcting P2-1 and P2-2, preferably also P3-1, followed by focused tests and a new settled-tuple review. Acceptance of hosts4a would still leave Plan step 4b, platform qualification and product activation open.
