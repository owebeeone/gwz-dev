# Independent local-clone plan/architecture re-review — GPT-6, round 2

Reviewer: GPT-6 Astra, medium reasoning. Date: 2026-09-05.

## Inputs, independence and verdict

I read the full three frozen inputs, `AGENTS_GWZ.md`, `gwz-core/AGENTS.md`, and my prior `GwzLocalClone-ReviewPlanArch-GPT6.md`. I did not read the other reviewer's report, any round-2 peer report, or communicate with the peer. I inspected current source and ran one disposable Git experiment. Only this report was written in the workspace.

| Frozen input | Revision | SHA-256, matched before and after review |
|---|---:|---|
| `dev-docs/GwzLocalCloneDesign.md` | 7 | `5fe266c7b947f147f3522c8f857d2ec4cb4fed77fd9e09f876b42b2c4a8ec3a8` |
| `dev-docs/GwzLocalClonePlan.md` | 3 | `64579bdec8c3a0341f4403d97206c9293b5aee76a1a60dc113f5de58a4d4c873` |
| `dev-docs/GwzLocalCloneImplementationArchitecture.md` | 2 | `65b4a9a0dcd4ca7ba092235af91d719f0182c19c2825294a8dbb3604b4eba6ae` |

Actual supporting HEADs match the supplied observations: gwz-dev `27cc4861ad9db7cb5db119105804599a7748e248`; core `87207c2e61e18aa11ce494a4113e29d20f6ea465`; CLI `c04b488efe7f5188e6c4fecebf74a4d63f20113f`; Python `65fc6678a1df7a0415fcb86891d50120be79033c`. Earlier baseline movement is not a finding.

**Start bounded LCM1.0: yes. Ready for parallel library implementation: no, not yet. Accept the final implementation/interface: no, not yet.** The simplified architecture is a coherent direction for the authorized best-effort scope. The dry-run boundary correction and the addendum's isolated build/test contracts belong in LCM1.0 setup. There is no implementation to accept, and its fixture exit remains unproved. This is not a recommendation to restore the withdrawn filesystem or lifecycle project.

Final findings, including the parallel-development addendum: **0 P0, 0 P1, 2 P2, 0 P3**.

## Disposition of my prior findings

| Prior finding | Disposition | Evidence and residual obligation |
|---|---|---|
| F1, P1: continuous retained endpoint lease conflicts with merge lifecycle reacquisition | **Closed for this revised scope.** The requirement causing the conflict was deliberately withdrawn. | Design lines 158–171 and architecture lines 233–238, 269–276 require only the family lock around the wrapper, with no outer receiver lock. Existing release/reacquisition at `gwz-core/src/workspace_ops/merge/start.rs:73–79` therefore does not self-conflict with a new receiver lease. Operator quiescence explicitly covers unrelated writers. The ordinary import/merge fixture in plan lines 126–133 still must pass; continuous lease transfer and a global mutator migration are not required. |
| F2, P2: status suppression can hide tracked edits from deletion observation | **Closed at the contract/planning level; implementation validation pending.** | Design lines 370–374, architecture lines 158–172 and plan lines 193–207 explicitly require physical observation or unknown for suppression flags, separate valid sparse absence, forbid live flag clearing, and name physical-byte fixtures. These satisfy the requested correction. They do not claim a detector has already passed those fixtures. |

## New finding

### R2-F1 — P2: the import-before-delegation wrapper bypasses merge's no-write dry-run boundary

**Document locations:** architecture lines 253–264 and 269–272 prescribe fetching persistent import refs before delegating into merge; lines 278–284 retain them after every outcome. Plan lines 174–182 prescribe the same ordering. Its explicit dry-run accommodation at lines 135–140 covers local create only. Design lines 465–484 also imports before invoking the planner/engine, while lines 25–26 preserve existing merge behavior.

**Current source:** `gwz-core/src/workspace_ops/merge/start.rs:48–52` intercepts dry-run before the lifecycle. `gwz-core/src/workspace_ops/merge/start/dry_run.rs:1–18` states the write-nothing guarantee, with a read-only planning backend and no mutator lock. That interception is downstream of the proposed family wrapper.

**Counterexample:** A has a new commit absent from the receiver. Submit a merge-start request with `local_source_name=A` and `meta.dry_run=true`. Following the specified wrapper, resolve A and fetch into `refs/gwz/local-imports/<id>`, then delegate with that ref. The ordinary engine correctly returns a prediction, but the receiver has already gained objects and a persistent ref. Repeating the dry-run can retain another ref each time; a later engine refusal does not remove it either. No concurrent writer or crash is needed.

