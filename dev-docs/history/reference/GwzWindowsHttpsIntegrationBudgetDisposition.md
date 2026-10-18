# Windows HTTPS integration — WH1 file-budget disposition

2026-10-04. Lane-owner disposition before implementation. Accepted scope,
2,600 handwritten added-line ceiling, no new dependency/wire/API/runtime owner
and all stop triggers remain unchanged. WH1 file ceiling is **40 source/test/
build files**, replacing the initial 24-file estimate; movement is separate.

Descoped: no generic pool/reservation rename or replacement, no configured
helper Job/path implementation (WH2), no new caller authentication selector,
no ordinary Windows activation, no broad Windows SSH/Pageant or performance
package. WH3 live proof still follows the boundary; it is not folded into a
bigger public test framework to justify this count.

Actual source inventory shows nine core guard sites, five CLI route/selection
sites and four Python route/dispatch/capture sites before implementation,
plus build check-cfg declarations, core endpoint/helper/setup boundaries,
private budget separation, native proxy capture, and meaningful guard/admission
and caller tests. A 24-file ceiling would force arbitrary slices of one platform
selection boundary. This allowance records existing distributed wiring, not
additional functionality. Mechanical changes must be exact and syntax-scoped;
existing disabled-branch debt is disclosed rather than claimed repaired.

Implement the single cohesive WH1 package and obtain dual Code/State review.
Stop for any new architecture/owner/interface or >120% growth; do not use the
40-file allowance to absorb WH2 or skip actual Windows checks. No publication,
push, tag, production activation or release approval.


Implementation-contact allowance, 2026-10-04: the TDD closure identifies up to
44 source/test/build files, with three mechanical switch inventories counted
separately. The lane owner permits that bounded growth (110% of the 40-file
estimate) while retaining 2,600 added lines and identical scope. The extra sites
are actual helper exclusion, neutral settings/capture and meaningful admission/
backend tests; no runtime owner, public API, dependency, WH2 or generic refactor
is added. Stop above 44 actual source files, the line ceiling, or any structural
trigger. Record final exact count before review; this is not release acceptance.

Native compiler disposition, 2026-10-04: the unpatched WH1 library and provisioned
CLI compile, but compiling the native unit-test binary produces 38 errors from
older Unix-only test imports and fixture dependencies. The drafter stopped at
44 files and identified five additional owning files: `placement_endpoint.rs`,
`transport_setting.rs`, `transport_host/https_endpoint.rs`,
`transport_host/request.rs`, and `transport_host/cleanup_tests.rs`. The owner
allows **49 source/test/build files** for this concrete closure. Changes to those
five files are limited to enclosing Unix fixture suites and retaining portable
ownership coverage with a local executor helper. No production primitive,
caller/API/wire, WH2 helper support or ordinary activation is added; the 2,600
added-line ceiling remains. This replaces the earlier file-count stop trigger
before those edits begin. Stop above 49 or on any functional/architectural scope
growth. Native failure and corrected regression evidence must accompany review.


Review remediation disposition, 2026-10-04: original Code/State NO-GO identifies
three concrete implementation omissions within accepted WH1. The owner allows
**55 source/test/build files**, plus the existing three switch inventories,
for round 1 before edits: the existing 49 plus capacity installation, native
Open publication/retained deadline scopes and their meaningful production-path
tests. Retain the 2,600 gross added-line ceiling. This bounded allowance permits
no new architecture, owner, API, schema, helper support or ordinary activation.
The drafter must list actual owning files/count and stop before exceeding either
ceiling or changing a structural invariant. All findings form one patch and
remain open until their original reviewers verify correction and regressions.

Round-2 disposition, 2026-10-04. The operator approved one shared-interface addition for State P2-3: `gwz-transport` `mux::asynchronous::Owner::send_if`, a neutral guarded send whose admit check runs under the mux lock. gwz-transport therefore joins WH1's changed members. The allowance stays at **55 source/test/build files**, plus the three switch inventories, and 2,600 gross added lines, counted across all members. Adding this method allows no other API, owner, dependency, schema, wire, helper support or ordinary activation. Exceeding either ceiling, or any further structural change, stops the patch for a new disposition. Fresh Code and State reviewers review round 2 ([remediation plan 2](GwzWindowsHttpsIntegrationImplementation-RemPlan-2.md)).

Operator decision, 2026-10-04: **the file-count budget is dropped.** The operator said: "drop the file budget, keep the 500 LOC after splitting an rs file that is >1000LOC". No file-count ceiling applies to WH1 or later WH work. The size rule stays as `docs/SplitPolicy.md` states it: a Rust source file over 1,000 lines is split by responsibility, and the files it splits into are at most 500 lines. For the record, the earlier ceilings (24, 40, 44, 49 and 55 files) were set by the lane owner, not the operator. Round 2 carried 55 forward. WH1 touched one Rust file now over 1,000 lines, `gwz-core/src/git/endpoint/https_worker/native.rs` (1,847 lines), which the size rule requires to be split.

Operator decision, 2026-10-04: **the package line ceilings are dropped as well.** The operator said: "yes, drop them and record it", answering a proposal to drop WH1's 2,600-line ceiling and WH2's and WH3's ceilings. The design draft's §7 asserted those ceilings without deriving them. What remains:
- the structural stop triggers: new public API, wire, dependency or runtime owner, an ownership crossing, or an unexpected change to a supported mechanism;
- the operator's step rule: steps of about 500 lines, aspirational and not a hard limit;
- the size rule: a Rust file over 1,000 lines is split into files of at most 500 lines.

WH2 and WH3 are planned as steps of about 500 lines, not as single large packages.
