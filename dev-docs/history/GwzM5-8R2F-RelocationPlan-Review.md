# GwzM5-8 R2-F Relocation Plan — single-axis peer-blind review

Reviewer: Opus, single axis, peer-blind. Target:
`GwzM5-8R2F-RelocationPlan.md` (DRAFT, 2026-09-01). Two-round cap; this is
round 1.

Verified against gwz-core `main` = `ea3a9246e5230a2b6ef04e269c9454283bd6db0a`
(`chore(release): gwz-core 0.12.1`), working tree clean. Read-only: no commit,
no edit, no build outside this report. Every tree fact below was re-derived
from the tree itself, not taken from either the plan or the trace — per the
house precedent that two prior reviews caught plans built on stale records.

---

## VERDICT: **NO-GO**

Not because the plan is careless — its four carried corrections all verify at
the tree, and the trace it rests on is sound. It is NO-GO because the
package's **central recommendation (§2, OPEN-R1 leave-in-place) is grounded on
an inversion of its own controlling trace**, and under that recommendation the
landing does not produce the state §4's exit criteria promise. The operator is
being asked to rule on evidence that says the opposite of what the tree says.
That is a re-plan of §2 and §4, not a conditions list.

Counts: **1 × P0, 3 × P1, 5 × P2, 8 × P3** (17 findings).

---

## Findings

### [P0-1] §2's OPEN-R1 grounds invert the trace: the cited facts are costs of leave-in-place, not grounds for it — and the plan's own §4 exit criteria fail under it

Plan §2 recommends leave-in-place and gives as its first two grounds:

> resident legacy residue stays catalog-unobservable under the current
> refusal/skip path (trace Q6); the Windows anchor-permanence evidence
> recorded at Phase4Closure ~:180-190 says leave-in-place keeps the catalog
> unobservable there

The trace says both of these as **costs**. `GwzM5-8R2F-RelocationTrace.md`
§6.1's heading is *"Leave-in-place still keeps the catalog unobservable — the
refusal path"*, and §6.2(b) states the conclusion outright: *"leave-in-place
is not a correctness hazard in the classifier — it is a leak plus a permanent
catalog blockage. **The blockage is the binding half.**"* The trace's "What
the plan must decide" item 2 names the deciding facts as *"the permanent
Windows catalog blockage (§6.1) and the unreclaimable orphan (§6.2(a))"* —
i.e. the two reasons **against** leaving it. The plan has restated both as
support.

The plan's hedge "the current **refusal/skip** path" papers over the
distinction that decides the question. It is a refusal, not a skip, and I
verified the chain end to end:

- `src/checked_artifact/capability/pre_catalog/provider/interior.rs:87` —
  `let class = exact_row(&name, mode)?;` — the `?` propagates per entry; there
  is no skip arm.
- `interior.rs:437-440` — every child that is neither an
  `InfrastructureSlotV1::ALL` name nor an `ActiveAction` row returns
  `Err(CheckedFsError::ambiguous("catalog interior", "catalog directory
  contains an unowned child"))`.
- `.../provider/completed.rs:64-74` — a `None` `completed_record` yields
  `ambiguous("completed catalog", "final catalog does not have the exact
  retired layout")`.

**And the blockage is worse than the trace states, on a fact neither document
carries.** It is not conditional on residue *files* at all, and not conditional
on Windows:

- `.../provider/directory_mutation.rs:179-185` —
  `publish_final_directory` refuses outright when the Final directory already
  exists: `"final destination was present in the fresh aggregate"`.
- `src/checked_artifact/cleanup.rs:62-175` — `finish()` removes the goal,
  source and authority *files* from the private area and **never removes the
  private directory**. There is no `remove_dir` of the private area anywhere
  in production (`src/checked_artifact/platform.rs:776`, `:823`, `:1068` and
  `.../provider/leaf_observation.rs:650`, `:664` are all `let _ =
  remove_dir_all` test scaffolding).
- `src/checked_artifact/catalog/enumeration.rs:373-388` (`fixed_roles`)
  recognises a directory named `checked-artifacts` as
  `CatalogRecognizedNameV1::Final` **by name**, regardless of contents.

So on **every** platform, any workspace that has ever run one checked-artifact
mutation is left with `.gwz/checked-artifacts/` present — empty on Unix, and on
Windows holding the permanent `.ca1-durability-anchor-<32hex>`
(`platform.rs:361-368`, `:687-694`; `prepare_private` is `Ok(())` on non-Windows
at `platform.rs:344-352`). Under leave-in-place that directory is exactly where
the catalog's `Final` still points, and `publish_final_directory` refuses on
its mere presence. The catalog is not merely "blocked on Windows" — it cannot
bootstrap in any already-used workspace anywhere. gwz-core 0.12.1 is released,
so that is the installed base, not a hypothetical.

Consequence for the plan's own text: §4's *"Then **E4 is open** and the M5c
sequence proceeds"* and R1.2's *"UNBLOCKS O1/O2"* do not hold in substance
under §2's recommendation. O2 is literally *"First production catalog
activation"* (`GwzM5-8R2E-Plan.md:47`).

**Required:** §2 must be re-derived. The recommendation must state what
leave-in-place actually hands E4, and must weigh the option the plan never
considers — that the *catalog's* `Final` leaf move instead of (or as well as)
the legacy leaf. `GwzM5-8R2DPhase4Closure.md` §2.7's decision text — *"the
legacy leaf private area is relocated out of the catalog Final directory"* —
is satisfied by either direction; moving `Final` is the only variant that both
leaves resident residue undisturbed **and** leaves the catalog a clean
directory to publish into, and it is free pre-activation for exactly the
digest reason the plan already carries as correction 3. That comparison is
owed to the operator before the ruling is sought, not after.

### [P1-1] R1.1's instruction for the second spelling contradicts the trace requirement that §1.4 claims to carry

Plan R1.1: *"update the second spelling site (`preservation_image.rs:8`) so
the dirt exemption and blindness prefix **track the new location**"* —
singular.

The trace's requirement is the opposite, and is stated twice:

- §4.2 item 2: *"If residue is left in place (§6), **both exemptions must keep
  covering the old path as well as the new**."*
- §6.2(c): *"Left-in-place residue stays exempt only while those constants
  still name the old path."*

Verified consumers of the constant:

- `src/git/gitbackend/preservation_image.rs:8`
  `pub(super) const CHECKED_ARTIFACT_PRIVATE_PATH: &str = ".gwz/checked-artifacts";`
- `preservation_image.rs:224-232` — `raw_excluded_paths` seeds the preservation
  image's blindness list with it.
- `src/git/gitbackend/repository_support.rs:131-135` —
  `checked_artifact_private_status_path`, consumed by
  `status_dirty_outside_checked_artifact_private` (`:115-129`), whose doc
  comment states the ground: *"The private area (`.gwz/checked-artifacts`) is
  permanent product infrastructure … must not classify its untracked residue
  as user work."*

Moving the constant with leave-in-place re-classifies leftover residue as
**user dirt** and makes it **visible to the preservation image** — a
user-visible regression on the rollback/recovery preflight, in the exact class
the comment was written to prevent. Both halves, with their real scope:

- **Dirt half — partially masked, and the masking is the wrong thing to lean
  on.** `status_dirty_outside_checked_artifact_private` has two production
  callers: `repository_support.rs:97` (`verify_merge_result` →
  `ErrorCode::GitCommandFailed`) and
  `git/gitbackend/recovery_support.rs:363` (`ensure_clean_recovery_state` →
  `ErrorCode::DirtyMember`), the latter inside `set_branch_target_checked`,
  which is the checked-rollback primitive behind root-preservation rollback
  and `gwz merge abort`. So the failure mode is a **blocked rollback/abort**.
  In a workspace root with a healthy managed block the residue never reaches
  `status.files` at all — `sync_workspace_boundary.rs:65-71` emits `/.gwz/`
  and `git/gitbackend/repository.rs:166-167` sets `include_ignored(false)` —
  so the regression is masked *there*. It is **not** masked before the first
  `sync_workspace_boundary`, after a user edits `info/exclude`, or in a member
  repo (members never get a gwz-managed exclude; `:88-90` writes only
  `<root>/.git/info/exclude`). Those are precisely the cases the exemption was
  written for, so "the ignore rule usually covers it" is not a defence.
- **Preservation-image half — largely unmasked.** Of the four
  `raw_excluded_paths` consumers, only `capture_normalized`
  (`preservation_image.rs:40`) is protected in the real v1 flow, by
  `workspace_ops/merge/preserve/artifacts.rs:572` pushing `".gwz"` wholesale.
  `capture()` (`:52`), `decode_stashes` (`:356`) and both sides of
  `checkout_matches_commit_with_overlay` (`:195-196`) pass `&[]` and are blind
  **only** to the constant. Old-path residue would enter the preimage hash and
  flip `preservation.rs:305-317` to `PreservationEvidenceMismatch` and
  `preservation_root.rs:245-274` to `Ambiguous`.

The fixtures that pin the current behaviour and go red:
`src/git/tests/g12.rs:812-840` (dirt half),
`src/git/tests/g15/root_preservation/stash.rs:295-343` (image half — its
fixture passes `excluded_worktree_paths = Vec::new()` at
`g15/root_preservation/support.rs:133`, so it rests **solely** on the
constant), and `src/git/tests/g15.rs:461-500`, which exists precisely to prove
the blindness is **leaf-specific, not `.gwz`-wide** (it asserts drift under
`checked-artifacts` is invisible while drift at the sibling `.gwz/sibling` is
visible).

