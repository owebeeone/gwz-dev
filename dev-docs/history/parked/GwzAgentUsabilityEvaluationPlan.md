# GWZ documentation and skill evaluation plan

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

Date: 2026-09-09. Status: native E: pilot complete (16 scored cells plus one infrastructure replacement); see GwzAgentUsabilityPilot-2026-09-09.md. Full coverage, candidate skill, and held-out evaluation remain open.

## Outcome

Use local models to perform realistic GWZ tasks, independently check their work,
and improve documentation, the existing skill, error messages, and API gaps
according to observed failures. Cover the entire supported surface, including
inspection, member lifecycle, and local-family lifecycle—not just clone/merge.

The main deliverable is a compact, reliable GWZ skill that reduces **total
context consumed to complete work**. Successful use without the skill remains a
separate goal. This is a usability evaluation, not a replacement for product
tests or a new release gate. This document authorizes no implementation claim:
the harness, task catalog, measurements, and skill changes are still to do.

## Location and integration

- Run the harness, tools, GWZ fixtures, and local-model inference on **Dabeest**.
  Proposed evaluation home: `E:\gwz-agent-eval` (`/e/gwz-agent-eval` in MinGW).
  Keep it separate from existing development workspaces and the model store.
- SSH is `gianni@dabeest` / `gianni@10.1.1.239`. For remote orchestration,
  send scripts through SSH stdin to explicit Git/MinGW Bash. Invoke native
  Windows tools from Python argument lists (`shell=False`), rather than nested
  quoted PowerShell command strings. A PowerShell installation task still tests
  the documented PowerShell command inside its disposable execution environment.
  The final shell claim in `/Users/owebeeone/limbo/DABEEST-CLIENT.md` is stale.
- sdax reports no inference running at handoff. Use loopback Ollama `/api/chat`,
  ordinary non-streaming responses, `think:false`, context setting `32768`, and
  output setting `4096`. These are tested settings, not verified model maxima.
  Pilot model: `qwen3.8:27b`; `gemma4:26b` is the later comparison model. Capture
  the actual model digest and server version before the first scored request.
- Start with one model and one inference episode at a time on the shared GPU.
  Check current occupancy before starting; the handoff is not a permanent GPU
  reservation. Avoid concurrent release builds and filesystem benchmarks during
  timing measurements. Do not change existing model services.
- Pin the executable, core/Python versions, binary hashes, docs snapshot, skill
  snapshot, runner revision, and model/configuration for each campaign. Start
  with released behavior; keep candidate product builds in separate campaigns.
- Verify the E: filesystem and GWZ native-copy capability. Keep all fixtures,
  remotes, lanes, temporary homes, and run output there. Copy-on-write is an
  optimization; each episode must have independent writable state. No hardlinks
  or shared writable Git stores. Keep fixture sources quiet during cloning.

### Handoff received: reusable components and new work

The supplied harness is `D:\sdax-exp2\exp4\runner\runner.py`. It compiles generated
Rust in separate directories; that is **not OS confinement**. Reuse its frozen
inputs, durable call accounting, interrupted-run reconciliation, and independent
scoring. It uses ordinary Ollama `/api/chat` responses, not native tool calling.
It is not an existing interactive shell-agent harness.

sdax's interrupted-run machinery accounts for **generation calls**. GWZ also
needs a separate durable **action ledger** for filesystem/process mutations;
generation reconciliation alone does not make command replay safe.

Before implementation, read that runner and capture its revision/hash, one example
input/result pair, and launch command. Port or import its reusable components
into the E: evaluation project without modifying the completed sdax experiment.
The new GWZ work is the interactive action adapter, native executor, fixture
builders, and state assertions. Missing usage fields remain unavailable, not zero.

### Decision 1: native Windows execution on E: (supersedes Sandbox)

The user explicitly instructed **“give up on sandbox - e: drive”** after the
Sandbox setup/desktop-launch delays. Run the pilot natively under the existing
Windows account in disposable `E:\gwz-agent-eval` trial directories. Windows
Sandbox, VM isolation, desktop login, and further reboots are not prerequisites.
The earlier OS-confinement requirement in this plan is superseded.

