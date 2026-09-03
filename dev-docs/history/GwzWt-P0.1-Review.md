# GwzWt — P0.1 Plan Review

- **Object under review:** `gwz-dev/dev-docs/GwzWtPlan.md` (status DRAFT,
  "GwzWt Implementation Plan").
- **Governing authority:** `gwz-dev/dev-docs/GwzWtDesign.md` (**ADOPTED
  2026-09-01**) and its two-round review `gwz-dev/dev-docs/GwzWtDesign-Review.md`.
  Where plan and design disagree, the design governs; any such disagreement is a
  finding.
- **Date:** 2026-09-01.
- **Tier / mode:** single-axis, peer-blind, Opus. No second axis. GO gates
  Phase 1.
- **House context read:** `gwz-cli/dev-docs/GwzLogPlan.md` (§1, §2, §3, §4, §6
  incl. every -B re-charter and the S1.1-B fallback execution),
  `gwz-cli/dev-docs/GwzLog-S0.1-Review.md` (finding classes F7 misassigned
  surface, F9 invented dependency, F10/F23 bundling, F11 process gaps, F13 LOC
  basis, F14 dangling citation, F22 optimistic sizing, F25 the carve-out),
  `gwz-cli/dev-docs/GwzLogRequirements.md` L-ENV-10/L-ENV-12,
  `gwz-cli/dev-docs/GwzLogTraceability.md`.
- **Tree read (read-only), gwz-core at HEAD:**
  `src/workspace_ops/sync_workspace_boundary.rs` (whole file),
  `src/workspace_ops/handle_materialize.rs` (materialize, capture, branch,
  clone and apply paths), `src/git/gitbackend/contract.rs` (AD1),
  `src/operation/` (child inventory), `scripts/checks/run_r4bg_aggregate_gates.py`,
  plus crate-wide greps for `workspace_exclude_path`,
  `ensure_workspace_exclude`, `sync_workspace_boundary`, `.join(".git")`, and
  the AD1 shell-out sites. `gwz-cli/docs/CLI.md` for the shipped verb set.
- **Measurements taken:** landed `gwz log` surface sizes; `run_r4bg_aggregate_gates.py`
  pin values and docstring provenance; `workspace_exclude_path` call-site fan-out.
- **Nothing was built, mutated, committed, pushed or tagged.** No probe fixtures
  were needed — every claim below is answered by the tree or by the cited
  documents. The only file this review creates is itself.

---

## Verdict

# GO WITH CONDITIONS

The skeleton is sound and better disciplined than its sibling was at the same
gate. §5's row set matches the design's exactly — 26 rows, no dangling citation,
no unowned row, no double-owned row, which is the thing gwz-log's S0.1 had to
repair. The phase order is genuinely foundational-first. §1.3 names the right
risk concentrate for the right reason. §1.4's LOC basis and §1.5's pre-stated -B
rule are the two gwz-log lessons that cost the most to learn, banked in advance.
§7's arithmetic is correct to the line.

Four P1s stand between this and a clean GO, none requiring a re-plan:

1. **The add executor's exclude posture is unowned.** S2.2 inherits *half* of the
   reused seam's forbidden behaviors — it excludes the rollback and is silent on
   the boundary sync, which the seam performs unconditionally on the root it
   creates. **[P1 F1]**
2. **The design still instructs the withdrawn mechanism.** §5.5 step 3 survived
   round 2's F19 withdrawal uncorrected, and the design governs. **[P1 F2]**
3. **§2 asserts an operator adoption that exists nowhere in the record**, and
   silently drops the parking rule it replaces. **[P1 F3]**
4. **S4.1's verb list under-proves W-CMD-6** — it omits four of the verbs
   W-CMP-1 itself names as reaching the function S1.1 changes. **[P1 F4]**

**Finding counts:** P0 × 0, P1 × 4, P2 × 7, P3 × 7 (18 total).

---

## Findings

### [P1 F1] S2.2 inherits half of the reused seam's forbidden behaviors: the add executor's boundary-sync path is unowned, and S2.3's skip rule is not a declared dependency

The design's §5.1 reuses "the jobs-bounded executor, and per-member response
construction". In the tree that machinery is `apply_materialize_plans`
(`gwz-core/src/workspace_ops/handle_materialize.rs`), and it carries **two**
behaviors the design forbids for `worktree add`:

- the reject-partial rollback at `:370-378` (`std::fs::remove_dir_all` over
  `fresh_clone_paths`), which W-LIF-3 explicitly rejects; and
- an unconditional trailing boundary sync at `:392`:

```rust
    // Refresh the workspace boundary (member + tmp excludes) from the authoritative
    // on-disk lock (rewritten above, or the existing one for a lock target).
    let lock = artifact::read_lock(root)?;
    sync_workspace_boundary(backend, root, manifest, &lock)?;
```

