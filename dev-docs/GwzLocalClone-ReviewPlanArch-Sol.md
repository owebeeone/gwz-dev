# Independent adversarial review — GWZ local-clone plan and architecture

**Reviewer:** GPT-5.6 Sol, medium reasoning  
**Date:** 2026-09-05  
**Scope:** document and supporting-code review only; no implementation or acceptance testing

## Frozen inputs

The hashes matched at the start of review:

- `GwzLocalClonePlan.md`, revision 2: `0baba9530ca3c1f0c2b063d12ee76745d9c8a6a0a7fc8b9660c6bff8764f7da1`
- `GwzLocalCloneImplementationArchitecture.md`, revision 1: `63a3fc2cb60cbbaaef9d0be1e478b91c19a2be0b50e5232e25b6f7897342a9f6`
- Governing `GwzLocalCloneDesign.md`, revision 6: `64807db5b828e4c3a78fc253829ee2591b6d4bb7726d642f7465c045ef57e95d`

I checked the same hashes again after writing this report; they were unchanged.

## Verdict

**Ready to enter LCM1.0: yes.** The proposal is explicit that LCM1.0 is a feasibility and contract-settlement stage, and it identifies most of the genuinely hard boundaries rather than claiming they are solved.

**Ready for implementation or interface freeze: no.** Three P1 contract defects must be resolved before LCM1.1/LCM1.2 behavior is frozen: included nested Git repositories can retain external metadata, inspection/recovery side effects are contradictory, and fetched source-object ownership is not crash-durable within the current merge lifecycle. Two P2 issues should be settled at the same checkpoint. There are no P0 findings.

The central safety posture is otherwise sound: byte copying is the correctness fallback; native cloning is only an optimization; unknown work/history evidence fails closed; deletion is deferred until proof and recovery exist; path identity is treated separately from a string path; and tests are required at each milestone rather than deferred to LCM3.

## Findings

### P1 — Included unmanaged nested Git repositories can escape the copied lane

**Locations:** design §4.0 lines 258–286 and §4.1 lines 288–320; architecture §4 lines 238–258 and independent tests at 264–268; plan LCM1.1b lines 189–194 and exit lines 207–213.

**Failure/counterexample:** an ignored or untracked submodule-like directory inside the source tree contains `.git` as a file pointing to a Git directory outside that nested directory. The create identity check is stated for the workspace root and materialized manifest members, while the additional-nested-repository inventory is stated for deletion. Verbatim copy includes ignored/untracked content. The nested `.git` file can therefore be copied unchanged and point back into the source lane. An unregistered nested repository with an external `commondir`, alternates, worktree metadata, or path-bearing config has the same problem. The architecture even lists an “ignored nested repository” detector test, but does not route that observation into create admission.

**Consequence:** the destination is reported ready while some copied Git work can mutate or depend on source/external metadata. That violates the advertised independent-lane boundary and can produce later source damage or data loss when either lane is removed.

**Minimal remedy:** at LCM1.0 define a create-time recursive Git-boundary inventory over every included path. For every detected nested repository, either apply the same self-contained identity/config checks and copy rules as a declared repository, or refuse before the creating-row reservation. Add fixtures for ignored nested `.git` files, linked worktrees/submodules, nested alternates, and nested absolute config paths. This is distinct from the later deletion inventory.

### P1 — `local list` is both inspection and potentially mutating/destructive recovery

**Locations:** design §3.1 lines 135–180 (“Every local verb runs [recover]”) and §3.2 lines 204–210 (“Read-only inspection may report the state”); plan LCM1.1c lines 196–201; architecture §6 lines 379–405 and `local_disposal` lines 458–493.

**Failure/counterexample:** a `creating` row is complete but not promoted, missing, or—once LCM2 ships—eligible for automatic incomplete-create deletion. The design says every local verb runs recovery, so `gwz local list` can promote/drop the row or reach automatic deletion. The same design calls read-only inspection report-only, and the architecture says `local_disposal` is the sole service authorized to delete. The plan groups recovery and `list` but never selects one behavior.

