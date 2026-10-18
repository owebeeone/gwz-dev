# GWZ SSPI installed hosts checkpoint

2026-10-04. **ACCEPTED installed-host packaging after Code/State/Surface GO.** The operator
authorized the next chunk after native worker acceptance. This is plan step 4a:
trusted artifact production, CLI early self-execution and Python bundled worker.
Step 4 as a whole remains open until the HTTP composition in 4b is accepted.
Windows endpoint activation, publishing and full release qualification remain
NO-GO. No push, tag or release is authorized by this checkpoint.

## Acceptance and review ledger

Independent final [Code](GwzSspiHosts-ReviewCode-2.md),
[State](GwzSspiHosts-ReviewState-2.md) and
[Surface](GwzSspiHosts-ReviewSurface-2.md) reviews report GO on this exact tuple:

| Repository | Reviewed commit |
|---|---|
| gwz-dev | `a17a7b07becb1e92da5519b64add39c41d6ee09e` |
| gwz-sspi | `14d834b311200b0984e7041e4a34419c59501119` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-core, unchanged reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Final SSPI `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` updates README status only.
Production, dependency, schema and selected fingerprint-input bytes are unchanged
after review. Root landing changes status/ledger and files verbatim reports only.
This accepts plan step 4a only: trusted producer, CLI self-execution, bundled Python
worker, installed descriptors and coherent packaging/publication. It does not
accept core HTTP composition, Windows authentication activation or release.

Initial Code/State NO-GO and Surface GO, all later reports, and the
[merged correction](GwzSspiHosts-RemPlan.md) are retained. Seven finding records
were discovered before acceptance: four P2 and three P3. All are independently
closed: interpreter target defaults, lossless paths, coherent wheel publication,
Cargo rustflags, matrix receipts, option defaults and candidate scratch placement.
One blocking remediation plus one bounded nonblocking caller cleanup; no new
architectural root cause, exact-defect blind convergence or escaped release defect
was established. Zero findings remain open in this packaging object.
Recorded tier: dual Code/State and cold caller Surface. Cumulative implementation
is 25 source/test/workflow files and 1,208 additions, within the recorded ceiling.
Six member and three root implementation documents; mandatory process audit
records are separate. No new runtime owner, wire, dependency or core behavior.

Final focused suites: owner 53 passes/one EILSEQ skip using the previous unchanged
native image. Producer/release/source-boundary checks and Rust library gates pass;
real Darwin wheel/sdist and extracted rebuild/installed/RECORD checks passed before
the Python-only scratch-root cleanup. No fresh final packaged build, Windows
surrogate/hard-link/runtime/Hello or cross-volume execution proof is attributed
to the last source-only tests. Existing unrelated formatting/Clippy debt stays
disclosed. Filesystem scratch-root tests inspect production handoffs with synthetic
subprocess outputs. Publication requires supported hard links and refuses collisions.

Next: settle finite deadline translation and honest native authentication facts,
then implement core identity/CBT/HTTPS lease/route/cancel/retry composition (4b)
with the accepted secret-adapter gates. Full Windows qualification remains NO-GO;
Digest and provider/EPA/trust/proxy/Pageant proof rows retain their own gates.
No push, tag, publication, release or activation occurred. Later sections preserve
the historical implementation/review snapshots; this acceptance controls status.

## Authority and baseline

[Design revision 2](GwzSspiDesign.md) §§2,4–6 controls the worker boundary.
[Plan](GwzSspiPlan.md) step 4 controls composition and its secret/installed caller
review gates. [Native acceptance](GwzSspiNativeAcceptance.md) accepts the worker,
not installed host provenance or remote authentication.

| Repository | Starting commit |
|---|---|
| gwz-dev | `306663037d6992584f88078be5c9db36f4fa4ce4` |
| gwz-sspi | `84266f412b12a31b9b643a51158079a5412470e5` |
| gwz-cli | `f925e1165c2b2d368a00277594450b010a95867a` |
| gwz-py | `5aeff4bfb11da048f1174db2b1045ed0c9b6c80a` |
| gwz-core, unchanged reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Unrelated untracked SSH N2b prompts, route-mapping draft, core bug report and
alpha-timeout evidence remain untouched. Read-only Git inspection is permitted;
all workspace staging/commits use GWZ. No managed workspace file is hand edited.

## Bounded implementation

The artifact producer runs only at packaging time. It resolves package sources
through Cargo metadata, hashes relevant source/contract/build inputs and declared
target/profile/features/toolchain, and supplies the same compiled 32-byte build
fingerprint to the host and its matching worker. Versions or Git commits alone
are insufficient. Runtime environment, repository settings, cwd, PATH, a sidecar
or a runtime executable hash cannot substitute for these trusted compiled bytes.
The supervisor's existing Hello check remains the worker mismatch boundary.

