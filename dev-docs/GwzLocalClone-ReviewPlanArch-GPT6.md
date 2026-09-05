# Independent review of the local-clone plan and architecture

Reviewer: GPT-6, medium reasoning. Date: 2026-09-05.

This is an independent adversarial document review. I did not communicate with the other reviewer or read their report. I read the workspace instructions and `gwz-core/AGENTS.md`; I changed only this report.

## Inputs and verdict

The following frozen inputs matched their supplied SHA-256 values at the beginning and end of review:

| Input | Revision | SHA-256 |
|---|---|---|
| `dev-docs/GwzLocalClonePlan.md` | 2 | `0baba9530ca3c1f0c2b063d12ee76745d9c8a6a0a7fc8b9660c6bff8764f7da1` |
| `dev-docs/GwzLocalCloneImplementationArchitecture.md` | 1 | `63a3fc2cb60cbbaaef9d0be1e478b91c19a2be0b50e5232e25b6f7897342a9f6` |
| `dev-docs/GwzLocalCloneDesign.md` | 6 | `64807db5b828e4c3a78fc253829ee2591b6d4bb7726d642f7465c045ef57e95d` |

**Ready to enter LCM1.0: yes. Ready for implementation/interface freeze: no.** The plan expressly makes LCM1.0 a feasibility and contract checkpoint, so its intentionally unresolved native primitives, metadata fidelity, source retention, and protocol details are not evidence that the planning exercise failed. The two findings below must inform that checkpoint and its resulting contracts. Neither establishes an implemented data-loss bug: this feature is still a proposal.

Findings: **0 P0, 1 P1, 1 P2, 0 P3**. The P1 is a concrete incompatibility with the inspected merge ownership seam; the P2 is a missing safety-observation contract and adversarial acceptance case.

## Findings

### F1 — P1: the proposed retained endpoint lease cannot simply wrap the existing merge lifecycle

**Locations:** `GwzLocalCloneImplementationArchitecture.md:393–400` (admission before the existing mutation guard; retained endpoint leases), `:436–451` (handoff to the existing merge lifecycle), and `GwzLocalClonePlan.md:217–229` (LCM1.2 bind/fetch and handoff). The relevant LCM1.0 checkpoint is plan `:165–169`.

**Concrete conflict:** admission is to acquire both endpoint workspace locks and retain them through transfer/application. However, the existing mutation guard independently acquires `WorkspaceMutatorLock` at `gwz-core/src/workspace_ops/merge/runtime/mutation_guard.rs:93`. More significantly, `gwz-core/src/workspace_ops/merge/start.rs:73–79` explicitly drops that guard because the v1 lifecycle reacquires a non-reentrant workspace lock. The v1 start then takes a creation lease at `gwz-core/src/workspace_ops/merge/v1_lifecycle/start.rs:64–85`, releases it, and calls the service. The service acquires yet another lease at `gwz-core/src/workspace_ops/merge/v1_lifecycle/service.rs:156–165`.

A straightforward composition of the described services therefore refuses as busy against its own receiver lock. Dropping the family receiver lease to accommodate these calls creates an unprotected handoff. If the index lock has already been released, another cooperating process can admit a receiver mutation or disposal in that gap. Retaining the source lease alone does not protect the receiver. This contradicts the continuous endpoint protection required by design §3.2 and the architecture's own admission claim.

**Consequence:** the principal MVP integration path either cannot run or loses the lifecycle exclusion on which its safety argument relies. This is more specific than deduplicating the two endpoint paths: the same endpoint is reacquired at several internal lifecycle boundaries.

**Minimal remedy:** make LCM1.0 explicitly prove a lease-transfer/borrow interface through the ordinary guard, merge start, record creation, and lifecycle service. One owner must retain the physical receiver lock while the existing checked-artifact/catalog capabilities are derived or passed through sanctioned interfaces. Name the protected files and allowed structural delta; do not merely add an outer guard or release locks around existing calls. Add a two-process test with pauses at both current release/reacquire boundaries that proves receiver mutation/disposal cannot enter, plus an ordinary non-family merge regression. Record this work in the LCM1.1 admission/LCM1.2 handoff package estimates.

**Gate interpretation:** the plan already names admission as a spike, so this does not block entering LCM1.0. It identifies the concrete experiment and interface change that must be resolved before accepting that spike or freezing the proposed layering.

### F2 — P2: the “unsaved chunks” contract omits tracked files hidden by index flags

**Locations:** `GwzLocalCloneImplementationArchitecture.md:230–249` (`observe_work` and its observation contents), `:284–310` (on-disk work promise and ignored-file policy), `:319–322` (detector acceptance cases); `GwzLocalClonePlan.md:173–175` (work-detector contract checkpoint), `:244–275` (LCM2 verifier/deletion gates).

**Concrete counterexample:** a tracked configuration file has `assume-unchanged` set and then receives a unique local edit. Its HEAD/index objects are fully preserved in the root workspace. Ordinary Git status is allowed to trust the bit and omit the edit. The index remains valid and the repository is neither an unfamiliar layout nor unreadable, so the proposal's existing `Unknown` cases do not cover it. A detector implemented by adapting the current status observation can report no dirty work while the history verifier correctly verifies the committed graph. Deletion then loses the unique working-file bytes without a `dirty` waiver.