Use a scoped adapter with explicit fixture cwd and pinned absolute GWZ path,
Python `shell=False` argument lists, fresh per-trial Git/home configuration, and
path resolution including reparse-point checks. The pilot offers GWZ command
execution plus read/list/search/write file actions. It provides no arbitrary
shell/generated-code execution, network operations, credential operations, or
force waivers. Model edits to `.git`, `.gwz`, and `gwz.conf` are refused; structural
changes use GWZ. Restrict reads/writes/cleanup to the current trial and frozen docs.
These are practical guardrails, **not an OS sandbox or a security guarantee**.

Native E: enables measurement of actual ReFS copying. Explicit local
`core.autocrlf=false` and `core.eol=lf` pin fixture behavior across Git/libgit2;
the first qualification exposed managed-metadata CRLF drift without this pin,
which remains a separately recorded Windows configuration finding.

Future shell/forall, authentication/network, and Python API coverage require
separately scoped adapter capabilities. They are explicit pilot gaps, not reasons
to block the four native CLI pilot tasks. Test the exact public installer command
separately as a trusted smoke test; do not grant arbitrary external pushes or
ambient SSH credentials to the task model.

### Decision 2: ordinary responses carrying JSON actions

Use sdax's tested `/api/chat` interface with Ollama `format: "json"` for the
native pilot. Unconstrained Qwen responses triggered server-side XML tool-parser
HTTP 500 errors during calibration; JSON format avoids confusing those
infrastructure failures with documentation failures. This remains ordinary chat
response parsing, not native tool calling. Ask for one JSON action per response,
validate it, execute it, and append a bounded structured result before the next
call. Do not rely on native tool calling or execute arbitrary response prose.
Pilot operations: read/list/search docs or workspace, write a workspace file,
execute a scoped GWZ argv vector, and finish with a short result summary. Example:

```json
{"action":"exec","cwd":"trial/workspace","argv":["gwz","status"]}
```

Resolve executable aliases to pinned tool paths; use Python argument lists and
`shell=False` in the native executor. Return exit code, stdout, stderr,
timeout, and explicit truncation indicators. Preserve raw command semantics:
no hidden target selection, argument correction, recovery, or command suggestions
in the adapter. Use the same action descriptions in both skill conditions.

Reject malformed actions with a consistent parser error; they consume a model
turn and the attempt budget. Record adapter-format failures separately from GWZ
failures. Commands needing shell semantics, `forall` subprocesses, and generated
Python API scripts are outside the initial native adapter and remain coverage gaps.
An argv allowlist alone is not confinement. Set subprocess timeouts and stop descendants when an attempt
ends. Qualify file paths, reparse points, and request/result handling with
negative checks before model execution.

### Durable action ledger and interruption recovery

Keep the authoritative ledger outside model-tool-accessible trial paths. Each
action records run/attempt ID, originating generation ID, unique action ID,
normalized payload/hash, workspace identity, lifecycle state, and result/evidence
references. Execute actions serially per attempt. Persist and flush transitions
before sending a command and before advancing the conversation:

1. `prepared`: validated action durably recorded; nothing dispatched yet.
2. `dispatch_intent`: durably recorded **before** sending it to the executor.
3. `completed`: result and relevant post-action evidence durably saved.
4. `delivered`: the saved result is included in durable conversation history.

The executor must reject reused IDs with different payloads. With a durable
receipt for the same ID, return the saved result instead of executing again.
After a restart, `prepared` may be dispatched only if the ordering contract proves
it was never sent. `completed` needs result delivery, not re-execution; deduplicate
that delivery by action ID. A missing completion after `dispatch_intent` is
**uncertain**, even if the process has disappeared or timed out.

For uncertain actions, stop dispatch, terminate any surviving descendants, and
inspect executor receipts plus independent fixture state. Resume only when the
outcome can be established without repeating the mutation. Otherwise mark the
episode infrastructure-indeterminate, preserve its evidence/accounting, and use
a fresh fixture and attempt ID for a replacement episode. Never automatically
rerun commit, merge, detach, dispose, or another uncertain mutation. Record such
replacements separately; bound them rather than retrying indefinitely.