CLI dispatch recognizes the existing private bootstrap before build-info, ordinary
argument parsing, Git, logging or application runtime. It uses the installed CLI
itself. Python packages the minimal worker executable with the native extension;
selection uses its absolute installed package location and never a child Python
interpreter. Both hosts expose a checked installed descriptor for the later core
handoff, with explicit missing/unprovisioned/mismatch handling and no fallback.

The real package, candidate and release-build entry points must use the producer,
including extracted packages. A plain unprovisioned Cargo build remains
Python-free and refuses worker use; it must not be described as a provisioned
artifact. Build caches remain outside all source/evidence members. Public tests
and packaging do not depend on private evidence.

Only host/package metadata APIs and dependency edges necessary for this boundary
are added. No core adapter, unused session facade, new supervisor owner, application
carrier, wire message, HTTP credential policy or timeout behavior is introduced.
All Windows endpoint/candidate guards and Digest refusal remain in place.

| Ceiling | Bound |
|---|---|
| Handwritten production plus test additions | 1,800 lines |
| Production/test files, excluding lockfiles | 20 |
| Documentation files, excluding generated review prompts/reports | 8 |
| New worker/process owners | 0 |
| Wire/schema delta | 0 |

One drafter owns member edits/tests; the lane owner owns root documents, settlement,
native execution and verdict merge. Stop for scope review before exceeding 120%
of a ceiling, adding another owner, changing shared architecture or widening wire
scope. Any increase must state what is cut first. Review tier is dual Code/State,
then installed caller Surface. At most two merged remediation rounds.

### Bounded file-ceiling disposition, 2026-10-04

The original proposed step-4 package included HTTP/core composition; that work,
all native/runtime campaigns and binary-attestation expansion were cut before
the installed-host draft. They remain outside this checkpoint. There is no new
wire, runtime/process owner, core dependency or authentication policy in this
revision, and no further host feature is added.

The owner authorizes the one remaining necessary publish-workflow connection:
its existing raw Maturin build bypasses the new backend and would omit the worker.
The initial 20-file ceiling remains recorded above. The revised file ceiling is
25 production/test/workflow files, excluding locks; actual 24 before that edit,
25 planned after. The extra files close actual candidate staging, registry-pin
regressions and publish entry-point seams, rather than widening product behavior.
LOC and documentation ceilings are unchanged. No further new source/test file is
authorized in this draft; any such need returns for scope disposition. Deferring
the publish seam would leave 4a incomplete and is not an acceptable substitute.

## Checks and acceptance

TDD covers strict metadata and descriptor handling; source/contract/build-input
sensitivity and relocation; silent early refusal before ordinary CLI parsing;
Python installed selection with absent/mismatched workers and no cwd/PATH/env
fallback; executable wheel placement, permissions and RECORD; and extracted
source/package builds. No token or secret is logged. Disabled platform branches
receive scoped source checks. Pure/portable checks do not qualify Windows process
or provider behavior. Existing native evidence is not rerun or reattributed.

Settle the intended library/CLI/Python/root tuple before reviewer dispatch. File
verbatim reports and record actual gates, budget and open findings here at
settlement. A passing host checkpoint does not close plan step 4 or qualify Windows.

## Remaining HTTP composition: concrete characterization

Core currently compiles the candidate endpoint and transport host only under
`all(unix, gwz_transport_candidate)`. Its actual per-operation entry points create
the runtime; adding a supervisor only to the future session HostContext would not
connect authentication to today's requests. Those guards stay unchanged in 4a.

Before 4b, pin down the accepted clock translation: HTTPS connect allowance ends
at connection setup; its network allowance is cumulative active I/O and may be
disabled by zero. SSPI requires one immutable finite absolute deadline across
admission, launch and all rounds. A fresh 401 timer, helper allowance or borrowed
allocation budget would silently change the accepted policy. The characterization
is an open implementation/contract question, not authority for such a change.

The existing transport AuthPolicy/Facts have no native mechanism variant. The
bridge must settle honest native-source/mechanism reporting without labelling
SSPI as Gh or authenticated traffic as anonymous. Internal protocol compatibility
is not an operator constraint, but any amendment still requires review.

Native-tls returns an ordinary dependency-owned CBT buffer, and Hyper owns HTTP
authorization/serialization copies. The bridge must document their ownership
limit, wipe core/library-owned fixed storage and retain actual final-origin CBT;
it cannot promise erasure of all third-party buffers. Identity/helper handoff,
exclusive physical lease, route retirement, cancellation, no post-effect replay
and existing retry/failure provenance remain the later secret-adapter gate.

## Settled draft and local verification, 2026-10-04

