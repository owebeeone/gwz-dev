# GWZ local-model evaluation execution checkpoint

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

Latest checkpoint, 2026-09-10: **compact-v2 comparison and independent audit
complete**, 56 scored cells / 58 actual episodes. See
[the v2 report](GwzSkillV2Comparison-2026-09-10.md). No inference or accounting
work remains for that campaign; the installed skill is unchanged. Results are
mixed, so automatic promotion is not recommended. Earlier execution history
follows.

Date: 2026-09-09. **Pilot complete: 16 scored cells plus one infrastructure
replacement.** See [the final pilot report](GwzAgentUsabilityPilot-2026-09-09.md)
for results and remaining coverage. Sandbox was superseded by explicit user
instruction; the following sections preserve the execution history.

## Compact skill comparison complete

**56/56 episodes completed.** See the
[final comparison report](GwzSkillComparison-2026-09-09.md) and
[per-cell accounting](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/agent-usability/runs/imported-summaries/GwzSkillComparison-2026-09-09.json). Development outcome
success: Qwen 12/12 for both skills, Gemma 7/12 original versus 11/12 compact.
Held-outs: 8/8 passed. Total 591 calls and 2,578,139 reported tokens, no unknown
usage or infrastructure replacements. Six Qwen episodes reached the correct
outcome at the turn limit; the report separates this from clean completion.

