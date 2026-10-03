# HTTPS SSPI composition acceptance

2026-10-04. **Accepted design contract only** at the following reviewed tuple:

| Repository | Reviewed HEAD |
|---|---|
| root | `721e07d65aa78a8bd79d41dae86ad99629a7aefc` |
| gwz-core | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-sspi | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |

[Consistency re-verdict](GwzSspiHttpsComposition-ReviewConsistency-1.md) and
[first completed Safety review](GwzSspiHttpsComposition-ReviewSafety.md) are GO
with no open findings. Consistency independently closed P2-1: mechanism authority
is preserved on Continue and native Complete is tracked independently. The
original [Surface GO](GwzSspiHttpsComposition-ReviewSurface.md) remains applicable:
the corrected caller guide changes only the pending/selected disposition status,
not its API, ownership, lifecycle or proposed timeout behavior. Consistency's
changed-range analysis independently verifies that classification.

The operator delegated the zero-timeout decision to the owner. Effective zero
supplies no finite deadline and refuses native selection before Begin or
credential publication. Anonymous, existing Basic and SSH zero behavior is
unchanged. Positive native setup uses one immutable logical Open deadline,
captured before first checkout/adoption; discovery, helper work and native rounds
cannot pause, reset or extend it. Active HTTP I/O accounting and retry eligibility
remain separate. These are implementation obligations, not executed evidence.

The accepted [composition contract](GwzSspiHttpsCompositionDesign-DRAFT.md)
authorizes the bounded remainder of step 4b: owned original-caller capture, native
transport policy/facts in taut, final-origin CBT, exclusive HTTPS lease and
native lifecycle/cleanup composition through the actual CLI/Python entry paths.
No new application carrier, process owner or generic worker framework is added.
The implementation ceiling and stop conditions in §9 remain controlling.

Review ledger: one blocking documents-only finding, one bounded correction,
one focused Consistency remediation round, zero new architectural causes and
zero escaped implementation defects established. Safety's earlier quota failure
produced no report/verdict and counts as an incomplete attempt only. Reports are
filed verbatim. Implementation requires its own Code/State secret-adapter review
and installed-caller Surface check, with meaningful production-bridge tests.

Windows endpoint guards remain intact. Native provider/EPA/Digest/trust/proxy,
installed Windows, performance, source/distribution and aggregate release gates
remain separate. This acceptance authorizes no push, tag, publication, Windows
activation or release.
