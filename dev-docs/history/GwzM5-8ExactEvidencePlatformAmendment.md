# M5-8 exact-evidence platform-semantics amendment (CRLF + private-area invisibility)

Date: 2026-08-15. Revised: 2026-08-16 (dual-review remediation round 1:
State F1-F6 + Code P2/P3-1/P3-2/P3-3, document-only).

Status: **accepted 2026-08-16** — dual review round 1: Code GO
(`GwzM5-8ExactEvidenceAmendment-ReviewCode.md`); State NO-GO against the
round-1 text alone, document-only remediation applied, State focused
re-verdict **GO** (appended to `GwzM5-8ExactEvidenceAmendment-ReviewState.md`).
The four amended contracts carry acceptance annotations dated 2026-08-16.
The two OPEN DECISIONS in Clause A's scope limits (foreign-filter policy;
real-Windows raw-byte satisfiability follow-up) remain tracked review debts —
acceptance does not close them.

*(Status correction, 2026-08-25, R2.3 anchor A [A-i] — the header
above went stale against this document's own body. Both OPEN
DECISIONS were closed at the landing commit `9939b02`, 2026-08-16:
the foreign-filter policy DISCHARGED by adopted decision D2 (A′) and
the raw-byte satisfiability follow-up by adopted decision D1 (Option
B); the in-body closure annotations below are authoritative. The
packet-mandated focused State reviews did NOT precede that landing —
they were filed 2026-08-25 as `GwzM5-8A1ReleaseR1-ReviewState.md`
(D2/A′, GO with conditions) and `GwzM5-8A1ReleaseR2-ReviewState.md`
(D1/B, GO with conditions), each reviewing the landed object as new.
What remains tracked is the narrowed residual, not the decisions:
ADOPTED worktrees (the post-A1 `renormalize` command) and
attribute-driven smudge (`eol=crlf`, `ident`, foreign `filter=`) on
every OS, both permanent fail-closed doctrine.)*
This revision applies, in one document-only round, State-axis remediation
round 1 (findings F1-F6, `GwzM5-8ExactEvidenceAmendment-ReviewState.md`) and
the Code-axis conditions (P2 + three P3 notes,
`GwzM5-8ExactEvidenceAmendment-ReviewCode.md`, verdict GO). Both round-1
verdicts accepted the code delta; the State NO-GO attached to this document's
round-1 text alone, with a pre-committed GO on re-read of the revision.

Evidence base: `GwzWindowsMatrix-ExactEvidenceDiagnosis.md` (matrix run 6
`31883655778` on `a506a93`, stability replay 6b `31885262853` on `117117e`,
windows-2022, rustc 1.95.0). The diagnosis's one environmental precondition —
system-level `core.autocrlf=true` reaching libgit2 on the runner image — was
confirmed by the probe-branch dispatch (`C:/Program Files/Git/etc/gitconfig`,
Git for Windows 2.55.0.windows.3). Every claim below is elaborated there with
file:line citations; this amendment states the semantics chosen to resolve the
two production finding classes and enumerates exactly what changed.

---

## 1. What this amends

The exact-evidence model holds two frozen commitments that Windows platform
semantics (CRLF smudge filters; the Windows-only checked-artifact durability
anchor) caused to disagree with each other:

- `GwzM5-8I2ActionJournalContract.md` — the closed repository observation for
  participant rollback: for `ResetIntegrated`, "completed is that same
  ref/HEAD at `before_commit` with the exact clean before index/worktree";
  for `AbortConflict`, completion requires the "exact clean before
  index/worktree". These observations compare raw worktree bytes against blob
  bytes (`preservation_image.rs` checkout-match model) and are filter-blind.
- `GwzM5-8R4bInterfaceAmendment-2.md` §5.3 — "After stash creation, the
  decoded stash image must equal the durable preimage and the live repository
  must contain no unrelated dirty state."
