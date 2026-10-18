# Local-clone plan and architecture — round-2 reconciliation

Date: **2026-09-05**. Status: **review complete; three distinct concerns
remain open**. The operator's best-effort scope is controlling. During
review the operator also required parallel development, isolated library
build/tests, and focused tests that run in seconds.

## Verdict

Both reviewers support starting the bounded **LCM1.0 setup and contract
work**. Neither accepts dispatching parallel library implementation or
freezing the final interfaces yet. No feature implementation is accepted.
The remaining corrections do not require a new filesystem catalog,
durable identity framework, deletion replay or merge lifecycle redesign.

| Reviewer | Model / reasoning | Final counts | Report |
|---|---|---|---|
| GPT-6 | GPT-6 Astra / medium | 2 P2; no P0/P1/P3 | `GwzLocalClone-ReviewPlanArch-GPT6-2.md` |
| Sol | GPT-5.6 Sol / medium | 1 P1, 2 P2; no P0/P3 | `GwzLocalClone-ReviewPlanArch-Sol-2.md` |

There are **three distinct concerns**, not five: build/test isolation and
dry-run admission were found by both reviewers. They differ on isolation
severity: GPT-6 calls it P2; Sol calls it P1, also identifying unresolved
family-metadata write ownership. That disagreement is preserved here.

## Remaining corrections

| Concern | Evidence | Small correction |
|---|---|---|
| Parallel development and test isolation | Architecture permits all libraries to remain modules in core; a filtered test still compiles the shared core target. Package/API ownership, complete dependency edges and per-library commands are not fixed. | Give each promised library an independently compilable target, owned files/types, allowed dependencies and focused command. Keep core adapters thin. Establish one family-metadata writer and have install call its port. |
| Family merge dry-run | Import refs are written before delegation, while the existing engine's no-write dry-run check occurs after that wrapper. | Validate requests before import; initially refuse family merge dry-run before any write, including creating a lock file. Keep ordinary merge dry-run unchanged. |
| Relative hooks-path admission | Detailed preflight singles out absolute hooks paths, while the broader rules prohibit any effective escaping path. Relative `../../outside-hooks` demonstrates the difference. | Resolve effective hook paths, absolute or relative, for the destination layout and refuse escapes before reservation. Keep internal relative-hook paths valid. |

The hooks finding is an admission-rule inconsistency, not proof that the
broader destination-completion rule permits an escaping ready clone.
Resolving it requires a small path-policy correction and fixture, not a
new filesystem identity protocol.

### What LCM1.0 must provide before parallel work

- A complete component/dependency map including install, factory and
  disposal, with a single owner for each shared API and family write.
- Independently compilable new library targets; private path crates are
  one straightforward option. Unrelated core/CLI code must not be in each
  library's development build closure merely because tests use a filter.
- Concrete input/result/error and partial-effect interfaces, plus small
  fakes so components can develop against stable contracts.
- One documented focused test command per target. Treat seconds-scale
  execution and warm edit/build/test turnaround as targets to measure;
  report cold dependency builds separately.
- Small integration commands separate from the normal library edit/test
  loop. Broad existing acceptance tests still run at integration boundaries.

This is packaging and interface work for the new components. It does not
require extracting the existing Git backend or merge lifecycle. Real
declared dependencies remain dependencies; isolation means unrelated
unfinished components cannot break a library's build/test target.

## Previous findings

The originating reviewers dispositioned **all seven previous findings
closed at the revised contract/planning level**: continuous receiver
lease integration was deliberately removed; imported refs now remain
ordinary persistent refs; list is observation-only; nested repositories
are inventoried; hidden tracked edits require physical observation or
unknown; proofs are bounded in memory rather than persisted; family id
is derived in core. Implementation tests for those requirements remain
future obligations.

The new isolated-build requirement is stronger than merely having unit
tests callable without the CLI. It was explicitly sent to both reviewers.
GPT-6 added a focused addendum and updated its aggregate verdict. Sol
included the requirement before completing its report. Neither read the
other round-2 report or an external F51 review.

## Evidence and limits

- Both reviewers traced the current merge planner: the same import-ref
  name can resolve to different per-repository source OIDs, which the
  existing record already stores. GPT-6 also ran a disposable Git import
  and ordinary-GC retention check successfully.
- Both identified the downstream existing merge dry-run boundary. The
  lead checked that source too. No unimplemented family command was run.
- Sol reproduced an escaping relative hook in a disposable Git repository.
  Its report distinguishes that physical behavior from the document's
  broader destination refusal rule.
- Sol ran one existing filtered core validation test: one passed, 1,728
  filtered out; **22.65 seconds command wall time after recompilation,
  0.00 seconds reported test execution**. This is not a benchmark of the
  proposed libraries and does not establish a thirty-minute test run.
- The lead confirmed the current Cargo manifest has one core library
  target and that core's test build includes wider modules/corpus code.
  No runtime target for a future isolated library has been achieved yet.

No full workspace suite, native-platform acceptance, deletion matrix or
local-clone implementation acceptance was run. F51 was not launched in
this task; the operator received a prompt for that independent third review.

## Exact reviewed inputs

| Document | Revision | SHA-256 |
|---|---|---|
| `GwzLocalCloneDesign.md` | 7 | `5fe266c7b947f147f3522c8f857d2ec4cb4fed77fd9e09f876b42b2c4a8ec3a8` |
| `GwzLocalClonePlan.md` | 3 | `64579bdec8c3a0341f4403d97206c9293b5aee76a1a60dc113f5de58a4d4c873` |
| `GwzLocalCloneImplementationArchitecture.md` | 2 | `65b4a9a0dcd4ca7ba092235af91d719f0182c19c2825294a8dbb3604b4eba6ae` |

Supporting heads: gwz-dev `27cc4861ad9db7cb5db119105804599a7748e248`,
core `87207c2e61e18aa11ce494a4113e29d20f6ea465`, CLI
`c04b488efe7f5188e6c4fecebf74a4d63f20113f`, Python
`65fc6678a1df7a0415fcb86891d50120be79033c`. Historical baseline movement
is acknowledged, not a new defect. All reviewed input documents remained
unchanged; only review reports were added or updated.
