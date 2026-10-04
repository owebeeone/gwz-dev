# Windows HTTPS integration — WH1 design acceptance

2026-10-04. **Accepted at root `1ddbfca026347c37deb135934d3b1af610aee781`
after Consistency-1 and Safety-1 both reported GO. This accepts the bounded
WH1 qualification-only design, not implementation, runtime qualification,
ordinary Windows activation or release.** Product/evidence tuple is unchanged
and listed verbatim in both review reports.

[Controlling design](GwzWindowsHttpsIntegrationDesign-DRAFT.md),
[Consistency closure](GwzWindowsHttpsIntegration-ReviewConsistency-1.md),
[Safety confirmation](GwzWindowsHttpsIntegration-ReviewSafety-1.md),
[initial reports](GwzWindowsHttpsIntegration-ReviewConsistency.md) and
[remediation](GwzWindowsHttpsIntegration-RemPlan-1.md) record the complete cycle.
One dual initial review and one bounded correction round. Consistency P2-1 is
independently closed: the actual qualification request backend selects Disabled
through the existing constructor and retains its host context. No CLI/Python
helper-disable option exists or is added. Normal callers use fetch/fetch_stream.

Both axes identified the same nonblocking P3 method-name error. The landing
corrects `request_kind` to the actual `TransportRuntime::open_request`; both
request and request_with_token converge there. Independent P3 text confirmation
is pending; this is not owner self-closure or a blocking design defect.

Only composition §9's first paragraph is superseded as specified in design §1:
actual Windows endpoint/caller selection is permitted solely under candidate
plus Windows qualification cfg in disposable artifacts. All other composition
invariants remain controlling. Ordinary/candidate-only Windows and Unix selection
remain unchanged; configured helpers/SSH/proxy are refused in this subset.

Next: WH1 implementation, behavior/guard/caller tests, native MSVC build and dual
Code/State review; then real TLS/SSPI/Git and installed CLI/Python qualification.
WH2 needs its own physical helper Job/path proof and accepted contract. WH3
results are not supplied by this design acceptance. No push/tag/publication.