This is not a speculative regression — it is a test that goes red, and the way
it goes red sets a trap for the builder. `src/git/tests/g12.rs:824-840` plants
`.ca1-durability-anchor-deadbeefdeadbeefdeadbeefdeadbeef` into
`.gwz/checked-artifacts`, under the comment *"rollback preflight and
post-verification must stay available over it"*, then asserts
`set_branch_target_checked` still returns `updated` and leaves the anchor
byte-identical. With the constant moved and the old path no longer exempt, that
anchor is untracked work outside the private area, so the preflight rejects and
the test fails. `src/git/tests/g15.rs:461-470`
(`complete_checkout_excludes_only_the_checked_artifact_private_tree`, committing
`".gwz/checked-artifacts/protocol"`) fails the same way for the blindness half.

**The trap:** under a HARD 250 cap the cheap fix is to repoint both fixtures at
the new leaf, which turns the suite green while silently removing the exemption
that real users' resident residue depends on. The expensive-but-correct fix is
to make both constants cover the old path *and* the new. R1.1's brief as
written points at the cheap one.

The plan's §1 correction 4 carries the *fact* of the second spelling but drops
the *obligation* attached to it. This is the one place where the plan's claim
to be "grounded in the trace" fails on a direct instruction.

### [P1-2] The quarantine leaf must be NAMED now; the stated constraint is not sufficient to make R1.1 buildable or reviewable

R1.1 defers the name to build time under one constraint — *"no longer than
`Final`'s footprint"*. That constraint is correct in direction (it keeps the
measured 197-char workspace suffix from growing) but it is not the binding
set, and three further constraints are load-bearing, all verified:

1. **ASCII, or a live production path panics.**
   `catalog_names.rs:58-60` — `relative_path` calls
   `.expect("fixed catalog name is valid ASCII")`. `observation.rs:93` computes
   `policy.private_parent()` unconditionally on `acquire`, including on the
   git-directory observe-only production path reached by every merge
   preservation preflight (`preservation_root/files.rs:28-35`).
2. **Must not case-fold onto a fixed catalog role.**
   `catalog/enumeration.rs:287-294` returns `alias_error()` — a hard refusal,
   not `None` — for any name that `eq_ignore_ascii_case`-matches
   `checked-artifacts`, `…bootstrap-v1.active` or `…bootstrap-v1.staging`
   under `PathComponentMode::AsciiCaseFold`.
3. **Must not enter the scratch-family grammar.**
   `catalog/enumeration.rs:296-313` against
   `SCRATCH_PREFIX = b"checked-artifacts-catalog-bootstrap-v1.scratch."`
   (`catalog/scratch.rs:6`) — a name equal to the family or family+`.` returns
   `malformed_error()`. (A ≤17-char leaf cannot trip this, but the rule belongs
   in the brief since the length constraint is itself under review.)

Beyond correctness, the name is an *input* to work R1.1 must do in the same
commit: the `max_path.rs` fixture arithmetic is calibrated to the 23-char
`.gwz/checked-artifacts/` prefix (`max_path.rs:33-44`, fixture root depth 120
at `:36`), and the `preservation_image.rs` digest re-pin cannot be computed
without it. A step whose first act is an unmade naming decision is not a
single-goal step, and a reviewer cannot check the constraint was met.

**Required:** name the leaf in the plan.

### [P1-3] R1.1 carries more than one semantic decision, against the house rule the plan's own §3 heading invokes

`GwzWtPlan.md:45-49`: *"No step carries more than one semantic decision (the
gwz-log actuals: every ≥350-LOC step with two decisions died at the cap; the
≤250 steps did not)."* R1.1 as briefed carries at least three unmade ones: the
leaf name ([P1-2]); whether the variant joins `ALL` ([P2-1]); and the dual-path
exemption semantics ([P1-1]). Each is a decision the trace explicitly handed to
the plan ("What the plan must decide" items 1, 2, 4), and each is being pushed
into the builder under a HARD 250 cap.

To be fair on the arithmetic: the **LOC** looks affordable. The private-area
literals funnel through four one-line helpers
(`tests/recovery_protocol.rs:3-5`, `tests/leaf_publication.rs:14-16`,
`tests/staging_recovery.rs:3-5`, `tests/removal_recovery.rs:3-5`), so most of
the edit is a handful of lines. The problem is not size, it is that the step's
**verification surface** is ~31 test functions across 10 files, of which ~21
fail hard, spanning both aggregate-gate partitions and the boundary checker —
while three semantic questions are still open inside it. That is the shape the
cited gwz-log actuals say dies at the cap, and it would burn the single `-B`
re-charter. Deciding the three questions in the plan converts R1.1 from a
step-with-decisions into a step-with-edits, which is what the cap is sized for.

### [P2-1] The `ALL`-membership decision — trace decision item 1 — is answered only by implication, and the trade is never stated

Adding a variant to `enum CatalogPrivateNameV1` (`catalog_names.rs:21-26`) and
adding it to `CatalogPrivateNameV1::ALL` (`catalog_names.rs:29-34`) are
separate acts. R1.1 says only *"Add the quarantine variant to
`CatalogPrivateNameV1`"*, then asserts *"the
`version_digest`/`historical_collision_digest` movement is BY DESIGN"* — which
is true **only if** it joins `ALL`. The plan thus decides the question by
implication while never naming it.

The trace named the trade: *"Outside `ALL` it has none of that coupling but
also gains none of the collision-domain protection."* I verified what that
protection is, so the decision can be made on substance rather than by
accident:

- `capability/collision.rs:239-251` — `for_root` maps `ALL` onto `members`.
- `.../provider/snapshot.rs:39-72` — `reject_private_collisions` refuses a git
  index path that overlaps any member: `ambiguous("private namespace
  collision", "Git index path … overlaps reserved path …")`.
- `collision.rs:261-271` → `.../provider/digests.rs:136-145`
  (`historical_digest` frames `domain.version_digest()`).

The digest chain of correction 3 verifies exactly as claimed — see §Mandate 1
below — so the "free pre-activation" argument is sound. But the plan owes one
sentence choosing in/out and saying why.

### [P2-2] The MAX_PATH correction train misses the two homes that are the claim's origin, and perpetuates a dead citation anchor

R1.2 corrects the rider at Phase4Closure §2.4/§2.7 and evidence-map F-5. The
false rider has **five** homes; the two it misses are the ones every other
record cites:

- `GwzM5-8R2DSettledTuple.md:792` — *"MAX_PATH product exposure (~173-char
  `ca1-*` names), retired **by** that relocation | checkpoint :1022-1024"*.
  **Not in R1.2's list.**
- `CurrentProgramCheckpoint.md:3192-3194` — *"MAX_PATH product exposure
  (~173-char `ca1-*` names; the private-area relocation option under `.git/`
  would retire it — candidate for R2-F scope)"*. **Not in R1.2's list**, and it
  is the origin the other four cite.