**Consequence:** a normal existing request mode produces lasting repository mutations while appearing to be a dry-run. The inner engine's existing protection cannot enforce the outer wrapper's behavior. This is a missing branch in a concretely prescribed call order, not a request for stronger crash consistency.

**Smallest correction:** explicitly refuse family merge dry-run at the outer core dispatch before acquiring a family lock that might create a file, minting/importing refs, or any other write. Preserve ordinary non-family dry-run behavior. Supporting a true family prediction can remain a later bounded enhancement; a scratch object store or new planning engine is unnecessary for this version. Apply the same pre-mutation unsupported-mode discipline when implementing family pull or other new local request paths.

**Focused test:** construct a ready family whose source has a receiver-absent commit; issue the direct core request with dry-run and a family selector. Assert a typed unsupported response and exact no-write observations for refs, object inventory, Git configuration/index, family metadata and runtime directories. Also retain an ordinary non-family merge dry-run regression. This belongs in LCM1.0c/LCM1.2 before accepting the adapter interface.

## Other assessed boundaries

- **Retained imports and merge:** the common ref name fits current planning: `merge/plan.rs:174–199` passes one source string to each selected member and, when explicitly selected, root. Different per-repository OIDs under that string are compatible with this structure. The separate namespace and indefinite retention avoid a new source-owner handoff. The plan correctly requires a real conflict/continue fixture rather than treating this source reading as acceptance.
- **Pull is a separate adapter seam:** architecture lines 288–290 and plan lines 258–263 require the existing same-branch/sync semantics. Current pull preparation does not accept an arbitrary source ref by merely changing `policy.remote`: `pull_head_member_preflight.rs:719–745` fetches a remote and constructs `refs/remotes/<remote>/<branch>`; root merge preparation does likewise at `pull_head_merge_preflight.rs:61–91`. A small source-preparation adapter is needed, feeding the captured import ref/OID into existing policy/barrier/application logic; `pull_head_plan.rs:12–17` already represents a source ref and expected OIDs. I do not classify this as a contradictory prohibition on implementation changes: the proposal assigns adapter work to `local_import` and LCM3.3. Final acceptance must exercise member and root paths, fetch-only/ff-only and applicable divergence modes, no second origin fetch, and unchanged origin fallback. The merge-only LCM1.0 experiment does not prove pull readiness.
- **Libraries and ownership — original assessment, superseded on build/test isolation by the addendum:** small core-owned modules provide useful logical test boundaries, but that alone does not satisfy the subsequently explicit isolated-build and parallel-development requirement. R2-F2 below requires concrete independent targets and API ownership. Source-layout refusal covers included nested repositories before reservation. The copy error/fallback and no-hardlink rules retain meaningful independence without requiring portable metadata equivalence.
- **Disposal/lifecycle:** observation-only list, intact-ready deletion, fresh bounded history/work checks, and stopped partial deletion form a coherent one-shot workflow. Keep/disband are explicit metadata-only exits. Unknown evidence still refuses ordinary deletion. No replay proof, durable allocation identity or continuous witness lease is needed under the accepted scope.
- **Milestones and estimate:** stage ranges sum to the stated 8–14, 7–12 and 4–7 days, totaling 19–33. MVP has a useful create/work/integrate/keep exit. The estimate expressly depends on reusable filesystem/Git helpers and excludes new platform infrastructure. The 1–2-day LCM1.0 fixture is a checkpoint, not evidence of actual throughput; pull's 1–2-day stage is particularly dependent on a small source-preparation seam. Re-estimate from that evidence without silently restoring rejected guarantees.

## Verification and limitations

Executed: initial/final SHA-256 checks, read-only source/HEAD inspection, and a temporary Git 2.52.0 experiment. The experiment created two disposable repositories, fetched an exact source OID to `refs/gwz/local-imports/review-check` through a local path with no named remote, ran `git gc --prune=now`, and verified the ref OID and exact retained blob contents. It passed; the temporary directory was automatically removed. This checks ordinary Git retention, not the proposed Rust/libgit2 adapter or merge lifecycle.

No implementation suite, native-copy benchmark, platform acceptance test, deletion fault injection or process-race test was run. The dry-run finding follows the specified wrapper order and actual existing source; no unimplemented family command was executed. I did not claim that inspection proves all protected seams, protocol allocation or platform support. All three frozen input hashes were unchanged after report creation.

## Addendum — explicit parallel-development and seconds-scale test requirement

The operator subsequently required an architecture that permits parallel development, with **each library isolated and independently testable, and focused tests running in seconds rather than requiring the full workspace suite**. This is an additional acceptance criterion, not a measured statement about the current checkout. I reviewed the same frozen proposals against it, plus the current Cargo/library boundaries; I did not read a peer or external F51 report.