This answers the review's chartered probe directly: **yes, gwz's own
bootstrap/materialize path runs `ensure_workspace_exclude` on the ROOT it
creates.** `clone_workspace_with_emitter` (`:495-560`) clones the root, then
drives `apply_materialize_plans` against it, so the trailing sync fires on the
freshly created root. `worktree add`'s root creation is the same shape by §5.5's
own words ("create the ROOT worktree first — its own path, as clone does").

S2.2's brief names the first exclusion — "NO rollback — partials stay legal" —
and is **silent on the second**. The consequences are ordered by which steps have
landed:

- **Before S1.1:** `workspace_exclude_path(composite_root)` traverses a gitfile
  → `ENOTDIR`, absorbed nowhere (`sync_workspace_boundary.rs:75-79` special-cases
  `NotFound` only). `add` hard-errors after creating worktrees. S2.2's declared
  dependency on S1.1 is therefore real and necessary — that edge is correct.
- **After S1.1, before S2.3:** resolution succeeds and points at the **primary's
  common-directory `info/exclude`**. The add executor then rewrites the primary's
  shared managed block from the *composite's* manifest and lock. That is a direct
  **W-SAF-2 violation** ("never the shared `info/exclude`"), and it perturbs the
  merge-evidence hash — the exact quantity S1.1 is chartered to pin unchanged
  (`merge/publication.rs:40`, `merge/finalize.rs:283`,
  `merge/abort/evidence.rs:767`).

§4 states "No step depends on anything it does not consume" and invites the F9
hunt. The inverse defect is the live one here: a consumed dependency that is not
declared.

**Required.** S2.2's brief must state, in the same sentence style as the rollback
exclusion, that **the add executor performs no exclude write and no boundary sync
on any constituent, including the composite root** — and then either (a) declare
S2.3 as an S2.2 dependency, or (b) state that S2.2 writes its own executor in
`operation/worktree/` that never reaches `sync_workspace_boundary`, with S2.2's
review verifying it by call-graph. Option (b) is compatible with keeping
`par_map_per_host` (which lives at `src/operation/par_map_per_host.rs`, outside
`workspace_ops/`, and is reusable without touching the materialize handler).

### [P1 F2] The design's §5.5 step 3 still instructs the withdrawn per-constituent exclude mechanism, and the plan neither inherits nor flags the contradiction

`GwzWtDesign.md` §5.5 step 3, verbatim:

> Write the intent record; create the ROOT worktree first (its own path, as clone
> does); then members under the jobs bound, per-repo results reported; **configure
> each constituent's worktree-scoped exclude per W-CMP-2.**

W-CMP-2, as re-decided at round 2 (F19), says the opposite: "composites get **no
exclude mechanism at all**." The round-2 fold cured W-SAF-2's write set and OQ-6
and mooted F20, but §5.5 step 3 was not touched — a live residue in an ADOPTED
document.

The plan is silently *more correct* than its authority here: S2.2's brief omits
the clause entirely. But §1 states "where this plan and the design disagree, the
design governs," and §2 sends step reviewers to the design's rows. An S2.2
builder reading §5.5 in good faith builds the mechanism round 2 withdrew — the
one whose probe showed it suppresses the user's global `core.excludesFile`, an
accident-class regression inside the design's own envelope.

**Required.** Either the lane owner corrects §5.5 step 3 at source with a dated
note (the gwz-log F26 precedent — the stale line that caused a P0 was corrected
at source, same day, as a standalone docs-only commit), or the plan records the
supersession by name in §1 and in S2.2's brief. A silent divergence from a
governing document is not a resolution. Closing this together with **[F1]** is
one edit.

### [P1 F3] §2's proportionality carve-out is asserted as operator-adopted with no record anywhere in the estate, and the parking rule it replaces has been silently dropped

§2, verbatim: "the **proportionality carve-out is STANDING** (adopted by the
operator at the gwz-log handoff)".

A grep across `gwz-dev/dev-docs/`, `gwz-cli/dev-docs/` and `gwz-core/dev-docs/`
returns exactly six hits for "proportionality". Five are the *proposal*:
`GwzLog-S0.1-Review.md:1022` (the finding), `:1045` (the proposed wording),
`:1124`; `GwzLogPlan.md:409` — which still reads "**operator's ruling
requested**… Proposed second carve-out, **NOT self-adopted**… the three round-2
P2 escalations stand **PARKED** as §2 requires" — and `GwzLogPlan.md:473`
recording it as *proposed*. The sixth hit is `GwzWtPlan.md:59` itself, the only
place in the estate that calls it adopted. Neither gwz-log's §6 adoption trail
nor its §5 open items was ever updated, and both record verbatim operator
directives elsewhere (the mirror directive, the module-home ruling) — so the
estate does record operator rulings when they happen.

gwz-log S0.1 F25's first point predicted this precise failure: "Program rules
amended in feature documents become precedent by accretion. It should be ratified
where the rule lives… rather than **inherited silently by the next feature plan
that copies this §2**."

