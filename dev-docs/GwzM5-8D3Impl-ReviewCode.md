# M5-8 D3 durable-cursor implementation — CODE-axis review (round 1)

Date: 2026-08-22. Axis: **Code** (adversarial, round 1 of a two-round cap;
program-level dual gate, peer-blind — the State report was not read).
Object: **the complete `workspace_ops` working-tree delta over gwz-core
`0b059e8`** (`git -C gwz-core diff 0b059e8 -- src/workspace_ops`, 36 tracked
files, +1,229/−100, **plus** the untracked new test file
`src/workspace_ops/merge/model/v1/validate/preservation_marker_tests.rs`,
304 lines, which the tracked-diff numbers do not include).
Contract: `GwzM5-8DurableCursorAmendment.md` (ACCEPTED 2026-08-16), read end
to end, including the four contracts' 2026-08-16 acceptance annotations and
the operator-escape amendment's accepted 2026-08-22 annotations
(`GwzM5-8OperatorEscapeAmendment.md` §1, §2.3, §3.3.2, §5.4, §6.1, §7
items 2/3). Tree caution honored: concurrent-lane files under
`src/checked_artifact/**` were treated as out of scope and judged nowhere in
this report.

Line numbers cite the working tree under review unless marked `@0b059e8`.

---

## 1. Method and gate evidence

