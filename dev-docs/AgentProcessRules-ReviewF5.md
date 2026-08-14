# AgentProcessRules.md — independent review (F5)

Date: 2026-08-15
Reviewer: Claude (Fable 5).
Object: `dev-docs/AgentProcessRules.md` (2,011 lines, initial extraction).
Method: full read; existence check of all 16 spot-sampled file citations;
section-anchor verification of 12 section-level citations; vocabulary-claim
checks against the review corpus; and cross-examination of the honesty and
grounding claims against the independently verified ground truth in
`GwzM5-8ProgressReviewF5.md` and `GwzReleaseReview-v0.10.5-F5.md`.

## Verdict

**Fit for purpose as the replication master after correction of four P2
findings.** The document substantially meets its own acceptance bar: the
layered L1/L2/L3 design with copyable quoted rules is the right shape, all 12
requirements of the commissioning prompt are addressed, every sampled file
citation resolves, the templates and role prompts consolidate real practice,
and the honesty section is genuinely self-critical where it engages. The
failures are specific: two citation defects, and two places where the
document's account of history is softer than the verified record — which
matters more than usual, because grounding and honesty are this document's
entire warrant.

## Findings

### [P2-1] Wrong section citations for the program-structure rules

L1-04 and L3-04 cite `GwzM5-8Refactor.md` "§§12-13" (and L3-03 cites "§13")
as the home of waves, dependency graphs, and package tables. Verified against
the current document: §12 is "Source provenance for M7" and §13 is
"Finalization decomposition"; the waves and work packages live in **§16 and
§17**. (The companion `GwzMergePlan.md` §§13/15/18/19 citations all check
out.) The rulebook thereby commits exactly the cross-reference-drift defect
class its own L1-24/L2-05 rules exist to police — and its §11.3 proposed
checker verifies only that cited *paths* exist, so it would not catch its own
defect. Fix the citations and extend the §11.3 proposal to verify section
anchors, not just paths.

### [P2-2] Cited ledger section does not exist

L1-18's grounding says the dual-review requirement is "formalized in
`GwzM5-8ChangeBudget.md` 'Mandatory review points.'" No such section exists
(case-insensitive search). The substance is real — the ledger mandates dual
reviews in per-package prose (e.g. the R4b-TI/TR review-gate sentences) — so
this is a citation-precision defect, not a fabricated practice. Cite the
actual package clauses, or create the named section the citation imagines.

### [P2-3] L1-17's grounding claim washes the R4b history

L1-17 (settle before review) claims it was "consistently used by the R4b
review reports." Under the rulebook's own §1.2 definition of *settled* —
"intended changes committed and unrelated dirt explicitly identified" — this
is not true of the R4b checkpoint era: TI through P0.2 were reviewed as
uncommitted trees later squashed into two ~19k-line commits, leaving the
reviewed-and-accepted intermediate states unrecoverable from git (verified in
`GwzM5-8ProgressReviewF5.md` §7). The commit-before-review discipline became
practice at the P1 freeze — imposed by the operator instruction the rulebook
itself reproduces in Appendix A.5. The honesty section (§14) omits this
episode entirely, though it is the most consequential process failure in the
program's history: acceptance evidence that cannot be re-derived from any
checked-in tree. Correct L1-17's grounding to name when the practice actually
began, and add the episode to §14.1.

### [P2-4] The budget ratchet is documented as mechanism, absent as history

§10.2 accurately describes the 120% stop-and-rereview mechanism, and its
verification clause ("ledger keeps original and revised ceilings") is true.
What §14 never admits is the verified pattern: every major ceiling revision
went upward (R4b-P production 1,450 → 14,000, ~10x; the R0 harness 500 →
7,100), and no ceiling ever forced descoping. A replication user reading this
rulebook would expect the ledger to control cost; in practice its real,
useful function has been to force re-review at each escalation. None of the
eight §14.4 proposed changes addresses cost control. Add the ratchet to
§14.2, and add a ninth proposal: a ceiling revision must answer "what is cut"
before "what is raised."

