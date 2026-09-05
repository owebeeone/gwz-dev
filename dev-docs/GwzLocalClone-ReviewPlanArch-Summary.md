# Local-clone plan and architecture — review reconciliation

Date: **2026-09-05**. Status: **review complete; findings open**.
This reconciles two independent adversarial reviews of the proposed
documents. It is not an implementation acceptance verdict or a claim
that the findings have been fixed.

## Result

Both reviewers agree: **ready to enter LCM1.0 for investigations and
contract work; not ready for interface freeze or subsequent feature
implementation**. The existing plan deliberately makes LCM1.0 the place
to prove the physical and integration boundaries.

| Reviewer | Reasoning | P0 | P1 | P2 | P3 | Report |
|---|---|---:|---:|---:|---:|---|
| GPT-6 Astra | Medium | 0 | 1 | 1 | 0 | `GwzLocalClone-ReviewPlanArch-GPT6.md` |
| GPT-5.6 Sol | Medium | 0 | 3 | 2 | 0 | `GwzLocalClone-ReviewPlanArch-Sol.md` |
| Combined | Seven distinct actionable findings | 0 | 4 | 3 | 0 | Findings remain open |

Each reviewer started in a fresh context with the same instructions and
hashed inputs. Neither read the other's report or communicated with the
other reviewer. The lead read both completed reports and checked the
supporting evidence. Sol subsequently reassessed two of its own planning
findings against the original inputs; no peer findings were supplied.

## Findings to carry into LCM1.0

The R-n labels below are reconciliation identifiers, not replacements
for the standalone report titles/severities. All seven remain open;
the final column identifies the required contract/test work, not closure.

| ID | Severity / source | Concrete issue | Required resolution |
|---|---|---|---|
| R-1 | P1 / GPT-6 F1 | Family admission retains the receiver lock, but current merge start and lifecycle independently release/reacquire a non-reentrant lock. An outer guard can self-conflict; releasing it creates a gap. | Define transfer/borrowing of the admitted lease through guard, record creation and service; test competing mutation/disposal at both handoff boundaries. |
| R-2 | P1 / Sol nested Git | Create admission covers root/declared members, while verbatim copying also includes unmanaged nested repositories whose Git metadata may point outside the destination. | Inventory included nested Git boundaries during create; validate them or refuse before reservation. |
| R-3 | P1 / Sol list/recovery | “Every local verb recovers” conflicts with read-only inspection and can make list perform CAS, pointer removal, or later automatic deletion. | Specify an operation/side-effect matrix; recommended resolution is observation-only list, with mutations routed through admitted lifecycle handlers. |
| R-4 | P1 / Sol source refs | The existing canonical rollback-ref grammar is not an imported-source retention protocol. Ownership before merge-record publication and through crash cleanup is unspecified. | Prove a source-ref namespace, durable association/publication order, recovery discovery and GC/archive cleanup compatible with the chosen record format. |
| R-5 | P2 / GPT-6 F2 | Index flags can hide unique tracked-file bytes from ordinary status, despite valid index/HEAD and preserved committed history. | Observe flagged paths physically or refuse as unknown; distinguish valid sparse absence and test edited present paths without modifying the live index. |
| R-6 | P2 / Sol proof representation | The persisted protected-root/witness proof can grow without a defined bound, even though the runtime verifier may have memory limits. | Bound and version the durable proof/index representation; fail before disposal intent if limits are exceeded. Any external proof artifact must have one sanctioned owner. |
| R-7 | P2 / Sol family id | Optional public `family_id` request data has no explicit trust/equality rule against the admitted family. | Prefer core-derived identity, or require exact equality with the admitted root; reject mismatches before mutation and add protocol cases. |

R-1 and R-4 are distinct: retaining the physical workspace lease does
not establish Git object/ref ownership, and preserving source objects
does not close an admission gap. R-2 concerns create-time independence;
the broader inventory already proposed for deletion does not supply it.

R-4 elaborates a spike already required by plan LCM1.0, rather than
establishing that a new record version is definitely necessary. Its
concrete contribution is the current cleanup grammar that the spike must
accommodate. If it cannot, the plan already requires revision and
re-estimation. R-5's complete deletion scan is needed before LCM2;
LCM1 can retain its explicitly limited operation-state detector.