This provides conservative recovery, not a claim of atomic exactly-once execution
across a ledger and arbitrary Git/filesystem changes. Validator outcomes stay
runner-controlled; an executor success receipt is supporting evidence only.

### Executor qualification: native pilot requirements

Run deterministic scripts through the actual adapter/executor path first. Save a
small qualification report with executor configuration, pinned inputs, observed
results, and explicit failures. No inference is necessary for these checks.

| Check | Required evidence |
| --- | --- |
| Scoped adapter | Reject outside-trial paths, managed metadata writes, shell executables, and junction/reparse-point escapes before access. Do not describe these checks as OS confinement. |
| Network capabilities | Native pilot adapter refuses external remote URLs, push/pull/auth/shell operations and identity overrides. No claim of host network isolation. |
| Real GWZ cycle | Initialize, URL-clone from a fixture remote, local-clone, edit, stage/commit, merge complete root/member history, and dispose. Verify state independently and confirm no surviving unpreserved work was deleted. |
| Storage | Record native E: ReFS behavior, native versus fallback copy counts, and independence after editing cloned files. |
| Action parsing/arguments | Reject malformed JSON, unknown actions, and invalid payload types without execution. Verify spaces, Unicode, empty arguments, quotes, and argument boundaries using a recording executable. |
| Timeouts/processes | An action spawning a child and grandchild times out; all descendants stop and a delayed write never occurs. Restart does not leave an old executor running against a reused fixture. |
| Action interruption | Inject a crash before dispatch, after execution but before saving the result, and after saving but before delivery. Prove committed history/files are not mutated twice; uncertain cases stop rather than replay. |
| Accounting | Replay recorded/fake provider exchanges through accounting: valid, malformed, timed-out, lost-response, and interrupted calls are all represented; available raw responses survive parse failure; no generation disappears from totals. |

Qualify with fixed synthetic responses before spending model tokens. A failed
qualification is an executor/harness finding, not evidence against the docs,
skill, or model. Keep corrections local to this harness; no release-suite work.

### Decision 3: exact starting build and documentation

The pilot uses the **released Windows x86_64 GWZ v1.0.8**, not `gwz-fixed`, `main`,
a rebuild, or a moving `latest` asset:

- Archive: `https://github.com/owebeeone/gwz-cli/releases/download/v1.0.8/gwz-x86_64-pc-windows-msvc.zip`
- Archive SHA-256 from the release asset metadata:
  `a6febb5b45ca4b5623bd86190a889ec9eaef5ddd5572ba99da44d20875e40d85`.
- CLI tag commit: `4f31f0b0ae042caed506a7d2564b753f5f657049`.
- Core tag commit: `ec6d1a5749c379fbd78d510c160b372f79bdda83`.
- Later Python phase: package `gwz==1.0.8`, tag commit
  `31148763e92851b9f9a29791067c34c139065808`; freeze the Windows wheel filename,
  hash, interpreter version, and dependencies before preparing API fixtures.
- CLI published docs snapshot: commit
  `62e70bb70e5bff67a96d889564eab626f90d6242`, which corrects the installer command
  without changing v1.0.8 behavior. Baseline name:
  **`gwz-docs-1.0.8-install-fix-62e70bb`**. This is not the original v1.0.8-tag
  documentation. All reports must name this corrected snapshot explicitly.

The corrected docs and current skill are now frozen locally under
`scratch/agent-usability-baselines/gwz-docs-1.0.8-install-fix-62e70bb/`.
[GwzAgentUsabilityBaseline.json](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/agent-usability/runs/imported-summaries/GwzAgentUsabilityBaseline.json) records the
source commit, included paths, and SHA-256 hashes of `docs.tar` and `SKILL.md`.
The bundles are outside version control; the small manifest is intended to be
tracked. Transfer these exact bytes to E: and verify hashes before execution.
Never replace this skill snapshot with the live skill file during a resumed run.

Validate archive bytes before extraction, then record executable hash and
`--version`/`--build-info`. Build fixtures and supported/refused-operation
expectations against that executable. The literal `latest` install smoke must
record the version it resolves to; if it changes, it is no longer the pinned
v1.0.8 trial. Candidate builds never silently replace the baseline.