- `GwzM5-8R4bTransitionDesign.md` and
  `GwzM5-8R4bReverseLifecycleInterface.md` — the normative reverse-lifecycle
  behavior and its ownership freeze, which both inherit the two commitments
  above; the frozen philosophy is raw-byte
  (ReverseLifecycleInterface:340, "enforces the I2 raw-byte canonical
  preimage"), and this amendment keeps it.

Those documents are **cited, not edited**, by this amendment. The semantics
below keep the raw-byte exactness model and change the production edges that
were violating it, exactly per the diagnosis's §5 fix direction ("keep
raw-byte exactness as the model and make production checkouts byte-faithful").

## 2. Amended semantic clauses

### Clause A — recovery-grade rewrite edges are blob-exact (filters disabled)

The two recovery-grade **rewrite** edges — the checked rollback and checked
native-abort checkouts, the production checkouts that *write* the state the
raw-byte exact-evidence model then certifies (rollback, recovery, root
preservation reset) — run with content filters disabled
(`git_checkout_options.disable_filters`, exposed as
`CheckoutBuilder::disable_filters(true)` in git2 0.21.0; verified in the
vendored libgit2 1.9.6: when set, `git_filter_list__load(...,
GIT_FILTER_TO_WORKTREE, ...)` is skipped entirely and blob bytes stream to the
worktree verbatim — CRLF, ident, and user smudge drivers all bypassed).
The clause deliberately does **not** cover every checkout whose result the
model may later judge: forward and user-facing checkouts stay filtered (table
below), and the scope limits that follow from that are stated explicitly.

Mutation-time verification (`verify_merge_result`, filter-aware git status)
and recovery-time verification (`checkout_matches_commit_*`, raw bytes) now
agree by construction **for every path these edges rewrite**: the bytes on
disk are the blob bytes. Paths a safe checkout does not rewrite keep whatever
bytes earlier — filtered — materialization left on disk; see "Scope limits,
precondition, and residuals" below.

Applied at (production checkout-site enumeration, decision per site):

| Site | Operation | Decision | Reason |
|---|---|---|---|
| `src/git/gitbackend/merge_recovery.rs` `set_branch_target_checked` | checked rollback / root preservation `ResetAttachedRef` | **filters disabled** | result is re-verified raw-byte by `observe_v1_participant_rollback` and `observe_reset` (`capture_normalized`) |
| `src/git/gitbackend/merge_recovery.rs` `abort_merge` | checked native-merge abort | **filters disabled** | restored before-state is re-verified raw-byte by the rollback/recovery observers |
| `src/git/gitbackend/refs.rs` `fast_forward` | user-facing member update | untouched | forward operation; verified by filter-aware state only; porcelain parity retained |
| `src/git/gitbackend/refs.rs` `switch_branch` | user-facing branch switch | untouched | same |
| `src/git/gitbackend/repository.rs` `checkout_commit` | user-facing detached checkout | untouched | same |
| `src/git/gitbackend/repository.rs` `checkout_branch` | user-facing branch materialization | untouched | same |
| `src/git/gitbackend/repository.rs` `reset_hard` (`git_reset` hard) | pull-side member alignment | untouched | forward/pull operation (pull_head flows); filter-aware verification |
| `src/git/gitbackend/merge_prepared.rs` checked upstream fast-forward | forward integration | untouched | forward direction; verified by `verify_merge_result` (filter-aware) |
| `src/git/gitbackend/merge_prepared.rs` post-true-merge `checkout_tree` | forward integration worktree materialization | untouched | same |

#### Clause A scope limits, precondition, and residuals

**Filtered materialization outside the rewrite set — unrewritten smudged
files (open exposure; State F1 = Code P2).** Clause A governs only the paths
the two recovery checkouts rewrite, and both checkouts rewrite **deltas
only**: the SAFE rollback checkout acts on baseline→target differences, and
the force abort checkout equally leaves paths alone where no rewrite is
needed. Paths materialized earlier by *filtered* checkouts — the user's
original porcelain clone/checkout, forward integration through
`merge_prepared.rs` (checked upstream fast-forward; post-true-merge worktree
materialization) and `refs.rs` `fast_forward` — keep their filtered on-disk
bytes. *(Closure annotation, 2026-08-16, adopted decision D1 landed: as of
the filter-policy package, `create_repo` pins `core.autocrlf=false` +
`core.eol=lf` at creation and the clone funnel materializes filters-off —
gwz-born and gwz-cloned worktrees are blob-exact from birth, closing this
exposure for them by construction. The exposure narrows to ADOPTED
worktrees and paths materialized by the user's own porcelain operations;
the doctrine sentinel
`doctrine_sentinel_adopted_crlf_worktree_classifies_ambiguous_in_the_reverse_observer`
(g12) pins the adopted-worktree Ambiguous classification on Windows and
ends CI blindness for this half of the residual. A post-A1 operator
`renormalize` command for adopted worktrees remains the tracked follow-up.
Known spec residual of the D2 probe (State F1, P3): the attribute stack is
read pre-checkout, so attribute coverage introduced by the target state
itself escapes the gate; ~15-LOC hardening recorded as a candidate.)*
*(Corrected 2026-08-25 per R1.2 F-2/C4
(`GwzM5-8A1ReleaseR1-ReviewState.md`): the "~15-LOC hardening"
candidate is UNACHIEVABLE AS WRITTEN — reading attributes from the
target tree needs `git_attr_get_ext`/`git_attr_options.attr_commit_id`,
which `libgit2-sys 0.18.7+1.9.6` does not bind and git2 0.21's
`AttrCheckFlags` does not expose. F1 is instead ACCEPTED AS A NAMED
RESIDUAL at R1 (re-confirmed empirically; realized harm is
real-git-visible divergence with a porcelain re-checkout remedy —
gwz's libgit2-based status runs no config-command drivers, per F6
below), pinned by the g12 doctrine sentinel
`doctrine_sentinel_target_side_attribute_coverage_escapes_the_foreign_filter_gate`
landed at gwz-core `a6ef094`, and owned by the post-A1 renormalize
package.)* The
raw-byte observations are **full-tree**
(`observe_v1_participant_rollback`, `verify_v1_no_mutation_participant`,
`terminal_v1_participant_is_exact` —
`src/workspace_ops/merge/abort/participants.rs:95-192`, via `clean_checkout`
→ `checkout_matches_commit_except`), and they gate normal rollback preflight
(`abort/preflight.rs:69,83,98`), no-mutation verification, and recovery
re-entry (`rolling_back_recovery.rs:44`). Concretely: Windows with
`autocrlf=true`, an ordinary clone, any member with at least one text file
unchanged between `resulting_commit` and `before_commit` → that file is CRLF
on disk in both candidate states → reverse entry classifies Ambiguous and
rollback never starts; where it does start, the After re-verification fails
on unchanged-but-smudged paths. The failure direction is closed — availability
is lost, wrong evidence is never accepted — and CI is structurally blind to
it (all fixtures are LF, and the run-7 fixture pins set
`core.autocrlf=false`). Ordinary Windows-CRLF worktrees therefore remain
**unsatisfiable for the raw-byte model regardless of this amendment**;
matrix-green must not be read as real-Windows exact-evidence closure. Closing
the gap needs its own reviewed decision; candidates named by the reviews:
recovery-grade full re-materialization of the participant worktree at
rollback entry, filters-off at member materialization, or a clean-side
(filtered) comparison mode for the reverse observers.

**Soundness precondition — clean-idempotence on blob bytes; foreign-filter
policy is an open decision.** Writing raw blob bytes is status-clean only
where every clean filter covering a rewritten path satisfies
`clean(blob_bytes) == blob_bytes`. Git's builtin filters satisfy it: crlf/eol
cleaning is idempotent on LF-normalized blob content, `ident` blobs are stored
in cleaned `$Id$` form, and Git LFS pointer blobs round-trip clean unchanged.
Where the precondition fails — encrypt-on-clean filters of the git-crypt
class, where `clean(blob_bytes) != blob_bytes` — a rewritten path is
status-dirty immediately after the checkout and `verify_merge_result` fails
**after** `transaction.commit()` in `set_branch_target_checked`
(`merge_recovery.rs:217-218`), or after the abort restore: a previously
working production v0 abort becomes a recovery-required wedge, on every OS —
the ref has moved, the retry hits the idempotent arm
(`merge_recovery.rs:189-196`) and fails the same verification, wedged until
manual intervention. No silent corruption, but a real behavioral regression
class for such repositories. The policy was recorded here as an explicit
OPEN DECISION; **DISCHARGED 2026-08-16 by adopted decision D2 (A′ refined
refusal, landed in the filter-policy package)**: option (a) — both
recovery-grade checkout sites now inspect the rewrite set's `filter`
attributes pre-mutation and refuse with a typed error naming path and
filter when a foreign non-passthrough driver is configured (`lfs`
allowlisted by name; True/False/Unspecified attributes pass; non-UTF-8
driver names refuse fail-closed). The refusal precedes every ref and
worktree mutation, converting the post-`transaction.commit()` wedge into a
pre-mutation stop. Mechanism precision from the landing review (F6):
foreign clean/process commands never execute inside gwz (libgit2 runs no
config-command drivers), so in-gwz status cannot manufacture the wedge
in-process; the hazard the refusal closes is real-git-visible divergence
on covered paths after a raw-byte rewrite — strictly worse than the LFS
pointer-bytes surprise since clean does not round-trip. A dedicated error
code (vs the house `DirtyMember` recovery-refusal idiom) is a candidate
for a future deliberate wire train (F3). Related user-visible effect even where
the precondition holds: an LFS-managed path rewritten by a recovery checkout
carries **pointer bytes** on disk after rollback — status stays clean, the
worktree content is a surprise, and a porcelain re-checkout restores the
filtered content.

*(Citation correction, 2026-08-25, R2.3 [A-ii]: "the landing review
(F6)" above, and the "Closure annotation" styling earlier in this
clause, reference a review round that was never filed as an artifact
— the F1-F6 findings were recorded in-body only, at the `9939b02`
landing. The filed reviews now exist:
`GwzM5-8A1ReleaseR1-ReviewState.md` re-verified the F6 mechanism
empirically (the probe rollback proceeds and gwz's libgit2-based
status stays clean) and carried it into the signed C2 named-residual
text; `GwzM5-8A1ReleaseR2-ReviewState.md` is the D1 package's first
filed review.)*

**Native-stash residual (tripwire owed).** libgit2's `git_stash_save`
internally resets stashed tracked files via its own checkout **with filters
active**, and exposes no checkout options (diagnosis §3.A item 3, the
`g15.rs:170` smoking gun). On Windows, a tracked dirty file swept by a
preservation stash re-materializes filtered; any raw-byte comparison of that
worktree against a commit afterwards remains exposed until this gets its own
decision (candidate: post-stash re-checkout of affected paths with filters
disabled, which is a new mutation and needs its own review). Since the run-7
fixture pins (`core.autocrlf=false`) this residual has **no failing sentinel
anywhere** — CI is structurally blind to it. A live tripwire is owed: a
windows-matrix expected-fail sentinel or a classification-ledger entry. The
lane owner is adding the `GwzWindowsMatrix-Classification.md` ledger entry;
this amendment records that entry as the tracking record for the debt.

*(Tripwire DISCHARGED 2026-08-25 — R2 release-train package, landed
gwz-core `bed072a`, recorded under R2.3's signing conditions
[C-i..C-iii]. The owed sentinel now exists AND rides every lane:
`crlf_sentinel_unpinned_worktree_materializes_blob_exact` (g01) is
`#[should_panic(expected = "CRLF-CLASS-SENTINEL: smudge source is
LIVE")]` — green while the smudge class exists, failing loudly the
day the class closes, so this text and the frozen ledger move with
it; `windows-matrix.yml`'s `crlf-sentinel` job runs it beside the
count-pinned six-test pinned half (both halves green-when-correct;
anti-vacuity rests on the two exact counts, since libtest exits 0 on
a zero-match filter). The gwz-BORN half of this exposure is closed by
D1's birth pins — inside a born repo the crlf filter is identity, so
`git_stash_save`'s internal reset is a byte no-op — **for the
config-driven filter class only**: attribute-driven smudge
(`eol=crlf`, `ident`, foreign `filter=`) still reaches `stash_save`
inside a born repo and stays permanent fail-closed doctrine [C-i].
Proof: `stash_round_trip_on_a_born_repo_is_a_filter_no_op` (g11)
against a live CONTROL arm that measures the smudge on macOS —
closed by construction, proven non-vacuously off-Windows; Windows
execution rides the next matrix dispatch [C-iii]. The restore leg
(`stash_apply`/`stash_pop`, edge E13 of R2.3's twelve-edge
inventory) is exercised by g11:94-115. The ADOPTED-worktree half
stays open, fail-closed, owner the post-A1 `renormalize` package
[C-ii].)*

### Clause B — the checked-artifact private area is invisible to the preservation-image model everywhere

`CHECKED_ARTIFACT_PRIVATE_PATH` (".gwz/checked-artifacts") was excluded only in
`capture_normalized` and the checkout-overlay paths, while `capture` /
`capture_inner` and `decode_stash` saw the area (diagnosis §3.B). The
Windows-only durability anchor
(`.gwz/checked-artifacts/.ca1-durability-anchor-<32hex>`, content
`GWZ-CHECKED-ARTIFACT-DURABILITY-ANCHOR-V1\n`, created by `prepare_private`,
never removed by `finish()`) therefore entered stash sweeps and un-normalized
captures as one extra `ImageEntry`, breaking `preimage_sha256` equality at the
`CreateStash` post-observation.

Amended semantics: the private area is excluded from **every** capture and
decode path of the preservation-image model —

- `capture_inner` (thus `capture` and the public `preservation_image`
  observer) excludes it, uniform with `capture_normalized`;
- `decode_stash` excludes it from all four stash-derived trees (head, index,
  worktree, untracked) before entry-building and before the dirty-summary
  computation;
- the checkout-match overlay paths already excluded it (unchanged).

Two consistency facts the State-axis review verified and asked recorded: the
frozen preimage guard (`capture_normalized`) **already** excluded the private
area before this amendment (`raw_excluded_paths`,
`preservation_image.rs:224-235`), so Clause B restores the frozen equation's
satisfiability rather than overriding its semantics — a consistency
completion, not a semantic override; and `checkout_matches_commit_*` was
already private-area-blind through the overlay exclusion, so the rollback
observers' coverage is unchanged by Clause B.

Symmetry audit of every comparison over these images:

- `stash_for_merge_preservation_checked` compares live `capture` against a
  recorded preimage produced by `capture` (merge preserve flow) and a decoded
  stash against the same — both sides now exclude; the comparison stays
  symmetric (pre-amendment it was symmetric only because the anchor sat on
  both sides).
- `observe_create_stash` compares a decoded stash against the
  `capture_normalized` guard — previously asymmetric (the class-B defect),
  now both sides exclude.
- `observe_plain_stash` / preserving-origin recovery compare decoded stashes
  and live `preservation_image` against recorded values — uniform.
- No caller relies on seeing private-area content in a preservation image:
  the checked-artifact layer manages its area through its own directory-handle
  observation machinery, never through preservation images.

Restore/pop safety (stash untracked tree may contain a swept anchor): no
production recovery flow applies or pops the preservation stash — restore is
the user-driven `gwz stash` command path only. On Unix a re-materialized
anchor is inert bytes (`prepare_private` is a no-op) and is invisible to the
image model. On Windows a re-materialized stale anchor cannot match its
identity-derived name, so `anchor_state` classifies it `Invalid` and the next
checked-artifact operation fails closed — pre-existing behavior, unchanged by
this amendment (the amendment changes what the image model sees, not what the
native stash sweeps). The MAX_PATH exposure of `ca1-*` residue and the option
of relocating the private area under `.git/` (diagnosis §5.B alternative)
remain open follow-ups. Doctrine boundary (Code P3-2): the workspace-visible
catalog-bootstrap siblings
`.gwz/checked-artifacts-catalog-bootstrap-v1.{scratch,active,staging}`
(`catalog_names.rs:38-49`) are prefix-siblings of the private area and are
correctly **not** discounted by the image exclusion or the B′ predicate;
crash-retained bootstrap residue would reproduce anchor-style symptoms on any
OS — the private-area relocation follow-up would subsume it.

### Clause B′ — recovery-grade cleanliness predicates are private-area-blind (additional consumer)

Fourth consumer of the invisibility doctrine, added in-scope by the lane
owner: `ensure_clean_recovery_state` (rollback preflight,
`src/git/gitbackend/recovery_support.rs`) refused any dirty status, so the
permanent Windows anchor made every checked rollback in an anchor-bearing repo
return `DirtyMember` forever (run-6 evidence:
`g15::root_preservation::observation::marker_presence_matrix_covers_attached_and_restore_clean_forms`).

Amended semantics: checked rollback/recovery preflights and their
post-verifications classify **untracked entries under the private area** as
product infrastructure, not user work. Implemented as one shared predicate
(`status_dirty_outside_checked_artifact_private`) that mirrors `is_dirty`
while discounting only untracked private-area entries; staged, unstaged, and
unresolved entries anywhere — including inside the private area — and any
untracked entry outside it remain dirt. Applied at:

- `ensure_clean_recovery_state` (the cited preflight), and
- `verify_merge_result` (its paired post-verification; without this the
  preflight fix would merely move the failure after the ref mutation, since
  `set_branch_target_checked` and `abort_merge` re-verify through it).

Actual reach of the `verify_merge_result` change (State F3 / Code P3-1):
beyond the rollback pairing, it also post-verifies forward integration
(`merge_prepared.rs:248, 261, 335` — checked upstream fast-forward,
up-to-date, and post-true-merge) and the resolution commit
(`merge_recovery.rs:459`), so those flows now tolerate private-area untracked
residue as well — benign in direction, and it heals a pre-existing Windows
failure mode where a forward root merge would error after mutating.

The predicate deliberately went in at the verification layer, not into
`status()` itself: `status()` is the porcelain-parity user-facing surface and
must keep reporting the residue. Untouched siblings, flagged for the
reviewers (same doctrine; all fail-closed today):

- `validate_abort_index_and_worktree` (`status.untracked > 0`) and
  `validate_resolution_index_and_worktree` — conflict-state preflights; safe
  today only because anchors are root-only and the root never enters
  native-merge conflict paths (both premises verified in this tree, neither
  pinned by a test).
- Forward preflights still using raw `is_dirty`: `merge_prepared.rs:78,155`;
  `merge_support.rs:90`; `refs.rs:127` (`switch_branch`); and
  `preservation.rs:189`'s retry arm (the latter unreachable from the checked
  root path — `observe_create_stash` absorbs the retry as `After` first).
  Consequence: an anchor-bearing repo still refuses checked **forward**
  merges at preflight with `DirtyMember` — unreachable today (members bear no
  anchors; the root does not forward-merge) but the same availability class
  B′ fixed for rollback.