**Consequence:** an operator or monitoring process can change family state, and potentially remove a partial directory, by invoking a command presented as inspection. Implementations can also diverge over whether list takes mutation locks, performs CAS, or delegates to disposal.

**Minimal remedy:** freeze an explicit operation matrix. Prefer making `local list` observation-only: it may compute and display a proposed recovery state but must not CAS, remove pointers, drop rows, or delete content. Put state-changing recovery behind create/dispose/disband or an explicit recover action. If auto-recovery on list is deliberately retained, name it as mutating in the command/API, require the full admission/deletion owner, and add side-effect and dry-run tests.

### P1 — Family-merge source refs have no crash-durable owner compatible with record v1

**Locations:** plan LCM1.0 lines 167–169 and LCM1.2 lines 217–235; architecture `local_transfer` lines 436–456.

**Failure/counterexample:** anonymous transfer writes exact source objects/temporary refs into several receiving repositories and the process crashes before the merge record is durably created, or between record creation and handoff. The documents say a failed handoff has one known cleanup owner and that the existing merge lifecycle owns retained source refs, but do not define a durable pre-record owner, ref grammar, recovery discovery, or transition of ownership. Current code does not already supply that contract: `gwz-core/src/workspace_ops/merge/preserve/plan.rs:318-338` creates canonical `refs/gwz/merge/<merge>/<participant>/head` refs for the receiver's protected/rollback commit, and `gwz-core/src/workspace_ops/merge/record_wire/archive/cleanup.rs:202-232` rejects noncanonical refs/evidence. `gwz-core/src/workspace_ops/merge/gc.rs:96-111` likewise accepts only that canonical head shape. These are receiver-preservation refs, not imported source-retention refs.

**Consequence:** a crash can leak refs indefinitely, allow GC to remove imported objects needed by continue/abort, or make the strict existing cleanup path fail closed. Since temporary refs are excluded from disposal-preservation witnesses, an undefined classification also risks either treating a live source as disposable or treating an orphan as permanent history.

**Minimal remedy:** the LCM1.0 spike must produce a complete durable ownership protocol: exact source-ref namespace, create/publish ordering relative to record v1, derivable association with merge id and participant, crash recovery before and after record publication, GC/archive cleanup, and disposal-verifier classification. Demonstrate it against the current strict cleanup code. If this cannot be expressed without a new durable record field/version or coordinator, revise the plan and estimate as its own line 428–430 requires.

### P2 — The preservation proof has no bounded durable representation

**Locations:** design §5.1 lines 422–429; architecture preservation algorithm lines 339–368, family-store lines 393–405, disposal lines 479–486, and complexity lines 541–562.

**Failure/counterexample:** a repository has millions of retained reflog entries/refs. The verifier correctly inventories them, but the proposal persists target identities, protected roots/evidence digests, witness identities, and covering roots in the single root-owned YAML family index. Runtime memory may be bounded by returning `Unknown`, yet no size/count limit, streaming representation, proof schema, or atomic-write bound is defined for the persisted intent.

**Consequence:** disposal can hold the family index and all witness locks while building or CAS-writing an unbounded index, and every later family operation pays to parse that enlarged index. ENOSPC or partial publication occurs at the most sensitive commit point. Different implementations can also persist different evidence and be unable to revalidate after target refs vanish.

**Minimal remedy:** freeze maximum proof/index bounds and a canonical compact proof representation at LCM1.0/LCM2.1. Exceeding bounds must return `Unknown` before disposal intent. If evidence must live outside the index, make it a versioned checked artifact owned atomically by `family_store` and bind its digest in the row; this is one store-owned transaction, not an independent journal.

### P2 — `family_id` is exposed as client request data without authority semantics

