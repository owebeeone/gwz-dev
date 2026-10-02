# TR2.5 CLI transport setting — SURFACE-AXIS REVIEW

**Review object:** CLI implementation diff `0164e66376dac204910552148c62cb6c5c55ed03..c4a588f8be4e91926deff2156dce00c7062f4184`, reviewed read-only in `/Volumes/projects/limbo/gwz-dev-tr2-5-cli`. Settlement review dated 2026-10-03.

**Baseline:** Root `b6153ec34cdb96972c1ceed057a073c3e7f03afc`; gwz-cli `c4a588f8be4e91926deff2156dce00c7062f4184`; gwz-core `2e64e88a28c332ed422cc390adc76738dc701bb1`. User-facing documents were read using `git show` at recorded revisions. Implementation source, implementation package, design and plan documents were excluded.

**Date:** 2026-10-03

**Axis:** Command discovery, names, defaults, configuration lifecycle and machine-output promises. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1 or P2 findings; one bounded P3 documentation finding.

---

## 0. Evidence base

Read:

- Canonical `cli-review/Surface.txt`, including scope, permitted commands and report contract.
- Workspace `AGENTS_GWZ.md`.
- Settled CLI `docs/commands/auth.md:1–73`, including transport defaults and configuration lifecycle at lines 50–73.
- Baseline CLI `docs/commands/auth.md:1–53` for comparison.
- Settled CLI `docs/MachineOutput.md`, particularly response/error envelopes at lines 13–162, transport-setting and authentication contracts at lines 201–240, JSONL completion at lines 541–621, and exit codes at lines 889–899.
- `cli-review/artifacts.json`.

Executed only permitted inspection and help commands:

- Candidate `--help`, `help fetch`, `fetch -h`, `fetch --help`, `push --help`, `pull --help`, and `auth --help`. All exited successfully.
- Ordinary binary `fetch --help`, which exited successfully.
- `git rev-parse HEAD` and `git status --short` in root, CLI and core at the beginning and end.
- SHA-256 checks of both supplied binaries.

Binary hashes matched the supplied receipt:

- Candidate: `964b060d09422a70d83a809b98968f5a384df550354802fee7033256f69961dc`.
- Ordinary: `0513cff45a18fd826ca0fe056bd311aa2c7a65b3b0d22aec42f11fb71267d3f9`.

The exact tuple remained unchanged. CLI status remained clean. Root and core retained the same pre-existing untracked, out-of-scope documents; their contents were not read.

No source inspection, writes, builds, tests or live network operations were performed. An isolated configuration-lifecycle execution was requested from the owner; no resulting receipt was available for this report.

## 1. Findings

### [P3-1] Documented global setter and remover can target a file GWZ explicitly ignores

**Location:** Settled `docs/commands/auth.md:63–69`; candidate `fetch --help:171–175`, with the same global-option wording in push, pull and auth help.

**Violated invariant:** The advertised install/remove commands must operate on configuration files the setting resolver recognizes, or state the condition under which they do so.

Help says `git config --global gwz.transport native` sets the persistent transport and `git config --global --unset-all gwz.transport` removes it. Immediately afterward, it says GWZ does not read a file selected through `GIT_CONFIG_GLOBAL`. Ordinary Git’s `--global` commands honor that environment override.

**Credible reproduction sequence:**

1. Set `GIT_CONFIG_GLOBAL` to a writable alternate Git configuration file.
2. Leave `gwz.transport` absent from the global files GWZ documents as reading.
3. Run the documented setter. Git writes `native` into the alternate file.
4. Run a GWZ network command without a transport flag or environment override. According to the documented resolver contract, GWZ ignores the newly written value and selects its default.
5. Conversely, with a recognized global file already selecting `native`, the documented remover targets the alternate file and leaves the effective setting intact.

This sequence was derived from the interface contract and Git command semantics; it was not executed by this restricted reviewer.

**Impact:** A user following the supplied lifecycle recipe can fail to activate the escape transport or fail to remove it. The nearby exception identifies the mismatch but supplies no corrected setter/remover recipe.

**Required correction:** Qualify both recipes for `GIT_CONFIG_GLOBAL`, and provide commands that explicitly target a supported global file or temporarily remove the override. Keep activation and removal instructions together.

**Closure/regression test:** Have the owner exercise the revised instructions in an isolated home with `GIT_CONFIG_GLOBAL` pointing elsewhere. Confirm activation selects `native`, removal restores the prior/default selection, and the alternate file is handled exactly as documented. Verify both long help and the auth page contain the corrected instructions.

## 2. Invariant analysis

**Discovery and placement held.** Root help names the existing network families and directs users to detailed command help. Fetch, push and pull expose the transport as a global network-operation option. The option selects infrastructure used by those commands; no new command family or persistent public verb is required.

**Names and summaries held.** `--transport`, `gwz` and `native` are explained in terms of the implementations they select. Short help states the default. Long help explains that native selection occurs before dispatch and does not provide automatic fallback.

**Defaults held for the reviewed change.** Help gives `gwz` as the transport default and states both sets of dependent defaults: jobs 100/50, per-host concurrency 32/8 and timeout 9/3 seconds. Explicit overrides are documented. Native’s lack of pooling and setup retries, and the resulting ineffectiveness of `--max-retries`, are explicit.

**The ordinary first-day lifecycle is discoverable.** From fetch help alone, a user can select native for one invocation, retain it through `GWZ_TRANSPORT`, or persist it through global Git configuration. Flag omission, environment removal and the direct global-key remover are presented together. The alternate-global-file exception is the bounded gap recorded in P3-1.

**Precedence held at the interface level.** Both help and docs state flag → environment → global configuration → default. Repository configuration is explicitly ignored and reported. Supported global locations, XDG fallback and exclusion of conditional includes are named.

**Machine-output promises held as a readable contract.** `meta.transport_setting` distinguishes selected transport from selection source; specifies deciding-file provenance, inclusion status and ignored/skipped arrays; and states when the object is omitted. The docs distinguish this setting object from authentication rows in `meta.transport`. JSONL consumers are directed to the final response, and machine modes promise no setting notices on stderr. No interface contradiction requiring a compatibility change was identified.

These conclusions assess the exposed contract. They do not claim that runtime resolution, transport behavior or serialization has been verified.

## 3. Risks and next action

Release, Linux/Windows, live SSH/HTTPS, performance and TR2.6 aggregate qualification remain deferred as specified. The restricted help-only review supplies no new execution evidence for those outcomes.

The next action is to record this Surface GO against the exact tuple and correct P3-1’s global-file lifecycle instructions, with an isolated owner-run receipt.