**Updated total: 0 P0, 0 P1, 2 P2, 0 P3.** The original dry-run finding and its verdict remain unchanged. **Ready for bounded LCM1.0 setup: yes. Ready to dispatch parallel library implementation against settled interfaces: no.** LCM1.0 should first supply the small compilable/testable boundaries below. This does not require a workspace-wide subsystem extraction.

### R2-F2 — P2: the proposed module-only default does not establish independent build/test boundaries for parallel library work

**Locations:** architecture lines 35–41 permit ordinary Rust modules and make separate packages optional; lines 43–53 identify nine responsibility rows and fixture styles, but no per-library build/test targets. Lines 69–72 propose small ports without fixing their types/ownership. Plan lines 135–145 make LCM1.0 fix APIs/metadata and prove copy/merge fixtures, but do not require compiling each library independently or establishing a fast test command. Plan lines 265–277 describe focused tests and broader regressions without separating the library developer's mandatory local loop from integration acceptance.

**Source evidence:** `gwz-core/Cargo.toml:9–16` defines one library target with unconditional filesystem, Git and generated-protocol dependencies. `gwz-core/src/lib.rs:18–36` includes artifact, Git, model, operation, protocol, runtime, stash, status, workspace and workspace operations in that same library. Its test build also includes the protocol corpus at lines 47–50. There is currently no proposed local-library target in that manifest. This is a build-graph observation, not a benchmark.

**Counterexample:** two developers implement `family_model` and `local_import` concurrently as modules in core, a layout expressly allowed by the architecture. The first runs a filtered library test such as `cargo test -p gwz-core --lib family_model`. An unfinished type change or compilation error in the second developer's module prevents that command from reaching the family-model tests: the common core library/test target must compile before test-name filtering. Even a pure name-validation change remains coupled to core's compiled dependency graph. Individual unit tests without the CLI therefore do not, by themselves, satisfy the newly required development isolation.

**Consequence:** the plan can meet its current wording while still blocking independent developers on unrelated compilation failures and leaving the promised seconds-scale feedback loop unverified. This is not evidence that a filtered core suite takes thirty minutes; no such timing was measured.

**Smallest correction:** add a bounded LCM1.0 deliverable that names an independently compilable test target for every promised library, its owned files and contract types, allowed dependencies, fixture ownership, and exact focused command. Private path crates for the new components are one straightforward option; another layout is acceptable if it demonstrates the same isolation. Keep generic copy/inspection/classification/policy code independent of core and the drivers; keep GWZ decoding, protected writes and merge/pull integration in narrow core adapters. Give those adapters small port/fake fixtures and a separate integration command. Do not extract the existing Git backend or merge lifecycle merely to obtain these boundaries. Resolve shared boundary type ownership once before parallel implementation rather than having implementers independently invent versions of `RepositoryInfo`, work/history observations, errors or import-source vectors.

Specify a seconds-scale budget as a **target**, then measure it on a named development machine. Report focused test execution, warm edit/build/test turnaround, and cold dependency build separately. A practical initial target could be at most ten seconds for the normal warm focused loop, subject to explicit agreement and measurement; this report does not establish that target as achieved. Native platform smoke tests and merge integration/regressions may use separate commands and budgets. They remain acceptance obligations without becoming every pure-library edit's test prerequisite.

**Focused acceptance test:** scaffold the isolated targets and compile their contracts using small fakes before full implementation. Run each documented focused command and record timings/test counts. In a disposable checkout, introduce a deliberate compile error in an unrelated sibling library and verify that the selected independent library's focused target still builds/runs when there is no declared dependency on that sibling. For a library with an intentional dependency, test against the agreed stable dependency API; independence does not mean ignoring real dependencies. Confirm the targeted invocation selects no CLI or full-workspace suite. Run one small adapter integration fixture separately to show that the ports compose.

### Packaging and ownership disposition

The existing responsibilities, pure `family_model`, read-only detector/history boundaries and one `family_store` owner are a useful foundation. The architecture also correctly avoids a mandatory rewrite of the whole backend. The missing artifact is a concrete package/target and API ownership map that developers can compile independently, together with reproducible focused commands and measured budgets. That work fits the bounded setup checkpoint, though its effort should be re-estimated rather than assumed to fit the original 1–2 days automatically.

Addendum verification was read-only inspection of the current Cargo manifest/library declarations and another before/after hash check. No library implementation exists to benchmark, and no full workspace or thirty-minute suite was run. The three frozen proposals remained unchanged; only this review report was amended.