- v0 preservation planning is status-based
  (`stash_for_merge_preservation`'s preservable check,
  `preservation.rs:213-220`): on Windows an anchor-bearing root plans
  preservation forever and can create an anchor-only stash whose
  post-amendment decoded image certifies nothing — fail-closed; v1 planning
  is image-based and immune (its bundle classifier additionally rejects
  nothing-dirty stashes); deferred under v1 supersession.
- Plan/execute divergence for private residue in members (Code P3-3):
  planning counts private untracked as preservable (`preserve/plan.rs:95,224`)
  while execution evidence is anchor-blind, so a member bearing **only**
  private residue — impossible today, residue is root-only — would journal a
  stash and then fail closed pre-mutation at
  `stash_for_merge_preservation_checked`'s `current.dirty == default` check.
  Root flows verified consistent end-to-end by the Code-axis review.

## 3. Affected behavior

- **Paths rewritten by recovery-grade checkouts land LF (blob-exact) on
  Windows.** After a checked rollback, abort restore, or preservation reset,
  files rewritten by those checkouts contain the exact blob bytes even under
  `core.autocrlf=true`; paths those checkouts do not rewrite keep their prior
  materialization (see the Clause A scope limit), so a rolled-back worktree
  can carry **mixed line endings** — rewritten paths LF, untouched paths
  CRLF. `git status` remains clean **wherever the clean-idempotence
  precondition of Clause A holds — all Git builtin filters**: status compares
  in the clean direction (worktree → object database), and CRLF-stripping is
  idempotent on LF-only content, so an LF worktree file hashes to its LF blob
  under any autocrlf/eol setting. For LFS-managed rewritten paths the
  worktree carries pointer bytes (status-clean; porcelain re-checkout
  restores content). Where a foreign non-idempotent clean filter covers a
  rewritten path, the checkout is **not** status-clean and the checked
  mutation wedges fail-closed — the open decision recorded in Clause A. Users
  who re-checkout rewritten paths through porcelain get their filtered
  representation back; the recovery-grade worktree is the exception that
  keeps evidence exact, not a change to normal checkout behavior.
