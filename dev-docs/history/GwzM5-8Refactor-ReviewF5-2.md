# GWZ M5–M8 Refactor Proposal — Independent Re-Review (F5-2)

Date: 2026-07-30. Reviewer: Claude (Fable 5).

Subject: `dev-docs/GwzM5-8Refactor.md`, revision "Review 7 and independent F5
incorporated" (2,984 lines; prior reviewed revision was 2,610).

Method: full re-read of the revised document against the prior revision (held
in context from the F5 review) and against gwz-core at the same baseline used
for F5. As before, the numbered reviews (including Review-7) were not read;
the comparison basis is the document itself, the code, and my own F5 findings.
Findings first; §2 gives the disposition of every F5 item; §4 records the
consistency checks performed across the restructure.

---

## 1. Findings

No P0, P1, or P2 findings. Three P3 findings, all small and suitable for the
next editorial pass or the I2 checkpoint.

### [P3-1] v3/M7 lacks the checkpoint parity that v2 and v4 received

The restructure gives M6 and M8 named full checkpoints — I6 and I8 — each of
which explicitly freezes "wire fields, … unknown-field retirement, archive
projection, and actual … downgrade fixtures" for its version
([:2048](GwzM5-8Refactor.md), [:2114](GwzM5-8Refactor.md)). For v3 the same
responsibilities are scattered and partially unnamed:

- §16.5 ([:1756](GwzM5-8Refactor.md)) refers to "the M7 source checkpoint",
  but no such package exists in §17;
- §22.3.19 assigns proof-gate ownership to "I6/M7/I8" — an implementation
  package standing in for a checkpoint;
- §22.5.2 requires "the v3 source contract has received its own explicit
  re-review before M7/A3", which covers re-review but not the freeze;
- the M7 package ([:2081](GwzM5-8Refactor.md)) freezes snapshot-provenance
  semantics and adds the v3 archive decoder, but never names the v3
  unknown-field retirement entries, the v3 archive-projection freeze, or the
  freezing (as opposed to running) of the actual-A1/v2 downgrade fixtures —
  precisely the items I6/I8 enumerate for their waves.

**Impact:** the intent is discernible, but the unenumerated freeze items are
the ones most likely to be skipped when M7 is implemented long after I2.
**Correction:** either add a small I7 checkpoint mirroring I6/I8's freeze
list, or add the three missing freeze bullets to the M7 package and name it
consistently as the v3 checkpoint in §16.5 and §22.
**Test/gate:** A3's activation list already runs the matrices; the fix is to
name who freezes them first.

### [P3-2] "Eligible" open-v0 migration is used but never defined

§22.4.9 ([:2830](GwzM5-8Refactor.md)) and §24 ([:2971](GwzM5-8Refactor.md))
now say "**eligible** open v0 uses only the approved atomic migration" and
"only **eligible** approved open-v0 migration is upgraded". The only defined
ineligibility is `mode: no_ff` → `UnsupportedLegacyMode`
([:1150](GwzM5-8Refactor.md)). Left open: whether a `RecoveryRequired` v0
record (§15.3.2 row L) or a record carrying operation-level drift is
migration-eligible. Row L forbids "automatic mutation", and whether the
migration rewrite itself counts as such a mutation is exactly the kind of
ambiguity this document elsewhere refuses to leave open.

**Impact:** A1's migration scope is under-specified at the margin where a
wrong answer either strands recoverable records on v0 indefinitely or
rewrites records the row-L contract says to leave untouched.
**Correction:** one sentence in §15.6 or §15.1.1 defining the eligibility
set — e.g., "any open v0 record that classifies into a §15.3.2 row other than
`UnsupportedLegacyMode` is migration-eligible, including `RecoveryRequired`,
because migration changes only the record representation and next-action
equivalence is separately proven" — or the conservative converse. Either is
defensible; it must be chosen explicitly.
**Test:** a `RecoveryRequired` v0 fixture through the A1 migration path,
asserting whichever disposition is chosen.

