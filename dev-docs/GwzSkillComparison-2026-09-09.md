# GWZ compact skill comparison — 2026-09-09

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

**The compact candidate is worth adopting for the tested workflows.** Across
56 actual episodes, it preserved Qwen's outcome success and improved Gemma's,
while reducing total reported tokens. It still failed one Gemma editing task,
and did not consistently reduce tool calls or terminate promptly. The live skill
has **not** been changed; this is a candidate and measured recommendation.

Candidate: [compact-v1 SKILL.md](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/agent-usability/runner/candidates/compact-v1/gwz/SKILL.md).
Detailed results, including every failed check and per-cell accounting:
[comparison JSON](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/agent-usability/runs/imported-summaries/GwzSkillComparison-2026-09-09.json).

## Results

Three fresh-context repetitions of four pilot tasks produced 12 episodes per
model/skill. Success means independent repository/file assertions passed;
an agent's completion claim alone never counts. Tokens below are the sum of
Ollama's reported input and output tokens, **including failed episodes**.

| Development tasks | Outcome success | Total tokens | Tokens per successful task | Calls |
| --- | ---: | ---: | ---: | ---: |
| Qwen, original | 12/12 | 928,997 | 77,416 | 154 |
| Qwen, compact | 12/12 | 582,743 | 48,562 | 174 |
| Gemma, original | 7/12 | 485,654 | 69,379 | 83 |
| Gemma, compact | 11/12 | 351,818 | 31,983 | 126 |

Compact reduced development tokens per success by **37% for Qwen** and **54%
for Gemma**. It used more calls: original Gemma sometimes stopped early with an
incorrect success claim. A shorter skill and a better outcome do not imply fewer
actions.

Qwen reached the right state but exhausted 24 turns in three episodes per arm:
original P4 lane disposal in all three repetitions, and compact P2 registration
in all three. Consequently **9/12** Qwen episodes in each arm both passed and
finished cleanly. All Gemma outcome successes finished cleanly.

Held-out variants were frozen before inference and never used to revise the
candidate. Each was run once per model/arm, with new contexts and fixtures:

| Held-out variants | Outcome success | Total tokens | Tokens per successful task | Calls |
| --- | ---: | ---: | ---: | ---: |
| Qwen, original | 2/2 | 94,000 | 47,000 | 17 |
| Qwen, compact | 2/2 | 23,532 | 11,766 | 10 |
| Gemma, original | 2/2 | 53,495 | 26,748 | 10 |
| Gemma, compact | 2/2 | 57,900 | 28,950 | 17 |

All eight held-out episodes passed and finished. Gemma's compact skill cost
**8% more** on these variants; the savings are not universal. The variants were
an inspection task with a spaced/Unicode workspace name and member-directory
starting-context instruction, and detach/reattach with an unrelated root draft
that had to remain uncommitted. These are narrow variations, not unseen commands.

Across development and held-outs together: Qwen original/compact both passed
14/14, using 1,022,997/606,275 tokens; Gemma passed 9/14 versus 13/14, using
539,149/409,718 tokens. Overall: **56 episodes, 591 generation calls, 2,578,139
reported tokens, zero unknown-usage calls, zero infrastructure replacements**.
Failures remain charged. Earlier pilot/calibration costs belong to the
[separate pilot report](GwzAgentUsabilityPilot-2026-09-09.md), not this comparison.

## What improved and what still failed

The original skill's Gemma failures were two P1 status reports and all three P4
lane tasks. Both status failures treated workspace-relative member file paths
as root-owned changes. The compact skill explicitly describes ownership and
JSON status, and passed all three corresponding inspection repetitions.

Original-skill lane traces included staging before editing, overwriting content
instead of appending, and reporting successful disposal despite unmet outcome
checks. Compact passed two of three Gemma lane repetitions. Its remaining
failure (`repeat-3/P4/gemma4-P4-B`) overwrote both member notes instead of
appending. It committed, merged and disposed those wrong contents, then claimed
success. Original Git history remained intact: this is an editing/verification
failure, not evidence of a GWZ merge failure.

Compact Qwen repeatedly inspected/synced an already registered repository until
the turn cap in P2. The operation was correct but stopping behavior remains
inefficient. Do not solve this by removing independent outcome checks or
counting confident final prose as evidence.

## Frozen comparison

Only the skill text differed between arms. Published docs, the released Windows
GWZ 1.0.8 executable, action interface, validators, local LF configuration and
model settings stayed fixed. Native execution used owned directories on Dabeest
E: with a scoped adapter; this is **not OS confinement**. Original CRLF/ReFS
qualification evidence remains preserved and was not normalized away.

| Skill | Bytes | SHA-256 |
| --- | ---: | --- |
| Original | 15,400 | `ee07d82a2975280cf4da16aaa4f5e9570ee8c77655e3efce609f272c97794105` |
| Compact-v1 | 5,253 | `01f0fd6e3f2f35bf2247b343b622840016088be853c0f189e91bcdcdac4ca21b` |

The candidate is 66% smaller. It removes historical version/build/release bulk;
retains workspace/target selection, staging, member lifecycle and complete lane
integration/disposal; explains combined-status ownership; and requires checking
outcomes. Installer guidance matches the corrected docs. Actual 1.0.8 probes
verified that local clone refuses dry-run while add/member detach support it.
Probe evidence is under `qualification/candidate-guidance-152607` on E:.

Baseline is **`gwz-docs-1.0.8-install-fix-62e70bb`**, the named corrected docs
snapshot, not the original 1.0.8-tag documentation. The executable hash remains
`b1921f8a32e6a40ca6885c222efa695607ef93da0101a5be6eda12476363e50a`.
Models were `qwen3.8:27b` and `gemma4:26b`, with the digests recorded in the
baseline/pilot artifacts. Settings: ordinary Ollama `/api/chat`, JSON response
format, non-streaming, `think:false`, temperature 0, seed 42, context 32,768,
output at most 4,096 per call. Episodes allowed 24 turns, 12,000 generated tokens
and ten minutes. Inference ran sequentially.

The candidate was written once using pilot failures, then frozen before this
comparison. No edits followed result inspection. Repeats use the same seed,
fresh fixture paths and contexts; they are **not independent random samples**.
The sample supports this workflow-specific recommendation, not significance
claims, universal model reliability, or a direct Codex-credit saving estimate.

## Artifacts and next decision

All raw requests/responses, action ledgers, Git fixtures, original/candidate
snapshots and the frozen manifest remain at
`E:\gwz-agent-eval\runs\skill-compact-v1`. `post-run-audit.json` independently
recounted 591 generation intents/responses and 591 fully delivered actions,
matched usage against every result, found no pending actions, and rechecked
binary/docs/skill hashes. The reusable comparison and summarizer are in
`scripts/agent-eval/compare_skills.py` and `summarize_skill_comparison.py`.

Adopt this candidate if the measured tradeoff is acceptable, retaining external
outcome validation for autonomous mutation. Live installation remains a separate
user decision. Broader command/Python API coverage remains the scope in
[the coverage manifest](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/agent-usability/runs/imported-summaries/GwzAgentUsabilityCoverage.json); this experiment does not
cover it. Any next candidate must use new held-outs rather than relabeling these
observed results as unseen.
