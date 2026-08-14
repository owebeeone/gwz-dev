# dev-docs/history

Nothing in this directory is normative. Files here are fully superseded
process artifacts (typically old review rounds) retained for provenance,
moved under rulebook rule L1-33 (`dev-docs/AgentProcessRules.md`).

Rules for this directory:

- A file moves here only when nothing current cites it as controlling:
  never a controlling document, the latest review round of an object, a
  checker-manifest entry, or a rulebook-cited file. A citation check (an
  ad-hoc scripted grep today; standing tooling arrives with the Phase 2
  work in `GwzProcessOptimization.md`) runs before every move.
- Moves use `git mv`; full lineage remains in git history.
- If a document in here turns out to be cited by something current, moving
  it back is the correction — do not duplicate it.
- Sweeps recur at checkpoint boundaries. Many currently-superseded rounds
  remain in `dev-docs/` only because live documents still cite them in
  embedded status histories; they become movable when the status-smearing
  cleanup (`GwzProcessOptimization.md` §2.2) lands.