Worse, the shared anchor is stale: every record cites *"checkpoint
:1022-1024"*, but `CurrentProgramCheckpoint.md:1018-1028` is now unrelated
A1-release text — the claim has drifted to `:3192-3194`. R1.2 as written would
quote the corrected arithmetic *at* a dead anchor. Per the checkpoint's own
citation-drift note (`:2628-2632`, the corpus's resolution index), the
checkpoint is the filing home for exactly this class.

Note also that `GwzM5-8R2DSettledTuple.md` is a settled record. The plan should
say how it corrects one — dated annotation in place, or a superseding entry at
the resolution index — rather than leaving the builder to invent a governance
form.

### [P2-3] The in-code records the trace enumerated cannot ride R1.2, because R1.2 is docs-only

Trace §4.2 item 5 lists the gate-lift record set, and two of its five entries
are **in code**:

- `src/checked_artifact/catalog.rs:10-16` — the allow-reason *"`recover_or_create`
  gains its first production caller in R2-E, behind the Phase 4.3 coexistence
  criterion (plan §5 item 2)"*, which the lift makes stale.
- `src/checked_artifact/namespace/mod.rs:8-23` — *"the allow's expiry is **E4's
  landing, wherever R2-F puts it**"*.

R1.2 is scoped *"(docs; rides R1.1's landing or follows immediately)"*, so
neither can land there; and R1.1's brief does not mention them. Note this is
not a free edit either: `catalog.rs` is a `PROTECTED_SOURCE_TREE_DIGESTS` root
(`scripts/checks/check_checked_artifact_boundaries.py:189`), so touching the
reason string moves a tree digest and must be in the landing commit
(`check_lane_commits.sh` gates each commit's own tree). R1.1's companion list
must name it or the plan must say the reason strings stay.

The plan's *"E4's gate note is marked open"* is ambiguous between the plan's
E4 gate note and the in-code `namespace/mod.rs` note; say which.

### [P2-4] Trace decisions 5 and 7 are not answered at all

The trace closes with seven decisions "the plan must decide". The plan answers
1 (by implication, [P2-1]), 2, 3 (partially, [P3-2]), 4 (wrongly, [P1-1]) and
6. It is silent on:

- **Item 5** — *"Whether the A1 gate gains a check that fires"*, given
  §3.2's finding that the gate is an `#[allow]` with no `#[expect]`, no
  `unfulfilled_lint_expectations` lint, and no caller count. I re-verified the
  premise: `src/operation/workspace_mutator_lock.rs:44-47` is still
  `#[allow(dead_code, reason = "R2-C0 freezes the checked catalog borrow …")]`,
  and `recover_or_create` still has no production caller. Since this package
  *lifts* that gate, "moot at the lift" may well be the right answer — but it
  has to be written, because the lift is precisely when a silent gate stops
  being harmless.
- **Item 7** — *"Whether `GwzM5-8R2F-EvidenceMap.md` §2.3 is re-owned first"*,
  given that F-4's and F-5's interim actions have both landed (G-5 as
  `src/git/tests/g15/root_preservation/max_path.rs`; F-4's correction (a) as
  `ready_edge_prologue`). R1.2 edits F-5 for the MAX_PATH rider while leaving
  the row stale in the other direction.

### [P2-5] "aggregate-gate pins if lib counts move" understates a near-certainty and omits the provenance duty the driver actually enforces

R1.1's companion list ends *"aggregate-gate pins if lib counts move"*. On
R1.1's own test list — *"writes land under the quarantine leaf on both roots;
resident legacy residue at the OLD leaf remains …; the digest movement is
asserted once"* — new tests land under `checked_artifact::`, so the pins move;
this is a **when**, not an **if**.

The pins are `scripts/checks/run_r4bg_aggregate_gates.py:300-301`
(`_fault_count("447 passed", "457 passed")`, the `checked_artifact::`
partition) and `:302-305` (`_fault_count("1097 passed", "1098 passed")`, the
lib remainder — which is where the `src/git/tests/**` fixtures fall, since the
remainder skips only `checked_artifact::` and
`workspace_ops::merge::v1_lifecycle::`). So the plan should expect **both**
pairs to move: the module's new tests move the first, and any added or renamed
fixture under `src/git/tests/**` moves the second.

Precisely: the *path-literal edits* R1.1 makes to existing test bodies move
neither pin — the markers count `N passed`, and renaming or rewriting a test
does not change the count. It is R1.1's **new** tests that move them. So the
plan's "if" is not merely cautious, it is pointed at the wrong half of its own
step.

The omission that matters is the duty attached to moving them.
`_fault_count`'s docstring (`:67-120`) is a standing house discipline: every
moved value needs a **dated per-OS provenance block** stating MEASURED vs
DERIVED per value, and a derived linux count must be marked
`LINUX-COUNT-OWED` / `FIRST-DISPATCH-EXPECTED` and is *"OWED at the lane
owner's three-platform landing dispatch"*, because *"every other value in this
function was executed before it was written, and this one was not."* The two
pairs are in different states today — `457` is measured (`:227-238`, release
run 33196576270) while the remainder's linux `1098` already stands DERIVED /
FIRST-DISPATCH-EXPECTED (`:263-266`) — so R1.1 must state provenance for each
of the four values separately, not as one block.

R1.3 names *"the Windows matrix leg carries this package's named
first-dispatch obligation"* — correct, but it is the **linux** count that
`_fault_count` mechanically enforces, and neither step names it.

### [P3-1] O3's close form is presented as a verbatim quote but is not one

R1.2: *"DISCHARGES O3 per its recorded close form (`"re-owned to the
relocation package, which discharges it directly on landing"`)"*. The recorded
text at `GwzM5-8R2E-E7-Acceptance.md:170` is *"**RE-OWNED to R2-F's
quarantine/relocation package, which discharges O3 directly on landing**"*.
Substantively identical; but a package whose entire §1 is about records that
say the wrong thing should quote exactly. The discharge itself is **correct**:
O3's text is *"Legacy writers stop mutating inside `.gwz/checked-artifacts`"*
(`GwzM5-8R2E-Plan.md:48`), and after the leaf change `open_private`
(`residue.rs:45-104`) only ever opens the new parent, so nothing mutates the
old path — leave-in-place included.

### [P3-2] The MAX_PATH statement does not say which git directory it measures, as the trace required

Trace §5.2 closes: *"Any MAX_PATH figure the plan adopts has to say which of
the two it measures"* — the legacy writer's per-worktree `repo.path()`
(`preservation_root/files.rs:29-31`) or the catalog's common directory
(`capability_permit.rs:79`, `retained.rs:200-206`). Plan §1.2 quotes the
201-char figure without saying. For a linked worktree the per-worktree dir is
`<common>/worktrees/<name>/`, longer again — so the recorded figure is a floor,
not the exposure.

### [P3-3] "Protected-tree digests" names the wrong dictionary for the file R1.1 actually touches

R1.1's companion list says *"protected-tree digests recomputed at land time"*.
The file R1.1 certainly touches is
`src/git/gitbackend/preservation_image.rs`, which is pinned in
`PROTECTED_SOURCE_DIGESTS` — the **per-file** dict
(`check_checked_artifact_boundaries.py:52`) — not
`PROTECTED_SOURCE_TREE_DIGESTS` (`:185-193`). The two files R1.1 edits first,
`catalog_names.rs` and `policy.rs`, are in neither dict, exactly as the trace's
§7.1 table says (I re-checked the dict at `:40-58`: 17 entries, neither
present). Name the dict, or the builder refreshes the wrong thing and
`check_lane_commits.sh` reds the commit.

### [P3-4] The single-owner correction has more than one home too

R1.2 corrects the single-owner claim *"at the R2-E plan §1.1"*. It also stands
at `GwzM5-8R2E-E7-Acceptance.md:170` (O3's row cites *"§7.6.2's single-owner
fact: `policy.rs:33-42` defines the legacy private parent, `observation.rs:93`
its only consumer"*) and in the addendum §7.6.2 it cites. E7-Acceptance is a
closed acceptance; say whether it takes a dated annotation or is superseded at
the resolution index.

### [P3-5] O1's unblock is right but incomplete as stated

R1.2 says the lift *"UNBLOCKS O1/O2 (their re-owned close's named carrier
arrives)"*. Correct against
`GwzM5-8R2E-Plan.md:100-113` and `E7-Acceptance.md:168-169`. Worth one clause:
`GwzM5-8R2E-Plan.md:94-95` records *"O1 cannot close DISCHARGED without O13"*,
whose substantive half rides E4.2/E4.3, and `E7-Acceptance.md:180` keeps the
dated accepted-residual record alive until those landings. Nothing is owed to
O13 *at* this lift — but the plan should say so rather than leave the reader to
check.

### [P3-6] §3's process line names the rituals without instantiating them

The heading *"(review loop at feature tier; two-round cap; the -B rule)"* is
consistent with house practice — `GwzM5-8R2E-Plan.md:160-166` (single-axis
peer-blind interior steps, automatic escalation on P0/P1/P2, two-round cap,
reports filed verbatim) and `GwzWtPlan.md:50-53` (the `-B` re-charter, no `-C`).
But the plan does not name the axes for R1.1's review, the escalation rule, or
the LOC basis (`GwzWtPlan.md:45-47`: handwritten incl. tests, generated
excluded) against which the HARD 250 is measured. R1.3's *"four CI legs"*
matches the house set (`E7-Acceptance.md:230-236`: Windows matrix, Platform
matrix, boundary, retained readers) and the Windows first-dispatch obligation
is correctly placed — those parts are fine.

### [P3-7] Two facts that refine §1.4 and R1.1, neither fatal

- **There is a third production spelling of the catalog family stem**, not two:
  `src/checked_artifact/catalog/scratch.rs:6`
  `const SCRATCH_PREFIX: &[u8] = b"checked-artifacts-catalog-bootstrap-v1.scratch.";`
  duplicates `catalog_names.rs:39` plus a trailing dot. It is **not** touched
  by this change (the `Final` leaf is unchanged), but §1.4's "a second
  production spelling" reads as an exhaustive count and is not one.
- **The `GitDirectory` arm never writes in production**, so R1.1's *"returns
  it for both roots"* is cosmetic on that half.
  `CheckedArtifactPolicy::git_directory` has exactly one production
  construction site (`entry.rs:182`), reached only from
  `observe_merge_preservation_git_directory` — so `<git-dir>/gwz/checked-artifacts`
  is never created by production code at all. Changing it is still right for
  symmetry; the plan should just not count it as a behaviour change.

Also worth recording as a closed question: trace §4.2 item 4 asked whether the
`.git/info/exclude` generator still needs its row. Under this design it does,
unchanged — `sync_workspace_boundary.rs:67` emits root-anchored `/.gwz/`,
which covers the old leaf and any new sibling automatically; nothing
reconciles the block against a literal (`replace_managed_block` `:94-118` keys
only on the markers), and `scripts/retained_readers/generate_retained_reader_fixtures.py:24-27`
pins `/.gwz/` only, so it does not go stale. One sentence in the plan closes it.

### [P3-8] The out-of-scope note conflates two different shortenings

§4's out-of-scope line reserves *"name-shortening for the true MAX_PATH
exposure (the honest remaining 173-char fact)"*. That is the 160-hex-character
triple in `authority.rs:238-240`, and reserving it is right. But it reads as
also reserving the *leaf* length, which R1.1 must choose anyway — and a shorter
leaf than `checked-artifacts` is a free reduction of the same exposure at zero
extra cost. Say whether the leaf choice takes it.

---

## Appendix — the companion set R1.1 actually moves (mandate item 2)

Enumerated against `scripts/checks/check_checked_artifact_boundaries.py` and
the trace's §7, for the change {new `CatalogPrivateNameV1` variant + `ALL`
entry; `policy.rs::private_parent` both arms; `preservation_image.rs:8`}.

| Companion | Moves? | Evidence | In the plan's list? |
| --- | --- | --- | --- |
| `PROTECTED_SOURCE_DIGESTS["git/gitbackend/preservation_image.rs"]` | **YES** | checker `:52`; the const at `preservation_image.rs:8` is edited | only as "protected-tree digests" — wrong dict ([P3-3]) |
| `PROTECTED_SOURCE_DIGESTS` — `catalog_names.rs`, `policy.rs`, `observation.rs` | **NO** | absent from `:40-58` (17 entries) | n/a |
| `PROTECTED_SOURCE_TREE_DIGESTS` | **NO** for R1.1 as briefed; **YES** if the `catalog.rs` reason string is folded | `:185-193`; `catalog.rs` root at `:189` | not named ([P2-3]) |
| `interface_tests/contracts.rs:167` and `:173` — `assert_eq!(…members().len(), 4)`, **twice** | **YES → 5** if it joins `ALL` | `members` is built 1:1 from `ALL` at `collision.rs:244-249`; these are the real `ALL.len()` pins | covered by "`ALL`-adjacent assertions", but unnamed ✔/~ |
| `interface_tests/catalog_recovery.rs:229-233` — members vs `ALL` | **NO** — both sides derive from `ALL`, so it stays green at any length | read at the tree | n/a |
| `interface_tests/catalog_recovery.rs:235-242` — the two `private_parent()` routing assertions | **YES** | both name `CatalogPrivateNameV1::Final` | same clause ✔ |
| Aggregate-gate pin `447/457` (`checked_artifact::`) | **YES**, from R1.1's *new* tests — **not** from the literal edits | `run_r4bg_aggregate_gates.py:300-301` | conditional, and the provenance duty omitted ([P2-5]) |
| Aggregate-gate pin `1097/1098` (lib remainder) | **YES** only if a `src/git/tests/**` test is **added**; editing bodies does not move it | `:302-305`; linux `1098` already DERIVED at `:263-266` | not distinguished ([P2-5]) |
| Fault-key census (165) | **NO** | trace §1.4; no key is per-root, and `fault.rs:66` exempts the legacy enum | correctly absent |
| Semantic vectors / golden corpus | **NO** | trace §7.6, re-checked: no `.gwz` or `checked-artifacts` in them | correctly out of scope |
| `APPROVED_RUST_PATH_EDGES`, `CATALOG_PUBLICATION_CALL_COUNTS`, `RAW_RENAME_CALL_ALLOWLIST`, `FORBIDDEN_PROVISIONAL_CATALOG_INTERFACES`, `CATALOG_LEASE_REFERENCE_SETS` | **NO**, unless R1.1 adds files or renames symbols | checker `:71-151`, `:773-826`, `:222-233`, `:757-765`, `:596-755` | n/a |
| In-module fixture helpers — `tests/recovery_protocol.rs:3-5`, `tests/leaf_publication.rs:14-16`, `tests/staging_recovery.rs:3-5`, `tests/removal_recovery.rs:3-5` | **YES**, one line each (the literals funnel through them) | swept | folded under "the test set" |
| Inline literals — `tests/recovery_protocol.rs:100`, `:139`, `:165`, `:166` | **YES** | swept | same |
| `src/git/tests/**` fixtures — `g12.rs:826`, `g15.rs:466/:482/:493`, `g15/root_preservation/stash.rs:306`, `max_path.rs:7/:33/:37` | **YES** | swept; `g12`/`g15`/`stash` are the [P1-1] red tests | same |
| Stale prose — `git/gitbackend/repository_support.rs:109`, `g15/root_preservation/support.rs:94-96` | **YES** (free — `repository_support.rs` is not digest-pinned) | absent from `PROTECTED_SOURCE_DIGESTS:40-58` | not named |
| `PROTECTED_COMPILER_MODULES` (`:26-35`, incl. `preservation_image.rs` at `:30`) | **NO** — only asserts the `#![forbid(...)]` line survives (`:1090-1096`), which is `preservation_image.rs:1` | swept | n/a |

**Scope credit:** the literal `checked-artifacts` appears **nowhere outside
`src/`** — not in `scripts/`, `dev-docs/`, `protocol/`, `.github/` or the
fixtures — so the plan is right that this is not a CI, protocol or corpus
event. Ignore coverage is by the `.gwz` prefix alone
(`src/workspace/mod.rs:8`, `src/diff/plan.rs:47`), so a new sibling leaf stays
git-ignored automatically. `catalog/enumeration.rs:373-388` (`fixed_roles`) is
a fixed 3-array, so the catalog tolerates the new sibling as `Ok(None)` rather
than refusing it, and no other file is forced to change: the only exhaustive
matches on `CatalogPrivateNameV1` are `leaf_bytes()` and `relative_path()`,
both inside `catalog_names.rs` itself.

**One fixture needs more than a repoint.**
`src/git/tests/g15/root_preservation/max_path.rs` is F-5's landed G-5
evidence. It plants the 173-char probe *inside the private area precisely
because that area is image-excluded*; after the relocation the old path is no
longer the private area, so repointing the literals is not enough — the
fixture must be re-derived against the new leaf's length or it stops modelling
the shape its own doc comment (`:1-13`) claims. R1.1's brief does not
distinguish it from the other five.

Commit shape is correctly handled: the plan's *"land in the SAME commit (the
duty-split ritual)"* is the right answer to trace decision 6, because
`.github/workflows/checked-artifact-boundary.yml:25` →
`scripts/checks/check_lane_commits.sh:25-43` runs the checker against each
commit's own tree (`git rev-list --reverse` → per-sha `git archive` → checker),
so a split series reds in the middle even when its head is green.