- **Ordinary Windows-CRLF worktrees remain outside the raw-byte model's
  satisfiable set.** This amendment fixes what the recovery edges themselves
  write; it does not re-materialize paths written earlier through filters, so
  reverse entry on a porcelain-cloned or forward-integrated Windows worktree
  can still classify Ambiguous before any mutation — availability loss only,
  never wrong evidence (Clause A scope limit; own reviewed decision owed).
- **Unix behavior is unchanged** at the checkout edges (no filters were
  active) and for images in practice (no private-area files exist unless
  crash residue was planted — which is precisely the latent case the
  exclusion now covers on every OS).
- **Preimages spanning the amendment boundary fail closed.** A preservation
  preimage recorded by a pre-amendment binary from a repo whose private area
  was visible to `capture` (Windows, or crash-retained `ca1-*` residue) will
  not match a post-amendment capture; recovery classifies it as evidence
  mismatch and refuses mutation — no silent adoption, stash contents intact.
  In-practice exposure is Windows-only, where these flows were already failing
  at the same boundaries. The preimage frame
  (`gwz.merge-preservation-preimage/v1\0`) is deliberately unchanged while
  coverage changed (State F5): equal-frame hashes recorded before and after
  this amendment are **incomparable-by-design**, safe because every
  comparison is fail-closed equality and no released writer exists — v0.10.5
  predates `preservation_image.rs`, `preservation_root.rs`, and the entire
  checked-artifact layer, so no released binary ever recorded one. The
  acceptance-time annotations of the amended contracts must state this
  discontinuity so v1-frame hashes are not treated as a stable cross-version
  identity.