### [P3-3] New typed results are not tied to the append-only protocol enum

The revision introduces `UnsupportedLegacyMode`
([:1151](GwzM5-8Refactor.md)), and relies heavily on
`UnsupportedRecordVersion` and `ArchivedRecordUnreadable`, all projected
cross-driver through JSON/JSONL (§18.7, §18.9). The document never states
that these join the append-only `GwzErrorCode` wire enum with pinned values —
the discipline the codebase applied to every merge error code to date, and
which §4.4 requires generically. **Correction:** one line in I2's freeze list
allocating the new codes (and their machine-output projection) alongside the
envelope registry. Minor, but the document is otherwise meticulous about
exactly this class of contract.

---

## 2. Disposition of F5 findings

Every F5 finding and both minor observations are addressed; none partially.

| F5 finding | Disposition in this revision |
| --- | --- |
| **P1-1** `--no-ff` cannot ship as v0 | **Fixed — recommended option 1 adopted end-to-end.** M5 split into M5a (custom messages, v0) and M5b (no-ff, v1, activation disabled until A1). §15.1 now states the M5 split with the exact evidence chain (v0.10.x decode-but-rederive-fast-forward; ≤v0.9.2 unknown-field flatten) and names the dormant `NoFf` variant "a compatibility liability, not grandfathered proof" [:1074]. v1's required semantics now include executable no-ff [:1097]; the creation matrix gained a `--no-ff` column with M5a → unsupported [:1130]; open v0 `mode: no_ff` records are rejected as `UnsupportedLegacyMode` before migration/mutation [:1150]; R0 captures the failing v0.9.2/v0.10.2 fixture up front [:1809]; R2a forbids making no-ff writable [:1843]; M5b reconciles from the exact persisted v1 action "rather than re-deriving merge mode from live analysis" [:2007] — the root cause, addressed architecturally; A1 activates writer, migration, and the no-ff start path as one reviewed change [:2836]. Ripple verified through §§1, 2.2, 3, 6, 15.1, 15.1.1, 16.1, 17, 18.6, 18.8, 19, 20, 21, 22.2/22.3/22.4, and 24 — no stale "M5 remains a v0 writer" text survives. |
| **P2-1** gate under-scopes values/variants/flatten | **Fixed.** §15.1 renamed "Same-version newly writable shape gate", scoped to "field, field value, enum variant, nested action, or combination—even when the type was predeclared" [:1036], with decode generations (field-unknown vs variant-known) named explicitly [:1043] and wired into the harness, guardrails [:2516], stop conditions [:2656], and a risk row [:2589]. |
| **P2-2** `merge/mod.rs` unowned | **Fixed.** §13 adds a responsibility→owner table for mod.rs (dispatch, open gate, mutation guard, store seam, persistence helpers) with the measured 1,158-line baseline; §14 map gains `runtime/` and `store/`; R1 performs the move behavior-preservingly [:1824]; acceptance §22.1.6, guardrail [:2568], stop condition [:2711], and risk row [:2614] added. |
| **P3-1** message-freeze owner ambiguity | **Fixed.** "R2a alone freezes the exact final message bytes" [:1672]; §22.2.3 makes R2a "the single owner". The impossible interface-checkpoint reference is gone. |
| **P3-2** retained-binary mechanics unspecified | **Fixed.** New §15.8: machine-readable manifest (tag, decode-generation label, artifact URL, SHA-256, platform, expected behavior), immutable tagged artifacts only, Linux+Windows lanes matching the release line, digest-verified content-addressed cache, bounded non-interactive invocation, fail-not-skip, and a reviewed-decision rule for support-window changes. R0 establishes it; I2 freezes it; §18.10 enforces it per release. |
| **P3-3** AV0-F unrepresentable | **Fixed.** `ArchivedMergeProjection { source_version, terminal_outcome, acceptance }` makes terminal outcome a sibling of acceptance availability [:1366]; a stop condition now forbids the old shape [:2681]. The revision goes further: version-specific persisted-archive decoders (AR-P), `UnsupportedRecordVersion` archives protected from older status/retention/GC mutation (AR-U), and "version-inapplicable ≠ LegacyUnavailable" [:1410]. |
| **P3-4** I2 gated on full M6+M8 designs | **Fixed.** I1 is now a short directional memo (M6 checkout-evidence and M8 lock-domain directions only); full policy moved to new I6/I8 checkpoints before v2/v4; the rationale is recorded verbatim in I2: "this deliberate decoupling prevents persisted acceptance, no-publication archival, and v1 no-ff safety from waiting on the hardest future product policies" [:1919]. §23 retitled accordingly. |
| **P3-5** Review-7 missing from basis | **Fixed.** Basis lists all seven numbered reviews plus F5, with the supersession relationship stated. |
| Minor: "six layers"/seven questions | **Fixed** ("seven layers" [:242]). |
| Minor: no LOC budgets | **Fixed, stronger than suggested:** R0 creates a change-budget ledger with numeric LOC/file ceilings and declared wire/protocol deltas per package; >20% overrun, an unlisted production module, or a changed wire delta stops the package [:1765]. |

