# GwzM5-8 A1 Release Plan — the first release carrying the activated v1 writer

Date: 2026-08-25. Author: the implementation lane. Status: PROPOSED —
phases R1-R3 and R6 are lane work ready to start; R4 carries an operator
decision; R5 halts on operator actions (member pushes, the tag).

## 0. Object, inputs, and one correction

**The object:** the first release cut from gwz-core main at or after the
A1 activation (`1a31851` + corrective `8e40fa8`), shipping `--no-ff` and
the production v1 writer to users. Version number is an operator
decision (§7); this plan calls it **vNEXT**.

**Governing inputs, consumed directly:** `GwzM5-8A1ActivationRecord.md`
(esp. §2 scope, §17); `GwzM5-8A1DecisionPacket.md` §1 (Decision 1 B),
§2 (Decision 2 A′), §4 (the sequencing order and its forbidden
ordering); `GwzM5-8A1Activation-ReviewSafety.md` §R2.2 condition (c)
(the release-time manifest truth obligation) and [P3-R2-3]/L7 (the
recorded release-lane battery scope); `GwzM5-8R2DSettledTuple.md` §11.2
(the T-5 OWNED-CARRIER row riding native-evidence regeneration);
`scripts/retained_readers/manifest.json` (the decode-generations
register). Decision 3 (durable cursor) is already consumed — its wire
amendment was accepted pre-A1 and its implementation landed with the
program; it imposes no release work.

**The correction, stated plainly:** the lane's earlier summary called
the escape story "the real long pole" of a release. The decision
packet's own interaction table says otherwise **provided A′ lands**:
choosing A′ "leaves the escape ladder as defense-in-depth" — it is
choosing B (accept-the-wedge) that would make the §4.4 runbook a
release blocker ([Q6]/[Q9], packet §2.4). This plan takes A′, so the
runbook is a strongly-recommended rider with an operator decision
attached (§R4), not a hard gate. The hard gates are §1.

## 1. The release gates (what must be true at the tag)

| Gate | What | Source | Phase |
| --- | --- | --- | --- |
| G1 | Decision 2 **A′** landed: pre-mutation typed refusal of configured non-passthrough foreign clean filters (allowlist `lfs`) at the recovery-grade checkouts | Packet §2: "Before the next release cut from this tree"; the §4 forbidden ordering — a release carrying the `disable_filters` code with Decision 2 open "ships the git-crypt wedge with no policy and no runbook" | R1 |
| G2 | Decision 1 **B** landed: birth-time `core.autocrlf=false` + `core.eol=lf` pins + filters-off clone checkout at every gwz materialization edge, plus the un-pinned CRLF matrix sentinel | Packet §1.5: "Option B in the next release train … every release shipped without the pins enlarges the legacy-workspace population" | R2 |
| G3 | The retained-reader **A1 generation** manifest entry describes SHIPPED behaviour (ordinary=v0, no-ff=v1), not the contract row | Safety §R2.2 binding condition (c) | R3 |
| G4 | Release-lane gates carry the R4b-G mechanical batteries that are in-repo; the workspace-root-reading checkers stay un-wired WITH their blocker recorded (no repeat of the `32749441874` class) | L7's recorded scope; activation record §17's minted topology lesson | R3 |
| G5 | Native evidence at the release candidate: three-arm matrix green at the RC sha; the T-5 candidate-pair regeneration discharged on its §11.2 carrier; retained-readers green with the new generation | R2-F/R5 standing release gates (must-not-flip item 7 kept them unmoved for exactly this moment) | R5 |
| G6 | Release notes name the shipped scope honestly: `--no-ff` live; ordinary starts remain v0 (the M5c named residual); the new refusal classes (foreign-filter, adopted-CRLF availability) with their remedies | Activation record §2; packet §§1-2 user-impact rows | R3 |

Not gates, decided here: the wedge runbook (§R4, operator decision);
the renormalize command (§R6, with-or-after release).

## 2. Phase R1 — the wedge-policy floor (Decision 2 A′)

Milestone: any release cut from this tree is safe against the
git-crypt-class post-ref-move wedge — the failure becomes a clean,
named, pre-transaction refusal. Unconditional first: the §4 forbidden
ordering binds every release from a tree carrying the amendment's
`disable_filters` checkouts, which the v0 abort flow already reaches.

