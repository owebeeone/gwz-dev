# GWZ agent usability pilot: Qwen and Gemma

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

**The pilot ran on Dabeest E: and produced usable results. The existing skill
helped Qwen complete the lane workflow, but did not keep total context use down.**
Gemma performed worse with the skill in this small sample. This is a starting
point for improving the guide, not a general model ranking or a completed
evaluation of all GWZ commands.

## Frozen inputs and execution

- Released Windows GWZ **1.0.8**, executable SHA-256
  `b1921f8a32e6a40ca6885c222efa695607ef93da0101a5be6eda12476363e50a`.
- Documentation baseline **`gwz-docs-1.0.8-install-fix-62e70bb`**; existing skill
  snapshot SHA-256 `ee07d82a2975280cf4da16aaa4f5e9570ee8c77655e3efce609f272c97794105`.
- Qwen `qwen3.8:27b` and Gemma `gemma4:26b`, with the previously frozen model
  digests. Ollama `0.33.0-dabeest`; `think:false`, `stream:false`, context 32768,
  response limit 4096, temperature 0, seed 42. Final pilot adds `format:"json"`.
- Native Windows execution, explicitly authorized after abandoning Sandbox.
  Scoped GWZ/file adapter, fresh repositories and home/configuration per episode,
  no shell, external remotes/credentials, or force waivers. **Not OS confinement.**
- Four tasks × two models × two conditions: **16 scored cells**. One incomplete
  provider response required a fresh replacement: **17 actual final episodes**.
- One episode per scored cell, maximum 24 turns, 12,000 generated tokens, ten
  minutes. Fresh context and equivalent state; condition order alternated by task.
  These are not independent repetitions or held-out tests.

## Results

| Model | Condition | Tasks passed | Model calls | Input tokens | Output tokens | Total tokens |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| Qwen | Docs/help | 3/4 | 70 | 249,081 | 1,657 | 250,738 |
| Qwen | Docs/help + skill | 4/4 | 53 | 323,712 | 3,463 | 327,175 |
| Gemma | Docs/help | 2/4 | 46 | 94,841 | 1,943 | 96,784 |
| Gemma | Docs/help + skill | 1/4 | 43 | ≥247,715 | ≥3,070 | ≥250,785 |

Input tokens are provider-reported prompt counts accumulated across calls,
including repeated conversation history. They are not unique documentation
tokens. Gemma+skill includes its replacement and charges the original incomplete
generation as one call with **unknown** usage; its token totals are lower bounds.
Failed tasks remain in every total.

Qwen used 24% fewer calls with the skill but **30% more total tokens**. Including
the cost of failed tasks, tokens per successful task were about 83,579 without
the skill and 81,794 with it. That small difference does not establish a robust
efficiency advantage. Gemma's skill condition cost over twice as many known
tokens and passed fewer tasks. Do not infer statistical significance from four
tasks per condition.

| Task | Qwen docs | Qwen skill | Gemma docs | Gemma skill |
| --- | --- | --- | --- | --- |
| P1: identify repository ownership and staged/unstaged/untracked changes | Pass | Pass | Fail | Fail |
| P2: register existing repo, preserve its uncommitted work | Pass* | Pass | Pass | Pass |
| P3: detach retiring member; reattach existing inactive designation | Pass | Pass | Pass | Fail |
| P4: lane → edit root and two members → commit → integrate → dispose | Fail | Pass | Fail | Fail |

\* Qwen completed the P2 state correctly but continued inspecting until the
24-turn limit. State validation passed; it did not issue a finish action.

The final episode times sum to about 226 seconds, including model calls and
actions but excluding fixture preparation and earlier calibration. Loading and
caching affect those times; this was not a model throughput benchmark.

## Concrete findings

1. **Combined status needs an explicit ownership explanation.** Gemma reported
   `components/blue/note.txt` and `components/amber/new.txt` as files owned by the
   root repository in both conditions. It correctly identified their change
   states and left the workspace untouched. A compact guide should explain that
   combined status paths are workspace-relative, and use member inventory/machine
   output when identifying which repository owns each path.