Clause map derived independently from the diff (not taken from the
implementer's report). Every production hunk was read in full; every changed
test file was read in diff form; the new marker test file was read whole.
Gates were executed on the working tree with
`CARGO_TARGET_DIR=<scratchpad>/d3rc-target`:

- **fmt** — `cargo fmt --check`: exit 0, no output. (The checkpoint's
  driver-lane disclosure — a whole-crate fmt slip canonicalizing whitespace
  in up to five D3 in-progress files — leaves the tree fmt-clean; nothing to
  attribute.)
- **clippy** — `cargo clippy --all-targets`: finished clean; the only
  "warning" line is cargo's pre-existing workspace-profile note about
  `gwz-cli/Cargo.toml`, not a code lint. Zero code warnings.
- **release build** — `cargo build --release`: green (27s).
- **release merge-suite spot subsets** (per the mandate; recorded full run
  559/0): the test binary enumerates exactly **559** tests under
  `workspace_ops::merge`, matching the recorded population. Spot re-runs,
  all release, all green — **195 passed / 0 failed** across:
  `preservation_marker_tests` 13, `prefixed_preservation` 7,
  `reverse::preservation::tests` 32 (contains both U3 demonstrations, the
  clean-owner marker pin, and the misnomer test),
  `unknown_fields` 32, `record_wire::archive` 28 (contains the four new
  cleanup marker tests), `journal_vocabulary` 2,
  `dispatcher_reconciliation` 6, `v1_lifecycle::tests::effect` 10,
  `tests::authority` 8, `continue_v0_gate` 3, `merge::store::tests` 19,
  `g23::gc` 5, `g23::preserve` 13, `characterization_preservation_v0` 5,
  `v1_lifecycle::archive::gc_tests` 6, `no_ff_wire` 6.
- **Concurrency disclosure.** Midway through the gate runs the shared tree
  stopped compiling (`checked_artifact/namespace/tests_managed.rs`, nine
  E04xx errors — the live R2-D lane's half-written file; zero errors in
  `workspace_ops`). Subsequent subsets were executed directly against the
  already-built release test binary; `find -newer` confirmed **no
  `workspace_ops` source is newer than that binary**, so the binary embeds
  exactly the object under review. A machine-wide disk-full event
  (`/System/Volumes/Data` at 100%) was mitigated by pruning this review's
  own scratch build artifacts only.

**F-1 gate:** `src/workspace_ops/tests/g23/continue_v0_gate.rs` — `git
status` empty (byte-identical over `0b059e8`); its exactly-3 tests green in
release (`v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair`,
`v0_resume_rejects_forged_no_ff_mode_row`,
`durable_two_parent_action_over_divergent_pair_still_resumes_and_merges`).

**M5b files:** the five Rust files of the M5b package other than
`store/tests.rs` (`v1_lifecycle/tests/forward.rs`, `tests/mod.rs`,
`no_ff_determinism.rs`, `no_ff_wire.rs`, `reverse_no_ff.rs`) — `git status`
empty on all five: byte-identical. `store/tests.rs` carries exactly one
mechanical two-line marker addition (counted below).

**The 12 mechanical lines:** verified exactly
`noop_commit: None, reset_commit: None` and nothing else moved, at six
two-line sites: `store/tests.rs:475-476` (1 site),
`preserve/artifacts.rs:385-386` (1 site), `tests/g23/gc.rs` (2 sites),
`tests/g23/preserve.rs` (1 site),
`tests/g23/characterization_preservation_v0.rs` (1 site).

---

## 2. Clause map (derived from the diff)

| Amendment clause | Implementation |
| --- | --- |
| §2.1 field delta | `model/v0.rs:164-173` — `noop_commit`/`reset_commit`, `Option<String>`, `#[serde(default, skip_serializing_if = "Option::is_none")]`, appended after `stash_object_id` on the shared row struct |
| §2.2 rules 1/2 + equations + empty row | `model/v1/validate/preservation.rs:121-128` (empty-row extension), `:137-172` (`validate_markers`: rule 1, rule 2, both value equations over `is_oid` = 40/64 lowercase hex at `:448-450`), multi-row rejection retained at `:86-88` |
| §2.2 pending-action cross-checks | `preservation.rs:192-196` (BackupRef × `noop_commit`), `:304-307` (Stash × `noop_commit`), `:249-256` (ResetAttachedRef × `reset_commit`) |
| §2.2 immutability | reducer-enforced: `reduce/preservation.rs:316-341` (`marker_write_preserves_prior_row`) checked at both marker arms (`record_marker`, `:274-314`) and at the reset retirement (`finish`, `:230-247`); plus footprint confinement (`footprint.rs:208-219`, `:264-268`) so non-owning transitions cannot move a marker past `verify_known_diff` |
| §2.3 known-key fork | `unknown_fields/extract/common.rs:12-45` (`EvidenceKeys::{V0,V1}`), threaded from `extract/mod.rs:36` (v0) and `:71` (v1) |
| §2.3 in-row collision leg | `unknown_fields/mod.rs:80-96` in `map_v0_to_v1`, keyed on identity kind `"preservation_evidence"` (matches `identity.rs:261-267`); migration-path only |
| §3.1 noop edge | `cursor.rs:44-46` (`artifact_noop_needed`, `:342-349`) → `artifact_noop_intent` (`:395-421`), one write per dispatch, write-ahead, degraded-record convergence by the same loop |
| §3.1 reset edges | executed: `phase.rs:410-430` (evidence attached at `R::Complete`, backfill predicate at `:423`) + `reduce/preservation.rs:229-247` (retirement now **requires** `reset_commit`); no-op: `cursor.rs:52-55` → `reset_noop_intent` (`:423-449`) |
| §3.1 backfill values | `cursor.rs:362-393` (`marker_row`): noop = recorded `backup_commit` else `plan.anchor`; reset = `plan.anchor`; present markers never revalued; validator mirrors via `owner_anchor` (`preservation.rs:396-420`), which is derivation-identical to the plan anchor (`plan.rs:288-308`, `:423-431`) for all three owner kinds |
| §3.2 decode-terminal prefix | `cursor.rs:255-266` (backup: pair ∨ noop ∨ live), `:283-305` (stash: noop; stash pair ∧ no pending stash at that position via `stash_action_pending_for` `:273-281`; live image fallback), `:310-335` (reset: `reset_commit`; live fallback verbatim). Root-reset carve-out dissolved into the general rule — the old `:264-275 @0b059e8` special case is deleted and the pending-reset shape now reads decode-complete through the stash-pair arm |
| §3.3 guard rescope | `reject_later_durable_owner` byte-unchanged (`cursor.rs:61-77`); unreachability for noop-bearing owners is structural: both call sites (`:30-37`) are guarded by `!backup_complete` / `!stash_complete`, and the noop arms return `true` first. The later-owner probe (`v1_owner_evidence(..).is_some()`) counts marker-only rows, so the retained live arm also covers post-amendment later shapes |
| §4 live fallback | all three `*_complete` fallbacks are byte-equivalent to the pre-amendment observations (verified against `@0b059e8`) |
| §5 terminal plane | `archive/cleanup.rs:167-178` (`collect_owner` marker-aware emptiness arm); markers contribute no worklist entries; see finding P2-1 for the v0 leg |
| §5 bundle identity | structural: `checked_bundle.rs:146-151` skips rows without stash ids (unchanged); see P2-2(iv) for the missing pin |
| Erratum (post-GC rewrite) | confirmed in code: `merge/gc.rs:196` — `post_gc_record(record).to_response(context)?` shapes the **response projection only**; the archive is deleted at `:205`; nothing is written back. The implementation correctly adds **no** post-GC rewrite, matching the escape amendment's §5.4 correction |
| §3.1 vocabulary | `transition/mod.rs:261-267` (`RecordArtifactNoop`/`RecordResetNoop` inside `PreservationTransition`, Boxed per the enum idiom), `effect.rs:66-67`, `footprint.rs:165-171`, resolver plumbing (`resolver.rs:503-508`, `resolver/observation.rs:31-36`), tokens (`authority.rs:694-699`) |

**Escape-amendment interaction (both directions), verified:** the delta
adds **no** top-level `V1Transition` arm; the two new arms live inside
`PreservationTransition` and collide with none of
`MarkOwnerUnrecoverable`/`BeginRollbackOverridden`/`AbortOperationOverridden`
(escape §2.3.1's separate `EscapeTransition` sub-enum). A
case-insensitive sweep of the diff for
`disposition|Unrecoverable|operator_override|EscapeTransition` matches only
the pre-existing `response.disposition()` API. The cursor neither implements
nor blocks the escape §7 item 2/3 mark-consumption texts: completeness is
factored into three per-position predicates (`backup_complete`,
`stash_complete`, `reset_complete`) that can each gain the third
retirement-fact arm (durable fact → live proof → mark) without
restructuring, and `exhausted` (`cursor.rs:577`) still issues the
complete-domain `VerifiedPreservationExhausted` only — the escape's
`…ExhaustedOrMarked` remains a distinct future proof, unimpeded.

---

## 3. Findings

### P0 — none.

### P1 — none.

### [P2-1] The release-compiled v0 cleanup leg adopts fabricated markers, flipping a fail-closed retention into a proceed

`archive/cleanup.rs` — `collect_owner` (`:149`) is shared by the
release-compiled `from_v0` (`:65`) and the `#[cfg(test)]` `from_v1`
(`:106-107`), and the marker-aware emptiness arm has **no version fork**:

> ```
> || row.backup_ref.is_none()
>     && row.stash_id.is_none()
>     && row.noop_commit.is_none()
>     && row.reset_commit.is_none()
> ``` — cleanup.rs:171-174

The amendment asserts the opposite for this exact seam: "The extension is
v0-inert: no v0 record carries markers, so the **`from_v0` leg of the shared
derivation never sees the new arm**" (§2.2 terminal-plane fate; §5 repeats
"Both deltas are v0-inert"). As implemented, the arm is version-blind and
**does** fire on the v0 leg: the shared struct (`model/v0.rs:158-173`) now
parses the two names into typed fields for v0 records too, and
`decode_v0` (`archive/mod.rs:92-108`) is permissive serde with no
unknown-key audit. Consequence, verified against `@0b059e8` behavior:

- **Pre-D3:** an archived v0 record whose evidence row contains a fabricated
  `noop_commit`/`reset_commit` and nothing else parses to an all-`None` row →
  `CleanupError::ContradictoryEvidence` → `ArchivedRecordUnreadable` →
  targeted GC and the retention sweep **refuse fail-closed** (stop-and-retain).
- **Post-D3:** the same bytes parse into the typed markers, the row is
  accepted, contributes nothing, and the derivation succeeds with an empty
  worklist — archive deletion (and retention-sweep eligibility,
  `has_stash_evidence == false`) **proceeds** where it previously refused.
  No §2.2 value equation is checked on this leg (the marker validator lives
  in `#[cfg(test)]` `model/v1`), so even a non-hex garbage value
  legitimizes the row.

This contradicts the §2.3 doctrine as extended to rows ("the value is never
adopted") — the v0 leg semantically adopts the marker as row-legitimizing
evidence — and it is release-reachable today (`from_v0` is production).
Reachability requires a fabricated/hand-edited v0 archive (no legitimate
writer emits the fields), and the blast radius is bounded to archive-plane
objects (the empty worklist deletes no repository refs), which is why this
is P2 rather than P1 — but it must be fixed before A1 widens exposure.

**Remediation (minimal):** fork the emptiness arm by leg, mirroring the
extractor's `EvidenceKeys` idiom — e.g. pass a `markers_known: bool` (or the
`EvidenceKeys` enum) into `collect_owner`; `from_v0` passes the v0 value so a
marker-only v0 row remains `ContradictoryEvidence` exactly as at `@0b059e8`,
`from_v1` passes the v1 value. Add the missing pin (see P3-3): `from_v0`
over a marker-bearing row → error; `from_v1` over the same row → derives.

### [P2-2] Four named §8 test obligations of the implementation package are not delivered

§8's preamble makes the suites "owed by the implementation package". Missing
from this package (searched the full `workspace_ops` tree, not just the
diff):

1. **§8.2(a)/(b) restart equivalence with markers stripped** — no test
   drives the same fixture "(a) with durable markers and (b) with the
   markers stripped to the pre-amendment shape", including the pointed
   sub-claim "stripping an `N`-only row deletes the row entirely, producing
   the legitimate absent-earlier-row shape, which must classify identically
   via the live fallback (**never reject at decode**)". That sentence is the
   round-1 P1-2 remediation surface of the amendment itself; today no test
   pins it (the two pre-D3 wedge tests that exercised the degraded live
   path were repurposed into the marker-bearing U3 demonstrations, and the
   retained live-guard arm `cursor.rs:61-77` has no direct pin either —
   nor did it at `@0b059e8`, but the degraded-shape *outcomes* were pinned
   there).
2. **§8.2 crash between a live pass and its marker write** — no
   crash-injection test proves the re-prove-and-rewrite convergence
   (idempotent marker write). The reducer accepts an idempotent rewrite by
   construction (`immutable_marker`, `reduce/preservation.rs:339-341`), but
   the named end-to-end pin is absent.
3. **§8.5 post-`reset_commit` preflight-only interference** — the named
   highest-latency case (branch moved **after** `reset_commit`; the reset
   position decode-skips; exhaustion proceeds; rollback-entry preflight is
   the sole catcher) is not exercised. The rewritten
   `invariants.rs::new_work_in_an_earlier_skipped_owner_...` is the nearest
   cousin but its interference lands *before* the reset marker exists and
   is worktree dirt, not a ref move — the decode-skip-under-regression arm
   of `reset_complete` (`cursor.rs:314-317`) is never the deciding fact in
   any current test.
4. **§8.7 bundle-identity invariance** — no pin that expected bundle bytes
   are identical with and without markers. (Structurally evident —
   `checked_bundle.rs:146-151` skips rows without stash ids, and the §2.2
   validator forbids `S+N` shapes — but the obligation names a test.)

Delivered in substance and verified green: §8.1 (the 13-test
`preservation_marker_tests` suite — structurally exhaustive 16 shapes with
separable equation values — plus the five-case disturbance-rejection test
and the byte-constancy pins in `prefixed_preservation.rs`), §8.2(c) at
reducer granularity (`B`→`B+N+R` backfill in
`prefixed_reset_accepts_only_each_exact_successor`; `B+S`→`B+S+R`
no-backfill in
`a_marker_less_backup_and_stash_row_retires_without_a_noop_backfill`),
§8.3 in full including the negative migration case, §8.4 both directions
(and note: the two U3 fixtures make any residual earlier-owner image capture
*outcome-detectable* — the dirty earlier owner would have produced the old
mismatch — which behaviorally substitutes for the §8.2 call-counting proof
on the two paths they cover), §8.6's worklist half (4 tests).

**Remediation:** land the four named tests (a strip-compare fixture in
`prefixed_preservation.rs` or the reverse-preservation suites; a
crash-boundary case in the existing crash-matrix idiom of
`faults.rs`; a §8.5 branch-move fixture; a `checked_bundle` byte-compare) —
or record a program-level deferral with the State axis's concurrence. Code
inspection found no defect on the untested paths, which is why this is P2
and not P1.

### [P3-1] Misnomer test — ruling: rename

`faults.rs:120` `later_pending_owner_cannot_advance_after_completed_prefix_regresses`
now asserts the exact opposite of its name: the run **succeeds**, the later
owner advances, and the test's own comment says "the later pending owner is
no longer wedged … it may advance on its own exact observations". The name
documents the pre-amendment wedge and will actively mislead anyone hunting
U3 regressions. **Ruling: rename** (e.g.
`regressed_completed_prefix_refuses_fail_closed_without_wedging_later_owner`),
comment-only edit, no assertion change. P3 — cosmetic, but it is the
demonstration row for the amendment's headline payoff.

### [P3-2] §3.1's "decode-derivable" entailment for stash-bearing owners is prose, not code — the backup position of an `S`/`S+R` row stays live-fallback forever

§3.1 justifies writing no `noop_commit` for stash-bearing owners because
"their artifact-pass completion is decode-derivable (§3.2), and a stash pair
preceded by no backup pair entails the backup position was a no-op". The
normative §3.2 bullet list, and the implementation
(`backup_complete`, `cursor.rs:255-266`), give the **backup position** only
two durable facts — backup pair or `noop_commit` — so an `S`-only (or
`S+R`) owner's backup position re-proves `protected_commit == anchor` live
on every later classification, forever: rule 1 (`S+N` illegal) plus
`artifact_noop_needed`'s stash exclusion (`cursor.rs:342-349`) mean no
marker can ever retire it. If that owner's protected ref later moves, the
wedge (`require_artifact_complete`, `cursor.rs:239-253`, or the retained
guard) persists exactly as pre-amendment — and §3.3's dichotomy ("the guard
no longer fires for an owner whose retirement is durably recorded") is not
true of reset-marker-bearing `S+R` rows reached through that live arm. This
is contract-conformant (§4 item 2 mandates today's outcomes verbatim where
no durable fact covers a position) and the check is a cheap ref read, not an
image capture — but the amendment's §3.1/§3.3 prose overstates what decodes.
**Remediation:** documentation-only — route a one-sentence precision fix
("the stash-pair entailment retires the stash position only; the backup
position of a stash-bearing row remains on the live ref-equality fallback")
into the D3 docs pass the escape amendment already routes (§10.1 item 7
company); alternatively, if the program wants the entailment decode-real, a
stash-pair arm in `backup_complete` is a one-line change **but** would amend
§3.2's normative list — contract first, not code first.

### [P3-3] No test exercises `from_v0` over a marker-bearing row

`archive/tests/cleanup.rs:249-272`
(`the_empty_row_rejection_is_unchanged_on_both_legs`) pins only the
all-`None` row on both legs. The one shape that distinguishes a forked from
an unforked `collect_owner` — a v0 row whose only content is a marker — is
untested, which is precisely why P2-1 survived the suite. Remediate together
with P2-1 (assert `from_v0` errors, `from_v1` derives, on the same
marker-only row).

### P4 (notes, no action required for the gate)

- **P4-1.** `record_marker` accepts an idempotent re-write of an
  already-present marker (same value) — intended per §4 item 2's idempotency
  and harmless under the store's known-diff discipline; noted so nobody
  "fixes" it into a rejection.
- **P4-2.** Pre-existing, unchanged by D3 (verified `@0b059e8` identical):
  a pending `BackupRef` beside a stash-bearing row is not a decode
  contradiction, and `backup_done` (`phase.rs:268-283`) builds its
  whole-row successor with `stash_id: None` — a shape that could, in a
  contrived action-free restart of a regressed last owner, erase a durable
  stash pair. D3's new cross-checks *narrow* this surface (the
  `noop_commit` variants now reject at decode) and D3's marker edges are
  guarded by `marker_write_preserves_prior_row`; the residual hazard
  belongs to the frozen pre-amendment vocabulary, flagged for the program's
  awareness only.
- **P4-3.** The two reducer arms keep the §3.1 working names
  (`RecordArtifactNoop`, `RecordResetNoop`). §3.1 delegates exact spellings
  to implementation review: **accepted as-is** — descriptive, non-colliding
  with the escape vocabulary, consistent with the surrounding
  `FinishResetAttachedRef` idiom.
- **P4-4.** Persistence traffic matches §9's pricing: one `noop_commit`
  write per skipped owner (artifact loop returns per write), one standalone
  `reset_commit` write per no-op reset position, executed resets ride the
  retirement write, and the loop ordering (artifact pass fully marker-clean
  before the reset loop runs) makes the no-op reset edge's backfill
  structurally unnecessary — `R`-alone/`B+R` shapes are unconstructible at
  every edge, with the §2.2 validator as the backstop at the checked write.

---

## 4. Verdict

**GO.** Zero P0, zero P1. Two P2s (the v0 cleanup-leg version fork with its
missing pin, and the four undelivered §8 test rows), three P3s (rename
ruling, docs-precision note, the from_v0 marker pin), four P4 notes. The
wire delta, validator, write edges, decode-terminal prefix, guard rescope,
terminal-plane worklist arm, release-reachability claim, erratum reading,
F-1 and M5b byte-identity, and the escape-amendment non-interference all
verified as specified; fmt/clippy clean; 195 release spot tests green
against the recorded 559-test population. The P2s are round-2 remediation
items, with P2-1 required before any A1 exposure of the archived plane.

---

# Round-2 re-verdict (focused, 2026-08-22)

Scope per the program's round-2 brief: my own round-1 findings only, against
the round-2 object (tracked diff vs `0b059e8`; the two test files —
`preservation_marker_tests.rs` 303 lines, `durable_cursor.rs` 452 lines —
now intent-to-add staged and confirmed present in `git status` as `A`).
Peer-independence held: the State axis's round-2 GO was disclosed to me but
nothing of its report was read. Every gate below was re-executed by me on a
release binary rebuilt from the round-2 tree, with `find -newer` confirming
zero `workspace_ops` sources newer than that binary.

## Per-finding disposition, verified in code and by execution

- **P2-1 — CLOSED (grade: pass).** The fork is exactly the remediation I
  specified, in the `EvidenceKeys` idiom: `CleanupMarkers::{Unknown,Known}`
  (`cleanup.rs:68-77`), threaded `Unknown` at both `from_v0` sites and
  `Known` at both test-gated `from_v1` sites, deciding arm at
  `cleanup.rs:201-207` (`marker_content = matches!(markers, Known) && …`).
  Verified semantics: on `Unknown` a marker-only row satisfies the
  emptiness disjunct regardless of marker value (non-hex included) →
  `ContradictoryEvidence` → targeted GC and the retention sweep refuse
  fail-closed byte-for-byte as at `0b059e8`; on a v0 row that also carries
  real evidence the marker is ignored entirely, so §2.3's "the value is
  never adopted" now holds of the acceptance surface. On `Known`, `N`/`N+R`
  rows derive, add no worklist entries, set no stash evidence, and never
  block another owner's ref enumeration. `Known` is
  `#[allow(dead_code)]`-annotated with an A1 reason — correct, since
  `from_v1` is `#[cfg(test)]` until A1.
- **P3-3 — CLOSED (pass).** The pin I demanded exists and is stronger than
  asked: `a_marker_only_row_is_row_content_on_the_v1_leg_and_an_empty_row_on_the_v0_leg`
  (`archive/tests/cleanup.rs:285`) drives four shapes — `N`, `R`, `N+R`,
  and a non-hex garbage marker — through **both** legs on the same row,
  asserting `from_v0 == Err(ContradictoryEvidence)` and `from_v1` derives
  empty. Cleanup suite 9/0 (release, my run).
- **P2-2 — CLOSED (pass).** All four missing §8 rows landed in the
  §8-designated homes, and each is genuinely load-bearing, not a
  formality:
  - **§8.2(a)/(b)** `restart_with_and_without_durable_markers_classifies_identically` —
    the strip of the `N+R`-retired rows produces the absent-earlier-row
    shape and the never-reject-at-decode claim is pinned by an `expect` on
    the stripped run; dispositions asserted equal; and §8.2(a)'s cost claim
    is measured at the real capture seam: durable path **0** image
    captures, stripped control **> 0**.
  - **§8.2 crash** `a_crash_between_the_live_pass_and_its_marker_write_reproves_and_converges` —
    crash injected by discarding the record bytes after exactly one durable
    write; restart converges to the same terminal disposition and re-writes
    the **identical** `noop_commit` (idempotence pinned by value equality).
  - **§8.5** `interference_after_reset_commit_is_caught_only_by_rollback_entry_preflight` —
    the branch-move shape I demanded, and constructed so the decode-skip is
    the *deciding* fact: the ref moves to the recorded protected commit,
    precisely where the live fallback would have matched and mutated
    (resetting the branch), so the head-unchanged and no-artifact
    assertions discriminate the §3.2 `reset_commit` arm from the live arm.
    Fail-closed, zero mutation, preflight sole catcher. This retires my
    round-1 complaint that `reset_complete`'s decode-skip-under-regression
    arm was never decisive in any test.
  - **§8.7** `expected_bundle_bytes_are_identical_with_and_without_markers` —
    a real preservation-created stash; bundle bytes byte-equal with markers
    added at every legal position (`S+R` on the stash row, `N+R` on the
    clean owner); no illegal `S+N` shape used.
  My run: `durable_cursor` 4/0 release. (The §8.6 post-GC projection pin
  also landed — `gc_tests` now 7/0 — that row belonged to the State axis.)
- **P3-1 — CLOSED (pass).** Renamed to my suggested identifier verbatim
  (`regressed_completed_prefix_refuses_fail_closed_without_wedging_later_owner`,
  `faults.rs:86`); the old identifier has zero hits tree-wide; 1/0 green.
- **P3-2 — deferred by design (accepted).** No code change, routed to the
  D3 docs pass — which is exactly the contract-first ruling my round-1
  report gave. The implementer's refusal of the State axis's proposed
  decode cross-check is, on my analysis, *correct*: its `reset_commit`
  half would decode-reject a shape my P3-2 shows is reachable (an
  `S`/`S+R` owner's backup position lives on the live fallback forever),
  violating §4 item 2.

## Round-2 production delta beyond my findings (reviewed, no new findings)

- **Marker-carry at the two whole-row write edges** (`backup_done` via new
  `prior_marker`, `phase.rs:274-281`/`:397-407`; `stash_evidence`,
  `evidence.rs:56-59`): on every legally reachable path the carried value
  is `None` — a row at either edge cannot legally hold a marker
  (`B+N`/pending-cross-checks decode-reject; `noop_commit` makes the
  backup edge cursor-unreachable) — so production behavior is
  byte-identical; on the fabricated `S+R`-plus-pending-`BackupRef` shape it
  converts a silent marker erasure into a §2.2-illegal `B+R` refused by the
  staged reread pre-publish. Fail-closed, principled under §2.2
  immutability, and it leaves my P4-2's pre-existing (marker-less) hazard
  exactly as frozen. Sound.
- **Edge-2 backfill computed, not hard-coded** (`reset_noop_intent`,
  `cursor.rs:433-441`): same predicate as the retirement edge; still
  provably `false` under today's loop order (correctly commented as a
  theorem); `marker_row` refactored to a flag-driven builder that never
  revalues a present marker. `S+N` remains unconstructible at every edge.
- **The two test seams, both confirmed test-bounded.** `post_gc_record`:
  signature/visibility + doc comment only, body untouched, module-internal
  (`pub(in …::merge)`) — no behavior, no new production caller.
  `V1_PRESERVATION_IMAGE_CAPTURES`: a `#[cfg(test)]` `thread_local!`
  incrementing inside `v1_preservation_image`, which is itself
  `#[cfg(test)]` — zero production surface; thread-local is the right
  discipline for the parallel harness, and sitting at the real capture
  seam is what makes the §8.2(a) zero-capture assertion meaningful given
  the sealed backend trait.
- **P4-grade note (new, non-blocking):** the crash test's
  `if let Some(marker)` skips the idempotence equality if the first durable
  write were ever not the first owner's noop marker; deterministic today
  (`assert_ne!` proves a write occurred), but a future fixture change could
  silently vacate the assertion. Worth a `.expect` when next touched.

## Gates (re-executed by me, release, round-2 binary)

fmt `--check` clean; clippy `--all-targets` zero findings (only cargo's
pre-existing profile note — the other lane's earlier finding no longer
reproduces, and `workspace_ops` specifically is clean); binary enumerates
the merge namespace consistent with the implementer's partitioned 565/0
(559 baseline + exactly the 6 new tests; my suite arithmetic 32−1+4=35
confirms `durable_cursor` sits inside `reverse::preservation::tests`).
My runs: `durable_cursor` 4/0, `archive::tests::cleanup` 9/0, `gc_tests`
7/0, renamed fault test 1/0, `preservation_marker_tests` 13/0,
`reverse::preservation::tests` (matrix skipped, no cause to re-run the
576s row) 35/0, `continue_v0_gate` 3/0, `no_ff` filter 28/0. F-1 file and
all five M5b files byte-identical vs `0b059e8`; `store/tests.rs` exactly
+2/−0 (the forced round-1 marker lines).

## Budget concurrence

Object +2192/−103 vs §9's ~700–1,100: the production slice is +621/−50 —
inside the envelope's intent — and the overage is test mass (+1,571 test
lines), of which 755 is the §8 acceptance suite this same contract
mandates. As the Code axis I **concur** with reconciling the ChangeBudget
row on that basis; the State axis owns the row.

## Final verdict

**GO — final and unconditional.** Both P2s are closed with verified code
and green pins; P3-1/P3-3 closed; P3-2 correctly deferred to the docs pass;
the round-1 condition (P2-1 before A1 exposure) is satisfied; the round-2
delta introduces no new findings above P4. Grades: P2-1 pass, P2-2 pass,
P3-1 pass, P3-2 accepted-deferred, P3-3 pass, test seams accepted, budget
concurrence recorded.
