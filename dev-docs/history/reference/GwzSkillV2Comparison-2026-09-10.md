# Compact-v2 skill trial — 2026-09-10

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

**Run complete; results are mixed.** V2 improved Qwen's development efficiency,
but did not fix Gemma's development lane failures or consistently eliminate
registration overchecking. Keep the installed skill unchanged pending a decision.

Durable accounting: [per-cell JSON](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/agent-usability/runs/imported-summaries/GwzSkillV2Comparison-2026-09-10.json).
Remote campaign: `E:\gwz-agent-eval\runs\skill-compact-v2`; its log ends with
`COMPARISON_COMPLETE`. No inference remains running for this campaign.

## Results

Each development arm ran three repetitions of four tasks. Outcome success is
independently checked, separate from the model stopping cleanly. Token totals
include failed attempts and infrastructure replacements; `≥` means unknown
provider usage remains additional, not zero.

| Development | Outcome pass | Passed and finished | Calls | Total tokens | Tokens per success |
| --- | ---: | ---: | ---: | ---: | ---: |
| Qwen installed v1 | 12/12 | 10/12 | 169 | 550,716 | 45,893 |
| Qwen v2 | 12/12 | 11/12 | 141 | 444,423 | 37,035 |
| Gemma installed v1 | 9/12 | 9/12 | 129 | ≥359,262 | ≥39,918 |
| Gemma v2 | 9/12 | 9/12 | 138 | 443,558 | 49,284 |

Qwen development used 19% fewer tokens and 17% fewer calls with v2. Gemma had
the same outcome rate, more calls and higher known token usage; its baseline's
two unknown-usage calls prevent an exact cost-ratio claim.

| New held-outs | Outcome pass | Passed and finished | Calls | Total tokens | Tokens per success |
| --- | ---: | ---: | ---: | ---: | ---: |
| Qwen installed v1 | 2/2 | 2/2 | 27 | 76,168 | 38,084 |
| Qwen v2 | 2/2 | 2/2 | 36 | 118,991 | 59,496 |
| Gemma installed v1 | 1/2 | 1/2 | 33 | 107,261 | 107,261 |
| Gemma v2 | 2/2 | 2/2 | 37 | 120,650 | 60,325 |

Held-outs reversed the efficiency picture: Qwen v2 used 56% more tokens, while
Gemma v2 completed the lane task that its baseline failed. These are two narrow
variants per arm, not proof of broad generalization.

Total: **56 scored cells, 58 actual episodes, 710 calls, ≥2,221,029 tokens**.
Two baseline Gemma inspection calls returned partial `done:false` responses
without usage. Both stopped before action execution and were replaced once in
fresh fixtures under the predeclared policy. Both replacements passed; all
original responses/costs remain retained. No infrastructure cells remain
unresolved. Across both phases Qwen passed 14/14 in both arms; Gemma passed
10/14 baseline versus 11/14 v2.

## Targeted behavior

Both Gemma skills failed all three development lane tasks. Baseline traces
included overwriting rather than appending and claiming success. V2's first
lane trace never performed any content edits: it consumed turns on path/command
recovery and inspections, then stopped at the limit. The held-out Unicode-root
lane passed with v2, but a one-off pass does not establish that preservation is
fixed. The product binary and scorer were unchanged.

Qwen registration reached the cap after success twice with baseline and once
with v2. Development `repo sync` calls fell from five to three, but v2 still
issued an additional sync in held-out registration. The instruction reduced
some loops, not all. No v2 episode finished while failing its independent
checks; two baseline Gemma lane episodes did. V2's three development lane
failures instead reached the turn cap, so this is not evidence of complete
execution or an early-stopping regression.

## Frozen inputs and scope

A was the installed, trimmed v1 snapshot, SHA-256
`b4617d6d399f58673ebdf96df8fdc69ba4c8d20d373e624b8e00515c21d18924`.
B was [compact-v2](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/agent-usability/runner/candidates/compact-v2/gwz/SKILL.md), SHA-256
`fbedeaa8c19bfbea436b7e328468d2171685509fb3d1afa49773e2c5891302d7`.
Both were frozen before inference under `inputs/comparisons/compact-v2/` on E:.
No live skill, product code, executable, or prior trial was changed.

The released Windows GWZ 1.0.8 binary, corrected docs baseline
`gwz-docs-1.0.8-install-fix-62e70bb`, Qwen/Gemma digests, JSON action interface,
validators and LF fixture configuration stayed fixed. Settings remained
temperature 0, seed 42, `think:false`, context 32,768, output at most 4,096 per
call; each episode had 24 turns, 12,000 generated tokens and ten minutes.
Inference was sequential on native E: using the existing scoped adapter.