2. **Completion checks matter more than fluent summaries.** Gemma claimed P3
   retirement was done although the retiring designation remained active. In P4
   it claimed completion despite missing committed contents or a surviving lane.
   Independent manifest, file, Git commit/ancestry, family, and merge-state checks
   caught those errors. Its own final summary is not a validator.
3. **The lane sequence is useful skill content.** Qwen with the skill completed
   the entire P4 outcome, including preserving all three imported lane commits.
   Without it, Qwen completed and integrated the changes but exhausted its turn
   budget before disposal. Gemma's attempts show that mentioning the sequence
   alone does not guarantee correct execution of every step.
4. **The current skill is too expensive to assume it is an optimization.** Its
   15,400-byte entry includes historical versions, superseded installation advice,
   platform notes, and gwz-dev-specific build/release guidance. The next candidate
   should retain current command/target/lifecycle guidance and measured completion
   checks, while moving uncommon detail to references. No candidate benefit has
   been measured yet; the live skill was not edited.
5. **A Windows line-ending failure surfaced during qualification.** The original
   native fixture hit `MergeDrift` at root integration after managed metadata
   changed from LF to CRLF. Explicit per-repository `core.autocrlf=false` and
   `core.eol=lf` made the same released executable complete the workflow. Keep this
   as a product/configuration investigation, not a model/docs failure. Evidence:
   `E:\gwz-agent-eval\qualification\native-20260909-144606`.

## Harness calibration and limitations

Two earlier 16-cell calibration passes are retained and excluded from the final
paired success table. They exposed:

- Qwen responses triggering Ollama's XML tool parser despite ordinary chat use:
  HTTP 500, `XML syntax error ... unexpected end element </parameter>`.
  Consistent JSON response mode removed these errors in the final Qwen cells.
  [Ollama documents JSON response format](https://docs.ollama.com/capabilities/structured-outputs).
- Explicit UTF-8 decoding needed on Windows; legitimate help/branch inspection
  needed to reach GWZ rather than being rejected by the adapter.
- A validator incorrectly rejected metadata-only root commits, although the task
  only forbade committing user work. It now verifies preserved history and that
  any new root commit changes only managed metadata.
- A redundant P4 check confused `cwd=<lane>` with equally valid `--root <lane>`.
  It was removed. The final score instead checks imported commits, their exact
  contents and ancestry, removed lane registration/directory, and closed merge.
  Original scores remain intact alongside separately hashed revalidation results.

Across calibration and final work: **49 episodes, 487 generation calls, at least
1,997,999 reported input/output tokens**, and ten calls with unknown usage. The
first calibration did not retain HTTP error bodies; later passes do. Every
generation intent survives in its ledger. These are local inference metrics,
**not Codex credit savings**.

Ten harness checks passed on native Windows, and deterministic task solutions
passed positive/negative validation. Local clone reported 106 native copies and
zero ordinary copies on ReFS in qualification. Ledger crash-boundary tests are
synthetic; actual GWZ process-crash recovery is not yet evaluated. The pilot also
does not test networking, arbitrary execution, installation, or Python surfaces.

## Evidence and next work

- [Compact results and accounting](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/agent-usability/runs/imported-summaries/GwzAgentUsabilityPilot-2026-09-09.json).
- [Coverage manifest](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/agent-usability/runs/imported-summaries/GwzAgentUsabilityCoverage.json): 33 CLI leaf commands and
  47 public Python `Client` definitions (including one property). The pilot maps
  to subsets of ten CLI entries; that is not full option/failure-path coverage.
- Harness: `scripts/agent-eval/`. Full transcripts, ledgers, fixtures, frozen
  runner and revalidation: `E:\gwz-agent-eval\runs\native-pilot3-json-20260909`.
  Replacement: `E:\gwz-agent-eval\runs\native-json-infra-replacement1`.

Next: draft a compact revision of the existing skill, test the ownership and
completion guidance on these tasks and fresh variants, and expand the remaining
command/API catalog in bounded batches. Repetitions and held-out variants remain
necessary before promoting a candidate as an improvement. No release, commit,
push, live skill change, or full-coverage claim was made by this experiment.