## Controlled comparisons

For every task, compare two conditions:

| Condition | Model-visible resources |
| --- | --- |
| Docs only | Frozen published documentation, installed help, task and fixture |
| Docs + skill | Exactly the same resources, plus the existing GWZ skill and its referenced files |

Use fresh conversations and fresh equivalent fixtures for every condition and
repeat. Alternate condition order to reduce model warm-up/cache effects. Match
model settings, tools, feedback, and budgets. Record random seeds when supported;
do not assume they make generation deterministic.

Prevent accidental skill loading through the runner's normal user profile,
ancestor instructions, memory, or tool descriptions. Put runner configuration
outside the development workspace. GWZ-generated bootstrap instructions are
part of actual product behavior: make them equally available in both conditions
and record whether they were read. Do not give docs-only runs a disguised copy
of the skill in the system prompt.

Keep product source, evaluator assertions, gold solutions, previous transcripts,
and other runs inaccessible to the task agent. Permit reading the frozen docs
on demand rather than stuffing every page into the initial prompt. Keep search,
output limits, and help access identical between conditions.

Evaluate three surfaces separately: Rust CLI, Python CLI, and Python `Client` API.
Reuse semantic tasks and state validators across them. API tasks must actually
use the public Python API; shelling out to the CLI is not an API success. The
current skill is CLI-focused; record that limitation rather than silently giving
Python runs extra instructions. Extend the same skill with referenced Python
guidance if the evaluation warrants it.

## Task coverage

Build an inventory from the pinned binary's recursive help and the public Python
surface. Starting sources are `gwz-cli/docs/commands/` (22 command pages),
`gwz-cli/docs/CLI.md`, `gwz-py/README.md`, and
`gwz-py/src/gwz/client.py`. Documentation is an input being evaluated, not the
sole authority for what is implemented.

Every supported verb, lifecycle action, and public API operation must map to a
task/assertion or an explicit unresolved coverage row. Exercise consequential
options without multiplying every command by every flag. Public streaming and
result-retrieval methods need coverage too. Classify advertised but unsupported
forms as expected refusals, not successful functionality or automatic new-feature
requirements. Record unsupported surface equivalents explicitly.

| Family | Required outcomes and representative failure paths |
| --- | --- |
| Installation/discovery | Exact published PowerShell install command; identify intended executable/version; help; wrong cwd; explicit root; paths containing spaces |
| Workspace setup | Initialize empty/existing roots; initialize from sources; clone from a fixture remote; update bootstrap; materialize from supported lock/head/snapshot/tag/branch modes |
| Member lifecycle | Register existing repo; create member; clone member; list; detach while preserving files; reattach historical designation; sync metadata; distinguish attach from adding a new repo |
| Inspection | Status including staged/unstaged/untracked/root/member state and divergence; list materialized/unmaterialized members; diff working/staged/history states; log workspace changes |
| Stage/commit | Selected paths, selected members, root-only, all targets; tracked versus untracked changes; verify exact committed content and composition, including unrelated staged work |
| History/composition | Capture, snapshot create/list, supported restoration; branch create/list/switch/delete/merge; tag create/list/delete/fetch/push; absent history and dirty checkout refusals |
| Stash | Push/list/apply/pop/drop; conflict and refusal behavior; verify saved work survives unsuccessful operations |
| Remote exchange | Pull/head/snapshot, push, target/remote selection, supported sync modes, divergence, missing credentials, unavailable remote, partial success reporting |
| Coordinated merge | Start, status, continue, abort, preservation, garbage collection; fast-forward/divergence/conflict; root metadata conflicts; restart with an open merge |
| Local-family lifecycle | Clone, list/status inspection, lane-to-root and root-to-lane integration, untouched-lane disposal, integrated-lane disposal, dirty/unpreserved-history refusal, detach with keep, disband |
| Local topology | Root plus several members; stray unregistered repository; nested destination; legitimate member submodule; existing destination; open merge at source; unsupported clone forms |
| Authentication | Inspect/set/unset local identity, invocation override, remote override, missing file, precedence, default quiet output and requested verbose diagnostics |
| Arbitrary execution | `forall` target resolution, argv versus shell form, quoting, failure propagation, dry-run spawning nothing |
| Machine/API behavior | JSON/JSONL validity and final outcomes; stdout/stderr separation; async client lifetime, progress, operation result retrieval, typed errors, transport timeout |