**Second limb — a weakening that is not flagged as a change.** gwz-log §2 reads:
"while the Fable pool is exhausted, an escalation **PARKS** the step until the
quota reset rather than waiving the second axis… Parked escalations are recorded
in the step's review file; **no step lands with an unserved escalation**." GwzWt
§2 reduces this to "auto-escalate to Fable on P0/P1/P2 **when the pool allows**",
and drops both following sentences. "When the pool allows" reads as *proceed
without the second axis when it does not* — converting a park into a waive. The
verified-fact carve-out, by contrast, is restated accurately.

**Required.** Cite the operator record for the proportionality carve-out, or
restate it as proposed-and-pending (and, per F25's own remedy, ratify it where
the rule lives). Reinstate the parking sentence and the "no step lands with an
unserved escalation" clause, or state the change as a deliberate amendment with
its authority.

### [P1 F4] S4.1's verb list under-proves W-CMD-6: four verbs that W-CMP-1 itself names as reaching the changed function are absent, and merge's omission hides a live hazard

W-CMD-6 is "every existing verb". S4.1 owns it and enumerates the proof:
`status / diff / log / commit / branch / stash / capture / snapshot / push+pull`.

W-CMP-1 enumerates its own blast radius: "reached from
**commit/branch/capture/pull/stage/repo/materialize** and the **merge evidence
chain**". Cross-referencing against the tree, the verbs that reach the function
S1.1 rewrites and are **absent from S4.1's list**:

| Verb | Call sites reaching the changed surface |
| --- | --- |
| `gwz add` (stage) | `handle_stage.rs:84` |
| `gwz repo` | `handle_repo_lifecycle.rs:153,229,342`; `handle_create_repo.rs:51,211,336,426` |
| `gwz materialize` | `handle_materialize.rs:116,392,444` |
| `gwz merge` | `merge/finalize.rs:283`; `merge/publication.rs:40`; `merge/abort/evidence.rs:335,767`; `merge/v1_lifecycle/.../publication/live.rs:109`; `merge/preserve/artifacts.rs:93` |
| `gwz init` *(inside a composite; init is used only as the fixture builder)* | `handle_init_from_sources.rs:203` |

These are not incidental verbs — they are the ones whose behavior S1.1's refactor
can change. A battery that omits them and then declares W-CMD-6 proven is the
"untestable MUST" class the design review named at round 1.

**Merge is the sharpest omission, and it is not a clean scope call.**
`merge/finalize.rs:283` computes `file_sha256(&workspace_exclude_path(root))` and
compares it against a recorded candidate. Post-S1.1, inside a composite that
resolves to the **primary's** shared file — which the primary may rewrite at any
moment through its own ordinary boundary sync. So a merge running in a composite
carries a cross-materialization evidence-invalidation window that neither
document names. W-SAF-2 is satisfied (the composite never *writes*), but W-CMD-6
is not: the verb does not behave as it does in a primary. S4.1's brief does
assert the catalog-lease sharing (W-SAF-4) is demonstrated — that is the *lock*
layer, and it does not cover this.

**Required.** Derive S4.1's verb list from W-CMP-1's own enumeration rather than
independently: add `stage`, `repo`, `materialize`. For merge, choose explicitly —
either add merge-in-composite with an evidence-stability assertion, or record it
as a **named accepted residual** with the hazard stated (the honest v0 option,
and the one W-CMD-6's "scope, not an audit" framing can carry). Silence is the
one option the design forecloses.

### [P2 F5] S2.2's HARD 500 is a bundle of the gwz-log F10 class, with two unstated buildability constraints inside the cap

S2.2's brief carries ten deliverables: root-first creation (a bespoke path
outside the member seam); members under the jobs bound; CLI-delegated
`git worktree add` per constituent; git2 self-verification; the intent-record
write; per-repo tolerant results; a **novel execution model** (no rollback,
partials legal); idempotent resume with present-and-correct skip; detached-at-pins
including the recorded `root_base`; and **five** crash-window fixtures. It is the
plan's largest step, its long pole, and the only one carrying an execution-model
invention.

The sibling's actuals are the evidence that this shape does not survive:

| gwz-log step | Budget | Outcome |
| --- | --- | --- |
| S1.1 | ~350-400 | candidate `c6ea636` **+7,154/−230, 12 files** → abandoned, not remediated; re-chartered S1.1-B HARD 300; S1.1-B terminal NO-GO; L-COA-8 descoped to v2 |
| S2.2 | ~350 | terminal NO-GO → S2.2-B, HARD **150** delta on reviewed base `7e2cd3c` |
| S2.5 | ~350 | terminal NO-GO → S2.5-B, HARD **200** delta on reviewed base `f165040` |
| S3.5 | ~450 | terminal NO-GO → S3.5-B, HARD **200** delta on base `f827e30` |