**Locations:** design §3 lines 116–119 and protocol lines 585–593; plan LCM1.0 contract checkpoint lines 148–155.

**Failure/counterexample:** a client supplies field 7 containing another family's id, a stale id, or an id different from the caller's admitted family. The prose says the id is minted by the root and a stray path cannot join merely by echoing it, but the wire sketch puts the value in `CloneLocalWorkspaceRequest` and only calls it optional. It does not say the server ignores, rejects, or validates client values.

**Consequence:** generated drivers expose an apparent authority-bearing field with inconsistent behavior, creating spoof/rejoin ambiguity exactly where the pointer/index identity is supposed to be authoritative.

**Minimal remedy:** remove `family_id` from the public input if core can derive it. If transport requires it, specify it as a server-produced/internal bind value and require exact equality with the already admitted root index; any user-supplied mismatch must refuse before mutation. Add new-client/old-core and malicious/mismatched-field corpus cases.

## Deliberate decisions and non-findings

- Source quiescence against editors, direct Git commands, and old binaries is an explicit LCM1.0/product assumption, not an accidental claim of snapshot isolation. It must remain visible in UX and acceptance evidence.
- Refusing gitfiles, alternates, external object stores, unsupported metadata, and missing/promisor objects is a reasonable LCM1.0 scope decision. The P1 above concerns included nested repositories that currently escape that stated check.
- Omitting source `.gwz/merge`, stash coordination, catalog, leases, and manifest during verbatim copy is deliberate identity separation. It narrows what “verbatim” means but is consistently documented.
- Deferring all directory deletion until LCM2 is a strong safety boundary. LCM1 incomplete copies being retained is acceptable.
- Requiring exact reachable history rather than patch equivalence, cached remote state, or unreferenced object presence is conservative and internally consistent.
- Five private path crates may prove too much packaging, but the architecture defers exact placement to the structural-gate checkpoint. That is an implementation tradeoff, not a present defect.
- The supporting core checkout moved from the plan's historical observation (`c67e0d4…`) to `2e2f695…`. Plan lines 93–97 explicitly disclaim that observation as a release baseline and require a fresh settled tuple; lines 362–365 and 421–423 require the corresponding review and entry-point recount. Movement is therefore an already-specified LCM1.0 obligation, not a present defect. It remains evidence that the checkpoint must use a fresh tuple.
- The 2–4 day LCM1.0 estimate and 18–32 day MVP range have low confidence because the native-platform matrix and source-ref fit are unmeasured. However, plan lines 375–383 state the assumptions and exclusions, lines 421–432 require re-estimation and plan revision when those assumptions fail, and platform scope may be explicitly reduced. Without measured contrary effort, this is estimate risk rather than a correctness or planning-contract defect. The P1 source-ref finding supplies the concrete condition that can trigger that re-estimation.

## What I verified and limitations

I read the three frozen documents, `AGENTS_GWZ.md`, and the applicable root `AGENTS.md`. I inspected the current core mutation-lock/guard and merge preservation/ref-cleanup seams cited above. I verified the current supporting heads reported for this review: gwz-dev `27cc4861ad9db7cb5db119105804599a7748e248`, gwz-core `2e2f695f30198e08ee71ff655f907e5cfa087254`, gwz-cli `c04b488efe7f5188e6c4fecebf74a4d63f20113f`, and gwz-py `65fc6678a1df7a0415fcb86891d50120be79033c`.

I did not run implementation tests, native clone/reflink experiments, fault injection, performance benchmarks, protocol generation, or cross-platform checks. No feature implementation exists to accept. I did not validate Apple/Linux/Windows behavior beyond assessing the proposed contracts, and I did not prove that every current public mutator was enumerated. The working tree already contained unrelated `Cargo.lock` and design changes plus the untracked input drafts; I did not alter them.

## Finding count

- P0: 0
- P1: 3
- P2: 2
- P3: 0
- Total actionable findings: 5