Git explicitly documents that the assume-unchanged bit permits it to assume worktree/index equality; this is a supported state, not corruption. See [git-update-index, assume-unchanged](https://git-scm.com/docs/git-update-index#Documentation/git-update-index.txt---assume-unchanged). The existing core status implementation calls libgit2 status at `gwz-core/src/git/gitbackend/repository.rs:164–173` and derives `is_dirty` from reported status counts at `:196`. This is evidence that the obvious reusable observation is status-based, not a claim that I ran a libgit2 reproducer. The core already treats index flag semantics as significant in a narrower preservation surface: `gwz-core/src/git/gitbackend/preservation_root/index.rs:318–326` refuses managed facts with `assume_valid`, `skip_worktree`, or `intent_to_add`.

**Consequence:** the new safety library's stated on-disk-work guarantee is stronger than the familiar status port, but the current contract and independent tests do not expose that difference. Explicitly inventorying ignored files alone does not close this hole.

**Minimal remedy:** add a flag-aware physical-work observation requirement. For deletion, either inspect the actual tracked path despite status-suppression flags or conservatively return `Unknown` for flags/layouts the detector cannot safely interpret. Do not clear flags in the live index to perform a supposedly read-only scan. Specify sparse/skip-worktree absent-path semantics separately so a valid sparse checkout is not confused with a removed user file. Add independent fixtures for an edited assume-unchanged file and an edited present skip-worktree file; assert their bytes against the fixture's known contents rather than using Git status as the sole oracle. The classification decision must block deletion or require the appropriate explicit hazard waiver.

**Gate interpretation:** this belongs in the work-observation API freeze and must pass before LCM2 deletion is enabled. LCM1's retain-only scope does not depend on a complete deletion scan, so it can proceed with its documented operation-state subset.

## Other assessed areas

- **Copy feasibility and fallback:** the distinction between classified unsupported-native errors and permission/I/O failures is sound. Resetting an owned partial destination before byte fallback, preserving metadata or refusing, applying exclusions during traversal, and testing actual independence are adequate planning requirements. Whole-workspace snapshot consistency is explicitly excluded under non-cooperating writers. I did not treat that deliberate concurrency envelope as a defect or claim that native behavior has been proved.
- **History and recovery:** the proposed verifier covers root/member/nested repositories, retained reflogs, annotated objects, stash coordination, exact surviving reachability, and recorded original inventories after partial deletion. Witness locks and recovery revalidation are correctly required. Its `Verified` result remains an observation rather than deletion authority. These are strong contracts; I did not validate a concrete graph-walk implementation, evidence decoder, or crash-safe encoding because none is supplied here.
- **Typed boundaries:** keeping generic inspection and copy packages below core, with GWZ evidence decoding and durable ownership in core, is coherent. The five-crate layout is a proposal, not independently accepted structural-gate authority. F1 is the material ownership conflict found in the inspected seams.
- **Source retention:** current records store per-participant source commits (`gwz-core/src/workspace_ops/merge/start/record.rs:71`), which is useful but does not by itself prove physical object retention. The plan expressly requires a source-capture/retention spike before interface freeze. That is an appropriate deliberate gate, not a second finding. Acceptance should distinguish open-record OIDs from durable Git reachability and cover failed handoff cleanup.
- **Milestones and usability:** LCM1.2 completes a useful create/work/integrate/detach workflow; LCM1.1 is accurately labeled a preview. Retaining incomplete trees until deletion proof exists is a justified conservative slice. Moving bare construction and push together into LCM2 produces a usable hub without requiring public clean mode or pull first. The sample member-only versus explicit-root merge distinction agrees with the existing planner (`gwz-core/src/workspace_ops/merge/plan.rs:149–166`).
- **Estimates:** the stage sums agree with the milestone totals. The 37–66 engineer-day estimate is conditional, not a validated forecast. The particularly exposed assumptions are the 2–4-day LCM1.0 checkpoint, the 3–5-day merge handoff, and native CI availability. F1 gives a concrete reason to re-estimate the protected admission work. There is no measured basis here to substitute a different numerical range. The proposal already requires re-estimation instead of silently absorbing a broad rewrite.
- **Independent oracles:** the proposal requires native-path assertions, process-level contention, observable disk results after faults, and invariants beyond a generated transition table. These are appropriate. F2 adds one specific missing independent oracle; further test infrastructure is not a prerequisite to starting the doc/physical spikes.

## Verification and limitations

I read the three complete proposals and inspected the cited core admission, merge planning/start/service, status, index-preservation, and stash evidence surfaces. The supporting core HEAD observed during this review was `2e2f695f30198e08ee71ff655f907e5cfa087254`, which differs from the earlier `c67e0d4…` observation recorded in the plan. Accordingly, the source citations describe the currently inspected tree, not proof about the earlier baseline or a settled implementation tuple.

I ran no implementation tests, platform benchmarks, deletion experiments, or mutation races. I did not audit every public mutation entry point, native primitive, driver parser, schema allocation, or protected-gate allowlist. No claim of platform support, implementation acceptance, or release authorization follows from this review. I consulted the primary Git manual for the index-flag semantics in F2; the other findings use the local source and frozen documents. Final hash verification confirmed the three inputs remained unchanged.
