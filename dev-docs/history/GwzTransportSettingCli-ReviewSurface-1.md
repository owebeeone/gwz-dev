# TR2.5 CLI transport setting — SURFACE-AXIS RE-REVIEW, ROUND 1

**Review object:** Bounded user-facing remediation in CLI revision `90fdb108f2a91ead456e07053da108b721300cd3`, following the original Surface review of `c4a588f8be4e91926deff2156dce00c7062f4184`. Reviewed read-only in `/Volumes/projects/limbo/gwz-dev-tr2-5-cli`.

**Baseline:** Root `52adfa0b8dba8752623d0ef5ada142105c48ac03`; gwz-cli `90fdb108f2a91ead456e07053da108b721300cd3`; gwz-core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Committed user-facing documents were read using `git show` at the settled CLI revision; help came from the freshly supplied binaries. Implementation source, test source, implementation package, design and plan documents were excluded.

**Date:** 2026-10-03

**Axis:** Command discovery, defaults, configuration lifecycle and changed machine-output promises. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on their reports. Filed verbatim by the lane owner.

**Verdict: GO** — original Surface P3-1 is closed. No new P0–P3 finding or new architectural root cause was identified in the changed user-facing ranges.

---

## 0. Evidence base

Read:

- Settled `docs/commands/auth.md:1–75`, particularly transport configuration and the corrected lifecycle recipes at lines 63–71.
- Settled `docs/MachineOutput.md:201–241`, including the added remote-tag, execution-error and decoded-path statements at lines 218–222.
- `/Volumes/projects/limbo/gwz-lanes-prewarm-20261003/cli-review/artifacts-round1.json`.
- The owner evidence README’s receipt-filename references.
- The resulting private `campaigns/transport-qualification/runs/2026-10-03-tr25-cli-remediation-1/workflow-closure.log:1–46`.

Executed permitted fresh help:

- Candidate `--help`, `fetch --help` and `fetch -h`.
- Candidate `help fetch`, retaining its numbered transport section at lines 162–181.
- Candidate `push --help`, `pull --help` and `auth --help`, inspecting their transport sections.
- Ordinary `fetch --help`, inspecting the existing concurrency, timeout and diagnostic options.

All help commands completed successfully.

SHA-256 checks matched the round-one artifact receipt:

- Candidate: `8e9fd3fd7c5c85fec294713a4b399cc7c6e2f586745d4a5ce6a3af7302bd21f5`.
- Ordinary: `20d1cf7da093371729a6b8f9ecc2428f6f13148fff2d545558d511bff34ece55`.

The exact root/CLI/core tuple matched at both start and end. Final CLI status was clean. Root retained the same out-of-scope untracked prompts and mapping draft; core retained its untracked private BugReport. Their contents were not read.

The receipt records `supported_file_recipe_sets_and_removes_under_git_config_global_override ... ok` at line 42 and **6 passed, 0 failed** at line 45. This is owner-run execution evidence, not a test performed by this reviewer. Test implementation was not inspected.

No writes, builds, tests, source inspection or peer-report inspection were performed.

## 1. Prior-finding closure

| Original Surface finding | Disposition | Closure evidence |
|---|---|---|
| **P3-1 — Documented global setter and remover can target a file GWZ explicitly ignores** | **Closed** | `auth.md:64–65` now pairs `git config --file "$HOME/.gitconfig" gwz.transport native` with `git config --file "$HOME/.gitconfig" --unset-all gwz.transport`. Lines 69–71 explicitly explain that Git’s `--global` commands honor `GIT_CONFIG_GLOBAL`, while these paired commands bypass it. Fresh `help fetch:172–175` contains the same corrected pair and explanation; push, pull and auth help agree. The owner’s matching lifecycle regression passes in `workflow-closure.log:42`. |

No new findings.

## 2. Changed-range invariant analysis

**The original counterexample no longer applies to the advertised recipes.** With `GIT_CONFIG_GLOBAL` pointing to an alternate file, both revised commands explicitly name `$HOME/.gitconfig`, one of the resolver’s documented inputs. The setter and remover therefore address the same supported file rather than the alternate file selected by Git’s global environment override. The exception and its remedy are explained together.

**The help-only lifecycle remains complete.** Root help directs the user to detailed command help. Fetch help supplies the transport values, default and precedence, followed by the persistent setter/remover pair. A user can select native for one invocation, retain it through `GWZ_TRANSPORT`, or save it in the named global file. Undo is stated for the environment and file forms. The quoted file operand also preserves a home path containing spaces.

**Command placement and defaults remain coherent.** Transport selection remains a global option exposed by the existing network command families. Short help states the `gwz` default and the native-dependent concurrency and timeout defaults. Long help retains explicit override behavior, native’s lack of setup retry and pooling, and the absence of automatic fallback. The remediation introduces no new command, renamed option or asymmetric lifecycle verb.

**The added machine-output text is understandable without implementation knowledge.** `MachineOutput.md:218–222` says remote tag listings retain their `kind: "tags"` and `entries` payload while optionally carrying the transport setting. It separately describes execution errors with object-valued metadata and explains that decoded paths contain actual characters while JSON escaping supplies their wire representation. These statements fit the surrounding optional-field, null-metadata and final-JSONL-response contract. No contradictory interface promise was found.

**Architectural classification:** No new architectural root cause was identified at the Surface level. The reviewed correction changes the documented file-targeting recipe and clarifies existing machine-output fields. It creates no new public command boundary, setting scope or lifecycle state. Implementation architecture was outside this review’s permitted evidence.

## 3. Risks and next action

The owner receipt supports the specific alternate-global-file lifecycle closure. This re-review does not independently verify test assertions, runtime serialization or transport execution beyond the supplied receipt.

Release, Linux/Windows, live SSH/HTTPS, performance and TR2.6 aggregate qualification remain deferred.

The next action is to record this Surface GO and P3-1 closure against the settled tuple, then advance through the remaining lane acceptance gates.