Every gwz-log step at ≥350 with more than one semantic decision died at the cap;
the ≤250 steps did not. gwz-log's own remedy for F10 was to split two fat steps
into five. S2.2 is not split at all, at twice the LOC of any step that survived.

Two constraints a builder will hit *inside* the cap, named in the design but not
carried into the brief:

- **`Worktree` is neither `Send` nor `Sync`** (design §5.3's own Ground cell),
  while the self-verify runs inside the jobs-bounded map. The brief says "members
  under the jobs bound… with git2 self-verification" and does not say how the
  handle is confined.
- **Whether S2.2 reuses `apply_materialize_plans` or writes its own executor** —
  see **[F1]**. Reuse requires diffing `workspace_ops/handle_materialize.rs` to
  suppress *two* behaviors; a bespoke executor duplicates roughly a hundred lines.
  Either choice is defensible; leaving it to the builder puts an architectural
  decision inside a stop-and-report boundary.

**Recommended.** Split into **S2.2a** (root-first + CLI-delegated member creation
+ self-verify + intent write + per-repo tolerant results, ≤300) and **S2.2b**
(idempotent resume + the five crash-window fixtures, ≤250), and state the
executor decision and the `Send`/`Sync` confinement in the brief. §1.5's -B rule
is a recovery mechanism, not a substitute for slicing.

### [P2 F6] §2's landing gates omit the aggregate-gate test-count pins, which every gwz-core step in this plan will move

`gwz-core/scripts/checks/run_r4bg_aggregate_gates.py` pins **exact** per-partition
test counts, not exit codes: `checked_artifact::` 447 (darwin) / 457 (linux),
`workspace_ops::merge::v1_lifecycle::` 256, root fault matrix 1, and the **lib
remainder** 1095 (darwin) / 1096 (linux). Its docstring records the sibling's
encounter verbatim:

> gwz log settlement (2026-09-01) moves the lib remainder and only it by 116
> portable tests… Linux 1096 is the same cfg-independent +116 over the
> workflow-measured v0.11.1 Linux value 980; it is FIRST-DISPATCH-EXPECTED until
> the corrected release workflow executes it.

Every gwz-core step in this plan — S1.1, S1.2, S1.3, S1.4, S2.1, S2.2, S2.3,
S2.4, S2.5 — adds tests to the lib remainder partition and therefore moves that
pin. The design named the hazard (§5.2: "the aggregate-gate test-count pins still
apply to a new child and its tests"). §2's landing bullet names the boundary
checker and stops there: no pin-move duty, no rule for who moves it, on what
measurement, or which platform legs owe a re-measure. The sibling's linux value
is *still* FIRST-DISPATCH-EXPECTED today, so this plan inherits an unmeasured pin
and will stack nine moves on top of it.

**Required.** Add the pin-move duty to §2's landing bullet, naming the
FIRST-DISPATCH-EXPECTED convention (a measured number wins) and who records the
provenance.

### [P2 F7] The plan states no frozen-surface position, and S1.1's fan-out reaches the tree the sibling plan froze

`workspace_exclude_path(root: &Path) -> PathBuf` is **infallible and
backend-free** (`sync_workspace_boundary.rs:88-90`). Resolution through the
repository handle — the design's mandated mechanism — is fallible and requires
opening the repository, so the signature changes and the change propagates. The
write path is cheap: `ensure_workspace_exclude` and `workspace_exclude_candidate`
already take `&B: GitBackend`. **The entire fan-out is the merge-evidence
readers**, which hold no backend:

`merge/finalize.rs:283`, `merge/publication.rs:40`,
`merge/abort/evidence.rs:335` and `:767`,
`merge/v1_lifecycle/authority/observe/finalization/publication/live.rs:109`,
`merge/preserve/artifacts.rs:93` — six production sites across five files — plus
eleven test references across eight files, **six of them under
`workspace_ops/merge/v1_lifecycle/tests/`**.

`GwzLogPlan.md` §1.7 named `gwz-core/src/workspace_ops/merge/v1_lifecycle/` a
**no-DIFFS frozen surface**. That freeze is plan-local (it appears in neither
`AgentProcessRules.md` nor `CurrentProgramCheckpoint.md` as a program rule), so
it does not automatically bind here — but GwzWt's very first step must diff that
tree, and the plan takes no position at all, in a document that otherwise
inherits its sibling's scope discipline carefully.

**Required.** S1.1's brief names the resolution strategy and its measured
fan-out, and the plan states its frozen-surface position — whether the
v1_lifecycle freeze applies, and if it is lifted, by whose authority. This is
also the finding most likely to move the 250 HARD cap: the churn across
six production sites and eleven test references is real LOC before a single line
of the fix.

### [P2 F8] S1.1's proof set proves the primary did not regress, but never proves the composite is fixed

§1.3's mandatory evidence is byte-identical primary behavior, the merge-evidence
hash regression (hash before == hash after), the full partitions, and the
real-workspace battery. Every one of those is a **negative** assertion: nothing
changed for the primary. None of them exercises W-CMP-1's actual purpose — that
resolution works **from inside a linked root**.

The in-tree precedent names the shape of the missing test:
`linked_worktree_uses_its_actual_git_directory_for_the_guard`
(`operation/workspace_mutator_lock.rs:189-204`), which pins that the bootstrap
guard lands in the linked worktree's own gitdir and not the main repo's. The
design review's round-1 probe supplies the exact regression to pin: reading
`<linked>/.git/info/exclude` returns `ENOTDIR (errno 20)` today, and the reader
absorbs only `NotFound`.

For the step the plan itself designates as carrying "the strictest proof duty",
an evidence set with no positive assertion is a gap, not a judgment call.

**Required.** Add to S1.1's mandatory evidence: (a) resolution from a linked root
resolves to the **common** directory, and (b) the pre-fix `ENOTDIR` failure is
pinned as a regression.

### [P2 F9] W-ENV-1's client-surface half is owned by no step — the gwz-log F7 class

W-ENV-1's closing clause: "Non-UTF-8 path bytes follow the recorded estate rows
(`gwz-cli/dev-docs/GwzLogRequirements.md` L-ENV-10/L-ENV-12: OS-accepted,
**rendered lossy**, **machine-output lossy-flagged**)." Both cited rows are
client-surface behaviors — L-ENV-10 (`:556`) governs human rendering, L-ENV-12
(`:571`) governs the `"lossy": true` flag in machine output.

§5 assigns W-ENV-1 wholly to **S2.1**, a gwz-core planner step that renders
nothing and emits no machine output. Neither S3.1's brief (which owns human
rendering and `gwz.worktree/v0`) nor S3.2's (which owns byte-parity assertions)
mentions lossy rendering or the lossy flag. This is gwz-log F7 exactly:
"Steps assign CLI-owned surface to gwz-core steps."

W-ENV-1 is in fact **three**-way split in practice: S1.4 builds the shared
parent-walking helper and adopts it into `gwz clone` (its brief cites W-ENV-1 by
name and correctly flags it as a shipped-behavior change), S2.1 owns the add-time
refusals and resume precedence, and the client halves belong to S3.1/S3.2.

**Required.** Declare W-ENV-1 split in §5 the way W-CMD-7 and W-OBS-1/2 are, and
name the halves in the owning briefs. S4.2's traceability sweep keys on the
matrix, so an undeclared split is a row that passes the sweep untested.

### [P2 F10] W-ENV-4 is split across S2.2 and the uncapped, row-less S4.2, and its cross-filesystem clause is owned by no brief

W-ENV-4 has three clauses: cross-filesystem composites permitted; **Windows in
scope with a named first-dispatch obligation**; detached/pinned members
materialize as detached worktrees. §5 assigns the whole row to S2.2, whose brief
covers only the third ("detached-at-pins default incl. the recorded root_base").

The Windows first-dispatch obligation is executed in **S4.2** — "Windows/platform
legs (the named first-dispatch obligation — this estate's standing Windows lesson
makes it a real gate)". S4.2 owns no rows in §5 and carries **no cap in §7**,
while §5's plumbing-rationale sentence — which pre-answers the "steps owning no
requirement row" tell — covers only S1.2 and S1.4. So a MUST-bearing obligation
lands in the one step that is both row-less and unbudgeted, and the plan's own
defence of row-less steps does not reach it.

No brief anywhere names a cross-filesystem fixture.

**Required.** Declare W-ENV-4 split S2.2 / S4.2; extend §5's plumbing-rationale
sentence to S4.2 or give S4.2 its row-halves; name an owner and a fixture for the
cross-filesystem clause.

### [P2 F11] S3.1 is gwz-log F23 recurring, and S3.2-after-S3.1 fully serializes Phase 3 with no stated rationale

S3.1 at cap 450 carries: the `gwz worktree` subcommand tree over five verbs, ALL
flags in one place, exit mapping, human rendering (list table **and** per-repo
result tables), the `gwz.worktree/v0` machine output, **and** command docs with
the docs gate at its landing.

gwz-log budgeted that same surface as **three** steps — S3.1 (subcommand + flags
+ exit mapping) 300, S3.2 (human rendering + docs) 300, S3.3 (machine output)
250 = **850** — and only after its reviewer split it, in F23: "three goals at the
budget ceiling before docs… The step most likely to blow its budget is now also
the one carrying the user-visible surface." GwzWt re-bundles the three into one
at 450. GwzWt's verb surface is genuinely smaller than `gwz log`'s, so 450 is not
absurd — but it is the same shape the sibling's reviewer split, re-formed at
roughly half the LOC for roughly two-thirds the surface.

**On the dependency (mandate item 2).** S3.2-after-S3.1 **is** justified, but not
for the reason the sibling used: S3.2's brief requires "machine-output byte-parity
assertions **against captured gwz-cli output**", which cannot precede gwz-cli
existing. That is a hard data dependency, not the flag-surface-stability rationale
gwz-log used — and the plan states neither. Note what the compression costs:
gwz-log deliberately kept the py **flag mirror** (S3.5) parallel with cli
rendering (S3.2/S3.3) and serialized only the **parity** step (S3.6). GwzWt's
two-step Phase 3 forfeits that parallelism, making 850 LOC of client work a
strictly serial chain, with no note that this is a choice.

**Recommended.** State the rationale; and either split S3.1 (flags + exit +
docs / rendering + machine output) — which also restores the parallel py-mirror
lane — or raise its cap honestly with a note that it exceeds the target by design.

### [P3 F12] "Intent record written at plan completion" is compatible with the design but ambiguous

Design §5.5 step 3 writes the intent record **before** root creation ("Write the
intent record; create the ROOT worktree first"). S2.2's brief says "intent record
written at plan completion". Read as "when the PLAN stage completes" these are the
same instant; read as "when execution planning completes" they are not. The brief
disambiguates itself correctly two clauses later — its crash-window fixture list
begins "before intent write; after intent, **no worktrees**" — which is only
constructible if intent precedes root creation. No semantic drift; a wording
hazard in a step that will be read under a HARD cap. Use the design's phrasing.

### [P3 F13] W-CMD-2's compression preserves the dispatch and drops the refusal content

S2.1's brief carries the tri-state faithfully: create base = lock pin/snapshot,
root base = primary HEAD at plan time, exists-in-none/all/some with
refuse-on-mixed, both git conflict classes absorbed, one aggregated teaching
refusal. Two testable obligations are lost in the compression: the design requires
the mixed-state refusal to **list which repos have the branch and which do not**,
and the exclusivity refusal to **name the repo, the branch, and the holding path**.
S2.1 also owns W-SAF-1 ("name the repo, the conflict, the exit"), which covers
part of it, and the design governs regardless — so the risk is low. One clause
restores it.

### [P3 F14] W-ENV-6's second half is asserted by the matrix but not row-cited in the brief

§5's note on W-ENV-6 reads "prune's half noted in S2.5's brief". S2.5's brief
names W-CMD-4, W-CMD-5 and W-CMD-8 in its owns list and **never cites W-ENV-6**,
though the substance is present ("orphaned-intent cleanup"). §2's own rule is
that "split-owned rows name their halves in **both** briefs". S4.2's sweep keys
on the matrix, so the matrix asserting a citation the brief does not make is the
one place that discipline can fail silently. Either add "W-ENV-6's prune half" to
S2.5's owns list, or declare the row `S2.4 + S2.5` the way W-OBS-1 is declared.

### [P3 F15] §7's "roughly a quarter of gwz log's final size" is off by about a factor of two

Measured, landed `gwz log` handwritten surface:
`gwz-core/src/operation/commit_log/` **7,495** LOC (of which `tests.rs` 3,014,
`coalesce_tests.rs` 765, `merge_tests.rs` 685, `request.rs` 750, `merge.rs` 692,
`handler.rs` 552, `mod.rs` 512); `gwz-cli`'s six log files (`log_exec`,
`log_render`, `log_machine`, `logargs`, `log_long`, `log_after`) **967**;
`gwz-py/src/gwz/cli_log.py` **395**. That is ≈**8,900** handwritten LOC including
tests, excluding the abandoned S1.1 candidate and generated protocol artifacts —
i.e. on this plan's own §1.4 basis.

GwzWt's 4,000 is ≈**45%** of that — roughly a half, not a quarter. The
arithmetic that matters is correct (Phase 1 250+150+250+250 = 900; Phase 2
400+500+150+300+400 = 1,750; Phase 3 450+400 = 850; Phase 4 500; total 4,000).
The comparison is offered as the *credibility argument* for those totals
("consistent with composing git features rather than inventing semantics"), and
at 45% it carries about half the weight claimed. Record the measured ratio; the
caps are the operative constraint either way.

### [P3 F16] The -B rule has no terminus

§1.5 charters `<step>-B` when a step dies at the two-round cap, replacing it in
place with base and delta named. It never says what happens when the **-B** also
dies. gwz-log needed exactly that twice, and both times the answer was written
*into the -B charter itself*: S1.1-B carried a **pre-authorized fallback**
(descope L-COA-8 to v2 — executed 2026-08-30 with no operator round-trip), and
S2.5-B/S3.5-B carried "if round 2 fails, dead as chartered; freeze, file, return
to the operator — no further rounds under any framing". As written, §1.5 permits
an unbounded -B chain. One sentence closes it: every -B charter states its
terminal disposition (descope, freeze-and-return, or operator escalation) at
charter time, not at failure time.

### [P3 F17] S1.3's "core dispatch stubs filled by Phase 2" names no per-stub owner — the shape of gwz-log's unplanned S2.7

S1.3 ends "Core dispatch stubs filled by Phase 2." gwz-log made the same
arrangement at S2.0 and had to charter an **unplanned mid-project step** on
2026-08-31 to close it: "a dependency audit proved that every Phase-2 engine seam
was complete but S2.0's public handler still returned `UnsupportedOperation`, no
commit-log output registry existed, and the adopted S3.1 step was explicitly
gwz-cli-only." GwzWt's arrangement is structurally better — each verb step owns a
whole verb, so each can fill its own stub — but the plan does not say so, and
"Phase 2" as a collective owner is precisely what left the gap open last time.
State that each verb step (S2.2/S2.4/S2.5) fills its own dispatch stub through
public dispatch end-to-end, and that its review checks the stub is gone.

### [P3 F18] The steps whose diffs trigger the §2 real-workspace battery are pre-named for only two of them

§2's rule fires on "any landing whose diff touches `workspace_ops/` or the
boundary-sync/merge-evidence surface". S1.1 and S2.3 name the duty in their
briefs — and **S2.3's is correct**: it touches the boundary-sync surface and says
so, consistent with §2's own rule (mandate item 4's check). Two more steps will
trigger it without saying so:

- **S1.4** adopts the nesting helper into `gwz clone`, whose target-only check
  sits at `handle_materialize.rs:476-481` — inside `workspace_ops/`.
- **S2.2**, if it reuses the materialize seam at all (see **[F1]**, **[F5]**).

The rule is stated generally and catches both automatically, so nothing is
unsafe. The cap arithmetic in those briefs does not account for the battery's
landing cost. Record-only.

---

## Re-derived coverage matrix (deltas from §5)

**Row set: exact match, 26/26.** Re-derived independently from `GwzWtDesign.md`
§4's headings — Command surface W-CMD-1..8 (8), Compatibility scope W-CMP-1..3
(3), Lifecycle and crash behavior W-LIF-1..3 (3), Safety and refusals W-SAF-1..4
(4), Validity envelopes W-ENV-1..6 (6), Observability W-OBS-1..2 (2). §5's table
carries the same 26. **Zero unowned rows, zero double-owned rows, zero dangling
citations, zero rows the design does not have.** Every declared split (W-CMD-7
three ways, W-OBS-1 and W-OBS-2 two ways each) names its halves in the owning
briefs, verified against §3. The two row-less steps (S1.2, S1.4) carry the stated
plumbing rationale, which correctly pre-answers gwz-log S0.1's tell. **Twenty-two
of twenty-six rows verify clean.** Four differ:

