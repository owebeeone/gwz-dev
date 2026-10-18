# App restart checkpoint — 2026-09-10

Campaign evidence and experiment runners referenced here moved to the private [GWZ archive](https://github.com/owebeeone/gwz-core-evidence) on 2026-09-10. Recorded paths and commands remain historical; see the archive README and REPLAY.md for current locations and fresh-run setup.

User requested pause to restart the app (reported 15 GB app RAM).

## Compact help work: uncommitted and UNVALIDATED

User authorized compact top-level help and structured `gwz --json help`.
- New `gwz-cli/src/help.rs`: compact root help; Clap-based explicit help routing;
  parser-derived JSON command/options/arguments metadata, optional nested topics.
- Modified `gwz-cli/src/globalargs/parser.rs`: override root help; retain old long
  descriptions for now; test-only CommandFactory import replaced with qualified use.
- Modified `gwz-cli/src/lib.rs`: use help::parse_from before workspace startup.
- Modified `gwz-cli/src/tests/g00.rs` and tests/mod.rs: focused help tests and Parser
  import. Existing generated docs drift check needs regeneration after validation.
- Tests written first but local compilation failed for disk exhaustion, before
  testing help. Second attempt with no core debug info also failed for disk space.
- `cargo clean -p gwz-core` removed disposable build output, but APFS sharing meant
  little physical space recovered. Do not claim tests pass.
- A source snapshot was streamed to E:/gwz-help-eval-20260910 on Dabeest using
  tracked source files from CLI/core/taut-shape-rs plus new help.rs and root Cargo
  manifests. No remote build launched yet. Source copy completed successfully; ready for remote validation.
- Next: run focused CLI g00 tests there, correct compilation/behavior, regenerate
  docs/CLI.md, validate real binary help exits/JSON/topics and normal CLI parsing.
  Consider JSON option arity/repetition/default accuracy; no model help trial yet.
- Preserve unrelated CLI docs changes and the other agent's NTFS work. No commit,
  push, release, or installed binary replacement authorized/performed here.

## Local LLM skill trial

Agent /root/gwz_local_model_evaluation is running a detached campaign on Dabeest:
E:/gwz-agent-eval/runs/skill-compact-v2, PID 2132 at launch;
runner E:/gwz-agent-eval/runner/compare_skills_v2.py.
56 primary cells plus predeclared infrastructure replacements. Latest report:
52 primary cells complete; all Qwen held-outs passed, Gemma development lane
failed all three repeats in both arms. Two Gemma original-arm calls omitted
usage and require separately charged replacements. Campaign has since completed: 58 episodes including two replacements, 710 calls, at least 2,221,029 tokens (two unknown-usage responses). Final report/audit is being written; per-cell results are saved in GwzSkillV2Comparison-2026-09-10.json.
Do not change its binary/docs/skills/runner mid-campaign.

Baseline installed skill hash:
b4617d6d399f58673ebdf96df8fdc69ba4c8d20d373e624b8e00515c21d18924
Candidate scripts/agent-eval/candidates/compact-v2/gwz/SKILL.md hash:
fbedeaa8c19bfbea436b7e328468d2171685509fb3d1afa49773e2c5891302d7
Live skill remains installed compact-v1 with installer/version trims.

Dabeest SSH is gianni@10.1.1.239, key ~/.ssh/id_rsa.no, -F /dev/null.
Use explicit /c/Program Files/Git/bin/bash.exe through SSH stdin for scripts,
Python argument arrays for native Windows tools. E: is separate from the Mac.