The user authorized one compact candidate of the existing skill, retained only
at `scripts/agent-eval/candidates/compact-v1/gwz/SKILL.md`; the live skill is
unchanged. Its frozen SHA-256 is
`01f0fd6e3f2f35bf2247b343b622840016088be853c0f189e91bcdcdac4ca21b`
(5,253 bytes, versus the original's 15,400). Candidate changes remove historical
build/release material and explain combined-status ownership, staging, complete
lane integration/disposal and outcome verification. Installer and dry-run
guidance was checked against corrected docs and the pinned released binary.

Actual comparison runs at `E:\gwz-agent-eval\runs\skill-compact-v1`, using
`scripts/agent-eval/compare_skills.py`: three repetitions of all four tasks with
both models and both skills (48 episodes), then two held-out variations across
both models/skills (8). Repeats use the
same seed and fresh contexts/fixtures, not independent random seeds.

The corrected docs, released Windows 1.0.8 binary, model settings, action
interface, validators and LF fixture configuration remain fixed. No candidate
edits follow result inspection. Every generation is retained; at most one fresh
replacement per infrastructure-failed cell is separately recorded and charged.
Reaching the correct state at a turn limit is distinguished from clean agent
completion. This comparison does not expand the pilot to full CLI/Python API
coverage.

## Native pilot launch and first result (historical)

The user instructed “give up on sandbox - e: drive.” The experiment now uses the
scoped native adapter in `scripts/agent-eval/native_runner.py`. This is **not OS
confinement**: the model gets GWZ-only command execution and scoped file actions;
no shell/generated code, external push/pull/auth operations, identity overrides,
force waivers, or manual managed-metadata edits. Model action paths resolve within
the current trial or frozen docs, with a junction escape check. The host runner
retains ledgers/validators outside model-accessible trial paths.

Native qualification passed at
`E:\gwz-agent-eval\qualification\native-20260909-144827`: real init, local clone,
root-plus-two-member edit/commit/merge/dispose, exact committed contents and
preserved original history, plus positive/negative validators for all four pilot
tasks. A clone qualification reported **106 native copies, zero ordinary copies**.
Six additional practical guardrail checks passed, including junction rejection
and timeout termination of a trusted parent/child/grandchild process tree.

The initial qualification without explicit local line-ending configuration
failed root integration with `MergeDrift`: managed metadata changed from LF to
CRLF during checkout. Pinning each fixture's `core.autocrlf=false` and
`core.eol=lf` made the full lifecycle pass. This configuration-sensitive finding
is preserved under `qualification/native-20260909-144606`; it is not scored as a
model failure, nor silently fixed in the released executable.

Actual campaign: `E:\gwz-agent-eval\runs\native-pilot-20260909`, with log
`native-pilot-20260909.log`. Qwen runs before Gemma, one generation at a time.
The first Qwen docs-only request received Ollama HTTP 500 before any tool action;
the generation remains charged with unknown token usage. A separate replacement
is required for that infrastructure-failed cell.

**First successful actual result:** Qwen plus frozen skill passed P1 inspection
and selection in four turns, 11.1 seconds, and 388 generated tokens. Independent
Git and file checks confirmed the exact report and unchanged workspace state.
This is one episode, not evidence that skill is better; paired results follow
after the complete pilot. The authoritative current counts are the campaign's
per-episode `result.json` records, not this interim paragraph.

## After the authorized reboot: desktop launch qualification pending

The root agent completed the user-authorized reboot, verified Windows boot time
`2026-09-09T14:34:32.5000000+10:00`, and resumed this background task.
`WindowsSandbox.exe` is now present and Ollama responds normally.

A minimal `.wsb` smoke mapped one empty trial directory writable and one bootstrap
directory read-only, disabled networking/vGPU/clipboard/audio/video/printer
redirection, and asked the guest to write a boot marker. The launcher started as
PID 4612 in SSH **session 0**, but no boot marker appeared after 40 seconds.
`hcsdiag list` showed no sandbox compute system. `query user` reported no
interactive user session; no `wsb.exe` headless CLI or Sandbox app package was
found. The installed launcher version is `10.0.26100.1`.

The next practical qualification step is **a normal interactive desktop login on
Dabeest**, then launching the probe in that session. This is a launch prerequisite
to test, not a claim that login will resolve every executor requirement. The
owned stalled probe was stopped; no existing workload was stopped. No additional
reboot is indicated.

Evidence and the exact `.wsb` probe live under
`E:\gwz-agent-eval\qualification\sandbox-smoke`.
`post-reboot-launch.json` records this state. **Actual model episodes: 0 of 16.**
The six protocol/accounting checks remain synthetic, not trials.

The four model-neutral pilot specifications are now in
`scripts/agent-eval/pilot-tasks.json`, with explicit independent assertions.
Fixture builders and executable validators remain to implement after the actual
sandbox storage/execution path works.

## Resumed setup: reboot now required

On the user's subsequent request to start the trials in the background, attempted
the authorized non-restarting feature setup:

```powershell
Enable-WindowsOptionalFeature -Online -FeatureName Containers-DisposableClientVM -All -NoRestart
```

The command exited **0**, reporting `Online: true`, **`RestartNeeded: true`**,
and `Restart is suppressed because NoRestart is specified.` A subsequent query
reports the feature as **Enabled**, while `WindowsSandbox.exe` remains absent.
The requested restart has not happened. **Dabeest needs a user-approved reboot before executor
qualification can proceed. No reboot was performed.**

Exact command output is retained in
`E:\gwz-agent-eval\qualification\sandbox-enable.stdout.txt` and
`sandbox-enable.stderr.txt`; `preflight.json` records the pending-restart state.
No inference calls were made. After reboot, first recheck feature/executable
availability and qualify unattended launch; do not treat installation as proof
that confinement or GWZ operations work.

## Executor prerequisite

Dabeest (`gianni@10.1.1.239`) reports:

- `Containers-DisposableClientVM`: **Disabled**.
- `C:\Windows\System32\WindowsSandbox.exe`: absent.
- SSH's Windows token is elevated; this is not an isolation boundary.
- A hypervisor and `vmcompute` are active; Ubuntu WSL2 is running.
- No `Get-VM` command, VirtualBox executable, or VMware runner was found in the
  probed standard locations. This does not prove no VM software exists elsewhere.

Windows Sandbox must be enabled and usable, or a disposable Windows VM supplied,
before the planned Windows pilot can execute. Enabling the Windows feature may
require a reboot; no feature changes or reboot were attempted. Existing WSL2
does not by itself run the pinned native Windows binary inside the required
boundary. No unrestricted model-generated commands were run as `gianni`.

Once available, first verify unattended/SSH-session launch, the read-only tools
mapping and one writable trial mapping, disabled network/clipboard/vGPU,
containment canaries, reparse-point handling, and normal GWZ filesystem behavior.
A feature being enabled is not a completed qualification.

## Prepared on E:

`E:\gwz-agent-eval` now exists on **ReFS** with roughly 982 GB free at preflight.
Only evaluation-owned paths were created. Frozen inputs were transferred and
their hashes verified:

| Input | SHA-256 |
| --- | --- |
| `inputs/docs.tar` | `1a41e445cd99bf72d3e062abddf62de427cd7cc1e39f083cb7f9ddfe4256db11` |
| `inputs/SKILL.md` | `ee07d82a2975280cf4da16aaa4f5e9570ee8c77655e3efce609f272c97794105` |
| `inputs/gwz-x86_64-pc-windows-msvc.zip` | `a6febb5b45ca4b5623bd86190a889ec9eaef5ddd5572ba99da44d20875e40d85` |
| `tools/gwz.exe` | `b1921f8a32e6a40ca6885c222efa695607ef93da0101a5be6eda12476363e50a` |
| `inputs/sdax-runner.py` | `2d03ec70d6778d37edd1149ed61c385b068a9302e1a1b60f5b0b0e9f5dbcda4a` |

Baseline remains **`gwz-docs-1.0.8-install-fix-62e70bb`**, the corrected
documentation snapshot, not original v1.0.8-tag docs. `--version` returned
`gwz 1.0.8`; `--build-info` confirmed CLI
`4f31f0b0ae042caed506a7d2564b753f5f657049` and core
`ec6d1a5749c379fbd78d510c160b372f79bdda83`, both clean.

Preflight and synthetic evidence are in `E:\gwz-agent-eval\qualification`:
`preflight.json`, `protocol-tests.txt`, and captured help/build-info output.
The complete sdax runner was copied for reuse without modifying its experiment.

Ollama server reports `0.33.0-dabeest`. Installed model identities match handoff:

| Model | Digest |
| --- | --- |
| `qwen3.8:27b` | `22130167c4c20e20c7b71454612966ca8e8171e9b3cc8ab6ce8aa6cbfec79643` |
| `gemma4:26b` | `08ae7ec1744bd7f451c4a530afb39d2673ad9d07a8369b8a33a3613b41212a68` |

Qwen was loaded during preflight. GPU availability must still be checked before
starting inference; this checkpoint does not reserve it.

## Implemented and checked

Reusable code: `scripts/agent-eval/protocol.py` and `test_protocol.py`, also copied
to `E:\gwz-agent-eval\runner`. Six synthetic tests passed on macOS and native
Windows Python (Windows elapsed 0.047 seconds):

1. Malformed JSON, duplicate keys, unknown actions, wrong field types rejected.
2. Spaces, Unicode, empty arguments, quotes, shell-looking text, and trailing
   backslashes preserved by parser plus Python `shell=False` argument lists.
3. Simulated interruption at action-ledger boundaries: prepared work distinguishable
   from uncertain dispatch; uncertain mutation is not authorized for replay;
   completed result remains available for delivery without execution.
4. Reused action IDs with changed payload/workspace and illegal transitions rejected.
5. Valid, malformed, timeout/lost/interrupted generations all retained in accounting;
   malformed raw response survives and missing token usage stays unknown.
6. A torn ledger fails closed instead of forgetting a possible dispatch.

These are primitive checks, **not executor qualification**. There is deliberately
no host command executor wired to model output. The full action adapter, OS
confinement, independent fixture validators, descendant timeout handling,
cross-boundary receipts, and actual GWZ crash/recovery tests remain to implement.
The current ledger conservatively stops uncertain actions; receipt reconciliation
is still future work. No with/without-skill result can be inferred from these tests.

## Next execution steps

1. Resolve Windows executor availability without disrupting other host work.
2. Implement the guest bridge and adapter, then run the entire deterministic
   qualification matrix from the plan, including real GWZ lifecycle operations.
3. Build the four pilot fixtures and trusted positive/negative validators.
4. Run four tasks × two conditions × two models = **16 pilot episodes**, sequential
   inference, fresh state/context, frozen inputs and bounded calls/time.
5. Publish measured successes/failures/accounting; expand the CLI/Python coverage
   manifest and batches only after the pilot is trustworthy.

No commit, push, release, live-skill update, model-service change, or production
GWZ installation replacement was performed by this background task. The later
authorized Sandbox feature enablement is recorded above; its reboot is pending.