| Row | §5 says | Re-derived | Finding |
| --- | --- | --- | --- |
| **W-CMD-6** | `S4.1` — "the parity proof (scope landed via W-CMP rows)" | `S4.1`, **under-proved**: the brief's verb list omits `stage`, `repo`, `materialize` and `merge`, all named by W-CMP-1's own blast radius as reaching the changed function | **[P1 F4]** |
| **W-ENV-1** | `S2.1` — "helper built in S1.4 (mechanism note)" | **Three-way split, undeclared:** `S1.4` (shared parent-walking helper + `gwz clone` adoption — a shipped-behavior change) + `S2.1` (add-time refusals + resume precedence) + **`S3.1`/`S3.2` (non-UTF-8 lossy render and lossy machine flag, L-ENV-10/L-ENV-12) — owned by no step at all** | **[P2 F9]** |
| **W-ENV-4** | `S2.2` | **Split, undeclared:** `S2.2` (detached-at-pins only) + `S4.2` (the Windows first-dispatch obligation — in a step with no rows and no cap); the cross-filesystem clause is in **no** brief | **[P2 F10]** |
| **W-ENV-6** | `S2.4` — "prune's half noted in S2.5's brief" | `S2.4` + `S2.5`; the asserted note is **not in S2.5's brief** — S2.5 names W-CMD-4/5/8 only, though prune's orphaned-intent substance is present | **[P3 F14]** |

