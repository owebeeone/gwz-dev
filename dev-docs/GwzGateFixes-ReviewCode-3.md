# Gate fixes — CODE-AXIS REVIEW (re-verdict, round 2)

**Review object:** gwz-core `b0ac8de7..9d901d77` (HEAD `9d901d77b47417da8e561061e09efba0fda65c01`, parent `b0ac8de7`; amended from `dc192389`, unpushed). The change since my last reviewed revision is `dc192389..9d901d77`: `scripts/windows_lane_check.py` and `scripts/test_windows_lane_check.py` only. The tools (`win-receipt.zsh`, `remote-gates.md`, `lanes-1024-common.md`, `remote-gates.zsh`) were not touched since round 1.
**Baseline:** the round-1 re-verdict revision `dc192389`, filed verbatim as `dev-docs/GwzGateFixes-ReviewCode-2.md`.
**Date:** 2026-10-30 **Axis:** Code (re-verdict round 2; filed as `GwzGateFixes-ReviewCode-3.md`)
**Verdict: GO** — P0 0, P1 0, P2 0, P3 0 open. P2-3 is closed. Two conditions on the receipt record are listed in section 3; neither blocks the verdict.

## Prior-finding closure table

| ID | Finding | Status | Evidence |
|---|---|---|---|
| P2-1 | Mac `.git` shipped; host checkout stays LF | Closed (round 1, unchanged) | Archive still excludes `.git/config`, hooks, logs and info, and still refuses gitfile, symlink and alternates trees. |
| P2-2 | Library step ran without errexit | Closed (round 1, unchanged) | `run_boundary_job.py` was not touched in this range. |
| P3-1..P3-3 | Untracked files in `--ref` mode; PIN MISMATCH note; refusal after booking | Closed (round 1, unchanged) | Not touched in this range. |
| **P2-3** | New CRLF gate assumed the host's global `core.autocrlf`; 19 of 19 recorded receipts showed LF | **Closed (option a).** | My original counterexample re-run on the new code: a real `transport_archive_command()` archive of the lane's gwz-transport, unpacked four times with `TRANSPORT_HOST_CONFIG` written and `git checkout-index -a -f` run. The host global config was unset, `autocrlf=false`, `autocrlf=input` and `autocrlf=true`. `GIT_CONFIG_GLOBAL` pointed at a scratchpad file, with system config disabled. In all four cases `protocol/transport.ir.json` has 2402 CRLF lines and 0 bare LF lines. On the old code with the global unset, the same checkout gave 0 CR. The recorded origin and value come out as `file:.git/config` and `true` in every case. |

## Changed-range analysis (`dc192389..9d901d77`, 2 files, +82/−12)

- **`TRANSPORT_HOST_CONFIG`** now writes `core.autocrlf = true` into the host repo's local config. A repo-local value beats the host's global and system values, so the checkout is CRLF by construction whatever the dabeest Git is set to. The comment cites `windows-matrix.yml` and `transport-candidate.yml`. I confirmed they document `core.autocrlf=true` at SYSTEM on the runner image and set it globally in the sentinel job. It also records the 19-receipt evidence that dabeest has no such setting.
- **`transport_host_lines`** appends `echo "TRANSPORT_AUTOCRLF $(git -C <tree> config --show-origin --get core.autocrlf)"`.
  - It sits inside the stage script (`set -euo pipefail`). The `echo` succeeds even if the substitution is empty, so it cannot fail the stage.
  - It runs before `rustc --version`, so `receipt.toolchain` (the last stdout line) is unaffected. A new test asserts this.
- **`parse_autocrlf`** returns `{'value', 'origin'}` from that line. An absent line yields empty strings, never an invented value. It is stored as `receipt.transport['host_autocrlf']` right after staging. Both outcomes are tested.
- **Tests** are now 129 (3.13) and 122 (3.11), both OK, up from 125 and 118. The new real-process test `test_the_host_checkout_is_crlf_whatever_the_hosts_own_global_autocrlf_says` loops over a host global of unset, `false`, `input` and `true`. For each it asserts CR bytes in `transport.ir.json` and `src/lib.rs`, and no bare LF in `lib.rs`. Other new tests cover the stage's recorded origin and value, and that the minimal config now contains `autocrlf = true` but still no `eol`, `credential`, `remote`, `safecrlf` or `hooksPath` keys. The weaker round-1 test, which set the global to `true`, still passes.
- **Remaining behaviour, not defects:**
  - The recorded `host_autocrlf` is the repo-local value the script itself wrote, so it states what was set rather than discovering the host's own configuration. That is exactly what the plan specifies (option a).
  - The receipt now checks gwz-transport out CRLF where the 19 earlier receipts used LF. This is a deliberate increase in fidelity to the hosted Windows runner. Any CRLF-only failure in a transport reader on the host would be a real CI-parity finding, not a regression.
  - `gwz-transport` tracks no `.sh` files and has no `.gitattributes`, so nothing there is broken by CRLF or by an `eol` override.