Across the catalog include `@root`, `@all`, member ID/path, exclusions, explicit
root and inherited cwd, dry-run where supported, explicit refusal where not,
and safe versus explicitly authorized destructive actions. Distinguish lane
detach (`local dispose --keep`) from member detach (`repo detach`). Verify exact
syntax against the pinned executable instead of inventing a `detach` command.

Include at least one complete multi-lane integration story, but also isolated
tasks so one early failure cannot hide all later operations. Prompts describe
the desired outcome and constraints, not command sequences. Use unfamiliar
names/paths; reserve unseen topology and conflict variants for final evaluation.

## Fixtures and independent checks

Use small **real Git repositories and real Windows filesystems**: these tests
measure product use, so mocks would conceal the failures under examination.
Keep them tiny and avoid running GWZ's full compiler or unit-test suites.

Fixture builders may use Git directly to construct known histories. Routine task
agents must use GWZ for workspace coordination; ordinary content editing is
allowed. Any task needing a documented Git escape hatch must say so explicitly
and be scored separately. Local bare remotes cover routine push/pull; use an
isolated loopback SSH service and disposable keys for actual authentication
cases, never the user's GitHub credentials.

Trusted assertions run outside the agent's tools and check file bytes, commits,
ancestry, refs, expected repository selection, registrations, managed metadata,
preserved work, and operation lifecycle. Use independent Git/file inspection
alongside GWZ read-only checks. Do not accept an exit code, plausible narration,
or GWZ's own success message as sufficient evidence. Avoid exact generated IDs
or commit hashes where timestamps make them variable.

Check forbidden side effects as well as intended outcomes. A refusal task can
pass only with the expected refusal and unchanged protected state. Include
damage that is later undone in the unsafe-action record. Test validators with
known successful runs and deliberately broken outcomes before scoring models.
Crash cases need a reproducible interruption point; arbitrary process kills
that never reach the intended state are harness failures, not usability scores.

The native adapter enforces its scoped paths/commands, including Windows
junction/reparse-point checks. Model tools expose the current writable fixture
and frozen documentation, not sibling trials, validators, existing workspaces,
SSH keys, or `D:\localmodels`. This is a practical adapter boundary, not OS
confinement; the user explicitly accepted native E: execution. Qualify the
actual native adapter before model-generated actions execute.

## Attempts, budgets, and measurement

Starting budgets, to calibrate in the pilot before freezing the baseline:

- One attempt is a fresh episode with at most 24 action/model turns (including
  malformed action responses), 12,000 generated
  tokens in total, at most 4,096 per response using the sdax settings above, and
  10 minutes elapsed. Stop at the
  first limit; use a shorter allowance for simple inspection tasks once measured.
- Maximum three attempts per task/condition. Each starts from the same fixture
  state and fresh context. Later attempts receive only a standardized outcome
  failure from the previous attempt, never the solution. Record first-attempt
  success separately from eventual success with feedback.
- Keep within the server's verified context limit; record peak context and
  truncation/compaction. Silent truncation invalidates a run. Identical policies
  apply to both conditions.
- Pilot: four representative tasks × two conditions, one attempt each. Then run
  bounded batches, initially at most 16 episodes or two hours, whichever comes
  first. Save a checkpoint between episodes; exhaustion leaves work queued.

The three attempts measure retry-assisted success, not independent statistical
repeats. For the final baseline/candidate comparison, run three independent seeds
or repetitions on the fixed evaluation set where affordable. Reduce catalog
variants before hiding repetitions or allowing unbounded retries. Publish
sample sizes and inconclusive results; do not claim significance from one run.

Record per episode: task/fixture/condition versions; model settings; actions and
exit codes; assertion results; attempts; time; input/output/reasoning/cache token
usage where available; peak context; documentation/help/skill text loaded;
unsafe actions; and infrastructure failures. Separate aggregate input tokens
(including repeated history) from unique text loaded and maximum context size.

