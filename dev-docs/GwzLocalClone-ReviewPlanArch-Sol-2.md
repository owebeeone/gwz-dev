# Independent adversarial re-review, round 2 — GWZ local-clone plan and architecture

**Reviewer:** GPT-5.6 Sol, medium reasoning  
**Date:** 2026-09-05  
**Scope:** report-only review of the frozen design, plan and architecture against the observed supporting source; no feature implementation or acceptance testing

## Frozen inputs and observed source

The requested SHA-256 hashes matched before review:

- `GwzLocalCloneDesign.md`, revision 7: `5fe266c7b947f147f3522c8f857d2ec4cb4fed77fd9e09f876b42b2c4a8ec3a8`
- `GwzLocalClonePlan.md`, revision 3: `64579bdec8c3a0341f4403d97206c9293b5aee76a1a60dc113f5de58a4d4c873`
- `GwzLocalCloneImplementationArchitecture.md`, revision 2: `65b4a9a0dcd4ca7ba092235af91d719f0182c19c2825294a8dbb3604b4eba6ae`

The supporting heads also matched the review request: gwz-dev `27cc4861ad9db7cb5db119105804599a7748e248`, gwz-core `87207c2e61e18aa11ce494a4113e29d20f6ea465`, gwz-cli `c04b488efe7f5188e6c4fecebf74a4d63f20113f`, and gwz-py `65fc6678a1df7a0415fcb86891d50120be79033c`.

I treated the operator's best-effort scope as review authority. I did not require a catalog, persistent filesystem identity, persisted preservation proofs, deletion replay, import pruning, continuous merge-lease integration, or global mutator migration. I read all three current inputs, `AGENTS_GWZ.md`, the applicable repository instructions, my preceding Sol review, and current source at the stated heads. I did not read another round-2 or external-review report and did not communicate with a peer reviewer.

The source trace concentrated on the existing merge request validation, mutation-guard dispatch, plan/start path, Git transport seam, pull preflight, protocol allocation, and CLI request construction. I also checked the newly stated requirement that the architecture enable parallel development with each library isolated and its focused tests completing in seconds.

## Disposition of the five preceding Sol findings

| Prior finding | Round-2 disposition | Evidence in the revised inputs |
|---|---|---|
| Included unmanaged nested Git repositories can escape the lane | **Closed for the prior counterexample.** Create now inventories every included declared, ignored and unmanaged repository before reservation and refuses gitfiles, alternates, external object/config paths and unsupported layouts. | Design 4.0 lines 206–236 and acceptance line 827; plan lines 154–159; architecture lines 144–150 and 168–172. The preflight wording for relative `core.hooksPath` remains inconsistent, as finding 1 explains. |
| `local list` could mutate or delete during recovery | **Closed.** List is explicitly observation-only; state-changing repeats are explicit commands. | Design lines 135–156 and 828; plan lines 51, 151–164 and 217–220; architecture lines 240–249. |
| Imported source refs had no durable pre-record owner | **Closed under the accepted best-effort scope.** Fresh `refs/gwz/local-imports/<transfer-id>` refs are ordinary persistent refs, retained indefinitely and deliberately outside merge cleanup. The common ref name fits the existing planner because `source_ref` is resolved independently in each participant; the v1 record already freezes each participant's source OID. | Design lines 463–497; plan lines 126–133 and 172–186; architecture lines 251–295. Current source: `workspace_ops/merge/plan.rs` lines 174–217 and `start.rs` lines 63–72. The anonymous explicit-refspec transport API does not exist yet (`git/gitbackend/transport.rs` lines 45–63 exposes named-remote fetch only), but proving that small seam is expressly the LCM1.0 spike, not a contradiction. |
| Preservation proof had no bounded durable representation | **Closed by removing persistence.** The proof is bounded, in-memory evidence for one quiescent invocation; limit exhaustion is `Unknown` before `disposing`. | Design lines 364–368; plan lines 191–207; architecture lines 174–218. |
| Public `family_id` had unclear authority | **Closed.** Core derives it from admitted index/pointer state and field 7 is explicitly not public request authority. | Design lines 100–108 and 521–529; plan lines 135–139 and 329–331; architecture lines 240–244. |