## 0. Evidence base

**Tuple.** `9d901d77b47417da8e561061e09efba0fda65c01` at start and at end, parent `b0ac8de7`. `git status` was clean.

**Run (read-only, `-B`):**
- `python3.13 -B -m unittest scripts.test_windows_lane_check scripts.checks.test_run_boundary_job scripts.checks.test_run_tests_session_crates` gives 129 tests OK.
- The same two changed modules under `python3.11` give 122 OK.
- The four-case counterexample above (host global unset, `false`, `input`, `true`).

**Implementer logs** `rem2-p23-red.log`, `rem2-p23-green.log`, `rem2-unit-3.11.log`, `rem2-unit-3.13.log` and `rem2-argfree-checks.log` exist in `build-targets/gate-fixes-logs/`; I did not need them beyond the reproductions above.

**Receipt and gates, not finished:**
- The Windows receipt `gate-fixes-20261030-b`: `win-b.log` and `gate-fixes-receipts/gate-fixes-20261030-b/receipt.json` do not exist yet. I cite no receipt result.
- The WSL gate `gate-fixes-20261030-b`: `remote-b.log` shows it at `candidate-test`, with no `summary.txt`. It started at 07:15Z, before this amendment's timing is known to me, so I cannot tell which tree it synced. See condition 1 in section 3.
- The earlier gate `gate-fixes-20261030-a` (old tuple) finished with OVERALL PASS and is not evidence for this tuple.

## Problem, requirement provenance, and scope

- **Requirement.** P2-3 closed under the round-2 plan (option a), which traces to my round-1 finding and its stated remedies. No new scope. The one-line premise change (the receipt now emulates the runner's `core.autocrlf=true` instead of inheriting the host's) is bounded to the receipt's transport staging and is recorded in the receipt.
- **Lane-owner decision I was asked to judge.** The in-flight receipt keeps label `gate-fixes-20261030-b`, stages this tree, and the trailer stays `Windows-receipt: gate-fixes-20261030-b`. That is acceptable, provided the receipt really stages `9d901d77`. The plan file's own label `-c` differs from the decision, which the owner has overruled. The label only has to match the receipt actually run. P2-3's premise resting on the 19 earlier LF receipts rather than a fresh pre-fix receipt is sound: those 19 receipts used the same host and the same stage path, and my local counterexample reproduces both the LF (old code) and the CRLF (new code) outcomes directly.
- **Out of scope**, unchanged: other lanes, the gwz-py pin gate, the Windows row-count step, the foreign-target clippy step, and sibling repos or old one-off scripts that still name `gwz_session_candidate`.

## 1. Findings

None open.

## 2. Invariant analysis

- **Host-independent CRLF.** The host checkout is CRLF for any host global `core.autocrlf` value. The `require_crlf` gate, which fails a clean-tree hosted-like receipt whose owner export is not CRLF, can now fail only if the host's Git ignores a repo-local setting (not credible) or something unrelated to config rewrites the line endings. Failure then points at a real defect in staging.
- **Receipt honesty.** `transport.host_autocrlf` records the effective value and its origin, or empty strings when the stage did not print it. It cannot overstate.
- **Earlier invariants** (errexit parity, minimal host config and refusals, tracked-only `--ref` staging, final-line mismatch note, preflight before slot booking) are unchanged by this range and were re-run by the full test classes above.

## 3. Risks and next action

1. **Receipt and gate must be for this tree (conditions on the record, not on the verdict).**
   - **Receipt.** When `gate-fixes-20261030-b` lands, check that `receipt.json`'s `commit` is `9d901d77…`. Check that the `hosted-like` entry has `owner_export_line_endings: "crlf"` and that `transport.host_autocrlf` is `{value: "true", origin: "file:…/.git/config"}`. A clean `hosted-like` PASS under CRLF is the host-side confirmation of this closure. The first CRLF run on this host could surface a CRLF-only failure in a transport reader; that would be a CI-parity finding for a separate fix, not a defect of this change.
   - **WSL gate.** The WSL gate `-b` began before I could tell which tree it synced. If its sync predates the amendment, re-run it, or accept the already-passing `-a` summary plus the unit evidence above. The amendment touches `windows_lane_check.py` and its tests only, which the boundary job exercises through the unit-test list.
2. The two newly-run session crates (channel, contract) on native Windows, and the hosted-like watchdog limits, remain the first-run risk carried over from round 1. The receipt will show them.
3. Out of scope, recorded only: sibling repos and old one-off scripts under `build-targets/` still name the retired `gwz_session_candidate`.