New held-outs were frozen before inference: H3 combines the full lane task with
`Shoal Δ Workspace`; H4 combines registration with an unrelated uncommitted
root draft. These are new combinations, not unseen commands. Old held-outs were
not relabeled unseen. `qualify_skill_v2.py` passed correct fixtures and rejected
an overwritten note and accidentally committed draft before any model call.
No candidate/scorer changes followed result inspection. Repeats use the same
seed with fresh paths/contexts and are not independent random samples.

## Restart checkpoint

Comparison and summaries are complete. All raw responses, action ledgers,
fixtures and frozen inputs are durable on E:. Local code is
`scripts/agent-eval/compare_skills_v2.py`, `qualify_skill_v2.py`, and
`summarize_skill_v2.py`. No restart/replay of the campaign is needed.

The independent post-run audit is also complete: 710 generation intents and
responses, 707 fully delivered actions, no pending actions, exact agreement
between raw response usage and all result records, and unchanged frozen
binary/docs/skills/runner/protocol hashes. Two usage-unknown responses and one
other response without a dispatched action remain charged. Audit details are
in remote `post-run-audit.json` and embedded in the local comparison JSON.
Nothing remains to run for this experiment. Do not rerun completed model
episodes. Nothing has been committed or pushed.

## Trace-level attribution correction

Follow-up inspection of `conversation.json` (2026-09-10) replaces the earlier
broad labels with these concrete observations. Paths below are relative to the
remote campaign; no new model runs were performed.

- `repeat-1/P4/gemma4-P4-A`: actions 14–16 supplied only `feather complete` to
  the adapter's write action, whose documented contract is complete replacement.
  The model never successfully read the originals. Actions 22–23 checked status
  and commit history, then action 24 claimed the text had been appended. History
  proved commits existed, not preservation of the previous contents. This is an
  editing-tool misuse and unsupported completion assertion; calling it a GWZ
  skill defect is not justified.
- `repeat-1/P4/gemma4-P4-B`: the initial clone ran from the workspace's parent
  and failed. After correcting that, `--root . add README.md` from inside the
  lane returned `PathEscape`. The model then tried `--root @root`, incorrect
  relative roots, repeated status, and `add --all`; ultimately it staged and
  committed without edits, merged feather into itself, disposed the unchanged
  lane from the parent, and attempted another operation in the deleted lane.
  The PathEscape response to that valid-looking invocation needs a product
  diagnosis; the subsequent incorrect roots and skipped edits are model errors.
- `repeat-2/P4/gemma4-P4-B`: three `--root . add <path>` calls returned PathEscape;
  omitting `--root .` allowed staging. This run did read and preserve baseline
  text (though omitted final newlines), but exhausted turns before completing
  all commits/integration/disposal. It was not just repeated inspection.
- `repeat-3/P4/gemma4-P4-B`: staged before editing, replaced contents, repeatedly
  wrote/read the same README, and merged before committing the actual edits.
  Disposal correctly refused dirty work, then unpreserved history after a later
  commit. The model tried `--force unpreserved-history` instead of merging again;
  the adapter rejected that attempted waiver. Thus v2 did not eliminate the
  original replacement mistake; this must not be hidden by the turn-limit label.
- `repeat-1/P2/qwen3.8-P2-A`: initial registration retries confused cwd-relative
  operands with `--root`. An absolute checkout operand succeeded at action 11.
  The subsequent listing confirmed membership; status showed the preserved edit
  and `lock_match: Differs`. Actions 15–24 were sync, status, read lock, sync,
  status, detach, attach, status, sync, status. All three syncs reported Noop.
  The lock itself already recorded the same HEAD and `dirty: true`. The sequence
  suggests an attempt to repair the Differs signal, but model intent is an
  inference. This is a status/metadata explanation candidate, not simply a
  missing instruction to stop.
- `repeat-3/P2/qwen3.8-P2-B`: after registration succeeded, it similarly read
  status, synced, read repo/add/sync/status help and both managed metadata files,
  and relisted members until the turn cap. No detach/attach in this example.

Actionable GWZ follow-ups: diagnose explicit-relative-root PathEscape; explain
operand bases and include the resolved path in repository-not-found errors;
determine why lock_match is Differs when the saved dirty flag and HEAD agree,
then expose the reason or clarify its meaning. These are distinct from general
editing correctness and unsupported model completion claims. None is fixed by
this report, and no claim is made that they caused every failed episode.