## Findings

### P2 — The preflight rule inconsistently scopes relative escaping `core.hooksPath`

**Locations:** design 4.0 lines 206–236, especially lines 217–221; design lines 775–778; plan lines 39–41 and 154–159; architecture lines 144–150.

**Counterexample:** the source repository has `core.hooksPath=../../outside-hooks`. The detailed admission list at design lines 217–221 singles out an **absolute** `hooksPath`, so an implementation following that enumeration can admit this relative value past preflight. After copying, Git resolves it outside the repository and can execute an external hook. In a disposable repository I configured exactly that value, placed an executable `pre-commit` hook two directories above the repository that exits 77, and observed `git commit` fail with exit 1 because the external hook ran.

Design lines 231–236 separately say destination completion fails if destination config still names any path outside destination, and architecture lines 144–149 broadly require escaping metadata/configuration to refuse before reservation. Read broadly, those clauses prevent `ready`; they do not resolve the specific preflight enumeration. The concrete failure is therefore inconsistent admission timing: one conforming reading refuses before reservation, while another copies first and fails destination completion, leaving an avoidable `creating` row and partial directory. An implementation that interprets “names a path” literally rather than as the effective resolved hook path could also miss the escape.

**Consequence:** preflight behavior and partial-effect timing can differ between implementations for a common Git configuration, contrary to the plan's requirement that nested Git escape paths refuse before reservation. The broader completion rule protects the ready-state boundary only if effective relative paths are resolved consistently.

**Smallest correction:** make the admission rule resolution-based for every effective `core.hooksPath`, whether absolute or relative, after modeling its destination location. Refuse before reservation if the effective hook directory escapes that repository's copied boundary. Apply the same check through allowed Git config includes; do not introduce a new identity or durability mechanism.

**Focused test:** create a source fixture with a relative escaping `core.hooksPath` and an external sentinel hook; assert aggregated refusal occurs before the family row or destination is created. Add a control whose relative hook path resolves inside the copied repository and remains independent after copy.

### P1 — The architecture does not yet provide the required parallel library/build/test isolation

**Locations:** architecture lines 35–53 and 69–72; architecture lines 220–248 and 297–326; plan lines 102–106 and 116–146.

**Counterexample:** architecture lines 37–41 permit all named components to remain modules in `gwz-core` and say separate Cargo packages are not a prerequisite. A filtered `cargo test -p gwz-core <one-test> --lib` still builds the complete `gwz-core` library. On this checkout, the single filtered merge-validation test executed in 0.00 seconds but the command took 22.65 seconds after recompilation. More directly, the proposed ownership is overlapping: `family_store` owns pointer/allocation-marker writes (architecture lines 227–231), while `workspace_install` is also assigned the fresh pointer and allocation marker (lines 299–305). No function-level API is given for `family_store`, `local_import`, `workspace_install`/`repo_factory`, or `local_disposal`, and the dependency diagram omits install/factory/disposal. Two parallel implementers therefore cannot tell who owns those writes or validate their unit without coordinating edits and a full-core build dependency.

**Consequence:** the proposal does not satisfy the explicit requirement that each library be isolated, independently testable in seconds, and suitable for parallel development. It also leaves a concrete double-writer seam at the most sensitive create publication boundary.

**Smallest correction:** make LCM1.0 exit include a checked-in component map with one owner and one narrow interface per named library. Give each library an independently invocable build/test target whose dependency closure excludes the full `gwz-core` integration layer; private path crates are the straightforward Rust mechanism, while a demonstrably equivalent isolated target is acceptable. Keep GWZ policy adapters and the existing merge call in a thin core integration layer. Define `family_store` as the sole writer of index, pointer and allocation-marker bytes; `workspace_install` should request those writes through the store port. Add the missing dependency edges and concrete request/result/error types, including cancellation and partial-effect reporting. Set a per-library focused-test wall-clock target in seconds and measure it in a clean incremental development cycle; do not describe the current unimplemented estimates as measurements.