Compare first-attempt success, success within budget, retries, unsafe actions,
and total inference usage across all attempts. Report total tokens divided by
successful tasks **including the cost of failed tasks**, plus failed-task counts;
never make a smaller skill look cheaper by dropping failures from the accounting.
Local inference usage is not Codex credit usage: record orchestration/review work
separately and make no assumed credit-savings claim.

Persist generation intent before each provider request and retain every available
raw response before parsing or action execution. Charge malformed actions,
provider retries, and uncertain/lost responses against call and attempt limits.
Preserve actual usage when supplied; otherwise flag token usage as unknown and
report totals as incomplete or lower bounds. Do not count unknown usage as zero
or invent an exact token charge. The generation ledger links to, but does not
replace, the action ledger. Synthetic qualification calls must be labelled and
excluded from actual inference totals.

## Improvement loop and skill shape

1. Freeze the existing skill and published docs, then collect baseline failures.
2. Classify each failure: misleading/missing documentation, retrieval problem,
   stale skill, error-message problem, API gap, product defect, model mistake,
   or harness/environment fault. Keep the smallest reproduction and evidence.
3. Propose the smallest correction. A model may draft it; a reviewer checks it
   against actual supported behavior. Do not loosen validators to make a run pass.
4. Change one category at a time: docs with skill held fixed, then skill with
   docs held fixed; product changes get a distinct build and regression test.
5. Rerun affected tasks and neighboring workflows, then the held-out variants.
   Keep improvements only when they preserve correctness and improve usability
   or context cost. Do not tune on held-out failures and still call them unseen.

Update the existing `/Users/owebeeone/.claude/skills/gwz/SKILL.md`; do not create
a competing skill. Its current baseline is about 2,162 words, not a measured
model-token count. It contains the superseded Windows installer command, lengthy
historical version notes, and broad dry-run statements that need verification.
Preserve those facts in the baseline rather than silently repairing it first.

Aim for a short entry file: identify the workspace, select targets, stage/commit,
integrate complete lanes, distinguish member/lane retirement, inspect recovery,
and choose machine output. Move rare operations, Python examples, platform
details, and migration history into referenced resources under the same skill.
Remove gwz-dev-specific build/release instructions from the general operating
guide unless the task requires them. Derive the final length from measured
success and total context cost; do not impose a line/token-count gate.

## Deliverables and execution order

1. **Handoff implementation:** inspect/freeze sdax runner; qualify native E:
   filesystem behavior and the scoped JSON action adapter; verify pinned install,
   action/generation ledgers, and trusted checker self-tests.
2. **Coverage manifest:** task IDs, prompts, fixtures, assertions, supported
   surfaces, expected refusals, and unresolved coverage rows for the full inventory.
3. **Pilot:** inspection/selection, add/create a member, member detach/attach, and
   complete lane integration/disposal in both conditions. Calibrate budgets and
   harness. Run exact public installation separately as a trusted smoke; retain
   model-driven network installation as an explicit pending coverage row.
4. **Full baseline:** run every coverage row in bounded resumable batches; prioritize
   CLI first, then Python CLI/API. Report outstanding rows rather than calling a
   pilot full coverage.
5. **Targeted improvements:** docs/skill edits and separately scoped API/bug findings.
6. **Final comparison:** paired baseline/candidate results, independent repetitions,
   unseen variants, compact skill, and an explicit list of remaining failures.

Keep reusable tasks, assertions, runner configuration, and concise findings in
version control. Keep large raw transcripts, fixtures, model output, and build
artifacts outside the repo under `E:\gwz-agent-eval\runs\<run-id>`. Record hashes
and paths in a compact run manifest; retain failing runs until reviewed, then
clean up only owned paths. No generic `evidence/` ignore and no sample dumps in
commits. This evaluation is opt-in, not part of the release critical path.

Completion means a full reconciled coverage manifest, trustworthy checks,
reported with/without-skill results, and a reviewed skill/docs candidate whose
benefit survives fresh tasks. It does not require forcing a local model to pass
every task or adding every feature it requests.
