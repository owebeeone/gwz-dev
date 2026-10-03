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