- **Rollback availability on Windows is restored** in repos where
  checked-artifact machinery has ever run (Clause B′); genuine untracked user
  work still blocks rollback exactly as the contract tests pin.

## 4. Verification (macOS, this tree)

Red/green discipline — repros written and run RED before any fix:

- Class A (`reverse_rollback/phases.rs::integrated_rollback_remains_exact_under_repo_local_autocrlf`,
  repo-local `core.autocrlf=true` set after LF fixture commits):
  RED `assertion 'left == right' failed: left: Ambiguous, right: After` at the
  post-rollback observation — the exact run-6 signature; GREEN after Clause A,
  including a byte assert that the rolled-back file is `b"before\n"`.
- Class B (`g15/root_preservation/stash.rs::create_stash_remains_exact_over_checked_artifact_private_residue`,
  fake anchor planted before `CreateStash`): RED
  `PreservationEvidenceMismatch: "root preservation mutation failed exact
  post-verification"` — the exact run-6 emit; GREEN after Clause B, including
  decoded-stash == durable-preimage equality.
- Clause B′ (`g12.rs::checked_rollback_tolerates_checked_artifact_private_residue`):
  RED `DirtyMember: "rollback requires a clean index and worktree"`; GREEN
  after the predicate change, with the anchor bytes proven untouched by the
  rollback.