## 3. Verdict on the revision's substance

The correction chosen for P1-1 is the right one, and its execution is notably
complete: the failing fixture is captured in R0 (so the evidence is durable
before any code moves), v0-safety of custom messages is stated with its
mechanism (recorded `commit_message` as recovery authority — matching what I
verified in code), and M5b's "reconcile from the exact persisted action, never
re-derive mode from live analysis" eliminates the class of bug, not just the
instance. The new §15.2 ordered disjoint envelope classifier and the
versioned-archive decoding (AR-P/AR-U) are genuine improvements beyond my
findings — presumably Review-7 material — and both are internally consistent
with the rest of the versioning contract.

## 4. Consistency checks performed

- **No-ff ripple:** searched every `no_ff`/`no-ff`/`NoFf` occurrence (24
  sites) and verified each against the M5a/M5b/A1 split; no contradictory or
  stale statement remains, including §3's wave table, §6's intent notes, and
  §24's final decision.
- **Creation matrix vs prose:** §15.1.1's table, §16.2–16.4's per-wave
  version statements, §18.8's matrix rows, and §22.4/22.5's assertions agree
  (M5a → v0 + no-ff rejected; A1+ → v1 floor; no-ff → v1; features → v2/v3/v4;
  combined → max; existing records never recomputed).
- **Checkpoint coverage:** every freeze item from the old monolithic I1/I2 was
  traced to a new owner — I1 memo, I2 (v1), I6 (v2), I8 (v4, including the
  preservation journal's M8 scope, previously I1's) — with the single v3 gap
  reported as P3-1.
- **ff-only safety cross-check (new):** confirmed in code that `mode: ff_only`
  records remain old-reader-safe across *both* decode generations — ff-only is
  fully enforced at planning, and M4's retry rules (unchanged before-commit)
  make mode re-enforcement at continue unnecessary by construction — so the
  revised v0 definition ("M4 integration plus custom messages") is sound and
  no analogous ff-only finding exists.
- **Envelope classifier:** walked the five priority classes against edge pairs
  (`v1/99` → unreadable; unknown string/valid number → unsupported, no wave
  claim; allocated-unsupported → unsupported with wave) — disjoint and total.
- **§15.8 vs release reality:** Linux+Windows lanes match the repository's
  actual release-test platforms; v0.9.2/v0.10.2 are the correct minimal
  generation pair per the tag archaeology from F5.

## 5. Final assessment

**The revision fully incorporates the F5 review; the immediate scope (R0, R1,
R2a, M5a) is now internally sound and safe to begin.** The three new P3s —
v3 checkpoint parity, the undefined migration-eligibility set, and protocol
allocation of the new typed results — are refinements that fit naturally into
the next editorial pass or I2's freeze list, and none blocks R0/R1/R2a/M5a.