- **Step R1.1 — the A′ predicate and refusals** (~350 LOC aspirational,
  packet's own estimate ~300-450). One predicate: a path the
  recovery-grade checkout would rewrite is covered by an attribute
  filter whose value is outside the allowlist (`lfs`) AND whose
  `filter.<name>.clean` (or `.process`) is present in effective config.
  Refuse with a typed error naming filter+path BEFORE the ref
  transaction. Built once, consumed at the call sites the packet names
  (the two recovery checkouts; §R6's renormalize command reuses it).
  Tests: configured non-passthrough → refusal pre-mutation; unconfigured
  attribute → proceeds; `lfs` → proceeds; refusal fires before any ref
  moves (assert on-disk state unchanged).
- **Step R1.2 — focused State-axis review + OPEN-DECISION closure**
  (docs only). The packet mandates a focused State review for this
  package; the review report and the decision-ledger closure annotation
  ("decided: A′, landed at <sha>") file together.

## 3. Phase R2 — CRLF birth-time pins (Decision 1 B)

Milestone: gwz-born worktrees are blob-exact from birth on every OS;
both tripwired exposures (unrewritten smudged files; `stash_save`
filtered reset) closed for gwz-created repos; CI stops being blind to
the class. Parallel-friendly with R1 — different code regions
(create/clone funnel vs recovery checkouts).

- **Step R2.1 — the pins** (~200 LOC aspirational, packet's estimate).
  `create_repo` (`repository.rs:63-70`) writes repo-local
  `core.autocrlf=false` + `core.eol=lf` at init;
  `clone_repo_with_progress` (`transport.rs:7-32`, the single funnel
  for all production clone sites) passes
  `CheckoutBuilder::disable_filters(true)` for the initial
  materialization and pins the same two keys immediately after.
  Creation-time ONLY — run 9's lesson (mid-life pinning manufactures
  dirt) gets pinned as a test, not just prose. Tests: born-repo
  worktree is blob-exact under host `autocrlf=true`; forward checkout
  stays exact; `stash_save` round-trip is a filter no-op.
- **Step R2.2 — the CRLF matrix sentinel + decided-annotations**
  (small). One un-pinned CRLF lane (or expected-fail sentinel) in
  `windows-matrix.yml` so the class pages in CI (the ledger's F6-class
  "no failing sentinel anywhere" complaint applied to B's regression
  surface). The tripwire-ledger and amendment OPEN DECISION entries for
  Decisions 1 and 2 close as decided in the same pass (packet §4 step
  1). Topology audit per the §17 lesson before wiring anything.
- **Step R2.3 — focused State-axis review** (docs only), per the packet.

## 4. Phase R3 — release-truth surfaces

Milestone: what the release SAYS matches what it SHIPS — manifest,
release-lane gates, versions, notes. Independent of R1/R2 code;
parallel-friendly.

- **Step R3.1 — the A1 decode generation** (small; G3). Add the vNEXT
  generation to `scripts/retained_readers/manifest.json` describing
  shipped behaviour: the reader decodes v0 AND v1; ordinary/custom
  starts still create v0 (`ACTIVE_WRITER_FLOOR = V0`, the M5c
  residual); `--no-ff` creates v1. Companion duty in the SAME commit
  (ritual 5): the retained-readers tuple growth and any L2-04
  count-pin movement.
- **Step R3.2 — release-lane battery wiring** (small; G4).
  `release.yml` gains the in-repo R4b-G batteries per L7's recorded
  scope (fault, byte-equivalence, unknown-field — plus privacy and
  call-graph if runtime cost is acceptable at release cadence; state
  the choice). Explicitly NOT wired: `check_m4_scenario_map.py` and
  L2-05 — release.yml checks out gwz-core alone, the same topology
  that failed run `32749441874`; their blocker comments extend to the
  release workflow. No wiring lands without the §17 topology audit.
- **Step R3.3 — versions and notes** (small; G6). Reconcile the
  version story first (tag `v0.10.5` exists while `Cargo.toml` says
  `0.10.4` — establish the convention from the v0.10.5 release before
  bumping; gwz-cli is on its own `0.2.0-dev` line). Bump to vNEXT per
  the operator's §7 choice. Release notes: `--no-ff` shipped; the V0
  ordinary-start floor named with M5c as its owner; the new
  foreign-filter refusal and adopted-CRLF availability classes with
  their remedies (renormalize command status per §R6; runbook pointer
  per §R4).

## 5. Phase R4 — the wedge runbook (Q9) [OPERATOR DECISION]

Milestone: a user who hits any surviving wedge or refusal class has a
documented path. Under A′ this is defense-in-depth, not a packet
mandate — but the escape design's own finding stands: the released line
"can wedge today with no tool escape and no documentation" (§9.1), and
vNEXT enlarges the open-merge population. The lane recommends shipping
the minimal runbook with vNEXT.

The decision is the operator's because the escape/runbook lane is
second-lane, blocked on operator handoff. Three branches:

- **(i) RECOMMENDED — narrow handoff:** the operator hands off ONLY the
  §4.4 minimal runbook slice (documentation of the quarantine/`mv`
  remedies + the new typed-refusal remedies; no escape-package
  implementation). One doc step, reviewed single-axis.
- **(ii)** the operator writes/ships the runbook themselves alongside
  vNEXT.
- **(iii)** the operator signs an explicit gap acceptance for vNEXT
  (defensible under A′; argued against — it ships a known-undocumented
  wedge surface into a growing population).

A one-line reply choosing a branch unblocks this phase; (iii) requires
only the signature, (i) requires only the handoff sentence.

## 6. Phase R5 — release execution (G5; halts on operator actions)

Milestone: vNEXT published with the R2-F/R5 gates green at the tag.
Depends on R1-R3 (and R4 if branch (i)/(ii)).

- **Step R5.1 — freeze the release candidate.** Member pushes for
  gwz-cli `3000916`+ and gwz-py `3d19dcd`+ (OPERATOR — the lane pushes
  only gwz-core); lock captured; the RC sha named in the checkpoint.
- **Step R5.2 — native evidence at the RC.** Three-arm matrix at the
  RC sha, pre-attributed expected-green; the T-5 candidate-pair
  regeneration discharged on its §11.2 OWNED-CARRIER row (apply the
  archived pair to the regenerated evidence, verify digests);
  retained-readers green with the A1 generation live.
- **Step R5.3 — the tag and the release.** OPERATOR ACTION ONLY: the
  lane performs no tag operations under any circumstances (standing
  git rules; released tags immutable). On the published tag,
  `release.yml` verify must go green on both OS legs.
- **Step R5.4 — the release record.** A `GwzMergeCheckpoint-vNEXT.md`
  per the existing convention + the program checkpoint record: the
  tag, the tuple, the gate tails, the generation entry, and the resume
  order for the post-release queue (R2-E, M5c, R2-F remainder).

## 7. Phase R6 — `gwz repair --renormalize` [non-gating, with/after]

Milestone: adopted/legacy CRLF worktrees get their operator-invoked
remedy (packet Option D as follow-up; schedule "with-A1 or after" —
i.e., with vNEXT if it fits, after it otherwise; the stranded
population it serves only shrinks once R2's pins ship). Depends on R1
(shares A′'s predicate as its refusal gate). Deliberately manual — it
rewrites worktree bytes; never automatic. ~1 command + tests + docs;
<500 LOC aspirational.

## 8. Open operator decisions (a one-line reply each)

1. **vNEXT number** — lane recommends `v0.11.0` (new user-visible
   capability + new refusal classes ⇒ minor bump; `v0.10.6` if the
   operator prefers the patch line).
2. **§R4 branch** — (i) narrow runbook handoff (recommended) / (ii)
   operator-authored / (iii) signed gap acceptance.
3. **§R6 timing** — renormalize with vNEXT or after.
4. **Member pushes + the tag** — operator actions inside R5; schedule
   at will.

## 9. Non-goals of this train

M5c (the ordinary-start v1 owner + floor raise — its own milestone);
R2-E (the re-reserved keys and binding obligations); the escape
implementation packages beyond the §R4 runbook slice; the multi-repo
CI checkout (tuple §11.3 item 7 — R2-F work; until it lands the m4
scenario map and L2-05 stay local-gate with their blockers recorded);
any change to `ACTIVE_WRITER_FLOOR`.

## 10. Review plan and ordering summary

R1.1 and R2.1 land behind **focused State-axis reviews** (packet
mandate); R3 steps are interior single-axis with auto-escalation on
P0-P2; R5.2's evidence is checked against the §11.2 carrier row. The
critical path is R1 → R5; R2/R3 run parallel to R1; R4 runs parallel
once its branch is chosen; R6 floats. Foundational-first: nothing in
R3-R6 blocks starting R1/R2 immediately.
