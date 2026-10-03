# GWZ SSPI installed hosts remediation 1

2026-10-04. Code and State NO-GO; Surface GO with one nonblocking P3.
Initial reviewed tuple: root 8cb284391dc5864c9ba6f41324cdd3c5807cf765,
SSPI e31b17e95defd3468140e9b5f73fef8591766599,
CLI 061385fc009fd173488dd07e18d5b2d5882de05d,
Python e9e228c0006d7f8b3d44c9657403dec5905c6e90;
reference core 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31 stays unchanged.
Original reports are retained verbatim. Merge all six findings into one patch,
then settle and return to the same independent reviewers for closure and
changed-range classification. No HTTP/native/activation expansion is authorized.

| Finding | One disposition | Closure evidence |
|---|---|---|
| State P2-1: wheel publication transaction | Accept. Stage raw wheel and provisioning privately, validate complete coherent RECORD, use unique temporary writes and atomic publication with explicit same-name serialization/collision refusal. Protect shared extension-cache capture too. | Real handoff/bundler with synthetic same-name wheels, different fingerprints and barriers; interrupted/copy/provision failure preserves prior complete destination or absence; separate output dirs sharing extension cache remain coherent. |
| State P2-2: CARGO_BUILD_RUSTFLAGS omission | Accept. Include conventional build-level rustflags or explicitly refuse before build; audit supported compiler flag channels together. | Fixed synthetic metadata/compiler inputs; unset/set and differing build flags plus precedence with ordinary/encoded/target flags distinguish identifiers or refuse; wrapper-refusal tests retained. |
| Code P2-1: interpreter target defaults | Accept. Resolve frontend/explicit interpreter and effective target before fingerprinting; normalize metadata, worker and extension choices together; preserve candidate --python contract. | Process-free AMD64/32-bit frontend/i686 case and explicit overrides, recorded producer inputs and actual argv agree; actual Darwin wheel and extracted-sdist paths retained. |
| Code P2-2: lossy filesystem path | Accept. Expose checked OS path through lossless PyO3 filesystem conversion or fixed refusal, never changed-success. | Unix non-UTF8 path fsencode round-trip through public conversion; Windows unpaired-surrogate conversion fixture; existing actual installed-selection fixture retained. |
| Code P3-1: matrix receipt collisions | Accept. Matrix-qualified receipts survive flattened aggregation, or combine with explicit validated aggregation. | At least two disjoint matrix target sets aggregate with all targets/fingerprints exactly once. |
| Surface P3-1: packaging option defaults | Accept. Document required/optional/options/defaults, supported profiles/targets, outputs, delegated flags and host-native/release examples. | Compare real wrapper help/options; independent cold walkthrough predicts host-native and explicit outputs and provisioning. |

No blind convergence on one exact defect: the axes found distinct packaging
causes. Four blocking P2 and two P3 records were discovered before acceptance;
no production exposure or escaped release defect is established. Corrections
retain private IPC/runtime owners/compiled fingerprint authority. Wheel staging
is build-time artifact ownership, not a new authentication process owner.
Any material architecture/interface change must be classified by reviewers;
the lane's two-remediation cap applies. Existing 25 source/test/workflow-file
ceiling and 1,800 addition budget remain; use existing files for regressions.

## Corrected tuple and gates, pending reviewer closure

SSPI 14d834b311200b0984e7041e4a34419c59501119, CLI
0c7dfaf0199731648d2360358284010b2b4575c1, Python
c3f5f7d6b614155e413db0036242849010ef1149; reference core unchanged.
Eleven existing files changed, no dependency/lock/wire/core delta. Wheel and source
archive adaptations stage privately; wheel publication uses atomic no-replace
hard links and explicitly refuses collisions/unsupported filesystems. Mutable
extension-cache reuse is removed from provisioned wheel builds and documented.
Metadata and build hooks normalize one effective interpreter/target. OS paths
cross PyO3 losslessly. Rustflags channels are conservative identity inputs and
matrix receipts have target-set names.

Original frontend/default and real handoff tests: seven failures to seven passes;
rustflags sensitivity, Unix path byte-round-trip and actual flattened matrix
aggregation each reproduced failure before correction. Owner independently passed
the corrected installed wheel suites (50 passed, one filesystem skip), producer
suite, CLI release tests and conditional-boundary guard; member diff checks clean.
Drafter additionally passed 27 Rust library tests, strict library Clippy, pinned
changed-file formatting, 16-artifact regeneration, real Darwin wheel/sdist and
extracted-sdist rebuild, extracted installed descriptor and RECORD validations.
Ordinary artifacts remain external in evidence-build-cache/gwz-sspi/hosts-remediation-1.

Limits: Darwin refuses the non-UTF8 fixture directory with EILSEQ. The Rust PyO3
conversion test passes, but no actual non-UTF8 loader proof is claimed. Windows
surrogate/runtime publication and installed Hello mismatch remain unexecuted.
Existing all-target Clippy needless_update at operations.rs:714 is unchanged.
Cumulative implementation: 25 source/test/workflow files, 1,168 additions and
six member plus three root implementation docs. Mandatory review/remediation
audit records are separate process artifacts. No further expansion or activation.
Same reviewers must classify changed-range materiality and verify closure;
these checks are implementation evidence, not self-closure.

## Remediation-1 verdicts and nonblocking cleanup

Code, State and Surface all returned GO on root 6b670283179f45f0dbbe38b1f6c300b326e50100
and the corrected member tuple above. Four original P2 findings are independently
closed. Code also closes its matrix-receipt P3. No new architectural root cause
was identified. Two bounded P3 records remain; they are routine corrections in
the same object, not a new feature or architecture package:

| Finding | Disposition | Closure |
|---|---|---|
| Code P3-2: candidate target-dir ignored | Preserve scratch-root selection with unique owned Rust target trees beneath the supplied root. Keep raw/provisioned wheel staging beneath output for same-filesystem publication. Update candidate defaults/help and tests. | Trace explicit/default candidate roots through production backend with mocked subprocesses; retain coherent concurrent/fault tests. |
| Surface P3-1 remainder | State delegated auditwheel/compatibility choices, absence/config policy and precise upstream contracts; no behavior change. | Cold comparison with installed Maturin 1.15 help and primary contract; same Surface reviewer predicts default/explicit selections. |

Artifact coherence, runtime authority, wire/core/auth ownership remain unchanged.
The original round-1 testimony is preserved. Reviewers verify the narrow correction;
no additional authentication/native campaign or expensive unchanged Rust gate is needed.

Nonblocking cleanup is committed at Python ded47130af23720099e7b6a92ccb9a161bb5db9a;
SSPI/CLI/core are unchanged. Four existing Python files change. Explicit/default
candidate scratch-root regressions reproduced two failures and one control pass,
then all three pass. Owned target capsules now live under the selected root;
private wheel transaction staging and no-replace publication remain under output.
Delegated option choices/absence policy link verified primary Maturin 1.15 contracts.
Owner independently passed the final focused suites with the previous unchanged
native image: 53 passed, one EILSEQ skip. The unprovisioned suite has 52 passes
and two opt-in fixture skips. No fresh packaged artifact/native qualification or
unchanged Rust build is claimed; diff checks pass. Final focused closure is pending.