### [P3-1] Independence ceiling unstated

L1-18 and §13.3 assert peer-blind dual review without disclosing that both
reviewers are the same model running different role prompts under the same
frame — procedural, not organizational, independence. The verified record
shows this still catches real defects (divergent GO/NON-GO verdicts on the
same tree; a P0 design bug; a false-green CI catch), which is precisely why
stating the limit honestly strengthens rather than weakens the replication
case. One paragraph in L1-18's commentary or §14.3 suffices.

### [P3-2] The freeze-time-physics lesson is diagnosed but not encoded

§14.1.2 and §14.2.5 correctly identify the pattern (frozen contracts
repeatedly failed on first physical contact: the compound root phases, the
Windows sync no-op, the publication race). No L1/L2 rule encodes the
countermeasure. L1-15 governs recovery grammar and proposed change #4 moves
native probes earlier, but the specific reviewable rule is missing: at
contract freeze, a physical-model checklist — one durable phase per
durability boundary; admitted primitives named per platform; no compound
action that no portable primitive can implement. That checklist item would
have prevented the largest amendment in the corpus.

### [P3-3] Vestigial verdict vocabulary incompletely listed

§2.1 marks "ready for M2b" as vestigial, but the Aug-era amendment reviews
used `HOLD` and `NON-GO` (six occurrences remain in
`GwzM5-8R4bInterfaceAmendment-2-ReviewArch.md` alone). L1-19 reads as if
GO/NO-GO plus counts were always the contract. Add HOLD/NON-GO to the
vestigial list so a reader of the historical corpus is not confused.

### [P3-4] No self-maintenance rule

The rulebook prescribes lifecycle and amendment discipline for every document
type except itself. State that changes to `AgentProcessRules.md` follow L1-08
(bounded amendment, changelog, operator sign-off), and which reviewer class,
if any, its future revisions require.

## Strengths worth preserving

- **Citation integrity is otherwise excellent**: all 16 sampled file
  citations resolve, including obscure ones (`evidence-macos-aarch64.json`,
  the historically misspelled `GwzDevCodeM2aReviewPriompt.md`), and §7/§19/
  §21/§22.4 section citations are accurate. The two failures above are the
  exception, not the pattern.
- **The refusal to invent**: `[unexercised]` marking is used honestly — the
  document declines to claim a branch/PR policy that was never practiced
  (§12.1), marks the bootstrap filenames unexercised outside GWZ, and its
  proposed doc-gate entries are correctly not implemented.
- **Instruction-boundary discipline**: §15.6 refuses to convert the
  operator's conversational carve-out remark into design authority, citing
  the accepted amendment as controlling until a real decision artifact
  exists; §15.5 records rather than silently resolves the filename conflict;
  Appendix A reproduces operator instructions verbatim, typos included, and
  correctly scopes out hidden runtime instructions.
- **The L1 quoted-text device** — only the quoted block is copyable, the
  commentary stays behind — is the single best structural idea in the
  document and should survive every future revision.
- **§15.4's acceptance-scope doctrine** (acceptance binds only its exact
  tuple and scope; the latest review of the same object controls) is the
  cleanest statement of that rule anywhere in the corpus, including the
  documents it summarizes.
- The §9 role prompts encode the right adversarial stance ("Do not trust
  green tests or the remediation narrative") and §14.2 is unusually candid
  for a self-assessment (the denylist-evasion catalogue, TOCTOU as an
  architecture defect, hidden-allocation sort case).

## Disposition

Recommended: apply P2-1..P2-4 as a single amendment round (they are all
documentation edits), fold P3-1..P3-4 in at the same time or the next
revision, then adopt the document as the replication master. After those
corrections this reviewer would return GO against the commissioning prompt's
acceptance bar without reservation.