**Focused test:** for every library target, run its documented test command after touching one source file and record build plus execution wall time. A CI check should fail if that command pulls in the CLI or full-core integration suite. Add a compile-time fake-store test proving `workspace_install` cannot directly write family metadata, plus parallel branch fixtures that change separate libraries without sharing source files.

### P2 — Family merge does not define non-mutating dry-run admission before retained-ref import

**Locations:** design lines 465–496 and acceptance lines 838–840; plan lines 126–145 and 172–186; architecture lines 251–284. Current source: `workspace_ops/merge/runtime/dispatch.rs` lines 188–213 and `workspace_ops/merge/start.rs` lines 48–52.

**Counterexample:** `gwz --dry-run merge --remote A` enters the documented family wrapper, which captures and fetches commits to persistent `refs/gwz/local-imports/<transfer-id>` before delegating to the existing engine. The existing engine's dry-run branch plans and returns without a record or lifecycle mutation, but the family wrapper has already created permanent refs. The documents explicitly settle create dry-run refusal as an option, yet give no equivalent family-merge rule or pre-import request-validation matrix. The same ordering can import refs before the existing engine rejects another malformed start request.

**Consequence:** a command presented as merge dry-run can permanently change every selected receiver and consume disk, contrary to the existing merge dry-run behavior the proposal says remains intact. Invalid requests can also leave avoidable partial effects before semantic admission.

**Smallest correction:** validate the complete family-merge start request before any import. For LCM1, explicitly refuse family merge with `dry_run=true` before writing refs; this is smaller than adding a non-mutating cross-repository object-analysis path. A later read-only implementation can be designed separately.

**Focused test:** snapshot all receiver refs, run family merge dry-run and malformed family start cases, and assert a typed refusal plus byte-for-byte unchanged refs and no transfer call. Retain the existing test that a valid non-dry-run partial import may leave refs on transfer or engine failure.

## Verdicts

**Starting the bounded LCM1.0 feasibility/contract stage: yes.** Its purpose is to prove the ordinary-copy and retained-import seams and settle the API before feature implementation. The three corrections above fit that bounded stage and require no rejected durability machinery. Finding 2 must be resolved before parallel LCM1.1/LCM1.2 work is dispatched; findings 1 and 3 must enter the authoritative contract and focused tests before their respective behavior is implemented.

**Accepting a final implementation or freezing the public/internal interface: no.** There is no feature code, and the relative-hook admission rule, parallel library boundaries, write ownership, seconds-scale test targets, and family-merge dry-run rule remain unsettled. Subject to those corrections, the retained-import design is a feasible small adapter to the current merge planner and record model, and the one-shot disposal contract is coherent with the operator's accepted manual-cleanup outcome.

## Tests run and limitations

I ran one disposable Git experiment for relative `core.hooksPath`; it confirmed that `../../outside-hooks` escapes the repository and executes the external hook. I ran one existing filtered core test, `workspace_ops::merge::validate::tests::accepted_field_matrix_covers_every_operation`; it passed, with 1 test run and 1,728 filtered out. Its measured wall time was 22.65 seconds after recompilation, of which the test harness reported 0.00 seconds execution. That is evidence about the current monolithic build boundary, not a benchmark of the unimplemented local-clone libraries.

I did not run the full implementation suite, native clone/reflink tests, fault injection, protocol generation, cross-platform tests, or performance benchmarks. No local-clone implementation exists to test. I did not attempt an exhaustive source audit or prove all effective Git-config escape forms; finding 1 uses one concrete supported relative form. Existing dirty working-tree files belonged to the surrounding review work and were not modified. Historical baseline movement was already acknowledged and is not a finding.

I rechecked all three frozen input hashes after writing this report; they still matched the values recorded above.

## Finding count

- P0: 0
- P1: 1
- P2: 2
- P3: 0
- Total actionable findings: 3