Two structural notes that are not row-ownership defects but bear on the matrix's
use as "the plan-review authority" (§5's own framing): W-SAF-1's "style proven
again at every step's review" is a soft co-ownership that S4.2's sweep cannot
mechanically check, and S4.2 itself owns nothing while executing W-ENV-4's
Windows obligation and the sweep over this table.

---

## Mandate item dispositions

1. **Coverage matrix, re-derived not trusted.** Row set is an **exact 26/26
   match** with the design; no dangling citation, no unowned or double-owned row;
   both row-less Phase-1 steps carry the stated rationale. Four ownership deltas,
   one of them a row-half owned by nobody. **[F4, F9, F10, F14]**
2. **Dependency truth.** No invented dependency found — the F9 class is clean, and
   S2.3→S1.1-only is correct (detecting a linked root needs S1.1's resolution; its
   residual fixture is hand-rollable, as the design review's own probes were). The
   defect is the **inverse**: S2.2 consumes the boundary-sync surface S2.3 owns and
   does not declare it — verified in the tree, where `apply_materialize_plans`
   boundary-syncs the root it creates (`handle_materialize.rs:392`, reached by
   `clone_workspace_with_emitter:545-560`). S2.2 also consumes S1.4's intent-record
   API without listing it, though the ordering holds transitively through S2.1.
   S3.2-after-S3.1 is genuinely justified — by captured-output byte-parity, not by
   the sibling's flag-stability rationale — but the reason is unstated and the
   collapse to two steps forfeits gwz-log's parallel py-mirror lane. **[F1, F11]**
3. **Step boundaries, caps, risk duties.** S1.1's proof set is the right *negative*
   set and has **no positive assertion** that the composite is fixed; 250 HARD is
   credible only if the resolution strategy avoids threading fallibility through
   six backend-free merge-evidence call sites, which the brief does not address.
   S2.2's 500 HARD is a **bundling smell of the F10 class** — ten deliverables, a
   novel execution model, five crash fixtures, and two unstated buildability
   constraints — in a plan whose sibling lost four steps to exactly this shape.
   **[F5, F7, F8]**
4. **Process fidelity.** Missing: the aggregate-gate count-pin duty that every core
   step will trigger; a frozen-surface position; a terminus for the -B rule; the
   parking rule §2 silently dropped. Misstated: the proportionality carve-out,
   asserted as operator-adopted with no record anywhere. Accurate: the verified-fact
   carve-out, the J-7 battery duty **including S2.3's boundary-surface trigger,
   which its brief names correctly**, the docs gate, the py/drift gates, ritual 7,
   the two-round cap, tier economy. Nothing is over-heavy. **[F3, F6, F7, F16, F18]**
5. **Design-fidelity spot checks.** W-CMD-2's tri-state and W-ENV-1's resume
   precedence survive compression **semantically intact**; what is lost is refusal
   *content* (which repos hold the branch; the holding path) and W-ENV-1's
   client-surface clause. "Intent record written at plan completion" is compatible
   with §5.5's before-root-creation and is disambiguated by S2.2's own fixture list.
   S4.1's verb list is **incomplete** against W-CMP-1's enumeration, and **merge's
   absence is a gap, not a deliberate scope call** — untested and carrying a named
   cross-materialization evidence hazard. Separately, the design itself still
   carries one uncured round-2 residue (§5.5 step 3's withdrawn exclude clause)
   which the plan neither inherits nor flags. **[F2, F4, F9, F12, F13]**
6. **Budget sanity.** §7's sums are **correct to the line** (900 / 1,750 / 850 /
   500 = 4,000). Two flags: the "roughly a quarter of gwz log's final size"
   comparison measures at ≈45%, roughly a half; and S4.2 carries a real platform
   gate with no cap at all. Cap history predicts S2.2 (500) and S3.1 (450) as the
   blow candidates, and S1.1 (250) if the fan-out is threaded rather than confined.
   **[F5, F7, F10, F11, F15]**

---

## Conditions for GO

1. **S2.2's exclude posture, stated.** The add executor performs no exclude write
   and no boundary sync on any constituent including the composite root; and
   either S2.3 becomes an S2.2 dependency or the plan states S2.2's executor never
   reaches `sync_workspace_boundary`. **[F1]**
2. **The design's §5.5 step 3 residue** corrected at source or its supersession
   recorded by name in the plan. Closes with condition 1 as one edit. **[F2]**
3. **§2's proportionality carve-out** cited to an operator record or restated as
   proposed-and-pending, and the parking rule reinstated or its removal
   authorized. **[F3]**
4. **S4.1's verb list** derived from W-CMP-1's enumeration (add `stage`, `repo`,
   `materialize`), with merge either proven or recorded as a named residual with
   its hazard. **[F4]**
5. **S2.2 split** (or its cap raised honestly), with the executor decision and the
   `Send`/`Sync` confinement named in the brief. **[F5]**
6. **§2 gains the aggregate-gate pin-move duty**; the plan states a frozen-surface
   position; S1.1's brief names its resolution strategy and fan-out, and gains a
   positive linked-root assertion plus the `ENOTDIR` regression. **[F6, F7, F8]**
7. **Matrix repairs:** W-ENV-1 declared three-way split with the client half
   assigned; W-ENV-4 declared split with S4.2 covered by §5's rationale sentence
   and a cross-FS fixture owner; W-ENV-6's prune half row-cited in S2.5.
   **[F9, F10, F14]**
8. **Record-only, no re-review needed:** **[F11]** S3.1's rationale and slicing,
   **[F12]** intent-write phrasing, **[F13]** refusal-content clauses, **[F15]**
   the measured size ratio, **[F16]** the -B terminus, **[F17]** per-stub dispatch
   ownership, **[F18]** battery-triggering steps.

Conditions 1-4 are the ones that stop a builder building the wrong thing.
Conditions 5-7 are what stop a step dying at its cap or a row shipping untested.
All are document edits; none reopens a settled design decision or requires a
re-plan.
