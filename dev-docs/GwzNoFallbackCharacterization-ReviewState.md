# L1-A and L3-A characterization packages — STATE-AXIS REVIEW

**Review object:** L1-A local-fetch characterization and L3-A commit/tag characterization at core `c63f497df29d51ad5d864738fa0056b513c3ab7d`; first-package evidence only; 2026-09-20  
**Baseline:** root `9592320ddf95fe53eef0619398b1f8170943a4cc`; git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Sources were read from committed objects with `git show`/`git diff`.  
**Date:** 2026-09-20  
**Axis:** State — fixture isolation, partial and failure effects, and accuracy of the recorded characterization. Independent, adversarial, read-only. Filed verbatim by the lane owner.

**L1-A verdict: GO** — **P0 0 · P1 0 · P2 0 · P3 0**.  
**L3-A verdict: GO** — **P0 0 · P1 0 · P2 0 · P3 1**. The P3 is nonblocking and does not trigger Code escalation.

---

## 0. Evidence base

I inspected the two package reports, complete owned test modules, integrator-owned test wiring, applicable checkpoint clauses, and committed diffs. Recorded evidence reports three L1 tests and five hermetic L3 tests green; the focused tests were not repeated. Diff checks passed.

The tuple remained unchanged at start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `9592320d...` | `102d3f12...` |
| core | `c63f497d...` | `81483f2d...` |
| git2-rs | `e883be38...` | `25d5d735...` |
| transport | `28f5afb3...` | `bf275d0e...` |
| taut | `733e8a78...` | `bb0694d0...` |

## 1. Findings

### [P3-1] L3 claims index preservation after hook rejection without checking the index

**Location:** `src/git/gitbackend/commit_tag_characterization.rs:158-168`; `dev-docs/GwzNoFallbackCommitTagDesign.md:64-68`.

The test stages `rejected\n`, invokes a failing `pre-commit` hook, and verifies only that the call failed and HEAD stayed at the preceding commit. Its final assertion rereads the immutable first commit’s tree; it does not inspect the current index. The report nevertheless states that the staged change remains available.

If Git cleared, rewrote, or otherwise changed the index after rejection, this test would still pass and the later failure-effects matrix could inherit a false state claim.

Before treating that row as complete, assert that the post-failure index entry for `tracked.txt` still identifies a blob containing `rejected\n` and record that result in the report.

## 2. Invariant analysis

**L1-A:** The fixtures are temporary and use pinned libgit2 1.9.7. The 25-row matrix separates native and backend receivers and verifies exact destination object IDs. The shared-tree fixture reproduces the precise native `InvalidSpec` / `Invalid` / `object is not a committish` state while the existing compatibility route succeeds on an independently seeded receiver. The multi-ref test establishes successful partial publication, missing second destination, and distinguishable malformed/missing-object errors. Unexecuted empty, cancellation, repeated-import, SHA-256, and scale rows are explicitly pending; no route is selected.

**L3-A:** Each case reexecutes in a clean child process and uses a temporary repository with repository-local identity, hooks, editor, signing configuration, and mock signer. Verified transitions cover index-versus-`-a` commit contents, message rewriting, HEAD preservation on pre-commit rejection, advisory post-commit failure, lightweight versus annotated tags, reference-hook visibility, deletion, configured-signing editor failure, signer failure, and absence of failed tag publication. Production methods and policy remain unchanged. The listed identity, signing-success, race, interruption, reflog, platform, and no-Git rows remain pending.

## 3. Risks and next action

Accept L1-A as bounded route-selection evidence and L3-A as bounded design input. Add the L3 index assertion before claiming that failure-effects row complete. Neither GO authorizes a replacement design, runtime change, or production activation.