- Suites: `git::tests::g15` 45 passed (44 baseline + new repro);
  `checked_artifact::` 244 passed; full `--lib` suite green; `cargo check`
  clean; `cargo fmt` applied.

## 5. Review routing

Both clauses change production semantics on Windows and therefore trigger the
program's State-axis review per the campaign rules quoted in the diagnosis
(§5); the checkout-edge and predicate changes are Code-axis reviewable against
the enumeration table above.

Round 1 outcomes (both reports filed):

- **State axis** (`GwzM5-8ExactEvidenceAmendment-ReviewState.md`): NO-GO on
  this document's round-1 text only — "the code delta is sound on the state
  axis" — findings F1-F6, all documentary; the reviewer pre-committed to GO
  on a re-read of the revised text.
- **Code axis** (`GwzM5-8ExactEvidenceAmendment-ReviewCode.md`): GO, no code
  changes; one P2 documentation condition (same class as State F1) and three
  P3 tracked notes.

This revision applies all of the above in one document-only round: State
F1+Code P2 (Clause A scope limits: unrewritten smudged files, tempered §3
lead), F2 (clean-idempotence precondition, foreign-filter OPEN DECISION, LFS
surprise), F3+Code P3-1 (verify_merge_result forward/resolution reach and the
forward-preflight sibling enumeration), F4+Code P3-3 (v0 planning anchor-only
stash; plan/execute divergence), F5 (preimage-frame discontinuity), F6
(stash_save tripwire owed; classification-ledger entry as tracking record),
and Code P3-2 (catalog-bootstrap doctrine boundary). **Pending State-axis
re-read (pre-committed GO) and acceptance-time annotation of the amended
contracts.**
