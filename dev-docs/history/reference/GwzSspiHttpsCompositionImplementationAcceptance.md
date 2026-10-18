# HTTPS SSPI composition — implementation acceptance

2026-10-04. **Accepted at the exact tuple below after original
[Code](GwzSspiHttpsCompositionImplementation-ReviewCode-1.md),
[State](GwzSspiHttpsCompositionImplementation-ReviewState-1.md) and
[Surface](GwzSspiHttpsCompositionImplementation-ReviewSurface-1.md) reported GO;
this accepts the bounded step4b implementation and caller surface only.**
It does not accept Windows activation, full platform qualification or release.

| Repository | Reviewed revision |
|---|---|
| root | `7066ff222abf7995e7e0103e9ad38543173d6079` |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` |
| gwz-sspi | `c88fa0e174b185a43e0d0d0c91660cb0957e380a` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| gwz-cli | `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311` |
| gwz-py | `e0c5af10b33289a455f662680af8ac12fd24f9d3` |
| gwz-core-evidence | `ba70034feaeb384619d48dbfaa06c02df6f508b3` |

## Accepted scope and closure

The [accepted composition contract](GwzSspiHttpsCompositionAcceptance.md) now
has an implemented production graph: original CLI/Python caller capture and
shared Supervisor; one immutable positive HTTPS Open deadline across discovery,
helper lookup, checkout/reuse and native rounds; timeout-zero native refusal;
final-origin prefixed TLS CBT; exact authenticated physical generation;
independent mechanism authority, native Complete and remote acceptance;
publication/effect boundaries; retained Start/session/Finish cleanup and both
core charges; truthful concurrent cleanup observation; visible local native
failure projection through Git/private materialize; and documented teardown.
The default transport offer and Windows activation guards remain unchanged.

The first settled implementation review found four unique P2 defects: CBT
representation, pending Start ownership, concurrent reaper visibility and local
native failure suppression. Two unique P3 issues concerned lint attribution and
caller teardown documentation. Code/State independently converged on both
ownership/counting defects and lint attribution. One
[merged correction](GwzSspiHttpsCompositionImplementation-RemPlan.md) resolved
all findings; original reviewers independently verified their counterexamples.
All findings are closed. No new architectural root cause was found at closure.

Metrics: one initial review and one remediation/closure round; four P2 and two
P3 settled-review defects; no post-acceptance escape observed at this landing.
Implementation-contact regressions and actual RED/GREEN receipts are separately
recorded in [the checkpoint](GwzSspiHttpsCompositionCheckpoint.md). Wall time
spans the parked/restarted session and is not measured as one session.

## Evidence and limits

The affected HTTPS suite passed199/199, including the native production tests;
the focused native boundary tests passed26/26 and private-member matrix8/8.
Both main reviewers reran the focused tests and verified the unchanged real
SSPI request validator, owned abort schedules and concurrent reaper counts.
SSPI doctests passed27/27, including the teardown recipe. Final supported
Python wheel packaging passed6 with one existing macOS fixture skip; actual
loaded ClientHost checks passed2, including overlapping operations. Source
guards, inventories, formatting and whitespace checks passed.

The exact50file source/document fingerprint and11raw receipts are committed
under the private evidence member's
`campaigns/https-integration/runs/2026-10-03-https-sspi-composition-rem1`.
Both main reviewers checked their hashes. Public tests do not require private
archive access. Compiled outputs remain in the external evidence-build cache.
The checkpoint lists exact commands, artifact paths and failed setup attempts.

**Full strict-core Clippy remains RED45.** The introduced guard diagnostic is
gone; the historical RED47/all-baseline assertion is withdrawn. Reviewers
verified44 exact baseline primary snippets and one inherited whitespace-only
match, with the documented limits of that attribution. This accepts neither a
full strict PASS nor a compiled-baseline proof, and does not waive release gates.

Portable tests prove core orchestration and structural SSPI admission; native
sessions in those tests are synthetic. They do not qualify Windows provider
execution, live caller identity, EPA/CBT algorithms, actual worker disposal or
installed Windows hosts. Digest and nonempty initial native offers remain
pre-Begin refusals. Unknown cleanup, including evicted tombstones, remains
charged and can retain capacity indefinitely; it never becomes disposal proof.
Dependency-owned HeaderValue/TLS/provider copies remain outside the declared
owner wiping guarantee.

## Next boundary

Run the real Windows native/provider/identity/CBT/cleanup and installed-host
qualification under the existing release matrix, with fixtures on the operator's
E: volume. Then close the remaining parity/dispositions and the deferred broad
platform, selected-source, performance, packaging and aggregate release gates.
Existing full strict-core debt also remains an explicit gate obligation.
Windows activation and full release remain **NO-GO**. No push, tag, publication
or activation occurred or is authorized by this acceptance.