---

## Mandate dispositions

**1. TRACE FIDELITY — PASS on all four corrections' substance; I re-derived
each from the tree rather than trusting either document.**

- *Leaf-name-not-root-switch:* **TRUE.** `catalog_names.rs:6-9` has both root
  variants; `:21-26` has four leaf variants and no quarantine one;
  `policy.rs:33-42` returns `CatalogPrivateNameV1::Final` for **both** arms. No
  migration verb and no reader of a prior location exists (`residue.rs:45-104`
  reads only the current `quarantine_parent`).
- *MAX_PATH rider falsified by arithmetic:* **TRUE, re-computed from the
  tree's own constants.** `source_name` (`authority.rs:238-240`) =
  `"ca1-"`(4) + 64 + `"-"`(1) + 64 + `"-"`(1) + `hex(&[u8;16])`(32) +
  `".source"`(7) = **173**, independently pinned at
  `max_path.rs:20-29`. `authority_name` 143, `goal_name` 171, `scratch_name`
  152/147 all reproduce. Prefixes: `/.gwz/checked-artifacts/` = 1+4+1+17+1 =
  **23**; `/.git/gwz/checked-artifacts/` = 1+4+1+3+1+17+1 = **27**; suffixes
  **197** vs **201**. The move costs **+4**. 64+64+32 = **160** of the 173 are
  hex. The fixture comment's "root + 198" (`max_path.rs:33-34`) is off by one
  against `root.join(".gwz").join("checked-artifacts").join(name)` = root+197,
  harmlessly (the assertion is `> 260` against a 120-deep root).
- *Persisted-digest ordering ground:* **TRUE, full chain verified.**
  `CatalogPrivateNameV1::ALL` (`catalog_names.rs:29-34`) → `for_root`'s
  `members` (`collision.rs:239-251`) → `version_digest` (`collision.rs:261-271`)
  → `historical_digest` (`.../provider/digests.rs:136-145`) →
  `HistoricalCollisionDigestV1`, which is **both** CBOR key 4 of
  `CheckedCatalogBootstrapV1` (`checked_artifact/protocol/generated.rs:402`,
  `:418`, `:434`; struct field `catalog_bootstrap_record.rs:45`) **and** an
  input to the on-disk scratch directory name (`catalog/scratch.rs:24-45`, hex
  of the digest appended into the name bytes; constructed at
  `capability/pre_catalog.rs:331-335`). Free pre-activation, durable surgery
  after.
  *One adjacent fact, checked and clear:* the `.gwz` container's
  `namespace_entry_count` / `namespace_encoded_name_bytes` (moved by adding a
  sibling leaf) feed only the **fresh** observation digest
  (`snapshot.rs:159-167` → `digests.rs:69-73`), not the persisted historical
  one — so there is no second persisted-digest coupling to worry about.
- *Second spelling at `preservation_image.rs:8`:* **TRUE**, with both consumers
  as described (`:224-232`, `repository_support.rs:131-135`). The plan carries
  the fact and **drops the obligation attached to it** — [P1-1].

The plan's §2 claim that the A1 gate is *"doubly fail-closed"* also verifies:
`workspace_mutator_lock.rs:44-47` still `#[allow(dead_code)]`, and
`recover_or_create` (`catalog/bootstrap.rs:233`) still has no production caller.

