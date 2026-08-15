# GWZ M5-8 A1 Decision Packet — the three load-bearing open decisions

Date: 2026-08-16

Status: **DRAFT — for the program owner.** This packet consolidates the three
open decisions that become load-bearing when A1 activates the v1 lifecycle for
real users: the two OPEN DECISIONS recorded in the accepted exact-evidence
amendment (`GwzM5-8ExactEvidencePlatformAmendment.md` :11-13 — "acceptance
does not close them") and the durable-preservation-cursor question the F5
progress review flagged as pre-A1 item 4 (`GwzM5-8ProgressReviewF5.md` §9
item 4, :451). Each section gives the problem with citations, concrete
options with mechanisms, trade-offs, costs, interactions, and one
recommendation. This document decides nothing by itself; every option that
touches code or frozen text names the amendment/review tier it would require.

Grounding: direct reads of the amendment and its dual-review record
(`GwzM5-8ExactEvidenceAmendment-ReviewState.md`), the standing residual
tripwire (`GwzWindowsMatrix-Classification.md` :181-197), the operator escape
design (`GwzM5-8OperatorEscapeDesign.md`), the F5 review, the frozen I2/R4b
texts, and the implemented observers and cursor: gwz-core
`src/workspace_ops/merge/abort/participants.rs`,
`src/git/gitbackend/preservation_image.rs`,
`src/git/gitbackend/merge_recovery.rs`,
`src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/cursor.rs`,
`src/workspace_ops/merge/preserve/plan.rs`, plus the vendored git2 0.21.0
sources for API feasibility. Line numbers are against the current lane tip.

---

## 0. Executive summary

| # | Decision | Recommendation | Must land | Blast radius if deferred |
| --- | --- | --- | --- | --- |
| 1 | Real-Windows raw-byte satisfiability | **Option B** — creation-time filter neutralization: pin `core.autocrlf=false` + `core.eol=lf` and clone filters-off at every gwz materialization edge, so gwz-born worktrees are blob-exact from birth; add an operator-invoked renormalize command later for adopted worktrees; keep the raw-byte doctrine untouched | Decision pre-A1; the pins in the **next release train** (every release without them grows the stranded-CRLF workspace population); renormalize command with-A1 or after | At A1, ordinary Windows-CRLF workspaces cannot enter the reverse lane: rollback/abort classify `Ambiguous` before any mutation (availability loss, never wrong evidence). With exact-or-nothing plus the open-merge gate, a wedged merge bricks the whole workspace until escapes ship. CI is structurally blind (fixture pins), so nothing will page before a user does |
| 2 | Foreign-filter policy (git-crypt class) | **Option A′** — refuse the recovery-grade checkout pre-mutation when a *configured, non-passthrough* foreign clean filter covers a path it would rewrite (allowlist `lfs`); typed error before the ref transaction, so the failure moves from post-commit wedge to clean refusal | **Before the next release cut from this tree.** NOT A1-gated: the v0 abort flow reaches both `disable_filters` checkouts today | git-crypt-class repos: a previously working v0 abort becomes a post-ref-move `RecoveryRequired` wedge on every OS, retry-proof, with the only remedy an undocumented `mv` (the escape design is still DRAFT) |
| 3 | Durable preservation cursor | **Adopt the minimal durable cursor** — per-owner no-op skip rows + a reset-completion bit, written write-ahead like every other evidence row — via an I2 journal/record amendment riding the same pre-A1 train as the operator-escape wire changes; per-action live exactness at execution is kept | Amendment (wire) **pre-A1** — the last cheap moment; implementation may land with the A1 package | The v1 wire freezes asymmetric at A1. Forever after: restart legality of a later pending owner rests on re-proving every earlier owner live (full-tree image capture per earlier owner per dispatch), the U3 preservation-wedge surface stays maximal, and any later fix is a same-version corridor (priced out by its own proof burden, F5 §3.4) or a v2 field coupled to M6 timing |

Framing corrections found while assembling this packet (details in-line):

1. **Decision 2 is not an A1 decision.** The wedge is production-reachable
   through the v0 abort flow in the current tree (State F2, review :45;
   amendment :143-147); it ships with whatever release next carries the
   amendment code. Its deadline is the release train, not A1.
2. **Decision 1 is two coupled exposures, not one.** The tripwire ledger
   couples "unrewritten smudged files" with the `stash_save` filtered-reset
   residual (`GwzWindowsMatrix-Classification.md` :181-197). Candidates (a)
   and (c) as named address only the rollback-observer side; option (b) is
   the only candidate that closes both for gwz-born repos, because a
   worktree that is LF from birth gives the stash sweep nothing to smudge.
3. **The exposure is not Windows-only in principle.** Any smudge-active
   attribute (`eol=crlf`, `ident`, foreign `filter=` drivers) puts a worktree
   outside the raw-byte satisfiable set on every OS; host `autocrlf=true` is
   just the dominant Windows instance. No option below closes the
   attribute-driven residual; it must be frozen as permanent fail-closed
   doctrine whichever way Decision 1 goes.
4. **Decision 3 already has a train scheduled.** The operator escape design
   proposes its own I2 wire deltas ride pre-A1 "with the [F5 §9 item 4]
   durable-cursor amendment if that proceeds"
   (`GwzM5-8OperatorEscapeDesign.md` §6.1 :565-567, [Q3] :749-754). Deciding
   item 4 late orphans that shared amendment window.

---

## 1. Decision 1 — real-Windows raw-byte satisfiability

### 1.1 Problem

The v1 reverse-lane observers are full-tree and raw-byte:
`observe_v1_participant_rollback`, `verify_v1_no_mutation_participant`, and
`terminal_v1_participant_is_exact` (`abort/participants.rs:95-133, :135-165,
:167-192`) all reduce to `clean_checkout` → `checkout_matches_commit_except`
(`participants.rs:235-252` → `preservation_image.rs:152-199`), which compares
raw worktree bytes (`read_worktree` → `std::fs::read`,
`preservation_image.rs:491-529`) against raw blob bytes (`flatten_tree`,
:437-489). They gate rollback preflight (`abort/preflight.rs:69,83,98`),
no-mutation verification, recovery re-entry (`rolling_back_recovery.rs:44`),
and terminal exactness.

Clause A of the accepted amendment made the two recovery-grade *rewrite*
edges blob-exact (`disable_filters` at `set_branch_target_checked` and
`abort_merge`, `merge_recovery.rs:211, :149-157`), but both rewrite **deltas
only**, and `disable_filters` is smudge-side only — the checkout's
modified-detection still cleans worktree bytes, so an unchanged-but-smudged
file is never rewritten by any checkout (State F1, ReviewState :43).
Every path materialized earlier through filters — the user's porcelain
clone, forward integration, `fast_forward` (`refs.rs:6-36`), gwz's own
member materialization (`create_repo` sets no config,
`repository.rs:63-70`) — keeps CRLF on disk. Consequence, frozen into the
amendment's scope limits (:99-130): Windows + `autocrlf=true` + any member
with one text file unchanged between `resulting_commit` and `before_commit`
→ reverse entry classifies `Ambiguous` and rollback never starts; where it
starts, After re-verification fails on unchanged-but-smudged paths.
"Ordinary Windows-CRLF worktrees therefore remain **unsatisfiable for the
raw-byte model regardless of this amendment**" (:122-125). Failure direction
is closed — availability loss, never wrong evidence — and CI is
structurally blind (LF fixtures; run-7 `core.autocrlf=false` pins;
amendment :122-126; ledger :183-190).

The standing tripwire couples a second exposure to the same decision:
libgit2 `git_stash_save` internally re-materializes swept tracked files
**with filters active** and exposes no checkout options (amendment
:156-168; ledger :185-187), so preservation-side raw-byte equalities (the
stash preimage model, `preservation_image.rs:7` frame; decode at :323-401)
are equally unsatisfiable on a CRLF worktree — run 8 measured exactly this
class in the un-pinned member fixtures (ledger :219-233).

Load-bearing point: the entire v1 lane is `#[cfg(test)]` until A1
(`merge/mod.rs:23-24`; `plan.rs:396-397`), so this exposure activates
exactly at A1 — on the platform the eleven-run matrix campaign was run to
support, where matrix-green explicitly "must not be read as real-Windows
exact-evidence closure" (amendment :125-127; ledger :8-12).

### 1.2 Options

**A — recovery-grade full re-materialization at rollback entry.**
Mechanism: a new backend primitive walks the target commit's tree
(`flatten_tree` already exists) and, for every tracked path whose raw disk
bytes differ from blob bytes while filter-aware status reports it
unmodified, overwrites the file with blob bytes and refreshes the index
stat cache; invoked at reverse entry and at rolling-back recovery re-entry.
It cannot be an ordinary checkout — FORCE + `disable_filters` still skips
content-matching paths (F1 mechanism above) — so this is a bespoke rewrite
loop. Problems: preflight is doctrinally read-only (DirentBarrier review
:282-283 verified exactly that property); a physical mutation before the
first Before-observation needs either a new journaled phase (I2 journal
vocabulary = wire change, pre-A1 only) or an un-journaled
"evidence-neutral" mutation the reverse-interface ownership freeze does not
currently own — an interface amendment plus dual review either way. It
maximizes Decision-2 exposure (rewrites every divergent path, so the
foreign-filter predicate becomes a hard precondition), silently converts
whole member worktrees to LF at entry even when rollback then fails, and
does nothing for the `stash_save` preservation exposure. Recurring cost:
full-tree byte compare per participant at every entry/re-entry.

**B — filters-off at member materialization (gwz-born worktrees are
blob-exact).** Mechanism: `create_repo` (`repository.rs:63-70`) writes
repo-local `core.autocrlf=false` + `core.eol=lf` at init;
`clone_repo_with_progress` (`transport.rs:7-32`, the single funnel for the
production clone sites `handle_materialize.rs:265,:491`,
`handle_init_from_sources.rs:137`, `handle_repo_lifecycle.rs:87`) passes
`RepoBuilder::with_checkout(CheckoutBuilder::disable_filters(true))`
(git2 0.21.0 `build.rs:215`, `:501`) for the initial materialization and
pins the same two keys immediately after. This is a creation-time-only pin
— exactly the point run 9 proved safe ("the pin is safe only at repo
creation, before any filtered materialization", ledger :274-279) after the
mid-life variant manufactured dirt in clone-based fixtures. Result:
gwz-born worktrees are LF/blob-exact from birth and stay so through every
forward checkout (repo-local config neutralizes host autocrlf), and
`stash_save`'s internal reset becomes a filter no-op — **both tripwire
exposures close for gwz-born repos**. Residuals, all fail-closed: adopted
pre-existing clones; workspaces materialized by pre-B gwz versions;
per-path attributes (`eol=crlf`, `ident`, foreign `filter=`) on any OS.
User impact: gwz-created worktrees on Windows are LF-on-disk; `git status`
stays clean (clean-idempotence, amendment :132-141); toolchains needing
CRLF via attributes keep CRLF — and stay outside the satisfiable set.

**C — clean-side (filtered) comparison mode for the reverse observers.**
Mechanism: replace raw equality in `clean_checkout` /
`checkout_matches_commit_*` with libgit2 filter-aware equivalence —
diff(target-tree ↔ index) empty ∧ diff(index ↔ workdir) empty, where
workdir hashing applies clean filters (the same model mutation-time
`verify_merge_result` already uses); per-file clean hashing is not cleanly
available (git2 0.21 has `get_attr`, `repo.rs:1112`, but no `hashfile`;
`blob_path` :1177 writes ODB objects during observation — disqualifying).
Problems: classification becomes config/attribute-dependent at observation
time — flip a filter setting mid-merge and the classification flips (run
9's manufactured-dirt incident is the empirical caution, ledger :262-282);
it amends the frozen raw-byte philosophy itself
(`GwzM5-8R4bReverseLifecycleInterface.md` :6, :348 "enforces the I2
raw-byte canonical preimage") and the closed-observation reading of
ActionJournal §3 the amendment deliberately kept (:40-57) — a doctrine
amendment, dual review. And it is only half a fix: the preservation-side
preimage equalities stay raw, so Windows `CreateStash` mismatches persist
unless the preimage model also goes clean-side — a preimage-frame
discontinuity (F5-class, amendment :328-343) and a scope balloon.

**D — hybrid: B now, plus an operator-invoked renormalization command.**
Option A's rewrite loop, packaged as an explicit consent-gated command
*outside* the lifecycle (the escape design's separate-consent-lane rule,
`GwzM5-8OperatorEscapeDesign.md` §2 rule 1): `gwz repair --renormalize
<member> --destructive` (name per [Q7]-style bikeshed), usable before a
merge or when reverse entry reports a typed smudged-worktree hint. No
journal phase, no preflight mutation, no doctrine change; reuses Decision
2's attribute predicate as its refusal gate. This is the adopted-worktree
and pre-B-workspace remedy that B alone lacks.

### 1.3 Trade-offs and costs

| | A: entry re-materialization | B: birth-time filters-off | C: clean-side compare | D: B + renormalize cmd |
| --- | --- | --- | --- | --- |
| Correctness | raw doctrine kept; new mutation inside reverse lane | raw doctrine kept; nothing new inside lane | doctrine amended; config-dependence re-enters | raw doctrine kept |
| Availability closed | rollback side only; gwz-born + adopted | both exposures, gwz-born only | rollback side only (preservation needs frame change) | both for gwz-born; adopted via explicit command |
| Complexity | high (new mutation edge + ownership amendment) | low (config + one clone funnel) | medium-high (observer rewrite ± preimage frame) | B + a bounded CLI package |
| User-visible | whole worktree flips LF at rollback entry, unasked | gwz-born worktrees LF from birth, status clean | none direct; evidence semantics loosen | B, plus opt-in normalize |
| Frozen texts touched | ReverseLifecycleInterface ownership (+ I2 journal if journaled) | amendment §2.A table annotation + ledger | ReverseLifecycleInterface :348 doctrine, ActionJournal §3 reading, likely preimage frame | same as B now; CLI/protocol append later |
| LOC (impl+tests) | ~400-900 | ~150-250 | ~450-800 (+300-500 if preimage side) | B + ~300-600 later |
| Review tier | interface amendment → dual | focused State-axis | doctrine amendment → dual | B: focused State; command: own small dual package |

### 1.4 Interactions

- **With Decision 2:** A (and D's command) rewrite every divergent path, so
  the foreign-filter predicate is a hard precondition for them — build the
  D2(a) attribute-inspection helper once and share it. B is orthogonal
  (config pins do not touch attribute-driven smudge).
- **With Decision 3:** B and D touch no wire; C's preimage-frame variant
  would have to ride the same pre-A1 amendment train as Decision 3. Cursor
  liveness checks use filter-aware dirty flags, not raw equality
  (`cursor.rs:251-283`), so Decision 1 does not change Decision 3's shape.
- **With the escape design:** every residual left open here lands in the U-
  class taxonomy as availability wedges whose remedy is quarantine; the
  smaller Decision 1's residual, the less load the escape ladder carries.

### 1.5 Recommendation

**Option B in the next release train, Option D's renormalize command as the
follow-up remedy for adopted/legacy worktrees; reject A and C.** B is the
only candidate that closes both tripwired exposures, it does so at the one
point the program has empirically proven safe to pin (creation time), it is
~200 LOC behind a focused State review, and it leaves every frozen text
standing (one amendment-table annotation). A buys adopted-repo coverage at
the price of a new mutation edge inside a lane whose read-only preflight two
independent reviews just verified; C trades away the exact property the
amendment was written to defend. Freeze the attribute-driven residual
(`eol=crlf`, `ident`, foreign filters — all platforms) as permanent
fail-closed doctrine in the ledger entry, and end the CI blindness by adding
one un-pinned CRLF lane (or expected-fail sentinel) to the windows matrix in
the same package — the ledger's own "no failing sentinel anywhere" complaint
(F6-class) applies to B's regression surface too. Land the decision pre-A1;
every release shipped without the pins enlarges the legacy-workspace
population only D can later serve.

---

## 2. Decision 2 — foreign-filter policy (git-crypt class)

### 2.1 Problem

Clause A's soundness precondition is clean-idempotence on blob bytes:
writing raw blob bytes is status-clean only where every clean filter
covering a rewritten path satisfies `clean(blob_bytes) == blob_bytes`.
Builtin filters satisfy it; encrypt-on-clean filters (git-crypt class) do
not. Where it fails, a rewritten path is status-dirty immediately after the
recovery checkout and `verify_merge_result` fails **after**
`transaction.commit()` in `set_branch_target_checked`
(`merge_recovery.rs:217-218`) or after the abort restore; the retry hits
the idempotent arm (:189-196) and fails the same verification — "a
previously working production v0 abort becomes a recovery-required wedge,
on every OS … wedged until manual intervention" (amendment :132-150; State
F2, ReviewState :45). The amendment records the policy as an explicit OPEN
DECISION (:147-150): (a) refuse the recovery-grade checkout when non-builtin
filter attributes cover paths it would rewrite, or (b) accept the wedge as
fail-closed.

Deadline correction: this is **production-reachable today** via the v0
abort flow (`abort/participants.rs:13-67` → runtime →
`set_branch_target_checked` / `abort_merge`; F2 verified the path), so the
regression ships with the next release cut from this tree — A1 is not the
gate. Related disclosed surprise even where the precondition holds: an
LFS-managed rewritten path carries pointer bytes on disk after rollback
(status-clean; porcelain re-checkout restores content; amendment :150-154).

### 2.2 Options

**A — strict refusal on attribute presence.** Before the checkout (and
before `transaction.set_target`), enumerate the rewrite set — for
`set_branch_target_checked`, the tree diff current→target; for
`abort_merge`, conservatively the union of the before-tree paths and the
currently modified/conflicted set — and query the `filter` attribute per
path via git2 0.21's `Repository::get_attr`/`get_attr_bytes`
(`repo.rs:1112/:1132`, wrapping `git_attr_get_ext`; attribute stacks are
cached by libgit2, so cost is O(rewrite set) lookups on top of a tree diff
the checkout does anyway). Refuse with a typed pre-mutation error naming
the filter and path when any value outside the allowlist (`lfs`) is set.
False positives: filter attribute present but no driver configured (git
treats it as pass-through — no wedge would have occurred), and genuinely
clean-idempotent custom filters.

**A′ — refined refusal (recommended variant).** As A, but refuse only when
the named driver is *configured and non-passthrough*: attribute value
outside the allowlist **and** `filter.<name>.clean` (or `process`) present
in the effective config. An unconfigured attribute cannot produce the
wedge, so this eliminates the largest false-positive class while remaining
conservative for every driver that could actually run. Residual false
positives: custom filters that are configured and happen to be
clean-idempotent — refused although they would have survived; these repos
get the typed error and the runbook pointer instead of a working rollback
(availability-only, and precisely the repos a wedge would otherwise
gamble on).

**B — accept the wedge and document it.** Zero code. The amendment already
records the mechanism honestly; the operator remedy is quarantine and the
manual runbook (`GwzM5-8OperatorEscapeDesign.md` §4.2, §4.4) — but both are
DRAFT, and the design's own findings say the released line "can wedge today
with no tool escape and no documentation" (§9.1 :709-717). Accepting B
therefore *hardens* the escape-package schedule: at minimum the §4.4
runbook must ship with or before the release that carries the amendment
code ([Q6]/[Q9]).

A per-path filtered-checkout fallback (checkout foreign-filtered paths with
filters active, everything else raw) was considered and rejected: it
reintroduces smudged bytes into the certified state, making the raw-byte
After verification fail instead — the same availability loss with more
machinery.

### 2.3 Trade-offs and costs

| | A strict | A′ refined | B accept |
| --- | --- | --- | --- |
| Failure point | pre-mutation typed refusal | pre-mutation typed refusal | post-`transaction.commit()` wedge, retry-proof |
| Correctness | fail-closed, earlier | fail-closed, earlier | fail-closed, later and stickier |
| Availability | refuses some repos that would have survived (unconfigured attrs) | refuses only configured foreign drivers | wedges exactly the repos it hits; workspace bricked until manual action |
| Detection cost | O(rewrite set) attr lookups | same + one config probe per distinct driver | none |
| User-visible | actionable error naming filter+path | same | error after the ref moved; recovery-required state |
| LOC + tier | ~250-400, focused State review + OPEN-DECISION closure entry | ~300-450, same | 0 code; runbook shipping decision + ledger/amendment closure annotation |

### 2.4 Interactions

- **Decision 1:** options A/D there need this predicate as their refusal
  gate; building A′ once serves three call sites (two recovery checkouts +
  the renormalize command).
- **Decision 3:** none direct.
- **Escape design:** choosing B makes quarantine/runbook shipping (its
  [Q9]) a release blocker rather than an A1 item; choosing A′ leaves the
  escape ladder as defense-in-depth.

### 2.5 Recommendation

**A′, landed before the next release cut.** The whole value of the checked
edges is that verification failures precede mutation; B knowingly ships a
class of post-mutation wedges into the released v0 line where the current
remedy is an undocumented `mv` of the record. A′ moves the failure to a
clean, named, pre-transaction refusal for ~350 LOC behind a focused State
review, with false positives confined to configured-but-idempotent custom
filters — repos that today sit one checkout away from the wedge anyway. The
LFS pointer-bytes surprise stays documentation-only (status-clean, already
disclosed in the amendment §3). If the owner prefers B, couple it
explicitly to shipping the §4.4 runbook in the same release — B without the
runbook is the one indefensible combination.

---

## 3. Decision 3 — durable preservation cursor

### 3.1 Problem

The v1 preservation wire "intentionally persists neither no-op owner skips
nor a separate reset completion bit"
(`GwzM5-8I2ActionJournalContract.md` :156-166), so "a pure decoder cannot
distinguish a legitimately later pending owner from the same bytes paired
with a live earlier owner that has become incomplete"
(`GwzM5-8R4bTransitionDesign.md` §6.7 :742-761). The compensation is
`VerifiedPreservationCursorPrefix`: before any pending preservation action
is classified, advanced, or executed, an exact live observation must prove
every earlier two-pass position complete or currently unnecessary.

In the implementation that proof is expensive and wholly live: every
dispatch of a `Preserving` record runs `observe_cursor`
(`observe/reverse/preservation/cursor.rs:14-45`), which re-derives all
owner plans (`v1_preservation_owners`, `preserve/plan.rs:397` — live ref
and commit reads per owner) and, per earlier owner, evaluates
`backup_complete` (:241-249 — durable ref row **or** the live no-op fact
`protected_commit == plan.anchor`), `stash_complete` (:251-283 — durable
stash row or a **full live preservation image capture**,
`v1_preservation_image`, `preserve/artifacts.rs:663-680`, which byte-reads
the worktree), and `reset_complete` (:285-305 — no durable bit at all:
live `plan.live_commit == plan.anchor` plus another live image capture).
`verify_pending_prefix` (:99-156) repeats the earlier-owner sweep before
any pending action runs. The F5 review named this "the design's weakest
joint": restart correctness shifted "from durable bytes onto repeated live
observation … asymmetric with rollback's fully record-derivable cursor"
(§3.3 :197-206; rollback's cursor is decode-derivable per ActionJournal
:154-157), and §9 item 4 (:451) says decide now — "last cheap moment". The
checkpoint carries it as the still-open pre-A1 item
(`CurrentProgramCheckpoint.md` :83-85). The DirentBarrier review's
resume-window [P3-1] (`GwzM5-8R2C2DirentBarrier-ReviewState.md` :126-188,
routed at :178-181/:393-397) is the same lesson from the catalog lane —
resume paths that skip a durability step because completeness was inferred
rather than recorded — and routes to the same durability work queue.

Why pre-A1 is the last cheap moment: the v1 writer is production-disabled
until A1, so amending the v1 wire now costs a contract amendment and matrix
work but **zero migration** (the escape design makes the identical argument
for its own fields, §6.1 :561-567). After A1, real records exist: a cursor
field becomes a same-version additive-corridor problem — a corridor F5
§3.4 (:216-219) already judges "priced out of use by its own proof burden"
— or a v2 field coupled to the M6 version ladder, and the live-re-proof
fallback must then be kept forever for records written before the field.

What the status quo costs at A1, concretely: per service-loop step of a
preserving merge, O(earlier owners × worktree bytes) reads on real
repositories; and an availability class — any transient ambiguity in any
earlier owner (mid-crash forms, external touch) blocks classification of a
later pending owner with `PreservationEvidenceMismatch`, feeding the U3
preservation-wedge row of the escape taxonomy
(`GwzM5-8OperatorEscapeDesign.md` §3.3 :160).

### 3.2 Options

**A — minimal durable cursor: per-owner skip rows + reset-completion bit
(recommended).** Wire: extend the per-owner preservation evidence rows
(participant `preservation` / publication `root_preservation`,
`plan.rs:714-745`) with an explicit no-artifact-needed marker written
write-ahead when the cursor passes a clean owner, and a reset-complete
marker written with the reset journal's retirement; both immutable once
written, unknown-field-survival tested like every other row. Observation:
the cursor prefix becomes decode-derived (symmetric with rollback);
`stash_complete`/`reset_complete` stop capturing live images for earlier
owners. Safety is not delegated to bytes alone: each pending action's own
observation still proves its exact live before-form (stash preimage, reset
`expected_commit`, `ensure_clean_recovery_state`), and rollback entry still
re-observes every member — so a regressed earlier owner is caught at its
own next action or at rollback preflight instead of at every later
classification; the fail-closed direction is unchanged, detection latency
moves. Contracts amended: ActionJournal §2 (the "persists neither" and
"adds no field" sentences), TransitionDesign §6.7, RecordContract row
shapes/collision notes, ReverseLifecycleInterface cursor text. One write
per skipped owner is added to the persistence traffic (bounded by owner
count).

**B — keep the live-re-proof semantics, make the probe cheap.** No wire
change: replace the full image captures in `stash_complete`/
`reset_complete`'s no-evidence arms with a status-only dirty probe (the
dirty summary already derives from `repo.statuses`,
`preservation_image.rs:95-141`; a probe that skips `read_worktree` byte
hashing preserves the verdict). Cuts the dominant per-dispatch cost
~O(tree bytes)→O(status); the asymmetry, the transient-ambiguity wedge
class, and the permanent post-A1 wire freeze all remain.

**C — status quo, decided.** Zero change; close F5 §9 item 4 as "accepted
asymmetry" with recorded rationale in the checkpoint. Honest only if the
owner genuinely accepts both permanent costs in §3.1's last paragraph and
that the door closes at A1.

### 3.3 Trade-offs and costs

| | A durable rows/bit | B cheap probe | C status quo |
| --- | --- | --- | --- |
| Restart model | decode-derived prefix, symmetric with rollback | live re-proof, cheaper | live re-proof, full cost |
| Wedge surface (U3) | shrinks (transient earlier ambiguity no longer blocks later classification) | unchanged | unchanged |
| Per-dispatch cost | ref/status reads only | status reads | full image capture per earlier owner |
| Wire | +2 row markers, pre-A1 amendment | none | none |
| Post-A1 flexibility | done | corridor/v2 only | corridor/v2 only |
| LOC (impl+tests+matrices) | ~700-1,100 | ~80-150 | 0 |
| Review tier | I2 interface amendment → mandated dual review | focused Code review | checkpoint closure note |

### 3.4 Interactions

- **Escape train (strongest):** the escape design's `operator_override`
  field and `AbortOperationOverridden` transition already require a pre-A1
  I2 amendment and propose traveling together with this one (§6.1
  :565-567). One amendment round, one matrix wave, two debts retired.
- **Decision 1:** cursor liveness uses filter-aware dirty flags, not raw
  bytes — option A neither helps nor hurts the CRLF question; fewer live
  captures does mean fewer chances for platform-semantics surprises inside
  the cursor path.
- **A1 cliff (F5 §3.2):** A adds reviewed wire surface before A1 — the cost
  side; it also deletes a whole restart-fragility class from the single
  riskiest package's blast radius — the benefit side.

### 3.5 Recommendation

**Option A, as a rider on the escape-design amendment train, amendment
pre-A1, implementation free to land with the A1 package.** The program's
own precedent decides this: Amendment 1 already bent the frozen wire when
correctness demanded a durable handoff field (F5 §3.3 cites it as proof the
wire bends), the escape design is about to open the same contracts anyway,
and rollback's decode-derivable cursor is the model the preservation side
was always meant to mirror. "A few bytes of durable cursor would delete
this entire live-proof class" (F5 §3.3) — at pre-A1 prices that is
~1k LOC inside an amendment round that is already scheduled; at post-A1
prices it is a corridor the program has effectively banned. If the owner
declines the wire change, take B in the same breath (it is small enough to
ride any package) and record C-style closure language so item 4 stops
appearing open — but note that B leaves the U3 wedge surface exactly where
the escape design found it.

---

## 4. Sequencing

1. **Next release train (pre-anything):** Decision 2 A′ (pre-mutation
   refusal) and Decision 1 B (creation-time pins + filters-off clone
   checkout) — both bounded packages behind focused State-axis reviews;
   plus the un-pinned CRLF matrix sentinel and the ledger/amendment
   annotations closing both OPEN DECISION entries as decided.
2. **Pre-A1 amendment round (shared train):** Decision 3 A wire deltas
   together with the operator-escape wire deltas (`operator_override`,
   `AbortOperationOverridden`) — one dual-reviewed I2 amendment, per the
   escape design's own [Q3].
3. **With/after A1:** Decision 1 D's renormalize command (adopted-worktree
   remedy; shares Decision 2's predicate), and the escape implementation
   packages per the escape design's placement.

The one ordering that must not happen: cutting a release that carries the
amendment's `disable_filters` code with Decision 2 still open — that ships
the git-crypt wedge with no policy and no runbook.