Implementation is frozen for review. Actuals: 25 production/test/workflow files,
845 handwritten added lines excluding locks/docs; six member documentation files
plus three root documents. The ninth documentation file corrects the existing
README's stale native-review status and links the installed-host guide (12.5%
variance from the original eight, below the 120% stop). No wire/schema changes,
core changes, new runtime dependencies, supervisor owner or activation.

Shared library packaging values, CLI early dispatch/callable self descriptor,
Python OS-loaded-image descriptor, isolated wheel backend, candidate staging,
sdist/extracted paths, cargo-dist matrix and registry release reconciliation are
implemented. Actual Python publish builds use the same backend. Ordinary Cargo
and editable development installs are explicitly unprovisioned; they preserve
ordinary application behavior and refuse SSPI worker use. Wheel receipts are
diagnostic only. Build environments are copied, and each worker build has owned
output so different artifact sets cannot overwrite its binary before bundling.

Meaningful RED/GREEN cases covered missing APIs, native-fork/vendor hash omissions,
ambient build-environment mutation, worker-output races, extracted backend layout
and registry-pin fixture expectations. The corrected tests use production package
paths or synthetic inputs and isolated test-owned copies, never source/compiler
mutation probes or real user package mutation.

Owner independently passed full locked SSPI all-feature tests/doctests on Rust
1.95, 16-artifact schema verification, producer and CLI release tests, Python
packaging/candidate/release tests and the actual installed-image fixture. The
unprovisioned run intentionally skips its installed row; the supplied Darwin wheel
fixture passes it. Core/CLI/Python conditional-boundary scan passes with no new
debt; the added SSPI scan finds no unbraced conditional occurrences. Diff checks
are clean and standalone SSPI package inputs include the producer/host guide.
The first unpinned regeneration refused a rustfmt-version mismatch; the pinned
1.95 rerun passes, without source changes.

Drafter gates additionally passed strict SSPI Darwin/MSVC/GNU Clippy, CLI Clippy
and early-dispatch tests, Python Rust library tests/library Clippy, changed-file
formatting, standalone SSPI archive verification, actual provisioned CLI build,
Python wheel and extracted-sdist wheel builds. An isolated unchanged Windows
descriptor-module compile supports source inspection only; it is not a complete
Windows host build or runtime test. All ordinary outputs are outside repositories
under `/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/hosts`.

Limits are explicit: Python all-target Clippy still reports existing
`native/src/operations.rs:714` needless_update; newer unpinned SSPI Clippy reports
existing manual_noop_waker. Pinned 1.95 SSPI passes. Existing full-repository format
debt was restored after incidental formatting and is untouched. Main CLI Cargo
packaging refuses its existing versionless core path dependency. Registry release
also needs the unpublished/publish=false SSPI package; local path/archive builds
do not satisfy that prerequisite. CLI's Cargo-resolved standalone lock additionally
reconciles stale fork/IDs/session dependencies and taut 0.10.0 already required by
its manifest; this is disclosed separately from SSPI/zeroize additions. Existing
unrelated third-party pins remain unchanged.

Windows installed Hello mismatch, complete host builds/native execution, Linux
installed-image qualification, remote auth/EPA, HTTP composition, aggregate release
and remote CI remain open. No publish, push, tag, release or activation occurred.
The review tuple is recorded in generated prompts; acceptance requires independent
Code/State and cold caller Surface verdicts on that exact tuple.

## Initial review: remediation required

Code and State NO-GO; Surface GO with one P3. Four P2 and two P3 records
are mapped to one correction and one closure test each in
[remediation 1](GwzSspiHosts-RemPlan.md). Original reports are retained verbatim.
No exact-defect blind convergence occurred. All were found before acceptance;
no escaped release defect is established. The existing budgets and scope apply.
The same reviewers verify their counterexamples after a corrected tuple settles;
the drafter cannot self-close findings. No HTTP/native qualification starts here.

Remediation 1 is implemented at SSPI 14d834b, CLI 0c7dfaf, Python c3f5f7d;
reference core remains unchanged. The merged plan records corrected gates and
platform/filesystem limitations. Cumulative implementation remains 25 files and
1,168 additions, six member plus three root implementation docs, with review
audit records separate. Same-reviewer closure is pending, not accepted.

All three remediation-1 verdicts are GO; four original P2 findings are closed.
The two nonblocking caller details are corrected at Python ded4713 in four
existing files, with production handoff/root tests and owner 53-pass focused
suite (one EILSEQ skip using previous unchanged native image). Same reviewers
perform final focused closure and changed-range inspection; this does not add
another authentication/native proof or change core/runtime/wire scope.

| Repository | Committed implementation |
|---|---|
| gwz-sspi | `e31b17e95defd3468140e9b5f73fef8591766599` |
| gwz-cli | `061385fc009fd173488dd07e18d5b2d5882de05d` |
| gwz-py | `e9e228c0006d7f8b3d44c9657403dec5905c6e90` |
| gwz-core, unchanged reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