## Evidence checked by the lead

These checks corroborate document risks. They are not tests of a
local-clone implementation, which has not been written here.

- Inspected `gwz-core/src/workspace_ops/merge/start.rs:73` and
  `v1_lifecycle/start.rs:64`: the code explicitly releases guards because
  the workspace lock is not reentrant. The lifecycle service acquires a
  further lease at `v1_lifecycle/service.rs:156`. This supports R-1.
- Ran a disposable Git fixture with a committed tracked file. Set
  `assume-unchanged`, edited the physical file, then ran
  `git status --porcelain=v1 --untracked-files=all --ignored=matching`.
  Status was empty while the file bytes differed from HEAD. Repeated
  with `skip-worktree`, with the same result. This supports R-5; it does
  not claim a libgit2-specific reproducer was run.
- Ran a disposable copy fixture containing a nested repository made
  with `git init --separate-git-dir`. An ordinary tree copy retained the
  nested gitfile; both copies resolved to the same Git metadata outside
  the destination. This supports R-2's boundary counterexample, not a
  claim that a proposed GWZ implementation has already accepted it.
- Inspected `gwz-core/src/workspace_ops/merge/preserve/plan.rs:318`,
  `record_wire/archive/cleanup.rs:223`, and `gc.rs:97`. The current
  canonical preservation ref is `refs/gwz/merge/<merge>/<participant>/head`;
  cleanup validates that shape. An imported-source protocol needs an
  explicit compatible ownership/cleanup design, supporting R-4.
- R-3 is a direct inconsistency between the design's recovery and
  inspection clauses. R-6 and R-7 are missing format/authority contracts,
  not executed failure demonstrations.

Temporary fixture directories were removed after the experiments.
No repository source/configuration was modified by those experiments.

## Planning observations that are not additional defects

Sol initially classified two further observations as P2, then withdrew
those classifications after checking the existing plan language:

- The code baseline moved after the recorded planning inspection. The
  plan explicitly treats that tuple as historical and requires a fresh
  settled tuple/public-entry-point audit at LCM1.0; this remains an exit
  obligation, not a newly discovered defect.
- The native/platform and merge integration estimates have low confidence.
  The plan already states assumptions, exclusions and re-estimation
  triggers. There is no measured contrary effort supporting a replacement
  estimate. R-1/R-4 provide concrete experiments that can change it.

This reconciliation preserves the independent reviewers' final findings;
it does not silently close a finding because the lead considers it a
future-stage concern. No design/plan/architecture corrections were made
during this review request.

## Reviewed inputs and limits

| Document | Revision | SHA-256 |
|---|---|---|
| `GwzLocalCloneDesign.md` | 6 | `64807db5b828e4c3a78fc253829ee2591b6d4bb7726d642f7465c045ef57e95d` |
| `GwzLocalClonePlan.md` | 2 | `0baba9530ca3c1f0c2b063d12ee76745d9c8a6a0a7fc8b9660c6bff8764f7da1` |
| `GwzLocalCloneImplementationArchitecture.md` | 1 | `63a3fc2cb60cbbaaef9d0be1e478b91c19a2be0b50e5232e25b6f7897342a9f6` |

All three hashes matched before and after review. Supporting heads
observed: gwz-dev `27cc4861ad9db7cb5db119105804599a7748e248`, gwz-core
`2e2f695f30198e08ee71ff655f907e5cfa087254`, gwz-cli
`c04b488efe7f5188e6c4fecebf74a4d63f20113f`, gwz-py
`65fc6678a1df7a0415fcb86891d50120be79033c`. The existing `Cargo.lock`
modification and input draft changes were left untouched.

These are hashed proposal reviews, not a settled implementation/release
acceptance. No native clone benchmarks, deletion fault matrix, Rust/Python
implementation suite, or cross-platform acceptance was run. Next work is
to resolve the listed contracts and prove the LCM1.0 spikes, then review
the resulting exact interface candidate before proceeding.