**2. STEP ADEQUACY — FAIL.** R1.1 is not one goal: it carries three unmade
semantic decisions ([P1-2], [P1-3], [P2-1]) under a HARD 250 that must also
absorb the fixture recalculations the trace enumerates (§7.3's six files) and
the same-commit companions. The companion list is directionally right but
names the wrong digest dictionary ([P3-3]) and omits the in-code reason strings
([P2-3]). **The leaf must be named now** — it is an input to the fixture
arithmetic and the digest re-pin, and without it neither the builder nor the
reviewer can check the constraint. The stated constraint ("no longer than
`Final`'s footprint") is correct in direction but incomplete: ASCII-or-panic,
case-fold aliasing, and the scratch-family grammar are all binding ([P1-2]).

**3. GATE-LIFT CORRECTNESS — MIXED.** The literal claims check out: settled
tuple §11.3 item 1 (`GwzM5-8R2DSettledTuple.md:800-802`) is *"No production
catalog activation at `.gwz/checked-artifacts` until the relocation lands"*,
and R1.1's landing satisfies it as written; "unblocks O1/O2" is the right verb
against `GwzM5-8R2E-Plan.md:100-113`; O3's discharge is genuinely correct
([P3-1]). But the *substance* fails under §2's own recommendation — O2 cannot
execute in any already-used workspace ([P0-1]). Additionally owed at the lift:
the two in-code records ([P2-3]), the two missed correction homes ([P2-2]), the
O1/O13 clause ([P3-5]) and the second single-owner home ([P3-4]).

**4. OPEN-R1 — FAIL.** The evidence chain is **not** solid as cited: the two
grounds are the trace's costs restated as benefits, and the "refusal/skip"
hedge conceals that `exact_row` refuses rather than skips ([P0-1]). The
*framing* is right — one line, the operator's, recorded verbatim at the routed
homes (`GwzM5-8R2E-Plan.md:121-126`, `E7-Acceptance.md:170`, `:305`) — and
§2's blank-line ruling slot is the correct form. It is the evidence beneath the
recommendation that must be re-derived before the operator is asked to sign it.

**5. RISK HUNT — see [P0-1], [P1-1], [P1-2].** All three probes in the mandate
were run to a definite answer:

- **The observe-only GitDirectory read path is NOT affected.** `acquire`
  computes `policy.private_parent()` at `observation.rs:93` and stores it at
  `:117`, but `observe()` (`:123-135`) reads only `self.parent`,
  `parent_is_current` and `observe_leaf`. `quarantine_parent` has exactly one
  reader — `residue.rs::open_private` (`:49`, `:59`) — whose six callers
  (`residue.rs:111/:232/:334`, `transition.rs:203/:342`, `cleanup.rs:62`) are
  all on `replace_exact`/`remove_exact`/`classify_*`, never on `observe()`. So
  `<git-dir>/info/exclude` classification, its `PreservationEvidenceMismatch`
  refusals and its fixtures are untouched. The one live exposure is the ASCII
  `expect` at `catalog_names.rs:59`, which *is* evaluated on that production
  path ([P1-2] item 1).
- **The dirt-exemption and blindness-prefix change is the sharpest unnamed
  risk** ([P1-1]), and it is the one place the plan's instruction actively
  points the builder the wrong way.
- **No anchor-family name-grammar interaction exists.**
  `src/checked_artifact/platform/anchor.rs` mints `.ca1-durability-anchor-…`
  (`:142`), `.ca1-anchor-scratch-v1` (`:152`) and `.ca1-anchor-retired-…`
  (`:169`) as **child** names and reaches the private area by open descriptor
  (`anchor::prepare(dir, …)`, entered at `platform.rs:687-695`); nothing in it
  reads the parent's name. The leaf's real Windows constraints are the ones in
  [P1-2] plus the 255-unit/510-byte parent-entry budget
  (`catalog/enumeration.rs:8-12`, charged at `:242-255`) and MAX_PATH, where
  every extra leaf character is a one-for-one reduction in tolerable
  workspace-root depth. The **Windows anchor** matters to this package through
  directory *presence*, not naming — which is what makes [P0-1]
  platform-independent rather than Windows-only.

**Credit where due:** R1.1's *"the digest movement is asserted once,
deliberately"* is exactly right and is load-bearing —
`interface_tests/contracts.rs:174` only asserts
`assert_ne!(domain.version_digest(), [0; 32])`, so the reserved-name-set change
is otherwise **silent**, caught by no gate. The plan spotted a hole the trace
only implied.

**6. PROCESS — PASS with a gap.** Tier, two-round cap, `-B` rule, same-commit
duty-split, real-workspace batteries, four CI legs and the Windows
first-dispatch obligation are all consistent with house practice. The loop is
named but not instantiated ([P3-6]).

---

## What round 2 must contain

1. **§2 re-derived** ([P0-1]) — leave-in-place stated with its real
   consequence, and the "move the catalog's `Final` instead" variant weighed
   against it, before the operator's ruling is sought.
2. **The leaf named**, with its constraints ([P1-2]): ASCII, not a case-fold
   alias of a fixed role, outside the scratch-family stem, and ≤ `Final`'s 17
   characters.
3. **The dual-path exemption semantics decided and written into R1.1**
   ([P1-1]) — explicitly, so the builder does not take the cheap fixture
   repoint that turns the suite green while dropping the exemption. Say the
   same for `max_path.rs`, which needs re-deriving rather than repointing.
4. **R1.1 re-shaped to one decision**, splitting or re-budgeting as needed
   ([P1-3]).
5. **`ALL` membership stated with its trade** ([P2-1]).
6. **The correction train completed** — settled tuple `:792`, checkpoint
   `:3192-3194`, the dead `:1022-1024` anchor, the in-code reason strings, the
   second single-owner home ([P2-2], [P2-3], [P3-4]).
7. **Trace decisions 5 and 7 answered**, even if the answer is "moot at the
   lift" ([P2-4]).
8. **The companion set enumerated** from the Appendix table rather than
   gestured at — in particular the correct digest dictionary ([P3-3]) and the
   four-value provenance block for the aggregate-gate pins ([P2-5]).

Nothing here asks the package to change its charter. The decision it executes
(`GwzM5-8R2DPhase4Closure.md` §2.7) is sound, the trace under it is sound, and
the four corrections all verify at the tree. What round 2 owes is the
reasoning built on them.

---

# Round 2

Target: `GwzM5-8R2F-RelocationPlan.md` DRAFT round 2 (2026-09-01), fold record
§5. Re-verified at the same tree, `ea3a924`, working tree clean; read-only, no
build, no commit. Round 2 is final under the two-round cap.

## VERDICT: **NO-GO**

Round-1's P0 is genuinely cured: §2's inversion is withdrawn without hedging,
the design is re-derived from the tree facts, and the de-recognition mechanism
works exactly as claimed. **13 of 17 round-1 findings are CURED, 3 PARTIALLY,
1 NOT.**

But the new design has a defect of the same class as the one it replaces, and
it is load-bearing: **`CatalogPrivateNameV1::Final` is one name serving two
independent consumers, and the plan renames it rather than splitting it.** The
legacy writer follows the catalog to `catalog-final`, and the coexistence
collision the package exists to end is reproduced at the new name — deferred by
exactly one checked-artifact drive.

New findings: **1 × P0, 2 × P2, 3 × P3** (6). The remedy is small and named
(§"The remedy" below); what makes this NO-GO rather than conditional is that
the correction also voids §2, which is the operator-facing half.

---

## [R2-P0-1] `Final` serves the legacy writer *and* the catalog; renaming it moves both, reproducing the collision at `catalog-final`

§1 adopts: *"`CatalogPrivateNameV1::Final`'s on-disk name changes to a NEW
leaf"*, and R1.1 names it `catalog-final`. The plan never says what happens to
`policy.rs::private_parent` — and `private_parent` is **defined in terms of
`Final`**:

`src/checked_artifact/policy.rs:33-42`

```rust
    pub(super) fn private_parent(&self) -> PathBuf {
        match self {
            Self::WorkspaceArtifact { .. } => {
                CatalogPrivateNameV1::Final.relative_path(CatalogPrivateRootV1::Workspace)
            }
            Self::GitDirectoryArtifact { .. } => {
                CatalogPrivateNameV1::Final.relative_path(CatalogPrivateRootV1::GitDirectory)
            }
        }
    }
```

The complete non-test consumer set of `CatalogPrivateNameV1::Final` splits into
two families that share nothing but the name:

| Consumer | Family |
| --- | --- |
| `policy.rs:36`, `:39` | **the legacy leaf writer's private area** |
| `catalog/enumeration.rs:384`; `.../provider/{directory_mutation.rs:207,:321, completed.rs:77,:325, namespace.rs:67, interior.rs:178, aggregate.rs:90}`; `protocol/catalog_bootstrap_record.rs:75,:242`; `protocol/infrastructure_record/owner.rs:271` | **the catalog's Final directory** |

`relative_path` (`catalog_names.rs:53-61`) resolves both through the same
`leaf_bytes()`. So changing `Final`'s bytes to `catalog-final` moves the
catalog's Final directory **and** the legacy writer's private area to
`.gwz/catalog-final` — the identical directory. That shared identity **is** the
coexistence bug; the plan renames it instead of splitting it.

The consequence chain, all verified:

1. **First bootstrap succeeds** — `.gwz/catalog-final` is absent by
   construction, so `publish_final_directory`'s presence-refusal
   (`directory_mutation.rs:179-185`) does not fire. §1(a) holds, and R1.1's
   named test passes.
2. **The next checked-artifact drive re-creates the collision.**
   `residue.rs::open_private(create = true)` calls `prepare_parent(&self.private_root,
   &self.quarantine_parent, …)` (`residue.rs:46-52`) — it *creates* the
   directory `private_parent` names — then `ensure_authority` (`:232`) and
   `ensure_goal` (`:334`) write `ca1-<64>-<64>[-<32>].{authority,goal,source}`
   into it. On Windows `prepare_private` additionally plants the permanent
   `.ca1-durability-anchor-<32hex>` (`platform.rs:687-695`, permanence
   documented `:361-368`).
3. **The catalog then refuses.** Those children are neither
   `InfrastructureSlotV1::ALL` names nor `ActiveAction` rows, so `exact_row`
   returns `ambiguous("catalog interior", "catalog directory contains an unowned
   child")` (`interior.rs:437-440`), propagated per entry at `:87`, and
   `retain_completed_catalog` refuses at `completed.rs:64-74`.

So §1(c)'s claim — *"keeps §2.7's real grounds intact (the catalog interior
grammar stays closed; the legacy family's names never become catalog rows)"* —
is **false under the plan's own design**. The legacy family's names become
children of the catalog's Final directory on the first drive after bootstrap,
and on Windows the anchor makes it permanent.

This is precisely the work-list item the trace opened with, now dropped:
trace §4.2 item 1 — *"**[code] A private-area name that is not the catalog's
`Final`.** Today `policy.rs:36`/`:39` return
`CatalogPrivateNameV1::Final.relative_path(…)` — literally the catalog's own
leaf. `CatalogPrivateNameV1` has no quarantine/legacy variant."* Round 1's
shape had that split and got the *direction* wrong; round 2 fixed the direction
and lost the split. The package needs both.

**This is not a reading ambiguity.** §2 is written entirely around the
consequence of Reading-it-as-written: *"Every previously-used workspace
permanently retains an inert `.gwz/checked-artifacts/` directory … reclaimed by
nothing"*. The residue is orphaned **only** if the legacy writer moved away from
it. So the plan does mean what it says.

## The remedy (small, and it collapses several other findings)

Split the name instead of renaming it: **the catalog moves to `catalog-final`;
the legacy writer stays at `checked-artifacts`.** Concretely, add a second
`CatalogPrivateNameV1` variant whose leaf bytes remain `b"checked-artifacts"`,
and have `policy.rs:36`/`:39` return *that* while `Final` becomes
`catalog-final`.

One new name, two directories, and the whole package gets simpler:

- **The catalog bootstraps everywhere** — `.gwz/catalog-final` is absent by
  construction. Round-1's P0 stays cured.
- **`exact_row` never sees a `ca1-*` child** — the legacy writer never writes
  into the catalog's directory. §2.7's grounds genuinely hold.
- **Nothing is orphaned.** The residue stays where the writer still looks, so
  `inspect_family` keeps finding it and the trace's §6.2(a) unreclaimable-orphan
  cost never arises. **§2/OPEN-R1 as reframed becomes void** — there is no leak
  to accept or migrate.
- **[P1-1] and R1.2 largely dissolve.** `preservation_image.rs:8` still names
  the legacy private area correctly and needs no edit; `g12.rs`, `g15.rs`,
  `stash.rs` and `max_path.rs` stay green unchanged. The only open question
  becomes whether the catalog's *new* directory also wants a dirt exemption —
  a much smaller question than parallel coverage.
- **MAX_PATH:** the legacy 197-char exposure is unchanged (not worsened), and
  the catalog's own paths get the −4 delta.

Both names should be in `ALL`, which is what today's tree already gives them
(one name, in `ALL`) and what R1.1's own collision-domain argument supports.

## [R2-P2-1] R1.1's test set cannot catch [R2-P0-1]

R1.1's tests are *"bootstrap succeeds at the new leaf on BOTH roots with
resident old-name residue present"* and *"old-name directories are
unrecognized-inert"*. Both **pass** under the broken design — the collision only
appears on the first legacy drive *after* bootstrap. The missing row is the one
that decides the package: **bootstrap the catalog, then run one checked-artifact
drive, then re-observe the catalog and assert it is still recoverable.** Under
the plan as written that test fails; under the remedy it passes. It belongs in
R1.1 either way, because it is the executable statement of §2.7's criterion.

## [R2-P2-2] OPEN-R1's "accept the leak" silently also accepts a permanent, no-longer-justified dirt and preservation-image blind spot

§2 frames the residue as disk hygiene — *"Migration-now … buys only disk
hygiene."* But R1.2 mandates the git-status dirt exemption and
preservation-image blindness cover the old path **permanently and in parallel**.
Combined, that means `.gwz/checked-artifacts/**` stays exempt from
`status_dirty_outside_checked_artifact_private` (`repository_support.rs:115-135`)
and invisible to `raw_excluded_paths` (`preservation_image.rs:224-232`) forever,
for a directory **nothing writes any more**.

Today that exemption is justified by the comment at `repository_support.rs:109`
— *"permanent product infrastructure"*. After the relocation it is justified by
nothing, and anything placed under that path is invisible to gwz's dirt
classifier and absent from the preservation preimage. That is a real
consequence of "accept the leak" and the operator should be ruling on it
explicitly. Under the remedy it does not arise.

## [R2-P3-1] The `ALL`-membership trade omits a second persisted field

R1.1 states the trade as *"membership moves `version_digest()` →
`historical_collision_digest` — a persisted CBOR field and the
scratch-directory name"*. Renaming `Final` **also** moves `final_name`, which is
independently persisted: written at `protocol/catalog_bootstrap_record.rs:75`
(`catalog_component(CatalogPrivateNameV1::Final)`), validated at `:242`, and
carried as CBOR key 8 (`checked_artifact/protocol/generated.rs`,
`(8, Cbor::Bytes(self.final_name.clone()))`). Both are free strictly
pre-activation, so the *decision* is unaffected — but a trade statement whose
whole force is "free before first activation" should enumerate what it is
declaring free.

## [R2-P3-2] "observed by nothing" overstates the de-recognition

The mechanism is real and I verified it: `fixed_roles`
(`catalog/enumeration.rs:373-388`) is a fixed 3-entry table built from the
current `leaf_bytes()`, `classify_ascii` (`:283-320`) returns `Ok(None)` for
anything unmatched, and `observe_ascii` (`:177-200`) counts only recognised
roles — so a resident `checked-artifacts/` becomes unrecognised content that
refuses nothing. §1(b) is correct.

It is not, however, *"observed by nothing"*. Every child of `.gwz` is charged
against the parent budget first (`:170-175` → `charge` at `:242-255`:
`MAX_CATALOG_PARENT_ENTRIES_V1` 4096, `MAX_CATALOG_ENCODED_NAME_BYTES_V1` 510,
plus the aggregate), and those counters feed the **fresh observation digest**
via `namespace_entry_count` / `namespace_encoded_name_bytes`
(`.../provider/snapshot.rs:159-167`, supplied at `digests.rs:66-67`). The cost
is negligible — 17 bytes against a ~2 MB aggregate — and it does **not** touch
the persisted historical digest. Say "inert, budget-charged" rather than
"observed by nothing"; the difference matters if anyone later reasons about
whether residue can be ignored entirely.

## [R2-P3-3] Renaming only `Final` splits the catalog's name family

The other three `CatalogPrivateNameV1` leaves keep the `checked-artifacts-`
stem (`catalog_names.rs:39-41`), and `catalog/scratch.rs:6`'s
`SCRATCH_PREFIX = b"checked-artifacts-catalog-bootstrap-v1.scratch."`
independently hardcodes it. After R1.1 the container holds `catalog-final/`
beside `checked-artifacts-catalog-bootstrap-v1.{scratch.*,active,staging}`, so
the surviving stem names nothing. Functionally harmless — nothing derives one
from the other, and I confirmed `SCRATCH_PREFIX` is only compared against
itself (`scratch.rs:49`, `:101-102`) and against `classify_ascii`'s family trap
(`enumeration.rs:296-313`). Worth one sentence saying the split is deliberate,
or renaming the family together.

---

## Verification of the four items the coordinator asked for

**(a) De-recognition — CONFIRMED, with [R2-P3-2]'s qualifier.** `fixed_roles`
recognises by current-table lookup, so a resident `checked-artifacts/` is
genuinely inert to the catalog: unrecognised, uncounted as a role, refusing
nothing. Sweep for other recognisers of the old string: the only independent
(non-`Final`-derived) production spellings are
`git/gitbackend/preservation_image.rs:8` (the legacy private path — R1.2's
subject) and `catalog/scratch.rs:6` (the scratch stem, [R2-P3-3]); every other
site resolves through `CatalogPrivateNameV1::Final`. Nothing else recognises
`checked-artifacts` as the catalog's Final. The claim holds.

**(b) `catalog-final` against the four constraints — ALL FOUR HOLD.**
(i) *ASCII* — plain ASCII, so `relative_path`'s
`.expect("fixed catalog name is valid ASCII")` (`catalog_names.rs:59`), live on
the git-directory production path, cannot fire. (ii) *Length* — 13 vs 17, a
genuine −4 per path; the plan's negative-delta claim is arithmetically right.
(iii) *Case-fold distinct* — distinct from `checked-artifacts` and all three
bootstrap leaves under `eq_ignore_ascii_case`, so `classify_ascii`'s
`alias_error()` (`:291-293`) cannot fire. (iv) *No `ca1-` stem* — `"cat…"` is
not `"ca1-"`, so `inspect_family`'s `ca1-{family}-` filter (`residue.rs:127`)
and the scratch-family trap (`:296-313`) never see it. I also checked it against
`InfrastructureSlotV1`'s ten interior names (`protocol/slots.rs:76-85`, all
`catalog-*`/`action-*`/`retired-*`): no collision, and they live in a different
namespace (children of Final, not siblings of `.gwz`). The name is sound.

**(c) Bootstrap-with-residue-present IS the round-1 P0's scenario — CONFIRMED,
and the cure is real for the catalog half.** Round-1's P0 was
`publish_final_directory` refusing on the *presence* of `.gwz/checked-artifacts`
(`directory_mutation.rs:179-185`) in every workspace that ever ran a drive,
because `cleanup.rs::finish` never removes the directory. Publishing at an
absent `catalog-final` sidesteps it exactly. R1.1's named test is the right
test. It is simply not sufficient — see [R2-P2-1].

**(d) What the reframed OPEN-R1 still gets wrong.** Its factual description is
accurate (empty on Unix — `prepare_private` is `Ok(())` at `platform.rs:344-352`;
anchored on Windows at `:687-695`; never reclaimed). Three problems: the
orphan it asks the operator to accept is an artefact of [R2-P0-1] and
disappears under the remedy; it omits the permanent dirt/blindness blind spot
that "accept" also buys ([R2-P2-2]); and it presents the choice as leak-vs-
migrate when the real choice — which name the *legacy writer* gets — is not
put to the operator at all.

---

## Round-1 disposition table

| # | Round-1 finding | Disposition |
| --- | --- | --- |
| P0-1 | OPEN-R1 grounds inverted; gate-lift false | **CURED** — §2 withdrawn in full, design re-derived, bootstrap-with-residue named as a test. (Superseded by [R2-P0-1], a new defect of the new design.) |
| P1-1 | Second-spelling instruction contradicts the trace | **CURED** — R1.2 mandates both-path coverage, repointing forbidden, the three named fixtures extended. (Largely dissolves under the remedy.) |
| P1-2 | Leaf must be named | **CURED** — `catalog-final`, all four constraints verified above. |
| P1-3 | More than one semantic decision in R1.1 | **PARTIALLY** — the R1.1/R1.2 split is real, but R1.1 still bundles the leaf rename, `ALL` membership, the digest comment **and** the A1 zero-caller tripwire, which is a distinct feature with its own test, not "the leaf and its coupling". |
| P2-1 | `ALL` membership decided only by implication | **CURED** — decided IN with the trade stated (incomplete; [R2-P3-1]). |
| P2-2 | MAX_PATH correction train incomplete | **CURED** — all five homes plus the drifted `:1022-1024` anchor. |
| P2-3 | In-code records cannot ride a docs-only step | **CURED** — they ride R1.1. |
| P2-4 | Trace decisions 5 and 7 unanswered | **CURED** — 5 YES (tripwire), 7 YES (evidence-map §2.3 re-owned). |
| P2-5 | Pin move stated as "if"; provenance duty omitted | **CURED** — near-certain, with the driver's dated per-OS provenance block. |
| P3-1 | O3's close form not verbatim | **CURED** — verbatim at the record site. |
| P3-2 | Which git directory is measured | **PARTIALLY** — commondir is right for the catalog, but the *legacy* writer's git-dir arm uses the per-worktree `repo.path()` (`preservation_root/files.rs:29-31`), which the plan still does not name; under the plan's own design that path moves too. |
| P3-3 | Wrong digest dictionary named | **CURED** — enumerated from the checker at build time, which is better than guessing. |
| P3-4 | Single-owner correction has more than one home | **PARTIALLY** — the plan adds *"the trace's §2 list"*, but the trace is a read-only dated note, not a record needing correction. The homes I named were `GwzM5-8R2E-E7-Acceptance.md:170` (O3's row, a closed acceptance) and the addendum §7.6.2. Neither is listed. |
| P3-5 | O1's unblock incomplete | **CURED** — `:280`-via-O13 carried. |
| P3-6 | Rituals named, not instantiated | **CURED** — per-commit lane gate with the squash arm, worktree, verbatim reports, batteries, `-B` rule, Windows first-dispatch. |
| P3-7 | Third spelling; GitDirectory arm never writes | **NOT** — §5 maps [P3-7] to "bootstrap-with-residue-present; the tripwire's dated E4.1 note", which is not what the finding said. Neither `catalog/scratch.rs:6` nor the never-writing git-dir arm appears. Re-raised in part as [R2-P3-3]. |
| P3-8 | Two shortenings conflated | **CURED** — §1's MAX_PATH paragraph separates them. |

**Totals: 13 CURED, 3 PARTIALLY, 1 NOT.**

---

## What the operator is being handed

Under the `-B` rule this returns to the operator rather than a third round. The
disposition is narrow, so it should be quick:

1. **Adopt the split** — catalog to `catalog-final`, legacy writer pinned to a
   variant whose leaf stays `checked-artifacts` ([R2-P0-1], §"The remedy").
   Everything else in round 2 survives unchanged.
2. **Void §2/OPEN-R1** — under the split there is no orphan and no leak, so
   the reframed question dissolves. If the operator prefers to keep a residue
   question alive, the honest one is [R2-P2-2]'s: whether the *catalog's* new
   directory wants a dirt exemption.
3. **Add the drive-after-bootstrap test** to R1.1 ([R2-P2-1]) — it is the
   executable form of §2.7's criterion and the row that distinguishes the two
   designs.
4. Fold [R2-P3-1..3] and the three PARTIALLY rows as wording.

The charter, the 2026-08-23 decision, the trace, the four carried corrections
and round 2's whole records train remain sound. What is wrong is one name doing
two jobs.

---

# Re-charter round 1

Target: `GwzM5-8R2F-RelocationPlan.md` re-chartered under the `-B` rule
(2026-09-01), charter trail §5. Fresh two-round cap. Re-verified at the same
tree, `ea3a924`, working tree clean; read-only, no build, no commit.

## VERDICT: **GO-WITH-CONDITIONS**

The design is right. §1 is the round-2 remedy, the two dead designs are named
accurately, the decisive test is correctly placed and correctly phrased, and
the process paragraph is fully instantiated. Nothing here is a P0: the package
as chartered would work.

Two conditions, one substantive and one budgetary:

1. **The `ALL`-placement decision rests on a mechanism that does not exist**
   ([RC-P1-1]). Outside-`ALL` has nothing to do with de-recognition, and its
   real cost — the live legacy area loses the git-index collision protection it
   has today — is unstated and contradicts §1's own "zero behavioral change for
   legacy".
2. **R1.1 will breach its HARD 250** ([RC-P2-1]), driven by a digest-consumer
   enumeration that is scope-inflated well past what the finding it cites asked
   for.

Counts: **0 × P0, 1 × P1, 1 × P2, 4 × P3** (6 findings).

---

## [RC-P1-1] Outside-`ALL` does not cause de-recognition, and the placement's real trade is unstated

§1's name-set bullet reads:

> the legacy variant sits OUTSIDE `ALL` (pure path composition — **so** the old
> string leaves the recognition table and old directories are RECOGNIZED BY
> NOTHING…)

The causal "so" is false. `ALL` and the recognition table are independent
mechanisms, and I verified both:

- **The recognition table is `fixed_roles`**
  (`src/checked_artifact/catalog/enumeration.rs:373-388`) — a **fixed
  three-entry array** naming `BootstrapActive`, `BootstrapStaging` and `Final`
  **explicitly by variant**. It does not iterate `ALL`. So the old string leaves
  the table because **`Final.leaf_bytes()` changed**, full stop. Placing the
  legacy variant inside `ALL` would **not** put `checked-artifacts` back into
  `fixed_roles`, and would not re-recognize anything.
- **`CatalogPrivateNameV1::ALL` has exactly two consumers in the whole tree**:
  `capability/collision.rs:244` (`PrivateControlDomain::for_root`'s `members`)
  and `interface_tests/catalog_recovery.rs:229` (a test pin). That is all
  `ALL` controls — the collision domain and its digest.

So the placement decision is real, but it buys and costs something entirely
different from what §1 says:

**What outside-`ALL` actually costs.** Today `Final` = `.gwz/checked-artifacts`
is a member, so `reject_private_collisions`
(`.../provider/snapshot.rs:39-72`) refuses any git index path overlapping the
legacy private area — `ambiguous("private namespace collision", "Git index path
… overlaps reserved path …")`. After the split with the legacy variant outside
`ALL`, `.gwz/checked-artifacts` is no longer a member and that refusal is gone.
The legacy area stays **live** until E4.7 retires the legacy writers, so this is
a protection loss on a directory still being written — and it directly
contradicts §1's *"zero behavioral change for legacy"*. (It is invisible today
only because the catalog observation path has zero production callers; it
becomes visible at E4, which is exactly what this package unblocks.)

**What outside-`ALL` buys.** A genuine argument exists and the plan does not
make it: `src/git/tests/g15.rs:461-501` commits `".gwz/checked-artifacts/protocol"`
as a *tracked* file — the control that makes the exclusion test meaningful. If
`checked-artifacts` stayed in the collision domain, any workspace tracking a
file under the legacy path would be refused at catalog observation once E4
activates. Outside-`ALL` avoids that.

**Disposition.** Either answer is defensible; neither has been argued. Note
what the choice actually turns on mechanically, since it also settles a
companion:

| | inside `ALL` | outside `ALL` |
| --- | --- | --- |
| Legacy area keeps today's index-collision refusal | **yes** | no |
| Tracked files under the legacy path refused at E4 | yes | **no** |
| `ALL.len()` | 5 → `contracts.rs:167`/`:173` move 4→5 (two-line companion) | 4 → **pins stay green** |
| De-recognition of `checked-artifacts` | unaffected | unaffected |

If "zero behavioral change for legacy" is the goal §1 states, **inside-`ALL` is
what delivers it**, and it costs two pinned integers. If the g15-class
tracked-file case is judged to outweigh that, say so — but say it, rather than
resting the decision on a recognition-table effect that does not occur. This is
the same defect class that killed rounds 1 and 2: a load-bearing conclusion
resting on a mechanism the tree does not implement.

## [RC-P2-1] R1.1 will breach its HARD 250, and the largest driver is scope inflation on [R2-P3-1]

R1.1 must land, in one commit: the new variant and `leaf_bytes` arm; `policy.rs`;
`Final`'s bytes; the two-path exemption surfaces; in-code dated comments
(ordering ground, scratch-stem note, GitDirectory symmetry note); `max_path.rs`
re-measured with both git-directory arms named; three fixture families
(`g12`/`g15`/`stash`) extended **both ways**; four new test rows (the decisive
drive-after-bootstrap row, bootstrap-with-legacy-directory-present on both
roots, recognized-by-nothing, the digest assertions); the aggregate-gate
provenance block; and the checker digest refreshes.

The surfaces are small where I could size them — `CHECKED_ARTIFACT_PRIVATE_PATH`
has exactly two consumers (`preservation_image.rs:225` via `std::iter::once`,
`repository_support.rs:133` via `strip_prefix`), so widening it to two paths is
~15 lines. The fixture extensions and the decisive test are the honest bulk,
~100-130 lines together.

**The problem is the digest clause.** §1 commits R1.1 to *"ENUMERATE EVERY
consumer of that digest from the tree … and assert each movement once"*, citing
[R2-P3-1]. There are **57 non-test references** to
`historical_collision_digest` / `HistoricalCollisionDigestV1` in `src/`.
Enumerating and asserting per-consumer across that surface is a large
test-writing job on its own, and it is not what the finding asked for.
[R2-P3-1] said, verbatim: *"a trade statement whose whole force is 'free before
first activation' should enumerate what it is declaring free"* — a **statement**
naming the two persisted fields (`historical_collision_digest`, CBOR key 4; and
`final_name`, CBOR key 8, `protocol/catalog_bootstrap_record.rs:75`/`:242`) plus
the scratch-directory name. Round 2 already had the right test shape: *"the
digest movement is asserted once, deliberately."*

My estimate for R1.1 as chartered is ~280-370 LOC; with the digest clause cut
back to the statement plus the single deliberate assertion, ~220-250 — inside
the cap. The cap is a stop-and-report boundary (`GwzWtPlan.md:45-47`), and the
`-B` re-charter has already been spent on this package, so a step that dies at
the cap now goes to the operator. **Condition: restate the digest clause as
[R2-P3-1] framed it**, or split the enumeration into its own step as the
tripwire was split.

## [RC-P3-1] The decisive test is right; two cheap assertions would make it decisive on both platforms

Placement (R1.1) and phrasing are correct, and I verified the claim *"fails
under either dead design, passes under this one"*: under round 1's design
bootstrap itself fails on the presence-refusal
(`directory_mutation.rs:179-185`); under round 2's the drive writes `ca1-*` into
`catalog-final` and re-observation refuses at `interior.rs:437-440`. Good.

Two additions, both a line or two:

- **Assert directory disjointness directly** — that the drive's `ca1-*` files
  land under `.gwz/checked-artifacts` and that `.gwz/catalog-final`'s interior
  is unchanged. "Still recoverable" is the consequence; disjointness is the
  property, and it fails louder and earlier.
- **Assert where the Windows anchor lands.** `prepare_private` has exactly
  **one** production caller — `residue.rs:102`, inside the legacy writer's
  `open_private` — so the catalog never plants an anchor and
  `.gwz/catalog-final` can never acquire a `.ca1-durability-anchor-*`. That is a
  real structural guarantee of this design and it is what makes the Windows arm
  safe; it belongs in the named first-dispatch obligation as an assertion, not
  left implicit.

## [RC-P3-2] The ASCII-or-panic constraint has three enforcement sites, not one

§1 justifies `catalog-final` as *"ASCII on the production path"*, citing the
single site from round 1. There are three, and all three are `expect`:
`catalog_names.rs:59` (`relative_path`, live on the git-directory preflight),
`.../provider/directory_mutation.rs:721` and `.../provider/completed.rs:561`
(both `private_name`: `from_utf8(name.leaf_bytes()).expect("fixed catalog names
are ASCII")`). `catalog-final` satisfies all three, so nothing is wrong — but
the in-code dated comment should name the constraint where it is enforced, so a
future leaf change meets it deliberately.

## [RC-P3-3] The catalog's new path needs its own stated exemption ground; the existing comment's ground does not transfer

R1.1 adds `.gwz/catalog-final` to the dirt-exemption and preservation-blindness
surfaces *"(both now justified: one live-legacy, one product-infrastructure)"*.
The legacy half's justification is written at `repository_support.rs:107-112`
and is anchor-specific: *"The private area (`.gwz/checked-artifacts`) is
permanent product infrastructure — **on Windows it retains a durability anchor
for the life of the repository**"*. Per [RC-P3-1], the catalog's directory never
gets an anchor, so that ground does not transfer. The catalog's exemption is
justified on different footing (catalog state is product infrastructure the user
never authored), and the doc comment must be rewritten to carry two grounds
rather than copied. Small, but it is prose that a reviewer will read as the
contract.

## [RC-P3-4] Two companion consequences of the placement decision should be stated where the decision is made

- `contracts.rs:167` and `:173` both assert `members().len() == 4`. Under
  outside-`ALL` they **stay green** and are not companions; under inside-`ALL`
  they move to 5 and are. My round-1 appendix listed them as companions on the
  assumption the new name would join `ALL` — that entry is **void** as the plan
  now stands, and correspondingly live if [RC-P1-1] is re-decided. Whichever
  way it goes, say so in R1.1 so the builder does not hunt for a pin that
  should not move.
- `.../provider/interior.rs:170-186` (`directory_fact`) matches on
  `CatalogPrivateNameV1` and already carries a `_ => CatalogDirectoryFactV1::Other`
  arm, so the new variant compiles without a new arm and silently classifies as
  `Other`. That is correct here — the legacy variant is never passed to
  `directory_fact` — but it is worth one line in the in-code comment, since the
  catch-all is what makes a name-set addition safe and a future reader should
  know it was checked rather than missed.

---

## The four mandate items

**(1) Remedy fidelity — SUBSTANTIALLY YES, with one unargued deviation.** §1 is
the round-2 remedy: `Final` → `catalog-final` (in `ALL`), a new variant whose
leaf stays `checked-artifacts` pinned to `policy.rs::private_parent`, nothing
moved, nothing orphaned, exemptions and the g12/g15/stash/max_path fixtures
correct as-is. The consequences §1 lists are the ones I verified in round 2 and
they hold. The deviation is the legacy variant's **outside-`ALL`** placement: my
remedy said both names should be in `ALL` (that is what today's single shared
name gives them). Deviating is legitimate; the ground given for it is not
([RC-P1-1]). **Nothing requires the legacy name inside `ALL`** — `private_parent`
needs only `relative_path`, which is pure composition off `leaf_bytes()`
(`catalog_names.rs:53-61`) and touches `ALL` not at all — so outside-`ALL` is
*mechanically viable*. It is the *rationale* and the *unstated protection loss*
that are wrong, not the feasibility.

**(2) The decisive test — correctly placed and correctly phrased.** R1.1 is the
right home (it is the executable statement of the split), and I verified it
fails under both dead designs and passes under this one. Two cheap
strengthenings in [RC-P3-1].

**(3) The 250 cap — NO, it will breach.** ~280-370 as chartered; ~220-250 with
the digest clause restated to what [R2-P3-1] actually asked. See [RC-P2-1].
Splitting the tripwire into R1.2 (cap 60) was the right call and is not the
problem.

**(4) New hazards the split creates — one real, the rest clean.** The real one
is the collision-protection loss ([RC-P1-1]) — a hazard neither dead design had,
because both kept a single name in `ALL` covering whatever the legacy writer
used. On the others I found no hazard: the **two-name dirt-exemption surface**
is mechanically small (two consumers) and semantically sound, needing only the
prose fix in [RC-P3-3]; the **anchor grammar** is safe and in fact
*strengthened* — `prepare_private`'s single caller (`residue.rs:102`) means the
catalog's directory structurally cannot acquire an anchor, so `exact_row` can
never meet one there; the **scratch grammar** does not assume `Final`'s old
bytes — `SCRATCH_PREFIX` (`catalog/scratch.rs:6`) is compared only against
itself (`scratch.rs:49`, `:101-102`) and against `classify_ascii`'s family trap
(`enumeration.rs:296-313`), never against `Final`, so leaving the stem is safe
as §1 says; and adding an enum variant breaks no exhaustive match
([RC-P3-4]).

---

## Conditions for GO

1. **Re-decide the `ALL` placement on the real trade** ([RC-P1-1]) — using the
   table above — and replace §1's recognition-table rationale with the actual
   one. If "zero behavioral change for legacy" stands as written, the answer is
   inside-`ALL` and `contracts.rs`'s two pins become companions.
2. **Restate the digest clause** as [R2-P3-1] framed it — a complete trade
   statement naming both persisted fields and the scratch name, plus the single
   deliberate assertion — or split it into its own step ([RC-P2-1]).
3. Fold [RC-P3-1..4] as test rows and wording.

No re-review is owed on these; they are checkable at the R1.1 step review
against this list. The charter, §1's design, §2's OPEN-R1 resolution, §3's
process and §4's exit criteria are sound as written.
