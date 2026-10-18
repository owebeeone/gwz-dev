# GwzSspiSupervisor — State-AXIS REVIEW

**Review object:** Root `526451cea593e7d9550a28b3c86cf09a0dee2e20..255d05e09c0433e27dd7afeaa9f9fe04699cad05`; gwz-sspi `44879481fbd54dab84b99fecdadc89a34a84dcbd..a75485cbdd03607909d11637c07f97548dd7902c`. Controlling document: `dev-docs/GwzSspiSupervisorCheckpoint.md`, **DRAFT implementation checkpoint**, dated 2026-10-03. Focused remediation round 1.
**Baseline:** Corrected root `255d05e09c0433e27dd7afeaa9f9fe04699cad05`; member `a75485cbdd03607909d11637c07f97548dd7902c`; unchanged reference core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Original reviewed root `d2821a2db90aa641b1af6f80cadaf1aba9b35a0c` and member `fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a`. Sources were read through `git show HEAD:` and exact original-to-corrected diffs. All three HEADs matched at start and end.
**Date:** 2026-10-03
**Axis:** State transitions, races, terminal arbitration, retained secret ownership, cleanup proofs and orchestration coverage. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their current-round reports. Filed verbatim by the lane owner.

**Verdict: GO** — both original P2 findings and the original P3 coverage finding are closed. No new finding or new architectural root cause was established.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State P2-1 | Recover the authoritative terminal cause when publication loses its record to reaping. | `futures.rs:164–181` uses `terminal` in the lost-record fallback. Direct and production fake-port regressions cover cancellation/expiry with both token ownership orders, returning the winning error and Confirmed cleanup. | **Closed** |
| State P2-2 | Dispose of the owned challenge on every completed step error path. | `Step::complete` takes and drops the challenge before Ready. All terminal Ready paths use it. Retained-future wipe tests cover oversized, wrong-phase, cancelled and expired challenges; no command is sent. | **Closed** |
| State P3-1 | Exercise production orchestration decisions through deterministic fake ports and owned completion outcomes. | Production launch, read/write, reap and dispatcher-join functions are shared with the new tests. Fake child observations and owner completion gate disposal and capacity; seeded schedules exercise the actual reaper. | **Closed** |

## Changed-range analysis

The member remediation changes 19 files, with 1,385 insertions and 269 deletions. Most additions are regression tests, fake ownership support and documentation.

The production changes fit the merged dispositions:

- `futures.rs` extracts publication and centralizes completed-step input disposal.
- `api.rs` uses precise capture for owned start/shutdown futures; step retains its mutable conversation borrow.
- `owners.rs` introduces a private completion seam. Its production implementation owns the actual `JoinHandle` and delegates finished observation, consuming join and advisory cancellation.
- `monitor.rs`, `io.rs` and `dispatch.rs` extract bounded decisions from existing loops. Production calls those same functions.
- Context task and launch storage now hold the private owner interface.
- Protocol fixture additions are enclosed in a test module. Production wire handling is unchanged.
- Documentation distinguishes synchronous captured-handle disposal, kernel schedules, orchestration schedules and deferred native qualification.

I compared the extracted paths with their original bodies. Process launch, containment checks, pipe ownership, termination, observations, joins and disposal remain with the same charged owners. The production owner port does not substitute synthetic completion or detach its thread handle. Reaping still keeps launch completion false until the dispatcher joins the finished launch owner.

**No NEW ARCHITECTURAL root cause or material contract change was found.** The private completion seam changes testability and representation, without creating a new OS ownership mechanism, public injection API, disposal queue or policy. Precise capture restores the accepted owned-future intent.

Root changes record remediation, clarify caller documentation, file original review artifacts and update the member pin. No behavior outside the merged dispositions was identified.

## 0. Evidence base

The original review’s authority and unchanged-source analysis remain applicable. This round additionally read:

- `dev-docs/GwzSspiSupervisor-PromptState-1.md`.
- Complete `dev-docs/GwzSspiSupervisor-RemPlan.md`.
- Corrected checkpoint and caller-guide changes; current-program checkpoint changes.
- All original-to-corrected member changes.
- Complete corrected `src/supervisor/owners.rs:1–31`, `dispatch.rs:1–69`, `monitor.rs:1–189`, `io.rs:1–178`, and the changed future/API/context ranges.
- Complete `remediation_tests.rs:1–113`.
- Complete `orchestration_support.rs:1–286`, `orchestration_tests.rs:1–401`, and `orchestration_schedule_tests.rs:1–74`.
- Updated Architecture, Testing, Supervision and implementation documentation.
- Public owned-future compilation checks and test-module/fixture wiring.

Executed the permitted prebuilt pure unit binary:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 supervisor --nocapture
```

Result: **29 passed, 0 failed**, exit 0. This includes both focused State regressions, the production fake-port lifecycle, token-readiness/reaping schedules, owner-completion tests and seeded reaper schedules. Synthetic waker, reader and writer panics were caught; their tests passed.

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 protocol::supervision_tests
```

Result: **2 passed, 0 failed**, exit 0.

The permitted binary contained and executed the corrected regression tests. No build was performed to independently recreate it. Owner compilation, lint, schema and archive results were read from the corrected checkpoint and remain owner evidence.

Member status was clean. Root and reference-core untracked files were outside scope. Current-round peer reports/prompts were not read. No files were written, and no build, compiler probe, native process campaign or Git mutation was performed.

## 2. Invariant analysis

**Original terminal-projection counterexample.** Publication still arbitrates through `Context::update`. If the record disappears between readiness and publication, the fallback now asks `terminal`, which recovers the saved completion fault before any synthetic Protocol result. Both direct regressions and the fake-port lifecycle reproduce cancellation and expiry after real token readiness, with token extraction before or after payload retirement. All return the winning error, Confirmed cleanup and no token. P2-1 is closed.

**Original retained-challenge counterexample.** Every terminal Ready path in `Step::poll` reaches `Step::complete`, which takes and drops the challenge outside the state lock before marking the future done and returning. The existing successful preparation path also releases it. Live-before-deallocation probes confirm wiping while failed futures remain retained, for all four requested refusal cases. The command receiver remains empty. P2-2 is closed.

**Production ownership bridge.** The tests now execute production decisions rather than merely assigning complete proof masks:

- Late successful launch after cancellation retains the child, performs no resume or secret write, and requires observations and launch completion before confirmation.
- Containment, resume and launch failures follow their refusal paths.
- Failed termination and incomplete exit/Job observations retain ownership and capacity.
- Reader/writer completion is independently controlled; unfinished owners prevent disposal and confirmation.
- Production reader/writer loops handle errors and caught panics, with frame wiping and port disposal.
- Failed owner-join outcomes do not manufacture native exit or Job emptiness.
- The complete Hello→Begin→Token→Finish→Finished bridge runs through public future polling and production frame decisions.
- Four seeded schedule families drive actual reaper decisions. Child disposal and reader/writer joins precede launch join and capacity release.

The production owner adapter retains the real `JoinHandle`; fake owners exist only behind private test support. P3-1 is closed.

**Extraction adversity.** Comparing old and new control flow found no reversal of cleanup proof direction. Joins remain outside the state lock. Reaper iterations cannot independently establish launch completion. Payload and child retirement occur before the launch owner returns, and the dispatcher records completion after consuming its finished join. Admission, immutable deadlines, first-terminal arbitration, checked IDs and tombstone semantics are unchanged.

**Supporting merged corrections.** Precise capture removes receiver-lifetime retention from start/shutdown while preserving owned contexts and step serialization. The synchronous disposal clarification matches actual unregistered refusal/Drop behavior and is exercised through observable fake origin destruction outside the state lock. Neither change introduces worker work into polling.

## 3. Risks and next action

Fake completion and observation outcomes prove the parent orchestration decisions; they do not qualify Windows thread creation, blocking I/O cancellation, Job behavior, provider disposal or installed-host composition. Those remain explicitly deferred. Seeded tests are bounded schedule evidence, not exhaustive concurrent native execution.

The State gate permits acceptance of this bounded parent-supervision checkpoint on the corrected tuple. The next action is for the lane owner to reconcile the independently returned gates and record acceptance; this GO does not itself qualify native SSPI or authorize activation/publication.
